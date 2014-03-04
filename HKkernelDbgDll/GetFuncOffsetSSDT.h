#pragma once
#include "windows.h"
#include "CallInKernel.h"
extern DWORD *SSDTAddress;
BOOL InitSSDTTableAddr(DWORD ModuleAddress,DWORD CheckOpenProc,DWORD CheckOpenThread);

DWORD GetAddressFromSSDTByName(LPCSTR FuncName);