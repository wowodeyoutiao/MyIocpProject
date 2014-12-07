/**************************************************************************************
@author: 陈昌
@content: 通用日志类
**************************************************************************************/
#ifndef __CC_LOG_SOCKET_H__
#define __CC_LOG_SOCKET_H__

#include <string>
#include <list>
#include "CCTcpSocketCommon.h"
#include "CCTcpClientSocket.h"
#include "CCProtocol_Server.h"
#include "CCGameCommon.h"

namespace CC_UTILS{

	const int LOG_SEGMENTATION_SIGN = 0xFFDDEEAA;             // 分隔标识
	const int SMM_PING = 1000;                                // 心跳
	const int SMM_REGISTER = 1001;                            // 登记
	const int SMM_DEBUG_MESSAGE = 1002;                       // 日志信息
	const int SMM_ADD_LABEL = 1003;                           // 增加一个新的Label (来自Service)
	const int SMM_UPDATE_LABEL = 1004;                        // 更新Label信息  (来自Service)&(发送到监控窗口)
	const int SMM_APP_CLOSE = 1005;                           // 程序关闭
	const int SMM_REGISTER_EXT = 1006;                        // 登记
	const int SMM_ADD_LISTVIEW = 1007;                        // 添加listview
	const int SMM_UPDATE_LISTVIEW = 1008;                     // 更新listview
	const int SMM_TRACE_DATA = 1009;                          // 数据跟踪

	const int SERVICE_NAME_LENGTH = 16;						  // 服务名长度
	const int MAX_LISTVIEW_COUNT = 10;						  // 监控上listview最大个数
	const int LABEL_CAPTION_LENGTH = 31;                      // 监控上label控件的标题最大长度
	const std::string DEFAULT_LOG_SERVICE_IP = "127.0.0.1";	  // 默认的服务ip
	const int DEFAULT_LOG_SERVICE_PORT = 7822;                // 日志服务连接中央日志服务器的端口 Service  -> Monitor Center  
	const int DEFAULT_LOG_MONITOR_PORT = 7823;                // Viewer  -> Monitor Center    ?????????????????????????

	//日志等级
	const int lmtMessage = 0;
	const int lmtWarning = 1;
	const int lmtError = 2;
	const int lmtException = 3;
	const int lmtDebug = 255;

	//日志消息最外层头结构
	typedef struct _TLogSocketHead
	{
		unsigned long ulSign;            // 分隔标识
		unsigned short usIdent;          // 协议号
		unsigned short usBehindLen;      // 后续封包的长度
	}TLogSocketHead, *PLogSocketHead;

    //SMM_REGISTER
	typedef struct _TRegisterInfo
	{
		char szServiceName[SERVICE_NAME_LENGTH + 1];
		char szIPAddress[IP_ADDRESS_MAX_LEN + 1];
		char szVersion[LABEL_CAPTION_LENGTH + 1];
	}TRegisterInfo, *PRegisterInfo;

	typedef char TShortValue[SERVICE_NAME_LENGTH];
	//SMM_ADD_LISTVIEW		
	typedef TShortValue TListViewInfo[MAX_LISTVIEW_COUNT];
	typedef TListViewInfo* PListViewInfo;

	//SMM_REGISTER_EXT
	typedef struct _TRegisterInfoEx
	{
		TRegisterInfo BaseInfo;
		TListViewInfo ListViewInfo;
	}TRegisterInfoEx, *PRegisterInfoEx;

	//SMM_ADD_LABEL  (Service -> Monitor IN) 来自Service
	typedef struct _TLogLabelInfo
	{
		int iLeft;
		int iTop;
		int iTag;
		char szCaption[LABEL_CAPTION_LENGTH + 1];
	}TLogLabelInfo, *PLogLabelInfo;

