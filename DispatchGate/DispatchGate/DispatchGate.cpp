/**************************************************************************************
@author: 陈昌
@content: DispatchGate的Main单元，加载CCWindowsService.dll，启动主线程
**************************************************************************************/

#include "stdafx.h"

using namespace CC_UTILS;

char DEFAULT_SERVICE_NAME[] = "DispatchGate";
char DEFAULT_DESCRIPTION[] = "LongGet 游戏登录网关服务";

bool DoAppStart(void* Sender)
{
	pG_MainThread = new CMainThread(DEFAULT_SERVICE_NAME);
	pG_MainThread->InitialWorkThread();
	return true;
}

bool DoAppStop(void* Sender)
{
	delete pG_MainThread;
	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HINSTANCE hWindowsServiceDll;
	TServiceManagerFunc ServiceManagerFunc;
#ifdef DEBUG
	hWindowsServiceDll = LoadLibrary("CCWindowsService_Debug.dll");
#else
	hWindowsServiceDll = LoadLibrary("CCWindowsService_Debug.dll");
#endif
	if (hWindowsServiceDll != nullptr)
	{
		if (DoInitialWinSocket())
		{
			G_CurrentExeFileName = argv[0];
			G_CurrentExeDir = "E:\\MyProject\\C++Project\\MyIocpProject\\DispatchGate\\Debug";

			ServiceManagerFunc = (TServiceManagerFunc)GetProcAddress(hWindowsServiceDll, "DoApplicationRun");
			if (ServiceManagerFunc != nullptr)
				ServiceManagerFunc(DEFAULT_SERVICE_NAME, DEFAULT_DESCRIPTION, DoAppStart, DoAppStop);
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