#pragma once

#pragma comment(lib, "ws2_32.lib")

#if (_MSC_VER == 1500)
#pragma comment(lib, "../ExternalLibs/google/lib/libprotobuf.lib")
#pragma comment(lib, "../ExternalLibs/raknet/lib/RakNetLibStatic.lib")
#else
#error Your compiler isn't supported - the libraries to build will need to be built manually.
#endif

#include "raknet/include/RakPeerInterface.h"
#include "raknet/include/RakNetworkFactory.h"
#include "raknet/include/MessageIdentifiers.h"
#include "raknet/include/BitStream.h"

#include "Messages.h"
