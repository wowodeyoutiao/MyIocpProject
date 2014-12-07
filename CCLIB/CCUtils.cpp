/**************************************************************************************
@author: 陈昌
@content:
**************************************************************************************/

#include "CCUtils.h"
#include <Windows.h>
#pragma comment(lib, "version.lib")

namespace CC_UTILS{

	std::string G_CurrentExeFileName;   //当前程序的完整路径
	std::string G_CurrentExeDir;        //当前程序所在的目录

	/************************Start Of _TSimpleHash******************************************/
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
	/************************End Of _TSimpleHash******************************************/


	/************************Start Of _TBufferStream******************************************/
	const int MEM_SIZE_DELTA = 0x2000;

	void _TBufferStream::Initialize()
	{
		m_iMemoryPosition = 0;
		m_iMemorySize = MEM_SIZE_DELTA;
		m_pMemory = malloc(m_iMemorySize);
	}

	void _TBufferStream::Finalize()
	{
		free(m_pMemory);
	}

	bool _TBufferStream::Write(const char* pBuf, const int iCount)
	{
		bool retflag = false;
		int iNeedLength = m_iMemoryPosition + iCount;
		if (iNeedLength > m_iMemorySize)
		{
			//在iNeedLength基础上，以整0x2000增加
			m_iMemorySize = (iNeedLength + (MEM_SIZE_DELTA - 1)) & (~(MEM_SIZE_DELTA - 1));
			void* pTempMem = realloc(m_pMemory, m_iMemorySize);
			if (nullptr == pTempMem)
			{
				//对于这里的内存重分配失败则再尝试重分配一次，如果还是失败则返回False，内存指针都不会改变
				//这段代码的健壮性还需要进一步研究！！！！！！！！！
				pTempMem = realloc(m_pMemory, m_iMemorySize);
				if (nullptr != pTempMem)
				{
					m_pMemory = pTempMem;
					retflag = true;
				}
			}
			else
			{
				m_pMemory = pTempMem;
				retflag = true;
			}
		}

		if (retflag)
		{
			char* pTemp = (char*)m_pMemory + m_iMemoryPosition;
			memcpy(pTemp, pBuf, iCount);
			m_iMemoryPosition = m_iMemoryPosition + iCount;
		}
		return retflag;
	}

	bool _TBufferStream::Reset(int iUsedLength)
	{
		bool retflag = (iUsedLength > 0) && (iUsedLength <= m_iMemoryPosition);
		if (retflag)
		{
			m_iMemoryPosition = m_iMemoryPosition - iUsedLength;
			if (m_iMemoryPosition > 0)
			{
				//buffer后面数据向前移动
				char* pTemp = (char*)m_pMemory + iUsedLength;
				//注意：这里不存在内存重叠的问题，也无buffer溢出的情况，所以只是用memcpy
				memcpy(m_pMemory, pTemp, m_iMemoryPosition);
			}
		}
		return retflag;
	}

	void* _TBufferStream::GetMemPoint()
	{
		return m_pMemory;
	}

	int _TBufferStream::GetPosition()
	{
		return m_iMemoryPosition;
	}
	/************************End Of _TBufferStream******************************************/


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

	std::string GetFileVersion(const std::string &sFileName)
	{
		unsigned long ulDummy;
		std::string sVersion("");
		unsigned long ulVerInfoSize = GetFileVersionInfoSize(sFileName.c_str(), &ulDummy);
		if (0 == ulVerInfoSize)
			return sVersion;

		char* pVerInfo = (char*)malloc(ulVerInfoSize);
		if (nullptr == pVerInfo)
			return sVersion;

		GetFileVersionInfo(sFileName.c_str(), 0, ulVerInfoSize, pVerInfo);
		VS_FIXEDFILEINFO* pFixedFileInfo = nullptr;
		if (0 == VerQueryValue(pVerInfo, "\\", (LPVOID*)&pFixedFileInfo, (PUINT)&ulVerInfoSize))
			return sVersion;

		if (pFixedFileInfo != nullptr)
		{
			int v1 = pFixedFileInfo->dwFileVersionMS >> 16;
			int v2 = pFixedFileInfo->dwFileVersionMS & 0xFFFF;
			int v3 = pFixedFileInfo->dwFileVersionLS >> 16;
			int v4 = pFixedFileInfo->dwFileVersionLS & 0xFFFF;
			sVersion = "V" + std::to_string(v1) + "." + std::to_string(v2) + "." + std::to_string(v3) + "." + std::to_string(v4);
		}
		free(pVerInfo);
		return sVersion;
	}

	void SplitStr(const std::string& s, const std::string& delim, std::vector<std::string>* ret)
	{
		size_t last = 0;   
		size_t index = s.find_first_of(delim, last);  
		while (index != std::string::npos)  
		{ 
			ret->push_back(s.substr(last, index - last));   
			last = index + 1;    
			index = s.find_first_of(delim, last); 
		}   
		if (index - last>0)  
		{
			ret->push_back(s.substr(last, index - last));
		}
	}

	int StrToIntDef(const std::string& sTemp, const int iDef)
	{
		int iRetValue;
		try
		{
			iRetValue = std::stoi(sTemp);
		}
		catch (...)
		{
			iRetValue = iDef;
		}
		return iRetValue;
	}
}