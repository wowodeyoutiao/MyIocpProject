/**************************************************************************************
@author: 陈昌
@content: Dispatch作为客户端方连接PIG服务器的端口
**************************************************************************************/
#include "stdafx.h"
#include "CPigClientSocket.h"

/************************Start Of CPigClientSocket******************************************/

CPigClientSocket::CPigClientSocket() : m_iPingCount(0)
{
	SendDebugString("CPigClientSocket 创建");
	SetReconnectInterval(10 * 1000);
	m_pReceiveBuffer = new CC_UTILS::TBufferStream;
	m_pReceiveBuffer->Initialize();

	m_OnConnect = std::bind(&CPigClientSocket::OnSocketConnect, this, std::placeholders::_1);
	m_OnDisConnect = std::bind(&CPigClientSocket::OnSocketDisconnect, this, std::placeholders::_1);
	m_OnRead = std::bind(&CPigClientSocket::OnSocketRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	m_OnError = std::bind(&CPigClientSocket::OnSocketError, this, std::placeholders::_1, std::placeholders::_2);	
}

CPigClientSocket::~CPigClientSocket()
{
	m_pReceiveBuffer->Finalize();
	delete(m_pReceiveBuffer);
	SendDebugString("CPigClientSocket 销毁");
}

void CPigClientSocket::LoadConfig(CWgtIniFile* pIniFileParser)
{
	if (pIniFileParser != nullptr)
	{
		m_Address = pIniFileParser->getString("PigServer", "IP", "");
		m_Port = pIniFileParser->getInteger("PigServer", "Port", DEFAULT_PIG_SERVER_PORT);
		if ("" == m_Address)
			Log("未配置PigServer的IP！");
		else
			Open();
	}
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