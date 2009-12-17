#include "Network.h"


void TLMP::Network::HostGame(u16 port)
{
  NetworkState = SERVER;
  Network::Server::getSingleton().Listen(port, 32);
}

void TLMP::Network::JoinGame(const char *address, u16 port)
{
  NetworkState = CLIENT;
  Network::Client::getSingleton().Connect(address, port);
}

void TLMP::Network::StartReceivingMessages()
{
  NetworkThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveMessages, NULL, CREATE_SUSPENDED, (LPDWORD)&nThreadID);
  log("NetworkThread = %i", NetworkThread);
  ResumeThread(NetworkThread);
}

void TLMP::Network::ReceiveMessages(void *args)
{
  while (1) {
    if (NetworkState == SERVER) {
      Network::Server::getSingleton().ReceiveMessages();
    } else if (NetworkState == CLIENT) {
      Network::Client::getSingleton().ReceiveMessages();
    }

    TimeManager::getSingleton().sleep(50);
  }
}
