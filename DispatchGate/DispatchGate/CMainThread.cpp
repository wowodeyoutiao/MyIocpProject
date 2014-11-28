/**************************************************************************************
@author: 陈昌
@content: 主线程单元
**************************************************************************************/
#include "stdafx.h"

using namespace CC_UTILS;

/************************Start Of CMainThread**************************************************/
CMainThread::CMainThread(const std::string &sServerName) : m_ulSlowRunTick(0), m_ulCheckConfigTick(0), m_iConfigFileAge(0)
{
	/*
	m_LogSocket := TLogSocket.Create(ServerName);
	G_DBSocket := TDBServerSocket.Create(ServerName);
	G_GateSocket := TServerSocket.Create;
	G_CenterSocket := TCenterSocket.Create;
	G_Echo := TUDPEchoServer.Create(53);
	G_PigSocket := TPigClient.Create;
	*/
}

CMainThread::~CMainThread()
{
	WaitThreadExecuteOver();
	/*
	G_PigSocket.Free;
	G_CenterSocket.Free;
	G_GateSocket.Free;
	G_DBSocket.Free;
	G_Echo.Free;
	m_LogSocket.Free;
	*/
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
				/*
				G_DBSocket.LoadConfig(IniFile);
				G_GateSocket.LoadConfig(IniFile);
				G_PigSocket.LoadConfig(IniFile);

				增加一个 
				G_CenterServer.LoadCOnfig(IniFile);
				*/
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
				/*
				G_CenterSocket.DoHeartbeat;
				G_PigSocket.DoHeartbest;
				*/
			}
		}
		catch (...)
		{

		}
		WaitForSingleObject(m_Event, 10);
	}
	/*
	G_PigSocket.Close;
	G_GateSocket.Close;
	G_DBSocket.Close;
	*/
}
/************************End Of CMainThread****************************************************/


void Log(const std::string& sInfo, byte loglv)
{
	SendDebugString(sInfo);
}