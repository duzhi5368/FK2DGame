/**
*	created:		2012-7-2   1:35
*	filename: 		AniObj
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/MapView/MapItemView.h"
#include "../../FKGame/Timer/ITimerAxis.h"
#include "../../FKGame/Common/CreatureViewDef.h"
#include "../../FKGame/Common/MapItemDef.h"
#include "ObjPool.h"
#include "ResLoader.h"
#include "Ani.h"
//------------------------------------------------------------------------
// 简单的动画上下文
struct SSimpleAniContext
{
	int nDelay;				// 帧延时，即速度
	int nDelay1;			// 两次完整播放之间的间隔
	BYTE nFrames;			// 帧数
	BYTE nCurFrame;			// 当前是第几帧
	
	SSimpleAniContext()			{Clear();}
	void Clear()			{memset(this,0,sizeof(SSimpleAniContext));}
	void NextFrame()		{if(++nCurFrame>=nFrames)nCurFrame=0;}
	int  CurFrame()			{return (int)nCurFrame;}
	int  Frames()			{return (int)nFrames;}
	void SetCurFrame(int nCurFrame)
	{
		this->nCurFrame = nCurFrame;
	}
};
//------------------------------------------------------------------------
// CAniObj存档格式
struct SAniObjArchive
{
	DWORD	dwResID;
};
//------------------------------------------------------------------------
struct SAniObjCreateParam
{
	int		nDelay;
	int		nDelay1;
	POINT	ptAnchor;
	POINT	ptSortOff1;
	POINT	ptSortOff2;
};
//------------------------------------------------------------------------
// 静态动画图片，无占位块
class CAniObj : public CMapItemView, public ITimerSink
{
	typedef int (CAniObj::*ProcCommand) (DWORD dwParam1, DWORD dwParam2);
	typedef void (CAniObj::*ProcProperty) (BOOL bSetProp, DWORD dwPropVal);
	static ProcCommand	m_Command[AniCmdMaxCommandCount];
	static ProcProperty	m_Property[AniPropMaxPropCount];
protected:
	SSimpleAniContext m_AniContext;	// 动画上下文
	DWORD		m_dwResID;			// 资源ID，用于向资源加载器请求图象资源
	POINT		m_ptAnchor;			// 图像的锚点
	CAni*		m_pPoolNodeObj;		// 图片内存，当此图片移出可见区域时将被丢进缓存里
	POINT		m_ptTile;
	POINT		m_ptSortOff1;		// 排序偏移点1
	POINT		m_ptSortOff2;		// 排序偏移点2

	DWORD		m_dwData;
	BYTE*		m_pCurPal;

public:
	CAniObj();
	virtual ~CAniObj();
	virtual void Close();

	// ITimerSink
	virtual void OnTimer(DWORD dwEventID);

	DWORD GetOffsetID(){return m_dwResID+ioAni;}
	CAni* RequestRes();
	virtual DWORD GetClassID() const{return miAniObj_NoOcc_RGB;}
	virtual void GetOldDirtyRect(list<RECT> &DirtyRectList);
	virtual void SetDirty(ICanvas* pCanvas, const POINT& ptViewTop);
	virtual void OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0);
	virtual void OnDrawTopmostObject(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag=0);
	BOOL OnSerialize(BOOL bWrite,IArchive* pArchive);
	BOOL OnCreateItem(const char* buf, int nLen);
	BOOL CreateItem(SAniObjArchive* pArchive);
	virtual void OnUpdate();
	virtual BOOL OnEnterViewport(BOOL bVisable);
	virtual BOOL HitTest(const RECT& rcScreen);
	virtual RECT GetShowRect();
	virtual POINT GetAnchor() const {return m_ptAnchor;}
	virtual IBitmap* GetBitmap();
    virtual DWORD GetItemData() const {return m_dwData;}
	virtual void SetItemData(DWORD dwData) {m_dwData=dwData;}
	virtual BOOL SetSelected(BOOL bSelected = TRUE, BOOL bHighLight = TRUE);
	virtual void SetTile(POINT& ptTile, IMapView* pMapView);
	virtual POINT GetTile() {return m_ptTile;}
	void UpdatePosInfo(IMapView* pMapView);
	virtual DWORD GetResID() {return m_dwResID;}
	virtual void  Release(){Close();delete this;}

	virtual void AddToTimeAxis();
	virtual void RemoveFromTimeAxis();
	// 命令相关
public:
	virtual int OnCommand(DWORD dwCmdID, DWORD dwParam1,DWORD dwParam2);
	int cmdSetProperty(DWORD dwParam1, DWORD dwParam2);
	int cmdGetProperty(DWORD dwParam1, DWORD dwParam2);
	virtual void propName(BOOL bSetProp, DWORD dwPropVal){}
	virtual void propNameColor(BOOL bSetProp, DWORD dwPropVal){}
};
//------------------------------------------------------------------------
// 不连续的动画(单占位)
class CAniObjDiscrete : public CAniObj
{
protected:
	int		m_nTimerObjType;
public:
	CAniObjDiscrete() : m_nTimerObjType(0) {}
	virtual void Close();
	virtual DWORD GetClassID() const	{return miAniObj_NoOcc_RGBA;}
	virtual void OnTimer(DWORD dwEventID);
	virtual void SetDirty(ICanvas* pCanvas, const POINT& ptViewTop);
	virtual void OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0);
};
//------------------------------------------------------------------------
// 多占位动画
class CAniObjMultiOcc : public CAniObj, public COccupant
{
public:
	virtual DWORD GetClassID() const{return miAniObj_MultiOcc_RGB;}
	virtual DWORD GetOccupantValue(){return 0x0;}
	virtual COccupantTileList* GetOccupantTileList()const {return m_pOccupantTileList;}
	virtual BOOL OnSerialize(BOOL bWrite, IArchive* pArchive)
	{
		BOOL bRtn = CAniObj::OnSerialize(bWrite, pArchive);
		if (bWrite)
			return bRtn;
		return SetOccupantInfo();
	}
	virtual BOOL OnCreateItem(const char* buf, int nLen)
	{
		if (!CAniObj::OnCreateItem(buf, nLen))
			return FALSE;
		return SetOccupantInfo();
	}
	virtual BOOL SetOccupantInfo();
};
//------------------------------------------------------------------------
// 不连续的动画(多占位)
class CAniObjMultiOccDiscrete : public CAniObjMultiOcc
{
protected:
	int		m_nTimerObjType;
public:
	CAniObjMultiOccDiscrete() : m_nTimerObjType(0) {}
	virtual void Close();
	virtual DWORD GetClassID() const	{return miAniObj_MultiOcc_RGBA;}
	virtual void OnTimer(DWORD dwEventID);
	virtual void SetDirty(ICanvas* pCanvas, const POINT& ptViewTop);
	virtual void OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0);
};
//------------------------------------------------------------------------
// 传送门
class CAniObjTransport : public CAniObj
{
protected:
	char	m_szName[MAX_ITEM_NAME_LENGTH];// 名字
	WPixel	m_clrName;	// 名字显示的颜色
	
	WPixel	m_wpMinMapFlagColor;					// 小地图的颜色
public:
	CAniObjTransport()
	{
		m_szName[0]='\0';
		m_clrName=WPixel(255,255,255);		
		m_wpMinMapFlagColor = WPixel(255,255,255);
	}
	virtual void	SetFlagColorOnMinmap(DWORD dwColor);						// 设置在小地图上的颜色
	virtual DWORD	GetFlagColorOnMinmap();										// 取得在小地图上的颜色

	virtual BOOL GetVisibleOnMinmap();											// 取得是否在小地图上显示
	virtual DWORD GetClassID() const{return miAniObj_Transprot;}
	virtual void propName(BOOL bSetProp, DWORD dwPropVal);
	virtual void propNameColor(BOOL bSetProp, DWORD dwPropVal);
	virtual void OnDrawTopmostObject(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag=0);
};
//------------------------------------------------------------------------