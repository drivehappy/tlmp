#pragma once

namespace TLMP {

  namespace Network {

    enum Messages {
      // Server messages pushed to clients
      S_UNKNOWN = 0,
      S_PLAYER_INFO,
      S_ENTITY_SETDEST,
      S_ITEM_CREATE,
      S_ITEM_DROP,
      S_ITEM_PICKUP,
      S_ITEM_EQUIP,
      S_ITEM_UNEQUIP,

      // Client messages pushed to server
      C_UNKNOWN = 0x1000,
      C_PLAYER_INFO,
      C_PLAYER_SETDEST,
      C_ITEM_DROP,
      C_ITEM_PICKUP,
      C_ITEM_EQUIP,
      C_ITEM_UNEQUIP,
    };

  };

};