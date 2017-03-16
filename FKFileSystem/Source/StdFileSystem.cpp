/**
*	created:		2012-6-16   0:32
*	filename: 		StdFileSystem
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../Include/FileSysCommon.h"
#include "../Include/StdFileObject.h"
#include "../Include/PackFileObject.h"
#include "../Include/StdFileSystem.h"
#include <io.h>
//------------------------------------------------------------------------
// 构造函数
CStdFileSystem::CStdFileSystem()
{
	m_dwFilePos = m_dwBlockCount = m_dwLastBlockOffset = 0;
	m_pPackFile = NULL;
	if (!m_MapTable.empty())
		m_MapTable.clear();
	m_strFullPathName = "";
	m_strRootDirectory = "";
}
//------------------------------------------------------------------------
// 初始化
BOOL CStdFileSystem::Init(LPCSTR szFileSystemName)
{
	// 默认用当前目录
	char szPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, szPath);
	m_strRootDirectory = szPath;
	if (*(m_strRootDirectory.end() - 1) != '\\')
		m_strRootDirectory += '\\';
	
	// 默认当前目录
	if (szFileSystemName == NULL || 
		*szFileSystemName == '.' || 
		*szFileSystemName == 0) 
		return TRUE;
	else // 不默认
	{
		// 是绝对路径，将后面改为\*，便于对跟目录和网络路径的访问
		if (szFileSystemName[1] == ':' || 
		(szFileSystemName[0] == '\\' && szFileSystemName[1] == '\\') ||
		(szFileSystemName[0] == '/' && szFileSystemName[1] == '/')) 
		{
			m_strRootDirectory = szFileSystemName;
			if (lstrlen(szFileSystemName) <= 3 && szFileSystemName[1] == ':')
				return TRUE;
		}
		else
			m_strRootDirectory += szFileSystemName;
		if (*(m_strRootDirectory.end() - 1) == '*')
			m_strRootDirectory.erase(m_strRootDirectory.end() - 1);
		if (*(m_strRootDirectory.end() - 1) == '\\')
			m_strRootDirectory.erase(m_strRootDirectory.end() - 1);
		m_strRootDirectory += "\\*";
		if (!DirectoryExist(m_strRootDirectory.c_str()))
		{
			if (!MakeDirectory(szFileSystemName))
			{
				TraceLn("错误：CStdFileSystem::Init()出现异常！路径[%s]不存在，创建文件失败。\r\n",
					szFileSystemName);
				return FALSE;
			}
		}
		m_strRootDirectory.erase(m_strRootDirectory.end() - 1);
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
// 析构函数
CStdFileSystem::~CStdFileSystem()
{

}
//------------------------------------------------------------------------
// 创建文件(用相对路径)
IFileObject* CStdFileSystem::CreateFile(LPCSTR szFileName, 
										char* pData /*= NULL*/, 
										int nSize /*= 0*/, 
										UINT nStyle/* = CFS_NEW*/)
{
	ASSERT(szFileName != NULL && *szFileName != 0);

	CStdFileObject* pStdFileObject = new CStdFileObject;
	ASSERT(pStdFileObject != NULL);

	pStdFileObject->SetFilePath(GetFullPathName(szFileName));

	if (nStyle == CFS_NEW) // 新建
	{
		if (pStdFileObject->IsExist()) // 文件存在则失败
		{
			delete pStdFileObject;
			pStdFileObject = NULL;
			TraceLn("错误：创建文件出现异常。 文件 %s 已经存在！", szFileName);
			return NULL;
		}
		// 创建新文件(默认不打开,用户自己设置打开模式来打开)
		pStdFileObject->Open("w");
		pStdFileObject->Close();
	}
	else if (nStyle == CFS_UPDATE) // 更新
	{
		if (!pStdFileObject->IsExist()) // 文件不存在则失败
		{
			delete pStdFileObject;
			pStdFileObject = NULL;
			TraceLn("错误：创建文件出现异常。 文件 %s 不存在！", szFileName);
			return NULL;
		}
		// 创建新文件(默认不打开,用户自己设置打开模式来打开)
		pStdFileObject->Open("w");
		pStdFileObject->Close();
	}
	return static_cast<IFileObject*>(pStdFileObject);
}
//------------------------------------------------------------------------
// 获得某个文件对象(但不打开对应文件!)
IFileObject* CStdFileSystem::GetFile(LPCSTR szFileName)
{
	ASSERT(szFileName != NULL && *szFileName != 0);

	CStdFileObject* pStdFileObject = new CStdFileObject;
	ASSERT(pStdFileObject != NULL);

	pStdFileObject->SetFilePath(GetFullPathName(szFileName));
	return static_cast<IFileObject*>(pStdFileObject);
}
//------------------------------------------------------------------------
// 删除指定的文件
BOOL CStdFileSystem::DeleteFile(LPCSTR szFileName)
{
	ASSERT(szFileName != NULL && *szFileName != 0);

	if (::DeleteFile(GetFullPathName(szFileName)) == 0) // fails
	{
		DWORD dwErr = ::GetLastError();
		if (dwErr == 2 || dwErr == 3) // err:系统找不到指定的文件(路径)
		{
			TraceLn("错误：删除文件时出现异常！文件 %s 不存在！", szFileName);
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------
// 文件(目录)重命名
// 文件可以改变原来路径,目录不支持
// 传入的参数均不能为NULL或空字符串
BOOL CStdFileSystem::Rename(LPCSTR szOldFileName,LPCSTR szNewFileName)	
{
	ASSERT(szOldFileName != NULL && szNewFileName != NULL &&
			*szOldFileName != 0 && *szNewFileName != 0);

	string old_name = GetFullPathName(szOldFileName);
	CStdFileObject* pStdFileObject = new CStdFileObject;
	pStdFileObject->SetFilePath(old_name.c_str());
	if (!pStdFileObject->IsExist()) // 原始文件根本就不存在!
	{
		pStdFileObject->Release();
		pStdFileObject = NULL;
		TraceLn("错误：文件重命名出现异常。文件 %s 不存在！", szOldFileName);
		return FALSE;
	}
	pStdFileObject->Release();
	pStdFileObject = NULL;

	string new_name = GetFullPathName(szNewFileName);
	if (/*rename*/::MoveFile(old_name.c_str(), new_name.c_str()) == 0)
		return TRUE;

	return FALSE;
}
//------------------------------------------------------------------------
// 创建目录
BOOL CStdFileSystem::CreateDirectory(LPCSTR szDirectoryName)
{
	ASSERT(szDirectoryName != NULL && *szDirectoryName != 0);
	return MakeDirectory(GetFullPathName(szDirectoryName));
}
//------------------------------------------------------------------------
// 删除目录
BOOL CStdFileSystem::DeleteDirectory(LPCSTR szDirectoryName)
{
	ASSERT(szDirectoryName != NULL && *szDirectoryName != 0);
	return DeleteDirectoryEx(GetFullPathName(szDirectoryName));
}
//------------------------------------------------------------------------
// 1、将目录态文件系统复制成包态文件系统；或反过来
// 2、将当前文件子系统复制到另一个文件子系统，
// 如在包文件系统中，经常增删时出现很多空洞，复制到新文件子系统可以解决这个问题
BOOL CStdFileSystem::CopyToFileSystem(LPCSTR szNewFileSystemName, int nFlags/* = 0*/, PACKPROC lpfnPackProc/* = NULL*/)
{
	ASSERT(szNewFileSystemName != NULL && *szNewFileSystemName != 0);
	return DirToPack(szNewFileSystemName, lpfnPackProc);
}
//------------------------------------------------------------------------
// 释放
BOOL CStdFileSystem::Release()
{
#ifdef _DEBUG
	char szbuf[256];
	wsprintf(szbuf, "释放文件系统 %s ...\r\n", m_strRootDirectory);
#endif
	if (m_pPackFile != NULL)
	{
		m_pPackFile->Release();
		m_pPackFile = NULL;
	}
#ifdef _DEBUG
	OutputDebugString(szbuf);
#endif
	delete this;
	return TRUE;
}
//------------------------------------------------------------------------
// 设置文件系统的初始化目录
BOOL CStdFileSystem::SetRootDir(LPCSTR szPath)
{
	if (szPath[1] != ':') // 是否绝对路径
		return FALSE;

	if (!DirectoryExist(szPath)) // 是否存在
		return FALSE;

	m_strRootDirectory = szPath;

	// 如果目录的最后一个字母不是'\',则在最后加上一个'\'
	if (*(m_strRootDirectory.end() - 1) != '\\')
		m_strRootDirectory += '\\';

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CStdFileSystem::BrowseDir(LPCSTR szDir, PACKPROC lpfnPackProc/* = NULL*/)
{
	CStdFileObject* pStdFileObject = NULL;
	HANDLE hFile;
	WIN32_FIND_DATA wfd;
	static int nBlockDepth = 0;

	//首先查找szDir目录中符合要求的文件
	string strDir = szDir;

	if (*(strDir.end() - 1) != '\\') // 先在最后加"\"
		strDir += '\\';

	string strFiles = strDir + "*.*";
	string strNextDir;

	// 这次全找文件
	if ((hFile = ::FindFirstFile(strFiles.c_str(), &wfd)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // 是文件
			{
				// 找到的文件构造成文件对象
				pStdFileObject = new CStdFileObject();
				string strFileName = strDir;
				strFileName += wfd.cFileName;
				pStdFileObject->SetFilePath(strFileName.c_str());

				// new一个块结构并填充
				BLOCKINFO* pBlockInfo = new BLOCKINFO();
				ASSERT(pBlockInfo != NULL);

				char strTmpFileName[MAX_RELATIVE_PATH_LEN];
				memset( strTmpFileName, 0, sizeof(strTmpFileName) );
				memcpy_s( strTmpFileName, MAX_RELATIVE_PATH_LEN, &strFileName[m_strRootDirectory.length()], strFileName.length() - m_strRootDirectory.length() );
				MAPTABLE mt(strTmpFileName, m_dwFilePos);
				pBlockInfo->bIsValid		= TRUE;
				pBlockInfo->u.bCompress		= FALSE;
				pBlockInfo->SetBlockName(wfd.cFileName);
				pBlockInfo->dwBlockSize		= wfd.nFileSizeLow;
				pBlockInfo->dwCompSize		= wfd.nFileSizeLow;
				pBlockInfo->dwOffset		= m_dwFilePos + sizeof(BLOCKINFO);
				pBlockInfo->dwPrevOffset	= m_dwLastBlockOffset;
				pBlockInfo->dwNextOffset	= m_dwFilePos + sizeof(BLOCKINFO) + wfd.nFileSizeLow;
				pBlockInfo->dwType			= CStdFileSystem::__FILE;
				pBlockInfo->dwVersion		= 1;
				m_dwLastBlockOffset = m_dwFilePos;
				m_dwFilePos += sizeof(BLOCKINFO) + wfd.nFileSizeLow; // 文件指针偏移

				// 将块写入包文件
				m_pPackFile->Write(pBlockInfo, sizeof(BLOCKINFO));
				m_MapTable.push_back(mt);
				m_dwBlockCount ++;

				// 将文件数据写入包文件
				char* pFileBuf = new char[wfd.nFileSizeLow];
				pStdFileObject->Open();
				pStdFileObject->Read(pFileBuf, wfd.nFileSizeLow);
				m_pPackFile->Write(pFileBuf, wfd.nFileSizeLow);

				// 回调
				if (lpfnPackProc)
					lpfnPackProc(1);

				// 释放缓冲
				SAFE_DELETE(pFileBuf);
				SAFE_DELETE(pBlockInfo);
				SAFE_DELETE(pStdFileObject);
			}
		}
		while (::FindNextFile(hFile, &wfd));

		::FindClose(hFile);
	}


	// 这次全找目录
	if ((hFile = ::FindFirstFile(strFiles.c_str(), &wfd)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // 是目录
			{
				if (strcmp(wfd.cFileName, ".") != 0 && strcmp(wfd.cFileName, "..") != 0)
				{
					nBlockDepth ++;

					// 修正下一层的目录名
					strNextDir = strDir + wfd.cFileName;
					
					// new一个块结构并填充
					BLOCKINFO* pBlockInfo = new BLOCKINFO();
					ASSERT(pBlockInfo != NULL);

					char strTmpFileName[MAX_RELATIVE_PATH_LEN];
					memset( strTmpFileName, 0, sizeof(strTmpFileName) );
					memcpy_s( strTmpFileName, MAX_RELATIVE_PATH_LEN, &strNextDir[m_strRootDirectory.length()], strNextDir.length() - m_strRootDirectory.length() );
					MAPTABLE mt(strTmpFileName, m_dwFilePos);

					pBlockInfo->bIsValid		= TRUE;
					pBlockInfo->u.nBlockDepth	= nBlockDepth;
					pBlockInfo->SetBlockName(wfd.cFileName);
					pBlockInfo->dwBlockSize		= 0;
					pBlockInfo->dwCompSize		= 0;
					pBlockInfo->dwOffset		= 0;
					pBlockInfo->dwPrevOffset	= m_dwLastBlockOffset;
					pBlockInfo->dwNextOffset	= m_dwFilePos + sizeof(BLOCKINFO);
					pBlockInfo->dwType			= CStdFileSystem::__DIR;
					pBlockInfo->dwVersion		= 1;
					m_dwLastBlockOffset = m_dwFilePos;
					m_dwFilePos += sizeof(BLOCKINFO); // 文件指针偏移

					// 将块写入包文件
					m_pPackFile->Write(pBlockInfo, sizeof(BLOCKINFO));
					m_MapTable.push_back(mt);
					m_dwBlockCount ++;

					// 释放块缓冲
					SAFE_DELETE(pBlockInfo);

					// 继续浏览下一层目录(递归)
					BrowseDir(strNextDir.c_str(), lpfnPackProc);
					nBlockDepth --;
				}
			}

		}
		while (::FindNextFile(hFile, &wfd));

		::FindClose(hFile);
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CStdFileSystem::DirToPack(LPCSTR szPackFileName, PACKPROC lpfnPackProc/* = NULL*/)
{
	m_pPackFile = new CStdFileObject();
	m_pPackFile->SetFilePath(szPackFileName);
	m_pPackFile->Open("wb");
	
	// 写入包文件头
	PACKHEADER	ph;
	m_pPackFile->Write(&ph, sizeof(PACKHEADER));

	// 文件指针偏移
	m_dwFilePos = 0;
	m_dwFilePos += sizeof(PACKHEADER);
	ph.dwBlockOffset = m_dwFilePos;

	// 写入目录名时去掉路径信息
	string strDirName = m_strRootDirectory;
	if (*(strDirName.end() - 1) == '\\') // 删除最后一个"\"
		strDirName.erase(strDirName.end() - 1); 
	int npos = strDirName.find_last_of("\\");
	strDirName.erase(strDirName.begin(), strDirName.begin() + npos + 1);

	// 填充顶层目录的块信息
	BLOCKINFO* pBlockInfo = new BLOCKINFO();
	ASSERT(pBlockInfo != NULL);

	MAPTABLE mt(strDirName.c_str(), m_dwFilePos);

	m_dwLastBlockOffset = 0;

	pBlockInfo->bIsValid		= TRUE;
	pBlockInfo->u.nBlockDepth	= 0;
	pBlockInfo->SetBlockName(strDirName.c_str());
	pBlockInfo->dwBlockSize		= 0;
	pBlockInfo->dwCompSize		= 0;
	pBlockInfo->dwOffset		= 0;
	pBlockInfo->dwPrevOffset	= m_dwLastBlockOffset; // 前面没有块信息头了!
	pBlockInfo->dwNextOffset	= m_dwFilePos + sizeof(BLOCKINFO);
	pBlockInfo->dwType			= CStdFileSystem::__DIR;
	pBlockInfo->dwVersion		= 1;
	m_dwLastBlockOffset = m_dwFilePos;
	m_dwFilePos += sizeof(BLOCKINFO); // 文件指针偏移

	m_pPackFile->Write(pBlockInfo, sizeof(BLOCKINFO));
	m_MapTable.push_back(mt);
	m_dwBlockCount ++;

	SAFE_DELETE(pBlockInfo); // 释放块内存

	// 浏览顶层目录下的所有的文件和子目录,并将数据写入包文件
	BrowseDir(m_strRootDirectory.c_str(), lpfnPackProc);

	// 写文件映射表到文件尾
	m_pPackFile->Write(&m_MapTable[0], m_MapTable.size()*sizeof(MAPTABLE));

	// 修改最后一个块信息头的dwNextOffset值
	BLOCKINFO bi;
	m_pPackFile->Seek(m_dwLastBlockOffset + offsetof(BLOCKINFO, dwNextOffset), SEEK_SET);
	DWORD dwLastBlock_NextOffset = 0;
	m_pPackFile->Write(&dwLastBlock_NextOffset, sizeof(DWORD));

	// 修改包文件头的某些数据
	ph.dwBlockCount = m_dwBlockCount;
	ph.dwMapOffset = m_dwFilePos;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Write(&ph, sizeof(PACKHEADER));

	// 包文件关闭
	m_pPackFile->Close();
	return TRUE;
}
//------------------------------------------------------------------------
// 判断目录是否存在
BOOL CStdFileSystem::DirectoryExist(LPCSTR szDirectoryName)
{
	string strDir = szDirectoryName;
	if (*(strDir.end() - 1) == '\\') // 先删除最后的"\"
		*(strDir.end() - 1) = '\0';

	WIN32_FIND_DATA wfd; // 查找

	BOOL bExist = FALSE;
	HANDLE hFind=::FindFirstFile(strDir.c_str(), &wfd); 
	if (hFind == INVALID_HANDLE_VALUE) // 没有找到匹配，目录肯定不存在
	{
		bExist = FALSE;
	}
	else
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // 检查找到的结果是否目录
			bExist = TRUE; // 是目录,目录存在
		else // 不是目录,目录不存在
			bExist = FALSE;
	}
	FindClose(hFind);
	return bExist;
}
//------------------------------------------------------------------------
// 创建目录(包括多级,szDirectoryName需绝对路径名;递归)
// 自动检测目录是否存在
BOOL CStdFileSystem::MakeDirectory(LPCSTR szDirectoryName)
{
	ASSERT(szDirectoryName != NULL && *szDirectoryName != 0);

	if (DirectoryExist(szDirectoryName)) // 目录已存在
		return TRUE;

	string strDir = szDirectoryName;
	string strParent;

	if (*(strDir.end() - 1) == '\\') // 先删除最后的"\"
		strDir.erase(strDir.end() - 1, strDir.end());

	// 分开父目录和本身目录名称
	for (int i = strDir.length()-1; i > 0; i --)
	{
		if (strDir.at(i) == '\\' || strDir.at(i) == '/')
		{
			strParent = strDir.substr(0, i);
			break;
		}
	}

	if (strParent.empty()) // 目录名称错误
		return FALSE; 

	BOOL Ret = TRUE;
	if (strParent.length() > 3) // 如果长度小于3，表示为磁盘根目录
		Ret = DirectoryExist(strParent.c_str());// 检查父目录是否存在

	if (!Ret) // 父目录不存在,递归调用创建父目录
		Ret = MakeDirectory(strParent.c_str()); 

	if (Ret) // 父目录存在,直接创建目录
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength=sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = 0;
		Ret = ::CreateDirectory(strDir.c_str(), &sa);
	}
	return Ret;
} 
//------------------------------------------------------------------------
// 删除目录(包括多级和目录里的文件;递归)
// 自动检测要删除的目录是否存在,不存在返回TRUE,跳过删除操作
BOOL CStdFileSystem::DeleteDirectoryEx(LPCSTR szDirectoryName)
{
	ASSERT(szDirectoryName != NULL && *szDirectoryName != 0);

	string strDir = szDirectoryName;

	// 参数长度必须大于3，即不能为磁盘根目录或空白
	if (strDir.empty() || strDir.length() < 4)
		return FALSE;

	if (*(strDir.end() - 1) != '\\') // 先在最后加"\"
		strDir += '\\';

	int len = strDir.length();

	string strFiles = strDir + "*.*";

	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(strFiles.c_str(), &wfd);
	if (hFind == INVALID_HANDLE_VALUE) // 目录不存在,我们返回TRUE
	{
		TraceLn("错误： CStdFileSystem::DeleteDirectoryEx() 出现异常。目录 %s 不存在！", szDirectoryName);
		return TRUE; 
	}

	BOOL Ret = TRUE;
	string strTmp;
	if (hFind != INVALID_HANDLE_VALUE) // 存在此目录
	{
		BOOL bFind = TRUE;
		while (bFind)
		{
			if (wfd.cFileName[0] != '.') // . ..
			{
				strTmp = strDir + wfd.cFileName;
				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // 删除所有子目录
				{ 
					Ret = Ret && DeleteDirectoryEx(strTmp.c_str());
				}
				else // 删除所有文件
				{ 
					SetFileAttributes(strTmp.c_str(), FILE_ATTRIBUTE_NORMAL);
					Ret = Ret && ::DeleteFile(strTmp.c_str());
				}
			}
			bFind = FindNextFile(hFind, &wfd);
		}
		FindClose(hFind);

		if (Ret)
		{
			BOOL b = ::RemoveDirectory(strDir.c_str());
			if (b)
			{
				return TRUE;
			}
			TraceLn("错误：移除文件夹失败。 GetLastError() = %d", ::GetLastError());
			return FALSE;
		}
	}

	return FALSE;
} 
//------------------------------------------------------------------------
// 获得相对路径的绝对路径(如果本身是绝对路径,则略过)
inline const char* CStdFileSystem::GetFullPathName(LPCSTR szRelativePathName)
{
	if (szRelativePathName[0]=='/' || 
		szRelativePathName[0]=='\\' || 
		szRelativePathName[1] == ':') // 本身就是绝对路径
		return szRelativePathName;

	m_strFullPathName = m_strRootDirectory;
	m_strFullPathName += szRelativePathName;
	return m_strFullPathName.c_str();
}
//------------------------------------------------------------------------