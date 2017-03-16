/**
*	created:		2012-6-24   20:11
*	filename: 		CDataUnit
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		DataUnit数据结构头文件
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
// DataUnit数据类型定义
enum
{
	DU_NUMBER = 0,	// 数字类型
	DU_STRING,		// 字符串类型
};
//------------------------------------------------------------------------
// DataUnit数据联合类型
union UDataUnit
{
	int		nNumber;	// 数字
	char	*szString;	// 字符串指针
};
//------------------------------------------------------------------------
// DataUnit类
class CDataUnit
{
private:
	void init();

public:
	// 构造函数
	CDataUnit();
	// 构造函数
	CDataUnit(CDataUnit &DataUnit);
	// 构造函数
	CDataUnit(int nKey, int nNumber);
	// 构造函数
	CDataUnit(int nKey, char *szString);
	// 构造函数
	CDataUnit(int nKey, int nType, char *pBuf);
	// 析构函数
	~CDataUnit();

	// 设置数据
	void Set(CDataUnit &DataUnit);
	// 设置数据
	void Set(int nKey, int nNumber);
	// 设置数据
	void Set(int nKey, char *szString);
	// 设置数据
	void Set(int nKey, char *szString, int nLen);
	// 设置数据
	void Set(int nKey, int nType, char *pBuf);
	// 清除数据
	void Clear(void);

	// int强制转换重载
	operator int();
	// char*强制转换重载
	operator char*();

	// 获取Key值
	int GetKey(void);
	// 获取类型
	int GetType(void);
	// 获取数据指针
	char* GetBuffer(void);
	// 获取数据长度
	int GetLength(void);
	// 数据是否为空
	BOOL IsEmpty(void);

private:
	int			m_nKey;		// Key值
	int			m_nType;	// 类型
	UDataUnit	m_Data;		// 数据

};
//------------------------------------------------------------------------