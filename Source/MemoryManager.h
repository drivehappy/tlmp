#pragma once

#include "DataTypes.h"

#include <vector>
using std::vector;


struct MemoryBlock 
{
  MemoryBlock() : pMemory(NULL), nSize(0) {}
  MemoryBlock(PVOID addr, u32 size) : pMemory(addr), nSize(size) {}

  PVOID pMemory;
  u32 nSize;
};

class MemoryManager
{
private:
  static MemoryManager* m_pSingleton;
  static vector<MemoryBlock>* m_vBlocks;

public:
  static MemoryManager& getSingleton();
  static MemoryManager* getSingletonPtr();

  static void addBlock(PVOID addr, u32 size);
  static void removeBlock(PVOID addr);

  static u32 getMemorySize(PVOID addr);

  static void reportMemory();

  ~MemoryManager();

protected:
  MemoryManager();
  MemoryManager(const MemoryManager&);
  MemoryManager& operator=(const MemoryManager&);
  //~MemoryManager();

};
