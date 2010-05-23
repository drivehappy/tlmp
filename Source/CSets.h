#pragma once

#include "_CString.h"

struct CSets : CRunicCore
{
  PVOID unk0;
  PVOID unk1;

  CString location;   // "media/sets/"

  // vector< unk * >  ptr to unknown structure
  vector<PVOID>   *pSets;

  // Unk rest
};
