#include "IrpFunc.h"
#include "Struct.h"

NTSTATUS OtherDispatchDeviceControl(PDEVICE_OBJECT  pDeviceObject,PIRP  pIrp)
{
	pIrp->IoStatus.Status=STATUS_SUCCESS;
	IoCompleteRequest(pIrp,IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DeviceIoControlFunc(PDEVICE_OBJECT  pDeviceObject,PIRP  pIrp)
{
	/*
	为了保证每个内核函数能够正确执行。不返回0XC0000005的STATUS的问题
	PMYTHREAD 为自定义的KTHREAD，由于windows没有具体定义。我是windbg查到的
	同时为了不导出太多的结构体，我把一些结构体指针变为了DWORD，这里只需要PreviousMode
	*/


//	DbgPrint("进入IRP DeviceIoControl派遣函数");
	ULONG info=0;
	PIO_STACK_LOCATION pStackLocation=IoGetCurrentIrpStackLocation(pIrp);

	switch(pStackLocation->Parameters.DeviceIoControl.IoControlCode)
	{
		/*
		依据不同的CODE进入不同的函数执行相应的派遣函数
		*/
	case IoRequest_code:
		CtrlCodeFunc_IoRequest(pIrp,pStackLocation,&info);
		break;
	case IoInit_code:
		CtrlCodeFunc_IoInit(pIrp,pStackLocation,&info);
		break;
	}

	pIrp->IoStatus.Information=info;
	pIrp->IoStatus.Status=STATUS_SUCCESS;
	IoCompleteRequest(pIrp,IO_NO_INCREMENT);



	return STATUS_SUCCESS;
}