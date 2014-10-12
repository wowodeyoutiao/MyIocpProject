/**************************************************************************************
@author: 陈昌
@content: 使用IOCP服务器框架快速构建服务器
		通过对 CIOCPServerSocketManager 和 CClientConnector的继承快速构建服务器
**************************************************************************************/
#ifndef __CC_IOCP_SAMPLE_SERVER_H__
#define __CC_IOCP_SAMPLE_SERVER_H__

#include "CCTcpServerSocket.h"

/**
*  
* CClientConnector的子类sample
*
*/
class CSampleConnector : public CClientConnector
{
public:
	CSampleConnector();
	~CSampleConnector();
protected:
	virtual void Execute(unsigned long ulTick);
	virtual void SocketRead(const char* pBuf, int iCount);
private:
	unsigned long m_ulLastRunTick;
};

/**
*  
* CIOCPServerSocketManager的子类sample
*
*/
class CSampleServerManager : public CIOCPServerSocketManager
{
public:
	CSampleServerManager();
	virtual ~CSampleServerManager();
protected:
	virtual void DoActive();
private:
	bool OnCheckIPAddress(const std::string& sIP);
	CSampleConnector* OnCreateClientSocket(const std::string& sIP);
	void OnSocketError(void* Sender, int& iErrorCode);
	void OnClientConnect(void* Sender);
	void OnClientDisconnect(void* Sender);
};

#endif //__CC_IOCP_SAMPLE_SERVER_H__