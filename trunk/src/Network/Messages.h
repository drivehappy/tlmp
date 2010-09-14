#pragma once

#include "DataTypes.h"

#include <map>
using std::map;

#define ENUMSTR(enumeration) "##enumeration##"

namespace TLMP {

  namespace Network {

    #define stringify(name) #name

    /** Version to signify any Messaging ID changes. */
    const u32 MessageVersion = 4;

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

      S_PUSH_CURRENT_LEVEL,         // Server pushes the exact level for the client to load

      S_REQUEST_CHARINFO,           // Server Requests Client for Character Information (Name, Inventory, Class, Minions, etc.)
      C_REPLY_CHARINFO,             // Client Respone for Character Information Request
      S_REPLY_CHARID,               // Server Respone for setting Character Network ID

      C_PUSH_EQUIPMENT,             // Client Pushes it's Suppressed Player's Equipment to server
      S_REPLY_EQUIPMENT_ID,         // Server Respone to client equipment with the common ID

      S_PUSH_NEWCHAR,               // Server Pushes a New Character to the Client
      S_PUSH_NEWEQUIPMENT,          // Server Pushes a New Equipment

      S_PUSH_ADDCHARMINION,         // Server Pushes a Minion setup to Client

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

      C_REQUEST_EQUIPMENT_IDENTIFY, // Client Requests an Equipment Identify to Server
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

      C_PUSH_CHARACTER_ACTION,      // Client Pushes a Character Action
      S_PUSH_CHARACTER_ACTION,      // Server Pushes a Character Action

      C_PUSH_CHARACTER_ATTACK,      // Client Pushes a Character Attack
      S_PUSH_CHARACTER_ATTACK,      // Server Pushes a Character Attack

      C_REQUEST_CHARACTER_USESKILL, // Client requests a specific character uses a skill
      S_PUSH_CHARACTER_USESKILL,    // Server Pushes a Character Use Skill

      C_PUSH_EQUIPMENT_STACKUPDATE, // Client Pushes an update on Equipment Stack size
      S_PUSH_EQUIPMENT_STACKUPDATE, // Server Pushes an update on Equipment Stack size

      C_PUSH_EQUIPMENT_ADD_GEM,     // Client pushes a gem attach to equipment
      S_PUSH_EQUIPMENT_ADD_GEM,     // Server pushes a gem attach to equipment
            
      C_PUSH_EQUIPMENT_REMOVE_GEMS, // Client pushes equipment remove all gems
      S_PUSH_EQUIPMENT_REMOVE_GEMS, // Server pushes equipment remove all gems

      C_PUSH_CHAT_PLAYER,           // Client pushes chat message out
      S_PUSH_CHAT_PLAYER,           // Server pushes chat message out

      S_PUSH_CHAR_PLAYER_JOINED,    // Server pushes player joined message

      S_PUSH_CHANGE_LEVEL,          // Server pushes a change level

      S_PUSH_CHAR_UPDATE_HEALTH,    // Server pushes a health update message

      S_PUSH_CHAR_DESTROY,          // Server pushes a character destroy
      
      S_PUSH_LEVEL_CREATE_ITEM,     // Push Item creation (not Equipment)

      S_PUSH_LEVEL_ITEM_DROP,       // Push Item drop on level

      C_REQUEST_BREAKABLE_TRIGGERED,// Client requests that a breakable be triggered
      S_PUSH_BREAKABLE_TRIGGERED,   // Server pushes that a breakable triggered
      
      C_REQUEST_TRIGGER_TRIGGERED,  // Client requests that a breakable be triggered
      S_PUSH_TRIGGER_TRIGGERED,     // Server pushes that a breakable triggered

      S_PUSH_ITEM_GOLD,             // Gold amount after the item is created

      S_PUSH_CHARACTER_ALIGNMENT,   // Server pushes character alignment

      C_REQUEST_CHARACTER_SETTARGET,// Client request the character set's the target
      S_PUSH_CHARACTER_SETTARGET,   // Server pushes the character set target

