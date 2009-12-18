#include "Network.h"
using namespace TLMP::Network;


NetworkState& NetworkState::getSingleton()
{
  static NetworkState m_pSingleton;
  return m_pSingleton;
}

NetworkState::NetworkState()
{
}

NetworkState::~NetworkState()
{
}

State NetworkState::GetState() const
{
  return m_State;
}

void NetworkState::SetState(State state)
{
  m_State = state;
}

void NetworkState::SetNetworkThread(HANDLE thread)
{
  m_NetworkThread = thread;
}

HANDLE NetworkState::GetNetworkThread()
{
  return m_NetworkThread;
}

void NetworkState::SetThreadId(DWORD id)
{
  m_ThreadID = id;
}

DWORD NetworkState::GetThreadId()
{
  return m_ThreadID;
}



void TLMP::Network::HostGame(u16 port)
{
  Network::Server::getSingleton().Listen(port, 32);
  Network::NetworkState::getSingleton().SetState(SERVER);
}

void TLMP::Network::JoinGame(const char *address, u16 port)
{
  Network::Client::getSingleton().Connect(address, port);
  Network::NetworkState::getSingleton().SetState(CLIENT);
}

void TLMP::Network::StartReceivingMessages()
{
  DWORD nThreadID;
  NetworkState::getSingleton().SetNetworkThread(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveMessages, NULL, CREATE_SUSPENDED, (LPDWORD)&nThreadID));
  NetworkState::getSingleton().SetThreadId(nThreadID);
  ResumeThread(NetworkState::getSingleton().GetNetworkThread());
}

void TLMP::Network::ReceiveMessages(void *args)
{
  while (1) {
    if (NetworkState::getSingleton().GetState() == SERVER) {
      Network::Server::getSingleton().ReceiveMessages();
    } else if (NetworkState::getSingleton().GetState() == CLIENT) {
      Network::Client::getSingleton().ReceiveMessages();
    }

    TimeManager::getSingleton().sleep(50);
  }
}
