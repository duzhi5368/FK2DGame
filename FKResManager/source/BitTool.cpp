/**
*	created:		2012-6-18   20:48
*	filename: 		BitTool
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/ResMgrCommon.h"
#include "../Include/BitTool.h"
//------------------------------------------------------------------------
CBitTool::CBitTool()
{
	memset(m_Data, 0 ,sizeof(m_Data));
	m_nBitPos = 0;
}
//------------------------------------------------------------------------
CBitTool::~CBitTool()
{
}
//------------------------------------------------------------------------
BOOL CBitTool::Push(int nType, void* pData, int nBitsCount)
{
	int nValue = *(int*)pData;				// 获得值
	if (nBitsCount < 32 && (nValue>>nBitsCount) > 0)
	{
		FKResTraceColorLn(MY_TRACE_COLOR, 
			"错误：BitTool::Push() 出现异常：数据大小 %d 过长，而存储字节 %d 过小。", nValue, nBitsCount);
		return FALSE;
	}

	int nMask = 1<<(nBitsCount - 1);
	nMask=(nMask<<2)-1;
	nValue &= nMask;

	for (int bits=1, count=0; count<nBitsCount ;bits<<=1, count++, m_nBitPos++) 
	{
		if (m_nBitPos>=MAX_DATA_BUFFER*8)
		{
			FKResTraceColorLn(MY_TRACE_COLOR,
				"ERROR: BitTool::Push() catch error: buffer overflow!");
			return FALSE;
		}

		int byte_pos = m_nBitPos>>3;
		int bit_pos = m_nBitPos&7;
		int value = 1<<bit_pos;

		if (bits&nValue)
			m_Data[byte_pos] |= value;				
		else
			m_Data[byte_pos] &= ~value;
	}

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CBitTool::PushFloat(float* f)
{
	int byte_pos = GetBytes(m_nBitPos);
	memcpy(m_Data + byte_pos, f, sizeof(float));
	m_nBitPos += 8*sizeof(float);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CBitTool::PushString(LPCSTR szString)
{
	Assert(szString);
	int byte_pos = GetBytes(m_nBitPos);
	int nLen = lstrlen(szString);
	memcpy(m_Data + byte_pos, szString, nLen);
	m_nBitPos += (nLen + 1)*8;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CBitTool::PushArray(int nType, void* pArray, int nSize)
{
	int byte_pos = GetBytes(m_nBitPos);
	if (byte_pos + nSize*GetTypeBytes(nType) >= MAX_DATA_BUFFER)
	{
		FKResTraceColorLn(MY_TRACE_COLOR, 
			"ERROR: BitTool::PushArray() catch error: buffer overflow!");
		return FALSE;
	}

	switch (nType)
	{
	case dt_bool:
	case dt_char:
	case dt_byte:
		{
			BYTE* lpArray = (BYTE*)pArray;
			BYTE* pData = new BYTE[nSize];
			Assert(pData);
			for (int i=0;i<nSize;i++)
				pData[i] = (BYTE)(lpArray[i]);
			memcpy(m_Data + byte_pos, pData, nSize*sizeof(BYTE));
			delete[] pData;
			m_nBitPos += 8*nSize*sizeof(BYTE);
		}
		break;
	case dt_short:
	case dt_word:
		{
			long* lpArray = (long*)pArray;
			WORD* pData = new WORD[nSize];
			for (int i=0;i<nSize;i++)
				pData[i] = (WORD)(lpArray[i]);
			memcpy(m_Data + byte_pos, pData, nSize*sizeof(WORD));
			m_nBitPos += 8*nSize*sizeof(WORD);
			delete[] pData;
		}
		break;
	case dt_int:
	case dt_dword:
		memcpy(m_Data + byte_pos, pArray, nSize*sizeof(DWORD));
		m_nBitPos += 8*nSize*sizeof(DWORD);
		break;
	case dt_float:
		{
			double* lpArray = (double*)pArray;
			float* pData = new float[nSize];
			Assert(pData);
			for (int i=0;i<nSize;i++)
				pData[i] = (float)(lpArray[i]);
			memcpy(m_Data + byte_pos, pData, nSize*sizeof(float));
			m_nBitPos += 8*nSize*sizeof(float);
			delete[] pData;
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
int CBitTool::GetBytes(int nBitsCount)
{
	int nBytes = 0;
	nBytes = nBitsCount >> 3;
	if (nBitsCount%8)
		nBytes ++;
	return nBytes;
}
//------------------------------------------------------------------------
int CBitTool::GetDataLength()
{
	return GetBytes(m_nBitPos);
}
//------------------------------------------------------------------------
void* CBitTool::GetDataBuffer()
{
	return m_Data;
}
//------------------------------------------------------------------------
void CBitTool::Reset()
{
	memset(m_Data, 0 ,sizeof(m_Data));
	m_nBitPos = 0;
}
//------------------------------------------------------------------------
int CBitTool::GetTypeBytes(int nType)
{
	switch (nType)
	{
	case dt_bool:
	case dt_char:
	case dt_byte:
		return 1;
	case dt_short:
	case dt_word:
		return 2;
	case dt_int:
	case dt_dword:
	case dt_float:
		return 4;
	}
	return 0;
}
//------------------------------------------------------------------------