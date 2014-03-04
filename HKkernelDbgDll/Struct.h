#pragma once
#include "windows.h"


typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID;
typedef CLIENT_ID *PCLIENT_ID;

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;

} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;


typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;// Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;
	// Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

typedef LONG NTSTATUS;

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define NT_ERROR(Status) ((ULONG)(Status) >> 30 == 3)

typedef struct _SSDT_FUNC{
	CHAR ssdtFuncName[256];
	DWORD MyssdtFunc;
	PVOID Next;
}SSDTFUNC,*PSSDTFUNC;

extern PSSDTFUNC pHead;

BOOL AddSSDTFunc(PCHAR ssdtFuncName,DWORD MyssdtFunc);
VOID FreeMemory();