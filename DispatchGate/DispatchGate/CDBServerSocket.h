/**************************************************************************************
@author: 陈昌
@content: Dispatch对DB服务器连接的监听socket管理
**************************************************************************************/
#ifndef __CC_DB_SERVER_SOCKET_H__
#define __CC_DB_SERVER_SOCKET_H__

#include "CCTcpServerSocket.h"
#include "CClientServerSocket.h"
#include "CPigClientSocket.h"

//dispatch上使用的服务器区组配置
typedef struct _TServerConfigInfo
{
	int iMaskServerID;
	std::string sServerName;
	int iRealServerID;
	std::string sServerIP;
}TServerConfigInfo, *PServerConfigInfo;

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
	std::string& GetServerName();
	std::string& GetDenyHint();
	TIpType GetDefaultRule();
	void SetServerName(const std::string& sName);
	void SendToClientPeer(unsigned short usIdent, int iParam, void* pBuf, unsigned short usBufLen);
	void AddIpRuleNode(const std::string& sIP, TIpType ipType);
	bool CheckClientIP(const std::string& sIP);
protected:
	virtual void SocketRead(const char* pBuf, int iCount);
	virtual void ProcessReceiveMsg(PServerSocketHeader pHeader, char* pData, int iDataLen);
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
public:
	CDBServerSocket(const std::string& sName);
	virtual ~CDBServerSocket();
	void LoadConfig(CWgtIniFile* pIniFileParser);
	int SelectServer(CDGClient* pClient);
	void SendSelectServer(CDGClient* pClient);
	void SendServerInfoToPig(CPigClientSocket* pPigClient);
	void SendPigMsg(char* pBuf, unsigned short usBufLen);
	int GetPlayerTotalCount();
	void ShowDBMsg(int iServerID, int iCol, const std::string &msg);
	bool RegisterDBServer(const std::string &sAddress, int iServerID, CDBConnector* pDBServer);
protected:
	virtual void DoActive();
private:
	bool OnCheckIPAddress(const std::string& sIP);
	CClientConnector* OnCreateDBSocket(const std::string& sIP);
	void OnSocketError(void* Sender, int& iErrorCode);
	void OnDBConnect(void* Sender);
	void OnDBDisconnect(void* Sender);
	void OnSetListView(void* Sender);

	void LoadServerConfig();
	std::string OnLineDBServer(int iServerID);
	void RemoveServerInfo(void* pValue, const std::string &sKey);
	//-----------------------------
	//-----------------------------
	//-----------------------------
	//procedure EnumAreaConfig(ElName: string; Elem: TlkJSONbase;data: pointer; var Continue: Boolean);
private:
	std::string m_sAllowDBServerIP;				// 允许的IP
	int m_iSessionID;           
	std::string m_sServerName;
	int m_iConfigFileAge;
	unsigned long m_ulLastCheckTick;
	CC_UTILS::CLogSocket* m_pLogSocket;			// 连接日志服务的端口
	CC_UTILS::CStringHash m_ServerHash;         // 区组列表 
};

extern CDBServerSocket* pG_DBSocket;

#endif //__CC_DB_SERVER_SOCKET_H__