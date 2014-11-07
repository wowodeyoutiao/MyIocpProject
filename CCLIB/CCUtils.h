/**************************************************************************************
@author: 陈昌
@content: 自己使用的一个常用结构函数库
**************************************************************************************/
#ifndef __CC_UTILS_H__
#define __CC_UTILS_H__

#include <string>

namespace CC_UTILS{
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

	//返回文件的最后修改时间代表的版本号
	int GetFileAge(const std::string &sFileName);
}

#endif //__CC_UTILS_H__