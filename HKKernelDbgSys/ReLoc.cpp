#include "ReLoc.h"


PIMAGE_DOS_HEADER  pDosHeader;
PIMAGE_NT_HEADERS32  pNtHeader;
PIMAGE_OPTIONAL_HEADER  pOptionalHeader;
PIMAGE_DATA_DIRECTORY  pRelocDirectory;


char* pBaseReloc;

BOOL DataRelocated(DWORD ModuleAddress,DWORD RealModuleAddress)
{
	BOOL bRet=FALSE;
	DWORD ImageBase=NULL;

	void* pRelocTableAddr=NULL;
	DWORD  RelocTableSize=NULL;

	pDosHeader=(PIMAGE_DOS_HEADER)ModuleAddress;
	pNtHeader=(PIMAGE_NT_HEADERS32)(pDosHeader->e_lfanew+ModuleAddress);
	pOptionalHeader=&(pNtHeader->OptionalHeader);

	ImageBase=pOptionalHeader->ImageBase;


    pRelocDirectory=&pOptionalHeader->DataDirectory[5];//重定位表结构是第五个

	RelocTableSize=pRelocDirectory->Size;
	pRelocTableAddr=(void *)(pRelocDirectory->VirtualAddress+ModuleAddress);

//	DbgPrint("重定位表地址为=0x%08x",pRelocTableAddr);

	if(RelocTableSize==0||(pRelocDirectory->VirtualAddress)==0)
	{
		DbgPrint("没有需要重定位的数据");
		return FALSE;
	}

	DWORD ReDataNum=0;//每个单独的定位块的需要重定位的数据的个数
	DWORD EveryAddr=0;

	pBaseReloc=(char*)pRelocTableAddr;





	do
	{
		
		ReDataNum=((PIMAGE_BASE_RELOCATION)pBaseReloc)->SizeOfBlock;//第一个块的大小
	    EveryAddr=((PIMAGE_BASE_RELOCATION)pBaseReloc)->VirtualAddress;//第一个块的虚拟地址 （没有加上基址)


		//计算得到每个单独块需要重定位的数据的个数
		ReDataNum=(ReDataNum-sizeof(IMAGE_BASE_RELOCATION))/2;

		WORD* pReData=(WORD*)((DWORD)pBaseReloc+8);

	

           for(DWORD i=0;i<ReDataNum;i++)
		   {
                if(((*pReData)>>12)==3)
				{
                    WORD NewOffset=(*pReData)&0xfff;
					DWORD Value=*(DWORD *)(NewOffset+ModuleAddress+((PIMAGE_BASE_RELOCATION)pBaseReloc)->VirtualAddress);
					Value=Value-ImageBase+RealModuleAddress;
					*(DWORD *)(NewOffset+ModuleAddress+((PIMAGE_BASE_RELOCATION)pBaseReloc)->VirtualAddress)=Value;

				}
				pReData++;

		   }
		   pBaseReloc=pBaseReloc+((PIMAGE_BASE_RELOCATION)pBaseReloc)->SizeOfBlock;

	}while(((PIMAGE_BASE_RELOCATION)pBaseReloc)->SizeOfBlock);

	return TRUE;
};