#include "InLineHookR3.h"
BYTE Rec[5];
VOID RecoverInLineHook(LPCSTR FuncName)//后面直接默认是HOOK NTDLL.DLL的。NT函数
{
	HANDLE hModule=GetModuleHandle(L"ntdll.dll");
	if(hModule==NULL)
	{
		OutputDebugStringA("getModuleHanlde error");
		return ;
	}
	DWORD FuncAddr=(DWORD)GetProcAddress((HMODULE)hModule,FuncName);
	if(FuncAddr==NULL)
	{
		MyDbgString("recover hook 没有获取到函数地址 %s",FuncName);
		return ;
	}
	FuncAddr+=5;
	DWORD Old,Old1;
	VirtualProtect((LPVOID)FuncAddr,5,PAGE_EXECUTE_READWRITE,&Old);
	memcpy((LPVOID)FuncAddr,(LPVOID)Rec,5);
	VirtualProtect((LPVOID)FuncAddr,5,Old,&Old1);

}
BOOL InLineHook(LPCWSTR ModuleName,LPCSTR FuncName,DWORD MyFuncAddr,BOOL IsInNtdll)
{
	HANDLE hModule=GetModuleHandle(ModuleName);
	if(hModule==NULL)
	{
		OutputDebugStringA("getModuleHanlde error");
		return FALSE;
	}

	DWORD FuncAddr=(DWORD)GetProcAddress((HMODULE)hModule,FuncName);
	if(IsInNtdll==TRUE)
	{
		FuncAddr+=5;
	}
	if(FuncAddr==NULL)
	{

		MyDbgString("没有获取到函数地址 %s",FuncName);
		return FALSE;
	}
	DWORD MyJmp=0;
	BYTE MyAdd[5]={0xe9,0x00,0x00,0x00,0x00};
	MyJmp=MyFuncAddr-5-FuncAddr;
	memcpy(MyAdd+1,&MyJmp,4);
	DWORD Old,Old1;
	VirtualProtect((LPVOID)FuncAddr,5,PAGE_EXECUTE_READWRITE,&Old);
	memcpy((LPVOID)FuncAddr,(LPVOID)MyAdd,5);
	VirtualProtect((LPVOID)FuncAddr,5,Old,&Old1);
	return TRUE;
}