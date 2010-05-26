#pragma once

#include "CBaseUnit.h"

// Size?: 180h
// Inheritance: CBaseUnit
struct CItem : CBaseUnit
{
  PVOID pSoundBank;

  u32 unk102[9];

  CString name;

  u32 unk103[5];        //
};
