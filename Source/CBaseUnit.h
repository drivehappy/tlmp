#pragma once

#include "CPositionableObject.h"

// Size?: 60h
// Inheritance: CPositionableObject
struct CBaseUnit : CPositionableObject
{
  u32 unk1[9];        //

  PVOID pCUnknownStruct2;     // ptr to CUnknownStruct2


  u32 unk2[8];        // See below
  /*
    1,   0,   0,   0
    1,   0,   0,   0
    1,   0,   0,   0
    0,   0,   0,   0
    0,   0,   0,   0
    0,   0,   0,   0
    1,   0,   0,   0
    0,   0,   0,   0
  */


  u32 unk3[6];        // See below
  /*
    0FFFFFFFFh
    0FFFFFFFFh
    0FFFFFFFFh
    0
    0FFFFFFFFh
    0FFFFFFFFh
  */

  u32 unk4[2];        // 0, 1000101

  float unk5[2];      // -1.460968, 0.8

  u32 unk6;           // 1010101

  u32 unk7;           // 340D5401h

  u64 GUIDUnk;        // 0F59522DA8B7A11DEh

  float unk8;         // 0.11084

  u32 unk9;           // 68h

  PVOID pCDataGroup;
  PVOID pCEffectManager;  // NULL
  PVOID pCCullingBounds;
  PVOID pCSkillManager;

  float unk12;        // 0.5
  u32 unk13;          // 1

};
