/**
*	created:		2012-7-1   2:59
*	filename: 		MiniMap
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		小地图管理
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKCanvas/IFKCanvas.h"
#include "../../FKFileSystem/IFKFileSystem.h"
#include "../../FKGame/SceneObj/IItemClassFactory.h"
#include "../../FKGame/Common/MapItemDef.h"
#include "../Include/MiniMapView.h"
//------------------------------------------------------------------------
#define		MINIMAP_SMALL_WIDTH				119				// 小地图（小型）的宽
#define		MINIMAP_SMALL_HEIGHT			119				// 小地图（小型）的高
//------------------------------------------------------------------------
#define		MINIMAP_SMALL_LEFT_IN_SCREEN	880			// 小地图（小型）在屏幕中的x坐标
#define		MINIMAP_SMALL_TOP_IN_SCREEN		25				// 小地图（小型）在屏幕中的y坐标
//------------------------------------------------------------------------
#define		MINIMAP_BIG_WIDTH				193				// 小地图（大型）的宽
#define		MINIMAP_BIG_HEIGHT				196				// 小地图（大型）的高
//------------------------------------------------------------------------
#define		MINIMAP_BIG_LEFT_IN_SCREEN		799				// 小地图（大型）在屏幕中的x坐标
#define		MINIMAP_BIG_TOP_IN_SCREEN		23				// 小地图（大型）在屏幕中的y坐标
//------------------------------------------------------------------------
class CMiniMap
{
	typedef void (CMiniMap::*ProcDrawObj)(ICanvas* pCanvas, int xPos, int yPos, DWORD dwOccVal, WPixel wpColor);
	static ProcDrawObj m_bufDrawObjFuncs[miMaxClassID];

	BOOL		m_bVisible;			// 小地图是否显示
	RECT		m_rcView;			// 小地图视图矩形
	int			m_nOldX, m_nOldY;	// 主角的旧位置
	int			m_nMapWidth;		// 地图的宽度
	int			m_nMapHeight;		// 地图的高度
	int			m_nMiniWidth;		// 小地图的宽高
	int			m_nMiniHeight;
	int			m_nMiniMapRate;		// 小地图的缩放比
	POINT		m_ptMainPlayer;		// 主角位置
	CMiniMapView	m_MiniMapView;	// 小地图地表视图
	BOOL		m_bMiniMapBigOrSmall;	// 小地图是大还是小，FALSE是小

public:
	ICanvasHelper	m_CanvasHelper;
	CMiniMap();
	virtual ~CMiniMap();

	BOOL Create(int nMapWidth, int nMapHeight, LPRECT lprcView, LPCSTR szMiniMapJpgFileDir, int nMiniMapRate, IFileObject* pTinyMapFile=NULL);
	// 设置缩微地图的视区，可以在地图上设置，也可以在某个窗口上设置
	void SetViewport(LPRECT lprcView);
	// 设置当前地图的相关数据，当改变地图后，需要重新设置这些数据
	BOOL SetMapData(int nMapWidth, int nMapHeight, LPCSTR szMiniMapJpgFileDir);
	// 设置缩微地图的可见性
	void SetVisible(BOOL bVisible) {m_bVisible = bVisible;}
	// 设置小地图的半透明值
	void SetMiniMapTransparent(int nAlpha)	{m_MiniMapView.SetAlpha(nAlpha);}
	
	// 设置小地图大小
	void SetMiniMapBigOrSmall(BOOL bBig);
	// 获取小地图大小
	BOOL GetMiniMapBigOrSmall();

	// 渲染缩微地图
	void OnDraw(ICanvas* pCanvas, int nMainPlayerX, int nMainPlayerY, BOOL bForceTrans);
	// 绘制其他各种各样的标志(xPos,yPos传入世界坐标)
	void DrawRectangleFlag(ICanvas* pCanvas, int xPos, int yPos, WPixel clr);
	// 游戏退出前释放缩微地图
	void Release()			{delete this;}

	BOOL IsVisible() {return m_bVisible;}

	void DrawObjByClassID(ICanvas* pCanvas, int xPos, int yPos, DWORD dwOccVal, DWORD dwClassID, WPixel wpColor);

	int GetViewWidth()	{return m_rcView.right - m_rcView.left;}
	int GetViewHeight()	{return m_rcView.bottom - m_rcView.top;}
	int GetViewLeft()	{return m_rcView.left;}
	int GetViewTop()	{return m_rcView.top;}

	void CalcMousePos(POINT& pt, POINT& ptMouse);

	void Close();
	void DrawMainActor(ICanvas* pCanvas, WPixel wpColor);
private:
	void drawFlags(ICanvas* pCanvas, int x, int y, WPixel clr);
	void drawRect(ICanvas* pCanvas, RECT& rc, WPixel clr);
	void DrawCreatureFlags(ICanvas* pCanvas, int xPos, int yPos, DWORD dwOccVal, WPixel wpColor);
	void DrawOreFlags(ICanvas* pCanvas, int xPos, int yPos, DWORD dwOccVal, WPixel wpColor);

	void __CalPicPos(int &xPos, int &yPos);  //计算图片的显示位置（左上）
};
//------------------------------------------------------------------------