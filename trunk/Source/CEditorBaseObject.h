#pragma once

#include "CRunicCore.h"

// Size?: 30h
// Note: Appears to be the base structure for all the GUID stuff
//       and additionally some ptrs
// Inheritance: CRunicCore
struct CEditorBaseObject : CRunicCore
{
  PVOID unk0;
  u64 guid0;
  u64 guid1;
  u64 guid2;
  u64 guid3;
};
