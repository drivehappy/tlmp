#pragma once

#include "DataTypes.h"


namespace TLMP {

  namespace Network {

    /** Version to signify any Messaging ID changes. */
    const u32 MessageVersion = 1;

    /** Network Control Messages */
    enum Message {

      S_VERSION = 0,                // These are expected to Match
      C_VERSION,                    // if not, both ends disconnect (Client will reply to server with version)

      C_REQUEST_HASGAMESTARTED,     // Requests the Server to determine if it's in the game yet,
      S_REPLY_HASGAMESTARTED,       // replies to client request
                                    // Clients are blocked from entering the game until the Server has established one

      S_PUSH_GAMESTARTED,           // Inform Clients that the game has started - Sent Post GameClient::LoadLevel
      S_PUSH_GAMEENDED,             // Inform Clients that the game has ended - Sent on TitleScreenProcess after leaving game (for now)

      C_PUSH_GAMEENTER,             // Inform Server that Client has entered the Game - Sent Post GameClient::LoadLevel
      C_PUSH_GAMEEXITED,            // Inform Server that Client has exited the Game - Sent on TitleScreenProcess after leaving game (for now)

      S_REQUEST_CHARINFO,           // Server Requests Client for Character Information (Name, Inventory, Class, Minions, etc.)
      C_REPLY_CHARINFO,             // Client Respone for Character Information Request

      S_PUSH_NEWCHAR,               // Server Pushes a New Character to the Client
      S_PUSH_NEWEQUIPMENT,          // Server Pushes a New Equipment

      S_PUSH_ADDCHARMINION,         // Server Pushes a Minion setup to Client
      S_PUSH_SETCHARALIGNMENT,      // Server Pushes a Character SetAlignment to Client

      C_PUSH_EQUIPMENT_DROP,        // Client Pushes an Equipment Drop from Character
      S_PUSH_EQUIPMENT_DROP,        // Server Pushes an Equipment Drop from Character
                                    // Note: These aren't requests, the Server will notify other clients of the action

      C_PUSH_EQUIPMENT_PICKUP,      // Client Pushes an Equipment Pickup from Character
      S_PUSH_EQUIPMENT_PICKUP,      // Server Pushes an Equipment Pickup from Character
                                    // Note: These aren't requests, the Server will notify other clients of the action

      C_PUSH_EQUIPMENT_EQUIP,       // Client Pushes an Equipment Equip from Character
      S_PUSH_EQUIPMENT_EQUIP,       // Server Pushes an Equipment Equip from Character
                                    // Note: These aren't requests, the Server will notify other clients of the action
     
      C_PUSH_EQUIPMENT_UNEQUIP,     // Client Pushes an Equipment Unequip from Character
      S_PUSH_EQUIPMENT_UNEQUIP,     // Server Pushes an Equipment Unequip from Character
                                    // Note: These aren't requests, the Server will notify other clients of the action

      C_PUSH_EQUIPMENT_USE,         // Client Pushes an Equipment Use to Server
      S_PUSH_EQUIPMENT_USE,         // Server Pushes an Equipment Use to clients
                                    // Note: These aren't requests, the Server will notify other clients of the action

      C_PUSH_EQUIPMENT_IDENTIFY,    // Client Pushes an Equipment Identify to Server
      S_PUSH_EQUIPMENT_IDENTIFY,    // Server Pushes an Equipment Identify to clients
                                    // Note: These aren't requests, the Server will notify other clients of the action

      C_PUSH_EQUIPMENT_ADDENCHANT,  // Client Pushes an Equipment Add Enchant to Server
      S_PUSH_EQUIPMENT_ADDENCHANT,  // Server Pushes an Equipment Add Enchant to clients
                                    // Note: These aren't requests, the Server will notify other clients of the action

      C_PUSH_EQUIPMENT_ADDSOCKET,   // Client Pushes an Equipment Add Socket to Server
      S_PUSH_EQUIPMENT_ADDSOCKET,   // Server Pushes an Equipment Add Socket to clients
                                    // Note: These aren't requests, the Server will notify other clients of the action

      C_PUSH_CHARACTER_SETDEST,     // Client Pushes a Character SetDestination to Server
      S_PUSH_CHARACTER_SETDEST,     // Server Pushes a Character SetDestination to Client
    };

  };

};