#pragma once

#include "CBaseUnit.h"

// Size?: 180h
// Inheritance: CBaseUnit
struct CItem //: CBaseUnit
{
  PVOID pSoundBank;

  u32 unk102[9];

  CString name;
  /*
  // These two look like it might be the usual string structure
  PVOID pStringName;    // "Grand Mana Potion"
  u32 stringPart[5];
  */

  u32 unk103[5];        //

  // CUTOFF THE ABOVE STRUCTURE SIZE SHOULD BE CORRECT
};
