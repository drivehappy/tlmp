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

  Reset();

  m_bWaitingForGame = true;

  SetCallback_OnClientConnect(&OnClientConnected);
  SetCallback_OnClientDisconnect(&OnClientDisconnected);

  srand(time(NULL));
  m_Seed = rand();
}

Server::~Server()
{
  delete m_pBitStream;
}

void Server::Reset()
{
  m_bSuppressNetwork_SetDestination = false;
  m_bSuppressNetwork_SendCharacterCreation = false;
  m_bSuppressNetwork_SendEquipmentEquip = false;
  m_bSuppressNetwork_SendEquipmentUnequip = false;
  m_bSuppressNetwork_SendEquipmentCreation = false;
  m_bSuppressNetwork_SendEquipmentStack = false;
  m_bSuppressNetwork_SendCharacterAttack = false;
  m_bSuppressNetwork_AddExperience = false;
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

  // Debugging - allow 2 minutes until disconnect
  m_pServer->SetTimeoutTime(30000, UNASSIGNED_SYSTEM_ADDRESS);
  //--

  SocketDescriptor socketDescriptor(port, 0);
  m_pServer->Startup(maxconnections, 30, &socketDescriptor, 1);
  m_pServer->SetMaximumIncomingConnections(maxconnections);

  multiplayerLogger.WriteLine(Info, L"Server listening on port %i (max connections = %i)", port, maxconnections);

  vector<OnListening>::iterator itr;
  for (itr = m_pOnListening.begin(); itr != m_pOnListening.end(); itr++) {
    (*itr)(NULL);
  }
}

void Server::Shutdown()
{
  if (m_pServer) {
    Reset();

    m_pServer->Shutdown(0);
    m_pServer = NULL;

    multiplayerLogger.WriteLine(Info, L"Server shutdown");

    vector<OnShutdown>::iterator itr;
    for (itr = m_pOnShutdown.begin(); itr != m_pOnShutdown.end(); itr++) {
      (*itr)(NULL);
    }
  }
}

void Server::SetCallback_OnListening(OnListening callback)
{
  m_pOnListening.push_back(callback);
}

void Server::SetCallback_OnShutdown(OnShutdown callback)
{
  m_pOnShutdown.push_back(callback);
}

void Server::SetCallback_OnClientConnect(OnClientConnect callback)
{
  m_pOnClientConnect.push_back(callback);
}

