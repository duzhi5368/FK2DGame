/**
*	created:		2012-6-28   9:28
*	filename: 		IDataArchive
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		请求对象保存或读取数据
	说    明：  	类似于读取写文件，执行Write()或Read操作以后，缓冲指针相应会后移

	使    用：		CArchive需要外部提供缓冲区，传入一个局部buffer变量，如:
					char buf[1024];
					CDataArchive archive(buf,1024);
					archive.Write("hello",5);
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <windows.h>
//------------------------------------------------------------------------
enum
{
	sWrite/*存盘*/ = TRUE,
	sRead/*读盘*/ = FALSE
};
//------------------------------------------------------------------------
struct IArchive
{
	// 请求写入缓冲。nLen < 4K
	// 这里有个假定：对象必须按顺序写入自身的数据，因为CMapContainer保存对象时
	virtual BOOL Write(const void* buf,int nLen) = NULL;
	
	// 请求读出指定数量的数据
	virtual BOOL Read(void* buf,int nLen) = NULL;
	
	// 获取当前缓冲指针所指的数据地址，适合直接操作缓冲区
	virtual void* GetBuffer() = NULL;
	
	// 把当前指针向后移动nOffset字节，返回当前指针
	// 如果操作失败，Seek返回-1
	virtual int Seek(int nOffset = 0) = NULL;
	
	// 将指针移动指定位置
	virtual int SeekTo(int nPtr = 0) = NULL;
	
	// 获得当前缓冲指针偏移
	virtual int GetBufferOffset() = NULL;
	
	// 读取数据时，获取剩余数据长度，写数据时，返回有效缓冲区长度
	virtual int GetLeftBufferLen() = NULL;
};
//------------------------------------------------------------------------
class CDataArchive : public IArchive  
{
protected:
	// 这个构造仅用于被继承
	CDataArchive(void)
	{
		m_nOffset = 0;
		m_pBindBuffer = NULL;
		m_nMaxSize = 0;
	}

	void Init(void* szBindBuffer, int nMaxSize)
	{
		if (NULL == szBindBuffer || nMaxSize <= 0)
		{
			return;
		}

		m_nOffset = 0;
		m_pBindBuffer = szBindBuffer;
		m_nMaxSize = nMaxSize;
	}
	
public:
	CDataArchive(void* szBindBuffer, int nMaxSize)
	{
		m_nOffset = 0;
		m_pBindBuffer = NULL;
		m_nMaxSize = 0;

		Init(szBindBuffer, nMaxSize);
	}

	virtual ~CDataArchive(void)
	{

	};
	
	// 请求写入缓冲。nLen < 4K
	virtual BOOL Write(const void* buf, int nLen)
	{
		if(buf == NULL || nLen <= 0 || m_nOffset + nLen > m_nMaxSize)
			return FALSE;

		memcpy((char*)m_pBindBuffer + m_nOffset,buf,nLen);
		m_nOffset += nLen;
		return TRUE;
	}
	
	// 请求读出批定数量的数据
	virtual BOOL Read(void* buf, int nLen)
	{
		if(buf == NULL || nLen <= 0 || m_nOffset + nLen > m_nMaxSize)
			return FALSE;

		memcpy(buf,(char*)m_pBindBuffer + m_nOffset,nLen);
		m_nOffset += nLen;
		return TRUE;
	}
	
	// 获取当前缓冲指针所指的数据地址，适合直接操作缓冲区
	virtual void* GetBuffer()
	{
		return (char*)m_pBindBuffer + m_nOffset;
	}
	
	// 把当前指针向后移动nOffset字节，返回移动前的指针
	// 设置nOffSet即可获得当前缓冲指针偏移
	// 如果操作失败，Seek返回-1
	virtual int Seek(int nOffset)
	{
		if(m_nOffset + nOffset > m_nMaxSize || m_nOffset + nOffset < 0)
			return -1;
		int nOld = m_nOffset;
		m_nOffset += nOffset;
		return nOld;
	}
	
	// 同上，把缓冲指针移动到指定位置，返回移动前的指针
	virtual int SeekTo(int nPtr = 0)
	{
		if(nPtr > m_nMaxSize || nPtr < 0)
			return -1;
		int nOld = m_nOffset;
		m_nOffset = nPtr;
		return nOld;
	}
	
	// 获得当前缓冲指针偏移
	virtual int GetBufferOffset(void)
	{
		return m_nOffset;
	}

	// 读取数据时，获取剩余数据长度，写数据时，返回有效缓冲区长度
	virtual int GetLeftBufferLen(void)
	{
		return m_nMaxSize - m_nOffset;
	}
	
private:
	int		m_nOffset;
	void* 	m_pBindBuffer;
	int		m_nMaxSize;
};
//------------------------------------------------------------------------