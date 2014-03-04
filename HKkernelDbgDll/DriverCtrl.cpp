#include "DriverCtrl.h"
BOOL LoadNtDriver(LPCTSTR lpServiceName,LPCTSTR lpFullPathSys)
{
	BOOL bRet=FALSE;
	SC_HANDLE h_Service=NULL;
	SC_HANDLE h_Scm=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(h_Scm==NULL)
	{
	   MyDbgString("OpenSCManager错误 error=%d",ERROR_CODE);
	   goto Loadend;
	}
	h_Service=CreateService( \
		h_Scm,       // handle to SCM database 
		lpServiceName,      // name of service to start
		lpServiceName,      // display name
		SERVICE_ALL_ACCESS,      // type of access to service
		SERVICE_KERNEL_DRIVER,        // type of service
		SERVICE_DEMAND_START,          // when to start service
		SERVICE_ERROR_NORMAL,       // severity of service failure
		lpFullPathSys,   // name of binary file
		NULL,   // name of load ordering group
		NULL,          // tag identifier
		NULL,     // array of dependency names
		NULL, // account name 
		NULL          // account password
		);
	if(h_Service==NULL)
	{
		DWORD dwError=ERROR_CODE;
		if(dwError!=ERROR_SERVICE_EXISTS)
		{
			MyDbgString("创建服务失败 error=%d",dwError);
			goto Loadend;
		}
		OutputDebugStringA("服务存在，只需要打开");
		h_Service=OpenService(h_Scm,lpServiceName,SERVICE_ALL_ACCESS);
		if(h_Service==NULL)
		{
			MyDbgString("打开服务失败 error=%d",ERROR_CODE);
			goto Loadend;
		}
	}

	bRet=StartService(h_Service,NULL,NULL);
	if(!bRet)
	{
		MyDbgString("启动服务失败error=%d",ERROR_CODE);
	}
	else
	{
		OutputDebugStringA("成功启动服务");
	}
		bRet=TRUE;
Loadend:

	if(h_Scm)
	{
		CloseServiceHandle(h_Scm);
	}
	if(h_Service)
	{
		CloseServiceHandle(h_Service);
	}
	return bRet;
};
BOOL UnLoadNtDriver(LPCTSTR lpServiceName)
{
	BOOL bRet=FALSE;
	SC_HANDLE h_Service=NULL;
	SC_HANDLE h_Scm=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(h_Scm==NULL)
	{
		MyDbgString("OpenSCManager错误 error=%d",ERROR_CODE);
		goto UnLoadend;
	}
	h_Service=OpenService(h_Scm,lpServiceName,SERVICE_ALL_ACCESS);
	if(h_Service==NULL)
	{
		MyDbgString("打开服务错误 UNLOAD error=%d",ERROR_CODE);
		goto UnLoadend;
	}
	SERVICE_STATUS status; 
	bRet=ControlService(h_Service,SERVICE_CONTROL_STOP,&status);
	if(!bRet)
	{
		MyDbgString("停止服务失败Unload %d",ERROR_CODE);
		goto UnLoadend;
	}
	bRet=DeleteService(h_Service);
	if(!bRet)
	{
		MyDbgString("删除服务失败Unload %d",ERROR_CODE);
		goto UnLoadend;
	}
	bRet=TRUE;
	OutputDebugStringA("卸载驱动成功");
UnLoadend:
	if(h_Scm)
	{
		CloseServiceHandle(h_Scm);
	}
	if(h_Service)
	{
		CloseServiceHandle(h_Service);
	}
	return bRet;
};