/**************************************************************************************
@author: 陈昌
@content: 
**************************************************************************************/
#include "CIocpSampleClient.h"
#pragma comment(lib, "ws2_32.lib")

/************************Start Of CIocpSampleClient******************************************/

CSampleClientManager::CSampleClientManager()
{
	SendDebugString("CSampleClientManager 创建");
	m_OnConnect = std::bind(&CSampleClientManager::OnSocketConnect, this, std::placeholders::_1);
	m_OnDisConnect = std::bind(&CSampleClientManager::OnSocketDisconnect, this, std::placeholders::_1);
	m_OnRead = std::bind(&CSampleClientManager::OnSocketRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	m_OnError = std::bind(&CSampleClientManager::OnSocketError, this, std::placeholders::_1, std::placeholders::_2);
}

CSampleClientManager::~CSampleClientManager()
{
	SendDebugString("CSampleClientManager 销毁");
}

void CSampleClientManager::ConnectToServer(const std::string& sIP, const int iPort)
{
	m_Address = sIP;
	m_Port = iPort;
	SetReconnectInterval(10000);
	SendDebugString("开始连接。。。");
}

void CSampleClientManager::Disconnect()
{
	SendDebugString("主动断开");
	Close(false);
}

bool CSampleClientManager::SendToServer()
{
	return true;
}

void CSampleClientManager::OnSocketConnect(void* Sender)
{
	SendDebugString("连接成功！");
}

void CSampleClientManager::OnSocketDisconnect(void* Sender)
{
	SendDebugString("连接已断开！");
}

void CSampleClientManager::OnSocketRead(void* Sender, const char* pBuf, int iCount)
{}

void CSampleClientManager::OnSocketError(void* Sender, int& iErrorCode)
{}

/************************End Of CIocpSampleClient********************************************/