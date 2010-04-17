#pragma once

// Size?: 44h
struct CWardrobe
{
  PVOID vtable;
  u32 unk0;

  PVOID pOctree0;

  PVOID pCGenericModel;

  PVOID unk1;

  PVOID pShadowCaster;    // ogre

  PVOID unk2;

  // UNICODE --
  union {
    wchar_t name[8];    // In-place unicode data
    PVOID namePtr[4];   // First is the unicode ptr, the rest are unk
  };

  u32 nameLength;
  u32 maxNameLength;    // Guess, double check this
  // --


  PVOID pTexturePtr;    // ogre
  PVOID unk3;
  PVOID unk4;

  PVOID unk5;           // NULL
};
