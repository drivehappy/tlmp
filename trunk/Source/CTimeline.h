#pragma once

// Size?: 30h
struct CTimeline
{
  PVOID vtable;
  u32 unk0;

  u64 guid0;  // 0A83F79FE49D311DFh
  u64 guid1;  // 0A83F75CD49D311DFh
  u64 guid2;  // 0CDEFB2A8BB7F11DEh
  u64 guid3;  // 0

  u32 unk1;

  PVOID pCTimelineDescriptor;
};
