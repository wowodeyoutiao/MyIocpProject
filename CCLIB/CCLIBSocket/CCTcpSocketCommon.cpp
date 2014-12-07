/**************************************************************************************
@author: 陈昌
@content: 
**************************************************************************************/

#include "CCTcpSocketCommon.h"
#include <iostream>
#pragma comment(lib, "ws2_32.lib")


//函数的实现必须定义到cpp文件中，不然多个文件引用后，编译时声称的obj对象里面都会有这个函数标识符，链接时候会导致重复
bool DoInitialWinSocket()
{
	WSADATA wsa;
	return (WSAStartup(0x2020, &wsa) == NO_ERROR);
}

void DoFinalizeWinSocket()
{
	WSACleanup();
}

void SendDebugString(const std::string& sInfo)
{
	std::cout << sInfo.c_str() << std::endl;
}

bool IsInternetIP(const u_long ulIP)
{
	bool retFlag = true;
	unsigned char ucFirst;
	unsigned char ucSecond;
	ucFirst = ulIP & 0xFF;
	ucSecond = (ulIP & 0xFF00) >> 8;
	if (192 == ucFirst)
		retFlag = false;
	else if (10 == ucFirst)
		retFlag = false;
	else if ((172 == ucFirst) && (ucSecond > 15) && (ucSecond < 33))
		retFlag = false;

	return retFlag;
}

std::string GetInternetIP(const std::string& sDefaultIP)
{
	std::string sRetIP("");
	char szHostName[256];
	if (0 == gethostname(szHostName, sizeof(szHostName)))
	{	
		if (sDefaultIP.find("127.") == 0)
		{
			sRetIP = sDefaultIP;
		}	
		else
		{
			HOSTENT* pHost = gethostbyname(szHostName);
			if (pHost != nullptr)
			{
				std::string sTempAddr;
				PIN_ADDR* pAddr = (PIN_ADDR*)pHost->h_addr_list;
				while (*pAddr != nullptr)
				{
					sTempAddr.assign(inet_ntoa(**pAddr));
					if (sDefaultIP.compare(sTempAddr) == 0)
					{
						sRetIP = sTempAddr;
						break;
					}
					else if (sRetIP.compare("") == 0)
					{
						sRetIP = sTempAddr;
					}
					else if (IsInternetIP((*pAddr)->s_addr))
					{
						sRetIP = sTempAddr;
					}
					pAddr += 1;
				}
			}
		}
	}
	return sRetIP;
}

/************************Start Of _TSendBufferLinkedList**************************************************/
void _TSendBufferLinkedList::DoInitial(const int iSize)
{
	m_pFirst = nullptr;
	m_pLast = nullptr;
	m_iNodeCacheSize = iSize;
}

void _TSendBufferLinkedList::DoFinalize()
{
	PSendBufferNode pNode;
	while (m_pFirst != nullptr)
	{
		pNode = m_pFirst;
		m_pFirst = pNode->Next;
		free(pNode->szBuf);
		delete(pNode);
	}
	m_pLast = nullptr;
}

/*
*  如有尾节点，先尝试向尾节点添加。如果尾节点不能放下发送的数据，则创建新节点
*  注意:这里的实现保证每个buffer只在一个发送节点中，一个节点可以对应多个发送buffer
*       这里的节点并没有保证维持m_iNodeCacheSize作为最大buffsize，是可能超大的
*/
void _TSendBufferLinkedList::AddBufferToList(const char* pBuf, int iCount)
{
	if ((m_pLast != nullptr) && (m_pLast->nBufLen + iCount <= m_iNodeCacheSize))
	{
		memcpy(&(m_pLast->szBuf[m_pLast->nBufLen]), pBuf, iCount);
		m_pLast->nBufLen += iCount;
	}
	else
	{
		int iAllocLen = 0;
		PSendBufferNode pNode = new TSendBufferNode;
		pNode->Next = nullptr;
		pNode->nStart = 0;
		if (iCount < m_iNodeCacheSize)
			iAllocLen = m_iNodeCacheSize;
		else
			iAllocLen = iCount;
		pNode->szBuf = (char*)malloc(iAllocLen);
		memcpy(pNode->szBuf, pBuf, iCount);
		pNode->nBufLen = iCount;

		if (m_pLast != nullptr)
			m_pLast->Next = pNode;
		else
			m_pFirst = pNode;
		m_pLast = pNode;
	}
}

int _TSendBufferLinkedList::GetBufferFromList(char* pDesBuf, int iBufMaxSize, int iBufUntreatedBytes)
{
	int iRemainLen = 0;
	//从队列中取等待发送的数据
	while (m_pFirst != nullptr)
	{
		PSendBufferNode pNode = m_pFirst;
		//当前用于发送的m_SendBlob中buffer中剩余的长度
		iRemainLen = iBufMaxSize - iBufUntreatedBytes;
		//该结点中要发送的数据长度
		int iDataLen = pNode->nBufLen - pNode->nStart;
		if (iDataLen > iRemainLen)
		{
			//数据不能一次发送完毕
			memcpy(&pDesBuf[iBufUntreatedBytes], &pNode->szBuf[pNode->nStart], iRemainLen);
			iBufUntreatedBytes = iBufMaxSize;
			pNode->nStart += iRemainLen;
			break;
		}
		else
		{
			memcpy(&pDesBuf[iBufUntreatedBytes], &pNode->szBuf[pNode->nStart], iDataLen);
			iBufUntreatedBytes += iDataLen;
			m_pFirst = pNode->Next;
			if (nullptr == m_pFirst)
				m_pLast = nullptr;
			free(pNode->szBuf);
			delete(pNode);
		}
	}
	return iBufUntreatedBytes;
}

/************************End Of _TSendBufferLinkedList****************************************************/



/************************Start Of CExecutableBase*********************************************************/
CExecutableBase::CExecutableBase() :m_bTerminated(false), m_pThread(nullptr), m_bExecuteOver(false), m_sThreadName("NoName")
{
	m_Event = CreateEvent(nullptr, false, false, nullptr);
}

CExecutableBase::~CExecutableBase()
{
	CloseHandle(m_Event);
}

void CExecutableBase::InitialWorkThread()
{
	if (nullptr == m_pThread)
	{
		m_pThread = new std::thread(&CExecutableBase::Execute, this);
		SendDebugString(m_sThreadName + ":new thread");
	}
}

void CExecutableBase::WaitThreadExecuteOver()
{
	if (!IsTerminated())
	{
		Terminate();
		if (!m_bExecuteOver)
		{
			SendDebugString(m_sThreadName + ":before wait");
			WaitForSingleObject(m_Event, INFINITE);
			SendDebugString(m_sThreadName + ":after wait");
		}
		else
		{
			SendDebugString(m_sThreadName + ":no join");
		}
		m_pThread = nullptr;
	}
}

void CExecutableBase::Execute()
{
	DoExecute();
	m_bExecuteOver = true;
	SetEvent(m_Event);
	SendDebugString(m_sThreadName + ":Execute Over");
}

void CExecutableBase::Terminate()
{ 
	m_bTerminated = true; 
	SendDebugString(m_sThreadName + ":Terminate");
}

bool CExecutableBase::IsTerminated()
{ 
	return m_bTerminated; 
}
/************************End Of CExecutableBase***********************************************************/