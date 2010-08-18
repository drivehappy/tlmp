#include "Multiplayer.h"
using namespace TLMP;


TLMP::Logger TLMP::multiplayerLogger("TLAPI/MultiplayerLog.txt");
TLAPI::CGameClient *TLMP::gameClient = NULL;

void TLMP::SetupNetwork()
{
  multiplayerLogger.SetLoggingLevel(Info);
  multiplayerLogger.WriteLine(Info, L"Torchlight Multiplayer Log");
  multiplayerLogger.WriteLine(Info, L"Registering Events...");

  //
  // Register event callbacks we need
  CCharacterSaveState::RegisterEvent_CharacterSaveState_LoadFromFile(NULL, CharacterSaveState_ReadFromFile);

  CGame::RegisterEvent_GameCtor(Game_Ctor, NULL);
  CGame::RegisterEvent_Game_CreateUI(NULL, GameClient_CreateUI);

  CResourceManager::RegisterEvent_ResourceManagerCreateMonster(CreateMonster, NULL);
  CResourceManager::RegisterEvent_ResourceManagerCreateEquipment(CreateEquipmentPre, CreateEquipmentPost);

  CLevel::RegisterEvent_LevelCharacterInitialize(Level_CharacterInitialize, NULL);
  CLevel::RegisterEvent_LevelDropEquipment(Level_DropEquipmentPre, Level_DropEquipmentPost);

  CGameClient::RegisterEvent_GameClientCtor(NULL, GameClient_Ctor);
  CGameClient::RegisterEvent_GameClientProcessObjects(NULL, GameClient_ProcessObjects);
  CGameClient::RegisterEvent_GameClientProcessTitleScreen(NULL, GameClient_TitleProcessObjects);
  CGameClient::RegisterEvent_GameClient_CreateLevel(GameClient_CreateLevelPre, NULL);
  CGameClient::RegisterEvent_GameClient_LoadLevel(GameClient_LoadLevelPre, NULL);
  CGameClient::RegisterEvent_GameClientLoadMap(GameClient_LoadMapPre, NULL);

  CMainMenu::RegisterEvent_MainMenu_Event(MainMenuEventPre, NULL);

  CEquipment::RegisterEvent_EquipmentCtor(Equipment_Ctor, NULL);
  CEquipment::RegisterEvent_EquipmentInitialize(EquipmentInitialize, NULL);

  CMonster::RegisterEvent_MonsterProcessAI2(Monster_ProcessAI, NULL);
  CMonster::RegisterEvent_MonsterIdle(Monster_Idle, NULL);

  CCharacter::RegisterEvent_CharacterCtor(Character_Ctor, NULL);
  CCharacter::RegisterEvent_CharacterSetAlignment(Character_SetAlignment, NULL);
  CCharacter::RegisterEvent_CharacterSetDestination(Character_SetDestination, NULL);
  CCharacter::RegisterEvent_CharacterPickupEquipment(Character_PickupEquipmentPre, Character_PickupEquipmentPost);

  CInventory::RegisterEvent_InventoryAddEquipment(Inventory_AddEquipment, NULL);
  CInventory::RegisterEvent_InventoryRemoveEquipment(Inventory_RemoveEquipmentPre, NULL);
  // --

  multiplayerLogger.WriteLine(Info, L"Registering Events... Done.");

  Server::getSingleton().SetCallback_OnClientConnect(&ServerOnClientConnected);

  Hook(GetProcAddress(GetModuleHandle("OgreMain.dll"), "?isActive@RenderWindow@Ogre@@UBE_NXZ"), OgreIsActive, 0, HOOK_THISCALL, 0);
}


