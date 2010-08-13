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

  extern vector<NetworkEntity*>* NetworkSharedEquipment;
  extern vector<NetworkEntity*>* NetworkSharedCharacters;

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
