/**
*	created:		2012-6-16   1:41
*	filename: 		PackFileObject
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../Include/FileSysCommon.h"
#include "../Include/PackFileObject.h"
#include <sys/stat.h>
//------------------------------------------------------------------------
// 构造函数
CPackFileObject::CPackFileObject()
{
	m_pFile = NULL;
	m_dwOffset = 0;
	m_strFileName = "";
	m_dwFileLength = 0;
}
//------------------------------------------------------------------------
// 析构函数
CPackFileObject::~CPackFileObject()
{
	Close();
}
//------------------------------------------------------------------------
BOOL CPackFileObject::Open(LPCSTR lpszMode)
{
	ASSERT(!m_strFileName.empty() && lpszMode != NULL && *lpszMode != 0);
	return SeekToBegin();
}
//------------------------------------------------------------------------
BOOL CPackFileObject::Close()
{
	m_pFile = NULL;
	m_dwOffset = 0;
	m_dwFileLength = 0;
	return TRUE;
}
//------------------------------------------------------------------------
UINT CPackFileObject::Read(void* lpBuf, UINT nCount)
{
	ASSERT(IsOpen());

	if (lpBuf == NULL || nCount == 0)
		return 0;

	if (GetPosition() + nCount > m_dwFileLength) // 越界
		return FALSE;

	return fread(lpBuf, 1, nCount, m_pFile);
}
//------------------------------------------------------------------------
BOOL CPackFileObject::Write(const void* lpBuf, UINT nCount)
{
	ASSERT(IsOpen()); 
	
	if (lpBuf == NULL || nCount == 0)
		return TRUE;

	if (GetPosition() + nCount > m_dwFileLength) // 越界
		return FALSE;

	if (fwrite(lpBuf, 1, nCount, m_pFile) != nCount)
		return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------
LPSTR CPackFileObject::ReadString(LPSTR lpsz, UINT nMax)
{
	ASSERT(IsOpen() && lpsz != NULL);

	int pos = GetPosition(); // 读之前的位置
	if (pos >= m_dwFileLength)
	{
		lpsz[0] = '\0';
		return NULL;
	}

	LPSTR lpszResult = fgets(lpsz, nMax, m_pFile);
	if (lpszResult == NULL && !feof(m_pFile)) // fgets()的调用出错
	{
		clearerr(m_pFile);
	}
	if (GetPosition() > m_dwFileLength) // 读最后一行时超出文件尾,剪掉!
	{
		SeekToEnd();
		lpsz[m_dwFileLength - pos] = '\0';
		return lpszResult;
	}
	
	int strlen = lstrlen(lpsz);
	lpsz[strlen - 1] = '\0';
	lpsz[strlen - 2] = '\n';
	return lpszResult;
}
//------------------------------------------------------------------------
BOOL CPackFileObject::WriteString(LPCSTR lpsz)
{
	ASSERT(IsOpen() && lpsz != NULL);

	if (fputs(lpsz, m_pFile) == EOF)
		return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CPackFileObject::Seek(LONG lOff, UINT nFrom /*= SEEK_SET*/)
{
	ASSERT(IsOpen());

	switch(nFrom)
	{
	case SEEK_SET:
		if (fseek(m_pFile, lOff + m_dwOffset + sizeof(BLOCKINFO), SEEK_SET) == 0)
			return TRUE;
		break;
	case SEEK_CUR:
		if (fseek(m_pFile, lOff, SEEK_CUR) == 0)
			return TRUE;
		break;
	case SEEK_END:
		if (fseek(m_pFile, m_dwOffset + sizeof(BLOCKINFO) + m_dwFileLength - lOff, SEEK_SET) == 0)
			return TRUE;
		break;
	}
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CPackFileObject::SeekToBegin()
{
	ASSERT(IsOpen());

	if (fseek(m_pFile, m_dwOffset + sizeof(BLOCKINFO), SEEK_SET) == 0)
		return TRUE;

	return FALSE;
}
//------------------------------------------------------------------------
BOOL CPackFileObject::SeekToEnd()
{
	ASSERT(IsOpen());

	if (fseek(m_pFile, m_dwOffset + sizeof(BLOCKINFO) + m_dwFileLength, SEEK_SET) == 0)
		return TRUE;

	return FALSE;
}
//------------------------------------------------------------------------
BOOL CPackFileObject::Flush()// 包文件不支持
{
	ASSERT(IsOpen());
	return TRUE;
}
//------------------------------------------------------------------------
DWORD CPackFileObject::GetLength()
{	
	return m_dwFileLength;
}
//------------------------------------------------------------------------
BOOL CPackFileObject::SetLength(DWORD dwNewLen)
{
	ASSERT(IsOpen());

	Seek((LONG)dwNewLen, SEEK_SET);

	if (!::SetEndOfFile((HANDLE)m_pFile))
		return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------
int CPackFileObject::GetPosition()
{
	ASSERT(IsOpen());

	int nPos = -1;
	if ((nPos = ftell(m_pFile)) != -1) // success
		return nPos - m_dwOffset - sizeof(BLOCKINFO);

	return nPos;
}
//------------------------------------------------------------------------
LPCSTR CPackFileObject::GetFileName()
{
	int nPos = m_strFileName.find_last_of("\\");
	return &m_strFileName.at(nPos + 1);
}
//------------------------------------------------------------------------
BOOL CPackFileObject::GetFileTitle(char* buf)
{
	ASSERT(buf != NULL);
	
	string strFileName = GetFileName();
	int nPos = strFileName.find_last_of(".");
	strFileName.copy(buf, nPos);
	*(buf + nPos) = 0;
	return TRUE;
}
//------------------------------------------------------------------------
LPCSTR CPackFileObject::GetFilePath()
{
	return m_strFileName.c_str();
}
//------------------------------------------------------------------------
BOOL CPackFileObject::Release()
{
	Close();

	delete this;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CPackFileObject::SetFilePath(LPCSTR lpszNewName)
{
	ASSERT(lpszNewName != NULL && *lpszNewName != 0);

	if (lpszNewName)
		m_strFileName = lpszNewName;
	else
		m_strFileName = "";

	return TRUE;
}
//------------------------------------------------------------------------
// 文件是否打开
BOOL CPackFileObject::IsOpen()
{
	return (m_pFile != NULL);
}
//------------------------------------------------------------------------
// 是否存在
BOOL CPackFileObject::IsExist()
{
	return FALSE;
}
//------------------------------------------------------------------------
// 设置文件句柄、偏移量和文件长度(不含块信息)信息
BOOL CPackFileObject::SetFileInfo(FILE* pFile, DWORD dwOffset, DWORD dwFileLength)
{
	ASSERT(pFile != NULL && dwOffset != 0);

	m_pFile = pFile;
	m_dwOffset = dwOffset;
	m_dwFileLength = dwFileLength;
	SeekToBegin();
	return TRUE;
}
//------------------------------------------------------------------------
// 获得文件句柄
FILE* CPackFileObject::GetHandle()
{
	return m_pFile;
}
//------------------------------------------------------------------------