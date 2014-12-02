/**************************************************************************************
@author: 陈昌
@content: Dispatch对DB服务器连接的监听socket管理
**************************************************************************************/
#include "stdafx.h"
#include "CDBServerSocket.h"

using namespace CC_UTILS;

/************************Start Of CDBConnector******************************************/
CDBConnector::CDBConnector() :m_iServerID(0), m_iPlayerCount(0), m_DefaultRule(itDeny), m_sDenyHint("服务器目前未开放，请等候"), m_sServerName("")
{
}

CDBConnector::~CDBConnector()
{
	ClearIPRule(itUnKnow);
}

int CDBConnector::GetServerID()
{
	return m_iServerID;
}

int CDBConnector::GetPlayerCount()
{
	return m_iPlayerCount;
}

std::string& CDBConnector::GetServerName()
{
	return m_sServerName;
}

void CDBConnector::SendToClientPeer(unsigned short usIdent, int iParam, void* pBuf, unsigned short usBufLen)
{
	int iDataLen = sizeof(TServerSocketHeader) + usBufLen;
	char* pData = (char*)malloc(iDataLen);
	if (pData != nullptr)
	{
		try
		{
			((PServerSocketHeader)pData)->ulSign = SS_SEGMENTATION_SIGN;
			((PServerSocketHeader)pData)->usIdent = usIdent;
			((PServerSocketHeader)pData)->iParam = iParam;
			((PServerSocketHeader)pData)->usBehindLen = usBufLen;
			if (usBufLen > 0)
				memcpy(pData + sizeof(TServerSocketHeader), pBuf, usBufLen);

			SendBuf(pData, iDataLen);
			free(pData);
		}
		catch (...)
		{
			free(pData);
		}
	}
}

void CDBConnector::AddIpRuleNode(const std::string& sIP, TIpType ipType)
{
	if ((itDeny == ipType) && ("all" == sIP))
	{
		m_DefaultRule = itDeny;
		return;
	}
	std::string sTempStr = sIP;
	std::vector<std::string> vec;
	SplitStr(sTempStr, "|", &vec);

	std::string sTempIP;
	std::vector<std::string>::iterator vIter;
	for (vIter = vec.begin(); vIter != vec.end(); ++vIter)
	{
		//这里的字符串处理需要再检查---------------------
		//这里的字符串处理需要再检查---------------------
		//这里的字符串处理需要再检查---------------------
		int iPos = (*vIter).find('*');
		if (iPos != string::npos)
			sTempIP = (*vIter).substr(0, iPos - 1);
		else
			sTempIP = (*vIter);

		if ("" == sTempIP)
			return;
		if (sTempIP.at(sTempIP.length()) != '.')
			sTempIP = sTempIP + '.';

		std::lock_guard<std::mutex> guard(m_IPRuleLockCS);
		PIpRuleNode pNode = new TIpRuleNode;
		pNode->ipType = ipType;
		pNode->sMatchIP = sIP;
		m_IPRuleList.push_back(pNode);
	}
}

bool CDBConnector::CheckClientIP(const std::string& sIP)
{
	bool retFlag = (m_DefaultRule != itDeny);
	std::string sTempIP(sIP);
	sTempIP.append(".");

	PIpRuleNode pNode;
	std::list<PIpRuleNode>::iterator vIter;
	std::lock_guard<std::mutex> guard(m_IPRuleLockCS);
	for (vIter = m_IPRuleList.begin(); vIter != m_IPRuleList.end(); ++vIter)
	{
		pNode = (PIpRuleNode)*vIter;
		//--------------------------------------
		//--------------------------------------
		//这个判断需要调试检验一下
		if (sTempIP.find(pNode->sMatchIP) == 1)
		{
			switch (pNode->ipType)
			{
			case itDeny:
				retFlag = false;
				break;
			case itAllow:
			case itMaster:
				retFlag = true;
				break;
			default:
				retFlag = false;
				break;
			}
		}
	}
	return retFlag;
}

void CDBConnector::SocketRead(const char* pBuf, int iCount)
{
	//在基类解析外层数据包，并调用ProcessReceiveMsg完成逻辑消息处理
	int iErrorCode = ParseSocketReadData(1, pBuf, iCount);
	if (iErrorCode > 0)
	{
		std::string temps("TDBServer Socket Read Error, Code = ");
		temps.append(to_string(iErrorCode));
		Log(temps, lmtError);
	}
}

