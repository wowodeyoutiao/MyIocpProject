/**************************************************************************************
@author: 陈昌
@content: 自己使用的Hash类
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
		m_iTotalCount = 0;
	}

	void CIntegerHash::DoRemoveItem(PIntHashItem pItem)
	{
		if (pItem != nullptr)
		{
			if (pItem->BNext != nullptr)
				pItem->BNext->BPrev = pItem->BPrev;

			if (pItem->BPrev != nullptr)
				pItem->BPrev->BNext = pItem->BNext;
			else
			{
				unsigned long ulHash = HashOf(pItem->Key) % m_ulBucketSize;
				m_TopBuckets[ulHash] = pItem->BNext;
			}

			if (pItem->LNext != nullptr)
				pItem->LNext->LPrev = pItem->LPrev;
			else
				m_pLastListNode = pItem->LPrev;

			if (pItem->LPrev != nullptr)
				pItem->LPrev->LNext = pItem->LNext;
			else
				m_pFirstListNode = pItem->LNext;

			try
			{
				if (m_RemoveEvent != nullptr)
					m_RemoveEvent(pItem->Value, pItem->Key);
			}
			catch (...)
			{
				//吞下remove函数的异常
			}

			if (pItem == m_pCurrentQueueNode)
				m_pCurrentQueueNode = pItem->LNext;

			delete pItem;
			--m_iTotalCount;
		}
	}

	bool CIntegerHash::Remove(const int iKey)
	{
		bool retFlag = false;
		unsigned long ulHash = HashOf(iKey) % m_ulBucketSize;
		PIntHashItem pCurr = m_TopBuckets[ulHash];
		while (pCurr != nullptr)
		{
			if (iKey == pCurr->Key)
			{
				DoRemoveItem(pCurr);
				retFlag = true;
				break;
			}
			pCurr = pCurr->BNext;
		}
		return retFlag;
	}

	void* CIntegerHash::ValueOf(const int iKey)
	{
		PIntHashItem pItem = *(Find(iKey));
		if (pItem != nullptr)
			return pItem->Value;
		else
			return nullptr;
	}

	//func返回true的时候，从hash中删除该结点
	int CIntegerHash::Touch(TTouchFunc func, unsigned long ulParam)
	{
		int iRetCode = 0;
		if (func != nullptr)
		{
			PIntHashItem pCurr = m_pFirstListNode;
			PIntHashItem pNext = nullptr;
			while (pCurr != nullptr)
			{
				pNext = pCurr->LNext;
				if (func(pCurr->Value, ulParam, iRetCode))
					DoRemoveItem(pCurr);
				pCurr = pNext;
			}
		}
		return iRetCode;		
	}

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



/************************Start Of CStringHash**************************************************/

	CStringHash::CStringHash(unsigned long ulSize) : m_RemoveEvent(nullptr), m_iTotalCount(0), m_ulBucketSize(ulSize), m_pFirstListNode(nullptr),
		m_pLastListNode(nullptr), m_pCurrentQueueNode(nullptr)
	{
		m_TopBuckets = new PStrHashItem[ulSize];
	}

	CStringHash::~CStringHash()
	{
		Clear();
		delete[] m_TopBuckets;
	}

	bool CStringHash::Add(const std::string &sKey, void* pValue)
	{
		bool retFlag = false;
		PStrHashItem pBucket = *(Find(sKey));
		if (nullptr == pBucket)
		{
			unsigned long ulHash = HashOf(sKey) % m_ulBucketSize;
			pBucket = new TStrHashItem;
			pBucket->Key = sKey;
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

	void CStringHash::Clear()
	{
		PStrHashItem pCurr, pNext, pLast;
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
		m_iTotalCount = 0;
	}

	void CStringHash::DoRemoveItem(PStrHashItem pItem)
	{
		if (pItem != nullptr)
		{
			if (pItem->BNext != nullptr)
				pItem->BNext->BPrev = pItem->BPrev;

			if (pItem->BPrev != nullptr)
				pItem->BPrev->BNext = pItem->BNext;
			else
			{
				unsigned long ulHash = HashOf(pItem->Key) % m_ulBucketSize;
				m_TopBuckets[ulHash] = pItem->BNext;
			}

			if (pItem->LNext != nullptr)
				pItem->LNext->LPrev = pItem->LPrev;
			else
				m_pLastListNode = pItem->LPrev;

			if (pItem->LPrev != nullptr)
				pItem->LPrev->LNext = pItem->LNext;
			else
				m_pFirstListNode = pItem->LNext;

			try
			{
				if (m_RemoveEvent != nullptr)
					m_RemoveEvent(pItem->Value, pItem->Key);
			}
			catch (...)
			{
				//吞下remove函数的异常
			}

			if (pItem == m_pCurrentQueueNode)
				m_pCurrentQueueNode = pItem->LNext;

			delete pItem;
			--m_iTotalCount;
		}
	}

	bool CStringHash::Remove(const std::string &sKey)
	{
		bool retFlag = false;
		unsigned long ulHash = HashOf(sKey) % m_ulBucketSize;
		PStrHashItem pCurr = m_TopBuckets[ulHash];
		while (pCurr != nullptr)
		{
			if (0 == sKey.compare(pCurr->Key))
			{
				DoRemoveItem(pCurr);
				retFlag = true;
				break;
			}
			pCurr = pCurr->BNext;
		}
		return retFlag;
	}

	void* CStringHash::ValueOf(const std::string &sKey)
	{
		PStrHashItem pItem = *(Find(sKey));
		if (pItem != nullptr)
			return pItem->Value;
		else
			return nullptr;
	}

	int CStringHash::Touch(TTouchFunc func, unsigned long ulParam)
	{
		int iRetCode = 0;
		if (func != nullptr)
		{
			PStrHashItem pCurr = m_pFirstListNode;
			PStrHashItem pNext = nullptr;
			while (pCurr != nullptr)
			{
				pNext = pCurr->LNext;
				if (func(pCurr->Value, ulParam, iRetCode))
					DoRemoveItem(pCurr);
				pCurr = pNext;
			}
		}
		return iRetCode;
	}

	void CStringHash::First()
	{
		m_pFirstListNode = m_pCurrentQueueNode;
	}

	bool CStringHash::Eof()
	{
		return (m_pCurrentQueueNode == nullptr);
	}

	void* CStringHash::GetNextNode()
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

	//CStringHash的key不区分大小写
	unsigned long CStringHash::HashOf(const std::string &sKey)
	{
		unsigned long ulRetCode = 0;
		bool bHead = false;
		bool bTail = false;
		unsigned char key;
		for (int i = 0; i < sKey.length(); i++)
		{
			key = (unsigned char)sKey[i];

			//判断单双字节
			//由于ANSI字符有128个, 所以, ANSI字符的bit最高位为0, 当bit最高位为1时, 
			//就表示是个双字节字符了。而char（也即是signed char）的正负恰好由最高位决定，于是直接利用char < 0 来判断是否是双字节字符了。
			if (!bHead)
			{
				bTail = false;
				if (sKey[i] < 0)
					bHead = true;
			}
			else
			{
				bTail = true;
				bHead = false;
			}
			//单字节的大写字母都转成小写
			if ((!bHead) && (!bTail) && (key >= 0x41) && (key <= 0x5A))
				key = key | 0x20;

			ulRetCode = ((ulRetCode << 2) | (ulRetCode >> (sizeof(ulRetCode) * 8 - 2))) ^ key;
		}
		return ulRetCode;
	}

	PPStrHashItem CStringHash::Find(const std::string &sKey)
	{
		unsigned long ulHash = HashOf(sKey) % m_ulBucketSize;
		PPStrHashItem ppItem = &m_TopBuckets[ulHash];
		while (*ppItem != nullptr)
		{
			if (0 == sKey.compare((*ppItem)->Key))
				break;
			else
				ppItem = &((*ppItem)->BNext);
		}
		return ppItem;
	}

/************************End Of CStringHash****************************************************/

}