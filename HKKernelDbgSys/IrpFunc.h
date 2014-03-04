#pragma once
extern "C"
{
#include "ntddk.h"
}
#include "DeviceIoControlFunc.h"


NTSTATUS OtherDispatchDeviceControl(\
									PDEVICE_OBJECT  pDeviceObject,\
									PIRP  pIrp\
									);

NTSTATUS DeviceIoControlFunc(\
							PDEVICE_OBJECT  pDeviceObject,\
							PIRP  pIrp);