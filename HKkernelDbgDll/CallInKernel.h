#pragma once
#include "windows.h"
#include "DbgOut.h"
#include "GetFuncOffsetSSDT.h"
#include "struct.h"
extern HANDLE hDevice;
extern HMODULE hm_Kernel;
extern HANDLE ha_Kernel;
extern DWORD KernelSizeInMemory;
extern DWORD KernelBase;
extern DWORD ImageBase;
#include "winioctl.h"

#define IoRequest_code CTL_CODE(\
	FILE_DEVICE_UNKNOWN,\
	0x800,\
	METHOD_BUFFERED,\
	FILE_ANY_ACCESS\
	)
#define IoInit_code CTL_CODE(\
	FILE_DEVICE_UNKNOWN,\
	0x801,\
	METHOD_BUFFERED,\
	FILE_ANY_ACCESS\
	)
HANDLE __stdcall KernelOpenProcess(\
	                  DWORD dwDesiredAccess,\
					  BOOL bInheritHandle,\
					  DWORD dwProcessId\
					  );
BOOL  __stdcall KernelNtWriteVirtualMemory(\
								HANDLE hProcess,\
								LPVOID lpBaseAddress,\
								LPVOID lpBuffer,\
								DWORD nSize,\
								LPDWORD lpNumberOfBytesWritten
								);
BOOL __stdcall KernelNtReadVirtualMemory( \
							HANDLE hProcess,\
							LPCVOID lpBaseAddress,\
							LPVOID lpBuffer,\
							DWORD nSize,\
							LPDWORD lpNumberOfBytesRead\
							);
HANDLE __stdcall KernelOpenThread(\
							DWORD dwDesiredAccess,
							BOOL bInheritHandle,
							DWORD dwThreadId
							);
DWORD __stdcall KernelNtDebugActiveProcess(HANDLE hProcess,HANDLE DbgHandle);

DWORD __stdcall KernelNtQueryInformationProcess(HANDLE hProcess,\
	ULONG EnumClass,\
	PVOID ProcessInfo,\
	ULONG InfoLength,\
	PULONG ReturnLength);

DWORD __stdcall KernelNtQuerySystemInformation(DWORD SystemInfoClass,\
	PVOID SystemInfo,\
	ULONG InfoLength,\
	ULONG ReturnLength);

DWORD __stdcall KernelNtQueryObject(HANDLE hanlde,\
	DWORD EnumClass,\
	PVOID ObjInfo,\
	ULONG length,\
	PULONG ReturnLength);

DWORD __stdcall KernelNtSetContextThread( HANDLE hThread,LPCONTEXT lpContext
	);
DWORD __stdcall KernelNtGetContextThread( HANDLE hThread,LPCONTEXT lpContext
	);

DWORD __stdcall KernelNtSetInformationThread(HANDLE hThread,\
	DWORD EnumThreadClass,\
	PVOID ThreadInfo,\
	ULONG Length);

DWORD __stdcall KernelNtOpenProcess(\
	PHANDLE pHandle,\
	DWORD dwDesiredAccess,\
	PVOID struct1,\
	PVOID struct2\
	);

DWORD __stdcall KernelNtOpenThread(\
	PHANDLE pHandle,\
	DWORD dwDesiredAccess,\
	PVOID struct1,\
	PVOID struct2\
	);