#pragma once

#include "CCameraControl.h"

// Size?: 
// 
struct CGameClient : CRunicCore
{
  PVOID unk0;

  PVOID vtableOgre_RenderTargetListener;
  PVOID vtableOgre_RenderQueue_RenderableListener;

  CCameraControl     *pCCameraControl;

};
