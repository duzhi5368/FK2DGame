/**
*	created:		2012-6-29   14:35
*	filename: 		DataPacker
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		数据打包器(数据的长度不能大于2048), 用来组合和打包数据
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define PACKER_BUF_SIZE 2048
//------------------------------------------------------------------------
// 数据打包器
class CDataPacker
{
public:
	CDataPacker(void)
	: m_nDataLen(0)
	{
		memset(m_buf, 0, PACKER_BUF_SIZE);
	}

	virtual ~CDataPacker(void)
	{
	}

	//-------------------------------------------------------------------
	// 描  述：添加数据
	// 参  数：[输入] const void * pData - 要添加的数据
	// 参  数：[输入] unsigned int nLen - 要添加的数据的长度
	// 返回值：BOOL - 可以添加返回TRUE,否则返回FALSE
	//-------------------------------------------------------------------
	BOOL AddData(const void * pData, unsigned int nLen)
	{
		if (m_nDataLen + nLen > PACKER_BUF_SIZE)
		{
			return FALSE;
		}

		memcpy(m_buf + m_nDataLen, pData, nLen);
		m_nDataLen += nLen;

		return TRUE;
	}

	//-------------------------------------------------------------------
	// 描  述：添加一个BYTE数据
	// 参  数：[输入] BYTE nData - 数据
	// 返回值：BOOL - 可以添加返回TRUE,否则返回FALSE
	//-------------------------------------------------------------------
	BOOL AddBYTE(BYTE nData)
	{
		return AddData(&nData, sizeof(BYTE));
	}

	//-------------------------------------------------------------------
	// 描  述：添加一个WORD数据
	// 参  数：[输入] WORD wData - 数据
	// 返回值：BOOL - 可以添加返回TRUE,否则返回FALSE
	//-------------------------------------------------------------------
	BOOL AddWORD(WORD wData)
	{
		return AddData(&wData, sizeof(WORD));
	}

	//-------------------------------------------------------------------
	// 描  述：添加一个DWORD数据
	// 参  数：[输入] DWORD dwData - 数据
	// 返回值：BOOL - 可以添加返回TRUE,否则返回FALSE
	//-------------------------------------------------------------------
	BOOL AddDWORD(DWORD dwData)
	{
		return AddData(&dwData, sizeof(DWORD));
	}

	//-------------------------------------------------------------------
	// 描  述：获取缓存区 
	//-------------------------------------------------------------------
	const char * GetBuffer(void) const
	{
		return m_buf;
	}

	//-------------------------------------------------------------------
	// 描  述：获取数据的长度 
	//-------------------------------------------------------------------
	int GetDataLen(void) const
	{
		return m_nDataLen;
	}

	void SeekTo(int nPtr)
	{
		if( nPtr < 0 || nPtr >PACKER_BUF_SIZE)
		{
			return;
		}
		m_nDataLen = nPtr;
	}

private:
	char m_buf[PACKER_BUF_SIZE];		// 缓存区
	int m_nDataLen;						// 已添加数据的长度
};
//------------------------------------------------------------------------