void TLMP::CharacterSaveState_ReadFromFile(CCharacterSaveState* saveState, PVOID file, u32 unk)
{
  /*
  // Dump character save state
  multiplayerLogger.WriteLine(Info, L"CharacterSaveState_ReadFromFile: %s (GUID: %016I64X)",
    saveState->name.c_str(), saveState->guid2);

  multiplayerLogger.WriteLine(Info, L"  Strength: %i", saveState->strength);
  multiplayerLogger.WriteLine(Info, L"  Dexterity: %i", saveState->dexterity);
  multiplayerLogger.WriteLine(Info, L"  Defense: %i", saveState->defense);
  multiplayerLogger.WriteLine(Info, L"  Magic: %i", saveState->magic);
  multiplayerLogger.WriteLine(Info, L"  Gold: %i", saveState->gold);

  multiplayerLogger.WriteLine(Info, L"  Spell 1: %s", saveState->nameSpell1.c_str());
  multiplayerLogger.WriteLine(Info, L"  Spell 2: %s", saveState->nameSpell2.c_str());
  multiplayerLogger.WriteLine(Info, L"  Spell 3: %s", saveState->nameSpell3.c_str());
  multiplayerLogger.WriteLine(Info, L"  Spell 4: %s", saveState->nameSpell4.c_str());

  // Dump character's item save states
  CItemSaveState **itr = saveState->itemSaveStateBegin;
  CItemSaveState **itrEnd = saveState->itemSaveStateEnd1;

  while (itr != itrEnd) {
    multiplayerLogger.WriteLine(Info, L"    Item: %s (GUID: %016I64X) Slot: %i",
      (*itr)->name.c_str(), (*itr)->guid2, (*itr)->slot);

    multiplayerLogger.WriteLine(Info, L"      Prefix: %s",
      (*itr)->name2.c_str());
    multiplayerLogger.WriteLine(Info, L"      Suffix: %s",
      (*itr)->name3.c_str());

    for (int i = 0; i < 6; i++) {
      multiplayerLogger.WriteLine(Info, L"      Unk1[%i]: %x",
        i, (*itr)->unk1[i]);
    }

    for (int i = 0; i < 30; i++) {
      multiplayerLogger.WriteLine(Info, L"      Unk2[%i]: %x",
        i, (*itr)->unk2[i]);
    }

    multiplayerLogger.WriteLine(Info, L"");

    itr++;
  }
  */
}

void TLMP::Equipment_Ctor(CEquipment* equipment)
{
  log(L"Equipment::Ctor = %p", equipment);
}

void TLMP::Character_Ctor(CCharacter* character)
{
  log(L"Character::Ctor = %p", character);
}

void TLMP::Character_SetAlignment(CCharacter* character, u32 alignment)
{
  multiplayerLogger.WriteLine(Info, L"Setting Character (%s) Alignment = %x",
    character->characterName.c_str(), alignment);
  log(L"Setting Character (%s) Alignment = %x",
    character->characterName.c_str(), alignment);
}

void TLMP::Game_Ctor(CGame* game)
{
  log(L"Game::Ctor = %p", game);
}

void TLMP::GameClient_Ctor(CGameClient* client)
{
  log(L"GameClient::Ctor = %p", client);

  gameClient = client;
}

void TLMP::CreateMonster(CMonster* character, CResourceManager* resourceManager, u64 guid, u32 level, bool noItems, bool & calloriginal)
{
  if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
    multiplayerLogger.WriteLine(Info, L"Creating character: %016I64X %i", guid, noItems);
    log(L"Creating character: %016I64X %i", guid, noItems);

    if (Client::getSingleton().GetSuppressed_CharacterCreation()) {
      //calloriginal = false;
      //character = NULL;
    }
  }
  else if (Network::NetworkState::getSingleton().GetState() == SERVER) {
    multiplayerLogger.WriteLine(Info, L"Created character with guid of: %016I64X (%p)",
      guid, character);
  }


  if (calloriginal) {
    multiplayerLogger.WriteLine(Info, L"Creating character: %016I64X", guid);
    log(L"Creating character: %016I64X", guid);
  }
}

void TLMP::CreateEquipmentPre(CEquipment* equipment, CResourceManager* resourceManager, u64 guid, u32 level, u32 unk0, u32 unk1, bool & calloriginal)
{
  if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
    if (Client::getSingleton().GetSuppressed_EquipmentCreation()) {
      //calloriginal = false;
      //equipment = NULL; 
    }
  }
  else if (Network::NetworkState::getSingleton().GetState() == SERVER) {
    //
  }


  if (calloriginal) {
    multiplayerLogger.WriteLine(Info, L"Creating equipment with guid of: %016I64X Level: %i (%p) %x %x",
      guid, level, equipment, unk0, unk1);

    log(L"Creating equipment with guid of: %016I64X Level: %i (%p) %x %x",
      guid, level, equipment, unk0, unk1);
  } else {
    multiplayerLogger.WriteLine(Info, L"Suppressing equipment creation with guid of: %016I64X Level: %i (%p) %x %x",
      guid, level, equipment, unk0, unk1);
    log(L"Suppressing equipment creation with guid of: %016I64X Level: %i (%p) %x %x",
      guid, level, equipment, unk0, unk1);
  }
}

