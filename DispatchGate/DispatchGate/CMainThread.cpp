/**************************************************************************************
@author: 陈昌
@content: 主线程单元
**************************************************************************************/
#include "stdafx.h"
#include "CMainThread.h"

/************************Start Of CMainThread**************************************************/
CMainThread::CMainThread(const std::string &sServerName) : m_ulSlowRunTick(0), m_ulCheckConfigTick(0), m_iConfigFileAge(0)
{
}

CMainThread::~CMainThread()
{
	WaitThreadExecuteOver();

}

void CMainThread::CheckConfig(const unsigned long ulTick)
{	
	if ((0 == m_ulCheckConfigTick) || (ulTick - m_ulCheckConfigTick >= 30 * 1000))
	{
		m_ulCheckConfigTick = ulTick;	
		//aaaa = 100;
		int iAge = CC_UTILS::GetFileAge(G_ConfigFileName);
	}

}

void CMainThread::DoExecute()
{

}
/************************End Of CMainThread****************************************************/


void Log(std::string &str, byte loglv)
{

}