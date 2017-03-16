/**
*	created:		2012-6-15   22:12
*	filename: 		FileSysCommon
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#ifdef _DEBUG
#define __USE_MALLOC
#endif
//------------------------------------------------------------------------
#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <vector>
#include <map>
#include "../IFKFileSystem.h"
//------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------
class CFileSystem;
class CStdFileObject;
class CPackFileObject;
//------------------------------------------------------------------------
void DebugPrintLn(LPCSTR lpszFormat, ...);
//------------------------------------------------------------------------
#ifdef TraceLn
#undef TraceLn
#endif
#define TraceLn	DebugPrintLn

#ifdef DebugMsgLn
#undef DebugMsgLn
#endif
#define DebugMsgLn ((void*)0)
//------------------------------------------------------------------------
#if !defined(ASSERT)
#if defined(_DEBUG)
#define ASSERT(f) {if (!(f))__asm int 3}
#else
#define ASSERT(f)	 ((void*)0)
#endif
#endif
//------------------------------------------------------------------------
#define SAFE_DELETE(a) { if (a) { delete a; a=NULL; } }
#define SAFE_RELEASE(a) { if (a) { a->Release(); a=NULL; } }
//------------------------------------------------------------------------
// 块信息头(这里,块泛指目录和文件)
typedef struct _BLOCKINFO				// 72bytes
{
	DWORD	dwIdentify;					// 块信息头标识( FKBK // BLCK = 1262701634)
	BOOL	bIsValid;					// 该块是否还有效(是否标记为删除状态)
	union{
		BOOL	bCompress;				// 该块是否压缩(用于文件)
		int		nBlockDepth;			// 块深度(用于目录层次结构)
	} u;
	char	cbName[MAX_BLOCK_NAME_LEN];	// 块名(目录名或文件名)(<32)
	DWORD	dwBlockSize;				// 该块原始尺寸(目录时为0)
	DWORD	dwCompSize;					// 该块压缩后尺寸(目录时为0)
	DWORD	dwOffset;					// 该块数据的偏移量(如果块更新,则指向新位置;目录时为0)
	DWORD	dwPrevOffset;				// 前一个块的偏移量
	DWORD	dwNextOffset;				// 下一个块的偏移量
	DWORD	dwType;						// 块类型(是目录还是文件)
	DWORD	dwVersion;					// 版本

	_BLOCKINFO(){memset(this, 0, sizeof(_BLOCKINFO));dwIdentify='FKFS';}
	BOOL IsValid()
	{
		if (dwIdentify == 'FKFS')
			return TRUE;
		TraceLn("错误：当前文件缓冲区不是一个有效的FreeKnightBlock头结构！");
		return FALSE;
	}
	void SetBlockName(LPCSTR szName)
	{
		lstrcpy(cbName, szName);
		if (lstrlen(szName) >= MAX_BLOCK_NAME_LEN)
		{
			TraceLn("错误：文件块 %s 名称大小过长！限制32字节内", szName);
			cbName[MAX_BLOCK_NAME_LEN - 1] = '\0';
		}
	}
} BLOCKINFO;
//------------------------------------------------------------------------
// 包文件头
typedef struct _PACKHEADER	// 28bytes
{
	DWORD	dwIdentify;		// 标识( FKPK //PACK = 1262698832)
	DWORD	dwVersion;		// 版本(0.0.0.1)
	DWORD	dwBlockCount;	// 块数
	DWORD	dwBlockOffset;	// 块偏移
	DWORD	dwMapOffset;	// 映射表偏移
	DWORD	dwReserved1;	// 保留1
	DWORD	dwReserved2;	// 保留2

	_PACKHEADER(){memset(this, 0, sizeof(_PACKHEADER));dwIdentify='KPKF';}
	BOOL IsValid()
	{
		if (dwIdentify == 'KPKF')
			return TRUE;
		TraceLn("错误：当前数据不是一个有效的FreeKnightPackage头结构！");
		return FALSE;
	}
} PACKHEADER;
//------------------------------------------------------------------------
// 文件映射表结构
typedef struct _MAPTABLE	// 64+4 btyes
{
	char	name[MAX_RELATIVE_PATH_LEN];
	DWORD	offset;

	_MAPTABLE()
	{
		memset(this, 0, sizeof(_MAPTABLE));
	}
	_MAPTABLE(LPCSTR szName, DWORD dwOffset)
	{
		memset(this, 0, sizeof(_MAPTABLE));
		assert(lstrlen(szName) < MAX_RELATIVE_PATH_LEN);
		lstrcpy(name, szName);
		offset = dwOffset;
	}
} MAPTABLE;
//------------------------------------------------------------------------