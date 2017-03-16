/**
*	created:		2012-7-2   1:38
*	filename: 		AniObj
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/AniObj.h"
#include "../Include/ResLoader.h"
#include "../Include/PalMgr.h"
//------------------------------------------------------------------------
// 所有支持的命令
CAniObj::ProcCommand CAniObj::m_Command[AniCmdMaxCommandCount] =
{
	NULL,
	&CAniObj::cmdSetProperty,
	&CAniObj::cmdGetProperty,
};
//------------------------------------------------------------------------
// 所有支持的属性
CAniObj::ProcProperty CAniObj::m_Property[AniPropMaxPropCount] =
{
	NULL,
	&CAniObj::propName,
	&CAniObj::propNameColor,
};
//------------------------------------------------------------------------
CAniObj::CAniObj()
{
	m_dwResID = 0;
	m_pPoolNodeObj = NULL;
	(__int64&)m_ptAnchor = 0;
	(__int64&)m_ptSortOff1 = 0;
	(__int64&)m_ptSortOff2 = 0;
	m_dwData = 0;
	m_pCurPal = NULL;
}
//------------------------------------------------------------------------
CAniObj::~CAniObj()
{
}
//------------------------------------------------------------------------
void CAniObj::Close()
{
	if (m_pPoolNodeObj)
	{
		g_ResLoader.ReleaseRes(ioAni + m_dwResID);
		m_pPoolNodeObj = NULL;
	}
	m_dwResID = 0;

	RemoveFromTimeAxis();
}
//------------------------------------------------------------------------
void CAniObj::OnTimer(DWORD dwEventID)
{
	m_AniContext.NextFrame();
}
//------------------------------------------------------------------------
CAni* CAniObj::RequestRes()
{
	if (m_pPoolNodeObj)
		return m_pPoolNodeObj;
	
	m_pPoolNodeObj = static_cast<CAni*>(g_ResLoader.RequestAni(m_dwResID));
	if (!m_pPoolNodeObj)
		return NULL;
	
	// initialize ani context
	if (m_pPoolNodeObj)
		m_AniContext.nFrames = m_pPoolNodeObj->GetTotalFrames();
	return m_pPoolNodeObj;
}
//------------------------------------------------------------------------
void CAniObj::GetOldDirtyRect(list<RECT> &DirtyRectList)
{
	if(!m_bNeedSetOldDirty)
		return;

	m_bNeedSetOldDirty = FALSE;
	
	DirtyRectList.push_back(m_rcAreaOld);
}
//------------------------------------------------------------------------
void CAniObj::SetDirty(ICanvas* pCanvas, const POINT& ptViewTop)
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
		// 如果还是画的同一祯
		if(m_nFrameOld == m_AniContext.CurFrame())
		{
			if(EqualRect(&rcAreaNew, &m_rcAreaOld))	// 如果和上次画的位置一样则不用处理了						
				return;
		}
		else
		{
			m_nFrameOld = m_AniContext.CurFrame();
		}

		// 如果画的不是同一祯则把上次和这次的矩阵设脏
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
void CAniObj::OnDraw(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag)
{
	if(!pCanvas || !RequestRes()) return;
	int offx, offy;
	m_pPoolNodeObj->GetOffset(m_AniContext.CurFrame(), offx, offy);
	m_pPoolNodeObj->Draw(pCanvas, 
		ptTileCenter.x - m_ptAnchor.x + offx, 
		ptTileCenter.y - m_ptAnchor.y + offy, 
		m_AniContext.CurFrame(), dwFlag, m_pCurPal);
}
//------------------------------------------------------------------------
void CAniObj::OnDrawTopmostObject(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag)
{
	/*
#ifdef _USE
	if (IsSelected())
	{
		DWORD dwFlags = CMapItemView::GetFlag();
		WPixel clr = WPixel(0,0,255);
		WPixel clrText = WPixel(0,255,0);
		char buf[MAX_PATH];
		POINT ptTile;
		g_pMapView->ScreenToTile(ptTileCenter, ptTile);

		if (dwFlags & flagInfo)
		{
			IResObject* pObj = g_ResLoader.m_pScpAni->GetResObject(m_dwResID);
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
			int offx, offy;
			m_pPoolNodeObj->GetOffset(m_AniContext.CurFrame(), offx, offy);
			rcTip.left = ptTileCenter.x - m_ptAnchor.x - 1 + offx;
			rcTip.right = rcTip.left + nW + 2;
			rcTip.top = ptTileCenter.y - m_ptAnchor.y - nH - 2 + offy;
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
BOOL CAniObj::OnSerialize(BOOL bWrite,IArchive* pArchive)
{
	if(bWrite)
	{
		SAniObjArchive a = {
			m_dwResID, 
		};

		pArchive->Write((char*)&a, sizeof(a));
		return TRUE;
	}
	else
	{
		Close();
		if (!CreateItem((SAniObjArchive*)pArchive->GetBuffer()))
			return FALSE;
		pArchive->Seek(sizeof(SAniObjArchive));
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CAniObj::OnCreateItem(const char* buf, int nLen)
{
	Close();
	return CreateItem((SAniObjArchive*)buf);
}
//------------------------------------------------------------------------
BOOL CAniObj::CreateItem(SAniObjArchive* pArchive)
{
	ASSERT(pArchive);
	m_dwResID = pArchive->dwResID;

	OnUpdate();
	
#ifdef _DEBUG
	if (!RequestRes())
		return FALSE;
#endif
	return TRUE;
}
//------------------------------------------------------------------------
void CAniObj::OnUpdate()
{
	SAniObjCreateParam aocp;
	memset(&aocp, 0, sizeof(aocp));
	g_ResLoader.GetAniObjCreateParam(m_dwResID, aocp);
	m_ptAnchor = aocp.ptAnchor;
	m_AniContext.nDelay = aocp.nDelay;
	m_AniContext.nDelay1 = aocp.nDelay1;
	m_ptSortOff1 = aocp.ptSortOff1;
	m_ptSortOff2 = aocp.ptSortOff2;
}
//------------------------------------------------------------------------
void CAniObj::SetTile(POINT& ptTile, IMapView* pMapView)
{
	m_ptTile = ptTile;
	UpdatePosInfo(pMapView);
}
//------------------------------------------------------------------------
void CAniObj::UpdatePosInfo(IMapView* pMapView)
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
BOOL CAniObj::OnEnterViewport(BOOL bVisable)
{
	if (bVisable)
	{
		if (!RequestRes())
			return FALSE;
		
		AddToTimeAxis();
	}
	else if (m_pPoolNodeObj)
	{
		SetSelected(FALSE); // 先恢复原来的调色板
		if (!g_ResLoader.ReleaseRes(ioAni + m_dwResID))
			return FALSE;
		m_pPoolNodeObj = NULL;

		RemoveFromTimeAxis();
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CAniObj::HitTest(const RECT& rcScreen)
{
	if (!m_pPoolNodeObj)
		return FALSE;
	IBitmap* pBitmap = m_pPoolNodeObj->GetBitmap(m_AniContext.CurFrame());
	if (!pBitmap)
		return FALSE;

	RECT rcSelect = rcScreen;
	int offx, offy;
	m_pPoolNodeObj->GetOffset(m_AniContext.CurFrame(), offx, offy);
	OffsetRect(&rcSelect, m_ptAnchor.x-offx, m_ptAnchor.y-offy);
	RECT rcBmp = {0, 0, pBitmap->GetWidth(), pBitmap->GetHeight()};
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
			if (!pBitmap->IsTransparent(ptSelect))
				return TRUE;
		}
	}
	
	return FALSE;
}
//------------------------------------------------------------------------
RECT CAniObj::GetShowRect()
{
	RECT rc = {0,0,0,0};
	int offx, offy;
	if(m_pPoolNodeObj)
	{
		m_pPoolNodeObj->GetOffset(m_AniContext.CurFrame(), offx, offy);
		rc.left = -m_ptAnchor.x + offx;
		rc.top = -m_ptAnchor.y + offy;
		rc.right = rc.left + m_pPoolNodeObj->GetWidth(m_AniContext.CurFrame());
		rc.bottom = rc.top + m_pPoolNodeObj->GetHeight(m_AniContext.CurFrame());
	}
	return rc;
}
//------------------------------------------------------------------------
IBitmap* CAniObj::GetBitmap()
{
	RequestRes();
	if (m_pPoolNodeObj)
		return m_pPoolNodeObj->GetBitmap(m_AniContext.nCurFrame);
	return NULL;
}
//------------------------------------------------------------------------
BOOL CAniObj::SetSelected(BOOL bSelected, BOOL bHighLight)
{
	if (!m_pPoolNodeObj) return FALSE;
	if (bSelected)
	{
		if (bHighLight)
			m_pCurPal = g_PalMgr.RequestPal(GetOffsetID(), 0, -1,
				m_pPoolNodeObj->GetPal(), m_pPoolNodeObj->GetUseColor())->GetPalette();
#ifdef _DEBUG
		AddFlag(CMapItemView::flagSelected);
#endif
	}
	else
	{
		m_pCurPal = m_pPoolNodeObj->GetPal();
#ifdef _DEBUG
		RemoveFlag(CMapItemView::flagSelected);
#endif
	}
	return TRUE;
}
//------------------------------------------------------------------------
void CAniObj::AddToTimeAxis()
{
	if (m_AniContext.nDelay > 0)
	{
		g_pTimeAxis->SetTimer(0,
			static_cast<ITimerSink*>(this),
			m_AniContext.nDelay,
			"BasicSys!!CAniObj::AddToTimeAxis");	
	}
}
//------------------------------------------------------------------------
void CAniObj::RemoveFromTimeAxis()
{
	if (m_AniContext.nDelay)
		g_pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));
}
//------------------------------------------------------------------------
// 各种指令的执行
int CAniObj::OnCommand(DWORD dwCmdID, DWORD dwParam1,DWORD dwParam2)
{
	if (dwCmdID >= AniCmdMaxCommandCount)
	{
		TraceLn("ERROR: CAniObj::OnCommand(): Unknow Command ID = %d", dwCmdID);
		return FALSE;
	}
	if (m_Command[dwCmdID] == NULL)
	{
		TraceLn("ERROR: CAniObj::OnCommand(): Command is not implemented, ID = %d", dwCmdID);
		return FALSE;
	}
	
	return (this->*m_Command[dwCmdID])(dwParam1, dwParam2);
}
//------------------------------------------------------------------------
// 动画对象的属性设置
// dwParam1: 属性类型
// dwParam2: 设置值(参看动画对象的属性定义)
int CAniObj::cmdSetProperty(DWORD dwParam1, DWORD dwParam2)
{
	if (dwParam1 >= AniPropMaxPropCount)
	{
		TraceColorLn(MY_TRACE_COLOR, "CAniObj::cmdSetProperty(), Unknow Property ID = %d", dwParam1);
		return FALSE;
	}
	if (m_Property[dwParam1] == NULL)
		return FALSE;
	
	(this->*m_Property[dwParam1])(TRUE, dwParam2);
	return TRUE;
}
//------------------------------------------------------------------------
// 获取动画对象的属性
// dwParam1: 属性类型
// dwParam2: 返回值(参看动画对象的属性定义)
int CAniObj::cmdGetProperty(DWORD dwParam1, DWORD dwParam2)
{
	if (dwParam1 >= AniPropMaxPropCount)
	{
		TraceColorLn(MY_TRACE_COLOR, "CAniObj::cmdSetProperty(), Unknow Property ID = %d", dwParam1);
		return FALSE;
	}
	if (m_Property[dwParam1] == NULL)
		return FALSE;
	
	(this->*m_Property[dwParam1])(FALSE, dwParam2);
	return TRUE;
}
//------------------------------------------------------------------------
void CAniObjDiscrete::Close()
{
	if (m_pPoolNodeObj)
	{
		g_ResLoader.ReleaseRes(ioAni + m_dwResID);
		m_pPoolNodeObj = NULL;
	}
	m_dwResID = 0;
	
	if (m_nTimerObjType == 0)
		RemoveFromTimeAxis();
	else
	{
		if (m_AniContext.nDelay1)
			g_pTimeAxis->KillTimer(1, static_cast<ITimerSink*>(this));
	}
}
//------------------------------------------------------------------------
void CAniObjDiscrete::OnTimer(DWORD dwEventID)
{
	if (m_nTimerObjType == 0)
	{
		m_AniContext.NextFrame();
		if (m_AniContext.CurFrame() == 0)
		{
			if (m_AniContext.nDelay1 > 0)
			{
				RemoveFromTimeAxis();

				g_pTimeAxis->SetTimer(1,
					static_cast<ITimerSink*>(this),
					m_AniContext.nDelay1,
					"BasicSys!!CAniObjDiscrete::OnTimer");

				m_nTimerObjType = 1;
			}
		}
	}
	else
	{
		if (m_AniContext.nDelay1)
		{
			g_pTimeAxis->KillTimer(1, static_cast<ITimerSink*>(this));
			AddToTimeAxis();
			m_nTimerObjType = 0;
		}
	}
}
//------------------------------------------------------------------------
void CAniObjDiscrete::SetDirty(ICanvas* pCanvas, const POINT& ptViewTop)
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
		// 如果还是画的同一祯
		if(m_nFrameOld == m_AniContext.CurFrame())
		{
			if(m_nTimerObjType == 0 && EqualRect(&rcAreaNew, &m_rcAreaOld))	// 如果和上次画的位置一样则不用处理了						
				return;
		}
		else
		{
			m_nFrameOld = m_AniContext.CurFrame();
		}

		// 如果画的不是同一祯则把上次和这次的矩阵设脏
		// 先把上次旧的画图区域设脏
		OffsetRect(&m_rcAreaOld,
			-ptViewTop.x,
			-ptViewTop.y);
		pCanvas->AddDirtyRect(m_rcAreaOld);
	}

	if(m_nTimerObjType != 0 || !RequestRes())
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
void CAniObjDiscrete::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if(m_nTimerObjType == 0)
		CAniObj::OnDraw(pCanvas, ptTileCenter, dwFlag);
}
//------------------------------------------------------------------------
BOOL CAniObjMultiOcc::SetOccupantInfo()
{
	char buffer[4096];
	unsigned char buf[2048];
	COccupantTileList *pL = (COccupantTileList*)buffer;
	BOOL nLen = g_ResLoader.GetAniObjOccupantInfo(m_dwResID, buf, 2048);
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
void CAniObjMultiOccDiscrete::Close()
{
	if (m_pPoolNodeObj)
	{
		g_ResLoader.ReleaseRes(ioAni + m_dwResID);
		m_pPoolNodeObj = NULL;
	}
	m_dwResID = 0;
	
	if (m_nTimerObjType == 0)
		RemoveFromTimeAxis();
	else
	{
		if (m_AniContext.nDelay1)
			g_pTimeAxis->KillTimer(1, static_cast<ITimerSink*>(this));
	}
}
//------------------------------------------------------------------------
void CAniObjMultiOccDiscrete::OnTimer(DWORD dwEventID)
{
	if (m_nTimerObjType == 0)
	{
		m_AniContext.NextFrame();
		if (m_AniContext.CurFrame() == 0)
		{
			if (m_AniContext.nDelay1 > 0)
			{
				RemoveFromTimeAxis();

				g_pTimeAxis->SetTimer(1,
					static_cast<ITimerSink*>(this),
					m_AniContext.nDelay1,
					"BasicSys!!CAniObjMultiOccDiscrete::OnTimer");

				m_nTimerObjType = 1;
			}
		}
	}
	else
	{
		if (m_AniContext.nDelay1)
		{
			g_pTimeAxis->KillTimer(1, static_cast<ITimerSink*>(this));
			AddToTimeAxis();
			m_nTimerObjType = 0;
		}
	}
}
//------------------------------------------------------------------------
void CAniObjMultiOccDiscrete::SetDirty(ICanvas* pCanvas, const POINT& ptViewTop)
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
		// 如果还是画的同一祯
		if(m_nFrameOld == m_AniContext.CurFrame())
		{
			if(m_nTimerObjType == 0 && EqualRect(&rcAreaNew, &m_rcAreaOld))	// 如果和上次画的位置一样则不用处理了						
				return;
		}
		else
		{
			m_nFrameOld = m_AniContext.CurFrame();
		}

		// 如果画的不是同一祯则把上次和这次的矩阵设脏
		// 先把上次旧的画图区域设脏
		OffsetRect(&m_rcAreaOld,
			-ptViewTop.x,
			-ptViewTop.y);
		pCanvas->AddDirtyRect(m_rcAreaOld);
	}

	if(m_nTimerObjType != 0 || !RequestRes())
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
void CAniObjMultiOccDiscrete::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if (m_nTimerObjType == 0)
		CAniObjMultiOcc::OnDraw(pCanvas, ptTileCenter, dwFlag);
}
//------------------------------------------------------------------------
// 名字
// 设置或返回: char*
void CAniObjTransport::propName(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
		lstrcpyn(m_szName, (LPCSTR)dwPropVal, MAX_ITEM_NAME_LENGTH);
	else
		lstrcpyn((char*)dwPropVal, m_szName, MAX_ITEM_NAME_LENGTH);
}
//------------------------------------------------------------------------
// 物品名字的颜色
// 设置或返回: WPixel*
void CAniObjTransport::propNameColor(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
		m_clrName = *(WPixel*)dwPropVal;
	else
		*(WPixel*)dwPropVal = m_clrName;
}
//------------------------------------------------------------------------
void CAniObjTransport::OnDrawTopmostObject(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
#ifndef _DEBUG
	DWORD dwFlags = CMapItemView::GetFlag();
	if (dwFlags & flagName)
	{
		int nHalfWidth = g_pFontWithOutline->GetLength(m_szName) / 2;
		int h = m_pPoolNodeObj->GetBitmap(0)->GetHeight();
		int x = ptTileCenter.x  - nHalfWidth;
		int y = ptTileCenter.y - h - 10;
		pCanvas->DrawText(g_pFontWithOutline, x, y, m_szName, m_clrName, FONT_NOALPHA);
	}
#else
	CAniObj::OnDrawTopmostObject(pCanvas, ptTileCenter, dwFlag);
#endif
};
//------------------------------------------------------------------------
BOOL CAniObjTransport::GetVisibleOnMinmap()
{
	return TRUE;
}
//------------------------------------------------------------------------
// 设置在小地图上的颜色
void CAniObjTransport::SetFlagColorOnMinmap(DWORD dwColor)
{
	m_wpMinMapFlagColor = WPixel(dwColor);
}
//------------------------------------------------------------------------
// 取得在小地图上的颜色
DWORD CAniObjTransport::GetFlagColorOnMinmap()
{
	return m_wpMinMapFlagColor;
}
//------------------------------------------------------------------------