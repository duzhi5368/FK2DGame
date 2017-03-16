/**
*	created:		2012-7-1   2:52
*	filename: 		MapArchiveMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/DataArchive/IDataArchive.h"
#include "../../FKFileSystem/IFKFileSystem.h"
//------------------------------------------------------------------------
#define MAX_MAPFILE_SIZE				(8 * 1024 * 1024)	// map文件最大长度
//------------------------------------------------------------------------
class CMapArchiveMgr  : public CDataArchive
{
	BYTE*	m_pMapBuf;
public:
	CMapArchiveMgr();
	virtual ~CMapArchiveMgr();

	IArchive* LoadMap(IFileObject* pFileObj);
	IArchive* BeginSaveMap();
	BOOL EndSaveMap(IFileObject* pFileObj);

	void Close()
	{
		if (m_pMapBuf)
		{
			delete m_pMapBuf;
			m_pMapBuf = NULL;
		}
	}
};
//------------------------------------------------------------------------