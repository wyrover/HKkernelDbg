#include "HKKernelDbg.h"

HANDLE  HKOpenProcess(  \
	                  DWORD dwDesiredAccess,\
					  BOOL bInheritHandle,\
					  DWORD dwProcessId\
					  )
{
	return KernelOpenProcess(dwDesiredAccess,\
					  bInheritHandle,\
					  dwProcessId\
					  );
};

DWORD  HKNtDebugActiveProcess(HANDLE hProcess,HANDLE DbgHandle)
{
	return KernelNtDebugActiveProcess(hProcess,DbgHandle);
};

HANDLE  HKOpenThread(\
	DWORD dwDesiredAccess,\
	BOOL bInheritHandle,\
	DWORD dwThreadId\
	)
{
	return KernelOpenThread(dwDesiredAccess,bInheritHandle,dwThreadId);
}

BOOL  HKReadVirtualMemory( \
	HANDLE hProcess,\
	LPCVOID lpBaseAddress,\
	LPVOID lpBuffer,\
	DWORD nSize,\
	LPDWORD lpNumberOfBytesRead\
	)
{
	return KernelNtReadVirtualMemory(hProcess,lpBaseAddress,lpBuffer,nSize,lpNumberOfBytesRead);
}

BOOL  HKNtWriteVirtualMemory(\
	HANDLE hProcess,\
	LPVOID lpBaseAddress,\
	LPVOID lpBuffer,\
	DWORD nSize,\
	LPDWORD lpNumberOfBytesWritten
	)
{
	return KernelNtWriteVirtualMemory(hProcess,lpBaseAddress,lpBuffer,nSize,lpNumberOfBytesWritten);
};

DWORD  HKNtQueryInformationProcess(\
	HANDLE hProcess,\
	ULONG EnumClass,\
	PVOID ProcessInfo,\
	ULONG InfoLength,\
	PULONG ReturnLength)
{
	return KernelNtQueryInformationProcess(\
		hProcess,\
		EnumClass,\
		ProcessInfo,\
		InfoLength,\
		ReturnLength);
};

DWORD  HKNtQuerySystemInformation(\
	DWORD SystemInfoClass,\
	PVOID SystemInfo,\
	ULONG InfoLength,\
	ULONG ReturnLength)
{
	return KernelNtQuerySystemInformation(\
		SystemInfoClass,\
		SystemInfo,\
		InfoLength,\
		ReturnLength);
}

DWORD  HKNtQueryObject(HANDLE hanlde,\
	DWORD EnumClass,\
	PVOID ObjInfo,\
	ULONG length,\
	PULONG ReturnLength)
{
	return KernelNtQueryObject(\
		hanlde,\
		EnumClass,\
		ObjInfo,\
		length,\
		ReturnLength);
}

DWORD HKNtSetContextThread( \
	HANDLE hThread,\
	LPCONTEXT lpContext
	)
{
	return KernelNtSetContextThread(\
		hThread,\
		lpContext);
}
DWORD  HKNtGetContextThread( \
	HANDLE hThread,\
	LPCONTEXT lpContext
	)
{
	return KernelNtGetContextThread( \
		hThread,\
		lpContext
		);
}

DWORD  HKNtSetInformationThread(\
	HANDLE hThread,\
	DWORD EnumThreadClass,\
	PVOID ThreadInfo,\
	ULONG Length)
{
	return KernelNtSetInformationThread(\
		hThread,\
		EnumThreadClass,\
		ThreadInfo,\
		Length);
}