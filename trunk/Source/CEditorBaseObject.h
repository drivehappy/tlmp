#pragma once

// Size?: 30h
// Note: Appears to be the base structure for all the GUID stuff
//       and additionally some ptrs
// Inheritance: CRunicCore
struct CEditorBaseObject
{
  PVOID vtable;

  u32 unk0;
  u64 guid0;
  u64 guid1;
  u64 guid2;
  u64 guid3;

  // Check rest, looking at other structs I think this is where similarities end
  // Really need to find a quantative number for this size
};