void TLMP::CreateEquipmentPost(CEquipment* equipment, CResourceManager* resourceManager, u64 guid, u32 level, u32 unk0, u32 unk1, bool & calloriginal)
{
  if (equipment) {
    log(L"Created equipment with guid of: %016I64X Level: %i (%p, %s)",
      guid, level, equipment, equipment->nameReal.c_str());

    multiplayerLogger.WriteLine(Info, L"Created equipment with guid of: %016I64X Level: %i (%p, %s)",
      guid, level, equipment, equipment->nameReal.c_str());

    // --
    if (Network::NetworkState::getSingleton().GetState() == SERVER) {
      NetworkEntity *newEntity = addEquipment(equipment);

      NetworkMessages::Equipment msgEquipment;
      msgEquipment.set_id(newEntity->getCommonId());
      msgEquipment.set_guid(equipment->GUID);

      // TODO Add Enchants and rest of data

      Server::getSingleton().BroadcastMessage<NetworkMessages::Equipment>(S_PUSH_NEWEQUIPMENT, &msgEquipment);
    }
  }
}

void TLMP::EquipmentInitialize(CEquipment* equipment, CItemSaveState* itemSaveState, bool & calloriginal)
{
  log(L"Equipment initializing: %p", equipment);

  log(L"Equipment Initialized (GUID: %016I64X  %s)",
    equipment->GUID, equipment->nameReal.c_str());

  log(L"  ItemSaveState: %s %s %s",
    itemSaveState->name.c_str(), itemSaveState->name2.c_str(), itemSaveState->name3.c_str());

  multiplayerLogger.WriteLine(Info, L"Equipment Initialized (GUID: %016I64X  %s)",
    equipment->GUID, equipment->nameReal.c_str());

  if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
    if (Client::getSingleton().GetSuppressed_EquipmentCreation()) {
      //calloriginal = false;
    }
  }

  if (calloriginal) {
  } else {
    multiplayerLogger.WriteLine(Info, L"Suppressing Equipment Initialize (GUID: %016I64X  %s)",
      equipment->GUID, equipment->nameReal.c_str());
  }
}

void TLMP::Level_CharacterInitialize(CCharacter* retval, CLevel* level, CCharacter* character, Vector3* position, u32 unk0, bool & calloriginal)
{
  const u32 CPLAYER_BASE = 0xA80064;
  const u32 CMONSTER_BASE = 0xA7F97C;

  const u64 CAT = 0x5C5BBC74483A11DE;
  const u64 DOG = 0xD3A8F9832FA111DE;
  const u64 DESTROYER = 0xD3A8F9982FA111DE;
  const u64 ALCHEMIST = 0x8D3EE5363F7611DE;
  const u64 VANQUISHER = 0xAA472CC2629611DE;
  const u64 BRINK = 0xBC1E373A723411DE;

  if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
    multiplayerLogger.WriteLine(Info, L"Client: Level::CharacterInitialize: %s (%f %f %f) unk0: %x",
      character->characterName.c_str(), position->x, position->y, position->z, unk0);
    log(L"Client: Level::CharacterInitialize: %s (%f %f %f) unk0: %x",
      character->characterName.c_str(), position->x, position->y, position->z, unk0);

    // True, attempt to suppress the character initialization
    if (Client::getSingleton().GetSuppressed_CharacterCreation()) {
      u64 guid = character->GUID;

      if (guid == DESTROYER || guid == VANQUISHER || guid == ALCHEMIST ||
        guid == DOG || guid == CAT || guid == BRINK)
      {
        log("Client: Detected CPlayer addition to CLevel, not suppressing load");
      } else {
        log(L"Client: Suppressing Monster load into level: %016I64X %s", character->GUID, character->characterName.c_str());
        calloriginal = false;
        retval = NULL;
      }
    }
  }
  else if (Network::NetworkState::getSingleton().GetState() == SERVER) {
    if (gameClient->inGame) {
      multiplayerLogger.WriteLine(Info, L"Level Character Initialized: %p %s (%f %f %f) unk0: %x",
        character, character->characterName.c_str(), position->x, position->y, position->z, unk0);
      log(L"Level Character Initialized: %p %s (%f %f %f) unk0: %x",
        character, character->characterName.c_str(), position->x, position->y, position->z, unk0);

      // Create a network ID to identify this character later
      NetworkEntity *newEntity = addCharacter(character);

      if (!newEntity) {
        log(L"NetworkEntity is NULL");
        return;
      }

      // Send this character to be created on the clients if it isn't suppressed
      if (!Server::getSingleton().GetSuppressed_SendCharacterCreation()) {
        multiplayerLogger.WriteLine(Info, L"Server: Pushing Initialized Character out to clients...");
        log(L"Server: Pushing Initialized Character out to clients...");

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

        msgNewCharacter.set_id(newEntity->getCommonId());

        // This will broadcast to all clients except the one we received it from
        Server::getSingleton().BroadcastMessage<NetworkMessages::Character>(S_PUSH_NEWCHAR, &msgNewCharacter);
      }
    }
  }
  
}

