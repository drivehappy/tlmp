#pragma once

// Inherits from CEditorBaseObject
struct CSceneNodeObject
{
  PVOID vtable;
  u32 unk0;
  u64 guid0;
  u64 guid1;
  u64 guid2;

  u32 unk1;
  u32 unk2;
  PVOID pAnimatinoTrack;        // ptr to Ogre Animation Track
  PVOID pCParticleTechWrapper;  // ptr to Particle Tech Wrapper
  u32 unk3;
  PVOID pStringBaseGlow;        // ptr string "Base Glow"
  
  u32 unk4[5];

  PVOID pCLayout;               // ptr to CLayout
  PVOID pCLayout;               // ptr to CLayout (same layout as above)
  PVOID pSharedMaterialPtr;     // ptr to Ogre shared mat ptr

  u32 unk5[4];

  PVOID pOctreeSM;              // ptr to Octree SM
};
