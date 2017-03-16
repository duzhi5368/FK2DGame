/**
*	created:		2012-6-16   0:12
*	filename: 		StdFileSystem
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CStdFileSystem  : public IFileSystem
{
public:
	enum {__DIR = 0, __FILE};
private:
	string			m_strRootDirectory;		// 文件系统顶层目录名
	CStdFileObject*	m_pPackFile;			// 包文件对象
	string			m_strFullPathName;		// 用于缓存文件(目录)的绝对路径名
	DWORD			m_dwLastBlockOffset;	// 最后一个块信息头的偏移(通过它可以将最后一个块的dwNextOffset置为0,仅仅用于DirToPack()功能)

	DWORD			m_dwFilePos;			// 标识目前写包操作进行到文件的哪个位置(字节)
	DWORD			m_dwBlockCount;			// 整个包文件的块数(目录和文件的总数)

	vector<MAPTABLE>	m_MapTable;			// 压缩目录系统时用于记录相对文件名和文件的偏移值

public:
	CStdFileSystem();
	virtual ~CStdFileSystem();

	// IFileSystem
	// 创建文件
	IFileObject* CreateFile(LPCSTR szFileName, char* pData = NULL, int nSize = 0, UINT nStyle = CFS_NEW);
	// 获得某个文件对象(但不打开对应文件!)
	IFileObject* GetFile(LPCSTR szFileName);
	virtual IFileObject* GetFirstFile() {return NULL;};
	virtual IFileObject* GetNextFile() {return NULL;};
	virtual IFileObject* GetIndexFile(LPCSTR szFileName, DWORD dwCompSize) {return NULL;};
	virtual DWORD GetLength() {return 0;};
	// 创建目录
	BOOL CreateDirectory(LPCSTR szDirectoryName);
	// 删除指定的文件
	BOOL DeleteFile(LPCSTR szFileName);
	// 删除目录
	BOOL DeleteDirectory(LPCSTR szDirectoryName);
	// 文件重命名
	BOOL Rename(LPCSTR szOldFileName,LPCSTR szNewFileName);
	// 1、将目录态文件系统复制成包态文件系统；或反过来
	// 2、将当前文件子系统复制到另一个文件子系统，
	// 如在包文件系统中，经常增删时出现很多空洞，复制到新文件子系统可以解决这个问题
	BOOL CopyToFileSystem(LPCSTR szNewFileSystemName, int nFlags = 0, PACKPROC lpfnPackProc = NULL);
	// 释放
	BOOL Release();

	// Other
	BOOL Init(LPCSTR szFileSystemName);
	BOOL SetRootDir(LPCSTR szPath);

	BOOL DirToPack(LPCSTR szPackFileName, PACKPROC lpfnPackProc = NULL);

	BOOL BrowseDir(LPCSTR szDir, PACKPROC lpfnPackProc = NULL);

	BOOL DirectoryExist(LPCSTR szDirectoryName);
	BOOL MakeDirectory(LPCSTR szDirectoryName);
	BOOL DeleteDirectoryEx(LPCSTR szDirectoryName);

	inline const char* GetFullPathName(LPCSTR szRelativePathName);
};
//------------------------------------------------------------------------