void TLMP::GameClient_ProcessObjects(CGameClient *client, float dTime, PVOID unk1, PVOID unk2)
{
  // Set the started flag - don't bother checking, it's just as fast to set it
  switch (Network::NetworkState::getSingleton().GetState()) {
    case SERVER:
      Server::getSingleton().SetGameStarted(true);

      Server::getSingleton().ReceiveMessages();
      break;

    case CLIENT:
      Client::getSingleton().SetGameStarted(true);

      Client::getSingleton().ReceiveMessages();
      break;
  }
}

void TLMP::GameClient_TitleProcessObjects(CGameClient *client, float dTime, PVOID unk1, PVOID unk2)
{
  //log("MainMenuShown: %i", MainMenuShown());

  // Determine if we've exited an existing game in network mode
  switch (Network::NetworkState::getSingleton().GetState()) {
    case SERVER:
      // Shutdown the Server if this is the first iteration out of the game
      //if (Server::getSingleton().GetGameStarted() && Server::getSingleton().HasGameStarted()) {
      //  Server::getSingleton().Shutdown();
      //}

      Server::getSingleton().SetGameStarted(false);

      Server::getSingleton().ReceiveMessages();
      break;

    case CLIENT:
      // Shutdown the Client if this is the first iteration out of the game
      //if (Client::getSingleton().GetGameStarted() && Client::getSingleton().HasGameStarted()) {
      //  Client::getSingleton().Disconnect();
      //}

      Client::getSingleton().SetGameStarted(false);

      Client::getSingleton().ReceiveMessages();
      break;
  }
}

void TLMP::GameClient_CreateLevelPre(CGameClient* client, wstring unk0, wstring unk1, u32 unk2, u32 unk3, u32 unk4, wstring unk5, bool & calloriginal)
{
  multiplayerLogger.WriteLine(Info, L"GameClient::CreateLevel Pre (%s, %s, %s)",
    unk0.c_str(), unk1.c_str(), unk5.c_str());

  // Send a Message to clients that the Server's game has started
  if (Network::NetworkState::getSingleton().GetState() == SERVER) {
    Server::getSingleton().SetGameStarted(true);

    NetworkMessages::GameStarted msgGameStarted;

    Server::getSingleton().BroadcastMessage<NetworkMessages::GameStarted>(S_PUSH_GAMESTARTED, &msgGameStarted);
  }

  // Send a Message to the Server that the client has entered the game
  else if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
    Client::getSingleton().SetGameStarted(true);

    NetworkMessages::GameEnter msgGameEnter;

    Client::getSingleton().SendMessage<NetworkMessages::GameEnter>(C_PUSH_GAMEENTER, &msgGameEnter);
  }
}

void TLMP::GameClient_LoadLevelPre(CGameClient* client, bool & calloriginal)
{
  // Suppress level changes
  multiplayerLogger.WriteLine(Info, L"GameClient::LoadLevel");
  calloriginal = false;
}

void TLMP::GameClient_LoadMapPre(PVOID retval, CGameClient*, u32 unk0, bool & calloriginal)
{
  multiplayerLogger.WriteLine(Info, L"GameClient::LoadMap(%x)", unk0);

  // This will suppress the game load if the Server hasn't setup the game yet
  // TODO: Re-show the UI and Display an Error dialog
  if (unk0 == 2) {
    if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
      if (!Client::getSingleton().GetServerGameStarted()) {
        calloriginal = false;
      }
    }
  }
}

void TLMP::MainMenuEventPre(CMainMenu* mainMenu, u32 unk0, wstring str, bool & calloriginal)
{
  enum {
    CONTINUELASTGAME = 3,
    NEWGAME = 4,
    CONTINUEGAME = 5,     // Load Game
  };

  // Suppress this event if any of these actions are taken
  if (unk0 == CONTINUELASTGAME || unk0 == NEWGAME || unk0 == CONTINUEGAME) {
    if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
      if (!Client::getSingleton().GetServerGameStarted()) {
        calloriginal = false;
        multiplayerLogger.WriteLine(Info, L"Enter game suppressed: Client, Server not in game");

        // Display the UI message for waiting for server to start
        DisplayWaitForServerWindow();
      }
    }
  }
}

void TLMP::Monster_Idle(CMonster* monster, float dTime, bool & calloriginal)
{
  const u64 DESTROYER = 0xD3A8F9982FA111DE;
  const u64 ALCHEMIST = 0x8D3EE5363F7611DE;
  const u64 VANQUISHER = 0xAA472CC2629611DE;

  if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
    calloriginal = false;
  }
  else if (Network::NetworkState::getSingleton().GetState() == SERVER) {
    if (monster->GUID == DESTROYER || monster->GUID == ALCHEMIST || monster->GUID == VANQUISHER) {
      calloriginal = false;
    }
  }
}

