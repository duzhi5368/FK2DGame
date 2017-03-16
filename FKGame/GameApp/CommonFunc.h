/**
*	created:		2012-6-21   22:23
*	filename: 		CommonFunc
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../MapView/IMapView.h"
#include "../Controller/IController.h"
//------------------------------------------------------------------------
class CCommonFunc  
{
public:
	// 关闭游戏世界
	void		CloseGameWorld(void);
	// 创建新游戏世界
	BOOL		CreateNewWorld(int nMapID, POINT ptWorld);
	// 载入地图
	BOOL		LoadMap(int nMapID, POINT ptWorld);
	// 载入一张图片
	IBitmap *	LoadBitmap(const char *szFileName);
	// 载入地图
	char*		LoadFile(const char *szFileName);
	// 创建
	BOOL		Create(void);
	// 关闭
	void		Close(void);
	// 小退,清掉冗余资源
	void		ClearMemoryResource(void);
public:
	CCommonFunc(void);
	~CCommonFunc(void);
public:
	CMapViewHelper		m_MapViewHelper;
	CControllerHelper	m_ControllerHelper;
	int					m_nMapID;
};
//------------------------------------------------------------------------
extern void			MsgBox(LPCSTR szMsg,LPCSTR szCaption = "FKWarning", DWORD uType = MB_OK | MB_ICONWARNING);
extern BOOL			GetCursorPointTile(POINT& ptTile);
//------------------------------------------------------------------------