#pragma once

#include "_CString.h"

struct CRecipes : CRunicCore
{
  PVOID unk0;

  PVOID unk1;       // ptr to unknown structure - probably some recipe structure
  CString location; // "media/recipes/"

  // vector< unk * >  ptr to unk recipe structure
  vector<PVOID> *pRecipes;

  u32 unk2;         // 46CC1340h
  u32 unk3;         // 4Bh      -- noticed this alot, is this a compiler signal for end of class?
};
