#include "DeviceIoControlFunc.h"
BOOL IsAlreadyInit=FALSE;
DWORD KernelBaseInDll=NULL;//Dll中的副本内核代码地址，用于COPY一份到内核
DWORD KernelSizeInMemory=NULL;//内核在内存中的大小
DWORD PoolType=-1;

LPVOID KernelCopyAddr=NULL;//内核地址中副本内核的地址
DWORD KernelAddr=0;//真实内核的地址

VOID CtrlCodeFunc_IoRequest(\
	PIRP pIrp,\
	PIO_STACK_LOCATION pstack,\
	PULONG pinfo\
	)
{           

    *pinfo=4;//OutBuf的输出大小，OutBuf[0]=内核函数执行返回的结果，只能为0或者1
    NTSTATUS status=0xeeeeeeee;//随意定义了一个错误
	DWORD FuncAddr=0;//我的copy的内核函数的地址；
	int* InBuf=(int*)pIrp->AssociatedIrp.SystemBuffer;
	int* OutBuf=(int*)pIrp->AssociatedIrp.SystemBuffer;
	if(!IsAlreadyInit)
	{
		DbgPrint("初始化尚未完成或者失败了");
		OutBuf[0]=status;
		return;
	}
	DWORD ArgNum=InBuf[0];//函数参数的个数
	DWORD FuncOffset=InBuf[ArgNum+1];//最后一个INBUF是函数的偏移
	BOOL ShouldBeInKernelMode=InBuf[ArgNum+2];

//	DbgPrint("ArgNum=%d",ArgNum);


	if(FuncOffset==NULL)
	{
		DbgPrint("函数偏移为0");
		OutBuf[0]=FALSE;
		return;
	}
	//真实的地址等于偏移加上副本内核的地址
	FuncAddr=FuncOffset+(DWORD)KernelCopyAddr;
//	DbgPrint("FuncAddr=0x%08x");
	//函数调用，并获取内核函数的返回的status
	PMYTHREAD ptk;
	char Cur_Mode;
	ptk=(PMYTHREAD)::KeGetCurrentThread();
	Cur_Mode=ptk->PreviousMode;//保存当前状态
	if(ShouldBeInKernelMode)
	{
		ptk->PreviousMode=KernelMode;//变为内核态
	}
	else
	{
		ptk->PreviousMode=UserMode;//变为用户态
	}

           __asm pushad
		   __asm
		   {
			   mov ecx,ArgNum
			   mov eax,InBuf
PushNextArg:
			   mov edx,ecx
			   shl edx,2
			   add eax,edx
			   mov ebx,[eax]
			   push ebx
			   sub eax,edx
			   dec ecx
			   cmp ecx,0
			   jne PushNextArg
			   call FuncAddr
			   mov status,eax

		   }
		   __asm popad

		   ptk->PreviousMode=Cur_Mode;//恢复之前的状态

		   if(NT_ERROR(status))
		   {
			   DbgPrint("指定的函数调用失败在内核中 error=0x%x",status);
		       
		   }
	       OutBuf[0]=status;

}

VOID CtrlCodeFunc_IoInit(\
	PIRP pIrp,\
	PIO_STACK_LOCATION pstack,\
	PULONG pinfo\
	)
{
	BOOL bRet=FALSE;
	if(IsAlreadyInit)
	{
		DbgPrint("初始化过程只需要进行一次");
		return ;
	}
	int* InBuf=(int*)pIrp->AssociatedIrp.SystemBuffer;
	KernelBaseInDll=InBuf[0];
	KernelSizeInMemory=InBuf[1];


//	DbgPrint("kernelBaseindll=0x%08x",KernelBaseInDll);
//	DbgPrint("KernelSizeInMemory=0x%08x",KernelSizeInMemory);
	if(KernelCopyAddr==NULL||KernelAddr==NULL)
	{
		DbgPrint("Init派遣函数中KernelCopyAddr为空或者内核基地址为空");
		return ;
	}

	KIRQL c_irql=::KeGetCurrentIrql();
	//PKIRQL p_OldIrql;
//	DbgPrint("current irql 派遣函数=0x%x",c_irql);

	//开始内核的COPY
  if(c_irql>=DISPATCH_LEVEL)
  {
	  DbgPrint("current_irql>=DISPATCH_LEVEL in INIT Function");
	  goto endcopy;
  }

  RtlCopyBytes(KernelCopyAddr,(VOID*)KernelBaseInDll,KernelSizeInMemory);
	//以真实内核的基址（KernelAddr）重定位副本（KernelCopyAddr）的数据
	bRet=::DataRelocated((DWORD)KernelCopyAddr,KernelAddr);
	if(bRet)
	{
	  ::IsAlreadyInit=TRUE;
	}
	else
	{
		DbgPrint("重定位表修改错误");
	}  
endcopy:
	DbgPrint("初始化状态=%d   (1表示成功 0表示失败)",IsAlreadyInit);

	int* OutBuf=(int*)pIrp->AssociatedIrp.SystemBuffer;
}