#pragma once

#pragma comment(lib, "../ExternalLibs/tlapi/out/tlapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "../ExternalLibs/google/lib/libprotobuf.lib")
#pragma comment(lib, "../ExternalLibs/raknet/lib/RakNetLibStatic.lib")


#include "DataTypes.h"
#include "Common.h"
#include "Messages.h"

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

};