      S_PUSH_TRIGGERUNIT_SYNC,      // Server pushes trigger unit IDs and names to sync
    };

    static const char* MessageString[] = {
      "S_VERSION",
      "C_VERSION",

      "C_REQUEST_HASGAMESTARTED",
      "S_REPLY_HASGAMESTARTED",

      "S_PUSH_GAMESTARTED",
      "S_PUSH_GAMEENDED",

      "C_PUSH_GAMEENTER",
      "C_PUSH_GAMEEXITED",

      "S_PUSH_CURRENT_LEVEL",

      "S_REQUEST_CHARINFO",
      "C_REPLY_CHARINFO",
      "S_REPLY_CHARID",

      "C_PUSH_EQUIPMENT",
      "S_REPLY_EQUIPMENT_ID",

      "S_PUSH_NEWCHAR",
      "S_PUSH_NEWEQUIPMENT",

      "S_PUSH_ADDCHARMINION",

      "C_PUSH_EQUIPMENT_DROP",
      "S_PUSH_EQUIPMENT_DROP",
      
      "C_PUSH_EQUIPMENT_PICKUP",
      "S_PUSH_EQUIPMENT_PICKUP",

      "C_PUSH_EQUIPMENT_EQUIP",
      "S_PUSH_EQUIPMENT_EQUIP",
      
      "C_PUSH_EQUIPMENT_UNEQUIP",
      "S_PUSH_EQUIPMENT_UNEQUIP",
      
      "C_PUSH_EQUIPMENT_USE",
      "S_PUSH_EQUIPMENT_USE",
            
      "C_REQUEST_EQUIPMENT_IDENTIFY",
      "S_PUSH_EQUIPMENT_IDENTIFY",
            
      "C_PUSH_EQUIPMENT_ADDENCHANT",
      "S_PUSH_EQUIPMENT_ADDENCHANT",
                  
      "C_PUSH_EQUIPMENT_ADDSOCKET",
      "S_PUSH_EQUIPMENT_ADDSOCKET",
                  
      "C_PUSH_CHARACTER_SETDEST",
      "S_PUSH_CHARACTER_SETDEST",

      "C_PUSH_CHARACTER_ACTION",
      "S_PUSH_CHARACTER_ACTION",
      
      "C_PUSH_CHARACTER_ATTACK",
      "S_PUSH_CHARACTER_ATTACK",
      
      "C_REQUEST_CHARACTER_USESKILL",
      "S_PUSH_CHARACTER_USESKILL",

      "C_PUSH_EQUIPMENT_STACKUPDATE",
      "S_PUSH_EQUIPMENT_STACKUPDATE",

      "C_PUSH_EQUIPMENT_ADD_GEM",
      "S_PUSH_EQUIPMENT_ADD_GEM",
      
      "C_PUSH_EQUIPMENT_REMOVE_GEMS",
      "S_PUSH_EQUIPMENT_REMOVE_GEMS",

      "C_PUSH_CHAT_PLAYER",
      "S_PUSH_CHAT_PLAYER",

      "S_PUSH_CHAR_PLAYER_JOINED",

      "S_PUSH_CHANGE_LEVEL",

      "S_PUSH_CHAR_UPDATE_HEALTH",
      
      "S_PUSH_CHAR_DESTROY",

      "S_PUSH_LEVEL_CREATE_ITEM",

      "S_PUSH_LEVEL_ITEM_DROP",

      "C_REQUEST_BREAKABLE_TRIGGERED",
      "S_PUSH_BREAKABLE_TRIGGERED",

      "C_REQUEST_TRIGGER_TRIGGERED",
      "S_PUSH_TRIGGER_TRIGGERED",

      "S_PUSH_ITEM_GOLD",
      
      "S_PUSH_CHARACTER_ALIGNMENT",

      "C_REQUEST_CHARACTER_SETTARGET",
      "S_PUSH_CHARACTER_SETTARGET",

      "S_PUSH_TRIGGERUNIT_SYNC",
    };

  };

};
