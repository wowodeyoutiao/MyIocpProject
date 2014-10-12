/**************************************************************************************
@author: 陈昌
@content: 使用IOCP客户端框架快速构建基于iocp的客户端
		  通过对 CIOCPClientSocketManager的继承快速构建客户端的连接管理
**************************************************************************************/
#ifndef __CC_IOCP_SAMPLE_CLIENT_H__
#define __CC_IOCP_SAMPLE_CLIENT_H__

#include "CCTcpClientSocket.h"

/**
*  
* 基于IOCP的客户端的一个管理器子类
*
*/
class CSampleClientManager : public CIOCPClientSocketManager
{
public:
	CSampleClientManager();
	virtual ~CSampleClientManager();
	void ConnectToServer(const std::string& sIP, const int iPort);
	void Disconnect();
	bool SendToServer();
private:
	void OnSocketConnect(void* Sender);
	void OnSocketDisconnect(void* Sender);
	void OnSocketRead(void* Sender, const char* pBuf, int iCount);
	void OnSocketError(void* Sender, int& iErrorCode);
};

#endif //__CC_IOCP_SAMPLE_CLIENT_H__