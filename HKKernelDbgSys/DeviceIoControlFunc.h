#pragma once
extern "C"
{
#include "ntddk.h"

};

#include "windef.h"
#include "ReLoc.h"
#include "Struct.h"
extern BOOL IsAlreadyInit;
//extern DWORD Addr_MmGSRA;
extern LPVOID KernelCopyAddr;
extern DWORD KernelAddr;
extern DWORD PoolType;

//HK导出函数调用时的CODE
#define IoRequest_code CTL_CODE(\
	FILE_DEVICE_UNKNOWN,\
	0x800,\
	METHOD_BUFFERED,\
	FILE_ANY_ACCESS\
	)
//DLL加载时初始化的CODE
#define IoInit_code CTL_CODE(\
	FILE_DEVICE_UNKNOWN,\
	0x801,\
	METHOD_BUFFERED,\
	FILE_ANY_ACCESS\
	)

//HK导出函数调用时的派遣函数
VOID CtrlCodeFunc_IoRequest(\
								PIRP pIrp,\
								PIO_STACK_LOCATION pstack,\
								PULONG pinfo\
								);
//DLL加载时初始化的派遣函数
VOID CtrlCodeFunc_IoInit(\
								PIRP pIrp,\
								PIO_STACK_LOCATION pstack,\
								PULONG pinfo\
								);