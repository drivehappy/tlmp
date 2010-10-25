#include "LobbyClient.h"
using namespace TLMP::Network::Lobby;


LobbyClient& LobbyClient::getSingleton()
{
  static LobbyClient m_Instance;

  return m_Instance;
}

LobbyClient::LobbyClient()
{
  m_pClient = NULL;
  m_pBitStream = new RakNet::BitStream(1024);
}

LobbyClient::~LobbyClient()
{
  if (m_pClient) {
    delete m_pClient;
  }
}


void LobbyClient::Connect(const char* address, unsigned short port)
{
  log("Lobby client connecting to %s (%i)", address, port);

  if (m_pClient) {
    m_pClient->Shutdown(0);
  }

  m_pClient = RakNetworkFactory::GetRakPeerInterface();

  m_pClient->SetTimeoutTime(10000, UNASSIGNED_SYSTEM_ADDRESS);

  SocketDescriptor socketDescriptor(0, 0);
  m_pClient->Startup(1, 30, &socketDescriptor, 1);
  m_pClient->Connect(address, port, 0, 0);
}

void LobbyClient::Disconnect()
{
  log("Lobby client disconnecting...");

  if (m_pClient) {
    m_pClient->Shutdown(0);
    m_pClient = NULL;
  }
}

void LobbyClient::ReceiveMessages()
{
  if (!m_pClient)
    return;

  Packet *packet = m_pClient->Receive();
  while (packet)
  {
    switch(packet->data[0])
    {
    case ID_CONNECTION_REQUEST_ACCEPTED:
      {
        log("Lobby client connected to server.");

        SystemAddress *sysAddress = new SystemAddress();
        *sysAddress = packet->systemAddress;

        CEGUI::Listbox* pWindowChatHistory = (CEGUI::Listbox*)getLobbyChatHistoryWindow();
        if (pWindowChatHistory) {
          pWindowChatHistory->appendText(string("Connected to lobby host."));
        }
      }

      break;
    case ID_NO_FREE_INCOMING_CONNECTIONS:
      {
        log("Could not connect to lobby server: No room.");
        CEGUI::Listbox* pWindowChatHistory = (CEGUI::Listbox*)getLobbyChatHistoryWindow();
        if (pWindowChatHistory) {
          pWindowChatHistory->appendText(string("Lobby host is full, cannot connect."));
        }
      }
      break;
    case ID_DISCONNECTION_NOTIFICATION:
      {
        log("Lobby client disconnected.");
        CEGUI::Listbox* pWindowChatHistory = (CEGUI::Listbox*)getLobbyChatHistoryWindow();
        if (pWindowChatHistory) {
          pWindowChatHistory->appendText(string("You have been disconnected from the lobby host."));
        }
      }
      break;
    case ID_CONNECTION_LOST:
      {
        log("Lobby client connection lost.");
        CEGUI::Listbox* pWindowChatHistory = (CEGUI::Listbox*)getLobbyChatHistoryWindow();
        if (pWindowChatHistory) {
          pWindowChatHistory->appendText(string("Connection to lobby host was lost."));
        }
      }
      break;
    case ID_CONNECTION_ATTEMPT_FAILED:
      {
        log("Lobby client connection failed.");
        CEGUI::Listbox* pWindowChatHistory = (CEGUI::Listbox*)getLobbyChatHistoryWindow();
        if (pWindowChatHistory) {
          pWindowChatHistory->appendText(string("Failed to connect to lobby host."));
        }
      }
      break;
    case ID_USER_PACKET_ENUM+1:
      unsigned int msg;
      m_pBitStream->Reset();
      m_pBitStream->Write((const char *)packet->data, packet->length);
      m_pBitStream->IgnoreBits(8);
      m_pBitStream->Read<unsigned int>(msg);

      WorkMessage((LobbyMessage)msg, m_pBitStream);

      break;
    }

    m_pClient->DeallocatePacket(packet);
    packet = m_pClient->Receive();
  }
}

void LobbyClient::WorkMessage(LobbyMessage msg, RakNet::BitStream *bitStream)
{
  wstring msgString = convertAsciiToWide(LobbyMessageString[msg]);

  multiplayerLogger.WriteLine(Info, L"Lobby Client Received Message: %s", msgString.c_str());
  log(L"Lobby Client Received Message: %s", msgString.c_str());

  switch (msg) {
  case L_S_VERSION:
    {
      LobbyMessages::Version *msgVersion = ParseMessage<LobbyMessages::Version>(m_pBitStream);

      HandleVersion(msgVersion);
    }
    break;

  case L_S_PLAYERNAME_JOIN:
    {
      LobbyMessages::ClientPlayerName *msgClientPlayerName = ParseMessage<LobbyMessages::ClientPlayerName>(m_pBitStream);

      HandleClientPlayerName(msgClientPlayerName);
    }
    break;

  case L_S_PLAYERNAME_BATCH:
    {
      LobbyMessages::BatchPlayerNames *msgBatchPlayerNames = ParseMessage<LobbyMessages::BatchPlayerNames>(m_pBitStream);

      HandleBatchPlayerNames(msgBatchPlayerNames);
    }
    break;

  case L_S_CHAT_MESSAGE:
    {
      LobbyMessages::ChatMessage *msgChatMessage = ParseMessage<LobbyMessages::ChatMessage>(m_pBitStream);

      HandleChatMessage(msgChatMessage);
    }
    break;

  case L_S_PLAYERNAME_LEAVE:
    {
      LobbyMessages::PlayerLeft *msgPlayerLeft = ParseMessage<LobbyMessages::PlayerLeft>(m_pBitStream);

      HandlePlayerLeft(msgPlayerLeft);
    }
    break;
  }
}

