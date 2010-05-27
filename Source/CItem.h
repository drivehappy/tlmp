#pragma once

#include "CBaseUnit.h"

// Size?: 180h
// Inheritance: CBaseUnit
struct CItem : CBaseUnit
{
  PVOID pSoundBank;

  u32 unk102[6];

  CString name;

  u32 unk103[5];        //


  // Dump item info
  void dumpItem() {
    log("CItem Dump: %p", this);
    log(L"  Name: %s", name.getString());
    log("  SoundBank: %p", pSoundBank);

    for (int i = 0; i < 6; i++) {
      log("  unk102[%i] = %i (%p)", i, unk102[i], unk102[i]);
    }
    for (int i = 0; i < 5; i++) {
      log("  unk103[%i] = %i (%p)", i, unk103[i], unk103[i]);
    }
  };
};
