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
	virtual ~CDGClient();
	void ForceClose();
	void OpenWindow(TClientWindowType wtype, int iParam, const std::string& msg = "");
	unsigned short GetSelectMaskServerID();
	int GetSelectRealServerID();
	bool GetIsGMIP();
	void SetGMIP(const bool bFlag);
	int GetEncodeIdx();
	int GetClientType();
	unsigned char GetNetType();
	void SendToClientPeer(unsigned short usIdent, void* pData, unsigned short usDataLen);
protected:
	virtual void Execute(unsigned long ulTick);
	virtual void SocketRead(const char* pBuf, int iCount);
private:
	void CMSelectServer(char* pBuf, unsigned short usBufLen);
	void CMCloseWindow(char* pBuf, unsigned short usBufLen);
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

const int MAX_NET_TYPE_CONFIG = 9;

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
	void LoadConfig(CWgtIniFile* pIniFileParser);
	bool IsMasterIP(std::string &sIP);
	void SMSelectServer(int iSocketHandle, char* pBuf, unsigned short usBufLen);  // 返回选服信息
	TIpType GetDefaultRule();
	unsigned char GetNetType(int nAddr);
protected:
	virtual void DoActive();
private:
	void CheckIpConfig(unsigned long ulTick);
	void LoadIpConfigFile(const std::string& sFileName);
	void Clear();
	void AddIpRuleNode(const std::string& sIP, TIpType ipType);
	bool CheckConnectIP(const std::string& sIP);

	CClientConnector* OnCreateClientSocket(const std::string& sIP);
	void OnSocketError(void* Sender, int& iErrorCode);
	void OnClientConnect(void* Sender);
private:
	unsigned long m_ulLastCheckTick;
	int m_iIPConfigFileAge;								 //记录ipconfig文件的版本号
	TIpType m_DefaultRule;								 //默认的ip规则
	std::string m_sWarWarning;							 //连接战斗提示
	std::mutex m_IPRuleLockCS;							 //iprule链表的临界区操作使用的互斥锁
	std::list<PIpRuleNode> m_IPRuleList;				 //iprule链表
	unsigned long m_NetTypes[MAX_NET_TYPE_CONFIG];      //配置的根据客户端对于固定域名的不同解析，判断的网络类型
};

extern CClientServerSocket* pG_GateSocket;

#endif //__CC_CLIENT_SERVER_SOCKET_H__