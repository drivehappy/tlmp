#pragma once

#include "NetworkEntity.h"

#include <map>
using std::map;
#include <vector>
using std::vector;

/*
  This file contains global variables that store state information used
  across both the server and clients.
**/

namespace TLMP {

  extern vector<NetworkEntity*>* NetworkSharedItems;
  extern vector<NetworkEntity*>* NetworkSharedEntities;

};
