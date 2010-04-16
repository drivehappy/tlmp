#pragma once

// Size: 200h
struct CCharacterSaveState
{
  PVOID vtable;
  u32 unk0;
  u64 guid0;  // 51D9A040C33711DEh
  u64 guid1;  // 81065BD0B793A300h
  u64 guid2;  // 0D3A8F9982FA111DEh
  u64 guid3;  // 0FFFFFFFFFFFFFFFFh
  u32 unk1[2];
  float unk2;
  float unk3;
  PVOID pStringCharacterName;   // ptr string "Drivehappy"
  u32 unk4;
  PVOID pUnknown;               // ptr unknown structure
  u32 unk5;
  u32 unk6[12];                 // 37c96440h, 0Ah, 0Fh, 1, 0, 4 dup (FFFFFFFFh), 302DBE98h, 1, 22h
  float unk7[14];               // 
  float unk8[5];                //

  u64 guid3;  // 6B0351E39E3311DEh
  u64 guid4;  // 6B0351E39E3311DEh
  u64 guid5;  // 0FFFFFFFFFFFFFFFFh

  // ... more
};
