#include <stdio.h>
#include <string>

#include "lobbyServer.h"


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

  
}
