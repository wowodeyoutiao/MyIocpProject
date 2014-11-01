/**************************************************************************************
@author: 陈昌
@content: iocp库的测试用例---服务器
**************************************************************************************/

#include "CIocpSampleServer.h"
#include <tchar.h>
#include <iostream>

using namespace std;

CSampleServerManager* pSampleServer = nullptr;

bool DoAppStart(void* Sender)
{
	pSampleServer = new CSampleServerManager;
	pSampleServer->InitialWorkThread();
	if (!pSampleServer->IsActive())
	{
		pSampleServer->m_sLocalIP = "0.0.0.0";
		pSampleServer->m_iListenPort = 7001;
		pSampleServer->Open();
	}
	return true;
}

bool DoAppStop(void* Sender)
{
	delete pSampleServer;
	return true;
}


int _tmain(int argc, _TCHAR* argv[])   
{    
	HINSTANCE hWindowsServiceDll;
	TServiceManagerFunc ServiceManagerFunc;
#ifdef DEBUG
	hWindowsServiceDll = LoadLibrary("CCWindowsService_Debug.dll");
#else
	hWindowsServiceDll = LoadLibrary("CCWindowsService.dll");
#endif
	if (hWindowsServiceDll != nullptr)
	{
		if (DoInitialWinSocket())
		{
			ServiceManagerFunc = (TServiceManagerFunc)GetProcAddress(hWindowsServiceDll, "DoApplicationRun");
			if (ServiceManagerFunc != nullptr)
				ServiceManagerFunc("IOCPSampleServer2", "IOCPSampleServer_Desc", DoAppStart, DoAppStop);
			else
				std::cout << "the calling is error!" << std::endl;
		}
		else
		{
			std::cout << "DoInitialWinSocket Fail!" << std::endl;
		}
		FreeLibrary(hWindowsServiceDll);
	}
	else
	{
		std::cout << "dll is missing!" << std::endl;
	}
	return 0;   
}