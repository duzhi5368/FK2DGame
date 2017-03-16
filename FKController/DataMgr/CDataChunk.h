/**
*	created:		2012-6-24   20:20
*	filename: 		CDataChunk
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		DataChunk数据结构头文件
					具体结构如下：
					总数据长度(4字节) + N个数据块
					数据块结构如下：
					4字节Key值 + 1字节类型 + 数据
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <list>
using std::list;
//------------------------------------------------------------------------
class CDataUnit;
//------------------------------------------------------------------------
// DataChunk类
class CDataChunk
{
public:
	// 构造函数
	CDataChunk();
	// 构造函数
	CDataChunk(int nMaxBufLen);
	// 析构函数
	~CDataChunk();

	// 设置DataChunk的最大数据内容长度
	BOOL SetMaxBufferLen(int nMaxBufLen);
	// 获取DataChunk的最大数据内容长度
	int GetMaxBufferLen(void);

	// 设置DataChunk的数据内容
	BOOL SetBuffer(char *pBuf, int nLen);
	// 获取DataChunk的数据内容
	BOOL GetBuffer(char *pBuf, int nLen);
	// 获取DataChunk的数据内容长度
	int GetBufferLen(void);
	// 清除DataChunk的数据内容
	void Clear(void);

	// 设置一个数据
	BOOL SetValue(CDataUnit &DataUnit);
	// 获取一个数据
	CDataUnit* GetValue(int nKey);
	// 删除一个数据
	void DelValue(int nKey);
	// 获取数据个数
	int  GetValueNum(void);

	// 重设迭代器至chunk首部
	void ResetBegin(void);
	// 在迭代器逐个返回数据指针
	CDataUnit* GetLoop(void);

private:
	int							m_nMaxBufLen;	// 最大数据内容长度
	int							m_nCurBufLen;	// 当前数据内容长度
	list<CDataUnit*>			m_DataList;		// 数据List表
	list<CDataUnit*>::iterator	m_itLoop;		// 用于遍历取数据的中间迭代器

};
//------------------------------------------------------------------------