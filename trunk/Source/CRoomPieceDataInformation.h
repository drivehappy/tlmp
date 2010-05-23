#pragma once

#include "CResourceSettings.h"

struct CRoomPieceDataInformation : CRunicCore
{
  PVOID unk0;

  typedef struct CRoomPieceCoreData {
    // Unk
  } *pRoomPieceCoredData;

  CResourceSettings   *pResourceSettings;

  PVOID *ptrtomyself;       // cyclic ptr to myself
};