void TLMP::Monster_ProcessAI(CMonster* monster, float dTime, bool & calloriginal)
{
  const u64 DESTROYER = 0xD3A8F9982FA111DE;
  const u64 ALCHEMIST = 0x8D3EE5363F7611DE;
  const u64 VANQUISHER = 0xAA472CC2629611DE;

  if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
    calloriginal = false;
  }
  else if (Network::NetworkState::getSingleton().GetState() == SERVER) {
    if (monster->GUID == DESTROYER || monster->GUID == ALCHEMIST || monster->GUID == VANQUISHER) {
      calloriginal = false;
    }
  }
}

void TLMP::Character_SetDestination(CCharacter* character, CLevel* level, float x, float z)
{
  NetworkMessages::CharacterDestination msgCharacterDestination;
  NetworkMessages::Position *msgPositionCurrent = msgCharacterDestination.add_current();
  NetworkMessages::Position *msgPositionDestination = msgCharacterDestination.add_destination();

  msgPositionCurrent->set_x(character->position.x);
  msgPositionCurrent->set_y(character->position.y);
  msgPositionCurrent->set_z(character->position.z);

  msgPositionDestination->set_x(x);
  msgPositionDestination->set_y(0);
  msgPositionDestination->set_z(z);

  // Find the CommonID for the Character
  u32 commonId;
  NetworkEntity* entity = searchCharacterByInternalObject((PVOID)character);

  if (entity) {
    commonId = entity->getCommonId();
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find Network Common ID for Character ptr: %p in SetDestination", character);
    return;
  }

  msgCharacterDestination.set_id(commonId);


  // Send a Network message off to the server if we're a client
  if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
    if (!Client::getSingleton().GetSuppressed_SetDestination()) {
      Client::getSingleton().SendMessage<NetworkMessages::CharacterDestination>(C_PUSH_CHARACTER_SETDEST, &msgCharacterDestination);
    }
  }
  // Send this message to all clients if this is the Server
  else if (Network::NetworkState::getSingleton().GetState() == SERVER) {
    if (!Server::getSingleton().GetSuppressed_SetDestination()) {
      multiplayerLogger.WriteLine(Info, L" Sending Destination for Common: %x (%p)", commonId, character);
      //log(L" Sending Destination for Common: %x (%p)", commonId, character);

      Server::getSingleton().BroadcastMessage<NetworkMessages::CharacterDestination>(S_PUSH_CHARACTER_SETDEST, &msgCharacterDestination);
    }
  }
}

void TLMP::Level_DropEquipmentPre(CLevel* level, CEquipment* equipment, Vector3 & position, bool unk0, bool& calloriginal)
{
  log(L"Level dropping EquipmentPre:");
  multiplayerLogger.WriteLine(Info, L"Level dropping Equipment %s at %f, %f, %f (unk0: %i)",
    equipment->nameReal.c_str(), position.x, position.y, position.z, unk0);
  log(L"Level dropping Equipment %s at %f, %f, %f (unk0: %i)",
    equipment->nameReal.c_str(), position.x, position.y, position.z, unk0);

}

void TLMP::Level_DropEquipmentPost(CLevel* level, CEquipment* equipment, Vector3 & position, bool unk0, bool& calloriginal)
{
  log(L"Level dropping EquipmentPost:");
  multiplayerLogger.WriteLine(Info, L"Level dropped Equipment %s at %f, %f, %f (unk0: %i)",
    equipment->nameReal.c_str(), position.x, position.y, position.z, unk0);
  log(L"Level dropped Equipment %s at %f, %f, %f (unk0: %i)",
    equipment->nameReal.c_str(), position.x, position.y, position.z, unk0);

  // Bump out before sending the network message if we're not supposed to
  if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
    if (Client::getSingleton().GetSuppressed_EquipmentDrop()) {
      return;
    }
  }

  //
  NetworkEntity *equipmentEntity = searchEquipmentByInternalObject((PVOID)equipment);

  if (equipmentEntity) {
    // Create a Network Message for sending off to clients the equipment addition to the inventory
    NetworkMessages::EquipmentDrop msgEquipmentDrop;

    NetworkMessages::Position *msgPosition = msgEquipmentDrop.add_position();
    msgPosition->set_x(position.x);
    msgPosition->set_y(position.y);
    msgPosition->set_z(position.z);

    msgEquipmentDrop.set_equipmentid(equipmentEntity->getCommonId());
    msgEquipmentDrop.set_unk0(unk0);

    // Client
    if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
      Client::getSingleton().SendMessage<NetworkMessages::EquipmentDrop>(C_PUSH_EQUIPMENT_DROP, &msgEquipmentDrop);
    }
    // Server
    else if (Network::NetworkState::getSingleton().GetState() == SERVER) {
      Server::getSingleton().BroadcastMessage<NetworkMessages::EquipmentDrop>(S_PUSH_EQUIPMENT_DROP, &msgEquipmentDrop);
    }
  } else {
    multiplayerLogger.WriteLine(Error, L"Could not find NetworkEntity for equipment: %p", equipment);
    log(L"Could not find NetworkEntity for equipment: %p", equipment);
  }
}

