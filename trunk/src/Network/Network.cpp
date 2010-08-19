#include "Network.h"
using namespace TLMP::Network;


NetworkState& NetworkState::getSingleton()
{
  static NetworkState m_pSingleton;
  return m_pSingleton;
}

NetworkState::NetworkState()
{
  m_bSuppressLevelChange = true;
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