void LobbyClient::HandleVersion(LobbyMessages::Version *msgVersion)
{
  unsigned int version = (unsigned int)(msgVersion->version());

  log("Lobby Client Got Server Version: %i", version);

  LobbyMessages::Version msgVersionReply;
  msgVersionReply.set_version(LobbyMessageVersion);
  SendMessage<LobbyMessages::Version>(L_C_VERSION, &msgVersionReply);

  // Send the player name off to the server so it can be listed in the player list
  string senderName(gameClient->pCPlayer->characterName.begin(), gameClient->pCPlayer->characterName.end());
  senderName.assign(gameClient->pCPlayer->characterName.begin(), gameClient->pCPlayer->characterName.end());

  LobbyMessages::ClientPlayerName msgClientPlayerName;
  msgClientPlayerName.set_playername(senderName);
  SendMessage<LobbyMessages::ClientPlayerName>(L_C_PLAYERNAME, &msgClientPlayerName);

  // Display version mismatch error
  if (version != LobbyMessageVersion) {
    CEGUI::Listbox* pWindowChatHistory = (CEGUI::Listbox*)getLobbyChatHistoryWindow();
    if (pWindowChatHistory) {
      pWindowChatHistory->appendText(string("Version mismatch with the lobby server. You most likely have an old version of the client that will not work with the new lobby server. Please update."));
      m_pClient->Shutdown(0);
    }
  }
}

void LobbyClient::HandleClientPlayerName(LobbyMessages::ClientPlayerName *msgClientPlayerName)
{
  string playerName = msgClientPlayerName->playername();

  CEGUI::MultiLineEditbox *pWindowPlayerList = (CEGUI::MultiLineEditbox *)getLobbyPlayerListWindow();
  if (pWindowPlayerList) {
    pWindowPlayerList->appendText(playerName);
  }

  CEGUI::MultiLineEditbox* pWindowChatHistory = (CEGUI::MultiLineEditbox*)getLobbyChatHistoryWindow();
  if (pWindowChatHistory) {
    pWindowChatHistory->appendText(string("Player joined lobby: ") + playerName);
  }
}

void LobbyClient::HandleBatchPlayerNames(LobbyMessages::BatchPlayerNames *msgBatchPlayerNames)
{
  int nameCount = msgBatchPlayerNames->playernames().size();

  CEGUI::MultiLineEditbox *pWindowPlayerList = (CEGUI::MultiLineEditbox *)getLobbyPlayerListWindow();
  if (pWindowPlayerList) {
    for (int i = 0; i < nameCount; i++) {
      string playerName = msgBatchPlayerNames->playernames().Get(i);
      pWindowPlayerList->appendText(playerName);
    }
  }
}

void LobbyClient::HandlePlayerLeft(LobbyMessages::PlayerLeft *msgPlayerLeft)
{
  string playerName = msgPlayerLeft->playername();
  int nameCount = msgPlayerLeft->playersremaining().size();

  CEGUI::MultiLineEditbox* pWindowChatHistory = (CEGUI::MultiLineEditbox*)getLobbyChatHistoryWindow();
  if (pWindowChatHistory) {
    pWindowChatHistory->appendText(string("Player has left the lobby: ") + playerName);
  }

  // Clear our player list and repopulate with the remaining players
  CEGUI::MultiLineEditbox *pWindowPlayerList = (CEGUI::MultiLineEditbox *)getLobbyPlayerListWindow();
  if (pWindowPlayerList) {
    pWindowPlayerList->setText("");
    for (int i = 0; i < nameCount; i++) {
      string playerName = msgPlayerLeft->playersremaining().Get(i);
      pWindowPlayerList->appendText(playerName);
    }
  }
}

void LobbyClient::HandleChatMessage(LobbyMessages::ChatMessage *msgChat)
{
  const std::string sender = msgChat->sender();
  const std::string msg = msgChat->message();

  CEGUI::Listbox* pWindowChatHistory = (CEGUI::Listbox*)getLobbyChatHistoryWindow();
  if (pWindowChatHistory) {
    pWindowChatHistory->appendText(sender + string(": ") + msg);
  }
}
