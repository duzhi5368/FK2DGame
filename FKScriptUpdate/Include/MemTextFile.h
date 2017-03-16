/**
*	created:		2012-7-1   0:05
*	filename: 		MemTextFile
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		内存文本文件(具有加解密功能)
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <stdio.h>
#include <windows.h>
//------------------------------------------------------------------------
class CMemTextFile  
{
protected:
	char*	m_pBuffer;		// 内存文件缓存，保存了解密后的文本文件内容
	int		m_nBufSize;		// 缓存尺寸
	int		m_nFileSize;	// 文件尺寸
	char*	m_pPos;			// 文件指针位置，用于GetLine的操作
public:
	CMemTextFile() : m_pBuffer(NULL), m_nBufSize(0), m_nFileSize(0), m_pPos(NULL) {}
	~CMemTextFile();

	BOOL IsEnd()							{return m_pPos - m_pBuffer >= m_nFileSize;}
	BOOL Open(LPCSTR szFileName);
	BOOL Save(LPCSTR szFileName);
	char* GetLine(char* pString, int nSize);
	BOOL MakeSimpleMap(BYTE* buf, int nLen);
};
//------------------------------------------------------------------------
