/**************************************************************************************
@author: 陈昌
@content: 主线程单元
**************************************************************************************/
#include "stdafx.h"
#include "CCenterClientSocket.h"
#include "CClientServerSocket.h"
#include "CDBServerSocket.h"
#include "CPigClientSocket.h"

using namespace CC_UTILS;

CMainThread* pG_MainThread;

/************************Start Of CMainThread**************************************************/
CMainThread::CMainThread(const std::string &sServerName) : m_ulSlowRunTick(0), m_ulCheckConfigTick(0), m_iConfigFileAge(0), m_pLogSocket(nullptr)
{
	pG_DBSocket = new CDBServerSocket(sServerName);
	pG_GateSocket = new CClientServerSocket;
	pG_CenterSocket = new CCenterClientSocket;
	pG_PigSocket = new CPigClientSocket;
	m_pLogSocket = new CC_UTILS::CLogSocket(sServerName);
}

CMainThread::~CMainThread()
{
	WaitThreadExecuteOver();
	delete pG_PigSocket;
	delete pG_CenterSocket;
	delete pG_GateSocket;
	delete pG_DBSocket;
}

void CMainThread::CheckConfig(const unsigned long ulTick)
{	
	if ((0 == m_ulCheckConfigTick) || (ulTick - m_ulCheckConfigTick >= 30 * 1000))
	{
		m_ulCheckConfigTick = ulTick;	
		std::string sConfigFileName(G_CurrentExeDir + "config.ini");
		int iAge = GetFileAge(sConfigFileName);
		if ((iAge != -1) && (iAge != m_iConfigFileAge))
		{
			if (m_iConfigFileAge > 0)
				Log("Reload Config File...", lmtMessage);

			m_iConfigFileAge = iAge;
			CWgtIniFile* pIniFileParser = new CWgtIniFile();
			pIniFileParser->loadFromFile(sConfigFileName);
			try
			{
				pG_DBSocket->LoadConfig(pIniFileParser);
				pG_GateSocket->LoadConfig(pIniFileParser);
				pG_PigSocket->LoadConfig(pIniFileParser);
				pG_CenterSocket->LoadConfig(pIniFileParser);
				delete pIniFileParser;
			}
			catch (...)
			{
				delete pIniFileParser;
			}
		}
	}
}

void CMainThread::DoExecute()
{
	m_pLogSocket->InitialWorkThread();
	pG_DBSocket->InitialWorkThread();
	pG_GateSocket->InitialWorkThread();
	pG_CenterSocket->InitialWorkThread();
	pG_PigSocket->InitialWorkThread();

	Log("DispatchGate 启动.");
	unsigned long ulTick;
	while (!IsTerminated())
	{
		int iErrorCode = 1;
		try
		{
			ulTick = GetTickCount();
			if (ulTick - m_ulSlowRunTick >= 1000)
			{
				m_ulSlowRunTick = ulTick;
				CheckConfig(ulTick);
				
				pG_CenterSocket->DoHeartBeat();
				pG_PigSocket->DoHeartBeat();	
				Log("DispatchGate execute.");
			}
		}
		catch (...)
		{

		}
		WaitForSingleObject(m_Event, 10);
	}
	pG_PigSocket->Close();
	pG_CenterSocket->Close();
	pG_DBSocket->Close();
	pG_GateSocket->Close();
}
/************************End Of CMainThread****************************************************/


void Log(const std::string& sInfo, byte loglv)
{
	//----------SendDebugString(sInfo);
	if ((pG_MainThread != nullptr) && (pG_MainThread->m_pLogSocket != nullptr))
		pG_MainThread->m_pLogSocket->SendLogMsg(sInfo, loglv);
}