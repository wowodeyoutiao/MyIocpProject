/**************************************************************************************
@author: 陈昌
@content: Dispatch作为客户端方连接PIG服务器的端口
**************************************************************************************/
#include "CPigClientSocket.h"

/************************Start Of CPigClientSocket******************************************/

CPigClientSocket::CPigClientSocket()
{
	SendDebugString("CSampleClientManager 创建");
	m_OnConnect = std::bind(&CPigClientSocket::OnSocketConnect, this, std::placeholders::_1);
	m_OnDisConnect = std::bind(&CPigClientSocket::OnSocketDisconnect, this, std::placeholders::_1);
	m_OnRead = std::bind(&CPigClientSocket::OnSocketRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	m_OnError = std::bind(&CPigClientSocket::OnSocketError, this, std::placeholders::_1, std::placeholders::_2);
}

CPigClientSocket::~CPigClientSocket()
{
	SendDebugString("CSampleClientManager 销毁");
}

void CPigClientSocket::LoadConfig(CWgtIniFile* pIniFileParser)
{

}

void CPigClientSocket::SendBuffer(unsigned short usIdent, int iParam, char* pBuf, unsigned short usBufLen)
{

}

void CPigClientSocket::DoHeartBeat()
{
	
}

void CPigClientSocket::OnSocketConnect(void* Sender)
{
	SendDebugString("连接成功！");
}

void CPigClientSocket::OnSocketDisconnect(void* Sender)
{
	SendDebugString("连接已断开！");
}

void CPigClientSocket::OnSocketRead(void* Sender, const char* pBuf, int iCount)
{}

void CPigClientSocket::OnSocketError(void* Sender, int& iErrorCode)
{}

/************************End Of CPigClientSocket********************************************/