void TLMP::SendInventoryAddEquipmentToServer(CCharacter* owner, CEquipment* equipment, u32 slot, u32 unk)
{
  NetworkEntity *ownerEntity = searchCharacterByInternalObject((PVOID)owner);
  NetworkEntity *equipmentEntity = searchEquipmentByInternalObject((PVOID)equipment);

  multiplayerLogger.WriteLine(Info, L"Character (%s) Adding Equipment to Inventory: %s (Slot=%i) (Unk = %i)",
    owner->characterName.c_str(), equipment->nameReal.c_str(), slot, unk);
  log(L"Character (%s) Adding Equipment to Inventory: %s (Slot=%i) (Unk = %i)",
    owner->characterName.c_str(), equipment->nameReal.c_str(), slot, unk);

  if (ownerEntity) {
    // If the equipment hasn't yet been created for network use, do it now and send it out to the clients
    if (!equipmentEntity) {
      equipmentEntity = addEquipment(equipment);

      NetworkMessages::Equipment msgEquipment;
      msgEquipment.set_guid(equipment->GUID);
      msgEquipment.set_id(equipmentEntity->getCommonId());
      msgEquipment.set_stacksize(equipment->stackSize);
      msgEquipment.set_stacksizemax(equipment->stackSizeMax);
      msgEquipment.set_socketcount(equipment->socketCount);

      // TODO Add Enchants

      Client::getSingleton().SendMessage<NetworkMessages::Equipment>(C_PUSH_EQUIPMENT, &msgEquipment);
    }

    // Create a Network Message for sending off to clients the equipment addition to the inventory
    NetworkMessages::InventoryAddEquipment msgInventoryAddEquipment;
    msgInventoryAddEquipment.set_ownerid(ownerEntity->getCommonId());
    msgInventoryAddEquipment.set_slot(slot);
    msgInventoryAddEquipment.set_unk0(unk);
    msgInventoryAddEquipment.set_guid(equipment->GUID);
    msgInventoryAddEquipment.set_equipmentid(equipmentEntity->getCommonId());

    Client::getSingleton().SendMessage<NetworkMessages::InventoryAddEquipment>(C_PUSH_EQUIPMENT_EQUIP, &msgInventoryAddEquipment);
  } else {
    multiplayerLogger.WriteLine(Error, L"Could not find NetworkEntity for inventory owner: %p (%s)",
      owner, owner->characterName.c_str());
    log(L"Could not find NetworkEntity for inventory owner: %p (%s)",
      owner, owner->characterName.c_str());
  }
}

