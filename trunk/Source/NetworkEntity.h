#pragma once

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
    

  public:
    NetworkEntity() {
      m_iCommonId = generateUniqueId();
      m_iInternalId = -1;
    };

    NetworkEntity(int internalId) {
      m_iCommonId = generateUniqueId();
      m_iInternalId = internalId;
    };

    NetworkEntity(int internalId, int commonId) {
      m_iCommonId = commonId;
      m_iInternalId = internalId;
    };

    int getCommondId() const  { return m_iCommonId; };
    int getInternalId() const { return m_iInternalId; };

    static int generateUniqueId() {
      return m_iUniqueIdGenerator++;
    };

  };

};
