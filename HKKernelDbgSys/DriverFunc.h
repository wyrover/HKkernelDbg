#pragma once
extern "C"
{
#include "ntddk.h"
};
#include "Struct.h"
#include "DeviceIoControlFunc.h"
#define DeviceName L"\\Device\\HKKernelDbgInKernel"
#define LinkName   L"\\??\\HKKernelDbg"
NTSTATUS MyCreateDevice(PDRIVER_OBJECT pDriverObj);
VOID DriverUnload(PDRIVER_OBJECT pDriverObj);