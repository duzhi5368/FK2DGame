/**
*	created:		2012-6-24   20:21
*	filename: 		CDataChunk
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		DataChunk数据结构实现文件
*/
//------------------------------------------------------------------------
#include "CDataUnit.h"
#include "CDataChunk.h"
//------------------------------------------------------------------------
// 构造函数
CDataChunk::CDataChunk()
{
	m_nMaxBufLen = 4;
	m_nCurBufLen = 4;
	m_DataList.clear();
	m_itLoop = m_DataList.begin();
}
//------------------------------------------------------------------------
// 构造函数
CDataChunk::CDataChunk(int nMaxBufLen)
{
	m_nMaxBufLen = nMaxBufLen;
	m_nCurBufLen = 4;
	m_DataList.clear();
	m_itLoop = m_DataList.begin();
}
//------------------------------------------------------------------------
// 析构函数
CDataChunk::~CDataChunk()
{
	Clear();
}
//------------------------------------------------------------------------
// 设置DataChunk的最大数据内容长度
BOOL CDataChunk::SetMaxBufferLen(int nMaxBufLen)
{
	if (nMaxBufLen <= GetBufferLen())
	{
		return FALSE;
	}

	m_nMaxBufLen = nMaxBufLen;

	return TRUE;
}
//------------------------------------------------------------------------
// 获取DataChunk的最大数据内容长度
int CDataChunk::GetMaxBufferLen(void)
{
	return m_nMaxBufLen;
}
//------------------------------------------------------------------------
// 设置DataChunk的数据内容
BOOL CDataChunk::SetBuffer(char *pBuf, int nLen)
{
	int	nDataChunkLen = *((int*)pBuf);
	if (nDataChunkLen > nLen)
	{
		return FALSE;
	}

	if (nDataChunkLen > m_nMaxBufLen)
	{
		return FALSE;
	}

	Clear();

	char *pCurPos = pBuf + 4;

	while (pCurPos < pBuf + nDataChunkLen)
	{
		CDataUnit DataUnit(*((int*)pCurPos), *(pCurPos + 4), pCurPos + 5);
		if (FALSE == SetValue(DataUnit))
		{
			return FALSE;
		}

		pCurPos += (5 + DataUnit.GetLength());
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 获取DataChunk的数据内容
BOOL CDataChunk::GetBuffer(char *pBuf, int nLen)
{
	int	nBufferLen = GetBufferLen();
	if (NULL == pBuf || nLen < nBufferLen)
	{
		return FALSE;
	}

	*((int*)pBuf) = nBufferLen;
	char *pCurPos = pBuf + 4;

	list<CDataUnit*>::iterator it = m_DataList.begin();
	for (; it != m_DataList.end(); ++it)
	{
		CDataUnit *pDataUnit = (*it);
		*((int*)pCurPos) = pDataUnit->GetKey();
		*(pCurPos + 4) = pDataUnit->GetType();
		memcpy(pCurPos + 5, pDataUnit->GetBuffer(), pDataUnit->GetLength());
		pCurPos += (5 + pDataUnit->GetLength());
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 获取DataChunk的数据内容长度
int CDataChunk::GetBufferLen(void)
{
	return m_nCurBufLen;
}
//------------------------------------------------------------------------
// 清除DataChunk的数据内容
void CDataChunk::Clear(void)
{
	list<CDataUnit*>::iterator it = m_DataList.begin();
	for (; it != m_DataList.end(); ++it)
	{
		delete (*it);
	}

	m_DataList.clear();
	m_nCurBufLen = 4;
}
//------------------------------------------------------------------------
// 设置一个数据
BOOL CDataChunk::SetValue(CDataUnit &DataUnit)
{
	list<CDataUnit*>::iterator it = m_DataList.begin();
	for (; it != m_DataList.end(); ++it)
	{
		CDataUnit *pDataUnit = (*it);
		if (pDataUnit->GetKey() == DataUnit.GetKey())
		{
			int	nOldLen = pDataUnit->GetLength();
			int	nNewLen = DataUnit.GetLength();

			if (nOldLen >= nNewLen || nNewLen - nOldLen + GetBufferLen() <= m_nMaxBufLen)
			{
				pDataUnit->Set(DataUnit);
				m_nCurBufLen += (nNewLen - nOldLen);
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}

	if (5 + DataUnit.GetLength() + GetBufferLen() <= m_nMaxBufLen) // 5 = 4字节Key值 + 1字节类型
	{
		CDataUnit *pDataUnit = new CDataUnit(DataUnit);
		if (NULL == pDataUnit)
		{
			return FALSE;
		}

		m_DataList.push_back(pDataUnit);
		m_nCurBufLen += (5 + pDataUnit->GetLength()); // 5 = 4字节Key值 + 1字节类型
		return TRUE;
	}

	return FALSE;
}
//------------------------------------------------------------------------
// 获取一个数据
CDataUnit* CDataChunk::GetValue(int nKey)
{
	list<CDataUnit*>::iterator it = m_DataList.begin();
	for(; it != m_DataList.end(); ++it)
	{
		CDataUnit *pDataUnit = (*it);
		if (pDataUnit->GetKey() == nKey)
		{
			return pDataUnit;
		}
	}

	return NULL;
}
//------------------------------------------------------------------------
// 删除一个数据
void CDataChunk::DelValue(int nKey)
{
	list<CDataUnit*>::iterator it = m_DataList.begin();
	for(; it != m_DataList.end(); ++it)
	{
		CDataUnit *pDataUnit = (*it);
		if (pDataUnit->GetKey() == nKey)
		{
			m_nCurBufLen -= (5 + pDataUnit->GetLength()); // 5 = 4字节Key值 + 1字节类型
			delete pDataUnit;
			m_DataList.erase(it);
			return;
		}
	}
}
//------------------------------------------------------------------------
// 获取数据个数
int CDataChunk::GetValueNum(void)
{
	return m_DataList.size();
}
//------------------------------------------------------------------------
// 重设迭代器至chunk首部
void CDataChunk::ResetBegin(void)
{
	m_itLoop = m_DataList.begin();
}
//------------------------------------------------------------------------
// 在迭代器逐个弹出数据
CDataUnit* CDataChunk::GetLoop(void)
{
	if (m_itLoop == m_DataList.end())
	{
		return NULL;
	}

	CDataUnit *pDataUnit = (*m_itLoop);
	++m_itLoop;

	return pDataUnit;
}
//------------------------------------------------------------------------