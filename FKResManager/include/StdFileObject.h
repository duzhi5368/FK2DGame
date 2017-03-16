/**
*	created:		2012-6-18   20:57
*	filename: 		StdFileObject
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CStdFileObject  : public IFileObject
{
private:
	FILE*	m_pFile;
	string	m_strFileName; 
public:
	CStdFileObject();
	virtual ~CStdFileObject();
public:
	// IFileObject
	BOOL Open(LPCSTR lpszMode = "rb");
	BOOL Close();

	UINT Read(void* lpBuf, UINT nCount);
	BOOL Write(const void* lpBuf, UINT nCount);
	LPSTR ReadString(LPSTR lpsz, UINT nMax);
	BOOL WriteString(LPCSTR lpsz);

	BOOL Seek(LONG lOff, UINT nFrom = SEEK_SET);
	BOOL SeekToBegin();
	BOOL SeekToEnd();
	int GetPosition();

	BOOL Flush();

	DWORD GetLength();
	BOOL SetLength(DWORD dwNewLen);
	LPCSTR GetFileName();
	BOOL GetFileTitle(char* buf);
	LPCSTR GetFilePath();

	BOOL IsExist();
	BOOL Release();

	BOOL SetFilePath(LPCSTR lpszNewName);
	BOOL IsOpen();
	FILE* GetHandle(); // 获得文件句柄
private:
	void TraceIOError(DWORD dwErrorID);
};
//------------------------------------------------------------------------