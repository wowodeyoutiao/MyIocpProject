/**************************************************************************************
@author: 陈昌
@content: Dispatch对客户端连接的监听socket管理
**************************************************************************************/
#ifndef __CC_CLIENT_SERVER_SOCKET_H__
#define __CC_CLIENT_SERVER_SOCKET_H__

#include "stdafx.h"

/**
*
* DispatchGate监听的单个客户端连接对象
*
*/
class CDGClient : public CClientConnector
{
public:
	CDGClient();
	~CDGClient();
	void ForceClose();
	void OpenWindow(TClientWindowType wtype, int iParam, const std::string& msg = "");
	unsigned short GetSelectMaskServerID();
	int GetSelectRealServerID();
	bool GetIsGMIP();
	int GetEncodeIdx();
	int GetClientType();
	unsigned char GetNetType();
protected:
	virtual void Execute(unsigned long ulTick);
	virtual void SocketRead(const char* pBuf, int iCount);
private:
	void CMSelectServer(char* pBuf, unsigned short usBufLen);
	void CMCloseWindow(char* pBuf, unsigned short usBufLen);
	void SendToClient(unsigned short usIdent, char* pData, unsigned short usDataLen);
	void ProcessReceiveMsg(char* pHeader, char* pData, int iDataLen);
private:
	unsigned long m_ulLastConnectTick;
	unsigned long m_ulForceCloseTick;
	unsigned short m_usSelectMaskServerID;       //玩家选择的服务器用于外显的编号
	int m_iEncodeIdx;                            //加密编号
	int m_iSelectRealServerID;                   //玩家选择的服务器真实编号
	bool m_bIsGMIP;								 //判断是否为gmIP段
	int m_iClientType;							 //不同的客户端类型，使用不同的包头标记符
	unsigned char m_ucNetType;                   //客户端的网络类型---通过客户端对固定网址的解析，来判断不同的网络类型
};

/**
*
* DispatchGate对客户端的监听管理器
*
*/
class CClientServerSocket : public CIOCPServerSocketManager
{
public:
	CClientServerSocket();
	virtual ~CClientServerSocket();
protected:
	virtual void DoActive();
private:
	bool OnCheckIPAddress(const std::string& sIP);
	CDGClient* OnCreateClientSocket(const std::string& sIP);
	void OnSocketError(void* Sender, int& iErrorCode);
	void OnClientConnect(void* Sender);
	void OnClientDisconnect(void* Sender);
};

#endif //__CC_CLIENT_SERVER_SOCKET_H__