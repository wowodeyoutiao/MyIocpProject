/**************************************************************************************
@author: 陈昌
@content: tcp网络连接的底层库基础 头文件
**************************************************************************************/
#ifndef __CC_TCP_SOCKET_COMMON_H__
#define __CC_TCP_SOCKET_COMMON_H__

#define _WINSOCKAPI_
#include <Windows.h>
#include <WinSock2.h>
#include <thread>
#include <functional>

//常量定义
const unsigned long SHUTDOWN_FLAG = 0XFFFFFFFF;   // iocp端口关闭标志
const int MAX_IOCP_BUFFER_SIZE = 8 * 1024;        // IOCP投递缓冲区大小，一般设置成8k性能较佳
const int MAX_CLIENT_SEND_BUFFER_SIZE = MAX_IOCP_BUFFER_SIZE * 10 * 1024;  // 客户端可以发送的最大数据缓冲，也是服务器这端阻塞的最大客户端缓冲区  80M 

//定义TcpSocket的回调函数
typedef std::function<void (void* Sender)> TNotifyEvent;
typedef std::function<void (void* Sender, const char* pBuf, int iBufLen)> TOnSocketRead;   
typedef std::function<void (void* Sender, int& iErrorCode)> TOnSocketError;
typedef std::function<bool (const std::string& sIp)> TOnCheckAddress;
typedef std::function<void (HANDLE hIocp, SOCKET hSocket, std::string& sRAddress, int iRPort)> TOnSocketAccept;
class CClientConnector;
typedef std::function<CClientConnector* (const std::string& sIP)> TOnCreateClient;

//定义dll中回调函数类型
typedef bool(*TServiceDllCallBackFunc)(void* Sender);
//定义CCWindowsService.dll中启动服务的通用模块
typedef void(__stdcall *TServiceManagerFunc)(char* ServiceName, char* ServiceDesc, TServiceDllCallBackFunc OnStartFunc, TServiceDllCallBackFunc OnStopFunc);

/**
* 发送缓冲节点结构
*/
typedef struct _TSendBufferNode
{
	char* szBuf;             
	int nBufLen;             // 长度
	int nStart;              // 起始位置
	_TSendBufferNode* Next;  // 下个节点指针 
}TSendBufferNode, *PSendBufferNode;

/**
* 发送节点组成的链表
*/
typedef struct _TSendBufferLinkedList
{
public:
	void DoInitial(const int iSize);
	void DoFinalize();
	inline bool IsEmpty(){ return nullptr == m_pFirst; };
	void AddBufferToList(const char* pBuf, int iCount);
	int GetBufferFromList(char* pDesBuf, int iBufMaxSize, int iBufUntreatedBytes);
private:
	PSendBufferNode m_pFirst;   //链表头节点
	PSendBufferNode m_pLast;    //链表尾节点
	int m_iNodeCacheSize;       //节点的存储大小
}TSendBufferLinkedList;

//报错的枚举类型
enum TSocketErrorType {seConnect, seRead, seSend, seClose};

//socket事件的枚举类型
enum TSocketEvent {soIdle, soWrite, soRead};

/**
* 重叠io的结构体
*/
typedef struct _TBlock
{
	OVERLAPPED Overlapped;					//重叠
	WSABUF wsaBuffer;						//系统缓冲
	TSocketEvent Event;						//标记Socket读写
	char Buffer[MAX_IOCP_BUFFER_SIZE - 1];	//用户缓冲
}TBlock, *PBlock;

/*
* 自旋锁的实现，比互斥锁更高效（不会休眠）,单占用cpu
*/
enum {LOCK_IS_FREE = 0, LOCK_IS_TAKEN = 1};
#define LOCK(l) while(InterlockedCompareExchange(&l, LOCK_IS_TAKEN, LOCK_IS_FREE) == 1) {};
#define UNLOCK(l) InterlockedExchange(&l, LOCK_IS_FREE);

//通过WSAStartup函数完成对Winsock服务的初始化
bool DoInitialWinSocket();

//终止Winsock 2 DLL (Ws2_32.dll) 的使用
void DoFinalizeWinSocket();

//Debug信息
void SendDebugString(const std::string& sInfo);

//判断当前ip是否是外网ip，根据当前内网设置来调整
bool IsInternetIP(const u_long ulIP);

//返回本机的对外ip地址
std::string GetInternetIP(const std::string& sDefaultIP = "");

/*
* 挂载到线程上运行的对象基类
*
*/
class CExecutableBase
{
public:
	CExecutableBase();
	virtual ~CExecutableBase();
	void InitialWorkThread();
	void Execute();
	virtual void DoExecute() = 0;
	std::thread* m_pThread;          // 内部执行线程指针
protected:
	void WaitThreadExecuteOver();
	void Terminate();
	bool IsTerminated();
protected:
	HANDLE m_Event;                  // 网络事件句柄
	std::string m_sThreadName;       // 线程名称
private:
	bool m_bTerminated;              // 停止Execute方法的循环执行标记
	bool m_bExecuteOver;             // Execute执行完毕
};

#endif //__CC_TCP_SOCKET_COMMON_H__