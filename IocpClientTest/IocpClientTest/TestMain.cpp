/**************************************************************************************
@author: ³Â²ý
@content: iocp¿âµÄ²âÊÔÓÃÀý---¿Í»§¶Ë
**************************************************************************************/

#include "CIocpSampleClient.h"
#include <tchar.h>
#include <iostream>
#include <fstream>
#include "JsonObjectBase.h"

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
	//std::thread* pClientThreads[10];
	if (DoInitialWinSocket())
	{
		/*
		for (int i = 0; i < 10; i++)
		{
			pClientThreads[i] = new std::thread(DoRunThread);
		}

		HANDLE m_Event = CreateEvent(nullptr, false, false, nullptr);
		while (true)
		{
			WaitForSingleObject(m_Event, 100);
		}
		*/


		/*
		CCJsonObjectTest test;
		test.saveData.bFlag1 = 1;
		test.saveData.sName1 = "hahahaha";
		test.saveData.sName2 = "hehehehe";

		std::ofstream file1;
		file1.open("d:\jsonobjtest.txt");
		file1 << test.AsString();
		

		CCJsonObjectTest test2;
		std::ifstream file2;
		file2.open("d:\jsonobjtest.txt");
		string sTemp;
		file2 >> sTemp;
		test2.LoadFrom(sTemp);

		std::cout << test2.saveData.bFlag1 << " " << test2.saveData.bFlag2 << " " << test2.saveData.sName1 << " " << test2.saveData.sName2 << std::endl;
		*/

		char c;
		std::cin >> c;
	}
	else
	{
		std::cout << "DoInitialWinSocket Fail!" << std::endl;
	}
	DoFinalizeWinSocket();
	return 0;
}