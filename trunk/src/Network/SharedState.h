#pragma once

#include "Common.h"

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

  extern vector<NetworkEntity*>* NetworkSharedEquipment;
  extern vector<NetworkEntity*>* NetworkSharedCharacters;
  extern vector<NetworkEntity*>* ClientTemporaryEquipment;
  extern map<SystemAddress, vector<NetworkEntity*>*>*  Server_ClientCharacterMapping;
  extern vector<NetworkEntity*>* ServerEquipmentOnGround;

  //
  // Client Temprorary Equipment helpers
  static NetworkEntity* searchEquipmentClientByInternalObject(PVOID internalObject) {
    vector<NetworkEntity*>::iterator itr;
    for (itr = ClientTemporaryEquipment->begin(); itr != ClientTemporaryEquipment->end(); itr++) {
      if ((*itr)->getInternalObject() == internalObject) {
        return (*itr);
      }
    }

    return NULL;
  };

  static NetworkEntity* addEquipmentClientTemporary(PVOID equipment) {
    NetworkEntity *entity = searchEquipmentClientByInternalObject(equipment);

    if (!entity) {
      NetworkEntity *newEntity = new NetworkEntity(equipment);
      ClientTemporaryEquipment->push_back(newEntity);

      return newEntity;
    }
    return entity;
  }

  static NetworkEntity* searchEquipmentByClientID(u32 clientId) {
    vector<NetworkEntity*>::iterator itr;
    for (itr = ClientTemporaryEquipment->begin(); itr != ClientTemporaryEquipment->end(); itr++) {
      if ((*itr)->getCommonId() == clientId) {
        return (*itr);
      }
    }

    return NULL;
  };

  static void removeEquipmentByClientID(u32 client_id) {
    vector<NetworkEntity*>::iterator itr;
    for (itr = ClientTemporaryEquipment->begin(); itr != ClientTemporaryEquipment->end(); itr++) {
      if ((*itr)->getCommonId() == client_id) {
        ClientTemporaryEquipment->erase(itr);
        break;
      }
    }
  }

  //
  // Equipment helpers
  static NetworkEntity* searchEquipmentByInternalObject(PVOID internalObject) {
    vector<NetworkEntity*>::iterator itr;
    for (itr = NetworkSharedEquipment->begin(); itr != NetworkSharedEquipment->end(); itr++) {
      if ((*itr)->getInternalObject() == internalObject) {
        return (*itr);
      }
    }

    return NULL;
  };

  static NetworkEntity* searchEquipmentByCommonID(u32 commondId) {
    vector<NetworkEntity*>::iterator itr;
    for (itr = NetworkSharedEquipment->begin(); itr != NetworkSharedEquipment->end(); itr++) {
      if ((*itr)->getCommonId() == commondId) {
        return (*itr);
      }
    }

    return NULL;
  };

  static NetworkEntity* addEquipment(PVOID equipment) {
    NetworkEntity *entity = searchEquipmentByInternalObject(equipment);

    if (!entity) {
      NetworkEntity *newEntity = new NetworkEntity(equipment);
      NetworkSharedEquipment->push_back(newEntity);

      return newEntity;
    }
    return entity;
  }

  static NetworkEntity* addEquipment(PVOID equipment, u32 commonId) {
    NetworkEntity *entity = searchEquipmentByInternalObject(equipment);

    if (!entity) {
      NetworkEntity *newEntity = new NetworkEntity(equipment, commonId);
      NetworkSharedEquipment->push_back(newEntity);

      return newEntity;
    }
    return entity;
  }


  //
  // Character helpers
  static NetworkEntity* searchCharacterByInternalObject(PVOID internalObject) {
    vector<NetworkEntity*>::iterator itr;
    for (itr = NetworkSharedCharacters->begin(); itr != NetworkSharedCharacters->end(); itr++) {
      if ((*itr)->getInternalObject() == internalObject) {
        return (*itr);
      }
    }

    return NULL;
  };

  static NetworkEntity* searchCharacterByCommonID(u32 commondId) {
    vector<NetworkEntity*>::iterator itr;
    for (itr = NetworkSharedCharacters->begin(); itr != NetworkSharedCharacters->end(); itr++) {
      if ((*itr)->getCommonId() == commondId) {
        return (*itr);
      }
    }

    return NULL;
  };

  // Create a network ID to identify this character later
  static NetworkEntity* addCharacter(PVOID character) {
    NetworkEntity *entity = searchCharacterByInternalObject(character);

    if (!entity) {
      NetworkEntity *newEntity = new NetworkEntity(character);
      NetworkSharedCharacters->push_back(newEntity);

      return newEntity;
    }
    return entity;
  }

  // Create a network ID to identify this character later
  static NetworkEntity* addCharacter(PVOID character, u32 commonId) {
    NetworkEntity *entity = searchCharacterByInternalObject(character);

    if (!entity) {
      NetworkEntity *newEntity = new NetworkEntity(character, commonId);
      NetworkSharedCharacters->push_back(newEntity);

      return newEntity;
    }
    return entity;
  }

};
