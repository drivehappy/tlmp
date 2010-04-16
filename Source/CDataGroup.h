#pragma once

#include "CDataValue.h"

struct CDataGroup
{
  PVOID vtable;
  u32 unk0;
  u32 unk1;             // 3211h
  PVOID unk2;
  PVOID pCDataValues;   // Probably: vector<CDataValue*>*
};
