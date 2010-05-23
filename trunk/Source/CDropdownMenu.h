#pragma once

// Inherits from COptionsMenu
struct CDropdownMenu : CRunicCore
{
  u32 unk0;
  PVOID pGUISheet;
  PVOID pGUISheet;
  PVOID pGUISheet;
  u32 unk1;                 // 0
  u32 unk2;                 // 100h
  PVOID pCSettings;         // ptr to CSettings
  PVOID pCGameUI;           // ptr to CGameUI
  PVOID pOctree;
  PVOID unk3;               // ptr to Unknown struct
  u32 unk4;                 // 0
  u32 unk5;                 // 0Fh
  u32 unk6;                 // 0
  PVOID pCResourceManager;  // ptr to CResourceManager
  PVOID pCSoundBank;        // ptr to CSoundBank
};
