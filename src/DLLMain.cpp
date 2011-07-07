#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include "tlapi.h"

#include "Multiplayer.h"
using namespace TLMP;

//
// Rerouting code developed by dengus
HMODULE dll_hm;

BOOL loadWinmm();
const char* __str__s_ln = "%s\n";
const char* __str__failed = "Bounce Function Failed: ";

extern "C" {

#define bounce(x)\
  void* __func__##x = 0; \
  const char*__str__##x = #x; \
  void __declspec(dllexport) __declspec(naked) x() { \
    _asm cmp dll_hm, 0 \
    _asm jz fail_load \
    _asm dll_good: \
    _asm cmp __func__##x, 0 \
    _asm jnz l1 \
    _asm push __str__##x  \
    _asm push dll_hm \
    _asm call ds:GetProcAddress \
    _asm test eax, eax \
    _asm jz fail \
    _asm mov __func__##x, eax \
    _asm l1: \
    _asm mov eax, __func__##x \
    _asm jmp eax \
    _asm fail: \
    _asm push 0 \
    _asm push __str__failed \
    _asm push __str__##x \
    _asm push 0 \
    _asm call ds:MessageBox \
    _asm retn \
    _asm fail_load: \
    _asm call ds:loadWinmm \
    _asm jmp dll_good \
  }

  bounce(aux32Message);
  bounce(auxGetDevCapsA);
  bounce(auxGetDevCapsW);
  bounce(auxGetNumDevs);
  bounce(auxGetVolume);
  bounce(auxOutMessage);
  bounce(auxSetVolume);
  bounce(CloseDriver);
  bounce(DefDriverProc);
  bounce(DriverCallback);
  bounce(DrvGetModuleHandle);
  bounce(GetDriverModuleHandle);
  bounce(gfxAddGfx);
  bounce(gfxBatchChange);
  bounce(gfxCreateGfxFactoriesList);
  bounce(gfxCreateZoneFactoriesList);
  bounce(gfxDestroyDeviceInterfaceList);
  bounce(gfxEnumerateGfxs);
  bounce(gfxModifyGfx);
  bounce(gfxOpenGfx);
  bounce(gfxRemoveGfx);
  bounce(joy32Message);
  bounce(joyConfigChanged);
  bounce(joyGetDevCapsA);
  bounce(joyGetDevCapsW);
  bounce(joyGetNumDevs);
  bounce(joyGetPos);
  bounce(joyGetPosEx);
  bounce(joyGetThreshold);
  bounce(joyReleaseCapture);
  bounce(joySetCapture);
  bounce(joySetThreshold);
  bounce(mci32Message);
  bounce(mciDriverNotify);
  bounce(mciDriverYield);
  bounce(mciExecute);
  bounce(mciFreeCommandResource);
  bounce(mciGetCreatorTask);
  bounce(mciGetDeviceIDA);
  bounce(mciGetDeviceIDFromElementIDA);
  bounce(mciGetDeviceIDFromElementIDW);
  bounce(mciGetDeviceIDW);
  bounce(mciGetDriverData);
  bounce(mciGetErrorStringA);
  bounce(mciGetErrorStringW);
  bounce(mciGetYieldProc);
  bounce(mciLoadCommandResource);
  bounce(mciSendCommandA);
  bounce(mciSendCommandW);
  bounce(mciSendStringA);
  bounce(mciSendStringW);
  bounce(mciSetDriverData);
  bounce(mciSetYieldProc);
  bounce(mid32Message);
  bounce(midiConnect);
  bounce(midiDisconnect);
  bounce(midiInAddBuffer);
  bounce(midiInClose);
  bounce(midiInGetDevCapsA);
  bounce(midiInGetDevCapsW);
  bounce(midiInGetErrorTextA);
  bounce(midiInGetErrorTextW);
  bounce(midiInGetID);
  bounce(midiInGetNumDevs);
  bounce(midiInMessage);
  bounce(midiInOpen);
  bounce(midiInPrepareHeader);
  bounce(midiInReset);
  bounce(midiInStart);
  bounce(midiInStop);
  bounce(midiInUnprepareHeader);
  bounce(midiOutCacheDrumPatches);
  bounce(midiOutCachePatches);
  bounce(midiOutClose);
  bounce(midiOutGetDevCapsA);
  bounce(midiOutGetDevCapsW);
  bounce(midiOutGetErrorTextA);
  bounce(midiOutGetErrorTextW);
  bounce(midiOutGetID);
  bounce(midiOutGetNumDevs);
  bounce(midiOutGetVolume);
  bounce(midiOutLongMsg);
  bounce(midiOutMessage);
  bounce(midiOutOpen);
  bounce(midiOutPrepareHeader);
  bounce(midiOutReset);
  bounce(midiOutSetVolume);
  bounce(midiOutShortMsg);
  bounce(midiOutUnprepareHeader);
  bounce(midiStreamClose);
  bounce(midiStreamOpen);
  bounce(midiStreamOut);
  bounce(midiStreamPause);
  bounce(midiStreamPosition);
  bounce(midiStreamProperty);
  bounce(midiStreamRestart);
  bounce(midiStreamStop);
  bounce(MigrateAllDrivers);
  bounce(MigrateSoundEvents);
  bounce(mixerClose);
  bounce(mixerGetControlDetailsA);
  bounce(mixerGetControlDetailsW);
  bounce(mixerGetDevCapsA);
  bounce(mixerGetDevCapsW);
  bounce(mixerGetID);
  bounce(mixerGetLineControlsA);
  bounce(mixerGetLineControlsW);
  bounce(mixerGetLineInfoA);
  bounce(mixerGetLineInfoW);
  bounce(mixerGetNumDevs);
  bounce(mixerMessage);
  bounce(mixerOpen);
  bounce(mixerSetControlDetails);
  bounce(mmDrvInstall);
  bounce(mmGetCurrentTask);
  bounce(mmioAdvance);
  bounce(mmioAscend);
  bounce(mmioClose);
  bounce(mmioCreateChunk);
  bounce(mmioDescend);
  bounce(mmioFlush);
  bounce(mmioGetInfo);
  bounce(mmioInstallIOProcA);
  bounce(mmioInstallIOProcW);
  bounce(mmioOpenA);
  bounce(mmioOpenW);
  bounce(mmioRead);
  bounce(mmioRenameA);
  bounce(mmioRenameW);
  bounce(mmioSeek);
  bounce(mmioSendMessage);
  bounce(mmioSetBuffer);
  bounce(mmioSetInfo);
  bounce(mmioStringToFOURCCA);
  bounce(mmioStringToFOURCCW);
  bounce(mmioWrite);
  bounce(mmsystemGetVersion);
  bounce(mmTaskBlock);
  bounce(mmTaskCreate);
  bounce(mmTaskSignal);
  bounce(mmTaskYield);
  bounce(mod32Message);
  bounce(mxd32Message);
  bounce(NotifyCallbackData);
  bounce(OpenDriver);
  bounce(PlaySoundA);
  bounce(PlaySoundW);
  bounce(SendDriverMessage);
  bounce(sndPlaySoundA);
  bounce(sndPlaySoundW);
  bounce(tid32Message);
  bounce(timeBeginPeriod);
  bounce(timeEndPeriod);
  bounce(timeGetDevCaps);
  bounce(timeGetSystemTime);
  bounce(timeGetTime);
  bounce(timeKillEvent);
  bounce(timeSetEvent);
  bounce(waveInAddBuffer);
  bounce(waveInClose);
  bounce(waveInGetDevCapsA);
  bounce(waveInGetDevCapsW);
  bounce(waveInGetErrorTextA);
  bounce(waveInGetErrorTextW);
  bounce(waveInGetID);
  bounce(waveInGetNumDevs);
  bounce(waveInGetPosition);
  bounce(waveInMessage);
  bounce(waveInOpen);
  bounce(waveInPrepareHeader);
  bounce(waveInReset);
  bounce(waveInStart);
  bounce(waveInStop);
  bounce(waveInUnprepareHeader);
  bounce(waveOutBreakLoop);
  bounce(waveOutClose);
  bounce(waveOutGetDevCapsA);
  bounce(waveOutGetDevCapsW);
  bounce(waveOutGetErrorTextA);
  bounce(waveOutGetErrorTextW);
  bounce(waveOutGetID);
  bounce(waveOutGetNumDevs);
  bounce(waveOutGetPitch);
  bounce(waveOutGetPlaybackRate);
  bounce(waveOutGetPosition);
  bounce(waveOutGetVolume);
  bounce(waveOutMessage);
  bounce(waveOutOpen);
  bounce(waveOutPause);
  bounce(waveOutPrepareHeader);
  bounce(waveOutReset);
  bounce(waveOutRestart);
  bounce(waveOutSetPitch);
  bounce(waveOutSetPlaybackRate);
  bounce(waveOutSetVolume);
  bounce(waveOutUnprepareHeader);
  bounce(waveOutWrite);
  bounce(wid32Message);
  bounce(winmmDbgOut);
  bounce(WinmmLogoff);
  bounce(WinmmLogon);
  bounce(winmmSetDebugLevel);
  bounce(wod32Message);
  bounce(WOW32DriverCallback);
  bounce(WOW32ResolveMultiMediaHandle);
  bounce(WOWAppExit)
}

