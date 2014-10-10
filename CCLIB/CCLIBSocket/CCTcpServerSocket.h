/**************************************************************************************
@author: 陈昌
@content: tcp网络连接的底层库-服务端端口管理，头文件
**************************************************************************************/
#ifndef __CC_TCP_SERVER_SOCKET_H__
#define __CC_TCP_SERVER_SOCKET_H__

#include <mutex>
#include <list>
#include "CCTcpSocketCommon.h"

typedef struct _THashPortItem
{	
	int iHandle;
	void* pItem;
	_THashPortItem* Next;
}THashPortItem, *PHashPortItem, **PPHashPortItem;

//用于管理延时释放客户端的链表结构
typedef struct _TDelayFreeNode
{
	unsigned short usHandle;
	unsigned long ulAddTick;
	void* pObj;
	_TDelayFreeNode* Next;	
}TDelayFreeNode, *PDelayFreeNode;

/**
*  
* 客户端连接到服务器上的每一个连接对象
*
*/
class CClientConnector
{
public:
	CClientConnector();
	virtual ~CClientConnector();
	void Close();
	void SafeClose();
	int SendBuf(const char* pBuf, int iCount);
	int SendText(const std::string& s);
	void IocpSendback(int iTransfered);
	bool IocpReadback(int iTransfered);
	virtual void OnCreate(){}
	virtual void OnDestroy(){}
protected:
	virtual void Execute(unsigned long ulTick){}
	virtual void SocketRead(const char* pBuf, int iCount){}
private:
	void UpdateActive();
	void Clear();
	void PrepareSend(int iUntreated, int iTransfered);
	bool PrepareRecv();
	void DoActive(unsigned long ulTick);
	bool IsCorpse(unsigned long ulTick, unsigned long ulMaxCorpseTime);
	bool IsBlock(int iMaxBlockSize);
private:
    SOCKET m_Socket;
	std::string m_RemoteAddress;
	int m_RemotePort;
	unsigned short m_SocketHandle;      // 对应CIOCPServerSocketManager::m_HandleBuckets的handle值
	PSendBufferNode m_First, m_Last;
	bool m_bSending;
	bool m_bSafeClose;
	TBlock m_SendBlock;
	TBlock m_RecvBlock;
	std::mutex m_LockCS;                // 队列操作使用的互斥锁
	TOnSocketError m_OnSocketError;
	int m_TotalBufferLen;
	unsigned long m_ActiveTick;
	unsigned long m_LastSendTick;
	unsigned long m_BufferFullTick;		// 客户端发送缓冲区堆积满数据多久后踢掉
friend class CIOCPServerSocketManager;
};

/**
*  
* 负责处理客户端数据接收和发送的子工作线程---【不被继承,也就不用虚函数了！！！！】
*
*/
class CSubIOCPWorker : public CExecutableBase
{
public:
	CSubIOCPWorker(PHANDLE ph, TNotifyEvent evt) : m_pHIOCP(ph), m_OnSocketClose(evt){}
	~CSubIOCPWorker(){}
	void Execute();					
    void ShutDown();
private:			
	//noncopy对象
	CSubIOCPWorker(){}
	CSubIOCPWorker(CSubIOCPWorker& rhs){}
	CSubIOCPWorker& operator=(const CSubIOCPWorker& worker){}
private:
	PHANDLE m_pHIOCP;
    TNotifyEvent m_OnSocketClose;
};

/**
*  
* 管理器维护的主线程---不被继承---【不被继承,也就不用虚函数了！！！！】
* 用于处理Accept消息创建客户端连接对象，并启动管理具体的接收发送的CSubIOCPWorker对象线程
*
*/
class CMainIOCPWorker : public CExecutableBase
{
public:
	CMainIOCPWorker(void* parent);
	~CMainIOCPWorker();				
	void Execute();					
private:
    void MakeWorkers();
    void Close();
    bool Start(const std::string& sIP, int iPort);
    
