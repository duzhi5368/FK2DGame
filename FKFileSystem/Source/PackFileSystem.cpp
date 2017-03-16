/**
*	created:		2012-6-16   1:46
*	filename: 		PackFileSystem
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../include/FileSysCommon.h"
#include "../include/StdFileObject.h"
#include "../include/PackFileObject.h"
#include "../Include/PackFileSystem.h"
//------------------------------------------------------------------------
// 构造函数
CPackFileSystem::CPackFileSystem()
{
	m_dwFilePos = m_dwBlockCount = 0;
	m_pPackFile = NULL;
	if (!m_Hash.empty())
		m_Hash.clear();
	m_strFullPathName = "";
	m_strRootDirectory = "";
	m_itCurFile = m_Hash.begin();
}
//------------------------------------------------------------------------
// 析构函数
CPackFileSystem::~CPackFileSystem()
{

}
//------------------------------------------------------------------------
// 初始化
BOOL CPackFileSystem::Init(LPCSTR szFileSystemName)
{
	ASSERT(szFileSystemName != NULL);

	// 创建新的包文件对象，并打开来进行读写操作
	m_pPackFile = new CStdFileObject();
	m_pPackFile->SetFilePath(szFileSystemName);
	if (!m_pPackFile->IsExist()) // 不存在，则新建
	{
#ifndef _DEBUG
		char buf[256];
		wsprintf(buf, "文件系统不存在(%s)!", szFileSystemName);
		::MessageBox(NULL, buf, "ERROR", MB_OK);
		TraceLn("错误： CPackFileSystem::Init() 出现异常！文件 %s 不存在！", szFileSystemName);		
		m_pPackFile->Release();
		return FALSE;
#else
		string	str = szFileSystemName;
		int nPos = str.find_last_of("\\");
		if (nPos != -1)
		{
			str.erase(str.begin() + nPos, str.end());
			if (!DirectoryExist(str.c_str()) && !MakeDirectory(str.c_str()))
				return FALSE;
		}
		m_pPackFile->Open("wb");

		// 写入包文件头
		PACKHEADER	ph;
		ph.dwBlockCount = 1;
		ph.dwBlockOffset = sizeof(PACKHEADER);	
		ph.dwMapOffset = sizeof(PACKHEADER) + sizeof(BLOCKINFO);
		m_pPackFile->Write(&ph, sizeof(PACKHEADER));

		// 写入目录名时去掉路径信息
		char strDirName[MAX_PATH];
		m_pPackFile->GetFileTitle(strDirName);

		// 填充顶层目录的块信息
		BLOCKINFO* pBlockInfo = new BLOCKINFO();
		ASSERT(pBlockInfo != NULL);
		pBlockInfo->bIsValid		= TRUE;
		pBlockInfo->u.nBlockDepth	= 0;
		pBlockInfo->SetBlockName(strDirName);
		pBlockInfo->dwBlockSize		= 0;
		pBlockInfo->dwCompSize		= 0;
		pBlockInfo->dwOffset		= 0;
		pBlockInfo->dwPrevOffset	= 0; // 前面没有块信息头了!
		pBlockInfo->dwNextOffset	= 0;
		pBlockInfo->dwType			= 0;
		pBlockInfo->dwVersion		= 1;
		m_pPackFile->Write(pBlockInfo, sizeof(BLOCKINFO));
		SAFE_DELETE(pBlockInfo); // 释放块内存

		// 写文件映射表到文件尾
		MAPTABLE mt(strDirName, sizeof(PACKHEADER));
		m_pPackFile->Write(&mt, sizeof(MAPTABLE));

		// 包文件关闭
		m_pPackFile->Close();
#endif		
	}
	if (GetFileAttributes(szFileSystemName) & FILE_ATTRIBUTE_READONLY )
	{
		if (!SetFileAttributes(szFileSystemName, FILE_ATTRIBUTE_NORMAL))
		{
			TraceLn("错误：CPackFileSystem::Init() 出现异常。文件 %s 不能设置文件属性 SetFileAttributes failed ！", szFileSystemName);
			return FALSE;
		}
	}
	if (!m_pPackFile->Open("r+b"))
	{
		TraceLn("错误：CPackFileSystem::Init() 出现异常。文件 %s 无法打开！", szFileSystemName);
		return FALSE;
	}
	m_strRootDirectory = szFileSystemName;
	int pos = m_strRootDirectory.find_last_of('\\');
	m_strRootDirectory.erase(m_strRootDirectory.begin() + pos + 1, m_strRootDirectory.end());

	// 读取文件映射表
	return UpdateMapTable();
}
//------------------------------------------------------------------------
BOOL CPackFileSystem::UpdateMapTable()
{
	// 读取文件映射表
	PACKHEADER ph;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid()) // 检测标识是否正确
	{
		m_pPackFile->Close();
		m_pPackFile->Release();
		m_pPackFile = NULL;
		return FALSE;
	}

	// 有时候CreateFile可能没有正常结束，所以这里对映射表进行一些强校验
	if (ph.dwReserved1 == 1)
	{
		if (m_pPackFile->GetLength() != 
			ph.dwMapOffset + sizeof(MAPTABLE)*ph.dwBlockCount)
		{
			TraceLn("错误：CPackFileSystem::UpdateMapTable() 出现异常。文件 %s 映射表偏移值非法！", m_strRootDirectory.c_str());
			return FALSE;
		}
	}
	m_dwBlockCount = ph.dwBlockCount;
	MAPTABLE* pmt = new MAPTABLE[ph.dwBlockCount];
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Read(pmt, sizeof(MAPTABLE)*ph.dwBlockCount);

	// 创建hash表
	if (!m_Hash.empty())
		m_Hash.clear();
	for (int i = 0; i < ph.dwBlockCount; i ++)
		AddToHash(pmt[i].name, pmt[i].offset);

	m_itCurFile = m_Hash.begin();
	// 读顶层目录名
	BLOCKINFO bi;
	m_pPackFile->Seek(ph.dwBlockOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid()) // 不是块信息头
	{
		delete[] pmt;
		m_Hash.clear();
		return FALSE;
	}
	m_strRootDirectory += bi.cbName;
	m_strRootDirectory += "\\";

	// Seek to begin
	m_pPackFile->SeekToBegin();

	delete[] pmt;
	return TRUE;
}
//------------------------------------------------------------------------
// 创建文件(用相对路径)
IFileObject* CPackFileSystem::CreateFile(LPCSTR szFileName, 
										 char* pData /*= NULL*/, 
										 int nSize /*= 0*/, 
										 UINT nStyle/* = CFS_NEW*/)
{
	ASSERT(szFileName != NULL && *szFileName != 0);
	if (pData == NULL || nSize <= 0)
		return NULL;

	// 文件是否存在?存在返回NULL?
	DWORD Offset = GetOffsetFromHash(szFileName);
	if (Offset)
	{
		BLOCKINFO bi;
		m_pPackFile->Seek(Offset, SEEK_SET);
		m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
		if (!bi.IsValid() || !bi.bIsValid || bi.dwType == CPackFileSystem::__DIR) // 不是块信息头,标记为无效,目录
		{
			TraceLn("CPackFileSystem::CreateFile()块文件信息头无效");
			return NULL;
		}

		if (bi.dwCompSize >= nSize) // 原地更新
		{
			bi.dwBlockSize = nSize;
			bi.dwCompSize = nSize;
			m_pPackFile->Seek(Offset, SEEK_SET);
			m_pPackFile->Write(&bi, sizeof(BLOCKINFO));
			m_pPackFile->Write(pData, nSize);// 将数据更新
			return GetFile(szFileName);
		}
		else // 删除本文件后重新建立一个文件
		{
			if (!DeleteFile(szFileName))
			{
				TraceLn("CPackFileSystem::CreateFile() 删除原文件 %s 失败！", szFileName );
				return NULL;
			}
			return CreateFile(szFileName, pData, nSize, nStyle);
		}
	}

	// 创建新文件
	// 获取块名和其父目录对应的路径名
	char szBlockName[MAX_BLOCK_NAME_LEN];
	char szParentPathName[MAX_RELATIVE_PATH_LEN];
	string strFileName = GetFullPathName(szFileName);
	int pos = strFileName.find_last_of('\\');
	lstrcpy(szBlockName, string( strFileName.substr( pos + 1, strFileName.length() - pos - 1 ) ).c_str() );
	string strParentDir = strFileName;
	strParentDir.erase(strParentDir.begin() + pos, strParentDir.end());
	if (strParentDir.length() == m_strRootDirectory.length() - 1) // 没有父目录
	{
		int n = strParentDir.find_last_of('\\');
		lstrcpy(szParentPathName, string( strFileName.substr( n + 1, strFileName.length() - n - 1 ) ).c_str() );
	}
	else
	{
		lstrcpy(szParentPathName, string( strFileName.substr( m_strRootDirectory.length(), strFileName.length() - m_strRootDirectory.length() ) ).c_str() );
	}
	
	// 获取其父目录的块信息
	BLOCKINFO bi;
	DWORD dwParentOffset = GetOffsetFromHash(szParentPathName);
	if (!dwParentOffset) // 父目录不存在!先创建父目录吧!!!
	{
		TraceLn("错误： CPackFileSystem::CreateFile() 出现异常。创建父目录失败，父目录不存在！");
		return NULL;
	}

	// 读取父目录的块信息，并修改它的下一个块的偏移值
	m_pPackFile->Seek(dwParentOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid()) // 不是块信息头
		return NULL;

	// 读取文件映射表
	PACKHEADER ph;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid()) // 不是包文件头
		return NULL;
	// 写一个标志，当CreateFile顺利结束后重置这个标志
	ph.dwReserved1 = 1;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Write(&ph, sizeof(PACKHEADER));
	m_pPackFile->Flush();

	// 将修改后的父目录块信息重写
	DWORD dwNextOffset = bi.dwNextOffset;
	bi.dwNextOffset = ph.dwMapOffset;
	m_pPackFile->Seek(dwParentOffset, SEEK_SET);
	m_pPackFile->Write(&bi, sizeof(BLOCKINFO));

	CPackFileObject* pPackFileObject = new CPackFileObject;
	ASSERT(pPackFileObject != NULL);
	pPackFileObject->SetFilePath(GetFullPathName(szFileName));

	// 加载映射表
	MAPTABLE* pmt = new MAPTABLE[ph.dwBlockCount];
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Read(pmt, ph.dwBlockCount*sizeof(MAPTABLE));

	// new一个块结构并填充
	BLOCKINFO* pBlockInfo = new BLOCKINFO();
	ASSERT(pBlockInfo != NULL);
	
	// 记录映射信息
	MAPTABLE mt( string( strFileName.substr( m_strRootDirectory.length(), strFileName.length() - m_strRootDirectory.length() ) ).c_str(), ph.dwMapOffset);

	pBlockInfo->bIsValid		= TRUE;
	pBlockInfo->u.bCompress		= FALSE;
	pBlockInfo->SetBlockName(szBlockName);
	pBlockInfo->dwBlockSize		= nSize;
	pBlockInfo->dwCompSize		= nSize;
	pBlockInfo->dwOffset		= ph.dwMapOffset + sizeof(BLOCKINFO);
	pBlockInfo->dwPrevOffset	= dwParentOffset;
	pBlockInfo->dwNextOffset	= dwNextOffset;
	pBlockInfo->dwType			= CPackFileSystem::__FILE;
	pBlockInfo->dwVersion		= 1;

	// 改写下一个块信息的dwPrevOffset
	if (dwNextOffset != 0) // 说明本块不是最后一个块
	{
		m_pPackFile->Seek(dwNextOffset + offsetof(BLOCKINFO, dwPrevOffset), SEEK_SET);
		m_pPackFile->Write(&ph.dwMapOffset, sizeof(DWORD));
	}

	// 将块写入包文件
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Write(pBlockInfo, sizeof(BLOCKINFO));
	AddToHash(mt.name, mt.offset);
	m_itCurFile = m_Hash.begin();

	// 将文件数据写入包文件
	m_pPackFile->Write(pData, nSize);

	// 重写映射表
	BOOL bFound = FALSE;
	for (int i = 0; i < ph.dwBlockCount; i ++)
	{
		if (pmt[i].offset == dwParentOffset) // 找到父目录的对应项!
		{
			bFound = TRUE;
			m_pPackFile->Seek(ph.dwMapOffset + sizeof(BLOCKINFO) + nSize, SEEK_SET); // 新位置
			m_pPackFile->Write(pmt, (i + 1)*sizeof(MAPTABLE)); // 写父目录以前的(包括父目录)
			m_pPackFile->Write(&mt, sizeof(MAPTABLE));// 写刚创建的文件的映射表
			m_pPackFile->Write(&pmt[i + 1], (ph.dwBlockCount - i -1)*sizeof(MAPTABLE));// 写新创建文件后面的映射表
			break;
		}
	}
	if (!bFound) // 没找到!操作失败
	{
		pPackFileObject->Release();
		pPackFileObject = NULL;
		SAFE_DELETE(pBlockInfo);
		delete[] pmt;
		return NULL;
	}

	// 设置文件对象
	pPackFileObject->SetFileInfo(m_pPackFile->GetHandle(), ph.dwMapOffset, nSize);

	// 修改包文件头
	m_dwBlockCount ++;
	ph.dwBlockCount = m_dwBlockCount;
	ph.dwMapOffset += sizeof(BLOCKINFO) + nSize;
	ph.dwReserved1 = 0;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Write(&ph, sizeof(PACKHEADER));
	m_pPackFile->Flush();

	// 释放缓冲
	SAFE_DELETE(pBlockInfo);
	delete[] pmt;
	return static_cast<IFileObject*>(pPackFileObject);
}
//------------------------------------------------------------------------
// 获得某个文件对象(但不打开对应文件!)
IFileObject* CPackFileSystem::GetFile(LPCSTR szFileName)
{
	ASSERT(szFileName != NULL && *szFileName != 0);

	DWORD dwOffset = GetOffsetFromHash(szFileName);
	if (!dwOffset) // 哈希表中不存在!
		return NULL;

	BLOCKINFO bi;
	m_pPackFile->Seek(dwOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid() || !bi.bIsValid || bi.dwType == CPackFileSystem::__DIR) // 不是块信息头,标记为无效,目录
	{
		return NULL;
	}

	CPackFileObject* pPackFileObject = new CPackFileObject;
	ASSERT(pPackFileObject != NULL);

	pPackFileObject->SetFilePath(GetFullPathName(szFileName));

	pPackFileObject->SetFileInfo(m_pPackFile->GetHandle(), dwOffset, bi.dwCompSize);
	return static_cast<IFileObject*>(pPackFileObject);
}
//------------------------------------------------------------------------
// 获得某个文件对象(但不打开对应文件!)
IFileObject* CPackFileSystem::GetIndexFile(LPCSTR szFileName, DWORD dwCompSize)
{
	ASSERT(szFileName != NULL && *szFileName != 0);

	DWORD dwOffset = GetOffsetFromHash(szFileName);
	if (!dwOffset) // 哈希表中不存在!
		return NULL;

	CPackFileObject* pPackFileObject = new CPackFileObject;
	ASSERT(pPackFileObject != NULL);

	pPackFileObject->SetFilePath(GetFullPathName(szFileName));

	pPackFileObject->SetFileInfo(m_pPackFile->GetHandle(), dwOffset, dwCompSize);
	return static_cast<IFileObject*>(pPackFileObject);
}
//------------------------------------------------------------------------
IFileObject* CPackFileSystem::GetFirstFile()
{
	if(m_Hash.size() == 0)
		return NULL;
	
	m_itCurFile = m_Hash.begin();
	
	DWORD dwOffset = (*m_itCurFile).second;
	if (!dwOffset) // 哈希表中不存在!
		return NULL;

	BLOCKINFO bi;
	m_pPackFile->Seek(dwOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid() || !bi.bIsValid || bi.dwType == CPackFileSystem::__DIR) // 不是块信息头,标记为无效,目录
	{
		return NULL;
	}

	CPackFileObject* pPackFileObject = new CPackFileObject;
	ASSERT(pPackFileObject != NULL);

	pPackFileObject->SetFilePath(GetFullPathName((*m_itCurFile).first.c_str()));

	pPackFileObject->SetFileInfo(m_pPackFile->GetHandle(), dwOffset, bi.dwCompSize);

	m_itCurFile++;
	return static_cast<IFileObject*>(pPackFileObject);
}
//------------------------------------------------------------------------
IFileObject* CPackFileSystem::GetNextFile()
{
	if(m_itCurFile == m_Hash.end())
		return NULL;

	DWORD dwOffset = (*m_itCurFile).second;
	if (!dwOffset) // 哈希表中不存在!
		return NULL;

	BLOCKINFO bi;
	m_pPackFile->Seek(dwOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid() || !bi.bIsValid || bi.dwType == CPackFileSystem::__DIR) // 不是块信息头,标记为无效,目录
	{
		return NULL;
	}

	CPackFileObject* pPackFileObject = new CPackFileObject;
	ASSERT(pPackFileObject != NULL);

	pPackFileObject->SetFilePath(GetFullPathName((*m_itCurFile).first.c_str()));

	pPackFileObject->SetFileInfo(m_pPackFile->GetHandle(), dwOffset, bi.dwCompSize);

	m_itCurFile++;
	return static_cast<IFileObject*>(pPackFileObject);
}
//------------------------------------------------------------------------
// 删除指定的文件
BOOL CPackFileSystem::DeleteFile(LPCSTR szFileName)
{
	ASSERT(szFileName != NULL && *szFileName != 0);


	DWORD dwOffset = GetOffsetFromHash(szFileName);
	if (!dwOffset) // 哈希表中不存在!权且当作删除了
	{
		TraceLn("错误：CPackFileSystem::DeleteFile() 出现异常。文件 %s 不存在！", szFileName);
		return TRUE;
	}

	BLOCKINFO bi;
	m_pPackFile->Seek(dwOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid()) // 不是块信息头
		return FALSE;

	// 修改前一块的dwNextOffset值
	if (bi.dwPrevOffset != 0)
	{
		m_pPackFile->Seek(bi.dwPrevOffset + offsetof(BLOCKINFO, dwNextOffset), SEEK_SET);
		m_pPackFile->Write(&bi.dwNextOffset, sizeof(DWORD));
	}

	// 修改后一块的dwPrevOffset值
	if (bi.dwNextOffset != 0)
	{
		m_pPackFile->Seek(bi.dwNextOffset + offsetof(BLOCKINFO, dwPrevOffset), SEEK_SET);
		m_pPackFile->Write(&bi.dwPrevOffset, sizeof(DWORD));
	}

	// 将本块标记为无效
	BOOL bIsValid = FALSE;
	m_pPackFile->Seek(dwOffset + offsetof(BLOCKINFO, bIsValid), SEEK_SET);
	m_pPackFile->Write(&bIsValid, sizeof(BOOL));

	// 删除映射表的对应项
	PACKHEADER ph;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid()) // 不是包文件头
		return FALSE;
	MAPTABLE* pmt = new MAPTABLE[ph.dwBlockCount];
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Read(pmt, sizeof(MAPTABLE)*ph.dwBlockCount);
	BOOL bFound = FALSE;
	for (int i = 0; i < ph.dwBlockCount; i ++)
	{
		if (pmt[i].offset == dwOffset) // 找到!
		{
			bFound = TRUE;
			m_pPackFile->Seek(ph.dwMapOffset + i*sizeof(MAPTABLE), SEEK_SET);
			m_pPackFile->Write(&pmt[i + 1], (ph.dwBlockCount - i -1)*sizeof(MAPTABLE)); // 不过最后一个映射项没有删除!!
			break;
		}
	}
	SAFE_DELETE(pmt);
	if (!bFound) // 没找到!!
		return FALSE;

	// 删除哈希表中的对应项
	EraseFromHash(szFileName);

	// 修改包文件头
	m_dwBlockCount --;
	ph.dwBlockCount = m_dwBlockCount;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Write(&ph, sizeof(PACKHEADER));

	TraceLn("DeleteFile     : %s", szFileName);
	return TRUE;
}
//------------------------------------------------------------------------
// 文件(目录)重命名
// 文件可以改变原来路径,目录不支持
// 传入的参数均不能为NULL或空字符串
BOOL CPackFileSystem::Rename(LPCSTR szOldFileName,LPCSTR szNewFileName)	
{
	ASSERT(szOldFileName != NULL && szNewFileName != NULL &&
			*szOldFileName != 0 && *szNewFileName != 0);

	DWORD dwOffset = GetOffsetFromHash(szOldFileName);
	if (!dwOffset) // 哈希表中不存在!
	{
		TraceLn("错误：CPackFileSystem::Rename()出现异常。文件 %s 不存在！", szOldFileName);
		return FALSE;
	}

	// 修改块中对应的信息
	BLOCKINFO bi;
	m_pPackFile->Seek(dwOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid()) // 不是块信息头
		return FALSE;
	string str = szNewFileName;
	if (*(str.end() - 1) == '\\') // 先删除最后的"\"
		str.erase(str.end() - 1, str.end());
	int pos = str.find_last_of('\\');
	lstrcpy(bi.cbName, string( str.substr( pos + 1, str.length() - pos - 1 ) ).c_str() ); // 更名
	m_pPackFile->Seek(dwOffset, SEEK_SET);
	m_pPackFile->Write(&bi, sizeof(BLOCKINFO));

	// 修改哈希表
	EraseFromHash(szOldFileName);
	AddToHash(str.c_str(), dwOffset);
	m_itCurFile = m_Hash.begin();

	// 修改映射表
	PACKHEADER ph;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid()) // 不是包文件头
		return FALSE;
	MAPTABLE* pmt = new MAPTABLE[ph.dwBlockCount];
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Read(pmt, sizeof(MAPTABLE)*ph.dwBlockCount);
	for (int i = 0; i < ph.dwBlockCount; i ++)
	{
		if (pmt[i].offset == dwOffset)
		{
			lstrcpy(pmt[i].name, str.c_str());
			break;
		}
	}
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Write(pmt, sizeof(MAPTABLE)*ph.dwBlockCount);

	SAFE_DELETE(pmt);

	return TRUE;
}
//------------------------------------------------------------------------
// 创建目录
BOOL CPackFileSystem::CreateDirectory(LPCSTR szDirectoryName)
{
	ASSERT(szDirectoryName != NULL && *szDirectoryName != 0);

	// 目录是否存在?存在返回TRUE?
	DWORD Offset = GetOffsetFromHash(szDirectoryName);
	if (Offset)
		return TRUE;

	// 读取文件映射表
	PACKHEADER ph;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid()) // 不是包文件头
		return FALSE;
	MAPTABLE* pmt = new MAPTABLE[ph.dwBlockCount];
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Read(pmt, sizeof(MAPTABLE)*ph.dwBlockCount);

	// 获取块名和其父目录对应的路径名
	char szBlockName[MAX_BLOCK_NAME_LEN];
	char szParentPathName[MAX_RELATIVE_PATH_LEN];
	string strDirName = GetFullPathName(szDirectoryName);
	int pos = strDirName.find_last_of('\\');
	lstrcpy(szBlockName,  string( strDirName.substr( pos + 1, strDirName.length() - pos - 1 ) ).c_str() );
	string strParentDir = strDirName;
	strParentDir.erase(strParentDir.begin() + pos, strParentDir.end());
	lstrcpy(szParentPathName, string( strParentDir.substr( m_strRootDirectory.length(), strParentDir.length() - m_strRootDirectory.length() ) ).c_str() );

	// 获取父目录的块信息
	BLOCKINFO bi;
	DWORD dwParentOffset = GetOffsetFromHash(szParentPathName);
	if (!dwParentOffset)
	{
		delete[] pmt;
		return FALSE;
	}
	m_pPackFile->Seek(dwParentOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid()) // 不是块信息头
	{
		delete[] pmt;
		return FALSE;
	}
	DWORD dwNextOffset = bi.dwNextOffset;	// 下一个块的位置
	DWORD dwPrevOffset = dwParentOffset;
	int nBlockDepth = bi.u.nBlockDepth;

	if (dwNextOffset == 0) // 父目录是最后一个块
	{
		// 修改父目录的块信息
		bi.dwNextOffset = ph.dwMapOffset;
		m_pPackFile->Seek(dwParentOffset, SEEK_SET);
		m_pPackFile->Write(&bi, sizeof(BLOCKINFO));
	}
	else
	{
		// 找父目录中的最后一个文件块
		while (1)
		{
			m_pPackFile->Seek(dwNextOffset, SEEK_SET);
			m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
			if (!bi.IsValid()) // 不是块信息头
			{
				delete[] pmt;
				return FALSE;
			}

			if (bi.dwType == CPackFileSystem::__DIR) // 目录
			{
				dwPrevOffset = bi.dwPrevOffset;
				// 修改本块的信息
				bi.dwPrevOffset = ph.dwMapOffset;
				m_pPackFile->Seek(dwNextOffset, SEEK_SET);
				m_pPackFile->Write(&bi, sizeof(BLOCKINFO));

				// 修改父目录下最后一个文件的块信息
				m_pPackFile->Seek(dwPrevOffset + offsetof(BLOCKINFO, dwNextOffset), SEEK_SET);
				m_pPackFile->Write(&ph.dwMapOffset, sizeof(DWORD));
				break;
			}
			else if (bi.dwType == CPackFileSystem::__FILE) // 文件
			{
				dwPrevOffset = dwNextOffset;
				dwNextOffset = bi.dwNextOffset;
				if (bi.dwNextOffset == 0) // 此文件块是最后一个块
				{
					bi.dwNextOffset = ph.dwMapOffset;
					m_pPackFile->Seek(dwPrevOffset, SEEK_SET);
					m_pPackFile->Write(&bi, sizeof(BLOCKINFO));
					break;
				}
			}
		}
	}

	// new一个块结构并填充
	BLOCKINFO* pBlockInfo = new BLOCKINFO();
	ASSERT(pBlockInfo != NULL);

	// 记录映射信息
	MAPTABLE mt( string( strDirName.substr( m_strRootDirectory.length(), strDirName.length() - m_strRootDirectory.length() ) ).c_str(), ph.dwMapOffset);

	pBlockInfo->bIsValid		= TRUE;
	pBlockInfo->u.nBlockDepth	= nBlockDepth + 1;
	pBlockInfo->SetBlockName(szBlockName);
	pBlockInfo->dwBlockSize		= 0;
	pBlockInfo->dwCompSize		= 0;
	pBlockInfo->dwOffset		= 0;
	pBlockInfo->dwPrevOffset	= dwPrevOffset;
	pBlockInfo->dwNextOffset	= dwNextOffset;
	pBlockInfo->dwType			= CPackFileSystem::__DIR;
	pBlockInfo->dwVersion		= 1;

	// 将块写入包文件
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Write(pBlockInfo, sizeof(BLOCKINFO));
	AddToHash(mt.name, mt.offset);
	m_itCurFile = m_Hash.begin();

	// 重写映射表
	BOOL bFound = FALSE;
	for (int i = 0; i < ph.dwBlockCount; i ++)
	{
		if (pmt[i].offset == dwPrevOffset) // 找到前一块的对应项!
		{
			bFound = TRUE;
			m_pPackFile->Seek(ph.dwMapOffset + sizeof(BLOCKINFO), SEEK_SET); // 新位置
			m_pPackFile->Write(pmt, (i + 1)*sizeof(MAPTABLE)); // 写前一块以前的(包括前一块)
			m_pPackFile->Write(&mt, sizeof(MAPTABLE));// 写刚创建的块的映射表
			m_pPackFile->Write(&pmt[i + 1], (ph.dwBlockCount - i -1)*sizeof(MAPTABLE));// 写新创建块后面的映射表
			break;
		}
	}
	if (!bFound) // 没找到!操作失败
	{
		SAFE_DELETE(pBlockInfo);
		delete[] pmt;
		return NULL;
	}

	// 修改包文件头
	m_dwBlockCount ++;
	ph.dwBlockCount = m_dwBlockCount;
	ph.dwMapOffset += sizeof(BLOCKINFO);
	m_pPackFile->SeekToBegin();
	m_pPackFile->Write(&ph, sizeof(PACKHEADER));

	// 释放缓冲
	SAFE_DELETE(pBlockInfo);
	delete[] pmt;
	return TRUE;
}
//------------------------------------------------------------------------
// 删除目录
BOOL CPackFileSystem::DeleteDirectory(LPCSTR szDirectoryName)
{
	ASSERT(szDirectoryName != NULL && *szDirectoryName != 0);

	DWORD dwOffset = GetOffsetFromHash(szDirectoryName);
	if (!dwOffset) // 哈希表中不存在!权且当作删除了
	{
		TraceLn("错误：CPackFileSystem::DeleteDirectory() 出现异常。目录 s% 不存在！", szDirectoryName);
		return TRUE;
	}

	BLOCKINFO bi;
	m_pPackFile->Seek(dwOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid()) // 不是块信息头
		return FALSE;

	// 记录本块的前后块偏移值
	DWORD dwPrevOffset = bi.dwPrevOffset;
	DWORD dwNextOffset = bi.dwNextOffset;
	int nBlockDepth = bi.u.nBlockDepth;

	if (dwPrevOffset == 0) // 想删顶层目录?没门!!
		return FALSE;

	// 块路径
	string strPathName = szDirectoryName;
	if (*(strPathName.end() - 1) == '\\')
		strPathName.erase(strPathName.end() - 1);

	// 将本块标记为无效
	BOOL bIsValid = FALSE;
	m_pPackFile->Seek(dwOffset + offsetof(BLOCKINFO, bIsValid), SEEK_SET);
	m_pPackFile->Write(&bIsValid, sizeof(BOOL));
	m_dwBlockCount --;
	EraseFromHash(strPathName.c_str());


	if (dwNextOffset == 0) // 本块是最后一个块
	{
		// 修改前一块的信息
		DWORD dwNext = 0;
		m_pPackFile->Seek(dwPrevOffset + offsetof(BLOCKINFO, dwNextOffset), SEEK_SET);
		m_pPackFile->Write(&dwNext, sizeof(DWORD));
	}
	else
	{
		while(1)
		{
			m_pPackFile->Seek(dwNextOffset, SEEK_SET);
			m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
			if (!bi.IsValid()) // 不是块信息头
				return FALSE;

			if (bi.dwType == CPackFileSystem::__DIR) // 目录
			{
				if (bi.u.nBlockDepth <= nBlockDepth) // 不是本目录的子目录
				{
					// 修改本块的信息
					bi.dwPrevOffset = dwPrevOffset;
					m_pPackFile->Seek(dwNextOffset, SEEK_SET);
					m_pPackFile->Write(&bi, sizeof(BLOCKINFO));

					// 修改待删目录前一个块的信息
					m_pPackFile->Seek(dwPrevOffset + offsetof(BLOCKINFO, dwNextOffset), SEEK_SET);
					m_pPackFile->Write(&dwNextOffset, sizeof(DWORD));
					break;
				}

				// 标记目录删除
				m_pPackFile->Seek(dwNextOffset + offsetof(BLOCKINFO, bIsValid), SEEK_SET);
				m_pPackFile->Write(&bIsValid, sizeof(BOOL));
				dwNextOffset = bi.dwNextOffset;
				strPathName += "\\";
				strPathName += bi.cbName;
				m_dwBlockCount --;
				EraseFromHash(strPathName.c_str());

				if (bi.dwNextOffset == 0) // 此目录块是最后一个块
				{
					if (dwPrevOffset == 0) // 没必要写了!
						break;
					m_pPackFile->Seek(dwPrevOffset + offsetof(BLOCKINFO, dwNextOffset), SEEK_SET);
					m_pPackFile->Write(&dwNextOffset, sizeof(DWORD));
					break;
				}				
			}
			else if (bi.dwType == CPackFileSystem::__FILE) // 文件
			{
				// 标记文件删除
				m_pPackFile->Seek(dwNextOffset + offsetof(BLOCKINFO, bIsValid), SEEK_SET);
				m_pPackFile->Write(&bIsValid, sizeof(BOOL));
				dwNextOffset = bi.dwNextOffset;
				string str = strPathName;
				str += "\\";
				str += bi.cbName;
				m_dwBlockCount --;
				EraseFromHash(str.c_str());

				if (bi.dwNextOffset == 0) // 此文件块是最后一个块
				{
					m_pPackFile->Seek(dwPrevOffset + offsetof(BLOCKINFO, dwNextOffset), SEEK_SET);
					m_pPackFile->Write(&dwNextOffset, sizeof(DWORD));
					break;
				}
			}

		}
	}

	// 删除映射表的对应项
	PACKHEADER ph;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid()) // 不是包文件头
		return FALSE;
	MAPTABLE* pmt = new MAPTABLE[ph.dwBlockCount];
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Read(pmt, sizeof(MAPTABLE)*ph.dwBlockCount);

	BOOL bFound = FALSE;
	if (dwPrevOffset == 0) // 如果删除的是顶层目录
	{
		m_dwBlockCount = 0;
		m_Hash.clear();
	}
	else
	{
		for (int i = 0; i < ph.dwBlockCount; i ++)
		{
			if (pmt[i].offset == dwPrevOffset) // 找到待删目录的前一块!
			{
				bFound = TRUE;
				m_pPackFile->Seek(ph.dwMapOffset + (i + 1)*sizeof(MAPTABLE), SEEK_SET);
				// 将后面没删的块移到待删的位置(特殊情况:后面的都删了)
				m_pPackFile->Write(&pmt[i + 1 + ph.dwBlockCount - m_dwBlockCount], 
					(m_dwBlockCount - i -1)*sizeof(MAPTABLE)); // 不过后面多余的映射表项没有删除!!
				break;
			}
		}
		if (!bFound) // 没找到!!
		{
			SAFE_DELETE(pmt);
			return FALSE;
		}
	}
	SAFE_DELETE(pmt);

	// 修改包文件头
	ph.dwBlockCount = m_dwBlockCount;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Write(&ph, sizeof(PACKHEADER));
	TraceLn("DeleteDirectory: %s", szDirectoryName);
	return TRUE;
}
//------------------------------------------------------------------------
// 1、将目录态文件系统复制成包态文件系统；或反过来
// 2、将当前文件子系统复制到另一个文件子系统，
// 如在包文件系统中，经常增删时出现很多空洞，复制到新文件子系统可以解决这个问题
BOOL CPackFileSystem::CopyToFileSystem(LPCSTR szNewFileSystemName, int nFlags/* = 0*/, PACKPROC lpfnPackProc/* = NULL*/)
{
	ASSERT(szNewFileSystemName != NULL && *szNewFileSystemName != 0);

	if (nFlags == 0)
		return PackToDir(szNewFileSystemName, lpfnPackProc);
	else // Pack to Pack
		return PackToPack(szNewFileSystemName, lpfnPackProc);

	return FALSE;
}
//------------------------------------------------------------------------
// 释放
BOOL CPackFileSystem::Release()
{
#ifdef _DEBUG
	char szbuf[256];
	wsprintf(szbuf, "释放文件系统 %s 完成......\r\n", m_pPackFile->GetFilePath());
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
BOOL CPackFileSystem::PackToDir(LPCSTR szDirectoryName, PACKPROC lpfnPackProc/* = NULL*/)
{
	// 读出包文件头
	PACKHEADER	ph;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid()) // 不是包文件头
		return FALSE;
	m_dwBlockCount = ph.dwBlockCount;

	// 文件指针偏移
	m_dwFilePos = 0;
	m_dwFilePos += sizeof(PACKHEADER);

	DWORD dwBlockCount = 0;
	int nBlockDepth = -1;
	string curDir = szDirectoryName;

	// 是否所有的块都读完了
	while (dwBlockCount < m_dwBlockCount)
	{
		// 读出块信息
		BLOCKINFO bi;
		m_pPackFile->Seek(m_dwFilePos, SEEK_SET);
		m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
		if (!bi.IsValid()) // 不是块信息头
		{
			TraceLn("错误：错误的块文件信息头");
			return FALSE;
		}

		dwBlockCount ++;
		m_dwFilePos = bi.dwNextOffset;

		// 无效块
		if (bi.bIsValid) // 没有标识为删除状态
		{
			// 确定块的绝对名
			if (*(curDir.end() - 1) != '\\') // 先在最后加"\"
				curDir += '\\';

			// 处理块
			if (bi.dwType == CPackFileSystem::__DIR) // 是目录块
			{
				// 比较目录的深度
				if (bi.u.nBlockDepth > nBlockDepth)
					nBlockDepth = bi.u.nBlockDepth;
				else
				{
					int pos = curDir.length();
					for( int i = 0; i < (nBlockDepth - bi.u.nBlockDepth + 2); i ++)
					{
						pos = curDir.rfind('\\', pos - 1);
					}
					curDir.erase(curDir.begin() + pos, 
						curDir.begin() + curDir.length() - 1);
				}
				nBlockDepth = bi.u.nBlockDepth;
				// 设置新的当前目录
				curDir += bi.cbName;

				if (!DirectoryExist(curDir.c_str()))
				{
					SECURITY_ATTRIBUTES sa;
					sa.nLength=sizeof(SECURITY_ATTRIBUTES);
					sa.lpSecurityDescriptor = NULL;
					sa.bInheritHandle = 0;
					if (!::CreateDirectory(curDir.c_str(), &sa))
					{
						TraceLn("错误： CreateDirectory() 出现异常。GetLastError() = %d", ::GetLastError());
						return FALSE;
					}
				}
			}
			else // 是文件块
			{
				// 设置路径文件名
				string curFile = curDir;
				curFile += bi.cbName;

				CStdFileObject* pBlockObject = NULL;
				if (bi.u.bCompress) // 有压缩
				{
					//....
				}
				else // 没有压缩
				{
					// 创建一个空文件,并打开来进行写操作
					pBlockObject = new CStdFileObject();
					pBlockObject->SetFilePath(curFile.c_str());
					if (!pBlockObject->Open("wb")) // 存在则清空
						return FALSE;

					// 开辟文件缓冲
					char* buf = new char[bi.dwBlockSize];

					// 将文件内容读入缓冲
					m_pPackFile->Seek(bi.dwOffset, SEEK_SET);
					m_pPackFile->Read(buf, bi.dwBlockSize);

					// 将缓冲内容写入新创建的文件中
					pBlockObject->Write(buf, bi.dwBlockSize);

					// 关闭文件
					pBlockObject->Close();

					// 释放缓冲空间
					delete[] buf;
					pBlockObject->Release();
					pBlockObject = NULL;
				}
			}
		}
		
		// 回调
		if (lpfnPackProc)
			lpfnPackProc(1);
	}

	m_pPackFile->SeekToBegin();

	return TRUE;
}
//------------------------------------------------------------------------
#if 0
BOOL CPackFileSystem::HandleBlock(LPCSTR szCurDir, PACKPROC lpfnPackProc/* = NULL*/)
{
	static DWORD dwBlockCount = 0;
	static int nBlockDepth = -1;

	// 是否所有的块都读完了
	if (dwBlockCount >= m_dwBlockCount)
	{
		dwBlockCount = 0;
		nBlockDepth = -1;
		return TRUE;
	}

	// 读出块信息
	BLOCKINFO bi;
	m_pPackFile->Seek(m_dwFilePos, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid()) // 不是块信息头
		return FALSE;
	dwBlockCount ++;
	m_dwFilePos = bi.dwNextOffset;

	// 无效块
	if (bi.bIsValid == FALSE)
		return TRUE;

	// 确定块的绝对名
	string strBlockName = szCurDir;
	if (*(strBlockName.end() - 1) != '\\') // 先在最后加"\"
		strBlockName += '\\';

	// 处理块
	if (bi.dwType == CPackFileSystem::__DIR) // 是目录块
	{
		// 比较目录的深度
		if (bi.u.nBlockDepth > nBlockDepth)
			nBlockDepth = bi.u.nBlockDepth;
		else
		{
			int pos = strBlockName.length();
			for( int i = 0; i < (nBlockDepth - bi.u.nBlockDepth + 2); i ++)
			{
				pos = strBlockName.rfind('\\', pos - 1);
			}
			strBlockName.erase(strBlockName.begin() + pos, 
				strBlockName.begin() + strBlockName.length() - 1);
		}
		nBlockDepth = bi.u.nBlockDepth;
		// 设置新的当前目录
		strBlockName += bi.cbName;

		if (!DirectoryExist(strBlockName.c_str()))
		{
			SECURITY_ATTRIBUTES sa;
			sa.nLength=sizeof(SECURITY_ATTRIBUTES);
			sa.lpSecurityDescriptor = NULL;
			sa.bInheritHandle = 0;
			if (!::CreateDirectory(strBlockName.c_str(), &sa))
			{
				TraceLn("ERROR: Api function CreateDirectory() catch error,GetLastError() = %d", ::GetLastError());
				return FALSE;
			}
		}

		// 回调
		if (lpfnPackProc)
			lpfnPackProc(1);

		//DebugMsgLn("CreateDirectory: %s", strBlockName.c_str());
		HandleBlock(strBlockName.c_str(), lpfnPackProc);
		return TRUE;
	}
	else // 是文件块
	{
		// 设置路径文件名
		strBlockName += bi.cbName;

		CStdFileObject* pBlockObject = NULL;
		if (bi.u.bCompress) // 有压缩
		{
			//....
		}
		else // 没有压缩
		{
			// 创建一个空文件,并打开来进行写操作
			pBlockObject = new CStdFileObject();
			pBlockObject->SetFilePath(strBlockName.c_str());
			if (!pBlockObject->Open("wb")) // 存在则清空
				return FALSE;

			// 开辟文件缓冲
			char* buf = new char[bi.dwBlockSize];

			// 将文件内容读入缓冲
			m_pPackFile->Seek(bi.dwOffset, SEEK_SET);
			m_pPackFile->Read(buf, bi.dwBlockSize);

			// 将缓冲内容写入新创建的文件中
			pBlockObject->Write(buf, bi.dwBlockSize);

			// 关闭文件
			pBlockObject->Close();

			// 释放缓冲空间
			delete[] buf;
			pBlockObject->Release();
			pBlockObject = NULL;
		}

		// 回调
		if (lpfnPackProc)
			lpfnPackProc(1);

		//DebugMsgLn("CreateFile     : %s", strBlockName.c_str());
		HandleBlock(szCurDir, lpfnPackProc);
		return TRUE;
	}


	return TRUE;
}
#endif
//------------------------------------------------------------------------
BOOL CPackFileSystem::PackToPack(LPCSTR szPackFileName, PACKPROC lpfnPackProc/* = NULL*/)
{
	PACKHEADER ph;
	BLOCKINFO bi;
	DWORD dwFilePos = 0;
	vector<MAPTABLE> MapTable;
	DWORD dwPrevOffset = 0;
	DWORD dwCurrOffset = 0;
	char* pFileBuffer = NULL;
	DWORD dwBlockDepth = 0;

	// 创建新的包文件对象
	CStdFileObject* pNewPackFile = new CStdFileObject;
	pNewPackFile->SetFilePath(GetFullPathName(szPackFileName));
	pNewPackFile->Open("wb");

	// 读出包文件头
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid())
		return FALSE;
	// 写包头
	ph.dwReserved1 = 1; // 先置为1，最后置为0，这样可以检测是否整个过程顺利完成
	pNewPackFile->Write(&ph, sizeof(PACKHEADER));
	pNewPackFile->Flush();

	dwFilePos += sizeof(PACKHEADER);
	dwCurrOffset = ph.dwBlockOffset;

	string strDir = "";
	string strFileName;

	// 写顶层目录
	m_pPackFile->Seek(dwCurrOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid())
		return FALSE;
	dwCurrOffset = bi.dwNextOffset;
	// 修改块信息
	bi.dwPrevOffset = dwPrevOffset;

	bi.dwNextOffset = dwFilePos + sizeof(BLOCKINFO);
	pNewPackFile->Write(&bi, sizeof(BLOCKINFO));	// 写块信息
	dwPrevOffset = dwFilePos;

	MAPTABLE mt(bi.cbName, dwFilePos);
	MapTable.push_back(mt);
	dwFilePos += sizeof(BLOCKINFO);


	// 写所有的块
	while (dwCurrOffset)
	{
		// 读出块信息
		m_pPackFile->Seek(dwCurrOffset, SEEK_SET);
		m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
		if (!bi.IsValid())
			return FALSE;
		dwCurrOffset = bi.dwNextOffset;
		// 是文件则读出文件内容
		if (bi.dwType == CPackFileSystem::__FILE)
		{
			pFileBuffer = new char[bi.dwCompSize];
			m_pPackFile->Seek(bi.dwOffset);
			m_pPackFile->Read(pFileBuffer, bi.dwCompSize);
		}

		// 修改块信息
		bi.dwPrevOffset = dwPrevOffset;
		if (bi.dwType == CPackFileSystem::__FILE)
		{
			bi.dwOffset = dwFilePos + sizeof(BLOCKINFO);
			bi.dwNextOffset = dwFilePos + sizeof(BLOCKINFO) + bi.dwCompSize;
			pNewPackFile->Write(&bi, sizeof(BLOCKINFO));		// 写块信息
			pNewPackFile->Write(pFileBuffer, bi.dwCompSize);	// 写文件
			SAFE_DELETE(pFileBuffer);
			dwPrevOffset = dwFilePos;

			// 回调
			if (lpfnPackProc)
				lpfnPackProc(1);

			strFileName = strDir;
			strFileName += bi.cbName;
			MAPTABLE mt(strFileName.c_str(), dwFilePos);
			MapTable.push_back(mt);

			dwFilePos += sizeof(BLOCKINFO) + bi.dwCompSize;
		}
		else if (bi.dwType == CPackFileSystem::__DIR)
		{
			bi.dwNextOffset = dwFilePos + sizeof(BLOCKINFO);
			pNewPackFile->Write(&bi, sizeof(BLOCKINFO));	// 写块信息
			dwPrevOffset = dwFilePos;

			if (bi.u.nBlockDepth > dwBlockDepth)
			{
				strDir += bi.cbName;
				strDir += "\\";
				dwBlockDepth = bi.u.nBlockDepth;
			}
			else
			{
				int pos = strDir.length();
				for( int i = 0; i < (dwBlockDepth - bi.u.nBlockDepth + 2); i ++)
				{
					pos = strDir.rfind('\\', pos - 1); // 当退到顶级时, pos = -1
				}
				strDir.erase(strDir.begin() + pos + 1, strDir.end());
				strDir += bi.cbName;
				strDir += "\\";
				dwBlockDepth = bi.u.nBlockDepth;
			}

			// 回调
			if (lpfnPackProc)
				lpfnPackProc(1);
			
			string str = strDir;
			if (*(str.end() - 1) == '\\')
				str.erase(str.end() - 1);
			MAPTABLE mt(str.c_str(), dwFilePos);
			dwFilePos += sizeof(BLOCKINFO);
			MapTable.push_back(mt);
		}
	}

	// 写文件映射表
	pNewPackFile->Write(&MapTable[0], MapTable.size()*sizeof(MAPTABLE));

	// 修改包文件头的某些数据
	//DebugMsgLn("Block count: %d", m_dwBlockCount);
	ph.dwBlockCount = m_dwBlockCount;
	ph.dwMapOffset = dwFilePos;
	ph.dwReserved1 = 0;
	pNewPackFile->SeekToBegin();
	pNewPackFile->Write(&ph, sizeof(PACKHEADER));

	// 文件关闭
	pNewPackFile->Close();
	pNewPackFile->Release();

	return TRUE;
}
//------------------------------------------------------------------------
// 获得相对路径的绝对路径(如果本身是绝对路径,则略过)
inline const char* CPackFileSystem::GetFullPathName(LPCSTR szRelativePathName)
{
	if (szRelativePathName[1] == ':') // 本身就是绝对路径
		return szRelativePathName;

	m_strFullPathName = m_strRootDirectory;
	m_strFullPathName += szRelativePathName;
	return m_strFullPathName.c_str();
}
//------------------------------------------------------------------------
void CPackFileSystem::AddToHash(LPCSTR szName, DWORD dwOffset)
{
	char name[MAX_RELATIVE_PATH_LEN];
	lstrcpy(name, szName);
	ToLower(name);
	m_Hash[name] = dwOffset;
}
//------------------------------------------------------------------------
DWORD CPackFileSystem::GetOffsetFromHash(LPCSTR szName)
{
	char name[MAX_RELATIVE_PATH_LEN];
	lstrcpy(name, szName);
	ToLower(name);
	return m_Hash[name];
}
//------------------------------------------------------------------------
void CPackFileSystem::EraseFromHash(LPCSTR szName)
{
	char name[MAX_RELATIVE_PATH_LEN];
	lstrcpy(name, szName);
	ToLower(name);
	m_Hash.erase(name);
}
//------------------------------------------------------------------------
void CPackFileSystem::ToLower(LPSTR szString)
{
	char* st = szString;
	while (*st != 0)
	{
		char val = *st;

		if ((val >= 'A') && (val <='Z')) 
			*st =(char)(val | 32);
		else if (val == '/')
			*st = (char)'\\';

		st ++;
	}
}
//------------------------------------------------------------------------
// 判断目录是否存在
BOOL CPackFileSystem::DirectoryExist(LPCSTR szDirectoryName)
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
BOOL CPackFileSystem::MakeDirectory(LPCSTR szDirectoryName)
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
DWORD CPackFileSystem::GetLength()
{
	return m_pPackFile->GetLength();
}
//------------------------------------------------------------------------