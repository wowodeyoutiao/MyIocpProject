/**************************************************************************************
@author: 陈昌
@content: Dispatch作为客户端方连接CenterServer服务器的端口
**************************************************************************************/
#ifndef __CC_CENTER_CLIENT_SOCKET_H__
#define __CC_CENTER_CLIENT_SOCKET_H__

#include "stdafx.h"

/**
*
* DispatchGate对Center服务器的连接端口
*
*/

class CCenterClientSocket : public CIOCPClientSocketManager
{
public:
	CCenterClientSocket();
	virtual ~CCenterClientSocket();
	void LoadConfig(CWgtIniFile* pIniFileParser);
	void SendToServerPeer(unsigned short usIdent, int iParam, void* pBuf, unsigned short usBufLen);
	void DoHeartBeat();					    // 发送心跳
protected:
	virtual void ProcessReceiveMsg(PServerSocketHeader pHeader, const char* pData, int iDataLen);
private:
	void OnSocketConnect(void* Sender);
	void OnSocketDisconnect(void* Sender);
	void OnSocketRead(void* Sender, const char* pBuf, int iCount);
	void OnSocketError(void* Sender, int& iErrorCode);
	void Reconnect();                       // 重连
	void SendRegisterServer();              // 注册服务器
private:
	unsigned long m_ulCheckTick;
	int m_iPingCount;
	int m_iWorkIndex;
	TServerAddress m_ServerArray[MAX_CENTER_SERVER_COUNT];        // 可配置多个CenterServer           
};

extern CCenterClientSocket* pG_CenterSocket;

#endif //__CC_CENTER_CLIENT_SOCKET_H__