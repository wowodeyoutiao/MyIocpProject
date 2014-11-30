/**************************************************************************************
@author: 陈昌
@content: 自己使用的Hash类
		  支持可重入的循环遍历，不调用First的时候是保留当前遍历到的结点状态
**************************************************************************************/
#ifndef __CC_HASH_CLASS_H__
#define __CC_HASH_CLASS_H__

#include <functional>

namespace CC_UTILS{

	typedef std::function<bool(void* pointer, unsigned long ulParam, int &iResult)> TTouchFunc;


	//整数作为key的hash
	typedef std::function<void(void* pValue, int iKey)> TRemoveIntValueEvent;
	typedef struct _TIntHashItem
	{
		_TIntHashItem* BPrev;   //单个bucket链表中的前置结点
		_TIntHashItem* BNext;   //单个bucket链表中的后置结点
		_TIntHashItem* LPrev;   //总链表中的前置节点
		_TIntHashItem* LNext;   //总链表中的后置节点
		int Key;
		void* Value;
	}TIntHashItem, *PIntHashItem, **PPIntHashItem;

	class CIntegerHash
	{
	public:		
		CIntegerHash(unsigned long ulSize = 1023);
		virtual ~CIntegerHash();
		bool Add(const int iKey, void* pValue);
		void Clear();
		bool Remove(const int iKey);
		void* ValueOf(const int iKey);
		int Touch(TTouchFunc func, unsigned long ulParam);
		int GetNext(void* ptr);
		void First();
		bool Eof();
		void* GetNextNode();         //该函数包含返回下一结点，并将m_pCurrentQueueNode指向下一结点这两个操作
		int GetCount(){ return m_iTotalCount; };
	public:
		TRemoveIntValueEvent m_RemoveEvent;
	private:
		unsigned long HashOf(const int iKey);
		PPIntHashItem Find(const int iKey);
	private:
		int m_iTotalCount;
		unsigned long m_ulBucketSize;
		PIntHashItem m_pFirstListNode;
		PIntHashItem m_pLastListNode;
		PIntHashItem m_pCurrentQueueNode;
		PPIntHashItem m_TopBuckets;
	};




}

#endif //__CC_HASH_CLASS_H__