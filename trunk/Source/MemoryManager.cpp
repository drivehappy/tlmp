#include "MemoryManager.h"

MemoryManager* MemoryManager::m_pSingleton = NULL;
vector<MemoryBlock>* MemoryManager::m_vBlocks = NULL;


MemoryManager& MemoryManager::getSingleton()
{
  if (!m_pSingleton)
    m_pSingleton = new MemoryManager();
  return *m_pSingleton;
}

MemoryManager* MemoryManager::getSingletonPtr()
{
  if (!m_pSingleton)
    m_pSingleton = new MemoryManager();
  return m_pSingleton;
}



void MemoryManager::addBlock(PVOID addr, u32 size, PVOID retInst)
{
  m_vBlocks->push_back(MemoryBlock(addr, size, retInst));
}

void MemoryManager::removeBlock(PVOID addr)
{
  vector<MemoryBlock>::iterator itr;

  for (itr = m_vBlocks->begin(); itr != m_vBlocks->end(); itr++) {
    if ((*itr).pMemory == addr) {
      m_vBlocks->erase(itr);
      break;
    }
  }
}

u32 MemoryManager::getMemorySize(PVOID addr)
{
  vector<MemoryBlock>::iterator itr;

  for (itr = m_vBlocks->begin(); itr != m_vBlocks->end(); itr++) {
    if ((*itr).pMemory == addr) {
      return (*itr).nSize;
    }
  }

  return -1;
}

PVOID MemoryManager::getMemoryAllocationAddress(PVOID addr)
{
  vector<MemoryBlock>::iterator itr;

  for (itr = m_vBlocks->begin(); itr != m_vBlocks->end(); itr++) {
    if ((*itr).pMemory == addr) {
      return (*itr).pRetInst;
    }
  }

  return NULL;
}

void MemoryManager::reportMemory()
{
  vector<MemoryBlock>::iterator itr;

  printf("Memory Dump:\n");
  for (itr = m_vBlocks->begin(); itr != m_vBlocks->end(); itr++) {
    printf("Block @%p | Size: %i\n", (*itr).pMemory, (*itr).nSize);
  }
}


MemoryManager::MemoryManager()
{
  m_vBlocks = new vector<MemoryBlock>(50000);
}

MemoryManager::~MemoryManager()
{
  delete m_vBlocks;
}

MemoryManager::MemoryManager(const MemoryManager &)
{
}

MemoryManager& MemoryManager::operator=(const MemoryManager &)
{
}