	//noncopy对象
	CMainIOCPWorker(){}
	CMainIOCPWorker(CMainIOCPWorker& rhs){}
	CMainIOCPWorker& operator=(const CMainIOCPWorker& worker){}
private:
	TOnSocketError m_OnSocketError;
	TNotifyEvent m_OnSocketClose;
	TNotifyEvent m_OnReady;
	TOnSocketAccept m_OnSocketAccept;
	void* m_Parent;
	HANDLE m_hIOCP;
	SOCKET m_Socket;
	int m_iSubThreadCount;
	CSubIOCPWorker** m_SubWorkers;
friend class CIOCPServerSocketManager;
};

/**
*  
* 服务器端的Socket管理器，用IOCP模型实现
* 他主要负责启动Accept线程，并管理客户端的连接对象列表
*
*/
class CIOCPServerSocketManager : public CExecutableBase
{
public:
	CIOCPServerSocketManager();
	virtual ~CIOCPServerSocketManager();
	void Open();
	void Close();
	bool IsActive(){ return (m_MainWorker != nullptr); }
    void Execute();							 //子类不重载此方法
	bool DoCheckConnect(const std::string& sRemoteAddress);
	virtual void OnCreate(){}            	 // 创建后，使用对象调用
	virtual void OnDestroy(){}				 // 在析构函数确认线程结束后的最前面调用
public:
	std::string m_Address;					 // 本地IP
	int m_Port;								 // 监听端口
    TNotifyEvent m_OnConnect;
    TNotifyEvent m_OnDisConnect;  
	TNotifyEvent m_OnListenReady; 
	TOnSocketError m_OnClientError;
	TOnCreateClient m_OnCreateClient;
	TOnCheckAddress m_OnCheckAddress;
protected:
	virtual void DoActive(){}         // Open 后在Execute中调用
    void* ValueOf(const int iKey);  
	void SetMaxCorpseTime(const int iTime){ m_MaxCorpseTime = iTime; }
	void SetMaxBlockSize(const int iSize){ m_MaxBlockSize = iSize; }
private: 
	void DoReady(void* Sender);
	void DoSocketClose(void* Sender);
	void DoSocketAccept(HANDLE hIOCP, SOCKET hSocket, const std::string& sRemoteAddress, int iRemotePort);
	void DoAcceptError(void* Sender, int& iErrorCode);
	void DoClientError(void* Sender, int& iErrorCode);

	bool DelayFreeClient(unsigned long ulTick);
	unsigned short AllocHandle();
	void AddClient(int iHandle, void* pClient);
	void ClearHandles();
	void AddPortItem(const int iKey, void* pClient);
	void RemovePortItem(const int iKey);
	void ClearPortItem();
	PPHashPortItem FindPortItemPointer(const int iKey);             //这里需要返回的是PortItem的指针
private:
	CMainIOCPWorker* m_MainWorker;           // 主工作对象，负责Accept，和管理收发子线程组
	int m_MaxCorpseTime;				     //	客户端和服务器无通信的最长维护时间--否则断线
	int m_MaxBlockSize;						 // 客户端阻塞后，服务器为该客户端的阻塞缓冲区最大值--否则断线
	std::mutex m_LockCS;                     // 临界区操作使用的互斥锁
	bool m_BoDelayFree;                      // 处理延时释放客户端连接的标记
	PDelayFreeNode m_DFNFirst;               // 延时释放的头节点
	PDelayFreeNode m_DFNLast;				 // 延时释放的尾节点
	std::list<void*> m_ActiveConnects;		 // 维护当前连接客户端的对象列表，该成员变量还是需要对外开放，最少需要对子类开放
	unsigned short m_NewCreateHandle;		 // 最新创建的Handle编号，保存
	int m_DelayFreeHandleCount;				 // 当前正在延时释放的客户端句柄数量
	int m_HashHandleCount;					 //	当前连接中的客户端句柄数量
	PPHashPortItem m_HandleBuckets;			 // 用于存储客户端对象的简易hash
};

#endif //__CC_TCP_SERVER_SOCKET_H__