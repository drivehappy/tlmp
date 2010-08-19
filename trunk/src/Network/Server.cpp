#include "Server.h"

using namespace TLMP::Network;

Server& Server::getSingleton()
{
  static Server m_pSingleton;
  return m_pSingleton;
}

Server::Server()
{
  m_pServer = NULL;
  m_pBitStream = new RakNet::BitStream(1024);

  m_bSuppressNetwork_SetDestination = false;
  m_bSuppressNetwork_SendCharacterCreation = false;
  m_bSuppressNetwork_SendEquipmentEquip = false;
  m_bSuppressNetwork_SendEquipmentUnequip = false;
  m_bSuppressNetwork_SendEquipmentCreation = false;

  m_pOnListening = NULL;
  m_pOnShutdown = NULL;
  m_pOnClientConnect = NULL;
  m_pOnClientDisconnect = NULL;

  m_bWaitingForGame = true;
}

Server::~Server()
{
  delete m_pBitStream;
}

void Server::Listen(u16 port, u16 maxconnections)
{
  multiplayerLogger.WriteLine(Info, L"Starting server...");

  if (m_pServer) {
    multiplayerLogger.WriteLine(Info, L"Shutting down server");
    m_pServer->Shutdown(0);
  }

  m_pServer = RakNetworkFactory::GetRakPeerInterface();
  multiplayerLogger.WriteLine(Info, L"Got server: %p", m_pServer);

  SocketDescriptor socketDescriptor(port, 0);
  m_pServer->Startup(maxconnections, 30, &socketDescriptor, 1);
  m_pServer->SetMaximumIncomingConnections(maxconnections);

  multiplayerLogger.WriteLine(Info, L"Server listening on port %i (max connections = %i)", port, maxconnections);
  if (m_pOnListening) {
    m_pOnListening(NULL);
  }
}

void Server::Shutdown()
{
  if (m_pServer) {
    m_pServer->Shutdown(0);
    m_pServer = NULL;

    multiplayerLogger.WriteLine(Info, L"Server shutdown");

    if (m_pOnShutdown)
      m_pOnShutdown(NULL);
  }
}

void Server::SetCallback_OnListening(OnListening callback)
{
  m_pOnListening = callback;
}

void Server::SetCallback_OnShutdown(OnShutdown callback)
{
  m_pOnShutdown = callback;
}

void Server::SetCallback_OnClientConnect(OnClientConnect callback)
{
  m_pOnClientConnect = callback;
}

void Server::SetCallback_OnClientDisconnect(OnClientDisconnect callback)
{
  m_pOnClientDisconnect = callback;
}

void Server::ReceiveMessages()
{
  if (!m_pServer)
    return;

  // Round up any packets we've received
  Packet *packet = m_pServer->Receive();
  while (packet) {
    switch(packet->data[0]) {
    case ID_NEW_INCOMING_CONNECTION:
      multiplayerLogger.WriteLine(Info, L"Client connected");
      if (m_pOnClientConnect) {
        SystemAddress *sysAddress = new SystemAddress();
        *sysAddress = packet->systemAddress;
        m_pOnClientConnect((void*)sysAddress);
      }
      break;
    case ID_DISCONNECTION_NOTIFICATION:
      multiplayerLogger.WriteLine(Info, L"Client disconnected");
      if (m_pOnClientDisconnect) {
        m_pOnClientDisconnect(NULL);
      }
      break;
    case ID_CONNECTION_LOST:
      multiplayerLogger.WriteLine(Info, L"Client disconnected");
      if (m_pOnClientDisconnect) {
        m_pOnClientDisconnect(NULL);
      }
      break;
    case ID_USER_PACKET_ENUM+1:
      u32 msg;
      m_pBitStream->Reset();
      m_pBitStream->Write((const char *)packet->data, packet->length);
      m_pBitStream->IgnoreBits(8);
      m_pBitStream->Read<u32>(msg);

      WorkMessage(packet->systemAddress, (Message)msg, m_pBitStream);

      break;
    }

    m_pServer->DeallocatePacket(packet);
    packet = m_pServer->Receive();
  }
}

