extern "C"
{
#include <ntddk.h>
}
#include "IncAll.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegistryString)
{
	DbgPrint("进入DriverEntry函数");
	NTSTATUS status;
	
	status=MyCreateDevice(pDriverObj);
	pDriverObj->DriverUnload=DriverUnload;
	pDriverObj->MajorFunction[IRP_MJ_CREATE]=OtherDispatchDeviceControl;
	pDriverObj->MajorFunction[IRP_MJ_CLOSE]=OtherDispatchDeviceControl;
	pDriverObj->MajorFunction[IRP_MJ_READ]=OtherDispatchDeviceControl;
	pDriverObj->MajorFunction[IRP_MJ_WRITE]=OtherDispatchDeviceControl;
	//主要的通信处理函数
	pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL]=DeviceIoControlFunc;
///////
//以下获取内核基址的ASM代码是COPY 网上的。感谢原作者。
/////////

	__asm
	{
		pushad
		mov eax,DWORD PTR FS:[0x34]
		add eax,0x18
		mov eax,[eax]
		mov eax,[eax]
		mov eax,[eax+0x18]
		mov KernelAddr,eax
		popad
	}
	
//	DbgPrint("真实的内核地址为=0x%08x",KernelAddr);
	PIMAGE_DOS_HEADER  pDosHeader;
	PIMAGE_NT_HEADERS32  pNtHeader;
	PIMAGE_OPTIONAL_HEADER  pOptionalHeader;

	pDosHeader=(PIMAGE_DOS_HEADER)KernelAddr;
	pNtHeader=(PIMAGE_NT_HEADERS32)(pDosHeader->e_lfanew+KernelAddr);
	pOptionalHeader=&(pNtHeader->OptionalHeader);
	//根据实际的内核加载后的大小申请内存
	DWORD SizeOfImage=pOptionalHeader->SizeOfImage;
//	DbgPrint("Sizeofimage=0x%08x",SizeOfImage);
	KIRQL c_irql=::KeGetCurrentIrql();
//	DbgPrint("current irql driver entry=0x%x",c_irql);
	if(c_irql==DISPATCH_LEVEL)
	{	
		PoolType=NonPagedPool;
		KernelCopyAddr=ExAllocatePool(NonPagedPool,SizeOfImage);
		RtlZeroMemory(KernelCopyAddr,SizeOfImage);
	}
	else if(c_irql<DISPATCH_LEVEL)
	{
		PoolType=PagedPool;
		KernelCopyAddr=ExAllocatePool(PagedPool,SizeOfImage);
		RtlZeroMemory(KernelCopyAddr,SizeOfImage);
	}
	else
	{
		DbgPrint("Current IRQL>DISPATCH_LEVEL in DriverEntry");
	}

//	DbgPrint("POOLTYPE=%d",PoolType);

//	DbgPrint("分配到的内存为0x%08x",KernelCopyAddr);
	if(KernelCopyAddr==NULL)
	{
		DbgPrint("分配内核内存错误");
	}
	if(!NT_SUCCESS(status))
	{
		return status;
	}
	return STATUS_SUCCESS;
}
