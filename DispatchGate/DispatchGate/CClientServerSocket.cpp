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
CDGClient::CDGClient() :m_ulLastConnectTick(GetTickCount()), m_ulForceCloseTick(0), m_usSelectMaskServerID(0), m_iEncodeIdx(0),
						m_iSelectRealServerID(0), m_bIsGMIP(false), m_iClientType(0), m_ucNetType(0)
{
	SendDebugString("CDGClient 创建");
}

CDGClient::~CDGClient()
{
	SendDebugString("CDGClient 销毁");
}

unsigned short CDGClient::GetSelectMaskServerID()
{
	return m_usSelectMaskServerID;
}

int CDGClient::GetSelectRealServerID()
{
	return m_iSelectRealServerID;
}

bool CDGClient::GetIsGMIP()
{
	return m_bIsGMIP;
}

int CDGClient::GetEncodeIdx()
{
	return m_iEncodeIdx;
}

int CDGClient::GetClientType()
{
	return m_iClientType;
}

unsigned char CDGClient::GetNetType()
{
	return m_ucNetType;
}

void CDGClient::ForceClose()
{
	m_ulForceCloseTick = GetTickCount();
}

void CDGClient::Execute(unsigned long ulTick)
{
	CClientConnector::Execute(ulTick);
	if (((ulTick > m_ulLastConnectTick) && (ulTick - m_ulLastConnectTick > MAX_CONNECT_TIMEOUT)) ||
		((m_ulForceCloseTick > 0) && (ulTick > m_ulForceCloseTick) && (ulTick - m_ulForceCloseTick > DELAY_DISCONNECT_TIME)))
	{
		Close();
	}

	if (ulTick < m_ulLastConnectTick)
		m_ulLastConnectTick = ulTick;
}

void CDGClient::SocketRead(const char* pBuf, int iCount)
{
	SendDebugString("CSampleConnector 读取数据");
}

void CDGClient::OpenWindow(TClientWindowType wtype, int iParam, const std::string& msg)
{
	int iStrLen = msg.length();
	int iDataLen = sizeof(TClientWindowRec)+iStrLen + 1;
	char* pData = (char*)malloc(iDataLen);
	try
	{
		memset(pData, 0, iDataLen);
		((PClientWindowRec)pData)->WinType = wtype;
		((PClientWindowRec)pData)->Param = iParam;
		if (iStrLen > 0)
		{
			memcpy((void*)pData[sizeof(TClientWindowRec)], msg.c_str(), iStrLen);
			pData[iDataLen - 1] = '\0';
		}
		SendToClient(SCM_OPEN_WINDOW, pData, iDataLen);
		free(pData);
	}
	catch (...)
	{
		free(pData);
	}
}

void CDGClient::CMSelectServer(char* pBuf, unsigned short usBufLen)
{

}

void CDGClient::CMCloseWindow(char* pBuf, unsigned short usBufLen)
{
	if (usBufLen >= sizeof(TClientWindowRec))
	{
		if (cwWarRule == ((PClientWindowRec)pBuf)->WinType)
		{
			//--------------------------------
			//--------------------------------
			//--------------------------------
			//G_DBSocket.SendSelectServer(Self);
		}
	}
}

void CDGClient::SendToClient(unsigned short usIdent, char* pData, unsigned short usDataLen)
{
	unsigned short usBufLen = sizeof(TClientSocketHead)+usDataLen;
	char* pBuf = (char*)malloc(usBufLen);
	try
	{
		((PClientSocketHead)pBuf)->ulSign = CS_SEGMENTATION_CLIENTSIGN;
		((PClientSocketHead)pBuf)->usPackageLen = usBufLen;
		((PClientSocketHead)pBuf)->usIdent = usIdent;
		((PClientSocketHead)pBuf)->ulIdx = 0;
		if (usDataLen > 0)
		{
			memcpy((void*)pBuf[sizeof(TClientSocketHead)], pData, usDataLen);
		}		
		SendBuf(pBuf, usBufLen);
		free(pBuf);
	}
	catch (...)
	{
		free(pBuf);
	}
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
