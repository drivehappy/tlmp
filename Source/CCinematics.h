#pragma once

#include "_CString.h"

struct CCinematics : CRunicCore
{
  PVOID unk0;
  PVOID unk1;

  CString location;   // "media/cinematics/"

  // vector< unk * >   ptr to unk structure
  vector<PVOID>     *pCinematics;

  // Unk rest
};