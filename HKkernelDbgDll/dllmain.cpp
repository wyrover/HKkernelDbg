#include "IncAll.h" 
#pragma data_seg("shareddata")
DWORD InstanceNum=0;
DWORD IsInit=FALSE;
#pragma data_seg()
/////////////////////////
#pragma comment(linker,"/section:shareddata,rws")


BOOL InitKernel();//内核DLL加载，以及部分数据初始化
BOOL DllLoad();
VOID DllUnload();
VOID UnHook();//恢复inlineHook (R3)Nt系列函数
VOID InLineHookAll();//InlineHook (R3) Nt系列函数
///////////////////////
#define SysLinkName L"HKKernelDbg"
#define SysFileName L"HKKernelDbg.sys"
#define SysPathCatch L"\\HKKernelDbgSys.sys"
#define SysCreateFileStr L"\\\\.\\HKKernelDbg"
#define DllFileName L"HKkernelDbgDll.dll"
//////////////////////////
#define IniPathCatch L"\\HKKernelDbg.ini"
#define KernelPathCatch L"\\Kernel.dll"
////////////////////////////////
PIMAGE_DOS_HEADER  pDosHeader;
PIMAGE_NT_HEADERS32  pNtHeader;
PIMAGE_OPTIONAL_HEADER32 pOptionalHeader;
///////////////////////////////////////////
HMODULE h_Self=NULL;
//DWORD Addr_MmGSRA_Offset=NULL;
//////////////////
//创建内核驱动句柄
HANDLE MyCreateFile(LPCTSTR lpFileName);
////////////////////
BOOL LoadDriver();
////////////////
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		h_Self=hModule;
		   IsInit=DllLoad();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		DllUnload();
		break;
	}
	return TRUE;
}
BOOL LoadDriver()
{

	if(InstanceNum>0)
	{
		InstanceNum++;
		return TRUE;
	}
	WCHAR lpBuffer[300]=L"";
    DWORD Num=GetModuleFileName(h_Self,lpBuffer,256);
	if(Num==0)
	{
		MyDbgString("GetCurrentDirectory ERROR=%d",ERROR_CODE);
		return FALSE;
	}
	while(lpBuffer[Num-1]!='\\')
	{
		Num--;
	}
	lpBuffer[Num-1]='\0';

	wcscat_s(lpBuffer,SysPathCatch);
//	::OutputDebugStringW(lpBuffer);
	if(!PathFileExists(lpBuffer))
	{
		OutputDebugString(L"目录下的SYS文件不存在");
		return FALSE;
	}
	 BOOL bRet=LoadNtDriver(SysFileName,lpBuffer);
	 if(bRet)
	 {
		 InstanceNum++;
	 }
	 return bRet;
}
BOOL InitKernel()
{
	BOOL bRet=FALSE;
	hDevice=MyCreateFile(SysCreateFileStr);
	if(hDevice==INVALID_HANDLE_VALUE)
	{
		MyDbgString("创建驱动设备失败%d",ERROR_CODE);
		return FALSE;
	}

	WCHAR lpBuffer[300]=L"";
	DWORD Num=GetModuleFileName(h_Self,lpBuffer,256);
//	::OutputDebugStringW(lpBuffer);
	if(Num==0)
	{
		MyDbgString("GetCurrentDirectory ERROR=%d",ERROR_CODE);
		return FALSE;
	}
	while(lpBuffer[Num-1]!='\\')
	{
		Num--;
	}
	lpBuffer[Num-1]='\0';

	wcscat_s(lpBuffer,KernelPathCatch);
	
	if(!PathFileExists(lpBuffer))
	{
		OutputDebugString(L"目录下的kernel.dll文件不存在");
		return FALSE;
	}
	hm_Kernel=::LoadLibrary(lpBuffer);
	if(hm_Kernel==NULL)
	{
	    OutputDebugStringA("装载内核DLL错误");
	return FALSE;
	}
	ha_Kernel=GetModuleHandle(L"kernel.dll");
	if(ha_Kernel==NULL)
	{
		OutputDebugStringA("装载内核DLL错误");
		return FALSE;
	}
	KernelBase=(DWORD)ha_Kernel;
	pDosHeader=(PIMAGE_DOS_HEADER)KernelBase;
	pNtHeader=(PIMAGE_NT_HEADERS32)(pDosHeader->e_lfanew+(DWORD)KernelBase);
	pOptionalHeader=&(pNtHeader->OptionalHeader);
	KernelSizeInMemory=pOptionalHeader->SizeOfImage;
	ImageBase=pOptionalHeader->ImageBase;
//	MyDbgString("kernelbase=0x%08x",KernelBase);

	HANDLE hNtdll=GetModuleHandle(L"ntdll.dll");
	/*注意一下获取的两个Nt函数
	不是在NTDLL.DLL里面的地址。
	而是在内核里面的地址，
	然后我再利用NTDLL中对应的索引号
	来定位SSDT表地址
	并不是所有的SSDT函数都在内核驱动中被导出
	因此我统一用SSDT来获取函数的偏移。
	*/
	DWORD CheckOpenProc=(DWORD)GetProcAddress(hm_Kernel,"NtOpenProcess");
	DWORD CheckOpenThread=(DWORD)GetProcAddress(hm_Kernel,"NtOpenThread");
	memcpy(Rec,LPVOID(CheckOpenProc+5),5);
	if(CheckOpenProc==NULL||CheckOpenThread==NULL)
	{
		OutputDebugStringA("get openthread or process addr error");
		return FALSE;
	}
	if(!InitSSDTTableAddr((DWORD)ha_Kernel,CheckOpenProc-(DWORD)hm_Kernel+ImageBase,CheckOpenThread-(DWORD)hm_Kernel+ImageBase))
	{
		OutputDebugStringA("initssdttalbe error");
		return FALSE;
	}


	//初始化参数。并调用DeviceIoControl在内核内也进行初始化

	DWORD OldPro;
	bRet=VirtualProtect(\
		(LPVOID)KernelBase,\
		KernelSizeInMemory,\
		PAGE_EXECUTE_READWRITE,\
		&OldPro);
	if(!bRet)
	{
		MyDbgString("virtualprotect error");
		return bRet;
	}

	DWORD InBuf[2];
	InBuf[0]=KernelBase;//Dll内核的基址
	InBuf[1]=KernelSizeInMemory;//内存中的内核大小
	DWORD OutBuf[1];
	DWORD BytesReturned;
	bRet=DeviceIoControl(hDevice,\
		IoInit_code,\
		InBuf,\
		sizeof(InBuf),\
		OutBuf,\
		sizeof(OutBuf),\
		&BytesReturned,\
		NULL
		);
	if(!bRet)
	{
		MyDbgString("Init DeviceIoControl执行失败");
		return bRet;
	}
	return TRUE;
}
//创建驱动设备
HANDLE MyCreateFile(LPCTSTR lpFileName)
{
	return CreateFile(\
		lpFileName,\
		GENERIC_READ|GENERIC_WRITE,\
		0,\
		NULL,\
		OPEN_EXISTING,\
		FILE_ATTRIBUTE_NORMAL,\
		NULL\
		);
}
VOID InLineHookAll()
{
	/*
	NtOpenProcess...
	NtOpenThread	...
	NtWriteVirtualMemory
	NtReadVirtualMemory
	NtDebugActiveProcess
	NtQueryInformationProcess...
	NtQuerySystemInformation.....
	NtQueryObject
	NtSetContextThread
	NtGetContextThread
	NtSetInformationThread...

	*/
	WCHAR lpBuffer[300]=L"";
	DWORD Num=GetModuleFileName(h_Self,lpBuffer,256);
	if(Num==0)
	{
		MyDbgString("GetModuleFileName InLineHook ERROR=%d",ERROR_CODE);
		return;
	}
	while(lpBuffer[Num-1]!='\\')
	{
		Num--;
	}
	lpBuffer[Num-1]='\0';
	wcscat_s(lpBuffer,IniPathCatch);
//	::OutputDebugStringW(lpBuffer);
	//////////////////////////

	WCHAR lpName[300]=L"";


	PSSDTFUNC pTemp=pHead;

	while(pTemp!=NULL)
	{
		a2w(pTemp->ssdtFuncName,lpName);
		if(GetPrivateProfileInt(L"R3HOOK",lpName,0,lpBuffer))
		{
		InLineHook(\
			L"ntdll.dll",\
			pTemp->ssdtFuncName,\
			pTemp->MyssdtFunc,\
			1);
		}

		pTemp=(PSSDTFUNC)pTemp->Next;
	}
}
BOOL AddR3HookFunc()
{
	BOOL bRet=FALSE;
	if(!AddSSDTFunc("NtOpenProcess",(DWORD)KernelNtOpenProcess))
	{
		goto end;
	}
	if(!AddSSDTFunc("NtOpenThread",(DWORD)KernelNtOpenThread))
	{
		goto end;
	}
	if(!AddSSDTFunc("NtWriteVirtualMemory",(DWORD)KernelNtWriteVirtualMemory))
	{
		goto end;
	}
	if(!AddSSDTFunc("NtReadVirtualMemory",(DWORD)KernelNtReadVirtualMemory))
	{
		goto end;
	}
	if(!AddSSDTFunc("NtDebugActiveProcess",(DWORD)KernelNtDebugActiveProcess))
	{
		goto end;
	}
	if(!AddSSDTFunc("NtSetInformationThread",(DWORD)KernelNtSetInformationThread))
	{
		goto end;
	}
	if(!AddSSDTFunc("NtQuerySystemInformation",(DWORD)KernelNtQuerySystemInformation))
	{
		goto end;
	}
	if(!AddSSDTFunc("NtQueryObject",(DWORD)KernelNtQueryObject))
	{
		goto end;
	}
	if(!AddSSDTFunc("NtSetContextThread",(DWORD)KernelNtSetContextThread))
	{
		goto end;
	}
	if(!AddSSDTFunc("NtGetContextThread",(DWORD)KernelNtGetContextThread))
	{
		goto end;
	}
	if(!AddSSDTFunc("NtQueryInformationProcess",(DWORD)KernelNtQueryInformationProcess))
	{
		goto end;
	}
	bRet=TRUE;
end:
	return bRet;
}
BOOL DllLoad()
{
	BOOL bRet=FALSE;
	bRet=LoadDriver();
	if(!bRet)
	{
		return bRet;
	}
	bRet=AddR3HookFunc();
	if(!bRet)
	{
		return bRet;
	}
	bRet=InitKernel();
	if(!bRet)
	{
		return bRet;
	}
    InLineHookAll();
	return bRet;
}

