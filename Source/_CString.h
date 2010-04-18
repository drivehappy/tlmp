#pragma once

struct CString
{
  union {
    wchar_t string[8];    // In-place unicode data
    PVOID stringPtr[4];   // First is the unicode ptr, the rest are unk
  };

  u32 length;
  u32 lengthMax;    // Guess, double check this
};