void TLMP::Inventory_AddEquipment(CEquipment* retval, CInventory* inventory, CEquipment* equipment, u32 slot, u32 unk0, bool& calloriginal)
{
  log(L"Inventory adding Equipment: %016I64X (%s) (%p) (Owner = %s)",
    equipment->GUID, equipment->nameReal.c_str(), inventory, inventory->pCCharacter->characterName.c_str());
  log(L"  inventory = %p  gameClient = %p",
    inventory, gameClient);
  log(L"  inventory->character = %p  player = %p",
    inventory->pCCharacter, gameClient->pCPlayer);

  const u32 CPLAYER_BASE = 0xA80064;
  const u32 CMONSTER_BASE = 0xA7F97C;

  CCharacter *owner = inventory->pCCharacter;
  u32 ptr_base = *((u32*)owner);

  // Client
  if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
    if (Client::getSingleton().GetSuppressed_EquipmentCreation()) {
      //if (ptr_base != CPLAYER_BASE) {
        calloriginal = false;
        retval = NULL;

        // If we're suppressing this, we need to send it to the Server
        // The server will create, add the equipment and send the message back to us
        SendInventoryAddEquipmentToServer(owner, equipment, slot, unk0);
      //}
    }
  }

  // Server
  else if (Network::NetworkState::getSingleton().GetState() == SERVER) {
    // If we're not suppressed to send this out - 
    // The reason this would be suppressed if a character had sent a Pickup message,
    // That triggers an Equip then a Pickup message on the client, causing a phantom item in the inventory
    if (!Server::getSingleton().GetSuppressed_SendEquipmentEquip()) {
      NetworkEntity *ownerEntity = searchCharacterByInternalObject((PVOID)owner);
      NetworkEntity *equipmentEntity = searchEquipmentByInternalObject((PVOID)equipment);

      if (ownerEntity) {
        if (equipmentEntity) {
          // Create a Network Message for sending off to clients the equipment addition to the inventory
          NetworkMessages::InventoryAddEquipment msgInventoryAddEquipment;
          msgInventoryAddEquipment.set_ownerid(ownerEntity->getCommonId());
          msgInventoryAddEquipment.set_equipmentid(equipmentEntity->getCommonId());
          msgInventoryAddEquipment.set_slot(slot);
          msgInventoryAddEquipment.set_unk0(unk0);

          Server::getSingleton().BroadcastMessage<NetworkMessages::InventoryAddEquipment>(S_PUSH_EQUIPMENT_EQUIP, &msgInventoryAddEquipment);
        } else {
          multiplayerLogger.WriteLine(Error, L"Could not find NetworkEntity for equipment: %p",
            equipment);
          log(L"Could not find NetworkEntity for equipment: %p",
            equipment);
        }
      } else {
        multiplayerLogger.WriteLine(Error, L"Could not find NetworkEntity for inventory owner: (%p) %p",
          inventory, owner);
        log(L"Could not find NetworkEntity for inventory owner: (%p) %p",
          inventory, owner);
      }
    }
  }
  

  if (calloriginal) {
    multiplayerLogger.WriteLine(Info, L"Inventory::AddEquipment(%p) (%p) (%s)",
      inventory, equipment, equipment->nameReal.c_str());
    log(L"Inventory::AddEquipment(%p) (%p) (%s)",
      inventory, equipment, equipment->nameReal.c_str());
  } else {
    multiplayerLogger.WriteLine(Info, L"  Suppressed");
    log(L"  Suppressed");
  }
}

void TLMP::Inventory_RemoveEquipmentPre(CInventory* inventory, CEquipment* equipment)
{
  log(L"Inventory removing Equipment:");

  multiplayerLogger.WriteLine(Info, L"Inventory::RemoveEquipment(%p) (%s)",
    inventory, equipment->nameReal.c_str());
  log(L"Inventory::RemoveEquipment(%p) (%s)",
    inventory, equipment->nameReal.c_str());

  //
  CCharacter *owner = inventory->pCCharacter;
  NetworkEntity *ownerEntity = searchCharacterByInternalObject((PVOID)owner);
  NetworkEntity *equipmentEntity = searchEquipmentByInternalObject((PVOID)equipment);

  //
  if (!ownerEntity) {
    multiplayerLogger.WriteLine(Error, L"Could not find NetworkEntity for inventory owner: (%p) %p",
      inventory, owner);
    log(L"Could not find NetworkEntity for inventory owner: (%p) %p",
      inventory, owner);
    return;
  } else if (!equipmentEntity) {
    multiplayerLogger.WriteLine(Error, L"Could not find NetworkEntity for equipment: %p",
      equipment);
    log(L"Could not find NetworkEntity for equipment: %p",
      equipment);
    return;
  }

  // Client
  if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
    if (Client::getSingleton().GetSuppressed_SendEquipmentUnequip()) {
      // Send the server that we're unequipping the equipment
      NetworkMessages::InventoryRemoveEquipment msgInventoryRemoveEquipment;
      msgInventoryRemoveEquipment.set_ownerid(ownerEntity->getCommonId());
      msgInventoryRemoveEquipment.set_equipmentid(equipmentEntity->getCommonId());

      Client::getSingleton().SendMessage<NetworkMessages::InventoryRemoveEquipment>(C_PUSH_EQUIPMENT_UNEQUIP, &msgInventoryRemoveEquipment);
    }
  }

  // Server
  else if (Network::NetworkState::getSingleton().GetState() == SERVER) {
    // Create a Network Message for sending off to clients the equipment addition to the inventory
    NetworkMessages::InventoryRemoveEquipment msgInventoryRemoveEquipment;
    msgInventoryRemoveEquipment.set_ownerid(ownerEntity->getCommonId());
    msgInventoryRemoveEquipment.set_equipmentid(equipmentEntity->getCommonId());

    Server::getSingleton().BroadcastMessage<NetworkMessages::InventoryRemoveEquipment>(S_PUSH_EQUIPMENT_UNEQUIP, &msgInventoryRemoveEquipment);
  }
}

