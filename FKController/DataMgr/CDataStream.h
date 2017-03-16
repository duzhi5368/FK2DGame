/**
*	created:		2012-6-24   20:15
*	filename: 		CDataStream
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		数据流读写

					具体结构如下：
					总数据长度(4字节) + N个数据块(数据块长度 + 数据)
					使用：CDataStream需要外部提供缓冲区，传入一个局部buffer变量，如:
					char buf[1024];
					CDataStream datastream(buf,1024);
					datastream.Write("hello",5);
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CDataUnit;
//------------------------------------------------------------------------
class CDataStreamBuild
{
public:
	CDataStreamBuild(void);
	CDataStreamBuild(void * szBindBuffer, int nMaxSize);
	~CDataStreamBuild(void);
public:
	void			Init(void * szBindBuffer, int nMaxSize);	

	// 请求写入缓冲。nLen < 4K
	// 这里有个假定：对象必须按顺序写入自身的数据
	virtual BOOL	Write(const void* buf, int nLen);

	// 请求写入DataUnit
	virtual BOOL	Write(CDataUnit &DataUnit);

	// 获取当前缓冲指针所指的数据地址，适合直接操作缓冲区
	virtual void *	GetBuffer(void);

	// 获得当前缓冲指针偏移
	virtual int		GetBufferOffset(void);

	// 写数据时，返回有效缓冲区长度
	virtual int		GetLeftBufferLen(void);

	// 取得Buffer的数据长度
	virtual int		GetTotalDataLen(void);
private:
	// 当前缓冲区指针偏移
	int				m_nOffset;
	// 当前缓冲区指针
	void * 			m_pBindBuffer;
	// 缓冲区最大长度
	int				m_nMaxSize;
	// 总数据长度
	int				m_nTotalDataSize;
};
//------------------------------------------------------------------------
class CDataStreamParse
{
public:
	CDataStreamParse(void);
	CDataStreamParse(void * szParseBuffer, int nSize);
	~CDataStreamParse(void);
public:
	// 载入要解析的BUFF
	BOOL			Load(void * szParseBuffer, int nSize);

	// 获取当前缓冲指针所指的数据地址，适合直接操作缓冲区
	virtual void *	GetBuffer(void);

	// 从当前缓冲指针处获得chunk
	virtual BOOL	GetChunk(void* buf, int &nLen);
	
	// 移到数据区首部
	virtual int		Reset(void);

	// 获得当前缓冲指针偏移
	virtual int		GetBufferOffset(void);
	
	// 获取剩余数据长度
	virtual int		GetLeftBufferLen(void);	

	// 取得Buffer的数据长度
	virtual int		GetTotalDataLen(void);

	// 是否到了buffer尾
	virtual BOOL	IsEnd(void);
private:
	// 当前缓冲区指针
	void *			m_pParseBuffer;
	// 当前缓冲区指针偏移
	int				m_nOffset;
	// 缓冲区大小
	int				m_nSize;
	// 数据区大小
	int				m_nTotalDataSize;
};
//------------------------------------------------------------------------