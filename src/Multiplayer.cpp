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
  CGame::RegisterEvent_Game_CreateUI(NULL, GameClient_CreateUI);

  CResourceManager::RegisterEvent_ResourceManagerCreateCharacter(NULL, CreateCharacter);
  CResourceManager::RegisterEvent_ResourceManagerCreateEquipment(NULL, CreateEquipment);

  CLevel::RegisterEvent_LevelCharacterInitialize(NULL, Level_CharacterInitialize);

  CGameClient::RegisterEvent_GameClientProcessObjects(NULL, GameClient_ProcessObjects);
  CGameClient::RegisterEvent_GameClientProcessTitleScreen(NULL, GameClient_TitleProcessObjects);
  // --

  multiplayerLogger.WriteLine(Info, L"Registering Events... Done.");
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
      break;

    case CLIENT:
      Client::getSingleton().SetGameStarted(true);
      break;
  }
}

void TLMP::GameClient_TitleProcessObjects(CGameClient *client, float dTime, PVOID unk1, PVOID unk2)
{
  gameClient = client;

  // Determine if we've exited an existing game in network mode
  switch (Network::NetworkState::getSingleton().GetState()) {
    case SERVER:
      // Shutdown the Server if this is the first iteration out of the game
      if (Server::getSingleton().GetGameStarted() && Server::getSingleton().HasGameStarted()) {
        Server::getSingleton().Shutdown();
      }

      Server::getSingleton().SetGameStarted(false);
      break;

    case CLIENT:
      // Shutdown the Client if this is the first iteration out of the game
      if (Client::getSingleton().GetGameStarted() && Client::getSingleton().HasGameStarted()) {
        Client::getSingleton().Disconnect();
      }

      Client::getSingleton().SetGameStarted(false);
      break;
  }
}
