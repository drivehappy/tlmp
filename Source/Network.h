#pragma once

#include "Messages.h"
#include "State.h"
#include "Server.h"
#include "Client.h"
#include "NetworkEntity.h"

namespace TLMP {

  namespace Network {

    class NetworkState {
    public:
      static NetworkState& getSingleton();

      void SetState(State state);
      State GetState() const;

      void SetNetworkThread(HANDLE thread);
      HANDLE GetNetworkThread();

      void SetThreadId(DWORD id);
      DWORD GetThreadId();

    protected:
      NetworkState();
      NetworkState(const NetworkState&);
      NetworkState& operator=(const NetworkState&);
      ~NetworkState();

    private:
      State   m_State;
      HANDLE  m_NetworkThread;
      DWORD   m_ThreadID;
    };

    void HostGame(u16 port);

    void JoinGame(const char *address, u16 port);

    void StartReceivingMessages();
    void ReceiveMessages(void *);

  };

};