	//SMM_UPDATE_LABEL  (Service -> Monitor IN) 来自Service
	typedef struct _TUpdateLabelInfo
	{
		int iTag;								// SMM_ADD_LABEL 指定的tagid
		char szValue[LABEL_CAPTION_LENGTH + 1];
	}TUpdateLabelInfo, *PUpdateLabelInfo;

	//SMM_UPDATE_LABEL  (Monitor OUT -> MonitorClient) 发送到监控窗口
	typedef struct _TUpdateLabelRec
	{
		char szServerName[SERVICE_NAME_LENGTH + 1];     // SMM_REGSITER注册的服务名
		TUpdateLabelInfo info;                   
	}TUpdateLabelRec, *PUpdateLabelRec;

	//SMM_UPDATE_LISTVIEW  (Service -> Monitor IN) 来自Service
	typedef struct _TUpdateViewInfo
	{
		unsigned short usRow;
		unsigned short usCol;
		TShortValue value;
	}TUpdateViewInfo, *PUpdateViewInfo;

    //SMM_UPDATE_LISTVIEW(Monitor OUT -> MonitorClient) 发送到监控窗口
	typedef struct _TUpdateListViewRec
	{
		char szServerName[SERVICE_NAME_LENGTH + 1];		// SMM_REGSITER注册的服务名
		TUpdateViewInfo info;
	}TUpdateListViewRec, *PUpdateListViewRec;

    //SMM_DEBUG_MESSAGE (Monitor OUT -> MonitorClient) 发送到监控窗口
	typedef struct _TDebugMsgRec
	{
		char szServerName[SERVICE_NAME_LENGTH + 1];		// SMM_REGSITER注册的服务名
		unsigned short usType;
	}TDebugMsgRec, *PDebugMsgRec;

	//SMM_TRACE_DATA
	typedef struct _TTraceData
	{
		//------------------------------------
		//这里的长度比原来多1
		//------------------------------------
		char szRoleName[ACTOR_NAME_MAX_LEN + 1];
	}TTraceData, *PTraceData;

	typedef struct _TWaitBufferNode
	{
		char* pBuf;
		unsigned short usBufLen;
		_TWaitBufferNode* pNext;
	}TWaitBufferNode, *PWaitBufferNode;

	/**
	*
	* 通用日志类
	* 创建该对象后，就可以直接调用，向配置的中央日志服务器发送日志
	*
	*/
	class CLogSocket : public CExecutableBase
	{
	public:
		CLogSocket(const std::string &sName, bool bListView = false);
		virtual ~CLogSocket();        
		virtual void DoExecute();    

		void AddLabel(const std::string &sDesc, int iLeft, int iTop, int iTag = 0);
		void UpdateLabel(const std::string &sDesc, int iTag);
		void AddListView(PListViewInfo pInfo);
		void SetListViewColumns(PListViewInfo pInfo);
		void UpdateListView(const std::string &sDesc, unsigned short usRow, unsigned short usCol);
		void SendLogMsg(const std::string &sMsg, int iType = 0);
		void SendToServer(unsigned short usIdent, int iParam, const char* pBuf, unsigned short usBufLen);
		void SendTracerData(const std::string &sRoleName, const char* pBuf, unsigned short usBufLen);		
		void SetServiceName(const std::string &sName);
		std::string GetServiceName();
	protected:
	private:
		void ClearWaitBuffers();
		void SendHeartBeat();
		void LoadConfig();
		void RegisterServer();
		void RegisterServerEx();
		void SendWaitMsg();
		void OnSocketConnect(void* Sender);
		void OnSocketDisconnect(void* Sender);
		void OnSocketRead(void* Sender, const char* pBuf, unsigned short usBufLen);
	private:
		std::string m_sServiceName;
		CNetworkEventClientSocketManager m_ClientSocket;
		int m_iPingCount;
		std::list<PWaitBufferNode> m_WaitSendList;
		std::mutex m_WaitMsgCS;
		bool m_bListView;
		PListViewInfo m_pListViewInfo;
	};
}

#endif //__CC_LOG_SOCKET_H__