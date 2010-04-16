#pragma once

struct CResourceManager
{
  PVOID vtable;
  u32 unk0;
  PVOID pOctreeSM;      // ptr to Octree scenemanager
  PVOID pCLevel;        // ptr to CLevel
  PVOID pCHierarchy;    // ptr to CHierarchy
  PVOID* ppCGameClient; // ptr ptr to CGameClient
  u32 unk1[4];

};
