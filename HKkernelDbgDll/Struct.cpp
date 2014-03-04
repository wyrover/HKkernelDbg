#include"struct.h"
#include "DbgOut.h"
#include "stdlib.h"

PSSDTFUNC pHead=NULL;
BOOL AddSSDTFunc(PCHAR ssdtFuncName,DWORD MyssdtFunc)
{
	SSDTFUNC *pTemp=(PSSDTFUNC)malloc(sizeof(SSDTFUNC));
	if(pTemp==NULL)
	{
		OutputDebugStringA("malloc error");
		return FALSE;
	}
	memset(pTemp->ssdtFuncName,0,256);
	strcpy_s(pTemp->ssdtFuncName,ssdtFuncName);
	pTemp->MyssdtFunc=MyssdtFunc;
	pTemp->Next=pHead;
	pHead=pTemp;
	return TRUE;
}
VOID FreeMemory()
{
	while(pHead->Next!=NULL)
	{
		PSSDTFUNC pTemp=(PSSDTFUNC)pHead->Next;
		free(pHead);
		pHead=pTemp;
	}
}