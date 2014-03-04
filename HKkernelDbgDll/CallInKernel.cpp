#include "CallInKernel.h"
#include "windows.h"
HMODULE hm_Kernel=NULL;
HANDLE ha_Kernel=NULL;
HANDLE hDevice=NULL;
DWORD KernelSizeInMemory=NULL;
DWORD KernelBase=NULL;
DWORD ImageBase=NULL;
////////////////////////////////////////////////////////////////////////////
DWORD  MyCommonCall(PCHAR NtFuncName,DWORD *InBuf);
DWORD  MyCommonCall(PCHAR NtFuncName,DWORD *InBuf)
{
	DWORD ArgNum=InBuf[0];
	DWORD RetValue=0xc0000005;//定义返回错误为拒绝访问;
	if(ArgNum==0)
	{
		MyDbgString("%s-参数个数为0",NtFuncName);
		return RetValue;
	}
	if(hDevice==NULL)
	{
		OutputDebugStringA("驱动设备为空");
		return RetValue;
	}
	DWORD FuncOffset=0;
	FuncOffset=(DWORD)GetAddressFromSSDTByName(NtFuncName);
	if(FuncOffset<=0||FuncOffset>=KernelSizeInMemory)
	{
		MyDbgString("GetFuncOffset In %s",NtFuncName);
		return RetValue;
	}
	InBuf[ArgNum+1]=FuncOffset;
	DWORD BytesReturned;
	BOOL bRet=DeviceIoControl(hDevice,\
		IoRequest_code,\
		InBuf,\
		(ArgNum+3)*sizeof(DWORD),\
		&RetValue,\
		sizeof(RetValue),\
		&BytesReturned,\
		NULL);
	if(!bRet)
	{
		MyDbgString("%s",NtFuncName);
		MyDbgString("DeviceIoControl执行失败error=%d",ERROR_CODE);
		return 0xc0000005;
	}
	//	MyDbgString("ntstatus=0x%08x",OutBuf[0]);
	if(NT_ERROR(RetValue))
	{
		MyDbgString("内核中%s执行失败",NtFuncName);
	}
	return RetValue;
}
//////////////////////////////////////////////////////////////////////

BOOL __stdcall KernelNtWriteVirtualMemory(\
	HANDLE hProcess,\
	LPVOID lpBaseAddress,\
	LPVOID lpBuffer,\
	DWORD nSize,\
	LPDWORD lpNumberOfBytesWritten
	)
{
	DWORD InBuf[8];
	InBuf[0]=5;
	InBuf[1]=(DWORD)hProcess;
	InBuf[2]=(DWORD)lpBaseAddress;
	InBuf[3]=(DWORD)lpBuffer;
	InBuf[4]=nSize;
	InBuf[5]=(DWORD)lpNumberOfBytesWritten;
	InBuf[6]=0;//参数偏移
	InBuf[7]=TRUE;
	return MyCommonCall("NtWriteVirtualMemory",InBuf);
};

BOOL __stdcall KernelNtReadVirtualMemory( \
	HANDLE hProcess,\
	LPCVOID lpBaseAddress,\
	LPVOID lpBuffer,\
	DWORD nSize,\
	LPDWORD lpNumberOfBytesRead\
	)
{
	DWORD InBuf[8];
	InBuf[0]=5;
	InBuf[1]=(DWORD)hProcess;
	InBuf[2]=(DWORD)lpBaseAddress;
	InBuf[3]=(DWORD)lpBuffer;
	InBuf[4]=nSize;
	InBuf[5]=(DWORD)lpNumberOfBytesRead;
	InBuf[6]=0;
	InBuf[7]=TRUE;
	return MyCommonCall("NtReadVirtualMemory",InBuf);
}



DWORD __stdcall KernelNtDebugActiveProcess(HANDLE hProcess,HANDLE DbgHandle)
{
	DWORD InBuf[5];
	InBuf[0]=2;
	InBuf[1]=(DWORD)hProcess;
	InBuf[2]=(DWORD)DbgHandle;
	InBuf[3]=0;
	InBuf[4]=TRUE;
	return MyCommonCall("NtDebugActiveProcess",InBuf);
};


DWORD __stdcall KernelNtQueryInformationProcess(HANDLE hProcess,\
	ULONG EnumClass,\
	PVOID ProcessInfo,\
	ULONG InfoLength,\
	PULONG ReturnLength)
{
	DWORD InBuf[8];
	InBuf[0]=5;
	InBuf[1]=(DWORD)hProcess;
	InBuf[2]=(DWORD)EnumClass;
	InBuf[3]=(DWORD)ProcessInfo;
	InBuf[4]=(DWORD)InfoLength;
	InBuf[5]=(DWORD)ReturnLength;
	InBuf[6]=0;
	InBuf[7]=FALSE;
	return MyCommonCall("NtQueryInformationProcess",InBuf);
};

