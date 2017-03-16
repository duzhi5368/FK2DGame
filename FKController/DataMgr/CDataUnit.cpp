/**
*	created:		2012-6-24   20:12
*	filename: 		CDataUnit
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "CDataUnit.h"
//------------------------------------------------------------------------
void CDataUnit::init()
{
	m_nKey = 0;
	m_nType = DU_NUMBER;
	memset(&m_Data, 0, sizeof(m_Data));
}
//------------------------------------------------------------------------
// 构造函数
CDataUnit::CDataUnit()
{
	init();
}
//------------------------------------------------------------------------
// 构造函数
CDataUnit::CDataUnit(CDataUnit &DataUnit)
{
	init();
	Set(DataUnit);
}
//------------------------------------------------------------------------
// 构造函数
CDataUnit::CDataUnit(int nKey, int nNumber)
{
	init();
	Set(nKey, nNumber);
}
//------------------------------------------------------------------------
// 构造函数
CDataUnit::CDataUnit(int nKey, char *szString)
{
	init();
	Set(nKey, szString);
}
//------------------------------------------------------------------------
// 构造函数
CDataUnit::CDataUnit(int nKey, int nType, char *pBuf)
{
	init();
	Set(nKey, nType, pBuf);
}
//------------------------------------------------------------------------
// 析构函数
CDataUnit::~CDataUnit()
{
	Clear();
}
//------------------------------------------------------------------------
// 设置数据
void CDataUnit::Set(CDataUnit &DataUnit)
{
	Clear();

	memcpy(this, &DataUnit, sizeof(*this));

	if (DU_STRING == m_nType)
	{
		int	nLen = DataUnit.GetLength();

		m_Data.szString = new char[nLen];
		if (m_Data.szString != NULL)
		{
			lstrcpyn(m_Data.szString, (char*)DataUnit, nLen);
		}
		else
		{
			Clear();
		}
	}
}
//------------------------------------------------------------------------
// 设置数据
void CDataUnit::Set(int nKey, int nNumber)
{
	Clear();

	m_nKey = nKey;
	m_nType = DU_NUMBER;
	m_Data.nNumber = nNumber;
}
//------------------------------------------------------------------------
// 设置数据
void CDataUnit::Set(int nKey, char *szString)
{
	Clear();

	if (NULL == szString)
	{
		return;
	}

	m_nKey = nKey;
	m_nType = DU_STRING;

	int	nLen = strlen(szString) + 1;

	m_Data.szString = new char[nLen];
	if (m_Data.szString != NULL)
	{
		lstrcpyn(m_Data.szString, szString, nLen);
	}
	else
	{
		Clear();
	}
}
//------------------------------------------------------------------------
// 设置数据
void CDataUnit::Set(int nKey, char *szString, int nLen)
{
	Clear();

	if(NULL == szString || nLen <= 0)
	{
		return;
	}

	m_nKey = nKey;
	m_nType = DU_STRING;

	m_Data.szString = new char[nLen];

	lstrcpyn(m_Data.szString, szString, nLen);
}
//------------------------------------------------------------------------
// 设置数据
void CDataUnit::Set(int nKey, int nType, char *pBuf)
{
	switch(nType)
	{
	case DU_NUMBER:
		Set(nKey, *((int*)pBuf));
		break;

	case DU_STRING:
		Set(nKey, pBuf);
		break;

	default:
		break;
	}
}
//------------------------------------------------------------------------
// 清除数据
void CDataUnit::Clear(void)
{
	if (DU_STRING == m_nType)
	{
		if (m_Data.szString != NULL)
		{
			delete[] m_Data.szString;
			m_Data.szString = NULL;
		}
	}

	m_nKey = 0;
	m_nType = DU_NUMBER;
	memset(&m_Data, 0, sizeof(m_Data));
}
//------------------------------------------------------------------------
// int强制转换重载
CDataUnit::operator int()
{
	if (DU_NUMBER == m_nType)
	{
		return m_Data.nNumber;
	}

	return atoi(m_Data.szString);
}
//------------------------------------------------------------------------
// char*强制转换重载
CDataUnit::operator char*()
{
	if (DU_STRING == m_nType)
	{
		return m_Data.szString;
	}

	return "";
}
//------------------------------------------------------------------------
// 获取Key值
int CDataUnit::GetKey(void)
{
	return m_nKey;
}
//------------------------------------------------------------------------
// 获取类型
int CDataUnit::GetType(void)
{
	return m_nType;
}
//------------------------------------------------------------------------
// 获取数据指针
char* CDataUnit::GetBuffer(void)
{
	if (DU_NUMBER == m_nType)
	{
		return (char*)&m_Data;
	}

	return m_Data.szString;
}
//------------------------------------------------------------------------
// 获取数据长度
int CDataUnit::GetLength(void)
{
	if (DU_NUMBER == m_nType)
	{
		return 4;
	}

	return strlen(m_Data.szString) + 1;
}
//------------------------------------------------------------------------
// 数据是否为空
BOOL CDataUnit::IsEmpty(void)
{
	if (DU_NUMBER == m_nType)
	{
		if (0 == m_Data.nNumber)
		{
			return TRUE;
		}
	}
	else
	{
		if (strlen(m_Data.szString) <= 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}
//------------------------------------------------------------------------