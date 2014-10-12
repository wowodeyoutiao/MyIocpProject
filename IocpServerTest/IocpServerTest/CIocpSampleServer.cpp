/**************************************************************************************
@author: 陈昌
@content: 使用IOCP服务器框架快速构建服务器
通过对 CIOCPServerSocketManager 和 CClientConnector的继承快速构建服务器
**************************************************************************************/
#include "CIocpSampleServer.h"
#pragma comment(lib, "ws2_32.lib")

/************************Start Of CSampleConnector****************************************************/
void CSampleConnector::Execute(unsigned long ulTick)
{
	if (ulTick - m_ulLastRunTick >= 10000)
	{
		m_ulLastRunTick = ulTick;
		SendDebugString("10秒执行");
	}
}

void CSampleConnector::SocketRead(const char* pBuf, int iCount)
{
	SendDebugString("CSampleConnector 读取数据");
}

CSampleConnector::CSampleConnector() :m_ulLastRunTick(0)
{
	SendDebugString("CSampleConnector 创建");
}

CSampleConnector::~CSampleConnector()
{
	SendDebugString("CSampleConnector 销毁");
}

/************************End Of CSampleConnector******************************************************/


/************************Start Of CSampleServerManager************************************************/
CSampleServerManager::CSampleServerManager()
{
	SendDebugString("CSampleServerManager 创建");
	m_OnCheckAddress = std::bind(&CSampleServerManager::OnCheckIPAddress, this, std::placeholders::_1);
	m_OnCreateClient = std::bind(&CSampleServerManager::OnCreateClientSocket, this, std::placeholders::_1);
	m_OnClientError = std::bind(&CSampleServerManager::OnSocketError, this, std::placeholders::_1, std::placeholders::_2);
	m_OnConnect = std::bind(&CSampleServerManager::OnClientConnect, this, std::placeholders::_1);
	m_OnDisConnect = std::bind(&CSampleServerManager::OnClientDisconnect, this, std::placeholders::_1);
}

CSampleServerManager::~CSampleServerManager()
{
	SendDebugString("CSampleServerManager 销毁");
}

void CSampleServerManager::DoActive()
{

}

bool CSampleServerManager::OnCheckIPAddress(const std::string& sIP)
{
	SendDebugString("CheckIPAddress");
	return true;
}

CSampleConnector* CSampleServerManager::OnCreateClientSocket(const std::string& sIP)
{
	SendDebugString("OnCreateClientSocket");
	return new CSampleConnector;
}

void CSampleServerManager::OnSocketError(void* Sender, int& iErrorCode)
{

}

void CSampleServerManager::OnClientConnect(void* Sender)
{
	SendDebugString("OnClientConnect");
}

void CSampleServerManager::OnClientDisconnect(void* Sender)
{
	SendDebugString("OnClientDisconnect");
}

/************************End Of CSampleServerManager****************************************************/
