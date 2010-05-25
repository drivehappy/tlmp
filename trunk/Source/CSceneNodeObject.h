#pragma once

#include "CEditorBaseObject.h"
#include "_CString.h"

// Size?: 5Ch
// Inheritance: CEditorBaseObject
struct CSceneNodeObject : CEditorBaseObject
{
  PVOID pAnimationTrack;        // ptr to Ogre Animation Track
  PVOID pCParticleTechWrapper;  // ptr to Particle Tech Wrapper

  PVOID unk0;

  CString name;        // ptr string "Base Glow", Also contains CPlayer's class name
  
  //u32 unk4[5];

  //PVOID pCLayout;               // ptr to CLayout
  //PVOID pCLayout;               // ptr to CLayout (same layout as above)
  PVOID pCEditorScene0;
  PVOID pCEditorScene1;

  PVOID pSharedMaterialPtr;     // ptr to Ogre shared mat ptr

  PVOID pOctreeNode0;
  PVOID pUnk5;                  // NULL
  PVOID pCResourceManager;
  PVOID pOctreeNode1;

  PVOID pOctreeSM;              // ptr to Octree SM
};
