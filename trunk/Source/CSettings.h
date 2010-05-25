#pragma once

#include "_CString.h"
#include "CDynamicPropertyFile.h"

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

};
