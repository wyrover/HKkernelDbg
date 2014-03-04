// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 HKKERNELDBG_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// HKKERNELDBG_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#pragma once
#ifdef HKKERNELDBG_EXPORTS
#define HKKERNELDBG_API __declspec(dllexport)
#else
#define HKKERNELDBG_API __declspec(dllimport)
#endif

#include "CallInKernel.h"
#pragma warning(disable:4273)
extern "C" HKKERNELDBG_API HANDLE  HKOpenProcess(  \
	                  DWORD dwDesiredAccess,\
					  BOOL bInheritHandle,\
					  DWORD dwProcessId\
					  );
extern "C" HKKERNELDBG_API DWORD  HKNtDebugActiveProcess(HANDLE hProcess,HANDLE DbgHandle);

extern "C" HKKERNELDBG_API HANDLE  HKOpenThread(\
	DWORD dwDesiredAccess,\
	BOOL bInheritHandle,\
	DWORD dwThreadId\
	);
extern "C" HKKERNELDBG_API BOOL  HKNtReadVirtualMemory( \
	HANDLE hProcess,\
	LPCVOID lpBaseAddress,\
	LPVOID lpBuffer,\
	DWORD nSize,\
	LPDWORD lpNumberOfBytesRead\
	);

extern "C" HKKERNELDBG_API BOOL  HKNtWriteVirtualMemory(\
	HANDLE hProcess,\
	LPVOID lpBaseAddress,\
	LPVOID lpBuffer,\
	DWORD nSize,\
	LPDWORD lpNumberOfBytesWritten
	);

extern "C" HKKERNELDBG_API DWORD  HKNtQueryInformationProcess(\
	HANDLE hProcess,\
	ULONG EnumClass,\
	PVOID ProcessInfo,\
	ULONG InfoLength,\
	PULONG ReturnLength);

extern "C" HKKERNELDBG_API DWORD  HKNtQuerySystemInformation(\
	DWORD SystemInfoClass,\
	PVOID SystemInfo,\
	ULONG InfoLength,\
	ULONG ReturnLength);

extern "C" HKKERNELDBG_API DWORD  HKNtQueryObject(\
	HANDLE hanlde,\
	DWORD EnumClass,\
	PVOID ObjInfo,\
	ULONG length,\
	PULONG ReturnLength);

extern "C" HKKERNELDBG_API DWORD  HKNtSetContextThread(\
	HANDLE hThread,\
	LPCONTEXT lpContext
	);
extern "C" HKKERNELDBG_API DWORD  HKNtGetContextThread( \
	HANDLE hThread,\
	LPCONTEXT lpContext
	);

extern "C" HKKERNELDBG_API DWORD  HKNtSetInformationThread(\
	HANDLE hThread,\
	DWORD EnumThreadClass,\
	PVOID ThreadInfo,\
	ULONG Length);