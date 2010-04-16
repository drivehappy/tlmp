#pragma once

struct CAIManager
{
  PVOID vtable;
  u32 unk0;
  PVOID pCharacterPtr;  // Pointer to character
  PVOID pAIFlagManager; // Ptr to AIFlagManager
  PVOID pAISkillManager;// Ptr to CAISkillManager
};
