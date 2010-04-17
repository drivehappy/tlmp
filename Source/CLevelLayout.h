#pragma once

// Size?: unk
struct CLevelLayout
{
  PVOID vtable;
  u32 unk0;
  PVOID pChunkList;   // Probably: vector<CChunk*>*
};
