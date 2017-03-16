/**
*	created:		2012-6-24   20:16
*	filename: 		CDataStream
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "CDataUnit.h"
#include "CDataStream.h"
//------------------------------------------------------------------------
CDataStreamBuild::CDataStreamBuild(void)
{
	m_nOffset = 0;
	m_pBindBuffer = NULL;
	m_nMaxSize = 0;
	m_nTotalDataSize = 0;
}
//------------------------------------------------------------------------
void CDataStreamBuild::Init(void* szBindBuffer, int nMaxSize)
{
	if (NULL == szBindBuffer || nMaxSize < 4)
	{
		return;
	}

	// 要偏移四个字节, 用来存储总数据长度
	m_nOffset = sizeof(int);

	// 当前缓冲区指针
	m_pBindBuffer = szBindBuffer;
	ZeroMemory(m_pBindBuffer, nMaxSize);

	// 填入长度
	m_nTotalDataSize = 0;
	memcpy(m_pBindBuffer, &m_nTotalDataSize, sizeof(int));

	// 缓冲区最大长度
	m_nMaxSize = nMaxSize;
}
//------------------------------------------------------------------------
CDataStreamBuild::CDataStreamBuild(void* szBindBuffer, int nMaxSize)
{
	m_nOffset = 0;
	m_pBindBuffer = NULL;
	m_nMaxSize = 0;
	m_nTotalDataSize = 0;

	Init(szBindBuffer, nMaxSize);
}
//------------------------------------------------------------------------
CDataStreamBuild::~CDataStreamBuild()
{

}
//------------------------------------------------------------------------
// 请求写入缓冲。nLen < 4K
// 这里有个假定：对象必须按顺序写入自身的数据，因为CMapContainer保存对象时
BOOL CDataStreamBuild::Write(const void* buf, int nLen)
{
	if (NULL == buf || nLen <= 0)
	{
		return FALSE;
	}

	if((m_nOffset + nLen + sizeof(int)) > m_nMaxSize)
	{
		return FALSE;
	}

	// 写入数据块的长度
	memcpy((char*)m_pBindBuffer + m_nOffset, &nLen, sizeof(int));
	m_nOffset += sizeof(int);

	// 写入数据块
	memcpy((char*)m_pBindBuffer + m_nOffset, buf, nLen);
	m_nOffset += nLen;

	m_nTotalDataSize += (sizeof(int) + nLen);
	memcpy(m_pBindBuffer, &m_nTotalDataSize, sizeof(int));

	return TRUE;
}
//------------------------------------------------------------------------
// 请求写入DataUnit
BOOL CDataStreamBuild::Write(CDataUnit &DataUnit)
{
	char	szBuf[1024];
	int		nKey = DataUnit.GetKey();

	memcpy(szBuf, &nKey, sizeof(int));
	memcpy(szBuf + sizeof(int), DataUnit.GetBuffer(), DataUnit.GetLength());

	return Write(szBuf, DataUnit.GetLength() + sizeof(int));
}
//------------------------------------------------------------------------
// 获取当前缓冲指针所指的数据地址，适合直接操作缓冲区
void * CDataStreamBuild::GetBuffer()
{
	return (char*)m_pBindBuffer + m_nOffset;
}
//------------------------------------------------------------------------
// 获得当前缓冲指针偏移
int CDataStreamBuild::GetBufferOffset()
{
	return m_nOffset;
}
//------------------------------------------------------------------------
// 读取数据时，获取剩余数据长度，写数据时，返回有效缓冲区长度
int CDataStreamBuild::GetLeftBufferLen()
{
	return m_nMaxSize - m_nOffset;
}
//------------------------------------------------------------------------
// 取得Buffer的数据长度
int CDataStreamBuild::GetTotalDataLen(void)
{
	return m_nTotalDataSize;
}
//------------------------------------------------------------------------
CDataStreamParse::CDataStreamParse(void)
{
	m_pParseBuffer = NULL;
	m_nOffset = 0;
	m_nSize = 0;
	m_nTotalDataSize = 0;
}
//------------------------------------------------------------------------
CDataStreamParse::CDataStreamParse(void * szParseBuffer, int nSize)
{
	m_pParseBuffer = NULL;
	m_nOffset = 0;
	m_nSize = 0;
	m_nTotalDataSize = 0;

	Load(szParseBuffer, nSize);
}
//------------------------------------------------------------------------
CDataStreamParse::~CDataStreamParse(void)
{

}
//------------------------------------------------------------------------
// 载入要解析的BUFF
BOOL CDataStreamParse::Load(void * szParseBuffer, int nSize)
{
	if(NULL == szParseBuffer || nSize < sizeof(int))
	{
		return FALSE;
	}

	int nTotalDataSize = *((int *)szParseBuffer);
	if (nSize != nTotalDataSize + 4)
	{
		return FALSE;
	}

	m_pParseBuffer = szParseBuffer;
	m_nSize = nSize;
	m_nOffset = sizeof(int);
	m_nTotalDataSize = nTotalDataSize;

	return TRUE;
}
//------------------------------------------------------------------------
// 获取当前缓冲指针所指的数据地址，适合直接操作缓冲区
void * CDataStreamParse::GetBuffer(void)
{
	return (char*)m_pParseBuffer + m_nOffset;
}
//------------------------------------------------------------------------
// 从当前缓冲指针处获得chunk
BOOL CDataStreamParse::GetChunk(void* buf, int &nLen)
{
	if ((m_nOffset + sizeof(int)) > m_nSize)
	{
		return FALSE;
	}

	int nDataLen = *((int *)((char*)m_pParseBuffer + m_nOffset));
	m_nOffset += 4;
	if(nDataLen > nLen)
	{
		return FALSE;
	}

	if((m_nOffset + nDataLen) > m_nSize)
	{
		return FALSE;
	}

	memcpy(buf, (char*)m_pParseBuffer + m_nOffset, nDataLen);
	m_nOffset += nDataLen;
	nLen = nDataLen;

	return TRUE;
}
//------------------------------------------------------------------------
// 移到数据区首部
int CDataStreamParse::Reset(void)
{
	int nOld = m_nOffset;
	m_nOffset = sizeof(int);

	return nOld;
}
//------------------------------------------------------------------------
// 获得当前缓冲指针偏移
int	CDataStreamParse::GetBufferOffset(void)
{
	return m_nOffset;
}
//------------------------------------------------------------------------
// 获取剩余数据长度
int	CDataStreamParse::GetLeftBufferLen(void)
{
	return m_nSize - m_nOffset;
}
//------------------------------------------------------------------------
// 取得Buffer的数据长度
int CDataStreamParse::GetTotalDataLen(void)
{
	return m_nTotalDataSize;
}
//------------------------------------------------------------------------
BOOL CDataStreamParse::IsEnd(void)
{
	return (m_nOffset >= m_nSize) ? TRUE : FALSE;
}
//------------------------------------------------------------------------