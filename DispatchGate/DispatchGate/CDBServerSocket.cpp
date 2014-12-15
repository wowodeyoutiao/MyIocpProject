/**************************************************************************************
@author: 陈昌
@content: Dispatch对DB服务器连接的监听socket管理
**************************************************************************************/
#include "stdafx.h"
#include "CDBServerSocket.h"
#include "CClientServerSocket.h"

using namespace CC_UTILS;

CDBServerSocket* pG_DBSocket;

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

std::string& CDBConnector::GetDenyHint()
{
	return m_sDenyHint;
}

TIpType CDBConnector::GetDefaultRule()
{
	return m_DefaultRule;
}

void CDBConnector::SetServerName(const std::string& sName)
{
	m_sServerName = sName;
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
		if (sTempIP.find(pNode->sMatchIP) == 0)
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

void CDBConnector::ProcessReceiveMsg(PServerSocketHeader pHeader, char* pData, int iDataLen)
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
		pG_GateSocket->SMSelectServer(pHeader->iParam, pData, iDataLen);
		break;
	case SM_SERVER_CONFIG:
		ReceiveConfig(pHeader->iParam, pData, iDataLen);
		break;
	default:
		Log("收到未知DBServer协议，Ident=" + to_string(pHeader->usIdent), lmtWarning);
		break;
	}
}

void CDBConnector::SendHeartBeat(int iCount)
{
	pG_DBSocket->ShowDBMsg(m_iServerID, 4, to_string(iCount));
	m_iPlayerCount = iCount;
	SendToClientPeer(SM_PING, 0, nullptr, 0);
}