void CDBConnector::ProcessReceiveMsg(PServerSocketHeader pHeader, const char* pData, int iDataLen)
{
	switch (pHeader->usIdent)
	{
	case SM_PING:
		SendHeartBeat(pHeader->iParam);
		break;
	case SM_REGISTER:
		if (0 == pHeader->usBehindLen)
			RegisterDBServer(pHeader->iParam);
		else
			Log("DBServer版本错误", lmtError);
		break;
	case SM_SELECT_SERVER:
		//----------------------------------------------
		//----------------------------------------------
		//----------------------------------------------
		//----------------------------------------------
		//G_GateSocket.smSelectServer(nParam, PData, wBehindLen);
		break;
	case SM_SERVER_CONFIG:
		ReceiveConfig(pHeader->iParam, pData, iDataLen);
		break;
	default:
		std::string temps("收到未知DBServer协议，Ident=");
		temps.append(to_string(pHeader->usIdent));
		Log(temps, lmtWarning);
		break;
	}
}

void CDBConnector::SendHeartBeat(int iCount)
{
	//----------------------------------------------
	//----------------------------------------------
	//G_DBSocket.ShowDBMsg(FServerID, 4, IntToStr(nCount));
	m_iPlayerCount = iCount;
	SendToClientPeer(SM_PING, 0, nullptr, 0);
}

void CDBConnector::RegisterDBServer(int iServerID)
{
	//if G_DBSocket.RegisterDBServer(RemoteAddress, ServerID, Self)
	if (true)
	{
		m_DefaultRule = itAllow;
		m_iServerID = iServerID;
		//G_DBSocket.ShowDBMsg(ServerID, 3, RemoteAddress);
		//G_DBSocket.ShowDBMsg(ServerID, 4, '0');
		std::string sTemp("DBServer ");
		sTemp.append(to_string(m_iServerID));
		sTemp.append(" Enabled.");
		Log(sTemp, lmtMessage);
	}
	else
	{
		std::string sTemp("没配置的DBServer:  ");
		sTemp.append(to_string(m_iServerID));
		Log(sTemp, lmtWarning);
		Close();
	}
}

void CDBConnector::ClearIPRule(TIpType ipType)
{
	PIpRuleNode pNode;
	std::list<PIpRuleNode>::iterator vIter;

	std::lock_guard<std::mutex> guard(m_IPRuleLockCS);
	for (vIter = m_IPRuleList.begin(); vIter != m_IPRuleList.end(); )
	{
		pNode = (PIpRuleNode)*vIter;
		if ((itUnKnow == ipType) || (pNode->ipType == ipType))
		{
			pNode->sMatchIP = "";
			delete(pNode);
			vIter = m_IPRuleList.erase(vIter);
		}
		else
			++vIter;
	}
	m_IPRuleList.clear();
}

void CDBConnector::ReceiveConfig(int iParam, const char* pBuf, unsigned short usBufLen)
{
	unsigned short usConfigType = iParam & 0x7FFF;
	switch (usConfigType)
	{
	case 1:													//未开服提示 
		if ((pBuf != nullptr) && (usBufLen > 0))
			//--------------------------------------
			//--------------------------------------
			//--------------------------------------
			//这里的长度需要注意！！！
			m_sDenyHint.assign(pBuf, usBufLen);
		break;
	case 2:													//禁止IP列表
		//最高位为1表示清除所有
		if ((iParam & 0x8000) != 0)
		{
			ClearIPRule(itDeny);
			m_DefaultRule = itAllow;
		}
		if (usBufLen > 0)
		{
			std::string sTemp;
			sTemp.assign(pBuf, usBufLen);
			AddIpRuleNode(sTemp, itDeny);
		}
		break;
	case 3:													//允许IP列表
		//最高位为1表示清除所有
		if ((iParam & 0x8000) != 0)
			ClearIPRule(itAllow);
		if (usBufLen > 0)
		{
			std::string sTemp;
			sTemp.assign(pBuf, usBufLen);
			AddIpRuleNode(sTemp, itAllow);
		}
		break;
	default:
		break;
	}
}

/************************End Of CDBConnector******************************************/




/************************Start Of CDBServerSocket******************************************/

CDBServerSocket::CDBServerSocket(const std::string& sName) :m_sAllowDBServerIP(""), m_iSessionID(1000), m_sServerName("#"+sName+"#"), 
m_iConfigFileAge(0), m_ulLastCheckTick(0), m_pLogSocket(nullptr), m_ServerHash(511) 
{
	SetMaxCorpseTime(60 * 1000);
	m_ServerHash.m_RemoveEvent = std::bind(&CDBServerSocket::RemoveServerInfo, this, std::placeholders::_1, std::placeholders::_2);

	m_OnCreateClient = std::bind(&CDBServerSocket::OnCreateClientSocket, this, std::placeholders::_1);
	m_OnClientError = std::bind(&CDBServerSocket::OnSocketError, this, std::placeholders::_1, std::placeholders::_2);
	m_OnConnect = std::bind(&CDBServerSocket::OnClientConnect, this, std::placeholders::_1);
	m_OnDisConnect = std::bind(&CDBServerSocket::OnClientDisconnect, this, std::placeholders::_1);
	m_OnCheckAddress = std::bind(&CDBServerSocket::OnCheckIPAddress, this, std::placeholders::_1);
}

