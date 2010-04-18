#pragma once

// Inheritance: CRunicCore
struct CAIFlagManager
{
  PVOID vtable;
  u32 unk0;
  PVOID UnkStruct0; // This appears to be a vector<CAIFlag*>* or similar structure, null term list
  u32 unk1[3];
  PVOID pCharacter; // Ptr to CCharacter
  PVOID pAIManager; // Ptr to AI Manager
};
