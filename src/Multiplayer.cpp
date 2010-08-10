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

  CGame::RegisterEvent_Game_CreateUI(NULL, GameClient_CreateUI);

  CResourceManager::RegisterEvent_ResourceManagerCreateCharacter(NULL, CreateCharacter);
  CResourceManager::RegisterEvent_ResourceManagerCreateEquipment(NULL, CreateEquipment);

  CLevel::RegisterEvent_LevelCharacterInitialize(NULL, Level_CharacterInitialize);

  CGameClient::RegisterEvent_GameClientProcessObjects(NULL, GameClient_ProcessObjects);
  CGameClient::RegisterEvent_GameClientProcessTitleScreen(NULL, GameClient_TitleProcessObjects);
  CGameClient::RegisterEvent_GameClient_CreateLevel(GameClient_CreateLevelPre, NULL);
  CGameClient::RegisterEvent_GameClient_LoadLevel(GameClient_LoadLevelPre, NULL);
  CGameClient::RegisterEvent_GameClientLoadMap(GameClient_LoadMapPre, NULL);

  CMainMenu::RegisterEvent_MainMenu_Event(MainMenuEventPre, NULL);
  // --

  multiplayerLogger.WriteLine(Info, L"Registering Events... Done.");

  Server::getSingleton().SetCallback_OnClientConnect(&ServerOnClientConnected);
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

void TLMP::CreateCharacter(CCharacter* character, CResourceManager* resourceManager, u64 guid, u32 level, bool unk0)
{
  multiplayerLogger.WriteLine(Info, L"Created character with guid of: %016I64X (%p, %s)",
    guid, character, character->characterName.c_str());
}

void TLMP::CreateEquipment(CEquipment* equipment, CResourceManager* resourceManager, u64 guid, u32 level, u32 unk0, u32 unk1)
{
  multiplayerLogger.WriteLine(Info, L"Created equipment with guid of: %016I64X Level: %i (%p, %s)",
    guid, level, equipment, equipment->nameReal.c_str());
}

void TLMP::Level_CharacterInitialize(CCharacter* retval, CLevel* level, CCharacter* character, Vector3* position, u32 unk0)
{
  multiplayerLogger.WriteLine(Info, L"Level Character Initialized: %p %s (%f %f %f) unk0: %x",
    character, character->characterName.c_str(), position->x, position->y, position->z, unk0);
}

void TLMP::GameClient_ProcessObjects(CGameClient *client, float dTime, PVOID unk1, PVOID unk2)
{
  gameClient = client;

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
  gameClient = client;

  //log("MainMenuShown: %i", MainMenuShown());

  // Determine if we've exited an existing game in network mode
  switch (Network::NetworkState::getSingleton().GetState()) {
    case SERVER:
      // Shutdown the Server if this is the first iteration out of the game
      if (Server::getSingleton().GetGameStarted() && Server::getSingleton().HasGameStarted()) {
        Server::getSingleton().Shutdown();
      }

      Server::getSingleton().SetGameStarted(false);

      Server::getSingleton().ReceiveMessages();
      break;

    case CLIENT:
      // Shutdown the Client if this is the first iteration out of the game
      if (Client::getSingleton().GetGameStarted() && Client::getSingleton().HasGameStarted()) {
        Client::getSingleton().Disconnect();
      }

      Client::getSingleton().SetGameStarted(false);

      Client::getSingleton().ReceiveMessages();
      break;
  }
}

void TLMP::GameClient_CreateLevelPre(CGameClient* client, wstring unk0, wstring unk1, u32 unk2, u32 unk3, u32 unk4, wstring unk5, bool & calloriginal)
{
  multiplayerLogger.WriteLine(Info, L"GameClient::CreateLevel Pre (%s, %s, %s)",
    unk0.c_str(), unk1.c_str(), unk5.c_str());
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
      if (!Client::getSingleton().GetGameStarted()) {
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
      if (!Client::getSingleton().GetGameStarted()) {
        calloriginal = false;
        multiplayerLogger.WriteLine(Info, L"Enter game suppressed: Client, Server not in game");

        // Display the UI message for waiting for server to start
        DisplayWaitForServerWindow();
      }
    }
  }

  log(L"MainMenuEvent: %x %s", unk0, str.c_str());
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