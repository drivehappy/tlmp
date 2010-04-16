#pragma once

// This isn't the CRunicCore structure, something else, maybe EditorBase? or PositionalObject?
struct CRunicCore
{
  PVOID vtable;
  u32 unk0;
  u64 guid0;
  u64 unk1;
  u64 guid1;
  u32 unk2[4];
};