DWORD __stdcall KernelNtQuerySystemInformation(DWORD SystemInfoClass,\
	PVOID SystemInfo,\
	ULONG InfoLength,\
	ULONG ReturnLength)
{
	DWORD InBuf[7];
	InBuf[0]=4;
	InBuf[1]=(DWORD)SystemInfoClass;
	InBuf[2]=(DWORD)SystemInfo;
	InBuf[3]=(DWORD)InfoLength;
	InBuf[4]=(DWORD)ReturnLength;
	InBuf[5]=(DWORD)0;
	InBuf[6]=TRUE;
	return MyCommonCall("NtQuerySystemInformation",InBuf);
}

DWORD __stdcall KernelNtQueryObject(HANDLE handle,\
	DWORD EnumClass,\
	PVOID ObjInfo,\
	ULONG length,\
	PULONG ReturnLength)
{
	DWORD InBuf[8];
	InBuf[0]=5;
	InBuf[1]=(DWORD)handle;
	InBuf[2]=(DWORD)EnumClass;
	InBuf[3]=(DWORD)ObjInfo;
	InBuf[4]=(DWORD)length;
	InBuf[5]=(DWORD)ReturnLength;
	InBuf[6]=0;
	InBuf[7]=TRUE;
	return MyCommonCall("NtQueryObject",InBuf);
};

DWORD __stdcall KernelNtSetContextThread( HANDLE hThread,LPCONTEXT lpContext
	)
{
	DWORD InBuf[5];
	InBuf[0]=2;
	InBuf[1]=(DWORD)hThread;
	InBuf[2]=(DWORD)lpContext;
	InBuf[3]=0;
	InBuf[4]=FALSE;//必须为FALSE
	return MyCommonCall("NtSetContextThread",InBuf);
};
DWORD __stdcall KernelNtGetContextThread( HANDLE hThread,LPCONTEXT lpContext
	)
{
	DWORD InBuf[5];
	InBuf[0]=2;
	InBuf[1]=(DWORD)hThread;
	InBuf[2]=(DWORD)lpContext;
	InBuf[3]=0;
	InBuf[4]=FALSE;
	return MyCommonCall("NtGetContextThread",InBuf);
};
//////////////////
DWORD __stdcall KernelNtSetInformationThread(HANDLE hThread,\
	DWORD EnumThreadClass,\
	PVOID ThreadInfo,\
	ULONG Length)

{
	DWORD InBuf[7];
	InBuf[0]=4;
	InBuf[1]=(DWORD)hThread;
	InBuf[2]=(DWORD)EnumThreadClass;
	InBuf[3]=(DWORD)ThreadInfo;
	InBuf[4]=(DWORD)Length;
	InBuf[5]=0;
	InBuf[6]=FALSE;
	return MyCommonCall("NtSetInformationThread",InBuf);
};

DWORD __stdcall KernelNtOpenProcess(\
	PHANDLE pHandle,\
	DWORD dwDesiredAccess,\
	PVOID struct1,\
	PVOID struct2\
	)
{
	DWORD InBuf[7];
	InBuf[0]=4;
	InBuf[1]=(DWORD)pHandle;
	InBuf[2]=dwDesiredAccess;
	InBuf[3]=(DWORD)struct1;
	InBuf[4]=(DWORD)struct2;
	InBuf[5]=0;
	InBuf[6]=TRUE;
	return MyCommonCall("NtOpenProcess",InBuf);
};

DWORD __stdcall KernelNtOpenThread(\
	PHANDLE pHandle,\
	DWORD dwDesiredAccess,\
	PVOID struct1,\
	PVOID struct2\
	)
{
	DWORD InBuf[7];
	InBuf[0]=4;
	InBuf[1]=(DWORD)pHandle;
	InBuf[2]=dwDesiredAccess;
	InBuf[3]=(DWORD)struct1;
	InBuf[4]=(DWORD)struct2;
	InBuf[5]=0;
	InBuf[6]=TRUE;
   return MyCommonCall("NtOpenThread",InBuf);
};

