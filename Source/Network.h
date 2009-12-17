#pragma once

#include "State.h"
#include "Server.h"
#include "Client.h"

namespace TLMP {

  namespace Network {

    static State  NetworkState  = SINGLEPLAYER;
    static HANDLE NetworkThread = 0;
    static DWORD  nThreadID     = 0;

    void HostGame(u16 port);

    void JoinGame(const char *address, u16 port);

    void StartReceivingMessages();
    void ReceiveMessages(void *);

  };

};