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

			HANDLE m_TempEvent = CreateEvent(nullptr, false, false, nullptr);
			int icount = 0;
			while (icount < 100)
			{
				WaitForSingleObject(m_TempEvent, 100);
				icount += 1;
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