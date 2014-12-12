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
		test.saveData.bFlag1 = true;
		test.saveData.sName1 = "hahahaha";
		test.saveData.sName2 = "yyyyyyyy";

		std::ofstream file1;
		file1.open("e:\\aaa.txt");
		file1 << test.AsString();
		file1.close();

		
		CCJsonObjectTest test2;
		std::ifstream file2;
		file2.open("e:\\aaa.txt");
		string sTemp;
		file2 >> sTemp;
		test2.LoadFrom(sTemp);

		std::cout << test2.saveData.bFlag1 << " " << test2.saveData.bFlag2 << " " << test2.saveData.sName1 << " " << test2.saveData.sName2 << std::endl;
		*/

		TSaveTestData tempData;
		tempData.iNum1 = 100;
		tempData.iNum2 = 900;
		tempData.sName1 = "aaa";
		tempData.sName2 = "xxx";

		CCJsonObjectTestEx testex;
		testex.saveDataEx.iNum1 = 100;
		testex.saveDataEx.iNum2 = 200;
		testex.saveDataEx.dataEx = tempData;
		int iArrayLen = sizeof(testex.saveDataEx.IntArrayData) / sizeof(testex.saveDataEx.IntArrayData[0]);
		for (int i = 0; i < iArrayLen; i++)
		{
			testex.saveDataEx.IntArrayData[i] = i;
		}
		iArrayLen = sizeof(testex.saveDataEx.dataExArray) / sizeof(testex.saveDataEx.dataExArray[0]);
		for (int i = 0; i < iArrayLen; i++)
		{
			tempData.iNum1 += 1;
			tempData.iNum2 += 1;
			testex.saveDataEx.dataExArray[i] = tempData;
		}

		std::ofstream file1;
		file1.open("e:\\aaa.txt");
		unsigned long tick = GetTickCount();
		file1 << testex.AsString() << std::endl;
		file1.close();
		unsigned long tick1 = GetTickCount();
		std::cout << tick1 - tick << std::endl;

		CCJsonObjectTestEx test2;
		std::ifstream file2;
		string sTemp;
		file2.open("e:\\aaa.txt");
		file2 >> sTemp;
		test2.LoadFrom(sTemp);
		file2.close();
		std::cout << test2.saveDataEx.iNum1 << "-" << test2.saveDataEx.iNum2 << "-" << test2.saveDataEx.dataEx.sName1 << "-" << test2.saveDataEx.dataEx.bFlag1 << "-"
			<< test2.saveDataEx.IntArrayData[1] << "-" << test2.saveDataEx.IntArrayData[99] << std::endl;
		std::cout << test2.saveDataEx.dataExArray[0].iNum1 << "-" << test2.saveDataEx.dataExArray[0].iNum2 << "-" << test2.saveDataEx.dataExArray[5].iNum1 << "-"
			<< test2.saveDataEx.dataExArray[5].iNum2 << "-" << test2.saveDataEx.dataExArray[9].sName1 << "-" << test2.saveDataEx.dataExArray[9].sName2 << std::endl;

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