void Server::SetCallback_OnClientDisconnect(OnClientDisconnect callback)
{
  m_pOnClientDisconnect.push_back(callback);
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
      {
        multiplayerLogger.WriteLine(Info, L"Client connected");
        logColor(B_GREEN, L"Client connected");

        vector<OnClientConnect>::iterator itr;
        for (itr = m_pOnClientConnect.begin(); itr != m_pOnClientConnect.end(); itr++) {
          SystemAddress *sysAddress = new SystemAddress();
          *sysAddress = packet->systemAddress;
          (*itr)((void*)sysAddress);
        }
      }
      break;

    case ID_DISCONNECTION_NOTIFICATION:
      {
        multiplayerLogger.WriteLine(Info, L"Client disconnected");
        logColor(B_GREEN, L"Client disconnected");

        vector<OnClientDisconnect>::iterator itr;
        for (itr = m_pOnClientDisconnect.begin(); itr != m_pOnClientDisconnect.end(); itr++) {
          SystemAddress *sysAddress = new SystemAddress();
          *sysAddress = packet->systemAddress;
          (*itr)((void*)sysAddress);
        }
      }
      break;

    case ID_CONNECTION_LOST:
      {
        multiplayerLogger.WriteLine(Info, L"Client disconnected");
        logColor(B_GREEN, L"Client disconnected");

        vector<OnClientDisconnect>::iterator itr;
        for (itr = m_pOnClientDisconnect.begin(); itr != m_pOnClientDisconnect.end(); itr++) {
          SystemAddress *sysAddress = new SystemAddress();
          *sysAddress = packet->systemAddress;
          (*itr)((void*)sysAddress);
        }
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

void Server::WorkMessage(const SystemAddress address, Network::Message msg, RakNet::BitStream *bitStream)
{
  if (msg >= sizeof(MessageString) / sizeof(char*)) {
    log("Error: Message out of range.");
    multiplayerLogger.WriteLine(Info, L"Error: Message out of range.");
  }

  wstring msgString = convertAsciiToWide(MessageString[msg]);

  // For sake of less spam
  if (msg != C_PUSH_CHARACTER_SETDEST) {
    multiplayerLogger.WriteLine(Info, L"Server Received Message: %s", msgString.c_str());
    logColor(B_GREEN, L"Server Received Message: %s", msgString.c_str());
  }

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

      HandleGameEnter(address, msgGameEnter);
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

      HandleEquipmentDrop(address, equipmentId, position, unk0);
    }
    break;

  case C_REQUEST_EQUIPMENT_PICKUP:
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

  case C_REQUEST_EQUIPMENT_IDENTIFY:
    {
      NetworkMessages::EquipmentIdentify *msgEquipmentIdentify = ParseMessage<NetworkMessages::EquipmentIdentify>(m_pBitStream);

      HandleEquipmentIdentify(msgEquipmentIdentify);
    }
    break;

  case C_PUSH_EQUIPMENT_ADDENCHANT:
    break;

  case C_PUSH_EQUIPMENT_ADDSOCKET:
    break;

  case C_PUSH_CHARACTER_SETDEST:
    {
      NetworkMessages::CharacterDestination *msgCharacterDestination = ParseMessage<NetworkMessages::CharacterDestination>(m_pBitStream);
      NetworkMessages::Position msgDestination = msgCharacterDestination->destination();

      u32 commonId = msgCharacterDestination->id();

      Vector3 destination;
      destination.x = msgDestination.x();
      destination.z = msgDestination.z();

      u8 running = msgCharacterDestination->running();
      u8 attacking = msgCharacterDestination->attacking();

      HandleCharacterDestination(address, commonId, destination, running, attacking);
    }
    break;

  case C_PUSH_CHARACTER_ACTION:
    {
      NetworkMessages::CharacterAction *msgCharacterAction = ParseMessage<NetworkMessages::CharacterAction>(m_pBitStream);

      HandleCharacterSetAction(address, msgCharacterAction);
    }
    break;

  case C_PUSH_CHARACTER_ATTACK:
    {
      //* Crashes the server when receiving
      NetworkMessages::CharacterAttack *msgCharacterAttack = ParseMessage<NetworkMessages::CharacterAttack>(m_pBitStream);

      HandleCharacterAttack(msgCharacterAttack);
      //*/
    }
    break;

  case C_REQUEST_CHARACTER_USESKILL:
    {
      NetworkMessages::CharacterUseSkill *msgCharacterUseSkill = ParseMessage<NetworkMessages::CharacterUseSkill>(m_pBitStream);

      HandleCharacterUseSkill(msgCharacterUseSkill);
    }
    break;
    
  case C_PUSH_EQUIPMENT_STACKUPDATE:
    {
      NetworkMessages::EquipmentUpdateStackSize *msgEquipmentUpdateStackSize = ParseMessage<NetworkMessages::EquipmentUpdateStackSize>(m_pBitStream);

      HandleEquipmentUpdateStack(address, msgEquipmentUpdateStackSize);
    }
    break;

  case C_PUSH_EQUIPMENT_ADD_GEM:
    {
      NetworkMessages::EquipmentAddGem *msgEquipmentAddGem = ParseMessage<NetworkMessages::EquipmentAddGem>(m_pBitStream);

      HandleEquipmentAddGem(address, msgEquipmentAddGem);
    }
    break;

  case C_PUSH_EQUIPMENT_REMOVE_GEMS:
    {
      NetworkMessages::EquipmentRemoveGems *msgEquipmentRemoveGems = ParseMessage<NetworkMessages::EquipmentRemoveGems>(m_pBitStream);

      HandleEquipmentRemoveGems(msgEquipmentRemoveGems);
    }
    break;

  case C_PUSH_CHAT_PLAYER:
    {
      NetworkMessages::ChatPlayerText *msgChatPlayerText = ParseMessage<NetworkMessages::ChatPlayerText>(m_pBitStream);

      HandleChatMessage(msgChatPlayerText);
    }
    break;

  case C_REQUEST_TRIGGER_TRIGGERED:
    {
      NetworkMessages::TriggerUnitTriggered *msgTriggerUnitTriggered = ParseMessage<NetworkMessages::TriggerUnitTriggered>(m_pBitStream);

      HandleTriggerUnitTriggered(msgTriggerUnitTriggered);
    }
    break;

  case C_REQUEST_BREAKABLE_TRIGGERED:
    {
      NetworkMessages::BreakableTriggered *msgBreakableTriggered = ParseMessage<NetworkMessages::BreakableTriggered>(m_pBitStream);

      HandleBreakableTriggered(msgBreakableTriggered);
    }
    break;

  case C_REQUEST_CHARACTER_SETTARGET:
    {
      NetworkMessages::CharacterSetTarget *msgCharacterSetTarget = ParseMessage<NetworkMessages::CharacterSetTarget>(m_pBitStream);

      //HandleCharacterSetTarget(msgCharacterSetTarget);
    }
    break;

    
  case C_REQUEST_CHARACTER_RESURRECT:
    {
      NetworkMessages::CharacterResurrect *msgCharacterResurrect = ParseMessage<NetworkMessages::CharacterResurrect>(m_pBitStream);

      HandleCharacterResurrect(msgCharacterResurrect);
    }
    break;

  case C_REQUEST_ORIENTATION:
    {
      NetworkMessages::CharacterOrientation *msgCharacterOrientation = ParseMessage<NetworkMessages::CharacterOrientation>(m_pBitStream);

      HandleCharacterOrientation(msgCharacterOrientation);
    }
    break;

  case C_REQUEST_WEAPONSWAP:
    {
      NetworkMessages::PlayerSwapWeapons *msgPlayerSwapWeapons = ParseMessage<NetworkMessages::PlayerSwapWeapons>(m_pBitStream);

      HandlePlayerWeaponSwap(msgPlayerSwapWeapons);
    }
    break;

  case C_REQUEST_SKILLLEVEL:
    {
      NetworkMessages::CharacterSetSkillPoints *msgCharacterSetSkillPoints = ParseMessage<NetworkMessages::CharacterSetSkillPoints>(m_pBitStream);

      HandleCharacterSetSkillPoints(msgCharacterSetSkillPoints);
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
void Server::HandleGameEnter(const SystemAddress clientAddress, NetworkMessages::GameEnter *msgGameEnter)
{
  multiplayerLogger.WriteLine(Info, L"Client has Entered the Game");

  vector<NetworkEntity*>::iterator itr;

  // Send the seed
  NetworkMessages::RandomSeed msgRandomSeed;
  msgRandomSeed.set_seed(m_Seed);
  
  Server::getSingleton().SendMessage<NetworkMessages::RandomSeed>(clientAddress, S_PUSH_RANDOM_SEED, &msgRandomSeed);

  // Grab the client's current level information
  NetworkMessages::CurrentLevel msgCurrentLevel = msgGameEnter->currentlevel();
  wstring dungeonSection = convertAsciiToWide(msgCurrentLevel.dungeonsection());
  u32 relativeLevel = msgCurrentLevel.relativelevel();
  u32 absoluteLevel = msgCurrentLevel.absolutelevel();

  // Check if the Client's current level is the same as ours
  if (!wcscmp(dungeonSection.c_str(), gameClient->pCDungeon->name0.c_str()) &&
      relativeLevel == gameClient->level) //&&
      //absoluteLevel == gameClient->levelAbsolute)
  {
    log(L"~~~ Server: Client is in the same level, supplying info...");

    // Check for destroyed equipment and remove them
    RemoveDestroyedEquipmentFromNetwork();

    CLevel *level = gameClient->pCLevel;

    // Send the Sync-Up for Trigger Units (since the server doesn't push these out,
    //  the level is assumed to be generated the same (same seed value),
    //  assuming they are created in the same order from the layout
    Helper_SendTriggerUnitSync(clientAddress);

    // Send all of the existing equipment in the game to the client
    LinkedListNode* itrInv = *level->ppCCharacters2;
    while (itrInv != NULL) {
      CCharacter* character = (CCharacter*)itrInv->pCBaseUnit;

      CInventory* inventory = character->pCInventory;
      for (u32 i = 0; i < inventory->equipmentList.size; i++) {
        CEquipmentRef *equipmentRef = inventory->equipmentList[i];
        CEquipment* equipment = equipmentRef->pCEquipment;

        NetworkEntity *entity = searchEquipmentByInternalObject(equipment);

        if (!entity) {
          entity = addEquipment(equipment);
        }

        if (equipment) {
          Helper_SendEquipmentToClient(clientAddress, equipment, entity);
        }
      }

      itrInv = itrInv->pNext;
    }

    // Send all of the ground equipment in game to the client
    LinkedListNode* itrEquipment = *level->ppCItems;
    while (itrEquipment) {
      CEquipment *equipment = (CEquipment*)itrEquipment->pCBaseUnit;

      // Check vtable to match up with CEquipment type
      if (*(u32*)equipment == 0xA7EA8C) {
        NetworkEntity *entity = searchEquipmentByInternalObject(equipment);

        if (!entity) {
          entity = addEquipment(equipment);
        }

        if (equipment) {
          Helper_SendEquipmentToClient(clientAddress, equipment, entity);
          Helper_SendGroundEquipmentToClient(clientAddress, equipment, entity);
        }
      }

      itrEquipment = itrEquipment->pNext;
    }
    

    /* Use the Level's character's inventories instead
    for (itr = NetworkSharedEquipment->begin(); itr != NetworkSharedEquipment->end(); itr++) {
      CEquipment *equipment = (CEquipment*)((*itr)->getInternalObject());

      Helper_SendEquipmentToClient(clientAddress, equipment, (*itr));
      log(L"Server: Sending Equipment To New Client: %s", equipment->nameReal.c_str());
    }
    */
    
    /*
    // Send all of the Equipment on the Ground in the game
    for (itr = ServerEquipmentOnGround->begin(); itr != ServerEquipmentOnGround->end(); itr++) {
      CEquipment *equipment = (CEquipment*)((*itr)->getInternalObject());

      Helper_SendGroundEquipmentToClient(clientAddress, equipment, (*itr));
    }
    */

    // Send all of the existing characters in the game to the client
    for (itr = NetworkSharedCharacters->begin(); itr != NetworkSharedCharacters->end(); itr++) {
      CCharacter* character = (CCharacter*)((*itr)->getInternalObject());

      if (character->GUID == 0xFFFFFFFFFFFFFFFF || character->destroy)
      {
        continue;
      }

      multiplayerLogger.WriteLine(Info, L"Server: Pushing existing Character (%016I64X %i %s) out to client that just joined...",
        character->GUID, (*itr)->getCommonId(), character->characterName.c_str());
      log(L"Server: Pushing existing Character (%p %016I64X %i %s) out to client that just joined...",
        character, character->GUID, (*itr)->getCommonId(), character->characterName.c_str());

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
      //msgNewCharacter.set_health(character->healthMax);
      msgNewCharacter.set_health(character->healthMax4);
      msgNewCharacter.set_mana(character->manaMax);
      msgNewCharacter.set_level(character->level);
      msgNewCharacter.set_weaponsetactive(character->weaponSetToggle);

      // This allows a workaround for the modified attributes needed for equipment
      /*
      msgNewCharacter.set_strength(character->baseStrength);
      msgNewCharacter.set_dexterity(character->baseDexterity);
      msgNewCharacter.set_defense(character->baseDefense);
      msgNewCharacter.set_magic(character->baseMagic);
      */
      msgNewCharacter.set_strength(10000);
      msgNewCharacter.set_dexterity(10000);
      msgNewCharacter.set_defense(10000);
      msgNewCharacter.set_magic(10000);

      msgNewCharacter.set_alignment(character->alignment);

      // Build the inventory tab indicies/tab size
      Helper_BuildInventoryTabIndexSize(msgNewCharacter, character);
      /*
      if (character->pCInventory) {
        CInventory *inv = character->pCInventory;
        for (u32 i = 0; i < inv->tabIndices.size(); ++i) {
          NetworkMessages::InventoryTabSize *msgTabSize = msgNewCharacter.add_inventory_tabsize();

          msgTabSize->set_tabindex(i);

          if (i + 1 < inv->tabIndices.size()) {
            // Use the next index to compute size
            msgTabSize->set_tabsize(inv->tabIndices[i + 1] - inv->tabIndices[i]);
          } else {
            // Use the inventory's max size to compute last element's size
            msgTabSize->set_tabsize(inv->maxSize - inv->tabIndices[i]);
          }
        }
      }*/

      // This will broadcast to all clients except the one we received it from
      Server::getSingleton().SendMessage<NetworkMessages::Character>(clientAddress, S_PUSH_NEWCHAR, &msgNewCharacter);

      // Old: Move through the body slots and send an equip message to show the initial equipment
      ///// New: Move throug all inventory slots and add equipment
      if (character->pCInventory) {
        //for (u32 i = 0; i <= 0x0C; i++) {
        for (u32 i = 0; i <= 0x100; i++) {
          CEquipment* equipment = character->pCInventory->GetEquipmentFromSlot(i);

          if (equipment) {
            NetworkEntity* netEquipment = searchEquipmentByInternalObject(equipment);

            if (netEquipment) {
              NetworkMessages::InventoryAddEquipment msgInventoryAddEquipment;
              msgInventoryAddEquipment.set_equipmentid(netEquipment->getCommonId());
              msgInventoryAddEquipment.set_guid(equipment->GUID);
              msgInventoryAddEquipment.set_ownerid((*itr)->getCommonId());
              msgInventoryAddEquipment.set_slot(i);
              msgInventoryAddEquipment.set_unk0(1);

              Server::getSingleton().SendMessage<NetworkMessages::InventoryAddEquipment>(clientAddress, S_PUSH_EQUIPMENT_EQUIP, &msgInventoryAddEquipment);
            }
          }
        }
      } else {
        log(L"Error: Character has no Inventory!");
        multiplayerLogger.WriteLine(Error, L"Error: Character has no Inventory!");
      }
    }

    // Send a request for character info
    NetworkMessages::RequestCharacterInfo msgRequestCharacterInfo;
    SendMessage<NetworkMessages::RequestCharacterInfo>(clientAddress, S_REQUEST_CHARINFO, &msgRequestCharacterInfo);
  } else {
    log(L"~~~ Server: Client isn't in the same level: Diffs:");
    log(L"    Server Name: %s, Client Name: %s", gameClient->pCDungeon->name0.c_str(), dungeonSection.c_str());
    log(L"    Server RelLevel: %i, Client RelLevel: %i", gameClient->level, relativeLevel);
    log(L"    Server AbsLevel: %i, Client AbsLevel: %i", gameClient->levelAbsolute, absoluteLevel);

    // Send the current level to the Client to load
    Helper_SendCurrentLevel(clientAddress);
  }

}

// Server receives game exit from client
void Server::HandleGameExited(const SystemAddress clientAddress)
{
  multiplayerLogger.WriteLine(Info, L"Client has Exited the Game");
  logColor(B_RED, L"Client has Exited the Game");

  Helper_RemoveClient(clientAddress);
}

void Server::HandleReplyCharacterInfo(const SystemAddress clientAddress, NetworkMessages::ReplyCharacterInfo *msgReplyCharacterInfo)
{
  NetworkMessages::Character msgPlayer;
  NetworkMessages::Position  msgPosition;
  NetworkEntity *entityCharacter;

  msgPlayer = msgReplyCharacterInfo->player();
  
  Vector3 posCharacter;
  Vector3 posPlayer;
  posPlayer.x = msgPlayer.position().x();
  posPlayer.y = msgPlayer.position().y();
  posPlayer.z = msgPlayer.position().z();

  u32 health = msgPlayer.health();
  u32 mana = msgPlayer.mana();
  u32 levelCharacter = msgPlayer.level();
  u32 strength = msgPlayer.strength();
  u32 dexterity = msgPlayer.dexterity();
  u32 defense = msgPlayer.defense();
  u32 magic = msgPlayer.magic();
  u32 weaponSetActive = msgPlayer.weaponsetactive();

  // Create this Player on this instance
  CResourceManager *resourceManager = (CResourceManager *)gameClient->pCPlayer->pCResourceManager;
  //Testing Skills
  CMonster *clientCharacter = resourceManager->CreateMonster(msgPlayer.guid(), levelCharacter, true);
  //CPlayer *clientCharacter = resourceManager->CreatePlayer(L"Destroyer", 1);
  //clientCharacter->level = levelCharacter;

  if (clientCharacter) {
    // Setup the skills
    /*
    u32 skillNameCount = msgPlayer.skills().size();
    for (u32 i = 0; i < skillNameCount; i++) {
      wstring skillName = convertAsciiToWide(msgPlayer.skills().Get(i));

      logColor(B_RED, L"Adding Skill to Character: %s", skillName.c_str());
      clientCharacter->AddSkill(&skillName, 1);
    }
    */

    // Setup the name and alignment
    clientCharacter->characterName.assign(convertAsciiToWide(msgPlayer.name()));
    clientCharacter->SetAlignment(1);

    // Lock the creation so we do not resend to all the clients
    Server::getSingleton().SetSuppressed_SendCharacterCreation(true);
    gameClient->pCLevel->CharacterInitialize(clientCharacter, &posPlayer, 0);
    Server::getSingleton().SetSuppressed_SendCharacterCreation(false);

    // Add the correct inventory slot sizes
    clientCharacter->pCInventory->AddTabSize(1, 0x15);   // Spells
    clientCharacter->pCInventory->AddTabSize(2, 0x15);   // Fish - Yummy

    // Base stats
    clientCharacter->healthMax = health;
    clientCharacter->manaMax = mana;
    clientCharacter->baseStrength = strength;
    clientCharacter->baseDexterity = dexterity;
    clientCharacter->baseDefense = defense;
    clientCharacter->baseMagic = magic;

    clientCharacter->weaponSetToggle = weaponSetActive;

    // Inventory tab sizes
    Helper_ExtractInventoryTabIndexSize(&msgPlayer, clientCharacter);

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

      // This will broadcast to all clients except the one we received it from
      msgPlayer.set_id(entityCharacter->getCommonId());
      Server::getSingleton().BroadcastMessage<NetworkMessages::Character>(clientAddress, S_PUSH_NEWCHAR, &msgPlayer);

      // Add this character to our ClientAddressCharacter map
      HandleAddClientCharacter(clientAddress, clientCharacter);
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
    CMonster *clientMinion = resourceManager->CreateMonster(msgMinion.guid(), msgMinion.level(), true);
    if (clientMinion) {
      clientMinion->characterName.assign(convertAsciiToWide(msgMinion.name()));
      clientMinion->SetAlignment(1);

      // Lock the creation so we don't resend to all the clients
      Server::getSingleton().SetSuppressed_SendCharacterCreation(true);
      gameClient->pCLevel->CharacterInitialize(clientMinion, &posCharacter, 0);
      Server::getSingleton().SetSuppressed_SendCharacterCreation(false);  

      // Add the minion to the player
      clientCharacter->AddMinion(clientMinion);

      Helper_ExtractInventoryTabIndexSize(&msgMinion, clientMinion);
      
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

        // This will broadcast to all clients except the one we received it from
        msgMinion.set_id(entityCharacter->getCommonId());
        Server::getSingleton().BroadcastMessage<NetworkMessages::Character>(clientAddress, S_PUSH_NEWCHAR, &msgMinion);

        // Add this character to our ClientAddressCharacter map
        HandleAddClientCharacter(clientAddress, clientMinion);
      }
    } else {
      log("Error: clientMinion is null!");
      multiplayerLogger.WriteLine(Error, L"Error: clientMinion is null!");
    }
  }
}


void Server::HandleCharacterDestination(const SystemAddress clientAddress, u32 commonId, Vector3 destination, u8 running, u8 attacking)
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
  character->running = running;
  character->attacking = attacking;
}


void Server::HandleInventoryAddEquipment(const SystemAddress clientAddress, u32 ownerId, u32 equipmentId, u32 slot, u32 unk0, u64 guid)
{
  multiplayerLogger.WriteLine(Info, L"Server received inventory add equipment: (CharacterID = %x, EquipmentID = %x, slot = %x, guid = %016I64X)",
    ownerId, equipmentId, slot, guid);
  //log(L"Server received inventory add equipment: (CharacterID = %x, EquipmentID = %x, slot = %x, guid = %016I64X)",
  //  ownerId, equipmentId, slot, guid);

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
      //log(L"Server: Could not find Equipment with ID = %x, creating it", equipmentId);

      equipmentReal = resourceManager->CreateEquipment(guid, 1, 1, 0);
  
      //log(L"Server: Adding equipment to shared network equipment...");
      NetworkEntity *newEntity = addEquipment(equipmentReal, equipmentId);
    }

    multiplayerLogger.WriteLine(Info, L"Server: Added equipment.. now adding to inventory...");
    //log(L"Server: Added equipment.. now adding to inventory...");

    multiplayerLogger.WriteLine(Info, L"  Character inventory size PRE: %i", inventory->equipmentList.size);

    // Suppress network send
    Server::getSingleton().SetSuppressed_SendEquipmentEquip(true);
    inventory->AddEquipment(equipmentReal, slot, unk0);
    Server::getSingleton().SetSuppressed_SendEquipmentEquip(false);

    multiplayerLogger.WriteLine(Info, L"  Character inventory size POST: %i", inventory->equipmentList.size);

    // And send it to the other clients
    NetworkMessages::InventoryAddEquipment msgInventoryAddEquipment;
    msgInventoryAddEquipment.set_ownerid(owner->getCommonId());
    msgInventoryAddEquipment.set_equipmentid(equipment->getCommonId());
    msgInventoryAddEquipment.set_slot(slot);
    msgInventoryAddEquipment.set_unk0(unk0);

    Server::getSingleton().BroadcastMessage<NetworkMessages::InventoryAddEquipment>(clientAddress, S_PUSH_EQUIPMENT_EQUIP, &msgInventoryAddEquipment);

    //
    multiplayerLogger.WriteLine(Info, L"Server: Done adding equipment to inventory.");
    //log(L"Server: Done adding equipment to inventory.");
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find Character with ID = %x", ownerId);
    //log(L"Error: Could not find Character with ID = %x", ownerId);
  }

  //log(L"HandleInventoryAddEquipment Done.");
}

