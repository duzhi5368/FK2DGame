/**
*	created:		2012-7-1   5:00
*	filename: 		MiniMap
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		小地图管理
*/
//------------------------------------------------------------------------
#include "../Include/MiniMap.h"
#include "../Include/MiniObjPic.h"
#include "../Include/MapViewCommon.h"
//------------------------------------------------------------------------
#define MINIOFFX	11
#define MINIOFFY	9
//------------------------------------------------------------------------
CMiniMap::ProcDrawObj CMiniMap::m_bufDrawObjFuncs[miMaxClassID] =
{
	NULL,							// 0
	NULL,							// 1
	NULL,							// 2
	NULL,							// 3
	NULL,							// 4
	NULL,							// 5
	NULL,							// 6
	NULL,							// 7
	NULL,							// 8
	NULL,							// 9
	NULL,							// 10
	&CMiniMap::DrawCreatureFlags,				// 11 NPC和怪物
	&CMiniMap::DrawCreatureFlags,				// 12 角色对象
	NULL,							// 13
	NULL,							// 14
	NULL,							// 15
	NULL,							// 16
	NULL,							// 17
	NULL,							// 18
	&CMiniMap::DrawOreFlags,					// 19 地上物（矿石）
	NULL,							// 20
	NULL,							// 21
	NULL,							// 22
	NULL,							// 23
	NULL,							// 24
	NULL,							// 25
	NULL,							// 26
	NULL,							// 27
	NULL,							// 28
	NULL,							// 29
	NULL,							// 30
	NULL,							// 31
	&CMiniMap::DrawOreFlags,					// 32 传送门
	NULL,							// 33
	NULL,							// 34
	NULL,							// 35
};
//------------------------------------------------------------------------
CMiniMap::CMiniMap()
{
	m_bVisible = FALSE;	
	SetRectEmpty(&m_rcView);
	m_nOldX = m_nOldY = 0;
	m_nMapWidth = m_nMapHeight = m_nMiniWidth = m_nMiniHeight = 0;
	m_ptMainPlayer.x = m_ptMainPlayer.y = 0;
	m_nMiniMapRate = 20;
	m_bMiniMapBigOrSmall = FALSE;

	if (!g_MiniObjPic.IsOK())
	{
		g_MiniObjPic.Init();
	}
}
//------------------------------------------------------------------------
CMiniMap::~CMiniMap()
{
	Close();
}
//------------------------------------------------------------------------
BOOL CMiniMap::Create(int nMapWidth, int nMapHeight, LPRECT lprcView, 
					  LPCSTR szMiniMapJpgFileDir, int nMiniMapRate, IFileObject* pTinyMapFile)
{
	SetVisible(TRUE);
	SetViewport(lprcView);
	m_nMiniMapRate = nMiniMapRate;

	return SetMapData(nMapWidth, nMapHeight, szMiniMapJpgFileDir);
}
//------------------------------------------------------------------------
// 设置缩微地图的视区，可以在地图上设置，也可以在某个窗口上设置
void CMiniMap::SetViewport(LPRECT lprcView)
{
	if(m_bMiniMapBigOrSmall)
	{
		m_rcView.top = MINIMAP_BIG_TOP_IN_SCREEN;
		m_rcView.bottom = MINIMAP_BIG_TOP_IN_SCREEN + MINIMAP_BIG_HEIGHT;
		m_rcView.left = MINIMAP_BIG_LEFT_IN_SCREEN;
		m_rcView.right = MINIMAP_BIG_LEFT_IN_SCREEN + MINIMAP_BIG_WIDTH;
	}
	else
	{
		m_rcView.top = MINIMAP_SMALL_TOP_IN_SCREEN;
		m_rcView.bottom = MINIMAP_SMALL_TOP_IN_SCREEN + MINIMAP_SMALL_HEIGHT;
		m_rcView.left = MINIMAP_SMALL_LEFT_IN_SCREEN;
		m_rcView.right = MINIMAP_SMALL_LEFT_IN_SCREEN + MINIMAP_SMALL_WIDTH;
	}

	if (m_MiniMapView.GetViewWidth() != 0)
	{
		int nMaxSize = (m_nMiniWidth > m_nMiniHeight) ? m_nMiniHeight : m_nMiniWidth;
		if (m_rcView.right - m_rcView.left > nMaxSize)
			m_rcView.left = m_rcView.right - nMaxSize;
		if (m_rcView.bottom - m_rcView.top > nMaxSize)
			m_rcView.bottom = m_rcView.top + nMaxSize;

		POINT ptOffset = *(POINT*)&m_rcView;
		m_MiniMapView.SetOffset(ptOffset);
		m_MiniMapView.SizeViewport(m_rcView.right - m_rcView.left, m_rcView.bottom - m_rcView.top);
	}
}
//------------------------------------------------------------------------
// 设置小地图大小
void CMiniMap::SetMiniMapBigOrSmall(BOOL bBig)
{
	m_bMiniMapBigOrSmall = bBig;
	if(m_bMiniMapBigOrSmall)
	{
		m_rcView.top = MINIMAP_BIG_TOP_IN_SCREEN;
		m_rcView.bottom = MINIMAP_BIG_TOP_IN_SCREEN + MINIMAP_BIG_HEIGHT;
		m_rcView.left = MINIMAP_BIG_LEFT_IN_SCREEN;
		m_rcView.right = MINIMAP_BIG_LEFT_IN_SCREEN + MINIMAP_BIG_WIDTH;
	}
	else
	{
		m_rcView.top = MINIMAP_SMALL_TOP_IN_SCREEN;
		m_rcView.bottom = MINIMAP_SMALL_TOP_IN_SCREEN + MINIMAP_SMALL_HEIGHT;
		m_rcView.left = MINIMAP_SMALL_LEFT_IN_SCREEN;
		m_rcView.right = MINIMAP_SMALL_LEFT_IN_SCREEN + MINIMAP_SMALL_WIDTH;
	}

	if (m_MiniMapView.GetViewWidth() != 0)
	{
		POINT ptOffset = *(POINT*)&m_rcView;
		m_MiniMapView.SetOffset(ptOffset);
		m_MiniMapView.SizeViewport(m_rcView.right - m_rcView.left, m_rcView.bottom - m_rcView.top);
	}
}
//------------------------------------------------------------------------
// 获取小地图大小
BOOL CMiniMap::GetMiniMapBigOrSmall()
{
	return m_bMiniMapBigOrSmall;
}
//------------------------------------------------------------------------
// 设置当前地图的相关数据，当改变地图后，需要重新设置这些数据
BOOL CMiniMap::SetMapData(int nMapWidth, int nMapHeight, LPCSTR szMiniMapJpgFileDir)
{
	ASSERT(!IsRectEmpty(&m_rcView));
	m_nMapWidth = nMapWidth;
	m_nMapHeight = nMapHeight;
	m_nMiniWidth = m_nMapWidth / m_nMiniMapRate;
	m_nMiniHeight = m_nMapHeight / m_nMiniMapRate;

	POINT ptOffset = *(POINT*)&m_rcView;
	RECT rc;
	rc.left = rc.top = 0;
	rc.right = m_rcView.right - m_rcView.left;
	rc.bottom = m_rcView.bottom - m_rcView.top;
	m_MiniMapView.SetOffset(ptOffset);
	return m_MiniMapView.Create(m_nMiniWidth, m_nMiniHeight, 
		160,120, &rc, szMiniMapJpgFileDir, NULL);
}
//------------------------------------------------------------------------
// 渲染缩微地图
void CMiniMap::OnDraw(ICanvas* pCanvas, int nMainPlayerX, int nMainPlayerY, BOOL bForceTrans)
{
	if (m_nMiniWidth == 0)
		return;

	int nViewWidth = m_rcView.right-m_rcView.left;
	int nViewHeight = m_rcView.bottom-m_rcView.top;
	int nHalfViewWidth = nViewWidth>>1;
	int nHalfViewHeight = nViewHeight>>1;
	int xSrc = (nMainPlayerX/m_nMiniMapRate) - nHalfViewWidth;
	int ySrc = (nMainPlayerY/m_nMiniMapRate) - nHalfViewHeight;
	int moffx = nHalfViewWidth, moffy = nHalfViewHeight;	// 主角偏移

	if (xSrc < 0)
	{
		moffx += xSrc;
		xSrc = 0;
	}

	if (xSrc > m_nMiniWidth-nViewWidth)
	{
		moffx += xSrc-m_nMiniWidth+nViewWidth;
		xSrc = m_nMiniWidth-nViewWidth;
	}

	if (ySrc < 0)
	{
		moffy += ySrc;
		ySrc = 0;
	}
	
	if (ySrc > m_nMiniHeight-nViewHeight)
	{
		moffy += ySrc-m_nMiniHeight+nViewHeight;
		ySrc = m_nMiniHeight-nViewHeight;
	}
	m_MiniMapView.ScrollViewport(xSrc - m_nOldX, ySrc - m_nOldY);
	int nAlpha = m_MiniMapView.GetAlpha();
	if (bForceTrans && nAlpha == 256)
	{
		m_MiniMapView.SetAlpha(128);
		m_MiniMapView.OnDraw(pCanvas);
		m_MiniMapView.SetAlpha(256);
	}
	else
		m_MiniMapView.OnDraw(pCanvas);

	m_ptMainPlayer.x = moffx, m_ptMainPlayer.y = moffy;

	m_nOldX = xSrc;
	m_nOldY = ySrc;
}
//------------------------------------------------------------------------
void CMiniMap::CalcMousePos(POINT& pt, POINT& ptMouse)
{
	int x = m_MiniMapView.GetViewTopX();
	int y = m_MiniMapView.GetViewTopY();
	x += pt.x - m_rcView.left;
	y += pt.y - m_rcView.top;
	ptMouse.x = x*m_nMiniMapRate;
	ptMouse.y = y*m_nMiniMapRate;
}
//------------------------------------------------------------------------
void CMiniMap::DrawRectangleFlag(ICanvas* pCanvas, int xPos, int yPos, WPixel clr)
{
	int x = (xPos/m_nMiniMapRate) - m_nOldX;
	int y = (yPos/m_nMiniMapRate) - m_nOldY;
	drawFlags(pCanvas, x, y, clr);
}
//------------------------------------------------------------------------
void CMiniMap::Close()
{
}
//------------------------------------------------------------------------
void CMiniMap::drawFlags(ICanvas* pCanvas, int x, int y, WPixel clr)
{
	POINT pt = {m_rcView.left + x, m_rcView.top + y};
	if (!PtInRect(&m_rcView, pt))
		return;

	RECT rc;
	if(m_bMiniMapBigOrSmall)
	{
		rc.left = m_rcView.left+x - MINIMAP_BIG_LEFT_IN_SCREEN;
		rc.top = m_rcView.top+y - MINIMAP_BIG_TOP_IN_SCREEN;
		rc.right = m_rcView.left+x+3 - MINIMAP_BIG_LEFT_IN_SCREEN;
		rc.bottom = m_rcView.top+y+3 - MINIMAP_BIG_TOP_IN_SCREEN;
	}
	else
	{
		rc.left = m_rcView.left+x - MINIMAP_SMALL_LEFT_IN_SCREEN;
		rc.top = m_rcView.top+y - MINIMAP_SMALL_TOP_IN_SCREEN;
		rc.right = m_rcView.left+x+3 - MINIMAP_SMALL_LEFT_IN_SCREEN;
		rc.bottom =	m_rcView.top+y+3 - MINIMAP_SMALL_TOP_IN_SCREEN;
	}

	pCanvas->DrawBox(rc, clr);
	InflateRect(&rc, 1, 1);
	drawRect(pCanvas, rc, 0);

	POINT ptFrom,ptTo;
	ptFrom.x = 0;
	ptFrom.y = 0;
	ptTo.x = 0;
	ptTo.y = 0;
	pCanvas->DrawLine(ptFrom, ptTo, clr);
}
//------------------------------------------------------------------------
void CMiniMap::drawRect(ICanvas* pCanvas, RECT& rc, WPixel clr)
{
	if (!pCanvas) return;
	POINT ptFrom,ptTo;
	ptFrom.x=rc.left, ptFrom.y=rc.top, ptTo.x=rc.right-1, ptTo.y=rc.top;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 上横
	ptFrom.x=rc.right-1, ptFrom.y=rc.top, ptTo.x=rc.right-1, ptTo.y=rc.bottom;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 右竖
	ptFrom.x=rc.right-1, ptFrom.y=rc.bottom-1, ptTo.x=rc.left, ptTo.y=rc.bottom-1;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 下横
	ptFrom.x=rc.left, ptFrom.y=rc.bottom-1, ptTo.x=rc.left, ptTo.y=rc.top;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 左竖
}
//------------------------------------------------------------------------
void CMiniMap::DrawMainActor(ICanvas* pCanvas, WPixel wpColor)
{
	int left = m_ptMainPlayer.x;
	int top = m_ptMainPlayer.y;	
	DWORD dwPICID = PIC_SELF ;
	if (!g_MiniObjPic.DrawPIC(pCanvas, left, top, dwPICID) )
	{
		WPixel color = g_MiniObjPic.GetColor(dwPICID);
		drawFlags(pCanvas, m_ptMainPlayer.x, m_ptMainPlayer.y, wpColor);
	}
}
//------------------------------------------------------------------------
void CMiniMap::DrawObjByClassID(ICanvas* pCanvas, int xPos, int yPos, DWORD dwOccVal, DWORD dwClassID, WPixel wpColor)
{
	if (dwClassID >= miMaxClassID)	// 当前未支持的命令码
	{
		TraceLn("CMiniMap::DrawObjByClassID ,Unknow classID=%d",dwClassID);
		return;
	}
	if (m_bufDrawObjFuncs[dwClassID] == NULL)
		return;
	
	(this->*m_bufDrawObjFuncs[dwClassID])(pCanvas, xPos, yPos, dwOccVal, wpColor);
}
//------------------------------------------------------------------------
void CMiniMap::DrawCreatureFlags(ICanvas* pCanvas, int xPos, int yPos, DWORD dwOccVal, WPixel wpColor)
{
	if (dwOccVal == 0) return;

	int left = xPos;
	int top = yPos;	
	__CalPicPos(left, top);	
	DWORD dwPICID = wpColor.red ;
	if (!g_MiniObjPic.DrawPIC(pCanvas, left, top, dwPICID) )
	{
		WPixel color = g_MiniObjPic.GetColor(dwPICID);
		DrawRectangleFlag(pCanvas, xPos, yPos, color);
	}
}
//------------------------------------------------------------------------
void CMiniMap::DrawOreFlags(ICanvas* pCanvas, int xPos, int yPos, DWORD dwOccVal, WPixel wpColor)
{
	int left = xPos;
	int top = yPos;	
	__CalPicPos(left, top);	
	DWORD dwPICID = wpColor.red ;
	if (!g_MiniObjPic.DrawPIC(pCanvas, left, top, dwPICID) )
	{
		WPixel color = g_MiniObjPic.GetColor(dwPICID);
		DrawRectangleFlag(pCanvas, xPos, yPos, color);
	}		
}
//------------------------------------------------------------------------
void CMiniMap::__CalPicPos(int &xPos, int &yPos)
{
	int x = (xPos/m_nMiniMapRate) - m_nOldX;
	int y = (yPos/m_nMiniMapRate) - m_nOldY;		
	if(m_bMiniMapBigOrSmall)
	{
		xPos = m_rcView.left+x - MINIMAP_BIG_LEFT_IN_SCREEN;
		yPos = m_rcView.top+y - MINIMAP_BIG_TOP_IN_SCREEN;		
	}
	else
	{
		xPos = m_rcView.left+x - MINIMAP_SMALL_LEFT_IN_SCREEN;
		yPos = m_rcView.top+y - MINIMAP_SMALL_TOP_IN_SCREEN;		
	}	
}
//------------------------------------------------------------------------