void Server::WorkMessage(const SystemAddress address, Message msg, RakNet::BitStream *bitStream)
{
  if (msg >= sizeof(MessageString) / sizeof(char*)) {
    log("Error: Message out of range.");
    multiplayerLogger.WriteLine(Info, L"Error: Message out of range.");
  }

  wstring msgString = convertAcsiiToWide(MessageString[msg]);

  multiplayerLogger.WriteLine(Info, L"Server Received Message: %s", msgString.c_str());
  logColor(B_GREEN, L"Server Received Message: %s", msgString.c_str());

  switch (msg) {
  case C_VERSION:
    {
      NetworkMessages::Version *msgVersion = ParseMessage<NetworkMessages::Version>(m_pBitStream);
      u32 version = (u32)(msgVersion->version());

      HandleVersion(address, version);
    }
    break;

  case C_REQUEST_HASGAMESTARTED:
    {
      NetworkMessages::GameHasStarted *msgGameHasStarted = ParseMessage<NetworkMessages::GameHasStarted>(m_pBitStream);

      HandleHasGameStarted(address);
    }
    break;

  case C_PUSH_GAMEENTER:
    {
      NetworkMessages::GameEnter *msgGameEnter = ParseMessage<NetworkMessages::GameEnter>(m_pBitStream);

      HandleGameEnter(address);
    }
    break;

  case C_PUSH_GAMEEXITED:
    {
      NetworkMessages::GameExited *msgGameExited = ParseMessage<NetworkMessages::GameExited>(m_pBitStream);

      HandleGameExited(address);
    }
    break;

  case C_REPLY_CHARINFO:
    {
      NetworkMessages::ReplyCharacterInfo *msgReplyCharacterInfo = ParseMessage<NetworkMessages::ReplyCharacterInfo>(m_pBitStream);

      HandleReplyCharacterInfo(address, msgReplyCharacterInfo);
    }
    break;

  case C_PUSH_EQUIPMENT:
    {
      NetworkMessages::Equipment *msgEquipment = ParseMessage<NetworkMessages::Equipment>(m_pBitStream);
      
      HandleEquipmentCreation(address, msgEquipment); 
    }
    break;

  case C_PUSH_EQUIPMENT_DROP:
    {
      NetworkMessages::EquipmentDrop *msgEquipmentDrop = ParseMessage<NetworkMessages::EquipmentDrop>(m_pBitStream);
      NetworkMessages::Position msgPosition = msgEquipmentDrop->position().Get(0);

      Vector3 position;
      position.x = msgPosition.x();
      position.y = msgPosition.y();
      position.z = msgPosition.z();

      u32 equipmentId = msgEquipmentDrop->equipmentid();
      bool unk0 = msgEquipmentDrop->unk0();

      HandleEquipmentDrop(equipmentId, position, unk0);
    }
    break;

  case C_PUSH_EQUIPMENT_PICKUP:
    {
      NetworkMessages::EquipmentPickup *msgEquipmentPickup = ParseMessage<NetworkMessages::EquipmentPickup>(m_pBitStream);

      u32 equipmentId = msgEquipmentPickup->equipmentid();
      u32 characterId = msgEquipmentPickup->characterid();

      HandleEquipmentPickup(characterId, equipmentId);
    }
    break;

  case C_PUSH_EQUIPMENT_EQUIP:
    {
      NetworkMessages::InventoryAddEquipment *msgInventoryAddEquipment = ParseMessage<NetworkMessages::InventoryAddEquipment>(m_pBitStream);
      u32 ownerId = msgInventoryAddEquipment->ownerid();
      u32 equipmentId = msgInventoryAddEquipment->equipmentid();
      u32 slot = msgInventoryAddEquipment->slot();
      u32 unk0 = msgInventoryAddEquipment->unk0();
      u64 guid = msgInventoryAddEquipment->guid();

      HandleInventoryAddEquipment(address, ownerId, equipmentId, slot, unk0, guid);
    }
    break;

  case C_PUSH_EQUIPMENT_UNEQUIP:
    {
      NetworkMessages::InventoryRemoveEquipment *msgInventoryRemoveEquipment = ParseMessage<NetworkMessages::InventoryRemoveEquipment>(m_pBitStream);
      u32 ownerId = msgInventoryRemoveEquipment->ownerid();
      u32 equipmentId = msgInventoryRemoveEquipment->equipmentid();

      HandleInventoryRemoveEquipment(address, ownerId, equipmentId);
    }
    break;

  case C_PUSH_EQUIPMENT_USE:
    break;

  case C_PUSH_EQUIPMENT_IDENTIFY:
    break;

  case C_PUSH_EQUIPMENT_ADDENCHANT:
    break;

  case C_PUSH_EQUIPMENT_ADDSOCKET:
    break;

  case C_PUSH_CHARACTER_SETDEST:
    {
      NetworkMessages::CharacterDestination *msgCharacterDestination = ParseMessage<NetworkMessages::CharacterDestination>(m_pBitStream);
      NetworkMessages::Position msgDestination = msgCharacterDestination->destination().Get(0);

      u32 commonId = msgCharacterDestination->id();

      Vector3 destination;
      destination.x = msgDestination.x();
      destination.z = msgDestination.z();

      HandleCharacterDestination(address, commonId, destination);
    }
    break;

  }
}