void TLMP::Character_PickupEquipmentPre(CCharacter* character, CEquipment* equipment, CLevel* level, bool & calloriginal)
{
  log(L"Character picking up Equipment Pre:");

  multiplayerLogger.WriteLine(Info, L"Character::PickupEquipment(%p) (%s) (Level: %p)",
    character, equipment->nameReal.c_str(), level);
  log(L"Character::PickupEquipment(%p) (%s) (Level: %p)",
    character, equipment->nameReal.c_str(), level);

  // If the client is requesting a pickup, request the server first
  // The server will respond with a pickup message
  if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
    if (Client::getSingleton().GetSuppressed_EquipmentPickup()) {
      if (!Client::getSingleton().Get_IsSendingPickup()) {
        // Send the server a message requesting our character to pickup the item
        NetworkEntity *netPlayer = searchCharacterByInternalObject((PVOID)character);
        NetworkEntity *netEquipment = searchEquipmentByInternalObject((PVOID)equipment);

        if (!netPlayer) {
          multiplayerLogger.WriteLine(Error, L"Error: Could not retrieve network entity of character for equipment pickup!");
          log(L"Error: Could not retrieve network entity of character for equipment pickup!");
        } else if (!netEquipment) {
          multiplayerLogger.WriteLine(Error, L"Error: Could not retrieve network entity of equipment for equipment pickup!");
          log(L"Error: Could not retrieve network entity of equipment for equipment pickup!");
        } else {
          NetworkMessages::EquipmentPickup msgEquipmentPickup;
          msgEquipmentPickup.set_characterid(netPlayer->getCommonId());
          msgEquipmentPickup.set_equipmentid(netEquipment->getCommonId());

          Client::getSingleton().SendMessage<NetworkMessages::EquipmentPickup>(C_PUSH_EQUIPMENT_PICKUP, &msgEquipmentPickup);
          Client::getSingleton().Set_IsSendingPickup(true);
        }
      }

      calloriginal = false;
    } else {
      Client::getSingleton().SetSuppressed_EquipmentCreation(false);
    }
  }
  // Send the message off to the clients
  else if (Network::NetworkState::getSingleton().GetState() == SERVER) {
    NetworkEntity *netEquipment = searchEquipmentByInternalObject(equipment);
    NetworkEntity *netCharacter = searchCharacterByInternalObject(character);

    if (netEquipment) {
      if (netCharacter) {
        NetworkMessages::EquipmentPickup msgEquipmentPickup;
        msgEquipmentPickup.set_characterid(netCharacter->getCommonId());
        msgEquipmentPickup.set_equipmentid(netEquipment->getCommonId());

        Server::getSingleton().BroadcastMessage<NetworkMessages::EquipmentPickup>(S_PUSH_EQUIPMENT_PICKUP, &msgEquipmentPickup);
        Server::getSingleton().SetSuppressed_SendEquipmentEquip(true);
      } else {
        multiplayerLogger.WriteLine(Error, L"Error: Could not find Network Entity for Character (%s)",
          character->characterName.c_str());
        log(L"Error: Could not find Network Entity for Character (%s)",
          character->characterName.c_str());
      }
    } else {
      multiplayerLogger.WriteLine(Error, L"Error: Could not find Network Entity for Equipment (%s)",
        equipment->nameReal.c_str());
      log(L"Error: Could not find Network Entity for Equipment (%s)",
        equipment->nameReal.c_str());
    }
  }
}

void TLMP::Character_PickupEquipmentPost(CCharacter* character, CEquipment* equipment, CLevel* level, bool & calloriginal)
{
  log(L"Character picking up Equipment Post:");

  multiplayerLogger.WriteLine(Info, L"Character::PickupEquipment(%p) (%s) (Level: %p)",
    character, equipment->nameReal.c_str(), level);
  log(L"Character::PickupEquipment(%p) (%s) (Level: %p)",
    character, equipment->nameReal.c_str(), level);
  
  // Turn on suppression again if we're the client
  if (Network::NetworkState::getSingleton().GetState() == CLIENT) {
    // If the original call was let through...
    if (calloriginal) {
      Client::getSingleton().SetSuppressed_EquipmentCreation(true);
      Client::getSingleton().Set_IsSendingPickup(false);
    }
  }
  // Server turn off suppression of Equipment Equips
  else if (Network::NetworkState::getSingleton().GetState() == SERVER) {
    Server::getSingleton().SetSuppressed_SendEquipmentEquip(false);
  }
  
}

// Server Events
void TLMP::ServerOnClientConnected(void *arg)
{
  SystemAddress address = *(SystemAddress *)arg;
  NetworkMessages::Version msgVersion;
  msgVersion.set_version(MessageVersion);

  multiplayerLogger.WriteLine(Info, L"Client connected - sending S_VERSION push to: %x:%i", address.binaryAddress, address.port);

  Server::getSingleton().SendMessage<NetworkMessages::Version>(address, S_VERSION, &msgVersion);
}
// --
