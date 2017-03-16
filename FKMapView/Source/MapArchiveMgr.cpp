/**
*	created:		2012-7-1   2:53
*	filename: 		MapArchiveMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/MapArchiveMgr.h"
#include "../Include/MapViewCommon.h"
//------------------------------------------------------------------------
CMapArchiveMgr::CMapArchiveMgr()
{
	m_pMapBuf = NULL;
}
//------------------------------------------------------------------------
CMapArchiveMgr::~CMapArchiveMgr()
{
	Close();
}
//------------------------------------------------------------------------
// 加载一个地图
IArchive * CMapArchiveMgr::LoadMap(IFileObject* pFileObj)
{
	if (!pFileObj) return NULL;

	Close();

	if (!pFileObj->Open())
	{
		return NULL;
	}

	int size = pFileObj->GetLength();
	if (size <= 0 || size > MAX_MAPFILE_SIZE)
	{
		return NULL;
	}
	if (m_pMapBuf)
		delete[] m_pMapBuf;
	m_pMapBuf = new BYTE[size];
	if (!m_pMapBuf)	
		return NULL;

	if (size != (int)pFileObj->Read(m_pMapBuf, size))
	{
		return NULL;
	}

	Init(m_pMapBuf, size);
	return static_cast<IArchive*>(this);
}
//------------------------------------------------------------------------
IArchive* CMapArchiveMgr::BeginSaveMap()
{
	Close();
	m_pMapBuf = new BYTE[MAX_MAPFILE_SIZE];
	if (m_pMapBuf == NULL)
		return NULL;
	Init(m_pMapBuf, MAX_MAPFILE_SIZE);
	return this;
}
//------------------------------------------------------------------------
BOOL CMapArchiveMgr::EndSaveMap(IFileObject* pFileObj)
{
	if (!pFileObj) 
		return FALSE;

	if (!pFileObj->Open("wb"))
	{
		TraceLn("CMapArchiveMgr::EndSaveMap(): can not open file : %s", pFileObj->GetFilePath());
		return FALSE;
	}
	pFileObj->Write(m_pMapBuf, GetBufferOffset());
	return TRUE;
}
//------------------------------------------------------------------------