//
// Below are message handlers

// Server receives a version message from Client.
void Server::HandleVersion(const SystemAddress clientAddress, u32 version)
{
  multiplayerLogger.WriteLine(Info, L"Server received Client Version: %i", version);

  if (version != MessageVersion) {
    multiplayerLogger.WriteLine(Info, L"Disconnecting the Client: Version Mismatch: (Client = %i  Server = %i)",
      version, MessageVersion);

    // TODO Disconnect Client
  }
}

// Server receives game started request from Client upon initial login.
void Server::HandleHasGameStarted(const SystemAddress clientAddress)
{
  NetworkMessages::GameHasStarted msgGameHasStarted;
  msgGameHasStarted.set_started(m_bGameStarted);

  SendMessage<NetworkMessages::GameHasStarted>(clientAddress, S_REPLY_HASGAMESTARTED, &msgGameHasStarted);
}

// Server receives game entrance from Client
void Server::HandleGameEnter(const SystemAddress clientAddress)
{
  multiplayerLogger.WriteLine(Info, L"Client has Entered the Game");

  // Send all of the existing characters in the game to the client
  vector<NetworkEntity*>::iterator itr;
  for (itr = NetworkSharedCharacters->begin(); itr != NetworkSharedCharacters->end(); itr++) {
    CCharacter* character = (CCharacter*)((*itr)->getInternalObject());

    multiplayerLogger.WriteLine(Info, L"Server: Pushing existing Character (%016I64X %i %s) out to client that just joined...",
      character->GUID, (*itr)->getCommonId(), character->characterName.c_str());
    log(L"Server: Pushing existing Character (%016I64X %i %s) out to client that just joined...",
      character->GUID, (*itr)->getCommonId(), character->characterName.c_str());

    string characterName(character->characterName.begin(), character->characterName.end());
    characterName.assign(character->characterName.begin(), character->characterName.end());

    // Create a new network message for all clients to create this character
    NetworkMessages::Character msgNewCharacter;
    msgNewCharacter.set_guid(character->GUID);
    msgNewCharacter.set_name(characterName);

    NetworkMessages::Position *msgPlayerPosition = msgNewCharacter.mutable_position();
    msgPlayerPosition->set_x(character->position.x);
    msgPlayerPosition->set_y(character->position.y);
    msgPlayerPosition->set_z(character->position.z);

    msgNewCharacter.set_id((*itr)->getCommonId());

    // This will broadcast to all clients except the one we received it from
    Server::getSingleton().SendMessage<NetworkMessages::Character>(clientAddress, S_PUSH_NEWCHAR, &msgNewCharacter);
  }

  // Send all of the existing equipment in the game to the client
  for (itr = NetworkSharedEquipment->begin(); itr != NetworkSharedEquipment->end(); itr++) {
    Helper_SendEquipmentToClient(clientAddress, (CEquipment*)((*itr)->getInternalObject()), (*itr));
  }

  // Send a request for character info
  NetworkMessages::RequestCharacterInfo msgRequestCharacterInfo;
  SendMessage<NetworkMessages::RequestCharacterInfo>(clientAddress, S_REQUEST_CHARINFO, &msgRequestCharacterInfo);
}

// Server receives game exit from client
void Server::HandleGameExited(const SystemAddress clientAddress)
{
  multiplayerLogger.WriteLine(Info, L"Client has Exited the Game");
}

