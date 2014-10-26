/**************************************************************************************
@author: 陈昌
@content: iocp库的测试用例---客户端
**************************************************************************************/

#include "CIocpSampleClient.h"
#include <tchar.h>
#include <iostream>

using namespace std;

void DoRunThread()
{
	CSampleClientManager sampleServer;
	sampleServer.ConnectToServer("127.0.0.1", 7001);
	sampleServer.InitialWorkThread();
	HANDLE m_Event = CreateEvent(nullptr, false, false, nullptr);
	while (true)
	{
		WaitForSingleObject(m_Event, 100);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::thread* pClientThreads[10];
	if (DoInitialWinSocket())
	{
		for (int i = 0; i < 10; i++)
		{
			pClientThreads[i] = new std::thread(DoRunThread);
		}

		HANDLE m_Event = CreateEvent(nullptr, false, false, nullptr);
		while (true)
		{
			WaitForSingleObject(m_Event, 100);
		}
	}
	else
	{
		std::cout << "DoInitialWinSocket Fail!" << std::endl;
	}
	DoFinalizeWinSocket();
	return 0;
}