#pragma once

#include "Messages.h"
#include "Server.h"
#include "Client.h"
#include "NetworkEntity.h"
#include "State.h"

namespace TLMP {

  namespace Network {

    class NetworkState {
    public:
      static NetworkState& getSingleton();

      void SetState(State state);
      State GetState() const;

    protected:
      NetworkState();
      NetworkState(const NetworkState&);
      NetworkState& operator=(const NetworkState&);
      ~NetworkState();

    private:
      State   m_State;
    };

    void HostGame(u16 port);

    void JoinGame(const char *address, u16 port);

    void StartReceivingMessages();
    void ReceiveMessages(void *);

  };

};
