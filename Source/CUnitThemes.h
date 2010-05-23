#pragma once

#include "CRunicCore.h"
#include "CUnitTheme.h"
#include "_CString.h"

struct CUnitThemes : CRunicCore
{
  PVOID unk0;   // 0
  
  CUnitTheme   *pUnitTheme0;
  CString       pathLocation;

  // vector<CUnitTheme *> ??
  // ptr to list of CUnitTheme*, after ptr is size?
  // check this, I don't know how vector types look in layout
  vector<CUnitTheme *>  *pThemes;
};
