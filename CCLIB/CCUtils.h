/**************************************************************************************
@author: 陈昌
@content: 自己使用的一个常用结构函数库
**************************************************************************************/
#ifndef __CC_UTILS_H__
#define __CC_UTILS_H__

#include <string>
#include <vector>

namespace CC_UTILS{

	extern std::string G_CurrentExeFileName;       //当前程序的完整路径
	extern std::string G_CurrentExeDir;            //当前程序所在的目录

	//简易的Hash结点
	typedef struct _THashPortItem
	{
		int iHandle;
		void* pItem;
		_THashPortItem* Next;
	}THashPortItem, *PHashPortItem, **PPHashPortItem;

	/**
	* 一个简易的hash，数组冲突开链表
	*/
	typedef struct _TSimpleHash
	{
	public:
		void DoInitial(int iSize);
		void AddPortItem(const int iKey, void* pClient);
		void RemovePortItem(const int iKey);
		void ClearAllPortItems();
		PPHashPortItem FindPortItemPointer(const int iKey);     //这里需要返回的是PortItem的指针
		int GetItemCount();
	private:
		PPHashPortItem m_ppItemBuckets;							//用于存储客户端对象的简易hash
		int m_iBucketSize;                                      //固定的数组长度
		int m_iHashItemCount;  				     			    //当前连接中的客户端句柄数量
	}TSimpleHash, *PSimpleHash;

	/**
	* 一个简易的buffer的写入管理对象，主要用在一个长期维护的接受buffer对象
	*/
	typedef struct _TBufferStream
	{
	public:
		void Initialize();
		void Finalize();
		bool Write(const char* pBuf, const int iCount);
		bool Reset(int iUsedLength);
		void* GetMemPoint();
		int GetPosition();
	private:
		void* m_pMemory;
		int m_iMemorySize;
		int m_iMemoryPosition;
	}TBufferStream, *PBufferStream;

	//返回文件的最后修改时间转化成的整数，用以检测文件是否修改
	int GetFileAge(const std::string &sFileName);

	//返回文件的版本信息  
	//注意：需要工程中加入version.lib
	std::string GetFileVersion(const std::string &sFileName);

	//字符串分割
	void SplitStr(const std::string& s, const std::string& delim, std::vector<std::string>* ret);

	//字符串转整数，带默认值，不抛出异常
	int StrToIntDef(const std::string& sTemp, const int iDef);
}

#endif //__CC_UTILS_H__