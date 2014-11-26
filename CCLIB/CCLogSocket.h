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

namespace CC_UTILS{

	const int LOG_SEGMENTATION_SIGN = 0xFFDDEEAA;             // 分隔标识
	const int SMM_PING = 1000;                                // 心跳
	const int SMM_REGSITER = 1001;                            // 登记
	const int SMM_DEBUG_MESSAGE = 1002;                       // 日志信息
	const int SMM_ADD_LABEL = 1003;                           // 增加一个新的Label (来自Service)
	const int SMM_UPDATE_LABEL = 1004;                        // 更新Label信息  (来自Service)&(发送到监控窗口)
	const int SMM_APP_CLOSE = 1005;                           // 程序关闭
	const int SMM_REGSITER_EXT = 1006;                        // 登记
	const int SMM_ADD_LISTVIEW = 1007;                        // 添加listview
	const int SMM_UPDATE_LISTVIEW = 1008;                     // 更新listview
	const int SMM_TRACE_DATA = 1009;                          // 数据跟踪

	const int SERVICE_NAME_LENGTH = 16;						  // 服务名长度
	const int MAX_LISTVIEW_COUNT = 10;						  // 监控上listview最大个数
	const int LABEL_CAPTION_LENGTH = 31;                      // 监控上label控件的标题最大长度
	const std::string DEFAULT_SERVICE_IP = "127.0.0.1";		  // 默认的服务ip
	const int DEFAULT_SERVICE_PORT = 7822;                    // Service  -> Monitor Center   ?????????????????????????
	const int DEFAULT_MONITOR_PORT = 7823;                    // Viewer  -> Monitor Center    ?????????????????????????

	//日志等级
	const int LOG_TYPE_MESSAGE = 0;  
	const int LOG_TYPE_WARNING = 1;
	const int LOG_TYPE_ERROR = 2;
	const int LOG_TYPE_EXCEPTION = 3;
	const int LOG_TYPE_DEBUG = 255;

	//日志消息最外层头结构
	typedef struct _TLogSocketHead
	{
		unsigned long ulSign;            // 分隔标识
		unsigned short usIdent;          // 协议号
		unsigned short usBehindLen;      // 后续封包的长度
	}TLogSocketHead, *PLogSocketHead;

	//SMM_ADD_LISTVIEW
	typedef char TShortValue[SERVICE_NAME_LENGTH];	
	typedef TShortValue TListViewInfo[MAX_LISTVIEW_COUNT];
	typedef TListViewInfo* PListViewInfo;

	//SMM_UPDATE_LISTVIEW
	typedef struct _TUpdateViewInfo
	{
		unsigned short usRow;
		unsigned short usCol;
		TShortValue value;
	}TUpdateViewInfo, *PUpdateViewInfo;

	//SMM_ADD_LABEL  (Service -> Monitor IN) 来自Service
	typedef struct _TLogLabelInfo
	{
		int iLeft;
		int iTop;
		int iTag;
		char szCaption[LABEL_CAPTION_LENGTH+1];
	}TLogLabelInfo, *PLogLabelInfo;

	//SMM_UPDATE_LABEL(Service -> Monitor IN) 来自Service
	typedef struct _TUpdateLabelInfo
	{
		int iTag;								// SMM_ADD_LABEL 指定的tagid
		char szValue[LABEL_CAPTION_LENGTH + 1];
	}TUpdateLabelInfo, *PUpdateLabelInfo;

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
		CLogSocket(std::string &sName, bool bListView);
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
		HANDLE m_DelayEvent;
		CNetworkEventClientSocketManager m_ClientSocket;
		int m_iPingCount;
		std::list<PWaitBufferNode> m_WaitSendList;
		std::mutex m_WaitMsgCS;
		bool m_bListView;
		PListViewInfo m_pListViewInfo;
	};
}

#endif //__CC_LOG_SOCKET_H__