void Server::HandleReplyCharacterInfo(const SystemAddress clientAddress, Vector3 posCharacter, u64 guidCharacter, string nameCharacter, u64 guidPet, string namePet)
{
  multiplayerLogger.WriteLine(Info, L"Server Received Client Character Info:");
  multiplayerLogger.WriteLine(Info, L"  GUID: %016I64X", guidCharacter);
  multiplayerLogger.WriteLine(Info, L"  Name: %s", convertAcsiiToWide(nameCharacter).c_str());
  multiplayerLogger.WriteLine(Info, L"  Position: %f %f %f", posCharacter.x, posCharacter.y, posCharacter.z);
  multiplayerLogger.WriteLine(Info, L"  GUID: %016I64X", guidPet);
  multiplayerLogger.WriteLine(Info, L"  Name: %s", convertAcsiiToWide(namePet).c_str());

  log(L"Server Received Client Character Info");

  //
  // Create a new network message for other clients to create this character
  NetworkMessages::Character msgNewCharacter;
  msgNewCharacter.set_guid(guidCharacter);
  msgNewCharacter.set_name(nameCharacter);

  NetworkMessages::Position  *msgPlayerPosition = msgNewCharacter.mutable_position();
  msgPlayerPosition->set_x(posCharacter.x);
  msgPlayerPosition->set_y(posCharacter.y);
  msgPlayerPosition->set_z(posCharacter.z);

  // This will broadcast to all clients except the one we received it from
  Server::getSingleton().BroadcastMessage<NetworkMessages::Character>(clientAddress, S_PUSH_NEWCHAR, &msgNewCharacter);

  // Create this character on this instance
  CResourceManager *resourceManager = (CResourceManager *)gameClient->pCPlayer->pCResourceManager;
  CMonster *clientCharacter = resourceManager->CreateMonster(guidCharacter, 1, true);

  log("Character created: %p", clientCharacter);

  if (clientCharacter) {
    clientCharacter->characterName.assign(convertAcsiiToWide(nameCharacter));
    clientCharacter->SetAlignment(2);
  } else {
    log("Error: clientCharacter is null!");
    multiplayerLogger.WriteLine(Info, L"Error: clientCharacter is null!");
  }

  // Lock the creation so we don't resend to all the clients
  Server::getSingleton().SetSuppressed_SendCharacterCreation(true);
  gameClient->pCLevel->CharacterInitialize(clientCharacter, &posCharacter, 0);
  Server::getSingleton().SetSuppressed_SendCharacterCreation(false);

  // Send this ID to the Client that created the character
  NetworkEntity *entityCharacter = searchCharacterByInternalObject(clientCharacter);

  if (!entityCharacter) {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find Shared NetworkEntity for the Client Character.");
  } else {
    multiplayerLogger.WriteLine(Info, L"Server sending client character ID reply: %i", entityCharacter->getCommonId());

    NetworkMessages::ReplyCharacterId msgReplyCharacterId;
    msgReplyCharacterId.set_id(entityCharacter->getCommonId());

    Server::getSingleton().SendMessage<NetworkMessages::ReplyCharacterId>(clientAddress, S_REPLY_CHARID, &msgReplyCharacterId);
  }
}

void Server::HandleReplyCharacterInfo(const SystemAddress clientAddress, NetworkMessages::ReplyCharacterInfo *msgReplyCharacterInfo)
{
  NetworkMessages::Character msgPlayer;
  NetworkMessages::Position  msgPosition;
  NetworkEntity *entityCharacter;

  msgPlayer = msgReplyCharacterInfo->player();

  // This will broadcast to all clients except the one we received it from
  Server::getSingleton().BroadcastMessage<NetworkMessages::Character>(clientAddress, S_PUSH_NEWCHAR, &msgPlayer);

  Vector3 posCharacter;
  Vector3 posPlayer;
  posPlayer.x = msgPlayer.position().x();
  posPlayer.y = msgPlayer.position().y();
  posPlayer.z = msgPlayer.position().z();

  // Create this Player on this instance
  CResourceManager *resourceManager = (CResourceManager *)gameClient->pCPlayer->pCResourceManager;
  CMonster *clientCharacter = resourceManager->CreateMonster(msgPlayer.guid(), 1, true);

  if (clientCharacter) {
    clientCharacter->characterName.assign(convertAcsiiToWide(msgPlayer.name()));
    clientCharacter->SetAlignment(1);

    // Lock the creation so we don't resend to all the clients
    Server::getSingleton().SetSuppressed_SendCharacterCreation(true);
    gameClient->pCLevel->CharacterInitialize(clientCharacter, &posPlayer, 0);
    Server::getSingleton().SetSuppressed_SendCharacterCreation(false);

    // Send this ID to the Client that created the character
    entityCharacter = searchCharacterByInternalObject(clientCharacter);
    if (!entityCharacter) {
      multiplayerLogger.WriteLine(Error, L"Error: Could not find Shared NetworkEntity for the Client Character.");
      log(L"Error: Could not find Shared NetworkEntity for the Client Character.");
    } else {
      multiplayerLogger.WriteLine(Info, L"Server sending client character ID reply: %i", entityCharacter->getCommonId());

      NetworkMessages::ReplyCharacterId msgReplyCharacterId;
      msgReplyCharacterId.set_id(entityCharacter->getCommonId());
      msgReplyCharacterId.set_guid(clientCharacter->GUID);

      Server::getSingleton().SendMessage<NetworkMessages::ReplyCharacterId>(clientAddress, S_REPLY_CHARID, &msgReplyCharacterId);
    }
  } else {
    log("Error: clientCharacter is null!");
    multiplayerLogger.WriteLine(Error, L"Error: clientCharacter is null!");
  }

  log(L"Minion Count: %i", msgPlayer.minion_size());

  // Create the pets
  for (int i = 0; i < msgPlayer.minion_size(); i++) {
    NetworkMessages::Character msgMinion = msgPlayer.minion().Get(i);
    posCharacter.x = msgMinion.position().x();
    posCharacter.y = msgMinion.position().y();
    posCharacter.z = msgMinion.position().z();

    // Create this Player on this instance
    CMonster *clientMinion = resourceManager->CreateMonster(msgMinion.guid(), 1, true);
    if (clientMinion) {
      clientMinion->characterName.assign(convertAcsiiToWide(msgMinion.name()));
      clientMinion->SetAlignment(1);

      // Lock the creation so we don't resend to all the clients
      Server::getSingleton().SetSuppressed_SendCharacterCreation(true);
      gameClient->pCLevel->CharacterInitialize(clientMinion, &posCharacter, 0);
      Server::getSingleton().SetSuppressed_SendCharacterCreation(false);  

      // Add the minion to the player
      clientCharacter->AddMinion(clientMinion);
      
      // Send this ID to the Client that created the character
      entityCharacter = searchCharacterByInternalObject(clientMinion);
      if (!entityCharacter) {
        multiplayerLogger.WriteLine(Error, L"Error: Could not find Shared NetworkEntity for the Client Minion Character.");
        log(L"Error: Could not find Shared NetworkEntity for the Client Minion Character.");
      } else {
        multiplayerLogger.WriteLine(Info, L"Server sending client character ID reply: %i", entityCharacter->getCommonId());

        NetworkMessages::ReplyCharacterId msgReplyCharacterId;
        msgReplyCharacterId.set_id(entityCharacter->getCommonId());
        msgReplyCharacterId.set_guid(clientMinion->GUID);

        Server::getSingleton().SendMessage<NetworkMessages::ReplyCharacterId>(clientAddress, S_REPLY_CHARID, &msgReplyCharacterId);
      }
    } else {
      log("Error: clientMinion is null!");
      multiplayerLogger.WriteLine(Error, L"Error: clientMinion is null!");
    }
  }
}