void Server::Helper_SendEquipmentToClient(const SystemAddress clientAddress, CEquipment *equipment, TLMP::NetworkEntity *netEquipment)
{
  multiplayerLogger.WriteLine(Info, L"Server: Pushing existing Equipment (%016I64X %i %s) out to client that just joined...",
    equipment->GUID, netEquipment->getCommonId(), equipment->nameReal.c_str());
  //log(L"Server: Pushing existing Equipment (%016I64X %i) out to client that just joined...",
  //  equipment->GUID, netEquipment->getCommonId(), equipment->nameReal.c_str());
  //log(L"        Equipment Type: %x", equipment->type__);

  // Create a new network message for all clients to create this character
  NetworkMessages::Equipment msgEquipment;
  Helper_PopulateEquipmentMessage(&msgEquipment, equipment, netEquipment);

  //log(L"  DEBUG: Equipment gemList: %p (Offset = %x)",
  //  &equipment->gemList, ((u32)this - (u32)&equipment->gemList));
  multiplayerLogger.WriteLine(Info, L"  Equipment contains %i Gems", equipment->gemList.size);
  //log(L"  Equipment contains %i Gems", equipment->gemList.size);

  // Add the Gem Equipment
  for (u32 i = 0; i < equipment->gemList.size; i++) {
    multiplayerLogger.WriteLine(Info, L"Server: Adding gem...");
    //log(L"Server: Adding gem...");

    NetworkMessages::Equipment *msgGem = msgEquipment.add_gems();
    CEquipment *gem = equipment->gemList[i];
    NetworkEntity *netGem = searchEquipmentByInternalObject(gem);
    
    // 
    if (netGem) {
      Helper_PopulateEquipmentMessage(msgGem, gem, netGem);
    } else {
      multiplayerLogger.WriteLine(Info, L"Could not find NetworkEntity for Gem, skipping equipment");
      //log(L"Could not find NetworkEntity for Gem, skipping equipment");
      continue;
    }
  }

  // This will broadcast to all clients except the one we received it from
  Server::getSingleton().SendMessage<NetworkMessages::Equipment>(clientAddress, S_PUSH_NEWEQUIPMENT, &msgEquipment);
}

