#pragma once

// Size?: F0h
// Inheritance: CPositionalObject, CSceneNodeObject
struct CAffectorWrapper
{
  PVOID unk0;     // not vtable, points to TextureAnimateWrapper? ISelected

  // CEditorBaseObject
  u32 unk0;
  u64 guid0;
  u64 guid1;
  u64 guid2;
  u64 guid3;

  // 
  float unk1;     // 0.42261505

  PVOID pCRotateTextureDescriptor;

  float unk2;     // 0.906308

  // UNICODE      "Texture Rotate2_clone2"
  union {
    wchar_t name[8];    // In-place unicode data
    PVOID namePtr[4];   // First is the unicode ptr, the rest are unk
  };

  u32 nameLength;
  u32 maxNameLength;    // Guess, double check this
  // --

  PVOID pCEditorScene0;
  PVOID pCEditorScene1; // Same val as above as usual

  u32 unk3[3];

  PVOID pCResourceManager;

  u32 unk4;

  PVOID pOctree0;

  u32 unk5;       // 22000000h

  float unk6[31];

  PVOID unk7;     // 
};
