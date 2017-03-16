/**
*	created:		2012-7-2   1:41
*	filename: 		StaticObj
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "ResLoader.h"
#include "ObjPool.h"
#include "Pic.h"
#include "Magic.h"
#include "AniGroup.h"
#include "../../FKGame/MapInfo/Tile.h"
#include "../../FKGame/Common/CreatureViewDef.h"
#include "../../FKGame/MapView/MapItemView.h"
#include "../../FKGame/SceneObj/IItemClassFactory.h"
//------------------------------------------------------------------------
// CStaticObj存档格式
struct SStaticObjArchive
{
	DWORD	dwResID;
};
//------------------------------------------------------------------------
// CStaticObjActEx存档格式
struct SStaticObjActExArchive
{
	int		nState;			// 当前状态
	int		nDir;			// 当前方向
};
//------------------------------------------------------------------------
// CStaticObjArticle存档格式
struct SStaticObjArticleArchive
{
	DWORD	dwResIDBox;
	DWORD	dwResIDStar;
};
//------------------------------------------------------------------------
struct SStaticObjCreateParam
{
	POINT	ptAnchor;
	POINT	ptSortOff1;
	POINT	ptSortOff2;
};
//------------------------------------------------------------------------
// 无占位块静态图片
class CStaticObj : public CMapItemView
{
	typedef int (CStaticObj::*ProcCommand) (DWORD dwParam1, DWORD dwParam2);
	typedef void (CStaticObj::*ProcProperty) (BOOL bSetProp, DWORD dwPropVal);
	static ProcCommand	m_Command[AtcCmdMaxCommandCount];
	static ProcProperty	m_Property[AtcPropMaxPropCount];
	
public:
	DWORD		m_dwResID;		// 图象ID，用于向资源管理器获取图片
	POINT		m_ptAnchor;		// 图像的锚点
	CPic*		m_pPoolNodeObj;	// 图片内存，当此图片移出可见区域时将被丢进缓存里
	POINT		m_ptTile;
	POINT		m_ptSortOff1;	// 排序偏移点1
	POINT		m_ptSortOff2;	// 排序偏移点2

	DWORD		m_dwData;
	BYTE*		m_pCurPal;		// 当前调色板指针

	BOOL		m_bVisibleOnMinmap;						// 是否在小地图上显示
	WPixel		m_wpMinMapFlagColor;					// 小地图的颜色
public:
	CStaticObj();
	virtual ~CStaticObj();
	virtual void Close();

	virtual void	SetFlagColorOnMinmap(DWORD dwColor);						// 设置在小地图上的颜色
	virtual DWORD	GetFlagColorOnMinmap();										// 取得在小地图上的颜色

	
	virtual void SetVisibleOnMinmap(BOOL bVisible);								// 设置是否在小地图上显示
	virtual BOOL GetVisibleOnMinmap();											// 取得是否在小地图上显示

	DWORD GetOffsetID()				{return m_dwResID+ioStatic;}
	virtual CPic* RequestRes();
	virtual DWORD GetClassID() const{return miStaticObj_NoOcc_NoTrans;}
	virtual DWORD GetOccupantValue(){return 0x0;}
	virtual DWORD GetLayerValue()	{return milvLowest-3;}
	virtual void GetOldDirtyRect(list<RECT> &DirtyRectList);
	virtual void SetDirty(ICanvas* pCanvas, const POINT& ptViewTop);
	virtual void OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0);
	virtual void OnDrawTopmostObject(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag=0);
	BOOL OnSerialize(BOOL bWrite, IArchive* pArchive);
	virtual BOOL OnCreateItem(const char* buf, int nLen);
	virtual BOOL CreateItem(SStaticObjArchive* pArchive);
	virtual void OnUpdate();
	virtual BOOL OnEnterViewport(BOOL bVisable);
	virtual BOOL HitTest(const RECT& rcScreen);
	virtual RECT GetShowRect();
	virtual POINT GetAnchor() const			{return m_ptAnchor;}
	virtual IBitmap* GetBitmap();
    virtual DWORD GetItemData() const		{return m_dwData;}
	virtual void SetItemData(DWORD dwData)	{m_dwData=dwData;}
	virtual BOOL SetSelected(BOOL bSelected = TRUE, BOOL bHighLight = TRUE);
	virtual DWORD GetResID();

	virtual void ChangePal(int nNewPalType);
	virtual void RestorePal();
	BYTE* GetCurPal(){
		if (m_pCurPal)
			return m_pCurPal;
		CPic* p = RequestRes();
		if (p)
			return p->GetPal();
		return NULL;
	}
	virtual void SetTile(POINT& ptTile, IMapView* pMapView);
	virtual POINT GetTile() {return m_ptTile;}
	void UpdatePosInfo(IMapView* pMapView);
	virtual void  Release(){Close();delete this;}

	// 命令相关
public:
	virtual int OnCommand(DWORD dwCmdID, DWORD dwParam1,DWORD dwParam2);
	int cmdSetProperty(DWORD dwParam1, DWORD dwParam2);
	int cmdGetProperty(DWORD dwParam1, DWORD dwParam2);
	int cmdChangePal(DWORD dwParam1, DWORD dwParam2);
	virtual int cmdThrow(DWORD dwParam1, DWORD dwParam2) {return 0;}
	virtual void propName(BOOL bSetProp, DWORD dwPropVal) {}
	virtual void propShowStar(BOOL bSetProp, DWORD dwPropVal) {}
	virtual void propShowName(BOOL bSetProp, DWORD dwPropVal) {}
	virtual void propNameColor(BOOL bSetProp, DWORD dwPropVal) {}
	virtual void propBkImage(BOOL bSetProp, DWORD dwPropVal) {}
};
//------------------------------------------------------------------------
// 特殊块(可以是无占位、单占位、多占位)
class CStaticObjSpecificBlock : public CStaticObj, public COccupant
{
	int m_nOccupantOption;
public:
	CStaticObjSpecificBlock() : m_nOccupantOption(0){}
	virtual DWORD GetClassID() const	{return miSpecificBlock;}
	virtual DWORD GetOccupantValue() {
			return 0x0;
	}
	virtual COccupantTileList* GetOccupantTileList()const {
		if (m_nOccupantOption < 2)
			return NULL;
		return m_pOccupantTileList;
	}
	virtual DWORD GetLayerValue()	{return milvSpecificBlock;}
	virtual BOOL OnSerialize(BOOL bWrite, IArchive* pArchive)
	{
		BOOL bRtn = CStaticObj::OnSerialize(bWrite, pArchive);
		if (bWrite)
			return bRtn;
		return SetOccupantInfo();
	}
	virtual BOOL SetOccupantInfo();
};
//------------------------------------------------------------------------
// 特殊块(可以是无占位、单占位、多占位)
class CStaticObjSpecificBlockDown : public CStaticObjSpecificBlock
{
public:
	virtual DWORD GetClassID() const	{return miSpecificBlockDown;}
	virtual DWORD GetLayerValue()		{return milvSpecificBlock-1;}
};
//------------------------------------------------------------------------
// 物品栏物品
class CStaticObjPicture : public CStaticObj
{
public:
	virtual DWORD GetClassID() const{return miStaticObj_Picture;}
};
//------------------------------------------------------------------------
class CStaticObjTrans : public CStaticObj
{
public:
	virtual DWORD GetClassID() const{return miStaticObj_NoOcc_Trans;}
};
//------------------------------------------------------------------------
class CStaticObjSingleOcc: public CStaticObj
{
public:
	virtual DWORD GetClassID() const{return miStaticObj_SingleOcc_NoTrans;}
	virtual DWORD GetOccupantValue(){return CTile::maskBlock_Item|CTile::maskDiffuseBlock_Item;}
	virtual DWORD GetLayerValue()	{return milvMidle;}
};
//------------------------------------------------------------------------
class CStaticObjSingleOccTrans: public CStaticObjSingleOcc
{
public:
	virtual DWORD GetClassID() const{return miStaticObj_SingleOcc_Trans;}	
};
//------------------------------------------------------------------------
// 多占位块的静态对象，如房子、大树等
class CStaticObjMultiOcc : public CStaticObj, public COccupant
{
public:
	virtual DWORD GetClassID() const{return miStaticObj_MultiOcc_NoTrans;}
	virtual DWORD GetOccupantValue(){return 0x0;}
	virtual DWORD GetLayerValue()	{return milvMidle;}
	virtual COccupantTileList* GetOccupantTileList()const {return m_pOccupantTileList;}
	virtual BOOL OnSerialize(BOOL bWrite, IArchive* pArchive)
	{
		BOOL bRtn = CStaticObj::OnSerialize(bWrite, pArchive);
		if (bWrite)
			return bRtn;
		return SetOccupantInfo();
	}
	virtual BOOL OnCreateItem(const char* buf, int nLen)
	{
		if (!CStaticObj::OnCreateItem(buf, nLen))
			return FALSE;
		return SetOccupantInfo();
	}
	virtual BOOL SetOccupantInfo();
};
//------------------------------------------------------------------------
// 多占位块的静态对象，如房子、大树等
class CStaticObjMultiOccTrans : public CStaticObjMultiOcc
{
public:
	virtual DWORD GetClassID() const{return miStaticObj_MultiOcc_Trans;}
};
//------------------------------------------------------------------------
// 货摊
class CStaticObjBooth : public CStaticObj
{
	enum {OFFX=0,OFFY=-65,MAX_STORE_NAME_LENGTH=20};
protected:
	char	m_szName[MAX_STORE_NAME_LENGTH+1];// 名字
	WPixel	m_clrName;		// 名字显示的颜色
	WPixel	m_clrCurColor;	// 名字的当前颜色
	IBitmap* m_pBitmap;		// 摆摊的文字的外框
public:
	CStaticObjBooth()
	{
		m_szName[0] = '\0';
		memset(m_szName, 0, sizeof(m_szName));
		m_clrName = WPixel(0,255,0);
		m_pBitmap = NULL;
	}
	virtual DWORD GetClassID() const{return miStaticObj_Booth;}
	virtual void propName(BOOL bSetProp, DWORD dwPropVal);
	virtual void propNameColor(BOOL bSetProp, DWORD dwPropVal);
	virtual void propBkImage(BOOL bSetProp, DWORD dwPropVal);
	virtual BOOL HitTest(const RECT& rcScreen);
	virtual RECT GetShowRect();
	virtual void OnUpdate(){}
	virtual void OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0){}
	virtual BOOL OnEnterViewport(BOOL bVisable);
	virtual BOOL SetSelected(BOOL bSelected = TRUE, BOOL bHighLight = TRUE);
	virtual void OnDrawTopmostObject(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag=0);
};
//------------------------------------------------------------------------
// 自定义的无占位块静态图片（为了冲锋搞的）
class CStaticObjEx : public CStaticObj, public ITimerSink
{
public:
	CCreatureView	*m_pCreatureView;

	// 位置相关
	POINT			m_ptOffsetTile;		// 
	SAniContext		m_AniContext;		// 动画上下文
	BYTE			m_nAlpha;
	BYTE			m_nOldAlpha;
	RECT			m_rcShowRect;
public:
	CStaticObjEx();
	virtual ~CStaticObjEx();

	// ITimerSink
	virtual void	OnTimer(DWORD dwEventID);
	
	virtual RECT	GetShowRect();

	virtual DWORD	GetClassID() const{ return miMagicObj; }
	virtual DWORD	GetLayerValue()	{return milvMidle - 1;}
	virtual IBitmap	*GetBitmap();
	virtual DWORD	GetResID()	{return 0;}

	virtual BOOL	OnEnterViewport(BOOL bVisable);
	virtual void	SetDirty(ICanvas* pCanvas, const POINT& ptViewTop);
	virtual void	OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0);

	virtual BOOL	AddToTimerAxis();
	virtual BOOL	AddToMapView(POINT& ptTile);
	virtual BOOL	RemoveFromTimerAxis();
	virtual BOOL	RemoveFromMapView();

	BOOL CreateItem(CCreatureView *pCreatureView);
};
//------------------------------------------------------------------------
// 自定义的无占位块静态图片（为了万剑搞的）
class CStaticObjActEx : public CStaticObjEx
{
public:
	int		m_nLife;
	POINT	m_ptBegin;
public:
	CStaticObjActEx();
	virtual ~CStaticObjActEx();

	// ITimerSink
	virtual void	OnTimer(DWORD dwEventID);
	virtual RECT	GetShowRect();

	virtual void	OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0);

	virtual BOOL	AddToTimerAxis();
	virtual BOOL	RemoveFromTimerAxis();
	virtual BOOL	AddToMapView(POINT& ptTile);
	virtual BOOL	RemoveFromMapView();

	BOOL CreateItem(CCreatureView *pCreatureView, SStaticObjActExArchive *pArchive);
};
//------------------------------------------------------------------------
// 地上物品
class CStaticObjArticle : public CStaticObj, public ITimerSink
{
	char		m_szArticleName[MAX_ITEM_NAME_LENGTH];	// 地上物的名字
	BOOL		m_bShowArticleName;						// 是否显示地上物的名字
	BOOL		m_bShowStar;							// 是否显示星星
	WPixel		m_clrName;								// 名字显示的颜色

	DWORD		m_dwResIDStar;							// 星星的图包ID
	CMagicStar*	m_pMagicStar;

	// 物品投掷相关
	enum 
	{
		MaxThrowHeight = 60,
		Speed0Time=10/*40个ontimer*/
	};

	int				m_nThrowHeight;		// 投掷高度
	int				m_t;
	BOOL			m_bIsThrowing;
	POINT			m_ptTileCenter;
	BOOL			m_bLayerValueMiddle;

