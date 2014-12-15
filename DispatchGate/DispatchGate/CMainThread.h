/**************************************************************************************
@author: 陈昌
@content: 主线程单元
**************************************************************************************/
#ifndef __CC_DISPATCH_GATE_MAIN_THREAD_H__
#define __CC_DISPATCH_GATE_MAIN_THREAD_H__

#include "stdafx.h"

/**
*
* CExecutableBase的子类----主线程(无子类继承，DoExecute不作为虚函数)
*
*/
class CMainThread : public CExecutableBase
{
public:
	CMainThread(const std::string &sServerName);
	virtual ~CMainThread();
	void DoExecute();
public:
	CC_UTILS::CLogSocket* m_pLogSocket;  //日志管理类
private:
	void CheckConfig(const unsigned long ulTick);
private:
	unsigned long m_ulSlowRunTick;       //慢速执行tick
	unsigned long m_ulCheckConfigTick;   //config文件检测
	int m_iConfigFileAge;                //记录config文件的版本号
};

extern CMainThread* pG_MainThread;

void Log(const std::string& sInfo, byte loglv = 0);

#endif //__CC_DISPATCH_GATE_MAIN_THREAD_H__