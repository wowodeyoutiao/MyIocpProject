/**************************************************************************************
@author: 陈昌
@content: Dispatch对DB服务器连接的监听socket管理
**************************************************************************************/
#ifndef __CC_DB_SERVER_SOCKET_H__
#define __CC_DB_SERVER_SOCKET_H__

#include "CCTcpServerSocket.h"
#include "CClientServerSocket.h"
#include "CPigClientSocket.h"

/**
*
* DispatchGate监听的单个DBServer的连接对象
*
*/
class CDBConnector : public CClientConnector
{
public:
	CDBConnector();
	virtual ~CDBConnector();
	int GetServerID();
	int GetPlayerCount();
	void SendToClientPeer(unsigned short usIdent, int iParam, char* pBuf, unsigned short usBufLen);
	void AddIpRuleNode(const std::string& sIP, TIpType ipType);
	bool CheckClientIP(const std::string& sIP);
protected:
	virtual void SocketRead(const char* pBuf, int iCount);
	virtual void ProcessReceiveMsg(PServerSocketHeader pHeader, const char* pData, int iDataLen);
private:
	void SendHeartBeat(int iCount);                // 心跳返回
	void RegisterDBServer(int iServerID);          // 注册DBServer
	void ClearIPRule(TIpType ipType);
	void ReceiveConfig(int iParam, const char* pBuf, unsigned short usBufLen);
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
	std::hash<std::string> myhash;
/*
public:
	CDBServerSocket(const std::string& sName);
	virtual ~CDBServerSocket();
	void LoadConfig(CWgtIniFile* pIniFileParser);
	int SelectServer(CDGClient &client);
	void SendSelectServer(CDGClient &client);
	void SendAreaInfoToPig(CPigClientSocket &pigClient);
	void SendPigMsg(const char* pBuf, unsigned short usBufLen);
	int GetPlayerTotalCount();
protected:
	virtual void DoActive();
private:
	bool OnCheckIPAddress(const std::string& sIP);
	CClientConnector* OnCreateClientSocket(const std::string& sIP);
	void OnSocketError(void* Sender, int& iErrorCode);
	void OnClientConnect(void* Sender);
	void OnClientDisconnect(void* Sender);
	void OnSetListView(void* Sender);

	void LoadAreaConfig();
	bool RegisterDBServer(const std::string &sAddress, int iServerID, CDBConnector &dbServer);
	void ShowDBMsg(int iServerID, int iCol, const std::string &msg);
	std::string OnLineDBServer(int iServerID);

	//-----------------------------
	//-----------------------------
	//-----------------------------
	//procedure RemoveServerInfo(Pvalue: Pointer; const Key: ansistring);
	//procedure EnumAreaConfig(ElName: string; Elem: TlkJSONbase;data: pointer; var Continue: Boolean);
private:
	std::string m_sAllowDBServerIP;				// 允许的IP
	int m_iSessionID;           
	std::string m_sServerName;
	int m_iConfigFileAge;
	unsigned long m_ulLastCheckTick;
	//FLogSocket: TLogSocket;
	//FServerList: TNamesHash;                  //  区组列表 
*/
};

#endif //__CC_DB_SERVER_SOCKET_H__