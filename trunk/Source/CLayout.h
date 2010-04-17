#pragma once

// Size?: 1A8h, base class data: 168h
struct CLayout
{
  PVOID vtable;
  u32 unk0;
  u64 guid0;      // 98E21BE849C411DFh
  u64 guid1;      // 0FFFFFFFFFFFFFFFFh
  u64 guid2;      // 98E21BE849C411DFh
  u64 guid3;      // 0h

  PVOID unk1;     // ptr to unk struct
  u32 unk2;       // 0
  u32 unk3;       // 94916B4Eh

  wchar_t name[8];     // Unicode "Layout"

  u32 nameLength;
  u32 nameMaxLength;  // maybe

  u32 unk4[2];

  u64 unk5;
  u32 unk6;

  PVOID pCResourceManager;
  PVOID pOctree0;
  PVOID pOctree1;

  // .. bunch of floats...
};
