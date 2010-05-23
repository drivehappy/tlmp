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

  PVOID               pOctreeSM0;
  PVOID               pOctreeSM1;
  PVOID               pOctreeSM2;
  PVOID               pOctreeSM3;
  PVOID               pOctreeSM4;

  CSoundManager      *pCSoundManager;
  CSettings          *pCSettings;
  CPlayer            *pCPlayer;

  PVOID unk1;       // NULL

  CQuestManager      *pCQuestManager;
  CLevel             *pCLevel;
  CGameUI            *pCGameUI;

  
};
