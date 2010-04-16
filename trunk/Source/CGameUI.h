#pragma once

struct CGameUI
{
  PVOID unk0;
  PVOID pOctreeSM[4];     // 4 ptrs to Octrees
  PVOID pCSoundManager;   // ptr to CSoundManager
  PVOID pCPlayer;         // ptr to CPlayer
};
