#pragma once

// Size?: 180h
// Note: Similar name structure to CItemSaveState
// Inheritance: CBaseUnit
struct _CItem
{
  PVOID vtable;
  PVOID unk0;
  u64 guid0;      // 88F1C7449B211DFh
  u64 guid1;      // 0FFFFFFFFFFFFFFFFh
  u64 guid2;      // 19463565308B11DEh
  u64 guid3;      // 0F61AC062B9FC11DEh

  PVOID pAnimationTrack;            // ogre
  PVOID pCUnitTriggerDescriptor;    // CUnitTriggerDescriptor

  u32 unk2;       // 38h

  // UNICODE String ie "4 Gold" in place (16 bytes)
  // OR I noticed a ptr string to "TNT Barrel"
  // This appears to be that if the string is > 7 in length then it uses
  // a ptr in place of the unicode data (union)
  // Burn off the 16 bytes for now
  union {
    wchar_t name[8];    // In-place unicode data
    PVOID namePtr[4];   // First is the unicode ptr, the rest are unk
  };

  u32 nameLength;
  u32 maxNameLength;    // Guess, double check this
  // --

  PVOID pCEditorScene0; // ptr to CEditorScene
  PVOID pCEditorScene1; // ptr to CEditorScene (same as above)

  float unk3;

  PVOID pOctree0;
  u32 unk4;             // 0

  PVOID pCResourceManager;
  u32 unk5;             // 0

  PVOID pOctree1;

  // ---- Possible inheritance breakoff: CPositionableObject ---

  u32 unk6;             // 0   another - 10000

  /// ----

  float unk91[3];       // 33.973099, 0.17998, 98.664

  float unk92[28];      //

  u32 unk93[24];        //

  u32 unk94;            // 1000001

  float unk95[2];       // 83.501953, 0.3
  
  u32 unk96;            // 10100h

  float unk97;          // 1.0

  u64 GUIDUnk;          // 8D3EE5363F7611DEh

  float unk98;          // 0.97267

  u32 unk99;            // 20h

  PVOID pCDataGroup;
  PVOID pCEffectManager;           // NULL
  PVOID pCCullingBounds;
  PVOID pCSkillManager;
  
  PVOID unk100;
  u32 unk101;

  PVOID pSoundBank;

  u32 unk102[9];

  // These two look like it might be the usual string structure
  PVOID pStringName;    // "Grand Mana Potion"
  u32 stringPart[5];

  u32 unk103[5];        //


  // CUTOFF THE ABOVE STRUCTURE SIZE SHOULD BE CORRECT
};
