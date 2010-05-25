#pragma once

#include "_CUnknownStruct3.h"
#include "_CList.h"

#include "CRunicCore.h"

// Size?: 30h
// Note: Appears to be the base structure for all the GUID stuff
//       and additionally some ptrs
// Inheritance: CRunicCore
struct CEditorBaseObject : CRunicCore
{
  CList<CUnknownStruct3>    *pListUnk;

  u64 guid0;
  u64 guid1;
  u64 guid2;
  u64 guid3;
};