//////////////////////////////////////////////
//UnLoadDLL
//////////////////////////////////////////////

VOID DllUnload()
{
	UnHook();
	if(hm_Kernel)
	{
		FreeLibrary(hm_Kernel);
	}
	if(hDevice)
	{
		CloseHandle(hDevice);
	}
	if(InstanceNum!=1)
	{
		InstanceNum--;
		return ;
	}
	UnLoadNtDriver(SysFileName);
}
VOID UnHook()
{
	WCHAR lpBuffer[300]=L"";
	DWORD Num=GetModuleFileName(h_Self,lpBuffer,256);
	if(Num==0)
	{
		MyDbgString("GetModuleFileName UnHook ERROR=%d",ERROR_CODE);
		return;
	}
	while(lpBuffer[Num-1]!='\\')
	{
		Num--;
	}
	lpBuffer[Num-1]='\0';
	wcscat_s(lpBuffer,IniPathCatch);
	//////////////////////////
	WCHAR lpName[300]=L"";
	PSSDTFUNC pTemp=pHead;

	while(pTemp!=NULL)
	{
		a2w(pTemp->ssdtFuncName,lpName);
		if(GetPrivateProfileInt(L"R3HOOK",lpName,0,lpBuffer))
		{
			RecoverInLineHook(pTemp->ssdtFuncName);
		}
		pTemp=(PSSDTFUNC)pTemp->Next;
	}
}