void Server::HandleCharacterDestination(const SystemAddress clientAddress, u32 commonId, Vector3 destination)
{
  multiplayerLogger.WriteLine(Info, L"Server received character setDestination (CommonID = %x), Position = %f, %f",
    commonId, destination.x, destination.z);

  // Search for our Character given the CommonID
  NetworkEntity *entity = searchCharacterByCommonID(commonId);

  if (!entity) {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find internal object in shared characters from commonId: %x when receiving character destination.", commonId);
    return;
  }

  CCharacter *character = (CCharacter *)entity->getInternalObject();

  character->SetDestination(gameClient->pCLevel, destination.x, destination.z);
}


void Server::HandleInventoryAddEquipment(const SystemAddress clientAddress, u32 ownerId, u32 equipmentId, u32 slot, u32 unk0, u64 guid)
{
  multiplayerLogger.WriteLine(Info, L"Server received inventory add equipment: (CharacterID = %x, EquipmentID = %x, slot = %x, guid = %016I64X)",
    ownerId, equipmentId, slot, guid);
  log(L"Server received inventory add equipment: (CharacterID = %x, EquipmentID = %x, slot = %x, guid = %016I64X)",
    ownerId, equipmentId, slot, guid);

  NetworkEntity* owner = searchCharacterByCommonID(ownerId);
  NetworkEntity* equipment = searchEquipmentByCommonID(equipmentId);
  CResourceManager *resourceManager = (CResourceManager *)gameClient->pCPlayer->pCResourceManager;

  if (owner) {
    CCharacter *characterOwner = (CCharacter*)owner->getInternalObject();
    CInventory *inventory = characterOwner->pCInventory;
    CEquipment *equipmentReal;

    if (equipment) {
      equipmentReal = (CEquipment*)equipment->getInternalObject();
    } else {
      multiplayerLogger.WriteLine(Info, L"Server: Could not find Equipment with ID = %x, creating it", equipmentId);
      log(L"Server: Could not find Equipment with ID = %x, creating it", equipmentId);

      equipmentReal = resourceManager->CreateEquipment(guid, 1, 1, 0);
  
      log(L"Server: Adding equipment to shared network equipment...");
      NetworkEntity *newEntity = addEquipment(equipmentReal, equipmentId);
    }

    multiplayerLogger.WriteLine(Info, L"Server: Added equipment.. now adding to inventory...");
    log(L"Server: Added equipment.. now adding to inventory...");

    // Suppress network send
    Server::getSingleton().SetSuppressed_SendEquipmentEquip(true);
    inventory->AddEquipment(equipmentReal, slot, unk0);
    Server::getSingleton().SetSuppressed_SendEquipmentEquip(false);

    // And send it to the other clients
    NetworkMessages::InventoryAddEquipment msgInventoryAddEquipment;
    msgInventoryAddEquipment.set_ownerid(owner->getCommonId());
    msgInventoryAddEquipment.set_equipmentid(equipment->getCommonId());
    msgInventoryAddEquipment.set_slot(slot);
    msgInventoryAddEquipment.set_unk0(unk0);

    Server::getSingleton().BroadcastMessage<NetworkMessages::InventoryAddEquipment>(clientAddress, S_PUSH_EQUIPMENT_EQUIP, &msgInventoryAddEquipment);

    //
    multiplayerLogger.WriteLine(Info, L"Server: Done adding equipment to inventory.");
    log(L"Server: Done adding equipment to inventory.");
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find Character with ID = %x", ownerId);
    log(L"Error: Could not find Character with ID = %x", ownerId);
  }

  log(L"HandleInventoryAddEquipment Done.");
}

