#pragma once

#include <vector>
using std::vector;

#include "Entity.h"
#include "Item.h"


namespace TLMP {

  namespace State {

    extern vector<c_entity>* ServerEntities;
    extern vector<c_item>* ServerItems;

  };

};
