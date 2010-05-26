#pragma once

#include "_CString.h"
#include "CDynamicPropertyFile.h"

struct SettingValue {
  const char *name;
  int index;
};

struct CSettings : CDynamicPropertyFile
{
  PVOID     unk2;
  CString   LocalSettingsDirectory;

  PVOID     unk3;
  CString   SettingsFile;

  PVOID     unk4;
  u32       unk5[2];

  PVOID     unk6[4];
  u32       unk7[2];

  PVOID     MessageIndexLow;        // These two: within sub_5BF870 (1.15) they appear to be used as a message ID filter? if the id > (unk9-unk8) then it returns eax!=0 else eax=0
  PVOID     MessageIndexHigh;       // and the message isn't written to ogre.log
                        /*
                        */

  void dumpSettings()
  {
    u32 *index = (u32*)MessageIndexLow;
    
    logColor(B_RED, "CSettings Dump: %p %p", MessageIndexLow, MessageIndexHigh);
    //log("Test: %i %i %p %p", unk6[2], unk6[3], unk7[0], unk7[1]);
    
    while (index < MessageIndexHigh) {
      logColor(B_RED, "  %#2x: %#x", index, *index);
      index++;
    }
  }
};