void CDBConnector::RegisterDBServer(int iServerID)
{
	if (pG_DBSocket->RegisterDBServer(GetRemoteAddress(), iServerID, this))
	{
		m_DefaultRule = itAllow;
		m_iServerID = iServerID;
		pG_DBSocket->ShowDBMsg(iServerID, 3, GetRemoteAddress());
		pG_DBSocket->ShowDBMsg(iServerID, 4, "0");
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

	m_OnCreateClient = std::bind(&CDBServerSocket::OnCreateDBSocket, this, std::placeholders::_1);
	m_OnClientError = std::bind(&CDBServerSocket::OnSocketError, this, std::placeholders::_1, std::placeholders::_2);
	m_OnConnect = std::bind(&CDBServerSocket::OnDBConnect, this, std::placeholders::_1);
	m_OnDisConnect = std::bind(&CDBServerSocket::OnDBDisconnect, this, std::placeholders::_1);
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
	LoadServerConfig();
	int iPort = pIniFileParser->getInteger("Setup", "DBPort", DEFAULT_DispatchGate_DB_PORT);
	if (!IsActive())
	{
		m_sLocalIP = "0.0.0.0";
		m_iListenPort = iPort;
		Log("接受DBServer连接, Port = " + std::to_string(m_iListenPort), lmtMessage);
		Open();
	}
}

int CDBServerSocket::SelectServer(CDGClient* pClient)
{
	int iRetCode = 0;
	if (nullptr == pClient)
		return iRetCode;

	int iErrCode = -1;
	int iServerID = 0;
	int iMaskID = pClient->GetSelectMaskServerID();
	{		
		PServerConfigInfo pInfo = nullptr;
		std::lock_guard<std::mutex> guard(m_LockCS);
		m_ServerHash.First();
		while (!m_ServerHash.Eof())
		{
			pInfo = (PServerConfigInfo)m_ServerHash.GetNextNode();
			if (iMaskID == pInfo->iMaskServerID)
			{
				iServerID = pInfo->iRealServerID;
				break;
			}
		}
	}

	std::string sHintMsg;
	if (iServerID > 0)
	{
		iErrCode = -2;
		CDBConnector* pDBConnector = nullptr;
		std::lock_guard<std::mutex> guard(m_LockCS);
		std::list<void*>::iterator vIter;
		for (vIter = m_ActiveConnects.begin(); vIter != m_ActiveConnects.end(); ++vIter)
		{
			pDBConnector = (CDBConnector*)*vIter;
			if (iServerID == pDBConnector->GetServerID())
			{
				if ((pClient->GetIsGMIP()) || (pDBConnector->CheckClientIP(pClient->GetRemoteAddress())))
				{
					iErrCode = 0;
					iRetCode = iServerID;
				}
				else
				{
					iErrCode = -3;
					if (itDeny == pDBConnector->GetDefaultRule())
						sHintMsg = pDBConnector->GetDenyHint();
					else
						sHintMsg = "您被禁止进入!";
				}
				break;
			}
		}
	}

	if ((iErrCode != 0) && (pClient != nullptr))
	{
		switch (iErrCode)
		{
		case -1:
			sHintMsg = "您所选的区组错误!";
			break;
		case -2:
			sHintMsg = "目前服务器处于维护中!";
			break;
		default:
			break;
		}

		pClient->OpenWindow(cwMessageBox, iErrCode, sHintMsg);
		pClient->ForceClose();
	}

	return iRetCode;
}

void CDBServerSocket::SendSelectServer(CDGClient* pClient)
{
	CDBConnector* pDBConnector = nullptr;
	TClientSelectServerInfo cInfo;
	std::lock_guard<std::mutex> guard(m_LockCS);
	std::list<void*>::iterator vIter;
	for (vIter = m_ActiveConnects.begin(); vIter != m_ActiveConnects.end(); ++vIter)
	{
		pDBConnector = (CDBConnector*)*vIter;
		if ((pDBConnector != nullptr) && (pClient != nullptr) && (pDBConnector->GetServerID() == pClient->GetSelectRealServerID()))
		{
			cInfo.iSessionID = m_iSessionID;
			cInfo.iEnCodeIdx = pClient->GetEncodeIdx();
			cInfo.iClientType = pClient->GetClientType();
			cInfo.bMasterIP = pClient->GetIsGMIP();
			cInfo.iSelectServerID = pClient->GetSelectMaskServerID();
			cInfo.ucNetType = pClient->GetNetType();
			pDBConnector->SendToClientPeer(SM_SELECT_SERVER, pClient->GetSocketHandle(), &cInfo, sizeof(TClientSelectServerInfo));

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
	std::string sTemp;
	for (vIter = m_ActiveConnects.begin(); vIter != m_ActiveConnects.end(); ++vIter)
	{
		pDBConnector = (CDBConnector*)*vIter;
		if ((pDBConnector != nullptr) && (pDBConnector->GetServerID() != 0))
		{
			memset(&info, 0, sizeof(TPigQueryServerInfo));
			info.iServerID = pDBConnector->GetServerID();
			sTemp = pDBConnector->GetRemoteAddress();
			memcpy_s(info.szServerIP, sizeof(info.szServerIP), sTemp.c_str(), sTemp.length() + 1);
			sTemp = pDBConnector->GetServerName();
			memcpy_s(info.szServerName, sizeof(info.szServerName), sTemp.c_str(), sTemp.length() + 1);
			pPigClient->SendToServerPeer(SM_PIG_QUERY_AREA, 0, &info, sizeof(TPigQueryServerInfo));
		}
	}
}

void CDBServerSocket::SendPigMsg(char* pBuf, unsigned short usBufLen)
{
	if (usBufLen > sizeof(TPigMsgData))
	{
		char* pCurr = pBuf;
		std::string sMsg("");
		std::string sAreaList("");
		PPigMsgData pMsgData = (PPigMsgData)pBuf;
		if (usBufLen < pMsgData->usAreaLen + pMsgData->usMsgLen)
		{
			Log("PigMsg 数据长度错误！", lmtMessage);
			return;
		}

		pCurr = pCurr + sizeof(TPigMsgData);
		if (pMsgData->usAreaLen > 0)
		{			
			//----------------------------------
			//-----这里的赋值，字符串的结束符问题
			sAreaList.assign(pCurr, pMsgData->usAreaLen);
		}
		pCurr = pCurr + pMsgData->usAreaLen;
		if (pMsgData->usMsgLen > 0)
		{
			//----------------------------------
			//-----这里的赋值，字符串的结束符问题
			sMsg.assign(pCurr, pMsgData->usMsgLen);
		}
		if ((sAreaList.compare("") == 0) || (sMsg.compare("") == 0))
			return;

		/*
    iDataLen := SizeOf(TPkgMsgHead) + Length(sMsg) + 10;
    AreaList := TStringList.Create;
    AreaList.Text := StringReplace(sAreaList, '|', #13#10, [rfReplaceAll]);
    GetMem(pData, iDataLen);
    try
      iDataLen := MakeMsgPkg(pData, sMsg, TMesssageType(PPigMsgData(Buf)^.MsgType), 255, 255, True, 0);
      Lock;
      try
        for i := 0 to ActiveConnects.Count - 1 do
        begin
          DBServer := ActiveConnects[i];
          if (DBServer.ServerID <> 0) and (AreaList.IndexOf(IntToStr(DBServer.ServerID)) > -1) then
          begin
            DBServer.SendBuffer(SM_PIG_MSG, 0, pData, iDataLen);
          end;
        end;
      finally
        UnLock;
      end;
    finally
      FreeMem(pData);
      AreaList.Free;
    end;
		*/
	}
}

int CDBServerSocket::GetPlayerTotalCount()
{
	CDBConnector* pDBConnector = nullptr;
	int retCount = 0;
	std::lock_guard<std::mutex> guard(m_LockCS);
	std::list<void*>::iterator vIter;
	for (vIter = m_ActiveConnects.begin(); vIter != m_ActiveConnects.end(); ++vIter)
	{
		pDBConnector = (CDBConnector*)*vIter;
		if (pDBConnector != nullptr)
			retCount += pDBConnector->GetPlayerCount();
	}
	return retCount;
}

void CDBServerSocket::DoActive()
{
	CIOCPServerSocketManager::DoActive();
	LoadServerConfig();
}

bool CDBServerSocket::OnCheckIPAddress(const std::string& sIP)
{
	return (m_sAllowDBServerIP.find(sIP) != string::npos);
}

CClientConnector* CDBServerSocket::OnCreateDBSocket(const std::string& sIP)
{
	return new CDBConnector;
}

void CDBServerSocket::OnSocketError(void* Sender, int& iErrorCode)
{
	Log("Server Socket Error, Code = " + to_string(iErrorCode), lmtError);
	iErrorCode = 0;
}

void CDBServerSocket::OnDBConnect(void* Sender)
{
	CDBConnector* pDBConnector = (CDBConnector*)Sender;
	Log(pDBConnector->GetRemoteAddress() + " Connected.");
}

void CDBServerSocket::OnDBDisconnect(void* Sender)
{
	CDBConnector* pDBConnector = (CDBConnector*)Sender;
	Log(pDBConnector->GetRemoteAddress() + " DisConnected.");
	ShowDBMsg(pDBConnector->GetServerID(), 3, "--");
	ShowDBMsg(pDBConnector->GetServerID(), 4, "--");
}

void CDBServerSocket::OnSetListView(void* Sender)
{
	PServerConfigInfo pInfo = nullptr;
	std::string sTemp;
	TListViewInfo lvInfo;
	memset(&lvInfo, 0, sizeof(TListViewInfo));
	{
		std::lock_guard<std::mutex> guard(m_LockCS);
		m_ServerHash.First();
		while (!m_ServerHash.Eof())
		{
			pInfo = (PServerConfigInfo)m_ServerHash.GetNextNode();
			sTemp = std::to_string(pInfo->iMaskServerID);
			memcpy_s(lvInfo[0], sizeof(TShortValue), sTemp.c_str(), sTemp.length() + 1);
			sTemp = pInfo->sServerName;
			memcpy_s(lvInfo[1], sizeof(TShortValue), sTemp.c_str(), sTemp.length() + 1);
			sTemp = std::to_string(pInfo->iRealServerID);
			memcpy_s(lvInfo[2], sizeof(TShortValue), sTemp.c_str(), sTemp.length() + 1);
			sTemp = OnLineDBServer(pInfo->iRealServerID);
			memcpy_s(lvInfo[3], sizeof(TShortValue), sTemp.c_str(), sTemp.length() + 1);
			sTemp = "--";
			memcpy_s(lvInfo[4], sizeof(TShortValue), sTemp.c_str(), sTemp.length() + 1);
			if (m_pLogSocket != nullptr)
				m_pLogSocket->AddListView(&lvInfo);
		}
	}
	sTemp = "区号";
	memcpy_s(lvInfo[0], sizeof(TShortValue), sTemp.c_str(), sTemp.length() + 1);
	sTemp = "     区名     ";
	memcpy_s(lvInfo[1], sizeof(TShortValue), sTemp.c_str(), sTemp.length() + 1);
	sTemp = "服务器编号";
	memcpy_s(lvInfo[2], sizeof(TShortValue), sTemp.c_str(), sTemp.length() + 1);
	sTemp = "    连接地址    ";
	memcpy_s(lvInfo[3], sizeof(TShortValue), sTemp.c_str(), sTemp.length() + 1);
	sTemp = "  人数  ";
	memcpy_s(lvInfo[4], sizeof(TShortValue), sTemp.c_str(), sTemp.length() + 1);
	if (m_pLogSocket != nullptr)
		m_pLogSocket->SetListViewColumns(&lvInfo);
}

void CDBServerSocket::LoadServerConfig()
{
	unsigned long ulTick = GetTickCount();
	if ((0 == m_ulLastCheckTick) || (ulTick - m_ulLastCheckTick >= 30000))
	{
		m_ulLastCheckTick = ulTick;
		std::string sFileName(G_CurrentExeDir + "AreaConfig.json");
		int iAge = GetFileAge(sFileName);
		if ((iAge != -1) && (iAge != m_iConfigFileAge))
		{
			if (m_iConfigFileAge > 0)
				Log("AreaConfig.json is Reloaded.", lmtMessage);
			m_iConfigFileAge = iAge;

			ifstream configFile;
			configFile.open(sFileName);
			std::string sJosnStr;
			configFile >> sJosnStr;
			configFile.close();

			Json::Reader reader;
			Json::Value root;
			if (reader.parse(sJosnStr, root))
			{
				if (root.isArray())
				{
					delete m_pLogSocket;
					m_ServerHash.Clear();
					{
						std::string sAreaName;
						PServerConfigInfo pInfo;
						Json::Value item;
						std::lock_guard<std::mutex> guard(m_LockCS);
						for (int i = 0; i < root.size(); i++)
						{
							item = root.get(i, 0);
							sAreaName = item.get("AreaName", "").asString();
							if (m_ServerHash.ValueOf(sAreaName) == nullptr)
							{
								pInfo = new TServerConfigInfo;
								pInfo->sServerName = sAreaName;
								pInfo->iMaskServerID = item.get("AreaID", 0).asInt();
								pInfo->iRealServerID = item.get("ServerID", 0).asInt();
								pInfo->sServerIP = item.get("ServerIP", "").asString();
								m_ServerHash.Add(sAreaName, pInfo);
								m_sAllowDBServerIP.append(pInfo->sServerIP + "|");
							}
							else
							{
								Log("区名重复: " + sAreaName, lmtError);
							}
						}
					}					
					m_pLogSocket = new CC_UTILS::CLogSocket(m_sServerName, true);
					m_pLogSocket->InitialWorkThread();
					//-------------------------------------
					//-------------------------------------
					//-------------------------------------
					//FLogSocket.OnConnect := OnSetListView;
				}
			}	
		}
	}
}

bool CDBServerSocket::RegisterDBServer(const std::string &sAddress, int iServerID, CDBConnector* pDBServer)
{
	bool retFlag = false;
	PServerConfigInfo pInfo = nullptr;
	std::lock_guard<std::mutex> guard(m_LockCS);
	m_ServerHash.First();
	while (!m_ServerHash.Eof())
	{
		pInfo = (PServerConfigInfo)m_ServerHash.GetNextNode();
		if (iServerID == pInfo->iRealServerID)
		{
			if (pInfo->sServerIP.find(sAddress) == string::npos)
				Log("DBServer " + sAddress + " not in " + pInfo->sServerIP, lmtWarning);
			if (pDBServer != nullptr)
				pDBServer->SetServerName(pInfo->sServerName);
			retFlag = true;
			break;
		}
	}
	return retFlag;
}

void CDBServerSocket::ShowDBMsg(int iServerID, int iCol, const std::string &msg)
{
	int iRow = 1;
	PServerConfigInfo pInfo = nullptr;
	std::lock_guard<std::mutex> guard(m_LockCS);
	m_ServerHash.First();
	while (!m_ServerHash.Eof())
	{
		pInfo = (PServerConfigInfo)m_ServerHash.GetNextNode();
		if (pInfo->iRealServerID == iServerID)
		{
			if (m_pLogSocket != nullptr)
				m_pLogSocket->UpdateListView(msg, iRow, iCol);
			break;
		}
		++iRow;
	}
}

std::string CDBServerSocket::OnLineDBServer(int iServerID)
{
	std::string sRetStr("--");
	CDBConnector* pDBConnector = nullptr;
	std::lock_guard<std::mutex> guard(m_LockCS);
	std::list<void*>::iterator vIter;
	for (vIter = m_ActiveConnects.begin(); vIter != m_ActiveConnects.end(); ++vIter)
	{
		pDBConnector = (CDBConnector*)*vIter;
		if ((pDBConnector != nullptr) && (iServerID == pDBConnector->GetServerID()))
		{
			sRetStr = pDBConnector->GetRemoteAddress();
			break;
		}	
	}
	return sRetStr;
}

void CDBServerSocket::RemoveServerInfo(void* pValue, const std::string &sKey)
{
	delete (PServerConfigInfo)pValue;
}

//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//procedure EnumAreaConfig(ElName: string; Elem: TlkJSONbase;data: pointer; var Continue: Boolean);

/************************End Of CDBServerSocket******************************************/