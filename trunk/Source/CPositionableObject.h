#pragma once

#include "CSceneNodeObject.h"

// Size: 68h
// Inheritance: CSceneNodeObject
struct CPositionableObject : CSceneNodeObject
{
  u32 unk10;

  float unk11[31];

  PVOID pSharedPtrMaterial;     // ogre
};