void Server::Helper_SendGroundEquipmentToClient(const SystemAddress clientAddress, CEquipment *equipment, TLMP::NetworkEntity *netEquipment)
{
  multiplayerLogger.WriteLine(Info, L"Server: Pushing existing ground Equipment (%016I64X %i %s) out to client that just joined...",
    equipment->GUID, netEquipment->getCommonId(), equipment->nameReal.c_str());
  //log(L"Server: Pushing existing ground Equipment (%016I64X %i %s) out to client that just joined...",
  //  equipment->GUID, netEquipment->getCommonId(), equipment->nameReal.c_str());

  // Create equipment message
  //Helper_PopulateEquipmentMessage(&msgEquipmentDrop, equipment, netEquipment);

  // Create a new network message for all clients to create this character
  NetworkMessages::EquipmentDrop msgEquipmentDrop;
  NetworkMessages::Position *msgPosition = msgEquipmentDrop.add_position();
  msgPosition->set_x(equipment->position.x);
  msgPosition->set_y(equipment->position.y);
  msgPosition->set_z(equipment->position.z);
  msgEquipmentDrop.set_equipmentid(netEquipment->getCommonId());
  msgEquipmentDrop.set_unk0(1);

  // This will broadcast to all clients except the one we received it from
  Server::getSingleton().SendMessage<NetworkMessages::EquipmentDrop>(clientAddress, S_PUSH_EQUIPMENT_DROP, &msgEquipmentDrop);
}

//
// Populates the msgEquipment with the given Equipment vars
void Server::Helper_PopulateEquipmentMessage(NetworkMessages::Equipment* msgEquipment, CEquipment *equipment, NetworkEntity *netEquipment)
{
  multiplayerLogger.WriteLine(Info, L"  Setting up regular variables for Equipment: %s", equipment->nameReal.c_str());
  //log(L"  Setting up regular variables for Equipment: %s", equipment->nameReal.c_str());

  // Fill out the easy stuff
  msgEquipment->set_guid(equipment->GUID);
  msgEquipment->set_id(netEquipment->getCommonId());
  msgEquipment->set_stacksize(equipment->stackSize);
  msgEquipment->set_stacksizemax(equipment->stackSizeMax);
  msgEquipment->set_socketcount(equipment->socketCount);

  msgEquipment->set_identified(equipment->identified);

  msgEquipment->set_physical_damage_min(equipment->minimumPhysicalDamage);
  msgEquipment->set_physical_damage_max(equipment->maximumPhysicalDamage);

  const u32 MAP_PORTAL = 0xAD;
  const u32 OPENABLE = 0x28;
  const u32 INTERACTABLE = 0x20;
  const u32 GOLD = 0x22;
  const u32 BREAKABLE = 0x1D;

  // Skip if certain types
  if (equipment->type__ != BREAKABLE &&
      equipment->type__ != GOLD &&
      equipment->type__ != INTERACTABLE &&
      equipment->type__ != OPENABLE &&
      equipment->type__ != MAP_PORTAL)  
  {
    string nameUnidentified(equipment->nameUnidentified.begin(), equipment->nameUnidentified.end());
    nameUnidentified.assign(equipment->nameUnidentified.begin(), equipment->nameUnidentified.end());
    msgEquipment->set_name_unidentified(nameUnidentified);

    string namePrefix(equipment->namePrefix.begin(), equipment->namePrefix.end());
    namePrefix.assign(equipment->namePrefix.begin(), equipment->namePrefix.end());
    msgEquipment->set_name_prefix(namePrefix);
    
    string nameSuffix(equipment->nameSuffix.begin(), equipment->nameSuffix.end());
    nameSuffix.assign(equipment->nameSuffix.begin(), equipment->nameSuffix.end());
    msgEquipment->set_name_suffix(nameSuffix);

    msgEquipment->set_req_level(equipment->requirements[0]);
    msgEquipment->set_req_strength(equipment->requirements[1]);
    msgEquipment->set_req_dexterity(equipment->requirements[2]);
    msgEquipment->set_req_magic(equipment->requirements[3]);
    msgEquipment->set_req_defense(equipment->requirements[4]);

    multiplayerLogger.WriteLine(Info, L"  Setting up Regular Enchants... size = %x",
      equipment->enchantList.size());
    //log(L"  Setting up Regular Enchants... size = %x",
    //  equipment->enchantList.size());

    // New
    // Work on Regular Enchants
    for (size_t index = 0; index < equipment->enchantList.size(); index++) {
      //log(L"  Adding Equipment Enchant: REGULAR");
      //log(L"    (%i: %i)", equipment->enchantTypeList[index], equipment->enchantList[index]);
      multiplayerLogger.WriteLine(Info, L"  Adding Equipment Enchant: REGULAR");
      multiplayerLogger.WriteLine(Info, L"    (%i: %i)", equipment->enchantTypeList[index], equipment->enchantList[index]);

      NetworkMessages::EnchantType *msgEnchantType = msgEquipment->add_enchants();
      msgEnchantType->set_type(KEFFECT_TYPE_DAMAGE_BONUS);
      msgEnchantType->set_subtype(equipment->enchantTypeList[index]);
      msgEnchantType->set_value_min((float)equipment->enchantList[index]);
      msgEnchantType->set_value_max((float)equipment->enchantList[index]);
    }


    // Add the Effect Enchants
    CEffectManager *effectManager = equipment->pCEffectManager;
    if (effectManager) {
      multiplayerLogger.WriteLine(Info, L"  Setting up Effect Enchants...");
      //log(L"  Setting up Effect Enchants...");

      // Work on Effect Enchants
      for (u32 i = 0; i < effectManager->effectList.size; i++) {
        CEffect *effect = effectManager->effectList[i];

        multiplayerLogger.WriteLine(Info, L"  Adding Equipment Effect Enchant: (%x: %f / %f)", effect->effectType, effect->effectValueMin, effect->effectValueMax);
        //log(L"  Adding Equipment Effect Enchant: (%x: %f)", effect->effectType, effect->effectValue);

        NetworkMessages::EnchantType *msgEnchantType = msgEquipment->add_enchants();
        msgEnchantType->set_type(effect->effectType);
        msgEnchantType->set_subtype(-1);  // Not used in this instance
        msgEnchantType->set_value_min(effect->effectValueMin);
        msgEnchantType->set_value_max(effect->effectValueMax);
      }
    }
  }
}