HANDLE __stdcall KernelOpenProcess(   DWORD dwDesiredAccess,\
	BOOL bInheritHandle,\
	DWORD dwProcessId\
	)
{

	if(hDevice==NULL)
	{
		MyDbgString("设备为空 openprocess失败");
		return NULL;
	}
	HANDLE handle=NULL;
	CLIENT_ID cli;
	BOOL bRet=FALSE;
	cli.UniqueProcess=(void *)dwProcessId;
	cli.UniqueThread=0;

	_asm
	{
		push eax
			mov eax,bInheritHandle
			neg eax
			sbb eax,eax
			add eax,2
			mov bInheritHandle,eax
			pop eax
	}

	OBJECT_ATTRIBUTES ObjectAttributes;
	ObjectAttributes.Length=sizeof(OBJECT_ATTRIBUTES);
	ObjectAttributes.RootDirectory=0;
	ObjectAttributes.ObjectName=0;
	ObjectAttributes.Attributes=bInheritHandle;
	ObjectAttributes.SecurityDescriptor=0;      
	ObjectAttributes.SecurityQualityOfService=0;

	//char FuncName[]="NtOpenProcess";
	DWORD FuncOffset=NULL;
	FuncOffset=(DWORD)GetAddressFromSSDTByName("NtOpenProcess");
	if(FuncOffset<=0||FuncOffset>=KernelSizeInMemory)
	{
		OutputDebugStringA("GetProcAddressIn NtOpenProcess");
		return NULL;
	}


	DWORD InBuf[7];
	InBuf[0]=4;
	InBuf[1]=(DWORD)&handle;
	InBuf[2]=dwDesiredAccess;
	InBuf[3]=(DWORD)&ObjectAttributes;
	InBuf[4]=(DWORD)&cli;
	InBuf[5]=(DWORD)FuncOffset;
	InBuf[6]=TRUE;
	NTSTATUS OutBuf[1];
	DWORD BytesReturned;

	bRet=DeviceIoControl(hDevice,\
		IoRequest_code,\
		InBuf,\
		sizeof(InBuf),\
		OutBuf,\
		sizeof(OutBuf),\
		&BytesReturned,\
		NULL);
	if(!bRet)
	{
		OutputDebugStringA(" device io OpenProcess");
		MyDbgString("DeviceIoControl执行失败error=%d",ERROR_CODE);
		return NULL;
	}
	//	MyDbgString("ntstatus=0x%08x",OutBuf[0]);
	if(NT_ERROR(OutBuf[0]))
	{
		MyDbgString("内核中ntopenprocess执行失败");
	}
	//	MyDbgString("handle =0x%08x",handle);
	return handle;
};
HANDLE __stdcall KernelOpenThread(\
	DWORD dwDesiredAccess,
	BOOL bInheritHandle,
	DWORD dwThreadId
	)
{

	if(hDevice==NULL)
	{
		MyDbgString("设备为空 OpenThread失败");
		return NULL;
	}
	HANDLE handle=NULL;
	CLIENT_ID cli;
	BOOL bRet=FALSE;
	cli.UniqueProcess=0;
	cli.UniqueThread=(void *)dwThreadId;
	_asm
	{

		push eax
			mov eax,bInheritHandle
			neg eax
			sbb eax,eax
			add eax,2
			mov bInheritHandle,eax
			pop eax
	}
	OBJECT_ATTRIBUTES ObjectAttributes;
	ObjectAttributes.Length=sizeof(OBJECT_ATTRIBUTES);
	ObjectAttributes.RootDirectory=0;
	ObjectAttributes.ObjectName=0;
	ObjectAttributes.Attributes=bInheritHandle;
	ObjectAttributes.SecurityDescriptor=0;      
	ObjectAttributes.SecurityQualityOfService=0;

	//	char FuncName[]="NtOpenThread";
	DWORD FuncOffset=NULL;
	FuncOffset=(DWORD)GetAddressFromSSDTByName("NtOpenThread");
	if(FuncOffset<=0||FuncOffset>=KernelSizeInMemory)
	{
		OutputDebugStringA("GetProcAddressIn NtOpenThread");
		return NULL;
	}
	DWORD InBuf[7];
	InBuf[0]=4;
	InBuf[1]=(DWORD)&handle;
	InBuf[2]=dwDesiredAccess;
	InBuf[3]=(DWORD)&ObjectAttributes;
	InBuf[4]=(DWORD)&cli;
	InBuf[5]=(DWORD)FuncOffset;
	InBuf[6]=TRUE;
	NTSTATUS OutBuf[1];
	DWORD BytesReturned;

	bRet=DeviceIoControl(hDevice,\
		IoRequest_code,\
		InBuf,\
		sizeof(InBuf),\
		OutBuf,\
		sizeof(OutBuf),\
		&BytesReturned,\
		NULL);
	if(!bRet)
	{
		OutputDebugStringA(" device io openthread");
		MyDbgString("DeviceIoControl执行失败error=%d",ERROR_CODE);
		return NULL;
	}
	//	MyDbgString("ntstatus=0x%08x",OutBuf[0]);
	if(NT_ERROR(OutBuf[0]))
	{
		MyDbgString("内核中NtOpenThread执行失败");
	}

	return handle;
};