void Server::Helper_SendEquipmentToClient(const SystemAddress clientAddress, CEquipment *equipment, TLMP::NetworkEntity *netEquipment)
{
  multiplayerLogger.WriteLine(Info, L"Server: Pushing existing Equipment (%016I64X %i) out to client that just joined...",
    equipment->GUID, netEquipment->getCommonId());
  log(L"Server: Pushing existing Equipment (%016I64X %i) out to client that just joined...",
    equipment->GUID, netEquipment->getCommonId());

  // Create a new network message for all clients to create this character
  NetworkMessages::Equipment msgEquipment;
  Helper_PopulateEquipmentMessage(&msgEquipment, equipment, netEquipment);

  log(L"  DEBUG: Equipment gemList: %p (Offset = %x)",
    &equipment->gemList, ((u32)this - (u32)&equipment->gemList));
  multiplayerLogger.WriteLine(Info, L"  Equipment contains %i Gems", equipment->gemList.size);
  log(L"  Equipment contains %i Gems", equipment->gemList.size);

  // Add the Gem Equipment
  for (u32 i = 0; i < equipment->gemList.size; i++) {
    multiplayerLogger.WriteLine(Info, L"Server: Adding gem...");
    log(L"Server: Adding gem...");

    NetworkMessages::Equipment *msgGem = msgEquipment.add_gems();
    CEquipment *gem = equipment->gemList[i];
    NetworkEntity *netGem = searchEquipmentByInternalObject(gem);
    
    // 
    if (netGem) {
      Helper_PopulateEquipmentMessage(msgGem, gem, netGem);
    } else {
      multiplayerLogger.WriteLine(Info, L"Could not find NetworkEntity for Gem, skipping equipment");
      log(L"Could not find NetworkEntity for Gem, skipping equipment");
      continue;
    }
  }

  // This will broadcast to all clients except the one we received it from
  Server::getSingleton().SendMessage<NetworkMessages::Equipment>(clientAddress, S_PUSH_NEWEQUIPMENT, &msgEquipment);
}

//
// Populates the msgEquipment with the given Equipment vars
void Server::Helper_PopulateEquipmentMessage(NetworkMessages::Equipment* msgEquipment, CEquipment *equipment, NetworkEntity *netEquipment)
{
  multiplayerLogger.WriteLine(Info, L"  Setting up regular variables...");
  log(L"  Setting up regular variables...");

  // Fill out the easy stuff
  msgEquipment->set_guid(equipment->GUID);
  msgEquipment->set_id(netEquipment->getCommonId());
  msgEquipment->set_stacksize(equipment->stackSize);
  msgEquipment->set_stacksizemax(equipment->stackSizeMax);
  msgEquipment->set_socketcount(equipment->socketCount);

  multiplayerLogger.WriteLine(Info, L"  Setting up Regular Enchants...");
  log(L"  Setting up Regular Enchants...");

  // Work on Regular Enchants
  u32 *itr = equipment->enchantListStart;
  u32 *itrType = equipment->enchantTypeListStart;
  while (itr != equipment->enchantListEnd) {
    log(L"  Adding Equipment Enchant: REGULAR");
    log(L"    (%i: %i)", *itrType, *itr);
    multiplayerLogger.WriteLine(Info, L"  Adding Equipment Enchant: REGULAR");
    multiplayerLogger.WriteLine(Info, L"    (%i: %i)", *itrType, *itr);

    NetworkMessages::EnchantType *msgEnchantType = msgEquipment->add_enchants();
    msgEnchantType->set_type(REGULAR);
    msgEnchantType->set_subtype(*itrType);
    msgEnchantType->set_value((float)*itr);

    itr++;
    itrType++;
  }
  
  CEffectManager *effectManager = equipment->pCEffectManager;

  if (effectManager) {
    multiplayerLogger.WriteLine(Info, L"  Setting up Effect Enchants...");
    log(L"  Setting up Effect Enchants...");

    // Work on Effect Enchants
    for (u32 i = 0; i < effectManager->effectList.size; i++) {
      CEffect *effect = effectManager->effectList[i];

      multiplayerLogger.WriteLine(Info, L"  Adding Equipment Effect Enchant: (%x: %f)", effect->effectType, effect->effectValue);
      log(L"  Adding Equipment Effect Enchant: (%x: %f)", effect->effectType, effect->effectValue);

      NetworkMessages::EnchantType *msgEnchantType = msgEquipment->add_enchants();
      msgEnchantType->set_type(effect->effectType);
      msgEnchantType->set_subtype(-1);  // Not used in this instance
      msgEnchantType->set_value(effect->effectValue);
    }
  }
}

