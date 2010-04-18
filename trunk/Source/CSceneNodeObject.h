#pragma once

// Size?: 5Ch
// Inheritance: CEditorBaseObject
struct CSceneNodeObject
{
  // Base Class Members: CEditorBaseObject
  PVOID vtable;
  u32 unk0;
  u64 guid0;
  u64 guid1;
  u64 guid2;
  u64 guid3;

  // My data members
  PVOID pAnimationTrack;        // ptr to Ogre Animation Track
  PVOID pCParticleTechWrapper;  // ptr to Particle Tech Wrapper
  u32 unk3;
  PVOID pStringBaseGlow;        // ptr string "Base Glow", Also contains CPlayer's class name
  
  u32 unk4[5];

  PVOID pCLayout;               // ptr to CLayout
  PVOID pCLayout;               // ptr to CLayout (same layout as above)
  PVOID pSharedMaterialPtr;     // ptr to Ogre shared mat ptr

  PVOID pOctreeNode0;
  PVOID pUnk5;                  // NULL
  PVOID pCResourceManager;
  PVOID pOctreeNode1;

  PVOID pOctreeSM;              // ptr to Octree SM
};
