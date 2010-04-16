#pragma once

struct CPositionalObject
{
  u32 unk0;
  u64 guid0;  // 0D5CFD651499D11DFh
  u64 guid1;  // 0FFFFFFFFFFFFFFFFh
  u64 guid2;  // 19463565308B11DEh
  u64 guid3;  // 0F61AC062B9FC11DEh
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
};
