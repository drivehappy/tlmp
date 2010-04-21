#pragma once

// Size?: 118h
struct CItemSaveState
{
  PVOID vtable;
  u32 unk0;

  PVOID pCItemSaveState;    // Linked list node? ptr to another CItemSaveState

  CString name;

  // .. more
};
