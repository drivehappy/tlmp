#pragma once

#include "_CString.h"

// Size: 68h
// Inheritance: CSceneNodeObject
struct CPositionableObject
{
  u32 unk0;
  u64 guid0;  // 3DAE0D294AA311DFh
  u64 guid1;  // 0FFFFFFFFFFFFFFFFh
  u64 guid2;  // 0CB7572EABB7F11DEh
  u64 guid3;  // 0h

  PVOID unk1;
  PVOID pCRandomGroupDescriptor;

  u32 unk2;   // 0

  CString pCString0;

  // Ditto structure from somewhere

  PVOID pCEditorScene0;
  PVOID pCEditorScene1;
  float unk3; // 4.0
  PVOID pOctree0;
  float unk4; // 0.0
  PVOID pCResourceManager;
  float unk5; // 0.0
  PVOID pOctree1;

  /* OLD STUFF - IS THIS AT ALL CORRECT?
  u32 unk1;
  PVOID pCUnitTriggerDescriptor;    // ptr to CUnitTriggerDescriptor
  u32 unk2;
  PVOID pStringUnitTrigger;         // string ptr to "Unit Trigger4"
  u32 unk3[5];
  PVOID pCEditorScene[2];           // two ptrs to same CEditorScene
  u32 unk4;
  PVOID pOctree;                    // ptr to ogre octree
  u32 unk5;
  PVOID pCResourceManager;          // ptr to CResourceManager
  u32 unk6;
  PVOID pOctree;                    // ptr to ogre octree
  u32 unk7[4];                      // values: 0x10000, 0,0,0

  // .. some floats maybe?
  */

};
