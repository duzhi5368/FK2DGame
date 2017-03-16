/**
*	created:		2012-7-2   1:48
*	filename: 		StaticObj
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/StaticObj.h"
#include "../Include/PalMgr.h"
#include "../Include/SkillCreate.h"
#include "../Include/CreatureView.h"
//------------------------------------------------------------------------
CStaticObjEx::CStaticObjEx()
{
	m_pCreatureView = NULL;
	m_nAlpha = 0;
	m_nOldAlpha = 0;
	m_ptOffsetTile.x = 0;
	m_ptOffsetTile.y = 0;
	SetRectEmpty(&m_rcShowRect);
}
//------------------------------------------------------------------------
CStaticObjEx::~CStaticObjEx()
{
	if(m_pCreatureView != NULL)
		m_pCreatureView->DelLinkView(&m_pCreatureView);

	RemoveFromMapView();
}
//------------------------------------------------------------------------
void CStaticObjEx::OnTimer(DWORD dwEventID)
{
	m_nAlpha -= 10;
	if(m_nAlpha > 101)
	{
		RemoveFromTimerAxis();
		RemoveFromMapView();
		delete this;
	}
}
//------------------------------------------------------------------------
RECT CStaticObjEx::GetShowRect()
{
	return m_rcShowRect;
}
//------------------------------------------------------------------------
IBitmap* CStaticObjEx::GetBitmap() 
{
	return NULL;
}
//------------------------------------------------------------------------
// 魔法进入屏幕视图区
BOOL CStaticObjEx::OnEnterViewport(BOOL bVisable)
{
	return TRUE;
}
//------------------------------------------------------------------------
void CStaticObjEx::SetDirty(ICanvas* pCanvas, const POINT& ptViewTop)
{
	if(!pCanvas)
		return;

	// 处理旧脏
	if(m_bNeedSetOldDirty && m_nOldAlpha == m_nAlpha)	// 如果和上次画的一样则不用处理了						
		return;

	// 保存这次的数据
	m_nOldAlpha = m_nAlpha;
	RECT rc = m_rcAreaOld;

	// 再把新的要画图区域设脏
	OffsetRect(&rc,
		-ptViewTop.x,
		-ptViewTop.y);
	pCanvas->AddDirtyRect(rc);

	// 表示这次这个对象画了，下次要把他画的地方设脏
	m_bNeedSetOldDirty = TRUE;
}
//------------------------------------------------------------------------
void CStaticObjEx::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if (!pCanvas || m_pCreatureView == NULL)
		return;

	POINT ptTemp = ptTileCenter;
	ptTemp.x += m_ptOffsetTile.x;
	ptTemp.y += m_ptOffsetTile.y;
	m_pCreatureView->OnDrawEx(pCanvas, ptTemp, BLIT_MASK | BLIT_TRANS, &m_AniContext, m_nAlpha);
}
//------------------------------------------------------------------------
inline BOOL CStaticObjEx::AddToTimerAxis()
{
	g_pTimeAxis->SetTimer(0,
		static_cast<ITimerSink*>(this),
		20,
		"BasicSys!!CStaticObjEx::AddToTimerAxis");

	return TRUE;
}
//------------------------------------------------------------------------
inline BOOL CStaticObjEx::AddToMapView(POINT& ptTile) 
{
	BOOL b = g_pMapView->AddItem(ptTile, this);
	if (b)
		m_ptTile = ptTile;
	return b;
}
//------------------------------------------------------------------------
inline BOOL CStaticObjEx::RemoveFromTimerAxis()
{
	g_pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));
	return FALSE;
}
//------------------------------------------------------------------------
inline BOOL CStaticObjEx::RemoveFromMapView()
{
	return g_pMapView->RemoveItem(m_ptTile, this);
}
//------------------------------------------------------------------------
// 不加入地图、不加入时间轴
BOOL CStaticObjEx::CreateItem(CCreatureView *pCreatureView)
{
	if(pCreatureView == NULL)
		return FALSE;

	m_pCreatureView = pCreatureView;
	m_pCreatureView->AddLinkView(&m_pCreatureView);

	m_rcShowRect = pCreatureView->GetShowRect();
	m_nOldAlpha = m_nAlpha = 101;
	m_ptTile = pCreatureView->GetTile();
	m_ptOffsetTile = pCreatureView->m_ptTileOffset;
	m_AniContext.SetData(&(pCreatureView->m_AniContext));

	POINT ptNew;
	Tile_To_World(m_ptTile, ptNew);
	m_rcAreaOld = m_rcShowRect;
	OffsetRect(&m_rcAreaOld,
		ptNew.x,
		ptNew.y);

	return TRUE;
}
//------------------------------------------------------------------------
CStaticObjActEx::CStaticObjActEx()
{
	m_ptBegin.x = 0;
	m_ptBegin.y = 0;
	m_nLife = 0;
}
//------------------------------------------------------------------------
CStaticObjActEx::~CStaticObjActEx()
{
}
//------------------------------------------------------------------------
RECT CStaticObjActEx::GetShowRect()
{
	RECT rc = {-200,-200,200,200};
	return rc;
}
//------------------------------------------------------------------------
inline BOOL CStaticObjActEx::AddToTimerAxis()
{
	g_pTimeAxis->SetTimer(0,
		static_cast<ITimerSink*>(this),
		1000 / m_AniContext.nFrames,
		"BasicSys!!CStaticObjActEx::AddToTimerAxis");

	g_pTimeAxis->SetTimer(1,
		static_cast<ITimerSink*>(this),
		5,
		"BasicSys!!CStaticObjActEx::AddToTimerAxis");
	return TRUE;
}
//------------------------------------------------------------------------
inline BOOL CStaticObjActEx::RemoveFromTimerAxis()
{
	g_pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));
	g_pTimeAxis->KillTimer(1, static_cast<ITimerSink*>(this));
	return FALSE;
}
//------------------------------------------------------------------------
inline BOOL CStaticObjActEx::AddToMapView(POINT& ptTile) 
{
	BOOL b = g_pMapView->AddItem(ptTile, this);
	if (b)
	{
		m_ptTile = ptTile;
		Tile_To_World(m_ptTile, m_ptBegin);
		m_ptBegin.x += m_ptOffsetTile.x;
		m_ptBegin.y += m_ptOffsetTile.y;
	}
	return b;
}
//------------------------------------------------------------------------
inline BOOL CStaticObjActEx::RemoveFromMapView()
{
	return g_pMapView->RemoveItem(m_ptTile, this);
}
//------------------------------------------------------------------------
// 不加入地图、不加入时间轴
BOOL CStaticObjActEx::CreateItem(CCreatureView *pCreatureView, SStaticObjActExArchive *pArchive)
{
	if(pArchive == NULL)
		return FALSE;

	if(!CStaticObjEx::CreateItem(pCreatureView))
		return FALSE;

	Tile_To_World(m_ptTile, m_ptBegin);
	m_ptBegin.x += m_ptOffsetTile.x;
	m_ptBegin.y += m_ptOffsetTile.y;

	m_nOldAlpha = m_nAlpha = 200;
	m_AniContext.nState = pArchive->nState;
	m_AniContext.nDir = pArchive->nDir;
	m_AniContext.nCurFrame = 0;

	return TRUE;
}
//------------------------------------------------------------------------
float StaticObjActExStepX[8] = 
{
	0,1.2f,2.4f,1.2f,0,-1.2f,-2.4f,-1.2f
};
//------------------------------------------------------------------------
float StaticObjActExStepY[8] = 
{
	-1.2f,-0.6f,0,0.6f,1.2f,0.6f,0,-0.6f
};
//------------------------------------------------------------------------
void CStaticObjActEx::OnTimer(DWORD dwEventID)
{
	if(dwEventID == 0)
	{
		m_nAlpha -= 15;
		if(m_nAlpha < 15)
			m_nAlpha = 15;
		if(++m_AniContext.nCurFrame >= m_AniContext.nFrames)
		{
			RemoveFromTimerAxis();
			RemoveFromMapView();
			delete this;
		}
	}
	else
	{
		m_nLife++;
		POINT ptCenter, ptTilePos, ptTempTile;
		ptCenter.x = m_ptBegin.x + StaticObjActExStepX[m_AniContext.nDir] * m_nLife;
		ptCenter.y = m_ptBegin.y + StaticObjActExStepY[m_AniContext.nDir] * m_nLife;

		World_To_Tile(ptCenter, ptTempTile);
		Tile_To_World(ptTempTile, ptTilePos);
		m_ptOffsetTile.x = ptCenter.x - ptTilePos.x;
		m_ptOffsetTile.y = ptCenter.y - ptTilePos.y;
		g_pMapView->MoveItem(m_ptTile, ptTempTile, this);
		m_ptTile = ptTempTile;
	}
}
//------------------------------------------------------------------------
void CStaticObjActEx::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if (!pCanvas || m_pCreatureView == NULL)
		return;

	POINT ptTemp = ptTileCenter;
	ptTemp.x += m_ptOffsetTile.x;
	ptTemp.y += m_ptOffsetTile.y;
	m_pCreatureView->OnDrawEx(pCanvas, ptTemp, BLIT_MASK | BLIT_TRANS, &m_AniContext, m_nAlpha);
}
//------------------------------------------------------------------------
// 所有支持的命令
CStaticObj::ProcCommand CStaticObj::m_Command[AtcCmdMaxCommandCount] =
{
	NULL,
	&CStaticObj::cmdSetProperty,
	&CStaticObj::cmdGetProperty,
	&CStaticObj::cmdChangePal,
	&CStaticObj::cmdThrow,
};
//------------------------------------------------------------------------
// 所有支持的属性
CStaticObj::ProcProperty CStaticObj::m_Property[AtcPropMaxPropCount] =
{
	NULL,
	&CStaticObj::propName, 
	&CStaticObj::propShowName,
	&CStaticObj::propNameColor,
	&CStaticObj::propBkImage,
	&CStaticObj::propShowStar,
};
//------------------------------------------------------------------------
CStaticObj::CStaticObj()
{
	m_dwResID = 0;
	m_pPoolNodeObj = NULL;
	m_ptAnchor.x = m_ptAnchor.y = 0;
	m_ptSortOff1.x = m_ptSortOff1.y = 0;
	m_ptSortOff2.x = m_ptSortOff2.y = 0;
	m_dwData = 0;
	m_pCurPal = NULL;
	m_ptTile.x = m_ptTile.y = 0;
	m_wpMinMapFlagColor = WPixel(255,255,255);
	m_bVisibleOnMinmap = FALSE;
}
//------------------------------------------------------------------------
CStaticObj::~CStaticObj()
{
	//Close();
}
//------------------------------------------------------------------------
void CStaticObj::Close()
{
	if (m_pPoolNodeObj)
	{
		g_ResLoader.ReleaseRes(ioStatic + m_dwResID);
		m_pPoolNodeObj = NULL;
	}
	m_dwResID = 0;
}
//------------------------------------------------------------------------
DWORD CStaticObj::GetResID()
{
	return m_dwResID;
}
//------------------------------------------------------------------------
// 设置在小地图上的颜色
void CStaticObj::SetFlagColorOnMinmap(DWORD dwColor)
{
	m_wpMinMapFlagColor = WPixel(dwColor);
}
//------------------------------------------------------------------------
// 取得在小地图上的颜色
DWORD CStaticObj::GetFlagColorOnMinmap()
{
	return m_wpMinMapFlagColor;
}
//------------------------------------------------------------------------
// 设置是否在小地图上显示
void CStaticObj::SetVisibleOnMinmap(BOOL bVisible)
{
	m_bVisibleOnMinmap = bVisible;
}
//------------------------------------------------------------------------
// 取得是否在小地图上显示
BOOL CStaticObj::GetVisibleOnMinmap()
{
	return m_bVisibleOnMinmap;
}
//------------------------------------------------------------------------
CPic* CStaticObj::RequestRes()
{
	if (!m_pPoolNodeObj)
		m_pPoolNodeObj = static_cast<CPic*>(g_ResLoader.RequestPic(m_dwResID));
	if( m_pPoolNodeObj == NULL )
	{
		TraceLn( "缺少资源: ID =  %d", m_dwResID );
		return NULL;
	}
	return m_pPoolNodeObj;
}
//------------------------------------------------------------------------
void CStaticObj::GetOldDirtyRect(list<RECT> &DirtyRectList)
{
	if(!m_bNeedSetOldDirty)
		return;

	m_bNeedSetOldDirty = FALSE;
	
	DirtyRectList.push_back(m_rcAreaOld);
}
//------------------------------------------------------------------------
void CStaticObj::SetDirty(ICanvas* pCanvas, const POINT& ptViewTop)
{
	if(!pCanvas) return;
	
	POINT ptNew;
	RECT rcAreaNew;
	// 获取最新的显示矩阵
	Tile_To_World(m_ptTile, ptNew);

	rcAreaNew = GetShowRect();
	OffsetRect(&rcAreaNew,
		ptNew.x,
		ptNew.y);

	// 处理旧脏
	if(m_bNeedSetOldDirty)
	{
		if(EqualRect(&rcAreaNew, &m_rcAreaOld))	// 如果和上次画的位置一样则不用处理了						
			return;

		// 先把上次旧的画图区域设脏
		OffsetRect(&m_rcAreaOld,
			-ptViewTop.x,
			-ptViewTop.y);
		pCanvas->AddDirtyRect(m_rcAreaOld);
	}

	if(!RequestRes())
		return;

	// 保存这次的数据
	m_rcAreaOld = rcAreaNew;

	// 再把新的要画图区域设脏
	OffsetRect(&rcAreaNew,
		-ptViewTop.x,
		-ptViewTop.y);
	pCanvas->AddDirtyRect(rcAreaNew);
	
	// 表示这次这个对象画了，下次要把他画的地方设脏
	m_bNeedSetOldDirty = TRUE;
}
//------------------------------------------------------------------------
void CStaticObj::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if(!pCanvas || !RequestRes()) return;
	
	m_pPoolNodeObj->Draw(pCanvas, 
			ptTileCenter.x - m_ptAnchor.x, 
			ptTileCenter.y - m_ptAnchor.y, 
			dwFlag, m_pCurPal);
}
//------------------------------------------------------------------------
void CStaticObj::OnDrawTopmostObject(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag)
{
	/*
#ifdef _USE
	if (IsSelected())
	{
		DWORD dwFlags = CMapItemView::GetFlag();
		WPixel clr = WPixel(0,255,0);
		WPixel clrText = WPixel(0,0,255);
		char buf[MAX_PATH];
		POINT ptTile;
		g_pMapView->ScreenToTile(ptTileCenter, ptTile);
		
		if (dwFlags & flagInfo)
		{
			IResObject* pObj = g_ResLoader.m_pScpBmp->GetResObject(m_dwResID);
			if (pObj)
			{
				IResObject* pParentObj = pObj->GetParentObject();
				wsprintf(buf, "%s\\%s,(%d,%d),CID=%d,RID=%d,SORT=%d", 
					pParentObj->GetObjectName(), pObj->GetObjectName(),
					ptTile.x, ptTile.y, GetClassID(), m_dwResID, dwFlag);
			}
			else
				wsprintf(buf, "(%d,%d),CID=%d,RID=%d,SORT=%d", ptTile.x, ptTile.y, GetClassID(), m_dwResID, dwFlag);
			
			int nW = g_pFont->GetLength(buf);
			int nH = g_pFont->GetHeight();
			RECT rcTip;
			rcTip.left = ptTileCenter.x - m_ptAnchor.x - 1;
			rcTip.right = rcTip.left + nW + 2;
			rcTip.top = ptTileCenter.y - m_ptAnchor.y - nH - 2;
			rcTip.bottom = rcTip.top + nH + 1;
			pCanvas->DrawBox(rcTip, clr);
			pCanvas->DrawText(g_pFont, rcTip.left+1, rcTip.top+1, buf, clrText, FONT_NOALPHA);
		}
		// draw select rectangle
		if (dwFlags & flagRectangle)
		{
			RECT rc = GetShowRect();
			OffsetRect(&rc, ptTileCenter.x, ptTileCenter.y);
			InflateRect(&rc, 1, 1);
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
	}
#endif
	*/
}
//------------------------------------------------------------------------
BOOL CStaticObj::OnSerialize(BOOL bWrite,IArchive* pArchive)
{
	if(bWrite)
	{
		SStaticObjArchive a = {
			m_dwResID, 
		};
			
		pArchive->Write((char*)&a, sizeof(a));
		return TRUE;
	}
	else
	{
		Close();
		if (!CreateItem((SStaticObjArchive*)pArchive->GetBuffer()))
		{
			TraceLn("ERROR: CStaticObj::OnSerialize(bRead), create item failed!");
		}
		pArchive->Seek(sizeof(SStaticObjArchive));
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CStaticObj::OnCreateItem(const char* buf, int nLen)
{
	Close();
	return CreateItem((SStaticObjArchive*)buf);
}

BOOL CStaticObj::CreateItem(SStaticObjArchive* pArchive)
{
	ASSERT(pArchive);
	m_dwResID = pArchive->dwResID; // 静态对象的资源ID等于对象ID

	OnUpdate();
	return TRUE;
}
//------------------------------------------------------------------------
void CStaticObj::OnUpdate()
{
	SStaticObjCreateParam socp;
	memset(&socp, 0, sizeof(socp));
	g_ResLoader.GetStaticObjCreateParam(m_dwResID, socp);
	m_ptAnchor = socp.ptAnchor;
	m_ptSortOff1 = socp.ptSortOff1;
	m_ptSortOff2 = socp.ptSortOff2;
}
//------------------------------------------------------------------------
void CStaticObj::SetTile(POINT& ptTile, IMapView* pMapView)
{
	m_ptTile = ptTile;
	UpdatePosInfo(pMapView);
}
//------------------------------------------------------------------------
void CStaticObj::UpdatePosInfo(IMapView* pMapView)
{
	RECT rc;
	COccupantTileList* potl = GetOccupantTileList();
	if (potl == NULL)
	{
		m_rcArea.right = m_rcArea.left = m_ptTile.x;
		m_rcArea.bottom = m_rcArea.top = m_ptTile.y;	// 是单点对象，右上角跟左下角一样
		CopyRect(&rc, &m_rcArea);
	}
	else
	{
		COccupantTileListHeader& octh = potl->GetPreData();
		m_rcArea.left = m_ptTile.x + octh.nOffAnchorX;
		m_rcArea.top = m_ptTile.y + octh.nOffAnchorY;
		m_rcArea.right = m_rcArea.left + octh.wWidth - 1;
		m_rcArea.bottom = m_rcArea.top + octh.wHeight - 1;
		
		rc.left = m_ptTile.x + m_ptSortOff1.x;
		rc.top = m_ptTile.y + m_ptSortOff1.y;
		rc.right = m_ptTile.x + m_ptSortOff2.x;
		rc.bottom = m_ptTile.y + m_ptSortOff2.y;
	}
	
	pMapView->TileToWorld((POINT&)rc, m_ptLeft);
	
	POINT ptRightTile = {rc.right, rc.bottom};
	pMapView->TileToWorld(ptRightTile, m_ptRight);
	
	pMapView->TileToWorld(m_ptTile, m_ptWold);
}
//------------------------------------------------------------------------
BOOL CStaticObj::OnEnterViewport(BOOL bVisable)
{
	if (bVisable)
	{
		if (!RequestRes())
			return FALSE;
	}
	else if (m_pPoolNodeObj)
	{
		// SetSelected(FALSE); // 先恢复原来的调色板
		if (!g_ResLoader.ReleaseRes(ioStatic + m_dwResID))
			return FALSE;
		m_pPoolNodeObj = NULL;
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CStaticObj::HitTest(const RECT& rcScreen)
{
	if (!m_pPoolNodeObj || !m_pPoolNodeObj->m_pBitmap)
		return FALSE;
	RECT rcSelect = rcScreen;
	OffsetRect(&rcSelect, m_ptAnchor.x, m_ptAnchor.y);
	IBitmap* pBmp = m_pPoolNodeObj->m_pBitmap;
	RECT rcBmp = {0, 0, pBmp->GetWidth(), pBmp->GetHeight()};
	RECT rcIntersect;
	if (!IntersectRect(&rcIntersect, &rcSelect, &rcBmp))
		return FALSE;
	BOOL bTransparent = TRUE;
	POINT ptSelect;
	for (int i=rcIntersect.top; i< rcIntersect.bottom; i++)
	{
		ptSelect.y = i;
		for (int j=rcIntersect.left; j<rcIntersect.right; j++)
		{
			ptSelect.x = j;
			if (!pBmp->IsTransparent(ptSelect))
				return TRUE;
		}
	}

	return FALSE;
}
//------------------------------------------------------------------------
RECT CStaticObj::GetShowRect()
{
	RECT rc = {0,0,0,0};
	RequestRes();
	if (m_pPoolNodeObj)
	{
		rc.left = -m_ptAnchor.x;
		rc.top = -m_ptAnchor.y;
		rc.right = rc.left + m_pPoolNodeObj->GetWidth();
		rc.bottom = rc.top + m_pPoolNodeObj->GetHeight();
	}
	return rc;
}
//------------------------------------------------------------------------
IBitmap* CStaticObj::GetBitmap() 
{
	RequestRes();
	if (m_pPoolNodeObj)
		return m_pPoolNodeObj->GetBitmap();
	return NULL;
}
//------------------------------------------------------------------------
BOOL CStaticObj::SetSelected(BOOL bSelected, BOOL bHighLight)
{
	if (!m_pPoolNodeObj) return FALSE;
	if (bSelected)
	{
		if (bHighLight)
			ChangePal(PalType_Light);
#ifdef _USE
		AddFlag(CMapItemView::flagSelected);
#endif
	}
	else
	{
		RestorePal();
#ifdef _USE
		RemoveFlag(CMapItemView::flagSelected);
#endif
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
void CStaticObj::ChangePal(int nNewPalType)
{
	if (!m_pPoolNodeObj) return;
	m_pCurPal = g_PalMgr.RequestPal(GetOffsetID(), 0, -1, GetCurPal(), 
		m_pPoolNodeObj->GetUseColor(), nNewPalType)->GetPalette();
}
//------------------------------------------------------------------------
void CStaticObj::RestorePal()
{
	if (!m_pPoolNodeObj) return;
	m_pCurPal = m_pPoolNodeObj->GetPal();
}
//------------------------------------------------------------------------
// 各种指令的执行
int CStaticObj::OnCommand(DWORD dwCmdID, DWORD dwParam1,DWORD dwParam2)
{
	if (dwCmdID >= AtcCmdMaxCommandCount)
	{
		TraceLn("ERROR: CStaticObj::OnCommand(): Unknow Command ID = %d", dwCmdID);
		return FALSE;
	}
	if (m_Command[dwCmdID] == NULL)
	{
		TraceLn("ERROR: CStaticObj::OnCommand(): Command is not implemented, ID = %d", dwCmdID);
		return FALSE;
	}
	
	return (this->*m_Command[dwCmdID])(dwParam1, dwParam2);
}
//------------------------------------------------------------------------
// 静态对象的属性设置
// dwParam1: 属性类型
// dwParam2: 设置值(参看静态对象的属性定义)
int CStaticObj::cmdSetProperty(DWORD dwParam1, DWORD dwParam2)
{
	if (dwParam1 >= AtcPropMaxPropCount)
	{
		TraceColorLn(MY_TRACE_COLOR, "CStaticObj::cmdSetProperty(), Unknow Property ID = %d", dwParam1);
		return FALSE;
	}
	if (m_Property[dwParam1] == NULL)
		return FALSE;
	
	(this->*m_Property[dwParam1])(TRUE, dwParam2);
	return TRUE;
}
//------------------------------------------------------------------------
// 获取静态对象的属性
// dwParam1: 属性类型
// dwParam2: 返回值(参看静态对象的属性定义)
int CStaticObj::cmdGetProperty(DWORD dwParam1, DWORD dwParam2)
{
	if (dwParam1 >= AtcPropMaxPropCount)
	{
		TraceColorLn(MY_TRACE_COLOR, "CStaticObj::cmdSetProperty(), Unknow Property ID = %d", dwParam1);
		return FALSE;
	}
	if (m_Property[dwParam1] == NULL)
		return FALSE;
	
	(this->*m_Property[dwParam1])(FALSE, dwParam2);
	return TRUE;
}
//------------------------------------------------------------------------
// 改变对象的调色板
// dwParam1: 是设置新的调色板(TRUE)还是恢复原来的调色板(FALSE)?
// dwParam2: 调色板类型
int CStaticObj::cmdChangePal(DWORD dwParam1, DWORD dwParam2)
{
	if (dwParam1)
		ChangePal(dwParam2);
	else
		RestorePal();
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CStaticObjSpecificBlock::SetOccupantInfo()
{
	char buffer[4096];
	unsigned char buf[2048];
	COccupantTileList *pL = (COccupantTileList*)buffer;
	BOOL nLen = g_ResLoader.GetStaticObjOccupantInfo(m_dwResID, buf, 2048);
	m_nOccupantOption = nLen;
	if (nLen)
	{
		m_nOccupantOption = 2;
		int w = buf[2], h = buf[3];
		pL->Init(w*h);
		pL->GetPreData().nOffAnchorX = - (char)buf[0];
		pL->GetPreData().nOffAnchorY = - (char)buf[1];
		pL->GetPreData().wWidth = w;
		pL->GetPreData().wHeight = h;
		
		int nRepeatCount = 0;
		int nDirectCount = 0;
		int nPos = 4;
		while (nPos < nLen)
		{
			if (buf[nPos] < 0x80) // 不同值
			{
				nDirectCount = buf[nPos++] + 1;
				for (int k=0; k<nDirectCount; k++,nPos++)
				{
					pL->Add(buf[nPos]);
				}
			}
			else // 相同值
			{
				nRepeatCount = buf[nPos++] - 127;
				for (int k=0; k<nRepeatCount; k++)
					pL->Add(buf[nPos]);
				nPos ++;
			}
		}
	}
	else
	{
		pL->Init(0);
		pL->GetPreData().nOffAnchorX = 0;
		pL->GetPreData().nOffAnchorY = 0;
		pL->GetPreData().wWidth = 0;
		pL->GetPreData().wHeight = 0;
	}
	return SetOccupantTileList(pL);
}
//------------------------------------------------------------------------
// 名字
// 设置或返回: char*
void CStaticObjBooth::propName(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
		lstrcpyn(m_szName, (LPCSTR)dwPropVal, MAX_STORE_NAME_LENGTH);
	else
		lstrcpyn((char*)dwPropVal, m_szName, MAX_STORE_NAME_LENGTH);
}
//------------------------------------------------------------------------
// 物品名字的颜色
// 设置或返回: WPixel*
void CStaticObjBooth::propNameColor(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
	{
		m_clrName = *(WPixel*)dwPropVal;
		m_clrCurColor = m_clrName;
	}
	else
		*(WPixel*)dwPropVal = m_clrName;
}
//------------------------------------------------------------------------
// 摆摊文字下面的背景图片
// 设置或返回: IBitmap*
void CStaticObjBooth::propBkImage(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
	{
		m_pBitmap = *(IBitmap**)dwPropVal;
		int w = m_pBitmap->GetWidth();
	}
	else
		*(IBitmap**)dwPropVal = m_pBitmap;
}
//------------------------------------------------------------------------
BOOL CStaticObjBooth::HitTest(const RECT& rcScreen)
{
	if (!m_pBitmap)
		return FALSE;
	RECT rcSelect = rcScreen;
	OffsetRect(&rcSelect, -OFFX, -OFFY);
	int w = m_pBitmap->GetWidth();
	int h = m_pBitmap->GetHeight();
	RECT rcBmp = {-w/2, -h/2, w/2, h/2};
	RECT rcIntersect;
	if (!IntersectRect(&rcIntersect, &rcSelect, &rcBmp))
		return FALSE;
	
	return TRUE;
}
//------------------------------------------------------------------------
RECT CStaticObjBooth::GetShowRect()
{
	RECT rc = {0,0,0,0};
	if (m_pBitmap)
	{
		int w = m_pBitmap->GetWidth();
		int h = m_pBitmap->GetHeight();		
		rc.left = -w/2+OFFX;
		rc.top = -h/2+OFFY;
		rc.right = rc.left + w;
		rc.bottom = rc.top + h;
	}
	return rc;
}
//------------------------------------------------------------------------
BOOL CStaticObjBooth::OnEnterViewport(BOOL bVisable)
{
	if (m_pBitmap && !bVisable)
		SetSelected(FALSE); // 先恢复原来的文字颜色
	return TRUE;
}
//------------------------------------------------------------------------
#define SEL_COLOR WPixel(255,0,0)
//------------------------------------------------------------------------
BOOL CStaticObjBooth::SetSelected(BOOL bSelected, BOOL bHighLight)
{
	if (bSelected)
		m_clrCurColor = SEL_COLOR;		
	else
		m_clrCurColor = m_clrName;		
	
	return TRUE;
}
//------------------------------------------------------------------------
void CStaticObjBooth::OnDrawTopmostObject(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if (!pCanvas || !m_pBitmap) return;
	int x = ptTileCenter.x  + OFFX;
	int y = ptTileCenter.y  + OFFY;
	int w = m_pBitmap->GetWidth();
	int h = m_pBitmap->GetHeight();
	int x1 = x - w / 2;
	int y1 = y - h / 2;
	
	// 画背景
	m_pBitmap->SetUserData(128);
	m_pBitmap->Draw(pCanvas, x1, y1, w, h, 0, 0, BLIT_TRANS|BLIT_MASK);
	
	// 画文字
	int nDeltaX = (w - g_pFontWithOutline->GetLength(m_szName)) / 2;
	int nDeltaY = (h - g_pFontWithOutline->GetHeight()) / 2;
	pCanvas->DrawText(g_pFontWithOutline, x1+nDeltaX, y1+nDeltaY, m_szName, m_clrCurColor, FONT_NOALPHA);
}
//------------------------------------------------------------------------
BOOL CStaticObjMultiOcc::SetOccupantInfo()
{
	char buffer[4096];
	unsigned char buf[2048];
	COccupantTileList *pL = (COccupantTileList*)buffer;
	BOOL nLen = g_ResLoader.GetStaticObjOccupantInfo(m_dwResID, buf, 2048);
	if (nLen)
	{
		int w = buf[2], h = buf[3];
		pL->Init(w*h);
		pL->GetPreData().nOffAnchorX = - (char)buf[0];
		pL->GetPreData().nOffAnchorY = - (char)buf[1];
		pL->GetPreData().wWidth = w;
		pL->GetPreData().wHeight = h;
		
		int nRepeatCount = 0;
		int nDirectCount = 0;
		int nPos = 4;
		while (nPos < nLen)
		{
			if (buf[nPos] < 0x80) // 不同值
			{
				nDirectCount = buf[nPos++] + 1;
				for (int k=0; k<nDirectCount; k++,nPos++)
				{
					pL->Add(buf[nPos]);
				}
			}
			else // 相同值
			{
				nRepeatCount = buf[nPos++] - 127;
				for (int k=0; k<nRepeatCount; k++)
					pL->Add(buf[nPos]);
				nPos ++;
			}
		}
	}
	else
	{
		pL->Init(0);
		pL->GetPreData().nOffAnchorX = 0;
		pL->GetPreData().nOffAnchorY = 0;
		pL->GetPreData().wWidth = 0;
		pL->GetPreData().wHeight = 0;
	}
	return SetOccupantTileList(pL);
}
//------------------------------------------------------------------------
// CStaticObjArticle
void CStaticObjArticle::Close()
{
	CStaticObj::Close();
	if (m_pMagicStar)
	{
		m_pMagicStar->RemoveFromTimerAxis();
		m_pMagicStar->Release();
		m_pMagicStar = NULL;
	}
	if (m_bIsThrowing)
	{
		g_pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));;
	}
}
//------------------------------------------------------------------------
BOOL CStaticObjArticle::CreateStarAni()
{
	if (m_dwResIDStar <= 0)
		return FALSE;
	
	SMagicArchive_Life st;
	memset(&st, 0, sizeof(st));
	st.dwResID = m_dwResIDStar;
	IResObject* pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(st.dwResID);
	g_ResLoader.ReadLifeMagic(pMagicObj, &st);
	st.nLifeLine = -1;
	
	m_pMagicStar = new CMagicStar;
	if (m_pMagicStar->OnCreateItem((const char*)&st, sizeof(st)))
		m_pMagicStar->AddToTimerAxis();
	
	return TRUE;
}
//------------------------------------------------------------------------
#define __g		-(float)((float)(2*MaxThrowHeight)/(float)(Speed0Time*Speed0Time))
#define __v0	(float)((float)(2*MaxThrowHeight)/Speed0Time)
#define __time	2*Speed0Time
//------------------------------------------------------------------------
BOOL CStaticObjArticle::CreateThrowAni()
{
	Tile_To_World(m_ptTile, m_ptTileCenter);
	
	g_pTimeAxis->SetTimer(0,
		static_cast<ITimerSink*>(this),
		31,
		"BasicSys!!CStaticObjArticle::CreateThrowAni");
	m_bIsThrowing = TRUE;
	
	m_t = 0;
	return TRUE;
}
//------------------------------------------------------------------------
// ITimerSink
void CStaticObjArticle::OnTimer(DWORD dwEventID)
{
	// 光源与水平面(XY平面)夹角为80度，光源与Y平面夹角为25度
	// x = h*ctg80*sin25;
	// y = h*ctg80*con25;
#define CTG_MUL_SIN 0.0745f
#define CTG_MUL_COS	0.1598f
	m_nThrowHeight = (__v0 + 0.5f*__g*m_t)*m_t;
	m_t ++;
	if (m_t >= __time)
	{
		g_pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));;
		m_bIsThrowing = FALSE;
		m_bLayerValueMiddle = FALSE;
	}
}
//------------------------------------------------------------------------
int CStaticObjArticle::cmdThrow(DWORD dwParam1, DWORD dwParam2)
{
	if (!CreateThrowAni())
		return FALSE;
	m_bIsThrowing = TRUE;
	return TRUE;
}
//------------------------------------------------------------------------
CPic* CStaticObjArticle::RequestRes()
{
	if (!m_pPoolNodeObj)
	{
		m_pPoolNodeObj = static_cast<CPic*>(g_ResLoader.RequestPic(m_dwResID));
		if (m_ptAnchor.x == 0 && m_ptAnchor.y == 0)
		{
			m_ptAnchor.x = m_pPoolNodeObj->GetWidth() >> 1;
			m_ptAnchor.y = m_pPoolNodeObj->GetHeight() >> 1;
		}
	}
	return m_pPoolNodeObj;
}
//------------------------------------------------------------------------
void CStaticObjArticle::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if(!pCanvas || !RequestRes()) return;
	if (!m_bIsThrowing)
	{
		m_pPoolNodeObj->Draw(pCanvas, 
			ptTileCenter.x - m_ptAnchor.x, 
			ptTileCenter.y - m_ptAnchor.y, 
			dwFlag, m_pCurPal);

	}
	else
	{
		int x = g_pMapView->GetViewTopLeftX();
		int y = g_pMapView->GetViewTopLeftY();
		POINT ptCenter = m_ptTileCenter;
		ptCenter.x -= x;
		ptCenter.y -= y;
		m_pPoolNodeObj->Draw(pCanvas, 
			ptCenter.x  - m_ptAnchor.x, 
			ptCenter.y  - m_ptAnchor.y - m_nThrowHeight, 
			dwFlag, m_pCurPal);
	}
}
//------------------------------------------------------------------------
RECT CStaticObjArticle::GetShowRect()
{
	RECT rc = {0,0,0,0};
	RequestRes();
	if (m_pPoolNodeObj)
	{
		rc.left = - m_ptAnchor.x;
		rc.top = - m_ptAnchor.y - m_nThrowHeight;
		rc.right = rc.left + m_pPoolNodeObj->GetWidth();
		rc.bottom = rc.top + m_pPoolNodeObj->GetHeight();
	}
	return rc;
}
//------------------------------------------------------------------------
void CStaticObjArticle::OnDrawTopmostObject(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if (m_bIsThrowing)
		return;

	if (m_pMagicStar && m_bShowStar)
		m_pMagicStar->OnDraw(pCanvas, ptTileCenter, BLIT_MASK);
	// draw user name
	DWORD dwFlags = CMapItemView::GetFlag();
	if (m_bShowArticleName)
	{
		int w = m_pPoolNodeObj->GetWidth();
		int h = m_pPoolNodeObj->GetHeight();
		int x = ptTileCenter.x  - m_ptAnchor.x + (w - (int)g_pFontWithOutline->GetLength(m_szArticleName))/2;
		int y = ptTileCenter.y - m_ptAnchor.y - (int)g_pFontWithOutline->GetHeight() - 8;
		pCanvas->DrawText(g_pFontWithOutline, x, y, m_szArticleName, m_clrName, FONT_NOALPHA);
	}
};
//------------------------------------------------------------------------
BOOL CStaticObjArticle::OnCreateItem(const char* buf, int nLen)
{
	Close();
	return CreateItem((SStaticObjArticleArchive*)buf);
}
//------------------------------------------------------------------------
BOOL CStaticObjArticle::CreateItem(SStaticObjArticleArchive* pArchive)
{
	ASSERT(pArchive);
	m_dwResID = pArchive->dwResIDBox;
	m_dwResIDStar = pArchive->dwResIDStar;

	OnUpdate();

	CreateStarAni();
	return TRUE;
}
//------------------------------------------------------------------------
// 地上物的名字
// 设置或返回: char*
void CStaticObjArticle::propName(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
	{
		lstrcpyn(m_szArticleName, (LPCSTR)dwPropVal, MAX_ITEM_NAME_LENGTH);		
	}
	else
		lstrcpyn((char*)dwPropVal, m_szArticleName, MAX_ITEM_NAME_LENGTH);
}
//------------------------------------------------------------------------
// 设置星星是否显示
// 设置或返回: char*
void CStaticObjArticle::propShowStar(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
		m_bShowStar = *(BOOL*)dwPropVal;
	else
		*(BOOL*)dwPropVal = m_bShowStar;
}
//------------------------------------------------------------------------
// 是否让地上物显示自己的名字
// 设置或返回: BOOL*
void CStaticObjArticle::propShowName(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
		m_bShowArticleName = *(BOOL*)dwPropVal;
	else
		*(BOOL*)dwPropVal = m_bShowArticleName;
}
//------------------------------------------------------------------------
// 物品名字的颜色
// 设置或返回: WPixel*
void CStaticObjArticle::propNameColor(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
		m_clrName = *(WPixel*)dwPropVal;
	else
		*(WPixel*)dwPropVal = m_clrName;
}
//------------------------------------------------------------------------