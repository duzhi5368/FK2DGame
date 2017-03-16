/**
*	created:		2012-6-18   20:59
*	filename: 		StdFileObject
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../Include/ResMgrCommon.h"
#include "../Include/StdFileObject.h"
#include <sys/stat.h>
//------------------------------------------------------------------------
CStdFileObject::CStdFileObject()
{
	m_pFile = NULL;
	m_strFileName = "";
}
//------------------------------------------------------------------------
CStdFileObject::~CStdFileObject()
{
	Close();
}
//------------------------------------------------------------------------
BOOL CStdFileObject::Open(LPCSTR lpszMode)
{
	ASSERT(!m_strFileName.empty() && lpszMode != NULL && *lpszMode != 0);
	
	if (IsOpen())
		return TRUE;

	m_pFile = fopen(m_strFileName.c_str(), lpszMode);
	if (m_pFile == NULL)
	{
		TraceIOError(GetLastError());
		return FALSE;
	}

	return m_pFile != NULL;
}
//------------------------------------------------------------------------
BOOL CStdFileObject::Close()
{
	if (m_pFile != NULL)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	return TRUE;
}
//------------------------------------------------------------------------
UINT CStdFileObject::Read(void* lpBuf, UINT nCount)
{
	ASSERT(IsOpen()); 
	if (lpBuf == NULL || nCount == 0)	
		return 0;

	return fread(lpBuf, 1, nCount, m_pFile);
}
//------------------------------------------------------------------------
BOOL CStdFileObject::Write(const void* lpBuf, UINT nCount)
{
	ASSERT(IsOpen());

	if (lpBuf == NULL || nCount == 0)	
		return 0;

	return fwrite(lpBuf, 1, nCount, m_pFile) == nCount;
}
//------------------------------------------------------------------------
LPSTR CStdFileObject::ReadString(LPSTR lpsz, UINT nMax)
{
	ASSERT(IsOpen() && lpsz != NULL);

	LPSTR lpszResult = fgets(lpsz, nMax, m_pFile);
	if (lpszResult == NULL && !feof(m_pFile))
	{
		clearerr(m_pFile);
	}
	return lpszResult;
}
//------------------------------------------------------------------------
BOOL CStdFileObject::WriteString(LPCSTR lpsz)
{
	ASSERT(IsOpen() && lpsz != NULL);

	if (fputs(lpsz, m_pFile) == EOF)
		return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CStdFileObject::Seek(LONG lOff, UINT nFrom /*= SEEK_SET*/)
{
	ASSERT(IsOpen());

	if (fseek(m_pFile, lOff, nFrom) == 0)
		return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CStdFileObject::SeekToBegin()
{
	ASSERT(IsOpen());

	if (fseek(m_pFile, 0, SEEK_SET) == 0)
		return TRUE;

	return FALSE;
}
//------------------------------------------------------------------------
BOOL CStdFileObject::SeekToEnd()
{
	ASSERT(IsOpen());

	if (fseek(m_pFile, 0, SEEK_END) == 0)
		return TRUE;

	return FALSE;
}
//------------------------------------------------------------------------
BOOL CStdFileObject::Flush()
{
	ASSERT(IsOpen());

	fflush(m_pFile);
	return TRUE;
}
//------------------------------------------------------------------------
DWORD CStdFileObject::GetLength()
{
	if (IsOpen())
	{
		DWORD dwCur = GetPosition();
		SeekToEnd();
		DWORD dwLen = GetPosition();
		Seek(dwCur, SEEK_SET);
		return dwLen;
	}

	struct _stat statbuf;
	_stat(m_strFileName.c_str(), &statbuf);
	return statbuf.st_size;
}
//------------------------------------------------------------------------
BOOL CStdFileObject::SetLength(DWORD dwNewLen)
{
	ASSERT(IsOpen());

	Seek((LONG)dwNewLen, SEEK_SET);

	if (!::SetEndOfFile((HANDLE)m_pFile))
		return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------
int CStdFileObject::GetPosition()
{
	ASSERT(IsOpen());

	int nPos = -1;
	if ((nPos = ftell(m_pFile)) != -1) 
		return nPos;
	return nPos;
}
//------------------------------------------------------------------------
LPCSTR CStdFileObject::GetFileName()
{
	int nPos = m_strFileName.find_last_of("\\");
	return &m_strFileName.at(nPos + 1);
}
//------------------------------------------------------------------------
BOOL CStdFileObject::GetFileTitle(char* buf)
{
	ASSERT(buf != NULL);
	
	string strFileName = GetFileName();
	int nPos = strFileName.find_last_of(".");
	strFileName.copy(buf, nPos);
	*(buf + nPos) = 0;
	return TRUE;
}
//------------------------------------------------------------------------
LPCSTR CStdFileObject::GetFilePath()
{
	return m_strFileName.c_str();
}
//------------------------------------------------------------------------
BOOL CStdFileObject::Release()
{
	Close();

	delete this;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CStdFileObject::SetFilePath(LPCSTR lpszNewName)
{
	ASSERT(lpszNewName != NULL && *lpszNewName != 0);

	if (lpszNewName)
		m_strFileName = lpszNewName;
	else
		m_strFileName = "";

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CStdFileObject::IsOpen()
{
	return (m_pFile != NULL);
}
//------------------------------------------------------------------------
BOOL CStdFileObject::IsExist()
{
	if (IsOpen())
		return TRUE;

	if (Open())
	{
		Close();
		return TRUE;
	}

	return FALSE;
}
//------------------------------------------------------------------------
FILE* CStdFileObject::GetHandle()
{
	return m_pFile;
}
//------------------------------------------------------------------------
void CStdFileObject::TraceIOError(DWORD dwErrorID)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dwErrorID,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf, 0, NULL);
	
	char buf[MAX_PATH];
	wsprintf(buf, "IOERROR(%d): %s(%s)\r\n", dwErrorID, lpMsgBuf, m_strFileName);
	OutputDebugString(buf);
	LocalFree( lpMsgBuf );
}
//------------------------------------------------------------------------