/**************************************************************************************
@author: 陈昌
@content: 通用日志类
**************************************************************************************/

#include "CCLogSocket.h"
#include "CCIniFileParser.h"

namespace CC_UTILS{

/************************Start Of CLogSocket******************************************/
	CLogSocket::CLogSocket(std::string &sName, bool bListView) : m_sServiceName(sName), m_iPingCount(0), m_bListView(bListView), m_pListViewInfo(nullptr)
	{
		m_ClientSocket.m_OnConnect = std::bind(&CLogSocket::OnSocketConnect, this, std::placeholders::_1);
		m_ClientSocket.m_OnDisConnect = std::bind(&CLogSocket::OnSocketDisconnect, this, std::placeholders::_1);
		m_ClientSocket.m_OnRead = std::bind(&CLogSocket::OnSocketRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	}

	CLogSocket::~CLogSocket()
	{
		WaitThreadExecuteOver();
	}

	void CLogSocket::DoExecute()
	{
		unsigned long ulLastCheckTick = 0;
		unsigned long ulTick = 0;
		LoadConfig();
		while (!IsTerminated())
		{
			try
			{
				m_ClientSocket.Execute();
				ulTick = GetTickCount();
				if (ulTick - ulLastCheckTick >= 10 * 1000)
				{
					ulLastCheckTick = ulTick;
					if (m_ClientSocket.IsConnected())
					{
						if (m_iPingCount > 3)
							m_ClientSocket.Close();
						else
							SendHeartBeat();
					}
					else
						m_ClientSocket.Open();
				}
			}
			catch (...)
			{
				SendDebugString("CLogSocket::DoExecute Exception");
			}
			WaitForSingleObject(m_Event, 10);
		}
		m_ClientSocket.Close();
	}

	typedef struct _TAddLabelSendRec
	{
		TLogSocketHead head;
		TLogLabelInfo info;
	}TAddLabelSendRec, *PAddLabelSendRec;

	void CLogSocket::AddLabel(const std::string &sDesc, int iLeft, int iTop, int iTag)
	{
		TAddLabelSendRec rec;
		if (m_ClientSocket.IsConnected())
		{
			memset((char *)&rec, 0, sizeof(rec));
			rec.head.ulSign = LOG_SEGMENTATION_SIGN;
			rec.head.usIdent = SMM_ADD_LABEL;
			rec.head.usBehindLen = sizeof(TLogLabelInfo);
			rec.info.iLeft = iLeft;
			rec.info.iTop = iTop;
			rec.info.iTag = iTag;
			memcpy_s(rec.info.szCaption, LABEL_CAPTION_LENGTH, sDesc.c_str(), sDesc.length());
		}
		m_ClientSocket.SendBuf((char*)&rec, sizeof(rec));
	}

	typedef struct _TUpdateLabelSendRec
	{
		TLogSocketHead head;
		TUpdateLabelInfo info;
	}TUpdateLabelSendRec, *PUpdateLabelSendRec;

	void CLogSocket::UpdateLabel(const std::string &sDesc, int iTag)
	{
		TUpdateLabelSendRec rec;
		if (m_ClientSocket.IsConnected())
		{
			memset((char *)&rec, 0, sizeof(rec));
			rec.head.ulSign = LOG_SEGMENTATION_SIGN;
			rec.head.usIdent = SMM_UPDATE_LABEL;
			rec.head.usBehindLen = sizeof(TUpdateLabelInfo);
			rec.info.iTag = iTag;
			memcpy_s(rec.info.szValue, LABEL_CAPTION_LENGTH, sDesc.c_str(), sDesc.length());
		}
		m_ClientSocket.SendBuf((char*)&rec, sizeof(rec));
	}

	typedef struct _TAddListViewSendRec
	{
		TLogSocketHead head;
		TListViewInfo info;
	}TAddListViewSendRec, *PAddListViewSendRec;

	void CLogSocket::AddListView(PListViewInfo pInfo)
	{
		TAddListViewSendRec rec;
		if (m_ClientSocket.IsConnected())
		{
			memset((char *)&rec, 0, sizeof(rec));
			rec.head.ulSign = LOG_SEGMENTATION_SIGN;
			rec.head.usIdent = SMM_ADD_LISTVIEW;
			rec.head.usBehindLen = sizeof(TListViewInfo);
			//---------------------------------
			//---------------------------------
			//---------------------------------
			//??????????这个结构赋值应该有问题
			memcpy(rec.info, pInfo, sizeof(TListViewInfo));
		}
		m_ClientSocket.SendBuf((char*)&rec, sizeof(rec));
	}

	void CLogSocket::SetListViewColumns(PListViewInfo pInfo)
	{
		//-----------------------------
		//-----------------------------
		//--------这个结构？？？？？？
		if (nullptr == m_pListViewInfo)
			m_pListViewInfo = new TListViewInfo[MAX_LISTVIEW_COUNT];

		//---------------------------------
		//---------------------------------
		//---------------------------------
		//??????????这个结构赋值应该有问题
		memcpy(m_pListViewInfo, pInfo, sizeof(TListViewInfo));
	}

	typedef struct _TUpdateListViewSendRec
	{
		TLogSocketHead head;
		TUpdateViewInfo info;
	}TUpdateListViewSendRec, *PUpdateListViewSendRec;

	void CLogSocket::UpdateListView(const std::string &sDesc, unsigned short usRow, unsigned short usCol)
	{
		TUpdateListViewSendRec rec;
		if (m_ClientSocket.IsConnected())
		{
			memset((char *)&rec, 0, sizeof(rec));
			rec.head.ulSign = LOG_SEGMENTATION_SIGN;
			rec.head.usIdent = SMM_UPDATE_LISTVIEW;
			rec.head.usBehindLen = sizeof(TUpdateViewInfo);
			rec.info.usRow = usRow;
			rec.info.usCol = usCol;
			memcpy_s(rec.info.value, SERVICE_NAME_LENGTH, sDesc.c_str(), sDesc.length());
		}
		m_ClientSocket.SendBuf((char*)&rec, sizeof(rec));
	}

	void CLogSocket::SendLogMsg(const std::string &sMsg, int iType)
	{
#ifndef TEST
		if (lmtDebug == iType)
			return;
#endif
		/*
		//---------------------------------
		//---------------------------------
		//---------------------------------
		if ((LOG_TYPE_ERROR == iType) || (LOG_TYPE_EXCEPTION == iType))
			EventReportError(msg);
		*/
		int iMsgLen = sMsg.length();
		int iBufLen = sizeof(TLogSocketHead) + 1 + iMsgLen;
		char* pBuf = (char*)malloc(iBufLen);
		((PLogSocketHead)pBuf)->ulSign = LOG_SEGMENTATION_SIGN;
		((PLogSocketHead)pBuf)->usIdent = SMM_DEBUG_MESSAGE;
		((PLogSocketHead)pBuf)->usBehindLen = iBufLen - sizeof(TLogSocketHead);

		char* pb = pBuf + sizeof(TLogSocketHead);
		*pb = iType;
		memcpy((pBuf + sizeof(TLogSocketHead) + 1), sMsg.c_str(), iMsgLen);

		if ((m_ClientSocket.IsConnected()) && (m_sServiceName != ""))
			m_ClientSocket.SendBuf(pBuf, iBufLen, true);
		else
		{
			std::lock_guard<std::mutex> guard(m_WaitMsgCS);
			if (m_WaitSendList.size() < 3000)
			{
				PWaitBufferNode pNode = new TWaitBufferNode;
				pNode->pBuf = pBuf;
				pNode->usBufLen = iBufLen;
				pNode->pNext = nullptr;
				m_WaitSendList.push_back(pNode);
			}
			else
				free(pBuf);
		}
	}

	void CLogSocket::SendToServer(unsigned short usIdent, int iParam, const char* pBuf, unsigned short usBufLen)
	{
		if (nullptr == pBuf)
			usBufLen = 0;
		unsigned short usSendLen = sizeof(TLogSocketHead) + usBufLen;
		char* pTempBuf = (char*)malloc(usSendLen);
		((PLogSocketHead)pBuf)->ulSign = LOG_SEGMENTATION_SIGN;
		((PLogSocketHead)pBuf)->usIdent = usIdent;
		((PLogSocketHead)pBuf)->usBehindLen = usSendLen - sizeof(TLogSocketHead);
		if ((pBuf != nullptr) && (usBufLen > 0))
			memcpy((pTempBuf + sizeof(TLogSocketHead)), pBuf, usBufLen);

		m_ClientSocket.SendBuf(pTempBuf, usSendLen, true);
	}

	void CLogSocket::SendTracerData(const std::string &sRoleName, const char* pBuf, unsigned short usBufLen)
	{
		if ((nullptr == pBuf) || (0 == usBufLen))
			return;

		unsigned short usSendLen = sizeof(TLogSocketHead) + sizeof(TTraceData) + usBufLen;
		char* pTempBuf = (char*)malloc(usSendLen);
		((PLogSocketHead)pBuf)->ulSign = LOG_SEGMENTATION_SIGN;
		((PLogSocketHead)pBuf)->usIdent = SMM_TRACE_DATA;
		((PLogSocketHead)pBuf)->usBehindLen = usSendLen - sizeof(TLogSocketHead);
		memset((pTempBuf + sizeof(TLogSocketHead)), 0, sizeof(TTraceData));
		memcpy_s(((PTraceData)(pTempBuf + sizeof(TLogSocketHead)))->szRoleName, ACTOR_NAME_MAX_LEN, sRoleName.c_str(), sRoleName.length());
		if ((pBuf != nullptr) && (usBufLen > 0))
			memcpy(pTempBuf+sizeof(TLogSocketHead)+sizeof(TTraceData), pBuf, usBufLen);
		m_ClientSocket.SendBuf(pTempBuf, usBufLen, true);
	}

	void CLogSocket::SetServiceName(const std::string &sName)
	{
		m_sServiceName = sName;
		if (!m_ClientSocket.IsConnected())
			m_ClientSocket.Open();
		RegisterServer();
	}

	std::string CLogSocket::GetServiceName()
	{
		return m_sServiceName;
	}

	void CLogSocket::ClearWaitBuffers()
	{
		std::list<PWaitBufferNode>::iterator vIter;
		PWaitBufferNode pNode;
		std::lock_guard<std::mutex> guard(m_WaitMsgCS);
		for (vIter = m_WaitSendList.begin(); vIter != m_WaitSendList.end(); ++vIter)
		{
			pNode = (PWaitBufferNode)*vIter;
			if ((pNode != nullptr) && (pNode->pBuf != nullptr))
				free(pNode->pBuf);
			delete pNode;
		}
		m_WaitSendList.clear();
	}

	void CLogSocket::SendHeartBeat()
	{
		TLogSocketHead head;
		head.ulSign = LOG_SEGMENTATION_SIGN;
		head.usIdent = SMM_PING;
		head.usBehindLen = 0;
		m_ClientSocket.SendBuf((char*)&head, sizeof(head));
	}

	void CLogSocket::LoadConfig()
	{
		//---------------------------------
		//---------------------------------
		//---------------------------------
		//std::string sConfigFileName(G_CurrentExePath + "config.ini");
		std::string sConfigFileName("");
		CWgtIniFile* pIniFileParser = new CWgtIniFile();
		pIniFileParser->loadFromFile(sConfigFileName);
		try
		{
			m_ClientSocket.m_Address = pIniFileParser->getString("Monitor", "IP", DEFAULT_LOG_SERVICE_IP);
			m_ClientSocket.m_Port = pIniFileParser->getInteger("Monitor", "Port", DEFAULT_LOG_SERVICE_PORT);
			m_ClientSocket.Open();
			delete pIniFileParser;
		}
		catch (...)
		{
			delete pIniFileParser;
		}
	}

	void CLogSocket::RegisterServer()
	{
		if ("" == m_sServiceName)
			return;
		int iBufLen = sizeof(TLogSocketHead)+sizeof(TRegisterInfo);
		char* pBuf = (char*)malloc(iBufLen);
		((PLogSocketHead)pBuf)->ulSign = LOG_SEGMENTATION_SIGN;
		((PLogSocketHead)pBuf)->usIdent = SMM_REGISTER;
		((PLogSocketHead)pBuf)->usBehindLen = iBufLen - sizeof(TLogSocketHead);

		//---------------------------------------
		//version := _FileVersion(ParamStr(0));
		std::string version = "111";  
		PRegisterInfo pInfo = (PRegisterInfo)(pBuf + sizeof(TLogSocketHead));
		memcpy_s(pInfo->szServiceName, SERVICE_NAME_LENGTH, m_sServiceName.c_str(), m_sServiceName.length());
		memcpy_s(pInfo->szVersion, LABEL_CAPTION_LENGTH, version.c_str(), version.length());
		m_ClientSocket.SendBuf(pBuf, iBufLen, true);
	}

	void CLogSocket::RegisterServerEx()
	{	
		if (("" == m_sServiceName) || (nullptr == m_pListViewInfo))
			return;
		int iBufLen = sizeof(TLogSocketHead)+sizeof(TRegisterInfoEx);
		char* pBuf = (char*)malloc(iBufLen);
		((PLogSocketHead)pBuf)->ulSign = LOG_SEGMENTATION_SIGN;
		((PLogSocketHead)pBuf)->usIdent = SMM_REGISTER_EXT;
		((PLogSocketHead)pBuf)->usBehindLen = iBufLen - sizeof(TLogSocketHead);

		//---------------------------------------
		//version := _FileVersion(ParamStr(0));
		std::string version = "111";
		PRegisterInfoEx pInfo = (PRegisterInfoEx)(pBuf + sizeof(TLogSocketHead));
		memcpy_s(pInfo->BaseInfo.szServiceName, SERVICE_NAME_LENGTH, m_sServiceName.c_str(), m_sServiceName.length());
		memcpy_s(pInfo->BaseInfo.szVersion, LABEL_CAPTION_LENGTH, version.c_str(), version.length());

		//---------------------------------
		//---------------------------------
		//---------------------------------
		//??????????这个结构赋值应该有问题
		memcpy(pInfo->ListViewInfo, m_pListViewInfo, sizeof(TListViewInfo));

		m_ClientSocket.SendBuf(pBuf, iBufLen, true);
	}

	void CLogSocket::SendWaitMsg()
	{
		if ((m_WaitSendList.size() > 0) && (m_sServiceName != ""))
		{
			std::list<PWaitBufferNode>::iterator vIter;
			PWaitBufferNode pNode;
			std::lock_guard<std::mutex> guard(m_WaitMsgCS);
			for (vIter = m_WaitSendList.begin(); vIter != m_WaitSendList.end(); ++vIter)
			{
				pNode = (PWaitBufferNode)*vIter;
				if ((pNode != nullptr) && (pNode->pBuf != nullptr))
					m_ClientSocket.SendBuf(pNode->pBuf, pNode->usBufLen, true);
				delete pNode;
			}
			m_WaitSendList.clear();
		}
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

	void CLogSocket::OnSocketDisconnect(void* Sender)
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