#pragma once

// Inherits CBaseUnit
struct CCharacter
{
  PVOID vtable;
  PVOID pUnknownStruct;
  u64 guid0;
  u64 guid1;
  u64 guid2;

  u32 unk0[3];        // 0, 0, 0
  PVOID pCMonsterDescriptor;      // ptr to CMonsterDescriptor

  u32 unk1;

  // Unicode string
  u32 stringStuff[6];

  PVOID pCEditorScene;            // ptr to CEditorScene
  PVOID pCEditorScene;            // ptr to CEditorScene  (same val as above)
  u32 unk2;

  PVOID pOctreeSM;                // ptr to Octree
  u32 unk3;

  PVOID pCResourceManager;        // ptr to CResourceManager
  u32 unk4;

  PVOID pOctreeSM2;               // ptr to Octree2
  u32 unk5;

  float unk6[9];                  // -2.13, 0, 1.5, 1.05075, 1.05075, 1.05075, 0.0078125, 1.0, 0

  // ... more
};
