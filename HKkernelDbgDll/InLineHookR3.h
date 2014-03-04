#include "windows.h"
#include "HKKernelDbg.h"
extern BYTE Rec[5];
BOOL InLineHook(LPCWSTR ModuleName,LPCSTR FuncName,DWORD MyFuncAddr,BOOL IsInNtdll);
VOID RecoverInLineHook(LPCSTR FuncName);