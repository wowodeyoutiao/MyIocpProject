/**************************************************************************************
@author: ³Â²ý
@content:
**************************************************************************************/

#include "CCUtils.h"
#include <Windows.h>

namespace CC_UTILS{

	void _TSimpleHash::DoInitial(int iSize)
	{
		m_iBucketSize = iSize;
		m_iHashItemCount = 0;
		m_ppItemBuckets = new PHashPortItem[m_iBucketSize];
		for (int i = 0; i < m_iBucketSize; i++)
			m_ppItemBuckets[i] = nullptr;
	}

	void _TSimpleHash::AddPortItem(const int iKey, void* pClient)
	{
		int iHash = iKey % m_iBucketSize;
		PHashPortItem pItem = new THashPortItem;
		pItem->iHandle = iKey;
		pItem->pItem = pClient;
		pItem->Next = m_ppItemBuckets[iHash];
		m_ppItemBuckets[iHash] = pItem;
		++m_iHashItemCount;
	}

	void _TSimpleHash::RemovePortItem(const int iKey)
	{
		PPHashPortItem pPrePointer = FindPortItemPointer(iKey);
		PHashPortItem pItem = *pPrePointer;
		if (pItem != nullptr)
		{
			*pPrePointer = pItem->Next;
			delete(pItem);
			--m_iHashItemCount;
		}
	}

	void _TSimpleHash::ClearAllPortItems()
	{
		PHashPortItem pItem = nullptr;
		PHashPortItem pNextItem = nullptr;

		for (int i = 0; i < m_iBucketSize; i++)
		{
			pItem = m_ppItemBuckets[i];
			while (pItem != nullptr)
			{
				pNextItem = pItem->Next;
				delete(pItem);
				pItem = pNextItem;
			}
			m_ppItemBuckets[i] = nullptr;
		}
		m_iHashItemCount = 0;
	}

	PPHashPortItem _TSimpleHash::FindPortItemPointer(const int iKey)
	{
		PPHashPortItem point = nullptr;
		int iHash = iKey % m_iBucketSize;
		if (m_ppItemBuckets[iHash] != nullptr)
		{
			point = &m_ppItemBuckets[iHash];
			while (*point != nullptr)
			{
				if (iKey == (*point)->iHandle)
					break;
				else
					point = &((*point)->Next);
			}
		}
		return point;
	}

	int _TSimpleHash::GetItemCount()
	{
		return m_iHashItemCount;
	}

	int GetFileAge(const std::string &sFileName)
	{
		WIN32_FIND_DATA FindData;
		HANDLE handle = FindFirstFile(sFileName.c_str(), &FindData);
		if (handle != INVALID_HANDLE_VALUE)
		{
			FindClose(handle);
			if (0 == (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				FILETIME LocalFileTime;
				FileTimeToLocalFileTime(&FindData.ftLastWriteTime, &LocalFileTime);
				unsigned short usHiWord, usLoWord;
				if (FileTimeToDosDateTime(&LocalFileTime, &usHiWord, &usLoWord))
				{
					int resultValue = usHiWord;
					resultValue = resultValue << (sizeof(usHiWord)* 8) | usLoWord;
					return resultValue;
				}
			}

		}
		return -1;
	}
}