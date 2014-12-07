/**************************************************************************************
@author: 陈昌
@content: Dispatch作为客户端方连接PIG服务器的端口
**************************************************************************************/
#ifndef __CC_PIG_CLIENT_SOCKET_H__
#define __CC_PIG_CLIENT_SOCKET_H__

#include "stdafx.h"

/**
*
* DispatchGate对PIG服务器的连接端口
*
*/
class CPigClientSocket : public CIOCPClientSocketManager
{
public:
	CPigClientSocket();
	virtual ~CPigClientSocket();
	void LoadConfig(CWgtIniFile* pIniFileParser);
	void SendToServerPeer(unsigned short usIdent, int iParam, void* pBuf, unsigned short usBufLen);
	void DoHeartBeat();
protected: 
	virtual void ProcessReceiveMsg(PServerSocketHeader pHeader, const char* pData, int iDataLen);
private:
	void OnSocketConnect(void* Sender);
	void OnSocketDisconnect(void* Sender);
	void OnSocketRead(void* Sender, const char* pBuf, int iCount);
	void OnSocketError(void* Sender, int& iErrorCode);
private:
	unsigned long m_ulCheckTick;
	int m_iPingCount;
};

extern CPigClientSocket* pG_PigSocket;

#endif //__CC_PIG_CLIENT_SOCKET_H__