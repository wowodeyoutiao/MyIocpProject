/**************************************************************************************
@author: ³Â²ý
@content:
**************************************************************************************/

#include "CCUtils.h"

void _TSimpleHash::DoInitial(int iSize)
{
	m_iBucketSize = iSize;
	m_iHashItemCount = 0;
	m_ItemBuckets = new PHashPortItem[m_iBucketSize];
	for (int i = 0; i < m_iBucketSize; i++)
		m_ItemBuckets[i] = nullptr;
}

void _TSimpleHash::AddPortItem(const int iKey, void* pClient)
{
	int iHash = iKey % m_iBucketSize;
	PHashPortItem pItem = new THashPortItem;
	pItem->iHandle = iKey;
	pItem->pItem = pClient;
	pItem->Next = m_ItemBuckets[iHash];
	m_ItemBuckets[iHash] = pItem;
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

	for (int i = 0; i<m_iBucketSize; i++)
	{
		pItem = m_ItemBuckets[i];
		while (pItem != nullptr)
		{
			pNextItem = pItem->Next;
			delete(pItem);
			pItem = pNextItem;
		}
		m_ItemBuckets[i] = nullptr;
	}
	m_iHashItemCount = 0;
}

PPHashPortItem _TSimpleHash::FindPortItemPointer(const int iKey)
{
	int iHash = iKey % m_iBucketSize;
	PPHashPortItem point = &m_ItemBuckets[iHash];
	while (*point != nullptr)
	{
		if (iKey == (*point)->iHandle)
			break;
		else
			point = &((*point)->Next);
	}
	return point;
}

int _TSimpleHash::GetItemCount()
{
	return m_iHashItemCount;
}