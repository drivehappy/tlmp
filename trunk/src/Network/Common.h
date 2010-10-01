#pragma once

#pragma comment(lib, "../ExternalLibs/tlapi/out/tlapi.lib")
#pragma comment(lib, "ws2_32.lib")

#if (_MSC_VER == 1500)
#pragma comment(lib, "../ExternalLibs/google/lib/libprotobuf.lib")
#pragma comment(lib, "../ExternalLibs/raknet/lib/RakNetLibStatic.lib")
#else
#error Your compiler isn't supported - the libraries to build will need to be built manually.
#endif

#include "DataTypes.h"
#include "Common.h"
#include "Messages.h"
using namespace TLMP::Network;

#include "raknet/include/RakPeerInterface.h"
#include "raknet/include/RakNetworkFactory.h"
#include "raknet/include/MessageIdentifiers.h"
#include "raknet/include/BitStream.h"

#include "network.pb.h"

#include "NetworkEntity.h"
#include "SharedState.h"

#include "Offsets.h"
#include "CEquipment.h"
#include "CPlayer.h"
using namespace TLAPI;

#include "Logger.h"

namespace TLMP
{

  extern Logger multiplayerLogger;
  extern CGameClient      *gameClient;

};
