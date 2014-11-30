/**************************************************************************************
@author: 陈昌
@content: 自己使用的Hash类
		  支持可重入的循环遍历，不调用First的时候是保留当前遍历到的结点状态
**************************************************************************************/

#include "CCHashClass.h"

namespace CC_UTILS{

/************************Start Of CIntegerHash**************************************************/

	CIntegerHash::CIntegerHash(unsigned long ulSize) :m_RemoveEvent(nullptr), m_iTotalCount(0), m_ulBucketSize(ulSize), m_pFirstListNode(nullptr),
		m_pLastListNode(nullptr), m_pCurrentQueueNode(nullptr)
	{
		m_TopBuckets = new PIntHashItem[ulSize];
	}	

	CIntegerHash::~CIntegerHash()
	{
		Clear();
		delete[] m_TopBuckets;
	}

	bool CIntegerHash::Add(const int iKey, void* pValue)
	{
		bool retFlag = false;
		PIntHashItem pBucket = *(Find(iKey));
		if (nullptr == pBucket)
		{
			unsigned long ulHash = HashOf(iKey) % m_ulBucketSize;
			pBucket = new TIntHashItem;
			pBucket->Key = iKey;
			pBucket->Value = pValue;
			pBucket->BPrev = nullptr;
			pBucket->BNext = m_TopBuckets[ulHash];
			pBucket->LPrev = m_pLastListNode;
			pBucket->LNext = nullptr;

			if (pBucket->BNext != nullptr)
				pBucket->BNext->BPrev = pBucket;
			if (pBucket->LPrev != nullptr)
				pBucket->LPrev->LNext = pBucket;
			else
				m_pFirstListNode = pBucket;

			m_TopBuckets[ulHash] = pBucket;
			m_pLastListNode = pBucket;
			++m_iTotalCount;
			retFlag = true;
		}
		return retFlag;
	}

	void CIntegerHash::Clear()
	{
		PIntHashItem pCurr, pNext, pLast;
		pCurr = m_pFirstListNode;
		while (pCurr != nullptr)
		{
			pNext = pCurr->LNext;
			pLast = pCurr;
			pCurr = pNext;

			if (m_RemoveEvent != nullptr)
				m_RemoveEvent(pLast->Value, pLast->Key);
			delete pLast;
		}
		m_pFirstListNode = nullptr;
		m_pLastListNode = nullptr;
		m_pCurrentQueueNode = nullptr;
		for (int i = 0; i < (int)m_ulBucketSize; i++)
			m_TopBuckets[i] = nullptr;
		m_iTotalCount;
	}

	bool CIntegerHash::Remove(const int iKey)
	{

	}

	void* CIntegerHash::ValueOf(const int iKey)
	{
		PIntHashItem pItem = *(Find(iKey));
		if (pItem != nullptr)
			return pItem->Value;
		else
			return nullptr;
	}

	int CIntegerHash::Touch(TTouchFunc func, unsigned long ulParam)
	{}

	int CIntegerHash::GetNext(void* ptr)
	{}

	void CIntegerHash::First()
	{
		m_pFirstListNode = m_pCurrentQueueNode;
	}

	bool CIntegerHash::Eof()
	{
		return (m_pCurrentQueueNode == nullptr);
	}

	void* CIntegerHash::GetNextNode()
	{
		if (nullptr == m_pCurrentQueueNode)
			First();
		if (m_pCurrentQueueNode != nullptr)
		{
			void* p = m_pCurrentQueueNode->Value;
			m_pCurrentQueueNode = m_pCurrentQueueNode->LNext;
			return p;
		}
		else
			return nullptr;
	}

	unsigned long CIntegerHash::HashOf(const int iKey)
	{
		return iKey;
	}

	PPIntHashItem CIntegerHash::Find(const int iKey)
	{
		unsigned long ulHash = HashOf(iKey) % m_ulBucketSize;
		PPIntHashItem ppItem = &m_TopBuckets[ulHash];
		while (*ppItem != nullptr)
		{
			if (iKey == (*ppItem)->Key)
				break;
			else
				ppItem = &((*ppItem)->BNext);
		}
		return ppItem;
	}

/************************End Of CIntegerHash****************************************************/

}