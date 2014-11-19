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

void CDGClient::ProcessReceiveMsg(char* pHeader, char* pData, int iDataLen)
{
	switch (((PClientSocketHead)pHeader)->usIdent)
	{
	case CM_PING:
		SendToClient(CM_PING, nullptr, 0);
		break;
	case CM_SELECT_SERVER_OLD:
		OpenWindow(cwMessageBox, 0, "客户端版本不正确，请先更新！");
		ForceClose();
		break;
	case CM_SELECT_SERVER:
		CMSelectServer(pData, iDataLen);
		break;
	case CM_CLOSE_WINDOW:
		CMCloseWindow(pData, iDataLen);
		break;
	case CM_QUIT:
		ForceClose();
		break;
	default:
		//----------------------------------------------------
		//----------------------------------------------------
		//----------------------------------------------------
		//----------------------------------------------------
		//Log(Format('收到未知客户端协议，IP=%s Ident=%d', [RemoteAddress, Ident]), lmtWarning);
		Log("收到未知客户端协议", lmtWarning);
		break;
	}
}

void CDGClient::SocketRead(const char* pBuf, int iCount)
{
	if (m_ulForceCloseTick > 0)
		return;
	if (iCount >= sizeof(TClientSocketHead))
	{
		PClientSocketHead pHead = (PClientSocketHead)pBuf;
		if (pHead->usPackageLen < sizeof(TClientSocketHead))
			return;
		if (CS_SEGMENTATION_CLIENTSIGN == pHead->ulSign)
			m_iClientType = 1;
		else if (CS_SEGMENTATION_CLIENTSIGN + 1 == pHead->ulSign)
			m_iClientType = 2;
		else
			return;

		char* pData = (char*)pHead + sizeof(TClientSocketHead);
		int iDataLen = pHead->usPackageLen - sizeof(TClientSocketHead);
		ProcessReceiveMsg((char*)pHead, pData, iDataLen);
	}
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
	m_ucNetType = 0;
	if (sizeof(int) == usBufLen)
	{
		m_usSelectMaskServerID = *((int*)pBuf);
	}
	else if (sizeof(TCMSelectServer) == usBufLen)
	{
		m_usSelectMaskServerID = ((PCMSelectServer)pBuf)->iMaskServerID;
		//---------------------------------------------------------
		//---------------------------------------------------------
		//---------------------------------------------------------
		//m_ucNetType = G_GateSocket.GetNetType(((PCMSelectServer)pBuf)->iMaskServerID);
	}
	else
	{
		OpenWindow(cwMessageBox, 0, "选择服务器失败");
		std::string temps("选服务器失败: Area = ");
		temps.append(to_string(m_usSelectMaskServerID));
		Log(temps.c_str());
		return;
	}

	//-------------------------------------------------
	//-------------------------------------------------
	//-------------------------------------------------
	//对资源服务器进行负载均衡
	//m_iSelectRealServerID = G_DBSocket.SelectServer(this);
	if (m_iSelectRealServerID > 0)
	{
		OpenWindow(cwWarRule, 0, War_Warning);
		int iMinCount = 100000;
		TServerAddress address;
		memset(&address, 0, sizeof(TServerAddress));
		for (int i = 0; i < MAX_RESSERVER_COUNT; i++)
		{
			if (0 == G_ResServerInfos[i].Addr.iPort)
				break;

			if (iMinCount > G_ResServerInfos[i].iConnectCount)
			{
				iMinCount = G_ResServerInfos[i].iConnectCount;
				address = G_ResServerInfos[i].Addr;
			}
		}
		if (address.iPort > 0)
			SendToClient(SCM_RESSERVER_INFO, (char*)&address, sizeof(TServerAddress));
	}
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

