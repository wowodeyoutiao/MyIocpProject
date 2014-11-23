/**************************************************************************************
@author: 陈昌
@content: Dispatch对DB服务器连接的监听socket管理
**************************************************************************************/
#ifndef __CC_DB_SERVER_SOCKET_H__
#define __CC_DB_SERVER_SOCKET_H__

#include "CCTcpServerSocket.h"

/**
*
* DispatchGate监听的单个DBServer的连接对象
*
*/
class CDBConnector : public CClientConnector
{
public:
	CDBConnector();
	~CDBConnector();
	int GetServerID();
	int GetPlayerCount();
	void SendToClientPeer(unsigned short usIdent, int iParam, char* pBuf, unsigned short usBufLen);
	void AddIpRuleNode(const std::string& sIP, TIpType ipType);
	bool CheckClientIP(const std::string& sIP);
protected:
	virtual void SocketRead(const char* pBuf, int iCount);
private:
	void SendHeartBeat(int iCount);                // 心跳返回
	void RegisterDBServer(int iServerID);          // 注册DBServer
	void ClearIPRule(TIpType ipType);
	void ReceiveConfig(int iParam, char* pBuf, unsigned short usBufLen);
private:
	int m_iServerID;                //服务器实际区号
	int m_iPlayerCount;             //DB上的玩家数量
	TIpType m_DefaultRule;          
	std::string m_sDenyHint;			   //连接服务器后的默认提示
	std::string m_sServerName;             //服务器区名
	std::mutex m_IPRuleLockCS;			   //iprule链表的临界区操作使用的互斥锁
	std::list<PIpRuleNode> m_IPRuleList;   //iprule链表
};


/**
*
* DispatchGate对DBServer的监听管理器
*
*/
class CDBServerSocket : public CIOCPServerSocketManager
{
public:
	CDBServerSocket();
	virtual ~CDBServerSocket();
protected:
	virtual void DoActive();
private:
	bool OnCheckIPAddress(const std::string& sIP);
	CDBConnector* OnCreateClientSocket(const std::string& sIP);
	void OnSocketError(void* Sender, int& iErrorCode);
	void OnClientConnect(void* Sender);
	void OnClientDisconnect(void* Sender);
};

#endif //__CC_DB_SERVER_SOCKET_H__