BOOL loadWinmm() {
  // Load the real winmm.dll and setup trampoline functions
  const UINT DLL_PATH_SIZE = 1024;
  const LPTSTR DLL_NAME = _T("\\winmm.dll");
  LPTSTR systemPath = new TCHAR[DLL_PATH_SIZE];
  DWORD pathSize = GetSystemDirectory(systemPath, DLL_PATH_SIZE);

  if (pathSize == 0) {
    DWORD dwErr = GetLastError();
    if (ERROR_ENVVAR_NOT_FOUND == dwErr) {
      MessageBox(NULL, _T("Environment variable does not exist."), _T("Error"), 0);
    } else {
      MessageBox(NULL, _T("Error occured when attempting to obtain system directory."), _T("Error"), 0);
    }
    return FALSE;
  }

  if (pathSize > DLL_PATH_SIZE - _tcslen(DLL_NAME)) {
    MessageBox(NULL, _T("System path too long."), _T("Error"), 0);
    return FALSE;
  }

  _tcscat_s(systemPath, DLL_PATH_SIZE, DLL_NAME);

  dll_hm = LoadLibrary(systemPath);
  if (!dll_hm) {
    log("SystemPath is bad: %s", systemPath);
    log("Error Code: %x", GetLastError());
    MessageBox(NULL, systemPath, _T("Bad system path"), 0);
    return FALSE;
  }

  return TRUE;
}

bool initialized = false;

BOOL WINAPI DllMain(HANDLE hModule, DWORD dwReason, void *lpReserved)
{
  HANDLE this_module = hModule;
  AllocConsole();
  freopen("CONOUT$", "w", stdout);

  if (GetModuleHandle("Torchlight.exe")) {
    if (dwReason == DLL_PROCESS_ATTACH) {
      log("Torchlight Multiplayer");
      log("Base is at %p", GetModuleHandle("torchlight.exe"));

      if (loadWinmm() == FALSE) {
        return FALSE;
      }

      TLAPI::Initialize();

      SetupNetwork();

      initialized = true;
    } else if (dwReason == DLL_THREAD_ATTACH) {
    } else if (dwReason == DLL_PROCESS_DETACH) {
    }
  }

  return TRUE;
}
