#pragma once

struct CRandomizer : CRunicCore
{
  CRandomizer     *pCRandomizer;

  vector<PVOID>   *pUnkList2;
  vector<PVOID>   *pUnkList3;

  u32 unk4[7];

  CDataGroup              *pCDataGroup;
  vector<CDataGroup *>    *pCDataGroupList;

};
