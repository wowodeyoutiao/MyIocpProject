/**************************************************************************************
@author: 陈昌
@content: iocp库的测试用例---服务器
**************************************************************************************/

#include "CIocpSampleServer.h"
#include <tchar.h>
#include <iostream>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])   
{    
	if (DoInitialWinSocket())
	{
		CSampleServerManager sampleServer;
		sampleServer.InitialWorkThread();
		if (!sampleServer.IsActive())
		{
			sampleServer.m_sLocalIP = "0.0.0.0";
			sampleServer.m_iListenPort = 7001;
			sampleServer.Open();

			HANDLE m_Event = CreateEvent(nullptr, false, false, nullptr);
			while (true)
			{
				WaitForSingleObject(m_Event, 100);
			}
		}
	}
	else
	{
		std::cout << "DoInitialWinSocket Fail!" << std::endl;
	}
	DoFinalizeWinSocket();
	return 0;   
}