/**************************************************************************************
@author: 陈昌
@content: Dispatch对客户端连接的监听socket管理
**************************************************************************************/
#include "stdafx.h"
#include "CClientServerSocket.h"

const std::string IPCONFIG_FILE = "ipaddress.txt";        // IP配置文件
const int MAX_CONNECT_TIMEOUT = 30 * 1000;                // 最长的连接时间
const int DELAY_DISCONNECT_TIME = 3000;                   // 延时断开时间
const std::string War_Warning = "  本游戏区是自由对战模式，遵守游戏规则，可获得畅快游戏体验。</br>  龙界争霸现已开启<font color=\"0xFFDD0000\">防沉迷系统</font>，详细情况请关注游戏官网信息。";

/************************Start Of CDGClient********************************************************/
void CDGClient::Execute(unsigned long ulTick)
{
	if (ulTick - m_ulLastRunTick >= 10000)
	{
		m_ulLastRunTick = ulTick;
		SendDebugString("10秒执行");
	}
}

void CDGClient::SocketRead(const char* pBuf, int iCount)
{
	SendDebugString("CSampleConnector 读取数据");
}

CDGClient::CDGClient() :m_ulLastRunTick(0)
{
	SendDebugString("CSampleConnector 创建");
}

CDGClient::~CDGClient()
{
	SendDebugString("CSampleConnector 销毁");
}

/************************End Of CDGClient***********************************************************/


/************************Start Of CClientServerSocket************************************************/
CClientServerSocket::CClientServerSocket()
{
	SendDebugString("CSampleServerManager 创建");
	m_OnCheckAddress = std::bind(&CClientServerSocket::OnCheckIPAddress, this, std::placeholders::_1);
	m_OnCreateClient = std::bind(&CClientServerSocket::OnCreateClientSocket, this, std::placeholders::_1);
	m_OnClientError = std::bind(&CClientServerSocket::OnSocketError, this, std::placeholders::_1, std::placeholders::_2);
	m_OnConnect = std::bind(&CClientServerSocket::OnClientConnect, this, std::placeholders::_1);
	m_OnDisConnect = std::bind(&CClientServerSocket::OnClientDisconnect, this, std::placeholders::_1);
}

CClientServerSocket::~CClientServerSocket()
{
	SendDebugString("CSampleServerManager 销毁");
}

void CClientServerSocket::DoActive()
{

}

bool CClientServerSocket::OnCheckIPAddress(const std::string& sIP)
{
	SendDebugString("CheckIPAddress");
	return true;
}

CDGClient* CClientServerSocket::OnCreateClientSocket(const std::string& sIP)
{
	SendDebugString("OnCreateClientSocket");
	return new CDGClient;
}

void CClientServerSocket::OnSocketError(void* Sender, int& iErrorCode)
{

}

void CClientServerSocket::OnClientConnect(void* Sender)
{
	SendDebugString("OnClientConnect");
}

void CClientServerSocket::OnClientDisconnect(void* Sender)
{
	SendDebugString("OnClientDisconnect");
}

/************************End Of CClientServerSocket****************************************************/