void Server::HandleEquipmentDrop(u32 equipmentId, Vector3 position, bool unk0)
{
  multiplayerLogger.WriteLine(Info, L"Server received Equipment Drop: (EquipmentId = %x, Position: %f, %f, %f, Unk0: %i",
    equipmentId, position.x, position.y, position.z, unk0);
  log(L"Server received Equipment Drop: (EquipmentId = %x, Position: %f, %f, %f, Unk0: %i",
    equipmentId, position.x, position.y, position.z, unk0);

  NetworkEntity *netEquipment = searchEquipmentByCommonID(equipmentId);

  if (netEquipment) {
    CEquipment *equipment = (CEquipment *)netEquipment->getInternalObject();
    CLevel *level = gameClient->pCLevel;

    level->EquipmentDrop(equipment, position, unk0);
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find Equipment from CommonId: %x", equipmentId);
    log(L"Error: Could not find Equipment from CommonId: %x", equipmentId);
  }
}

void Server::HandleEquipmentCreation(const SystemAddress clientAddress, TLMP::NetworkMessages::Equipment *msgEquipment)
{
  u32 id = msgEquipment->id();
  u64 guid = msgEquipment->guid();
  u32 stacksize = msgEquipment->stacksize();
  u32 stacksizemax = msgEquipment->stacksizemax();
  u32 socketcount = msgEquipment->socketcount();
  u32 gemCount = msgEquipment->gems_size();
  u32 enchantCount = msgEquipment->enchants_size();
  u32 client_id = msgEquipment->client_id();

  multiplayerLogger.WriteLine(Info, L"Server received equipment creation: (CommonID = %x) (GUID = %016I64X) (Stack: %i/%i) (SocketCount: %i)",
    id, guid, stacksize, stacksizemax, socketcount);
  log(L"Server received equipment creation: (CommonID = %x) (GUID = %016I64X) (Stack: %i/%i) (SocketCount: %i)",
    id, guid, stacksize, stacksizemax, socketcount);

  CResourceManager *resourceManager = (CResourceManager *)gameClient->pCPlayer->pCResourceManager;

  if (resourceManager) {
    // Suppress the server from sending this out, we do it afterwards only for the other clients
    Server::getSingleton().SetSuppressed_SendEquipmentCreation(true);
    CEquipment *equipment = resourceManager->CreateEquipment(guid, 1, 1, 0);
    Server::getSingleton().SetSuppressed_SendEquipmentCreation(false);

    equipment->socketCount = socketcount;
    equipment->stackSize = stacksize;
    equipment->stackSizeMax = stacksizemax;
    
    multiplayerLogger.WriteLine(Info, L"Server: Adding gems to new equipment...");
    log(L"Server: Adding gems to new equipment...");

    // Add the gems
    for (u32 i = 0; i < gemCount; i++) {
      NetworkMessages::Equipment msgGem = msgEquipment->gems().Get(i);
      CEquipment *gem = resourceManager->CreateEquipment(msgGem.guid(), 1, 1, 0);
      
      multiplayerLogger.WriteLine(Info, L"Server: Adding gem to shared network equipment: %i", msgGem.id());
      log(L"Server: Adding gem to shared network equipment: %i", msgGem.id());
      NetworkEntity *newEntity = addEquipment(gem, msgGem.id());

      for (int j = 0; j < msgGem.enchants_size(); j++) {
        NetworkMessages::EnchantType msgGemEnchantType = msgGem.enchants().Get(j);
        gem->AddEnchant((EffectType)msgGemEnchantType.type(), (EnchantType)msgGemEnchantType.subtype(), msgGemEnchantType.value());
      }

      equipment->gemList.push(gem);
    }

    multiplayerLogger.WriteLine(Info, L"Server: Adding enchants to new equipment...");
    log(L"Server: Adding enchants to new equipment...");

    // Add the enchants
    for (u32 i = 0; i < enchantCount; i++) {
      NetworkMessages::EnchantType msgEnchantType = msgEquipment->enchants().Get(i);
      equipment->AddEnchant((EffectType)msgEnchantType.type(), (EnchantType)msgEnchantType.subtype(), msgEnchantType.value());
    }

    multiplayerLogger.WriteLine(Info, L"Server: Adding equipment to shared network equipment");
    log(L"Server: Adding equipment to shared network equipment");
    NetworkEntity *newEntity = addEquipment(equipment);

    multiplayerLogger.WriteLine(Info, L"Server: Broadcasting new equipment to all other clients.");
    log(L"Server: Broadcasting new equipment to all other clients.");

    // Send the Equipment w/ CommonID back to the clients
    // Simply change the slot ID on the message we received and send it back out to all other clients
    msgEquipment->set_id(newEntity->getCommonId());
    Server::getSingleton().BroadcastMessage<NetworkMessages::Equipment>(clientAddress, S_PUSH_NEWEQUIPMENT, msgEquipment);

    multiplayerLogger.WriteLine(Info, L"Server: Sending equipment common ID to owner Client of equipment.");
    log(L"Server: Sending equipment common ID to owner Client of equipment.");

    // Send a special equipment common ID message to the client that sent us their equipment
    NetworkMessages::EquipmentSetID msgEquipmentSetID;
    msgEquipmentSetID.set_guid(guid);
    msgEquipmentSetID.set_client_id(client_id);
    msgEquipmentSetID.set_id(newEntity->getCommonId());
    Server::getSingleton().SendMessage<NetworkMessages::Equipment>(clientAddress, S_REPLY_EQUIPMENT_ID, &msgEquipmentSetID);
  }
}

