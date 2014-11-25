/**************************************************************************************
@author: 陈昌
@content: 通用日志类
**************************************************************************************/

#include "CCLogSocket.h"

namespace CC_UTILS{

/************************Start Of CLogSocket******************************************/
	CLogSocket::CLogSocket(std::string &sName, bool bListView) : m_sServiceName(sName), m_iPingCount(0), m_bListView(bListView), m_pListViewInfo(nullptr)
	{
		m_DelayEvent = CreateEvent(nullptr, false, false, nullptr);
		m_ClientSocket.m_OnConnect = OnSocketConnect;
		m_ClientSocket.m_OnDisConnect = OnSocketDisConnect;
		m_ClientSocket.m_OnRead = OnSocketRead;
	}

	CLogSocket::~CLogSocket()
	{
		WaitThreadExecuteOver();
	}

	void CLogSocket::DoExecute()
	{
	}

	typedef struct _TAddLabelRec
	{
		TLogSocketHead head;
		TLogLabelInfo info;
	}TAddLabelRec, *PAddLabelRec;

	void CLogSocket::AddLabel(const std::string &sDesc, int iLeft, int iTop, int iTag = 0)
	{
		TAddLabelRec rec;
		if (m_ClientSocket.IsConnected)
		{
			memset((char *)&rec, 0, sizeof(rec));
			rec.head.ulSign = LOG_SEGMENTATION_SIGN;
			rec.head.usIdent = SMM_ADD_LABEL;
			rec.head.usBehindLen = sizeof(TLogLabelInfo);
			rec.info.iLeft = iLeft;
			rec.info.iTop = iTop;
			rec.info.iTag = iTag;
			//StrPLCopy(szCaption, Desc, LABEL_CAPTION_LENGTH);
		}
		m_ClientSocket.SendBuf((char*)&rec, sizeof(rec));
	}

	void CLogSocket::UpdateLabel(const std::string &sDesc, int iTag)
	{
	}

	void CLogSocket::AddListView(PListViewInfo pInfo)
	{
	}

	void CLogSocket::SetListViewColumns(PListViewInfo pInfo)
	{
	}

	void CLogSocket::UpdateListView(const std::string &sDesc, unsigned short usRow, unsigned short usCol)
	{
	}

	void CLogSocket::SendLogMsg(const std::string &sMsg, int iType = 0)
	{
	}

	void CLogSocket::SendImportantLog(unsigned short usIdent, const char* pData, unsigned short usDataLen)
	{
	}

	void CLogSocket::SendToServer(unsigned short usIdent, int iParam, const char* pBuf, unsigned short usBufLen)
	{
	}

	void CLogSocket::SendTracerData(const std::string &sRoleName, const char* pBuf, unsigned short usBufLen)
	{
	}

	void CLogSocket::SetServiceName(const std::string &sName)
	{
	}

	std::string CLogSocket::GetServiceName()
	{
		return m_sServiceName;
	}

	void CLogSocket::ClearWaitBuffers()
	{
	}

	void CLogSocket::SendHeartBeat()
	{
	}

	void CLogSocket::LoadConfig()
	{
	}

	void CLogSocket::RegisterServer()
	{
	}

	void CLogSocket::RegisterServerEx()
	{
	}

	void CLogSocket::SendWaitMsg()
	{
	}

	void CLogSocket::OnSocketConnect(void* Sender)
	{
		//if (m_OnConnectEvent != nullptr)
		//	m_OnConnectEvent(Sender);
		if ("" == m_sServiceName)
		{
			if (m_bListView)
				RegisterServerEx();
			else
				RegisterServer();
		}
		SendWaitMsg();
	}

	void CLogSocket::OnSocketDisConnect(void* Sender)
	{
		//if (m_OnDisConnectEvent != nullptr)
		//	m_OnDisConnectEvent(Sender);
	}

	void CLogSocket::OnSocketRead(void* Sender, const char* pBuf, unsigned short usBufLen)
	{
		//这里不解析收到的数据，只要远端的中央服务器有数据发出，就将m_iPingCount置零
		m_iPingCount = 0;
	}

/************************End Of CLogSocket******************************************/

}