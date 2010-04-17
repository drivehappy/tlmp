#pragma once

// Size?: 120h
struct CRoomPiece
{
  PVOID vtable;
  PVOID unk0;
  
  u64 guid0;    // 3F34310949D211DFh
  u64 guid1;    // 3F342D1049D211DFh
  u64 guid2;    // 0F61AAE04B9FC11DEh
  u64 guid3;    // 0

  PVOID unk1;
  PVOID pCRoomPieceDescriptor;

  float unk2;   // 0.14362749

  PVOID pStringFloor;   // ptr string "Floor_clone1561786"

  float unk3;   // 0.14362749
  float unk4;   // 1
  float unk5;   // 0.14362749

  u32 unk6[2];

  PVOID pCEditorScene;

  u32 unk7;

  PVOID pOctree0;
  PVOID pOgreShadowCaster;    // ogre
  PVOID pCResourceManager;
  PVOID unk8;
  PVOID pOctree1;

  float unk9[32];

  PVOID unk10;
  
  // ---------- Base class breakoff ----

  PVOID vtableISnap;
  PVOID vtableIHighlight;
  PVOID vtableICollision;

  PVOID pCCollisionModel;
  PVOID pCGenericModel;

  PVOID unk11;

  PVOID pStringMediaLevelset; // ptr string = "media/levelsets/props/mine_props/mine_pulley_support/mine_pulley_support.mesh"

  u32 unk12[5];
};
