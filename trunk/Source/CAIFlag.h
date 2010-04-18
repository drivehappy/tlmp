#pragma once

/*
  FIXME:
    Unsure of the exact size of this
    Define functions
**/

// Inheritance: CRunicCore
struct CAIFlag
{
  PVOID vtable;
  u32 unk0;
  u32 unk1;
  PVOID pCAIFlagManager;  // Ptr to CAIFlagManager
  u32 unk2[4];            
};
