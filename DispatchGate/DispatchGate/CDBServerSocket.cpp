/**************************************************************************************
@author: 陈昌
@content: Dispatch对DB服务器连接的监听socket管理
**************************************************************************************/
#include "stdafx.h"
#include "CDBServerSocket.h"

/************************Start Of CDBConnector******************************************/
CDBConnector::CDBConnector()
{
}

CDBConnector::~CDBConnector()
{
}

int CDBConnector::GetServerID()
{
	return m_iServerID;
}

int CDBConnector::GetPlayerCount()
{
	return m_iPlayerCount;
}

void CDBConnector::SendToClientPeer(unsigned short usIdent, int iParam, char* pBuf, unsigned short usBufLen)
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
{}

bool CDBConnector::CheckClientIP(const std::string& sIP)
{}

void CDBConnector::SocketRead(const char* pBuf, int iCount)
{}

void CDBConnector::SendHeartBeat(int iCount)
{}

void CDBConnector::RegisterDBServer(int iServerID)
{}

void CDBConnector::ClearIPRule(TIpType ipType)
{}

void CDBConnector::ReceiveConfig(int iParam, char* pBuf, unsigned short usBufLen)
{}

/************************Start Of CDBConnector******************************************/