public:
	CStaticObjArticle() {
		m_bShowArticleName=FALSE;
		m_bShowStar=FALSE;
		m_szArticleName[0]='\0';
		m_clrName=WPixel(255,255,255);

		m_dwResIDStar = 0;
		m_pMagicStar = NULL;
		(__int64&)m_ptTileCenter = 0;
		m_nThrowHeight = 0;
		m_t = 0;
		m_bIsThrowing = FALSE;
		m_bLayerValueMiddle = TRUE;
	}
	virtual BOOL	OnCreateItem(const char* buf, int nLen);
	virtual BOOL	CreateItem(SStaticObjArticleArchive* pArchive);
	virtual void	Close();
	virtual DWORD	GetClassID() const	{return miStaticObj_Article;}
	virtual DWORD	GetOccupantValue()	{return 0x0;}
	virtual DWORD	GetLayerValue()		{if (m_bLayerValueMiddle) return milvMidle;		return milvLowest;}
	virtual RECT	GetShowRect();
	virtual void	OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0);
	virtual void	OnDrawTopmostObject(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag=0);
	virtual int		cmdThrow(DWORD dwParam1, DWORD dwParam2);
	virtual void	OnTimer(DWORD dwEventID);
	virtual CPic*	RequestRes();

	virtual void	propName(BOOL bSetProp, DWORD dwPropVal);
	virtual void	propShowName(BOOL bSetProp, DWORD dwPropVal);
	virtual void	propShowStar(BOOL bSetProp, DWORD dwPropVal);
	virtual void	propNameColor(BOOL bSetProp, DWORD dwPropVal);

protected:
	BOOL CreateStarAni();
	BOOL CreateThrowAni();
};
//------------------------------------------------------------------------