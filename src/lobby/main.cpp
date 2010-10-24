#ifdef WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif

#include <stdio.h>
#include <string>

#include "lobbyServer.h"
using namespace TLMP::Network::Lobby;


int main(int argc, char** argv)
{
  // arg fmt:
  // ./lobbyServer port maxconnection
  if (argc != 3) {
    printf("Format is: lobbyServer port maxconnections\n");
    return 1;
  }

  const unsigned short port = (unsigned short)atoi(argv[1]);
  const unsigned short maxconnections = (unsigned short)atoi(argv[2]);

  printf("Torchlight Multiplayer Lobby Server\n");
  printf("  Listening on Port: %i\n", port);
  printf("  Maximum connections: %i\n", maxconnections);

  LobbyServer server;
  server.Initialize(port, maxconnections);

  while (true) {
    server.Process();

#ifdef WIN32
    Sleep(10);
#else
#error Define sleep unix: usleep(us);
#endif
  }
}
