#pragma once

#include "DataTypes.h"

#include <vector>
using std::vector;

namespace TLMP {

  /** Provides IDs for objects between two instances of Torchlight.
      @remarks
        Allows for network communication to act upon common objects.
  */
  class NetworkEntity {

  private:
    static int m_iUniqueIdGenerator;
    int m_iCommonId;
    int m_iInternalId;
    PVOID m_pInternalObject;

    int findFirstAvailableCommonId(vector<NetworkEntity *> *entityList, int baseCommonId) const {
      const int maxRange = 1000;
      vector<NetworkEntity *>::iterator itr;

      // If the list hasn't be initialized, assume the ID is ok
      if (!entityList) {
        return baseCommonId;
      }

      // Search the range of common IDs
      for (int i = baseCommonId; i < baseCommonId + maxRange; i++) {
        bool available = true;

        // Search the entity list for collisions
        for (itr = entityList->begin(); itr != entityList->end(); itr++) {
          if ((*itr)->getCommonId() == i) {
            available = false;
            break;
          }
        }

        // We found one
        if (available) {
          return i;
        }
      }

      // Couldn't find a suitable id within the range, return default
      return -1;
    };

  public:
    NetworkEntity() {
      m_iCommonId = generateUniqueId();
      m_iInternalId = -1;
      m_pInternalObject = NULL;
    };

    NetworkEntity(PVOID internalObject) {
      m_iCommonId = generateUniqueId();
      m_iInternalId = (int)internalObject;
      m_pInternalObject = internalObject;
    };

    NetworkEntity(PVOID internalObject, int commonId) {
      m_iCommonId = commonId;
      m_iInternalId = (int)internalObject;
      m_pInternalObject = internalObject;
    };

    NetworkEntity(PVOID internalObject, int baseCommonId, vector<NetworkEntity *> *entityList) {
      m_iCommonId = findFirstAvailableCommonId(entityList, baseCommonId);
      m_iInternalId = (int)internalObject;
      m_pInternalObject = internalObject;
    };

    void SetNewInternalObject(PVOID internalObject) {
      m_pInternalObject = internalObject;
    }

    int getCommonId() const  { return m_iCommonId; };
    int getInternalId() const { return m_iInternalId; };
    PVOID getInternalObject() const { return m_pInternalObject; };

    static int generateUniqueId() {
      return m_iUniqueIdGenerator++;
    };

  };

};