void Server::HandleEquipmentPickup(u32 characterId, u32 equipmentId)
{
  NetworkEntity *netEquipment = searchEquipmentByCommonID(equipmentId);
  NetworkEntity *netCharacter = searchCharacterByCommonID(characterId);

  if (netEquipment) {
    if (netCharacter) {
      CEquipment *equipment = (CEquipment *)netEquipment->getInternalObject();
      CCharacter *character = (CCharacter *)netCharacter->getInternalObject();

      character->PickupEquipment(equipment, gameClient->pCLevel);
    } else {
      multiplayerLogger.WriteLine(Error, L"Error: Could not find Character from CommonId: %x", characterId);
      log(L"Error: Could not find Character from CommonId: %x", characterId);
    }
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find Equipment from CommonId: %x", equipmentId);
    log(L"Error: Could not find Equipment from CommonId: %x", equipmentId);
  }
}

void Server::HandleInventoryRemoveEquipment(const SystemAddress clientAddress, u32 ownerId, u32 equipmentId)
{
  multiplayerLogger.WriteLine(Info, L"Server received inventory remove equipment: (CharacterID = %x, EquipmentID = %x)",
    ownerId, equipmentId);
  log(L"Server received inventory remove equipment: (CharacterID = %x, EquipmentID = %x)",
    ownerId, equipmentId);

  NetworkEntity* owner = searchCharacterByCommonID(ownerId);
  NetworkEntity* equipment = searchEquipmentByCommonID(equipmentId);

  if (owner) {
    if (equipment) {
      CCharacter *characterOwner = (CCharacter*)owner->getInternalObject();
      CEquipment *equipmentReal = (CEquipment*)equipment->getInternalObject();

      CInventory *inventory = characterOwner->pCInventory;

      // Suppress this and send it manually
      Server::getSingleton().SetSuppressed_SendEquipmentUnequip(true);
      inventory->RemoveEquipment(equipmentReal);
      Server::getSingleton().SetSuppressed_SendEquipmentUnequip(false);

      // And send to other clients
      // Create a Network Message for sending off to clients the equipment addition to the inventory
      NetworkMessages::InventoryRemoveEquipment msgInventoryRemoveEquipment;
      msgInventoryRemoveEquipment.set_ownerid(owner->getCommonId());
      msgInventoryRemoveEquipment.set_equipmentid(equipment->getCommonId());

      Server::getSingleton().BroadcastMessage<NetworkMessages::InventoryRemoveEquipment>(clientAddress, S_PUSH_EQUIPMENT_UNEQUIP, &msgInventoryRemoveEquipment);
    } else {
      multiplayerLogger.WriteLine(Error, L"Error: Could not find Equipment with ID = %x", equipmentId);
      log(L"Error: Could not find Equipment with ID = %x", equipmentId);
    }
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find Character with ID = %x", ownerId);
    log(L"Error: Could not find Character with ID = %x", ownerId);
  }
}
