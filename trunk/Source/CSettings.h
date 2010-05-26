#pragma once

#include "_CString.h"
#include "CDynamicPropertyFile.h"


struct SettingValue {
  const char *name;
  int index;
};

struct _CUnknownStructureFloatContainer {
  PVOID unk;
  float data[5];
};

struct _CUnknownStructureStringContainer {
  CString value;
  float   unk;
};

struct CSettings : CDynamicPropertyFile
{
  PVOID     unk2;
  CString   LocalSettingsDirectory;

  PVOID     unk3;
  CString   SettingsFile;

  PVOID     unk4;
  u32       unk5[2];

  _CUnknownStructureFloatContainer   *UnkStruct0Min;
  _CUnknownStructureFloatContainer   *UnkStruct0Max;

  PVOID     unk6[2];

  u32       unk7[2];

  u32      *SettingIntIndexLow;        // These are pointers to int setting values
  u32      *SettingIntIndexHigh;       // for both local_settings and settings.txt

  PVOID     unk8[2];

  u32       unk9[2];

  float    *SettingFloatIndexLow;     // These are pointers to float setting values
  float    *SettingFloatIndexHigh;    // For "local_settings.txt" and settings.txt

  PVOID     unk10;

  u32       unk11[3];

  _CUnknownStructureStringContainer  *StringSettingValueMin;    // Contains pointers to CString + float structure  these strings are values that appear in
  _CUnknownStructureStringContainer  *StringSettingValueMax;    // For "local_settings.txt" only

  float    *unk12;

  PVOID     unk13;

  u32       unk14[2];

  _CUnknownStructureStringContainer  *StringSettingIntMin;      // Contains pointers to CString + float struct
  _CUnknownStructureStringContainer  *StringSettingIntMax;      // these strings are values that are in SETTINGS.TXT - For Ints only

  // WHERE"S THE LOCAL_SETTING.TXT String Keys + SETTINGS.TXT String keys for floats???

  float    *unk15;    // Pointer to time val float?

  PVOID     unk16;







  void dumpSettings()
  {
    u32 *index = (u32*)SettingIntIndexLow;
    
    logColor(B_RED, "CSettings Dump: %p %p", SettingIntIndexLow, SettingIntIndexHigh);
    //log("Test: %i %i %p %p", unk6[2], unk6[3], unk7[0], unk7[1]);
    
    while (index < SettingIntIndexHigh) {
      logColor(B_RED, "  %#2x: %#x", index, *index);
      index++;
    }
  }
};
