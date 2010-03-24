#pragma once

#include "DataTypes.h"

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

    int getCommonId() const  { return m_iCommonId; };
    int getInternalId() const { return m_iInternalId; };
    PVOID getInternalObject() const { return m_pInternalObject; };

    static int generateUniqueId() {
      return m_iUniqueIdGenerator++;
    };

  };

};
