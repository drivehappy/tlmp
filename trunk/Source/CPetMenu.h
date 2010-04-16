#pragma once

struct CPetMenu
{
  PVOID vtable;
  PVOID unk0;
  PVOID vtableIInventoryListener; // vtable ptr
  PVOID pCEGUIBaseGUISheet;       // ptr to GUI Sheet
  // 6 more GUISheet ptrs
};