void Server::HandleEquipmentDrop(const SystemAddress client, u32 equipmentId, Vector3 position, bool unk0)
{
  multiplayerLogger.WriteLine(Info, L"Server received Equipment Drop: (EquipmentId = %x, Position: %f, %f, %f, Unk0: %i",
    equipmentId, position.x, position.y, position.z, unk0);
  //log(L"Server received Equipment Drop: (EquipmentId = %x, Position: %f, %f, %f, Unk0: %i",
  //  equipmentId, position.x, position.y, position.z, unk0);

  NetworkEntity *netEquipment = searchEquipmentByCommonID(equipmentId);

  if (netEquipment) {
    bool permission = false;
    CEquipment *equipment = (CEquipment *)netEquipment->getInternalObject();
    CLevel *level = gameClient->pCLevel;

    // Check the Unequip for the client
    map<SystemAddress, NetworkEntity*>::iterator itr = (*Server_ClientUnequipMapping).find(client);
    if (itr != (*Server_ClientUnequipMapping).end()) {
      permission |= (CEquipment*)((*itr).second->getInternalObject()) == equipment;
    }

    // Check the client's permissable characters
    permission |= Helper_CheckEquipmentPermission(client, equipment);

    if (permission) {
      level->EquipmentDrop(equipment, position, unk0);
    } else {
      multiplayerLogger.WriteLine(Error, L"Client does not have permission to drop equipment: %p", equipment);
      log(L"Client does not have permission to drop equipment: %p", equipment);
    }
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find Equipment from CommonId: %x", equipmentId);
    //log(L"Error: Could not find Equipment from CommonId: %x", equipmentId);
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

  u32 identified = msgEquipment->identified();

  u32 minPhysicalDamage = msgEquipment->physical_damage_min();
  u32 maxPhysicalDamage = msgEquipment->physical_damage_max();

  wstring nameUnidentified = convertAsciiToWide(msgEquipment->name_unidentified());
  wstring namePrefix = convertAsciiToWide(msgEquipment->name_prefix());
  wstring nameSuffix = convertAsciiToWide(msgEquipment->name_suffix());

  u32 reqLevel = msgEquipment->req_level();
  u32 reqStrength = msgEquipment->req_strength();
  u32 reqDexterity = msgEquipment->req_dexterity();
  u32 reqMagic = msgEquipment->req_magic();
  u32 reqDefense = msgEquipment->req_defense();


  multiplayerLogger.WriteLine(Info, L"Server received equipment creation: (CommonID = %x) (GUID = %016I64X) (Stack: %i/%i) (SocketCount: %i)",
    id, guid, stacksize, stacksizemax, socketcount);
  //log(L"Server received equipment creation: (CommonID = %x) (GUID = %016I64X) (Stack: %i/%i) (SocketCount: %i)",
  //  id, guid, stacksize, stacksizemax, socketcount);

  /*
  // DEBUGGING
  log(L"  Test Equipment Received:");
  log(L"    min damage: %i", minPhysicalDamage);
  log(L"    max damage: %i", maxPhysicalDamage);
  log(L"    nameUn: %s", nameUnidentified.c_str());
  log(L"    namePr: %s", namePrefix.c_str());
  log(L"    nameSu: %s", nameSuffix.c_str());
  log(L"    req Level: %i", reqLevel);
  log(L"    req Str: %i", reqStrength);
  log(L"    req Dex: %i", reqDexterity);
  log(L"    req Magic: %i", reqMagic);
  log(L"    req Def: %i", reqDefense);
  */
  // --

  CResourceManager *resourceManager = (CResourceManager *)gameClient->pCPlayer->pCResourceManager;

  if (resourceManager) {
    // Suppress the server from sending this out, we do it afterwards only for the other clients
    Server::getSingleton().SetSuppressed_SendEquipmentCreation(true);
    CEquipment *equipment = resourceManager->CreateEquipment(guid, 1, 1, 0);
    Server::getSingleton().SetSuppressed_SendEquipmentCreation(false);

    equipment->socketCount = socketcount;
    equipment->stackSize = stacksize;
    equipment->stackSizeMax = stacksizemax;
    equipment->gemList.size = 0;

    equipment->identified = identified;

    equipment->minimumPhysicalDamage = minPhysicalDamage;
    equipment->maximumPhysicalDamage = maxPhysicalDamage;
    equipment->changeMeForDamageUpdate = maxPhysicalDamage;

    equipment->nameUnidentified = nameUnidentified;
    equipment->namePrefix = namePrefix;
    equipment->nameSuffix = nameSuffix;
    equipment->requirements[0] = reqLevel;
    equipment->requirements[1] = reqStrength;
    equipment->requirements[2] = reqDexterity;
    equipment->requirements[3] = reqMagic;
    equipment->requirements[4] = reqDefense;

    // Add to our BaseUnits list
    OutsideBaseUnits->push_back(equipment);

    multiplayerLogger.WriteLine(Info, L"Server: Adding gems to new equipment...");
    //log(L"Server: Adding gems to new equipment...");

    // Add the gems
    for (u32 i = 0; i < gemCount; i++) {
      NetworkMessages::Equipment msgGem = msgEquipment->gems().Get(i);
      CEquipment *gem = resourceManager->CreateEquipment(msgGem.guid(), 1, 1, 0);
      
      if (gem) {
        multiplayerLogger.WriteLine(Info, L"Server: Adding gem to shared network equipment: %i", msgGem.id());
        //log(L"Server: Adding gem to shared network equipment: %i", msgGem.id());
        NetworkEntity *newEntity = addEquipment(gem, msgGem.id());

        // Add to our BaseUnits list
        OutsideBaseUnits->push_back(gem);

        for (int j = 0; j < msgGem.enchants_size(); j++) {
          NetworkMessages::EnchantType msgGemEnchantType = msgGem.enchants().Get(j);
          gem->AddEnchant((EffectType)msgGemEnchantType.type(), (EnchantType)msgGemEnchantType.subtype(), msgGemEnchantType.value_min(), msgGemEnchantType.value_max());
        }

        equipment->gemList.push(gem);

        //
        // Same as below, broadcast and send this gem ID to our client and others
        //
        multiplayerLogger.WriteLine(Info, L"Server: Broadcasting new gem to all other clients.");
        //log(L"Server: Broadcasting new gem to all other clients.");

        // Send the Equipment w/ CommonID back to the clients
        // Simply change the slot ID on the message we received and send it back out to all other clients
        msgGem.set_id(newEntity->getCommonId());
        Server::getSingleton().BroadcastMessage<NetworkMessages::Equipment>(clientAddress, S_PUSH_NEWEQUIPMENT, &msgGem);

        multiplayerLogger.WriteLine(Info, L"Server: Sending gem common ID to owner Client of equipment.");
        //log(L"Server: Sending gem common ID to owner Client of equipment.");

        // Send a special equipment common ID message to the client that sent us their equipment
        NetworkMessages::EquipmentSetID msgEquipmentSetID;
        msgEquipmentSetID.set_guid(msgGem.guid());
        msgEquipmentSetID.set_client_id(msgGem.client_id());
        msgEquipmentSetID.set_id(newEntity->getCommonId());
        Server::getSingleton().SendMessage<NetworkMessages::Equipment>(clientAddress, S_REPLY_EQUIPMENT_ID, &msgEquipmentSetID);
      } else {
        multiplayerLogger.WriteLine(Error, L"Could not create gem of GUID: %016I64X", msgGem.guid());
        //log(L"Could not create gem of GUID: %016I64X", msgGem.guid());
      }
    }

    multiplayerLogger.WriteLine(Info, L"Server: Adding enchants to new equipment...: %i", enchantCount);
    //log(L"Server: Adding enchants to new equipment...: %i", enchantCount);

    // Add the enchants
    for (u32 i = 0; i < enchantCount; i++) {
      NetworkMessages::EnchantType msgEnchantType = msgEquipment->enchants().Get(i);

      multiplayerLogger.WriteLine(Info, L"  EnchantType: %x %x  Value: %f / %f",
        (EffectType)msgEnchantType.type(), (EnchantType)msgEnchantType.subtype(), msgEnchantType.value_min(), msgEnchantType.value_max());
      //log(L"  EnchantType: %x %x  Value: %f",
      //  (EffectType)msgEnchantType.type(), (EnchantType)msgEnchantType.subtype(), msgEnchantType.value());

      equipment->AddEnchant((EffectType)msgEnchantType.type(), (EnchantType)msgEnchantType.subtype(), msgEnchantType.value_min(), msgEnchantType.value_max());
    }

    multiplayerLogger.WriteLine(Info, L"Server: Adding equipment to shared network equipment");
    //log(L"Server: Adding equipment to shared network equipment");
    NetworkEntity *newEntity = addEquipment(equipment);

    multiplayerLogger.WriteLine(Info, L"Server: Broadcasting new equipment to all other clients.");
    //log(L"Server: Broadcasting new equipment to all other clients.");

    // Send the Equipment w/ CommonID back to the clients
    // Simply change the slot ID on the message we received and send it back out to all other clients
    msgEquipment->set_id(newEntity->getCommonId());
    Server::getSingleton().BroadcastMessage<NetworkMessages::Equipment>(clientAddress, S_PUSH_NEWEQUIPMENT, msgEquipment);

    multiplayerLogger.WriteLine(Info, L"Server: Sending equipment common ID to owner Client of equipment.");
    //log(L"Server: Sending equipment common ID to owner Client of equipment.");

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

  if (!netEquipment) {
    netEquipment = searchItemByCommonID(equipmentId);
  }

  if (netEquipment) {
    if (netCharacter) {
      CEquipment *equipment = (CEquipment *)netEquipment->getInternalObject();
      CCharacter *character = (CCharacter *)netCharacter->getInternalObject();

      character->PickupEquipment(equipment, gameClient->pCLevel);
    } else {
      multiplayerLogger.WriteLine(Error, L"Error: Could not find Character from CommonId: %x", characterId);
      //log(L"Error: Could not find Character from CommonId: %x", characterId);
    }
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find Equipment from CommonId: %x", equipmentId);
    //log(L"Error: Could not find Equipment from CommonId: %x", equipmentId);
  }
}

void Server::HandleInventoryRemoveEquipment(const SystemAddress clientAddress, u32 ownerId, u32 equipmentId)
{
  multiplayerLogger.WriteLine(Info, L"Server received inventory remove equipment: (CharacterID = %x, EquipmentID = %x)",
    ownerId, equipmentId);
  //log(L"Server received inventory remove equipment: (CharacterID = %x, EquipmentID = %x)",
  //  ownerId, equipmentId);

  NetworkEntity* owner = searchCharacterByCommonID(ownerId);
  NetworkEntity* equipment = searchEquipmentByCommonID(equipmentId);

  if (owner) {
    if (equipment) {
      CCharacter *characterOwner = (CCharacter*)owner->getInternalObject();
      CEquipment *equipmentReal = (CEquipment*)equipment->getInternalObject();

      // Check client permission with the equipment
      if (Helper_CheckEquipmentPermission(clientAddress, equipmentReal)) {
        CInventory *inventory = characterOwner->pCInventory;

        // Suppress this and send it manually
        Server::getSingleton().SetSuppressed_SendEquipmentUnequip(true);
        inventory->RemoveEquipment(equipmentReal);
        Server::getSingleton().SetSuppressed_SendEquipmentUnequip(false);

        (*Server_ClientUnequipMapping)[clientAddress] = searchEquipmentByInternalObject(equipmentReal);
        log(L"Server set client unequip mapping to %p", (*Server_ClientUnequipMapping)[clientAddress]);

        // And send to other clients
        // Create a Network Message for sending off to clients the equipment addition to the inventory
        NetworkMessages::InventoryRemoveEquipment msgInventoryRemoveEquipment;
        msgInventoryRemoveEquipment.set_ownerid(owner->getCommonId());
        msgInventoryRemoveEquipment.set_equipmentid(equipment->getCommonId());

        Server::getSingleton().BroadcastMessage<NetworkMessages::InventoryRemoveEquipment>(clientAddress, S_PUSH_EQUIPMENT_UNEQUIP, &msgInventoryRemoveEquipment);
      }
    } else {
      multiplayerLogger.WriteLine(Error, L"Error: Could not find Equipment with ID = %x", equipmentId);
      //log(L"Error: Could not find Equipment with ID = %x", equipmentId);
    }
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find Character with ID = %x", ownerId);
    //log(L"Error: Could not find Character with ID = %x", ownerId);
  }
}

void Server::HandleCharacterSetAction(const SystemAddress clientAddress, NetworkMessages::CharacterAction* msgCharacterAction)
{
  u32 id = msgCharacterAction->characterid();
  u32 action = msgCharacterAction->action();

  NetworkEntity *networkCharacter = searchCharacterByCommonID(id);

  if (networkCharacter) {
    CCharacter* character = (CCharacter*)networkCharacter->getInternalObject();
    character->SetAction(action);
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find character with common id = %x", id);
    //log(L"Error: Could not find character with common id = %x", id);
  }
}

void Server::HandleCharacterAttack(NetworkMessages::CharacterAttack* msgCharacterAttack)
{
  u32 id = msgCharacterAttack->characterid();

  NetworkEntity *networkCharacter = searchCharacterByCommonID(id);

  if (networkCharacter) {
    CCharacter* character = (CCharacter*)networkCharacter->getInternalObject();

    SetSuppressed_SendCharacterAttack(true);
    character->Attack();
    SetSuppressed_SendCharacterAttack(false);
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find character with common id = %x", id);
    //log(L"Error: Could not find character with common id = %x", id);
  }
}

void Server::HandleCharacterUseSkill(NetworkMessages::CharacterUseSkill* msgCharacterUseSkill)
{
  u32 id = msgCharacterUseSkill->characterid();
  u64 skill = msgCharacterUseSkill->skill();
  Vector3 direction;

  NetworkMessages::Position msgPosition = msgCharacterUseSkill->direction();
  direction.x = msgPosition.x();
  direction.y = msgPosition.y();
  direction.z = msgPosition.z();

  NetworkEntity *networkCharacter = searchCharacterByCommonID(id);
  if (networkCharacter) {
    CCharacter* character = (CCharacter*)networkCharacter->getInternalObject();
  
    // Set the character orientation
    //character->SetOrientation(direction);

    // Have the character use the skill

    // Testing - SkillManager will show the correct graphic on the
    // client character, but the Server Player will not cast the skill
    //character->pCSkillManager = gameClient->pCPlayer->pCSkillManager;
    character->UseSkill(skill);

    // Test, have the player do it too and see if the effect shows
    //gameClient->pCPlayer->UseSkill(skill);
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find character with common id = %x", id);
    //log(L"Error: Could not find character with common id = %x", id);
  }
}

void Server::RemoveDestroyedEquipmentFromNetwork()
{
  vector<NetworkEntity*>::iterator itr;

  // Search for destroyed equipment
  bool clean = false;
  while (!clean) {
    clean = true;

    for (itr = NetworkSharedEquipment->begin(); itr != NetworkSharedEquipment->end(); itr++) {
      CEquipment *equipment = (CEquipment*)(*itr)->getInternalObject();

      if (equipment->destroy) {
        NetworkSharedEquipment->erase(itr);
        clean = false;
        break;
      }
    }
  }

}

void Server::HandleEquipmentUpdateStack(const SystemAddress clientAddress, NetworkMessages::EquipmentUpdateStackSize *msgEquipmentUpdateStackSize)
{
  u32 id = msgEquipmentUpdateStackSize->id();
  u32 amount = msgEquipmentUpdateStackSize->change_amount();

  NetworkEntity *netEquipment = searchEquipmentByCommonID(id);

  if (netEquipment) {
    CEquipment *equipment = (CEquipment*)netEquipment->getInternalObject();

    // Suppress the server from sending it
    SetSuppressed_SendEquipmentStack(true);
    EquipmentAddStackCount(equipment, amount);
    SetSuppressed_SendEquipmentStack(false);

    // Manually send it to the other clients
    Server::getSingleton().BroadcastMessage<NetworkMessages::EquipmentUpdateStackSize>(clientAddress, S_PUSH_EQUIPMENT_STACKUPDATE, msgEquipmentUpdateStackSize);

  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find equipment with common id = %x", id);
    //log(L"Error: Could not find equipment with common id = %x", id);
  }
}

void Server::HandleEquipmentAddGem(const SystemAddress address, NetworkMessages::EquipmentAddGem* msgEquipmentAddGem)
{
  u32 equipmentId = msgEquipmentAddGem->equipmentid();
  u32 gemId = msgEquipmentAddGem->gemid();

  NetworkEntity *netEquipment = searchEquipmentByCommonID(equipmentId);
  NetworkEntity *netGem = searchEquipmentByCommonID(gemId);

  if (netEquipment && netGem) {
    CEquipment *equipment = (CEquipment*)netEquipment->getInternalObject();
    CEquipment *gem = (CEquipment*)netGem->getInternalObject();

    equipment->AddGem(gem);
  } else {
    //log(L"Equipment or Gem commonId not found: %x %x  ptrs: %p %p",
    //  equipmentId, gemId, netEquipment, netGem);
    multiplayerLogger.WriteLine(Error, L"Equipment or Gem commonId not found: %x %x  ptrs: %p %p",
      equipmentId, gemId, netEquipment, netGem);
  }
}

void Server::HandleEquipmentRemoveGems(NetworkMessages::EquipmentRemoveGems *msgEquipmentRemoveGems)
{
  u32 equipmentId = msgEquipmentRemoveGems->equipmentid();

  NetworkEntity *netEntity = searchEquipmentByCommonID(equipmentId);

  if (netEntity) {
    CEquipment* equipment = (CEquipment*)netEntity->getInternalObject();
    equipment->gemList.size = 0;

    // Broadcast it back out
    Server::getSingleton().BroadcastMessage<NetworkMessages::EquipmentRemoveGems>(S_PUSH_EQUIPMENT_REMOVE_GEMS, msgEquipmentRemoveGems);
  } else {
    //log(L"Error: Could not find NetworkEntity for equipment of id: %x", equipmentId);
    multiplayerLogger.WriteLine(Error, L"Error: Could not find NetworkEntity for equipment of id: %x", equipmentId);
  }
}

void Server::HandleChatMessage(NetworkMessages::ChatPlayerText *msgChatPlayerText)
{
  //log(L"Server: Received player chat text.");

  CEGUI::MultiLineEditbox *pWindow = (CEGUI::MultiLineEditbox *)getChatHistoryWindow();
  size_t selectionEnd;

  if (pWindow) {
    CEGUI::String message = msgChatPlayerText->text().c_str();
    u32 characterId = msgChatPlayerText->characterid();

    NetworkEntity *netCharacter = searchCharacterByCommonID(characterId);

    if (netCharacter) {
      CCharacter *character = (CCharacter*)netCharacter->getInternalObject();
      string characterName(character->characterName.begin(), character->characterName.end());
      characterName.assign(character->characterName.begin(), character->characterName.end());

      //selectionStart = pWindow->getText().length();
      pWindow->appendText(CEGUI::String(characterName) + CEGUI::String(": ") + message + CEGUI::String("\n"));
      selectionEnd = pWindow->getText().length();
      pWindow->setCaratIndex(selectionEnd);
      
      // Send it back out to all clients
      Server::getSingleton().BroadcastMessage<NetworkMessages::ChatPlayerText>(S_PUSH_CHAT_PLAYER, msgChatPlayerText);
    } else {
      multiplayerLogger.WriteLine(Error, L"Error could not find Network entity of id: %x", characterId);
    }
  }
}

void Server::OnClientConnected(void *args)
{
  
}

void Server::OnClientDisconnected(void *args)
{
  SystemAddress address = *(SystemAddress*)args;

  Helper_RemoveClient(address);
}

void Server::Helper_RemoveClient(const SystemAddress clientAddress)
{
  map<SystemAddress, vector<NetworkEntity*>*>::iterator itr;

  // Search for an existing address
  for (itr = Server_ClientCharacterMapping->begin(); itr != Server_ClientCharacterMapping->end(); ++itr) {
    if ((*itr).first == clientAddress) {
      vector<NetworkEntity*>::iterator itr2;
      for (itr2 = (*itr).second->begin(); itr2 != (*itr).second->end(); itr2++) {
        if (*itr2) {
          CCharacter *character = (CCharacter*)(*itr2)->getInternalObject();
          if (character) {
            character->Destroy();
          }
        }
      }
      break;
    }
  }
}

void Server::HandleAddClientCharacter(const SystemAddress address, CCharacter* character)
{
  map<SystemAddress, vector<NetworkEntity*>*>::iterator itr;

  // Search for an existing address
  bool found = false;
  for (itr = Server_ClientCharacterMapping->begin(); itr != Server_ClientCharacterMapping->end(); ++itr) {
    if ((*itr).first == address) {
      found = true;
      break;
    }
  }

  // Add to our BaseUnits list
  OutsideBaseUnits->push_back(character);

  // Add the new key
  if (!found) {
    (*Server_ClientCharacterMapping)[address] = new vector<NetworkEntity*>();
  }

  // Find the NetworkEntity for the character
  NetworkEntity *netCharacter = searchCharacterByInternalObject(character);

  // We now have a vector at the key, add the character
  if (netCharacter) {
    (*Server_ClientCharacterMapping)[address]->push_back(netCharacter);
  } else {
    //log(L"Error: Could not add character to Server_ClientCharacterMapping - Could not find network entity.");
    multiplayerLogger.WriteLine(Error, L"Error: Could not add character to Server_ClientCharacterMapping - Could not find network entity.");
  }
}

void Server::HandleBreakableTriggered(NetworkMessages::BreakableTriggered *msgBreakableTriggered)
{
  u32 itemId = msgBreakableTriggered->itemid();
  u32 characterId = msgBreakableTriggered->characterid();

  NetworkEntity *entity = searchItemByCommonID(itemId);
  NetworkEntity *netCharacter = searchCharacterByCommonID(characterId);

  if (entity) {
    CBreakable *breakable = (CBreakable*)entity->getInternalObject();
    CPlayer *character = NULL;
    
    if (netCharacter) {
      character = (CPlayer*)netCharacter->getInternalObject();
    }

    if (breakable) {
      breakable->Break(character);
    }
  } else {
    //log(L"Server: Error could not find entity with common ID = %x OR character with common ID = %x",
    //  itemId, characterId);
  }
}

void Server::HandleTriggerUnitTriggered(NetworkMessages::TriggerUnitTriggered *msgTriggerUnitTriggered)
{
  u32 itemId = msgTriggerUnitTriggered->itemid();
  u32 characterId = msgTriggerUnitTriggered->characterid();

  NetworkMessages::Position msgPosition = msgTriggerUnitTriggered->position();
  Vector3 position;
  position.x = msgPosition.x();
  position.y = msgPosition.y();
  position.z = msgPosition.z();

  NetworkEntity *entity = searchItemByCommonID(itemId);
  NetworkEntity *netCharacter = searchCharacterByCommonID(characterId);

  /* Testing - Don't search by ID, but instead by position
  if (entity) {
    CTriggerUnit *triggerUnit = (CTriggerUnit*)entity->getInternalObject();
    CPlayer *character = NULL;
    
    if (netCharacter) {
      character = (CPlayer*)netCharacter->getInternalObject();
    }

    if (triggerUnit) {
      triggerUnit->Trigger(character);
    }
  } else {
    //log(L"Server: Error could not find entity with common ID = %x OR character with common ID = %x",
    //  itemId, characterId);
  }
  */

  // Assume the server's trigger units are sync'd with ours
  CLevel *level = gameClient->pCLevel;
  level->DumpTriggerUnits();

  // List our trigger Units
  LinkedListNode* itr = *level->ppCTriggerUnits;
  while (itr != NULL) {
    CTriggerUnit* triggerUnit = (CTriggerUnit*)itr->pCBaseUnit;
    CPlayer *character = NULL;
    
    if (netCharacter) {
      character = (CPlayer*)netCharacter->getInternalObject();
    }

    if (triggerUnit->position.squaredDistance(position) < 0.1f) {
      triggerUnit->Trigger(character);
      break;
    }
    
    itr = itr->pNext;
  }
}

void Server::HandleCharacterSetTarget(NetworkMessages::CharacterSetTarget *msgCharacterSetTarget)
{
  u32 characterId = msgCharacterSetTarget->characterid();
  u32 targetId = msgCharacterSetTarget->targetid();

  NetworkEntity *netCharacter = searchCharacterByCommonID(characterId);
  CCharacter *target = NULL;

  if (targetId != -1) {
    NetworkEntity *netTarget = searchCharacterByCommonID(targetId);
    if (netTarget) {
      target = (CCharacter*)netTarget->getInternalObject();
    } else {
      //log(L"Error: Could not find Target character of ID: %x", targetId);
      return;
    }
  }

  if (netCharacter) {
    CCharacter *character = (CCharacter*)netCharacter->getInternalObject();

    if (character) {
      character->SetTarget(target);
    }
  } else {
    //log(L"Error: Could not find Character of ID: %x", characterId);
  }
}

void Server::HandleEquipmentIdentify(NetworkMessages::EquipmentIdentify *msgEquipmentIdentify)
{
  u32 equipmentId = msgEquipmentIdentify->equipmentid();

  NetworkEntity *entity = searchEquipmentByCommonID(equipmentId);

  if (entity) {
    CEquipment* equipment = (CEquipment*)entity->getInternalObject();

    if (equipment) {
      equipment->identified = 1;
      equipment->UpdateTooltip();

      Server::getSingleton().BroadcastMessage<NetworkMessages::EquipmentIdentify>(S_PUSH_EQUIPMENT_IDENTIFY, msgEquipmentIdentify);
    }
  } else {
    //log(L"Error: Could not find equipment of ID: %x", equipmentId);
  }
}

void Server::Helper_SendCurrentLevel(const SystemAddress clientAddress)
{
  NetworkMessages::CurrentLevel msgCurrentLevel;

  string sDungeonSection(gameClient->pCDungeon->name0.begin(), gameClient->pCDungeon->name0.end());
  sDungeonSection.assign(gameClient->pCDungeon->name0.begin(), gameClient->pCDungeon->name0.end());

  msgCurrentLevel.set_dungeonsection(sDungeonSection);
  msgCurrentLevel.set_relativelevel(gameClient->level);
  msgCurrentLevel.set_absolutelevel(gameClient->levelAbsolute);

  SendMessage<NetworkMessages::CurrentLevel>(clientAddress, S_PUSH_CURRENT_LEVEL, &msgCurrentLevel);
}

void Server::Helper_SendTriggerUnitSync(const SystemAddress clientAddress)
{
  log(L"Server: Sending Trigger Units...: %p", gameClient->pCLevel);
  
  u32 offset = (u32)&gameClient->pCLevel->ppCTriggerUnits - (u32)gameClient->pCLevel;
  log(L"  offset of triggerUnits = %x", offset);

  NetworkMessages::TriggerUnitSync msgTriggerUnitSync;

  CLevel *level = gameClient->pCLevel;
  LinkedListNode* itr = *level->ppCTriggerUnits;
  while (itr != NULL) {
    CTriggerUnit* triggerUnit = (CTriggerUnit*)itr->pCBaseUnit;
  
    NetworkEntity *entity = searchItemByInternalObject(triggerUnit);

    if (entity) {
      logColor(B_RED, L"Server: %s (ID: %x)  (%f, %f, %f)",
        triggerUnit->nameReal.c_str(), entity->getCommonId(),
        triggerUnit->position.x, triggerUnit->position.y, triggerUnit->position.z);

      NetworkMessages::TriggerUnit *msgTriggerUnit = msgTriggerUnitSync.add_triggerunits();
      NetworkMessages::Position *msgPosition = msgTriggerUnit->mutable_triggerposition();

      msgPosition->set_x(triggerUnit->position.x);
      msgPosition->set_y(triggerUnit->position.y);
      msgPosition->set_z(triggerUnit->position.z);
      msgTriggerUnit->set_triggerid(entity->getCommonId());
    } else {
      //logColor(B_RED, L"Server Error: Could not find Item ID for TriggerUnit: %s   Sync", triggerUnit->nameReal.c_str());
    }

    itr = itr->pNext;
  }
 
  Server::getSingleton().SendMessage<NetworkMessages::TriggerUnitSync>(clientAddress, S_PUSH_TRIGGERUNIT_SYNC, &msgTriggerUnitSync);

  logColor(B_RED, "Server done sending trigger units...");
}

bool Server::Helper_CheckEquipmentPermission(const SystemAddress client, CEquipment *equipment)
{
  // Determine if the client has permission to drop this item, e.g. this item should be on their person or minion
  if (Server_ClientCharacterMapping) {
    vector<NetworkEntity*>::iterator itr;

    log(L"Helper_CheckEquipmentPermission: Client owns following characters:");
    for (itr = (*Server_ClientCharacterMapping)[client]->begin(); itr != (*Server_ClientCharacterMapping)[client]->end(); ++itr) {
      CCharacter* clientCharacter = (CCharacter*)(*itr)->getInternalObject();
      CInventory* inv = clientCharacter->pCInventory;

      log(L"  %p (%s) (Inv size: %i)", clientCharacter, clientCharacter->characterName.c_str(), inv->equipmentList.size);

      // Traverse the character inventory equipmentRef and search for the equipment
      for (u32 i = 0; i < inv->equipmentList.size; ++i) {
        if (equipment == inv->equipmentList[i]->pCEquipment) {
          return true;
        }
      }
    }
  }

  return false;
}

bool Server::Helper_CheckCharacterPermission(const SystemAddress client, CCharacter *character)
{
  // Determine if the client has permission to drop this item, e.g. this item should be on their person or minion
  if (Server_ClientCharacterMapping) {
    vector<NetworkEntity*>::iterator itr;

    log(L"Helper_CheckEquipmentPermission: Client owns following characters:");
    for (itr = (*Server_ClientCharacterMapping)[client]->begin(); itr != (*Server_ClientCharacterMapping)[client]->end(); ++itr) {
      CCharacter* clientCharacter = (CCharacter*)(*itr)->getInternalObject();
      
      if (clientCharacter == character) {
        return true;
      }
    }
  }

  return false;
}

void Server::HandleCharacterResurrect(NetworkMessages::CharacterResurrect *msgCharacterResurrect)
{
  //log(L"Client received character resurrection.");

  NetworkEntity *entity = searchCharacterByCommonID(msgCharacterResurrect->characterid());

  if (entity) {
    CCharacter *character = (CCharacter*)entity->getInternalObject();

    character->Resurrect();
  }
}

void Server::HandleCharacterOrientation(NetworkMessages::CharacterOrientation *msgCharacterOrientation)
{
  u32 characterId = msgCharacterOrientation->characterid();
  NetworkEntity *entity = searchCharacterByCommonID(characterId);
  NetworkMessages::Position msgOrientation = msgCharacterOrientation->orientation();

  if (!entity) {
    log("Error: Could not find character for network ID: %x", characterId);
    return;
  }

  CCharacter *character = (CCharacter*)entity->getInternalObject();
  character->UpdateOrientation(msgOrientation.x(), msgOrientation.z());
}

void Server::HandlePlayerWeaponSwap(NetworkMessages::PlayerSwapWeapons *msgPlayerSwapWeapons)
{
  u32 characterId = msgPlayerSwapWeapons->characterid();
  NetworkEntity *entity = searchCharacterByCommonID(characterId);

  if (!entity) {
    log("Error: Could not find character for network ID: %x", characterId);
    return;
  }

  CCharacter *character = (CCharacter*)entity->getInternalObject();
  character->WeaponSwap();
}

void Server::HandleCharacterSetSkillPoints(NetworkMessages::CharacterSetSkillPoints *msgCharacterSetSkillPoints)
{
  u32 characterId = msgCharacterSetSkillPoints->characterid();
  u64 skillGUID = msgCharacterSetSkillPoints->skillguid();
  u32 skillLevel = msgCharacterSetSkillPoints->skilllevel();

  NetworkEntity *entity = searchCharacterByCommonID(characterId);
  if (!entity) {
    log("Error: Could not find character for network ID: %x", characterId);
    return;
  }

  CCharacter *character = (CCharacter*)entity->getInternalObject();
  CSkillManager *skillManager = character->pCSkillManager;
  if (!skillManager) {
    log("Error: SkillManager for character is null");
    return;
  }

  CSkill *skill = skillManager->getSkillFromGUID(skillGUID);
  if (!skill) {
    log("Error: Skill with GUID: %016I64X could not be found", skillGUID);
    return;
  }

  character->pCSkillManager->setSkillLevel(skill, skillLevel);
}
