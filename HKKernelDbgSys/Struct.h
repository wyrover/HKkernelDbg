#pragma once
extern "C"
{
#include "ntddk.h"
};
#include "windef.h"
typedef struct _KAPC_STATE{

	LIST_ENTRY ApcListHead[2];
	ULONG Process         ;
	UCHAR KernelApcInProgress ;
	UCHAR KernelApcPending;
	UCHAR UserApcPending ;
}KAPC_STATE,*PKAPC_STATE;
/////////////////////////////////
//_MYKTHREAD=_KTHREAD
//////////////////////////////////
typedef struct _MYKTHREAD {
	DISPATCHER_HEADER   Header; 
	LIST_ENTRY  MutantListHead; 
	PVOID  InitialStack  ;     
	PVOID  StackLimit     ;    
	PVOID  Teb          ;      
	PVOID  TlsArray      ;    
	PVOID  KernelStack    ;    
	UCHAR  DebugActive  ;      
	UCHAR  State         ;   
	UCHAR  Alerted [2]; 
	UCHAR  Iopl     ;          
	UCHAR  NpxState    ;      
	CHAR  Saturation   ;     
	CHAR  Priority       ;    
	KAPC_STATE  ApcState;
	UINT  ContextSwitches    ;
	UCHAR  IdleSwapBlock   ;   
	UCHAR  VdmSafe       ;     
	UCHAR  Spare0 [2] ;
	INT32  WaitStatus   ;     
	UCHAR  WaitIrql      ;    
	CHAR  WaitMode      ;     
	UCHAR  WaitNext      ;    
	UCHAR  WaitReason   ;      
	PKWAIT_BLOCK  WaitBlockList ;
	union{    
		LIST_ENTRY  WaitListEntry    ;  
		SINGLE_LIST_ENTRY  SwapListEntry ; 
	};    
	UINT WaitTime   ;        
	CHAR  BasePriority  ;   
	UCHAR  DecrementCount   ;  
	CHAR  PriorityDecrement ; 
	CHAR  Quantum      ;     
	KWAIT_BLOCK  WaitBlock [4] ;
	PVOID  LegoData     ;     
	UINT KernelApcDisable  ; 
	UINT UserAffinity    ;   
	UCHAR  SystemAffinityActive  ; 
	UCHAR  PowerState     ;    
	UCHAR  NpxIrql       ;     
	UCHAR  InitialNode    ;    
	PVOID  ServiceTable  ;     
	UINT  Queue      ;       
	UINT ApcQueueLock  ;    
	KTIMER  Timer       ;       
	LIST_ENTRY QueueListEntry    ; 
	UINT SoftAffinity  ;    
	UINT  Affinity       ;   
	UCHAR  Preempted   ;       
	UCHAR  ProcessReadyQueue  ; 
	UCHAR  KernelStackResident ; 
	UCHAR NextProcessor   ; 
	PVOID  CallbackStack   ;   
	PVOID Win32Thread ;      
	UINT  TrapFrame   ;      
	PKAPC_STATE  ApcStatePointer[2]; 
	CHAR  PreviousMode ;     
	UCHAR  EnableStackSwap;
	UCHAR  LargeStack ;       
	UCHAR  ResourceIndex;      
	UINT  KernelTime;
	UINT  UserTime      ;     
	KAPC_STATE  SavedApcState    ;  
	UCHAR  Alertable     ;     
	UCHAR  ApcStateIndex   ;   
	UCHAR  ApcQueueable    ;   
	UCHAR  AutoAlignment    ; 
	PVOID  StackBase      ;    
	KAPC  SuspendApc     ;    
	KSEMAPHORE  SuspendSemaphore   ;
	LIST_ENTRY  ThreadListEntry   ; 
	CHAR  FreezeCount   ;     
	CHAR  SuspendCount   ;    
	UCHAR  IdealProcessor ;   
	UCHAR  DisableBoost    ;  


}MYTHREAD,*PMYTHREAD;