CDBServerSocket::~CDBServerSocket()
{
	m_ServerHash.m_RemoveEvent = nullptr;
	if (m_pLogSocket != nullptr)
		delete m_pLogSocket;
}

void CDBServerSocket::LoadConfig(CWgtIniFile* pIniFileParser)
{
	LoadAreaConfig();
	int iPort = pIniFileParser->getInteger("Setup", "DBPort", DEFAULT_DispatchGate_DB_PORT);
	if (!IsActive())
	{
		m_sLocalIP = "0.0.0.0";
		m_iListenPort = iPort;
		Log("接受DBServer连接, Port = " + std::to_string(m_iListenPort), lmtMessage);
		Open();
	}
}

int CDBServerSocket::SelectServer(CDGClient &client)
{

}

void CDBServerSocket::SendSelectServer(CDGClient &client)
{
	CDBConnector* pDBConnector = nullptr;
	TClientSelectServerInfo cInfo;
	std::lock_guard<std::mutex> guard(m_LockCS);
	std::list<void*>::iterator vIter;
	for (vIter = m_ActiveConnects.begin(); vIter != m_ActiveConnects.end(); ++vIter)
	{
		pDBConnector = (CDBConnector*)*vIter;
		if ((pDBConnector != nullptr) && (pDBConnector->GetServerID() == client.GetSelectRealServerID()))
		{
			cInfo.iSessionID = m_iSessionID;
			cInfo.iEnCodeIdx = client.GetEncodeIdx();
			cInfo.iClientType = client.GetClientType();
			cInfo.bMasterIP = client.GetIsGMIP();
			cInfo.iSelectServerID = client.GetSelectMaskServerID();
			cInfo.ucNetType = client.GetNetType();
			pDBConnector->SendToClientPeer(SM_SELECT_SERVER, client.GetSocketHandle(), &cInfo, sizeof(TClientSelectServerInfo));

			++m_iSessionID;
			if (m_iSessionID < 0)
				m_iSessionID = 1000;
		}
	}
}

void CDBServerSocket::SendServerInfoToPig(CPigClientSocket* pPigClient)
{
	if (nullptr == pPigClient)
		return;

	CDBConnector* pDBConnector = nullptr;
	TPigQueryServerInfo info;
	std::lock_guard<std::mutex> guard(m_LockCS);
	std::list<void*>::iterator vIter;
	for (vIter = m_ActiveConnects.begin(); vIter != m_ActiveConnects.end(); ++vIter)
	{
		pDBConnector = (CDBConnector*)*vIter;
		if ((pDBConnector != nullptr) && (pDBConnector->GetServerID() != 0))
		{
			memset(&info, 0, sizeof(TPigQueryServerInfo));
			info.iServerID = pDBConnector->GetServerID();
			memcpy_s(info.szServerIP, 15, pDBConnector->GetRemoteAddress().c_str(), pDBConnector->GetRemoteAddress().length());
			memcpy_s(info.szServerIP, 50, pDBConnector->GetServerName().c_str(), pDBConnector->GetServerName().length());
			pPigClient->SendToServerPeer(SM_PIG_QUERY_AREA, 0, &info, sizeof(TPigQueryServerInfo));
		}
	}
}

void CDBServerSocket::SendPigMsg(const char* pBuf, unsigned short usBufLen)
{

}

int CDBServerSocket::GetPlayerTotalCount()
{

}

void CDBServerSocket::DoActive()
{

}

bool CDBServerSocket::OnCheckIPAddress(const std::string& sIP)
{

}

CClientConnector* CDBServerSocket::OnCreateClientSocket(const std::string& sIP)
{

}

void CDBServerSocket::OnSocketError(void* Sender, int& iErrorCode)
{

}

void CDBServerSocket::OnClientConnect(void* Sender)
{

}

void CDBServerSocket::OnClientDisconnect(void* Sender)
{

}

void CDBServerSocket::OnSetListView(void* Sender)
{

}

void CDBServerSocket::LoadAreaConfig()
{

}

bool CDBServerSocket::RegisterDBServer(const std::string &sAddress, int iServerID, CDBConnector &dbServer)
{

}

void CDBServerSocket::ShowDBMsg(int iServerID, int iCol, const std::string &msg)
{

}

std::string CDBServerSocket::OnLineDBServer(int iServerID)
{

}

void CDBServerSocket::RemoveServerInfo(void* pValue, const std::string &sKey)
{

}

//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//procedure EnumAreaConfig(ElName: string; Elem: TlkJSONbase;data: pointer; var Continue: Boolean);

/************************End Of CDBServerSocket******************************************/