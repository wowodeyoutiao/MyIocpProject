/**************************************************************************************
@author: 陈昌
@content: tcp网络连接的底层库-客户端端口管理，头文件
**************************************************************************************/
#ifndef __CC_TCP_CLIENT_SOCKET_H__
#define __CC_TCP_CLIENT_SOCKET_H__

#include <mutex>
#include "CCUtils.h"
#include "CCTcpSocketCommon.h"
#include "CCProtocol_Server.h"

/**
*  
* 客户端的Socket管理器，用单线程网络事件模型实现 ---- 注：这个模型的实现还需要更多的调整！！！！
*
*/
class CNetworkEventClientSocketManager
{
public:
    CNetworkEventClientSocketManager();
	virtual ~CNetworkEventClientSocketManager();
    unsigned int SendBuf(const char* pBuf, unsigned int Len, bool BoFree = false);
    bool Open();
    bool Close();
    bool Execute();  
	bool IsActive(){ return m_BoActive; }
	bool IsConnected(){ return m_BoConnected; }
public:
    std::string m_LocalAddress;			// 本地ip
    std::string m_Address;				// 远端ip
    int m_Port;							// 远端port
    TOnSocketRead m_OnRead;
	TOnSocketError m_OnError;
    TNotifyEvent m_OnConnect;
    TNotifyEvent m_OnDisConnect;    
private:
    void DoRead();
    void DoWrite();
    bool DoError(TSocketErrorType seType);
    bool DoInitialize();
    void DoSend();
private:
	SOCKET m_CSocket;                   // 原始套接字
	HANDLE m_Event;                     // 网络事件句柄
    std::mutex m_SendCS;                // 发送队列使用的互斥锁
    PSendBufferNode m_First, m_Last;    // 发送缓冲区链表
    int m_Count;                        // 发送队列的个数
    bool m_BoActive;                    // WSAConnect成功后开启状态, Close时关闭
	bool m_BoConnected;					// DoWrite后开启状态, Close时关闭
};

/**
*  
* 客户端的Socket管理器，用IOCP模型实现
*
* 注：现在是通过互斥锁来实现临界区的管理
*	lock_guard 进行临界区的处理，不担心异常或其它导致的死锁
*	{			
*		std::lock_guard<std::mutex> guard(m_SendCS); 
*		//加锁的代码段
*	}
*   离开这个代码块，guard就要释放
*
* 后面会考虑通过自旋锁来实现，CCTcpSocketCommon.h 中定义的 LOCK 和 UNLOCK
* 自旋锁无sleep，它的效率更高，但因为异常出现死锁的可能性感觉更大一些，这个需要进一步评估
*/
class CIOCPClientSocketManager : public CExecutableBase
{
public:
	CIOCPClientSocketManager();
	virtual ~CIOCPClientSocketManager();
    int SendBuf(const char* pBuf, int Count);
    bool Open();
    void Close(bool BoClearBuffer = true);
	bool IsActive(){ return m_BoActive; }
	bool IsConnected(){ return m_BoConnected; }
	void SetReconnectInterval(unsigned long interval){ m_Reconnect_Interval = interval; }
    void DoExecute();						//子类不重载此方法
public:
    TOnSocketRead m_OnRead;
	TOnSocketError m_OnError;
    TNotifyEvent m_OnConnect;
    TNotifyEvent m_OnDisConnect;  
    std::string m_LocalAddress;			// 本地ip
    std::string m_Address;				// 远端ip
    int m_Port;							// 远端port
	int m_TotalBufferlen;               // buffer总长度
protected:
	int ParseSocketReadData(int iType, const char* pBuf, int iCount);                                //由子类的onsocketread调用
	virtual void ProcessReceiveMsg(PServerSocketHeader pHeader, const char* pData, int iDataLen){};  //处理具体的消息包数据，子类实现
private:
	bool DoInitialize();
	bool DoError(TSocketErrorType seType);
	bool DoConnect();
	void DoQueued();
	void DoClose();
	void Clear();
	void PrepareSend(int iUntreated, int iTransfered);
	bool PrepareRecv();
	void IocpSendback(int Transfered);
	bool IocpReadback(int Transfered);
private:
    bool m_BoActive;                    // 
	bool m_BoConnected;					// 和远端端口的连接标记
	bool m_Sending;                     //
	HANDLE m_hIOCP;                     // iocp的句柄
	SOCKET m_CSocket;                   // 原始套接字	
	std::mutex m_LockCS;                // 队列操作使用的互斥锁
	unsigned long m_SendLock;			// 接收自旋锁 变量 （暂未使用）
	unsigned long m_RecvLock;           // 发送自旋锁 变量 （暂未使用）
	TSendBufferLinkedList m_SendList;   // 发送缓冲区链表
	TBlock m_SendBlock;                 // 用于发送的
	TBlock m_RecvBlock;                 // 用于接收的
    SOCKADDR_IN m_SocketAddr;					   // 
	unsigned long m_Reconnect_Interval;			   // 重连间隔
	CC_UTILS::PBufferStream m_pReceiveBuffer;      // 处理socket数据接收的buffer
};

#endif //__CC_TCP_CLIENT_SOCKET_H__