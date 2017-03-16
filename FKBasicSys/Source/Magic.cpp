/**
*	created:		2012-7-2   1:28
*	filename: 		Magic
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/Magic.h"
#include "../../FKGame/MapInfo/Tile.h"
#include "../Include/CreatureView.h"
//------------------------------------------------------------------------
// 当有阻挡而返回时
int line(int x1, int y1, int x2, int y2, POINT* pptArray, 
		 int& nLen, int nMaxLen, bool bTestTile, int nTestTileMode)
{	
	int nRtn=1;
	POINT ptTile;
	int step_x,step_y,dx,dy,dxa,dyb,x,y,pk,twodxa,twodyb;
	x = x1;
	y = y1;
	dx = x2 - x1;
	dy = y2 - y1; 

	if (dx > 0)	
	{
		step_x = 1;
		dxa = dx;
	}
	else if (dx < 0)
	{
		step_x = -1;
		dxa = -dx;
	}
	else // 垂直线
	{
		if (dy > 0)			step_y = 1;
		else if (dy < 0)	step_y = -1;
		else // 两个点重叠
			return false;

		while (y != y2)
		{
			y += step_y;
			ptTile.x = x,ptTile.y = y;
			if (bTestTile)
			{
				nRtn = _test_tile_block(ptTile, nTestTileMode);
				if (nRtn < 1) goto end;
			}
			pptArray[nLen++] = ptTile;
			if (nLen >= nMaxLen)
				goto end;
		}
		return true;
	}

	if (dy > 0)
	{
		step_y = 1;
		dyb = dy;
	}
	else if (dy < 0)
	{
		step_y = -1;
		dyb = -dy;
	}
	else // 水平线
	{
		while (x != x2)
		{
			x += step_x;
			ptTile.x = x,ptTile.y = y;
			if (bTestTile)
			{
				nRtn = _test_tile_block(ptTile, nTestTileMode);
				if (nRtn < 1) goto end;
			}
			pptArray[nLen++] = ptTile;
			if (nLen >= nMaxLen)
				goto end;
		}
		return true;
	}

	twodxa = dxa<<1;
	twodyb = dyb<<1;
	if (dxa >= dyb)
	{
		int twodyb_dxa = twodyb - twodxa;
		pk = twodyb - dxa;
		while (x != x2)
		{
			x += step_x;
			if (pk < 0)
			{
				pk += twodyb;
				ptTile.x = x,ptTile.y = y;
				if (bTestTile)
				{
					nRtn = _test_tile_block(ptTile, nTestTileMode);
					if (nRtn < 1) goto end;
				}
				pptArray[nLen++] = ptTile;
				if (nLen >= nMaxLen)
					goto end;
			}
			else
			{
				pk += twodyb_dxa;
				y += step_y;
				ptTile.x = x,ptTile.y = y;
				if (bTestTile)
				{
					nRtn = _test_tile_block(ptTile, nTestTileMode);
					if (nRtn < 1) goto end;
				}
				pptArray[nLen++] = ptTile;
				if (nLen >= nMaxLen)
					goto end;
			}
		}
	}
	else
	{
		int twodxa_dyb = twodxa - twodyb;
		pk = twodxa - dyb;
		while (y != y2)
		{
			y += step_y;
			if (pk < 0)
			{
				pk += twodxa;
				ptTile.x = x,ptTile.y = y;
				if (bTestTile)
				{
					nRtn = _test_tile_block(ptTile, nTestTileMode);
					if (nRtn < 1) goto end;
				}
				pptArray[nLen++] = ptTile;
				if (nLen >= nMaxLen)
					goto end;
			}
			else
			{
				pk += twodxa_dyb;
				x += step_x;
				ptTile.x = x,ptTile.y = y;
				if (bTestTile)
				{
					nRtn = _test_tile_block(ptTile, nTestTileMode);
					if (nRtn < 1) goto end;
				}
				pptArray[nLen++] = ptTile;
				if (nLen >= nMaxLen)
					goto end;
			}
		}
	}
	return 1;
end:
	if (nRtn == 0) // 将最后一个格挡的TILE也加入列表
		pptArray[nLen++] = ptTile;
	return nRtn;
}
//------------------------------------------------------------------------
// 返回值：无效块(-1),有蔓延格挡(0),无蔓延格挡(1)
int _test_tile_spread(POINT& ptTile)
{
	CTile* pTile;
	if (!g_pMapView->IsValidTile(ptTile))
		return -1;
	if ((pTile = g_pMapView->GetTile(ptTile)) != NULL && !pTile->IsDiffuseBlock())
		return 1;
	return 0;
}
//------------------------------------------------------------------------
// 返回值：无效块(-1),有飞行格挡(0),无飞行格挡(1)
int _test_tile_block(POINT& ptTile, int nBlockTestMode)
{
	CTile* pTile = NULL;
	if (!g_pMapView->IsValidTile(ptTile)) return -1;
	if ((pTile = g_pMapView->GetTile(ptTile)) != NULL)
	{
		// normal_block
		if (nBlockTestMode == 1)
			if (pTile->IsBlock_Surface()) return 0;

		// fly_block_test
		if (pTile->IsFlyBlock()) return 0;
	}
	return 1;
}
//------------------------------------------------------------------------
// 魔法管理器基类
CMagicMgrBase::CMagicMgrBase()
{
	m_bBeginDamage = FALSE;
	m_bBeginAttack = FALSE;
	m_pMasterView = NULL;
	m_pAttackView = NULL;
	m_nMagicIDBeHit = 0;
	m_nSoundIDBeHit = 0;
	m_nCheckID = -1;
	m_nAttackDelay = MAGIC_MGR_DEFAULT_ATTACK_DELAY_TIME;
	m_vDamageData.clear();
	m_FlowDamNum = 0;
}
//------------------------------------------------------------------------
CMagicMgrBase::~CMagicMgrBase()
{
	FlowAllDamage();

	if(m_pMasterView != NULL)
	{
		m_pMasterView->DelMagic(this);
		m_pMasterView->DelLinkView(&m_pMasterView);
	}

	if(m_pAttackView != NULL)
		m_pAttackView->DelLinkView(&m_pAttackView);
}
//------------------------------------------------------------------------
// 设置魔法管理器的主人
void CMagicMgrBase::SetMasterView(CCreatureView	*pView)
{
	if(pView == NULL)
		return;
	m_pMasterView = pView;
	m_pMasterView->AddLinkView(&m_pMasterView);
}
//------------------------------------------------------------------------
// 设置魔法要攻击的对象
void CMagicMgrBase::SetAttackView(CCreatureView	*pView)
{
	if(pView == NULL)
		return;
	m_pAttackView = pView;
	m_pAttackView->AddLinkView(&m_pAttackView);
}
//------------------------------------------------------------------------
// 飘多少个伤害
void CMagicMgrBase::FlowDamage(int nNum)
{
	while(!m_vDamageData.empty() && nNum-- > 0)
	{
		SDamageData *pDamageData = m_vDamageData.front();
		if(pDamageData != NULL)
		{
			if (pDamageData->pView != NULL) // 存在
			{
				m_FlowDamNum++;
				pDamageData->pView->FlowHP(pDamageData->FlowParam, m_pMasterView, m_nMagicIDBeHit, m_nSoundIDBeHit);
				pDamageData->pView->DelLinkView(&(pDamageData->pView));
			}
			m_vDamageData.pop_front();
			delete pDamageData;
		}
		else
		{
			m_vDamageData.pop_front();
		}
	}
}
//------------------------------------------------------------------------
// 返回当前飘了多少个伤害
int CMagicMgrBase::GetFlowDamageNum()
{
	return m_FlowDamNum;
}
//------------------------------------------------------------------------
// 飘伤害
void CMagicMgrBase::FlowAllDamage()
{
	int nSize = m_vDamageData.size();
	if(nSize <= 0)
		return;

	FlowDamage(nSize);
}
//------------------------------------------------------------------------
// 添加进一个伤害包
void CMagicMgrBase::AddDamage(CCreatureView *pView, SFlowParam FlowParam)
{
	ASSERT(pView);

	if(m_bBeginDamage)
		pView->FlowHP(FlowParam, m_pMasterView, m_nMagicIDBeHit, m_nSoundIDBeHit);
	else
	{
		SDamageData * pdata = new SDamageData;
		pdata->FlowParam = FlowParam;
		pdata->pView = pView;
		pdata->pView->AddLinkView(&(pdata->pView));
		m_vDamageData.push_back(pdata);
	}
}
//------------------------------------------------------------------------
// 表示魔法可以伤害了，以后添加的伤害都马上飘伤害
void CMagicMgrBase::BeginDamage()
{
	m_bBeginDamage = TRUE;
	FlowAllDamage();
}
//------------------------------------------------------------------------
// 获取最前面的伤害数据
SDamageData* CMagicMgrBase::GetFrontDamageData()
{
	while(!m_vDamageData.empty())
	{
		SDamageData *pDamageData = m_vDamageData.front();
		if(pDamageData == NULL)
		{
			m_vDamageData.pop_front();
			continue;
		}
		if(pDamageData->pView == NULL)
		{
			m_vDamageData.pop_front();
			delete pDamageData;
			continue;
		}
		return pDamageData;
	}
	return NULL;
}
//------------------------------------------------------------------------
// 删掉最前面的一个伤害
void CMagicMgrBase::DelFrontDamage()
{
	if(!m_vDamageData.empty())
	{
		SDamageData *pDamageData = m_vDamageData.front();
		m_vDamageData.pop_front();
		delete pDamageData;
	}
}
//------------------------------------------------------------------------
// 魔法基类
CMagic::CMagic()
{
	m_dwResID = 0;
	m_pPoolNodeObj = NULL;
	(__int64&)m_ptAnchor = 0;
	(__int64&)m_ptTile = 0;
	m_pMgr = NULL;
	m_dwSoundResID = 0;
	m_bSoundLoop = FALSE;
	m_nSoundChunnelID = -1;
}
//------------------------------------------------------------------------
CMagic::CMagic(CMagicMgrBase* pMgr)
{
	m_dwResID = 0;
	m_pPoolNodeObj = NULL;
	(__int64&)m_ptOffset = 0;
	(__int64&)m_ptAnchor = 0;
	(__int64&)m_ptTile = 0;
	SetMgr(pMgr);
}
//------------------------------------------------------------------------
CMagic::~CMagic()
{
	Close();
}
//------------------------------------------------------------------------
void CMagic::Close()
{
	if (m_pPoolNodeObj)
	{
		g_ResLoader.ReleaseRes(ioMagic + m_dwResID);
		m_pPoolNodeObj = NULL;
	}

	//下面这个很危险
	if (m_pMgr)
	{
		m_pMgr->OnPlayDone(this, m_ptTile);
		m_pMgr = NULL;
	}
	
	if (m_ptTile.x != 0 || m_ptTile.y != 0) // 说明加入了地图，加入了时间轴
	{
		RemoveFromMapView();
	}
	RemoveFromTimerAxis();
	(__int64&)m_ptAnchor = 0;
	(__int64&)m_ptTile = 0;
}
//------------------------------------------------------------------------
void CMagic::OnTimer(DWORD dwEventID)
{
	if (++m_AniContext.nCurFrame >= m_AniContext.nFrames)
	{
		m_AniContext.SetCurFrame(0);
		if (m_pMgr)
		{
			m_pMgr->OnPlayDone(this, m_ptTile);
			m_pMgr = NULL;
		}
		return;
	}
}
//------------------------------------------------------------------------
CAni* CMagic::RequestRes()
{
	if (m_pPoolNodeObj)
		return m_pPoolNodeObj;
	
	m_pPoolNodeObj = static_cast<CAni*>(g_ResLoader.RequestMagic(m_dwResID));
	if (m_pPoolNodeObj)
		m_AniContext.nFrames = m_pPoolNodeObj->GetTotalFrames();
	return m_pPoolNodeObj;
}
void CMagic::GetOldDirtyRect(list<RECT> &DirtyRectList)
{
	if(!m_bNeedSetOldDirty)
		return;

	m_bNeedSetOldDirty = FALSE;
	
	DirtyRectList.push_back(m_rcAreaOld);
}
//------------------------------------------------------------------------
void CMagic::SetDirty(ICanvas* pCanvas, const POINT& ptViewTop)
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
void CMagic::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if (!pCanvas || !RequestRes()) return;
	int offx, offy;
	m_pPoolNodeObj->GetOffset(m_AniContext.CurFrame(), offx, offy);
	m_pPoolNodeObj->Draw(pCanvas, 
		ptTileCenter.x - m_ptAnchor.x + offx + m_ptOffset.x, 
		ptTileCenter.y - m_ptAnchor.y + offy + m_ptOffset.y,
		m_AniContext.CurFrame(), dwFlag);
}
//------------------------------------------------------------------------
void CMagic::OnDrawTopmostObject(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag)
{
#ifdef _DEBUG
	if (IsSelected())
	{
		DWORD dwFlags = CMapItemView::GetFlag();
		WPixel clr = WPixel(255,0,255);
		WPixel clrText = WPixel(0,0,255);
		char buf[MAX_PATH];
		POINT ptTile;
		g_pMapView->ScreenToTile(ptTileCenter, ptTile);
		
		if (dwFlags & flagInfo)
		{
			IResObject* pObj = g_ResLoader.m_pScpMagic->GetResObject(m_dwResID);
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
}
//------------------------------------------------------------------------
BOOL CMagic::OnSerialize(BOOL bWrite,IArchive* pArchive)
{
	ASSERT(pArchive);
	if (bWrite)
	{
		SMagicArchive a = {
			m_dwResID,
			(__int64&)m_ptAnchor,
			(__int64&)m_ptOffset,
			m_AniContext.nDelay,
			m_nLayerValue,
			m_dwSoundResID,
			m_bSoundLoop
		};

		return pArchive->Write((char*)&a, sizeof(a));
	}
	else
	{
		if (!OnCreateItem((const char*)pArchive->GetBuffer(), (int)pArchive->GetLeftBufferLen()))
		{
			TraceLn("ERROR: CMagic::OnSerialize(bRead), create item failed!");
		}
		pArchive->Seek(sizeof(SMagicArchive));
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMagic::OnCreateItem(const char* buf, int nLen)
{	
	ASSERT(buf);
	SMagicArchive st;
	SMagicCreateParam mcp;
	g_ResLoader.GetMagicCreateParam(*(DWORD*)buf, mcp);
	st.dwResID = *(DWORD*)buf;
	st.ptAnchor.x = mcp.ptAnchor.x;
	st.ptAnchor.y = mcp.ptAnchor.y;
	st.ptOffset.x = mcp.ptOffset.x;
	st.ptOffset.y = mcp.ptOffset.y;
	st.nDelay = mcp.nDelay;
	CreateItem((const char*)&st, nLen);
	AddToTimerAxis();
	return TRUE;
}
//------------------------------------------------------------------------
// 不加入地图、不加入时间轴
BOOL CMagic::CreateItem(const char* buf, int nLen)
{
	SMagicArchive* pArchive	= (SMagicArchive*)buf;
	m_dwResID				= pArchive->dwResID;
	m_ptOffset.x			= pArchive->ptOffset.x;
	m_ptOffset.y			= pArchive->ptOffset.y;
	m_ptAnchor.x			= pArchive->ptAnchor.x;
	m_ptAnchor.y			= pArchive->ptAnchor.y;
	m_AniContext.nDelay		= pArchive->nDelay;
	m_nLayerValue			= pArchive->nLayerValue;
	
	m_dwSoundResID			= pArchive->dwSoundResID;
	m_bSoundLoop			= pArchive->bSoundLoop;

	if (!RequestRes())
		return FALSE;
	
	return TRUE;
}
//------------------------------------------------------------------------
void CMagic::SetTile(POINT& ptTile, IMapView* pMapView)
{
	m_ptTile = ptTile;
	UpdatePosInfo(pMapView);
}
//------------------------------------------------------------------------
void CMagic::UpdatePosInfo(IMapView* pMapView)
{
	m_rcArea.left = m_ptTile.x;
	m_rcArea.top  = m_ptTile.y;
	
	COccupantTileList* potl = GetOccupantTileList();
	if (potl == NULL)
	{
		m_rcArea.right = m_rcArea.left; 
		m_rcArea.bottom = m_rcArea.top;	// 是单点对象，右上角跟左下角一样
	}
	else
	{
		COccupantTileListHeader& octh = potl->GetPreData();
		m_rcArea.right = m_rcArea.left + octh.wWidth - 1;		// rcArea.right为平视右端点
		m_rcArea.bottom = m_rcArea.top + octh.wHeight - 1;
	}
	
	pMapView->TileToWorld((POINT&)m_rcArea, m_ptLeft);
	
	POINT ptRightTile = {m_rcArea.right, m_rcArea.bottom};
	pMapView->TileToWorld(ptRightTile, m_ptRight);
	
	pMapView->TileToWorld(m_ptTile, m_ptWold);
}
//------------------------------------------------------------------------
inline BOOL CMagic::AddToTimerAxis()
{
	if(m_AniContext.nDelay <= 0 )
		return FALSE;
	
	g_pTimeAxis->SetTimer(0,
		static_cast<ITimerSink*>(this),
		m_AniContext.nDelay,
		"BasicSys!!CMagic::AddToTimerAxis");

	return TRUE;
}
//------------------------------------------------------------------------
inline BOOL CMagic::AddToMapView(POINT& ptTile) 
{
	BOOL b = g_pMapView->AddItem(ptTile, this);
	if (b)
	{
		m_ptTile = ptTile;
		m_nSoundChunnelID = Fun_PlaySound(ptTile, m_dwSoundResID, m_bSoundLoop);
	}
	return b;
}
//------------------------------------------------------------------------
inline BOOL CMagic::RemoveFromTimerAxis()
{
	if (m_AniContext.nDelay)
	{
		g_pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));
		return TRUE;
	}
	g_pTimeAxis->KillTimer(1, static_cast<ITimerSink*>(this));
	g_pTimeAxis->KillTimer(2, static_cast<ITimerSink*>(this));
	return FALSE;
}
//------------------------------------------------------------------------
inline BOOL CMagic::RemoveFromMapView()
{
	Fun_StopSound(m_nSoundChunnelID);
	return g_pMapView->RemoveItem(m_ptTile, this);
}
//------------------------------------------------------------------------
void CMagic::SetItemData(DWORD dwData)
{
	m_ptTile = *(POINT*)dwData;
}
//------------------------------------------------------------------------
DWORD CMagic::GetLayerValue()
{
	switch(m_nLayerValue) 
	{
	case 0:
		return milvSpecificBlock + 1;
	case 1:
		return milvMidle;
	case 2:
		return milvHight;
	default:
		return milvMidle;
	}
}
//------------------------------------------------------------------------
DWORD CMagic::GetOccupantValue()
{
	return 0x0;
}
//------------------------------------------------------------------------
IBitmap* CMagic::GetBitmap() 
{
	RequestRes();

	if (m_pPoolNodeObj)
		return m_pPoolNodeObj->GetBitmap(m_AniContext.nCurFrame);

	return NULL;
}
//------------------------------------------------------------------------
POINT CMagic::GetTile()
{
	return m_ptTile;
}
//------------------------------------------------------------------------
DWORD CMagic::GetResID()
{
	return m_dwResID;
}
//------------------------------------------------------------------------
void CMagic::SetMgr(CMagicMgrBase* pMgr)
{
	m_pMgr = pMgr;
}
//------------------------------------------------------------------------
// 魔法进入屏幕视图区
BOOL CMagic::OnEnterViewport(BOOL bVisable)
{
	if (bVisable)
	{
		if (!RequestRes())
			return FALSE;
	}
	else if (m_pPoolNodeObj)
	{
		if (!g_ResLoader.ReleaseRes(ioMagic + m_dwResID))
			return FALSE;
		m_pPoolNodeObj = NULL;
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMagic::HitTest(const RECT& rcScreen)
{
#ifndef _USE
	return FALSE;
#else
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
#endif	
}
//------------------------------------------------------------------------
RECT CMagic::GetShowRect()
{
	RECT rc = {0,0,0,0};
	int offx, offy;
	if (m_pPoolNodeObj)
	{
		m_pPoolNodeObj->GetOffset(m_AniContext.CurFrame(), offx, offy);
		rc.left = -m_ptAnchor.x + offx + m_ptOffset.x;
		rc.top = -m_ptAnchor.y + offy + m_ptOffset.y;
		rc.right = rc.left + m_pPoolNodeObj->GetWidth(m_AniContext.CurFrame());
		rc.bottom = rc.top + m_pPoolNodeObj->GetHeight(m_AniContext.CurFrame());
	}
	return rc;
}
//------------------------------------------------------------------------
// 有生命期的魔法
void CMagicLife::OnTimer(DWORD dwEventID)
{
	if (m_nLifeLine == -1 || ++m_nCurLife < m_nLifeLine)
	{
		m_AniContext.NextFrame();
	}
	else // lifeline over
	{
		if (m_pMgr)
		{
			m_pMgr->OnPlayDone(this, m_ptTile);
			m_pMgr = NULL;
		}
		
		delete this;
		return;
	}
}
//------------------------------------------------------------------------
BOOL CMagicLife::OnCreateItem(const char* buf, int nLen)
{
	if (CMagic::CreateItem(buf, nLen))
	{
		m_nLifeLine	= ((SMagicArchive_Life*)buf)->nLifeLine;
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------
// 带有3个状态的魔法
BOOL CMagicThreeState::OnCreateItem(const char* buf, int nLen)
{
	if (CMagicLife::OnCreateItem(buf, nLen))
	{
		m_nLoopStartIndex = ((SMagicArchive_ThreeState*)buf)->nLoopStartIndex - 1;
		m_nLoopEndIndex = ((SMagicArchive_ThreeState*)buf)->nLoopEndIndex - 1;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
void CMagicThreeState::OnTimer(DWORD dwEventID)
{
	if (m_nState == stateCreate) // 启动阶段
	{
		if (++m_AniContext.nCurFrame >= m_nLoopStartIndex) 
			m_nState = stateLoop;
	}
	else if (m_nState == stateLoop) // 循环运行阶段
	{
		if (++m_AniContext.nCurFrame >= m_nLoopEndIndex)
			m_AniContext.SetCurFrame(m_nLoopStartIndex);

		if (++m_nCurLife >= m_nLifeLine - (m_AniContext.nFrames - m_nLoopEndIndex - 1))
			m_nState = stateRelease;
	}
	else if (m_nState == stateRelease) // 销毁阶段
	{
		if (++m_AniContext.nCurFrame >= m_AniContext.nFrames)
		{
			if (m_pMgr)
			{
				m_pMgr->OnPlayDone(this, m_ptTile);
				m_pMgr = NULL;
			}
			
			delete this;
			return;
		}
	}
}
//------------------------------------------------------------------------
// 流星雨式的魔法
BOOL CMagicRain::OnCreateItem(const char* buf, int nLen)
{
	if (CMagicThreeState::OnCreateItem(buf, nLen))
	{
		if(((SMagicArchive_Rain*)buf)->bNeedRandomOffset)
		{
			m_nOffX = rand() % 60 - 30;
			m_nOffY = rand() % 60 - 30;
		}
		else
		{
			m_nOffX = 0;
			m_nOffY = 0;
		}
		m_ptStepOffset = ((SMagicArchive_Rain*)buf)->ptStepOffset;
		m_ptOldOffset = m_ptOffset;
		m_nMoveTime = 0;
		m_bMove = TRUE;

		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CMagicRain::AddToTimerAxis()
{
	if(m_AniContext.nDelay <= 0 )
		return FALSE;
	
	g_pTimeAxis->SetTimer(0,
		static_cast<ITimerSink*>(this),
		m_AniContext.nDelay,
		"BasicSys!!CMagicRain::AddToTimerAxis");
		
	g_pTimeAxis->SetTimer(1,
		static_cast<ITimerSink*>(this),
		5,
		"BasicSys!!CMagicRain::AddToTimerAxis");

	return TRUE;
}
//------------------------------------------------------------------------
void CMagicRain::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if (!pCanvas || !RequestRes()) return;

	int offx, offy;
	m_pPoolNodeObj->GetOffset(m_AniContext.CurFrame(), offx, offy);
	m_pPoolNodeObj->Draw(pCanvas, 
		ptTileCenter.x - m_ptAnchor.x + offx + m_nOffX + m_ptOffset.x, 
		ptTileCenter.y - m_ptAnchor.y + offy + m_nOffY + m_ptOffset.y,
		m_AniContext.CurFrame(), dwFlag);
}
//------------------------------------------------------------------------
RECT CMagicRain::GetShowRect()
{
	RECT rc = {0,0,0,0};
	int offx, offy;
	if (m_pPoolNodeObj)
	{
		m_pPoolNodeObj->GetOffset(m_AniContext.CurFrame(), offx, offy);
		rc.left = -m_ptAnchor.x + offx + m_nOffX + m_ptOffset.x;
		rc.top = -m_ptAnchor.y + offy + m_nOffY + m_ptOffset.y;
		rc.right = rc.left + m_pPoolNodeObj->GetWidth(m_AniContext.CurFrame());
		rc.bottom = rc.top + m_pPoolNodeObj->GetHeight(m_AniContext.CurFrame());
	}
	return rc;
}
//------------------------------------------------------------------------
void CMagicRain::OnTimer(DWORD dwEventID)
{
	if(dwEventID == 0)
	{
		if (m_nState == 0) // 启动阶段
		{
			if (++m_AniContext.nCurFrame >= m_nLoopStartIndex) m_nState = 1;
		}
		else if (m_nState == 1) // 循环运行阶段
		{
			if (++m_AniContext.nCurFrame >= m_nLoopEndIndex)
				m_AniContext.SetCurFrame(m_nLoopStartIndex);
			
			if (++m_nCurLife >= m_nLifeLine - (m_AniContext.nFrames - m_nLoopEndIndex - 1))
			{
				m_AniContext.SetCurFrame(m_nLoopEndIndex + 1);
				if (m_pMgr)
				{
					m_pMgr->OnPlayExplode(this);
				}
				m_bMove = FALSE;
				m_nState = 2;
			}
		}
		else if (m_nState == 2) // 销毁阶段
		{
			if (++m_AniContext.nCurFrame >= m_AniContext.nFrames)
			{
				if (m_pMgr)
				{
					m_pMgr->OnPlayDone(this, m_ptTile);
					m_pMgr = NULL; 
				}
				
				delete this;
				return;
			}
		}
	}
	else
	{
		if(!m_bMove)
			return;

		m_nMoveTime++;
		m_ptOffset.x = m_ptOldOffset.x + m_ptStepOffset.x * 5 * m_nMoveTime / m_AniContext.nDelay;
		m_ptOffset.y = m_ptOldOffset.y + m_ptStepOffset.y * 5 * m_nMoveTime / m_AniContext.nDelay;
		if (m_ptOffset.y > 0)
		{
			m_ptOffset.y = 0;
			m_bMove = FALSE;
		}
	}
}
//------------------------------------------------------------------------
// 环形的魔法
void CMagicRing::OnTimer(DWORD dwEventID)
{
	if (--m_nDirs > 0 )
	{
		++m_nCurLife;
		m_AniContext.NextFrame();
	}
	else
	{
		if (m_pMgr)
		{
			m_pMgr->OnPlayDone(this, m_ptTile);
			m_pMgr = NULL;
		}
		
		delete this;
		return;
	}
}
//------------------------------------------------------------------------
BOOL CMagicRing::OnCreateItem(const char* buf, int nLen)
{
	if (CMagicLife::OnCreateItem(buf, nLen))
	{
		m_nFramesPerDir	= ((SMagicArchive_Ring*)buf)->nFramesPerDir;
		
		m_nDirs = m_nFramesPerDir;

		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
void CMagicRing::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	/*

	if (!pCanvas || !RequestRes()) return;
	
	int offx,offy,nFrame;

	for (int i=0; i<m_nNoBlockDirs; i++)
	{
		nFrame = m_aDirs[i]*m_nFramesPerDir + m_AniContext.CurFrame();
		
		m_pPoolNodeObj->GetOffset(nFrame, offx, offy);
		
		m_pPoolNodeObj->Draw(pCanvas, 
			ptTileCenter.x - m_ptAnchor.x + offx, 
			ptTileCenter.y - m_ptAnchor.y + offy,
			nFrame, dwFlag);
	}
	*/
}
//------------------------------------------------------------------------
void CMagicRing::SetDirsInfo(int* nDirList, int nDirCount)
{
	if (nDirList && nDirCount <= 32)
	{
		memcpy(m_aDirs, nDirList, nDirCount*sizeof(int));
		m_nNoBlockDirs = nDirCount;
	}
}
//------------------------------------------------------------------------
// 八个方向的魔法
void CMagicEightDir::OnTimer(DWORD dwEventID)
{
	ASSERT(m_nCurLife < m_nFramesPerDir + 2);
	if (m_nLifeLine == -1 || ++m_nCurLife < m_nFramesPerDir)
	{
		if (++m_AniContext.nCurFrame >= m_nEndFrameIndex)
			m_AniContext.SetCurFrame(m_nStartFrameIndex);
	}

	else
	{
		POINT ptPos = {0, 0};
		m_pMgr->OnPlayDone(this, ptPos);
		m_pMgr = NULL;

		delete this;
		return;
	}
}
//------------------------------------------------------------------------
DWORD CMagicEightDir::GetLayerValue()
{
	if (m_ptOffset.y < 0)
		return milvHight;

	return milvLowest;
}
//------------------------------------------------------------------------
BOOL CMagicEightDir::OnCreateItem(const char* buf, int nLen)
{
	if (CMagicLife::OnCreateItem(buf, nLen))
	{
		SMagicArchive_EightDir* pArchive = (SMagicArchive_EightDir*)buf;

		m_nFramesPerDir	= pArchive->nFramesPerDir;
		if (m_nFramesPerDir == 0)
			return FALSE;

		if(pArchive->nDir < 0 || pArchive->nDir >= 8)
			return FALSE;

		m_nStartFrameIndex = m_nFramesPerDir * pArchive->nDir;
		m_nEndFrameIndex = m_nStartFrameIndex + m_nFramesPerDir;
		m_AniContext.SetCurFrame(m_nStartFrameIndex);

		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
// 地上物品上闪烁的星星
void CMagicStar::OnTimer(DWORD dwEventID)
{
	if (++m_AniContext.nCurFrame >= m_AniContext.nFrames)
	{
		m_AniContext.SetCurFrame(0);
		m_nCount = 1;
	}
}
//------------------------------------------------------------------------
void CMagicStar::SetDirty(ICanvas* pCanvas, const POINT& ptViewTop)
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
			if(m_nCount == 0 && EqualRect(&rcAreaNew, &m_rcAreaOld))	// 如果和上次画的位置一样则不用处理了						
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

	if(m_nCount != 0 || !RequestRes())
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
void CMagicStar::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
		CMagicLife::OnDraw(pCanvas, ptTileCenter, dwFlag);
}
//------------------------------------------------------------------------
// 脚印
BOOL CMagicFootprint::OnCreateItem(const char* buf, int nLen)
{
	if (CMagicLife::OnCreateItem(buf, nLen))
	{
		SMagicArchive_FootPrint* pArchive = (SMagicArchive_FootPrint*)buf;
		m_nDirection = pArchive->nDirection;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
void CMagicFootprint::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if (!pCanvas || !RequestRes()) return;
	int offx, offy;
	m_pPoolNodeObj->GetOffset(m_nDirection, offx, offy);
	m_pPoolNodeObj->Draw(pCanvas, 
		ptTileCenter.x - m_ptAnchor.x + offx + m_ptOffset.x, 
		ptTileCenter.y - m_ptAnchor.y + offy + m_ptOffset.y,
		m_nDirection, dwFlag);
}
//------------------------------------------------------------------------
RECT CMagicFootprint::GetShowRect()
{
	RECT rc = {0,0,0,0};
	int offx, offy;
	if (m_pPoolNodeObj)
	{
		m_pPoolNodeObj->GetOffset(m_nDirection, offx, offy);
		rc.left = -m_ptAnchor.x + offx + m_ptOffset.x;
		rc.top = -m_ptAnchor.y + offy + m_ptOffset.y;
		rc.right = rc.left + m_pPoolNodeObj->GetWidth(m_nDirection);
		rc.bottom = rc.top + m_pPoolNodeObj->GetHeight(m_nDirection);
	}
	return rc;
}
//------------------------------------------------------------------------
DWORD CMagicFootprint::GetLayerValue()
{
	return milvSpecificBlock + 1;
}
//------------------------------------------------------------------------
void CMagicFootprint::SetDirection(int nDir)
{
	m_nDirection = nDir;
}
//------------------------------------------------------------------------
// 地上残留物
void CMagicLife_LeftObj::OnTimer(DWORD dwEventID)
{
	if (m_nLifeLine == -1 || ++m_nCurLife < m_nLifeLine)
	{
		m_AniContext.NextFrame();
		return;
	}

	if (m_pMgr)
	{
		m_nCurLife = 0;
		m_AniContext.SetCurFrame(0);
	}
}
//------------------------------------------------------------------------
// 单向射击类魔法，如火球等
inline BOOL CMagicSingleDir::AddToMapView(POINT& ptTile) 
{
	if(!CMagic::AddToMapView(ptTile))
		return FALSE;

	// 下面先偏移一段位置先
	POINT ptStep = Fun_CalcDirSpeedDefaultSpeed32_45(m_fOldDir);

	int nTemp = 4;
	while(nTemp-- > 0)
	{
		if(abs(m_ptTile.x - m_ptTileAttack.x) == 0 && abs(m_ptTile.y - m_ptTileAttack.y) == 0)
			break;

		// 计算飞行方向
		POINT ptCenter, ptTilePos;
		Tile_To_Screen(m_ptTile, ptCenter);
		ptCenter.x = ptCenter.x + m_ptTileOffset.x + ptStep.x;
		ptCenter.y = ptCenter.y + m_ptTileOffset.y + ptStep.y;

		POINT	ptTempTile;
		Screen_To_Tile(ptCenter, ptTempTile);
		Tile_To_Screen(ptTempTile, ptTilePos);
		m_ptTileOffset.x = ptCenter.x - ptTilePos.x;
		m_ptTileOffset.y = ptCenter.y - ptTilePos.y;
		g_pMapView->MoveItem(m_ptTile, ptTempTile, this);
		m_ptTile = ptTempTile;
	}

	return TRUE;
}
//------------------------------------------------------------------------
inline BOOL CMagicSingleDir::AddToTimerAxis()
{
	if(m_AniContext.nDelay <= 0 )
		return FALSE;
	
	g_pTimeAxis->SetTimer(0,
		static_cast<ITimerSink*>(this),
		m_AniContext.nDelay,
		"BasicSys!!CMagicSingleDir::AddToTimerAxis");

	return TRUE;
}

//------------------------------------------------------------------------
void CMagicSingleDir::OnTimer(DWORD dwEventID)
{
	if(m_nLife > 0)
	{
		if(m_nLen > m_nLife)
		{
			if (m_pMgr)
			{
				m_pMgr->OnPlayDone(this, m_ptTile);
				m_pMgr = NULL;
			}

			delete this;
			return;
		}
		m_nLen++;

		// 祯控制
		m_nStartFrameIdx = ((int)m_fOldDir) / m_nDirBI *m_nFramesPerDir;

		m_AniContext.NextFrame();
		if (m_AniContext.nCurFrame  >= m_nFramesPerDir) m_AniContext.nCurFrame = 0;
		
		// 计算速度
		POINT ptStep = Fun_CalcDirSpeedDefaultSpeed32_45(m_fOldDir);

		POINT ptCenter, ptTilePos, ptTempTile;
		Tile_To_Screen(m_ptTile, ptCenter);
		ptCenter.x = ptCenter.x + m_ptTileOffset.x + ptStep.x * 3 * m_AniContext.nDelay / m_nTimeTile;
		ptCenter.y = ptCenter.y + m_ptTileOffset.y + ptStep.y * 3 * m_AniContext.nDelay / m_nTimeTile;

		Screen_To_Tile(ptCenter, ptTempTile);
		Tile_To_Screen(ptTempTile, ptTilePos);
		m_ptTileOffset.x = ptCenter.x - ptTilePos.x;
		m_ptTileOffset.y = ptCenter.y - ptTilePos.y;
		g_pMapView->MoveItem(m_ptTile, ptTempTile, this);
		m_ptTile = ptTempTile;
	}
	else
	{
		// 如果有目标再更新一下目标的位置
		if( m_pAttackView != NULL )
			m_ptTileAttack = m_pAttackView->GetTile();
	
		if(abs(m_ptTile.x - m_ptTileAttack.x) <= 0 && abs(m_ptTile.y - m_ptTileAttack.y) <= 0 || m_nLen > MAX_LINE_PATH_LENGTH)
		{
			if (m_pMgr)
			{
				m_pMgr->OnPlayDone(this, m_ptTileAttack);
				m_pMgr = NULL;
			}

			delete this;
			return;
		}
		m_nLen++;
		
		// 计算飞行方向
		POINT ptCenter, ptAttackCenter, ptTilePos;
		Tile_To_Screen(m_ptTile, ptCenter);
		Tile_To_Screen(m_ptTileAttack, ptAttackCenter);
		ptCenter.x += m_ptTileOffset.x;
		ptCenter.y += m_ptTileOffset.y;

		float fNewDir = Fun_CalcDirection32(&ptCenter, &ptAttackCenter);

		Fun_CalcNewDir(m_fOldDir, fNewDir, m_fDirInc);
		m_fDirInc += 0.1f;

		// 祯控制
		m_nStartFrameIdx = ((int)m_fOldDir) / m_nDirBI *m_nFramesPerDir;

		m_AniContext.NextFrame();
		if (m_AniContext.nCurFrame  >= m_nFramesPerDir) m_AniContext.nCurFrame = 0;
		
		// 计算速度
		POINT ptStep = Fun_CalcDirSpeedDefaultSpeed32_45(m_fOldDir);

		ptCenter.x += ptStep.x * 3 * m_AniContext.nDelay / m_nTimeTile;
		ptCenter.y += ptStep.y * 3 * m_AniContext.nDelay / m_nTimeTile;

		POINT	ptTempTile;
		Screen_To_Tile(ptCenter, ptTempTile);
		Tile_To_Screen(ptTempTile, ptTilePos);
		m_ptTileOffset.x = ptCenter.x - ptTilePos.x;
		m_ptTileOffset.y = ptCenter.y - ptTilePos.y;
		g_pMapView->MoveItem(m_ptTile, ptTempTile, this);
		m_ptTile = ptTempTile;
	}
}
//------------------------------------------------------------------------
void CMagicSingleDir::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if (!pCanvas || !RequestRes()) return;
	int offx, offy;
	m_pPoolNodeObj->GetOffset(m_AniContext.nCurFrame+m_nStartFrameIdx, offx, offy);
	
	m_pPoolNodeObj->Draw(pCanvas, 
		m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + m_ptOffset.x + offx,
		m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + m_ptOffset.y + offy, 
		m_AniContext.nCurFrame+m_nStartFrameIdx, dwFlag);
}
//------------------------------------------------------------------------
RECT CMagicSingleDir::GetShowRect()
{
	RECT rc = {0,0,0,0};
	int offx, offy;
	if (m_pPoolNodeObj)
	{
		m_pPoolNodeObj->GetOffset(m_AniContext.nCurFrame + m_nStartFrameIdx, offx, offy);
		rc.left = m_ptTileOffset.x + - m_ptAnchor.x + m_ptOffset.x + offx;
		rc.top = m_ptTileOffset.y + - m_ptAnchor.y + m_ptOffset.y + offy;
		rc.right = rc.left + m_pPoolNodeObj->GetWidth(m_AniContext.nCurFrame + m_nStartFrameIdx);
		rc.bottom = rc.top + m_pPoolNodeObj->GetHeight(m_AniContext.nCurFrame + m_nStartFrameIdx);
	}
	return rc;
}

//------------------------------------------------------------------------
BOOL CMagicSingleDir::OnCreateItem(const char* buf, int nLen)
{
	if (!CMagic::CreateItem(buf, nLen))
		return FALSE;
	
	// 填充数据
	SMagicArchive_SingleDir* pArchive = (SMagicArchive_SingleDir*)buf;

	m_ptTileOffset		= pArchive->ptTileOffset;
	m_fOldDir			= pArchive->fDir;
	m_fDirInc			= pArchive->fDirInc;
	m_ptTile			= pArchive->ptStartTile;
	m_ptTileAttack		= pArchive->ptAttackTile;
	m_pAttackView		= pArchive->pAttackView;
	m_nFramesPerDir		= pArchive->nFramesPerDir;
	m_nDirBI			= 32 / pArchive->nDir;
	m_nStartFrameIdx	= (int)m_fOldDir / m_nDirBI * m_nFramesPerDir;
	m_nTimeTile			= pArchive->nTimeTile;
	m_AniContext.nDelay = 25;
	m_nLife				= pArchive->nLife;
	if(m_nLife > 0)
	{
		m_nLife = m_nLife * m_nTimeTile / m_AniContext.nDelay;
	}

	if( m_pAttackView != NULL )
	{
		m_pAttackView->AddLinkView(&m_pAttackView);
		m_ptTileAttack = m_pAttackView->GetTile();
	}

	return TRUE;
}
//------------------------------------------------------------------------
void CMagicSingleDir::Close()
{
	CMagic::Close();
}
//------------------------------------------------------------------------
CMagicSingleDir::~CMagicSingleDir()
{
	if(m_pAttackView != NULL)
		m_pAttackView->DelLinkView(&m_pAttackView);
}
//------------------------------------------------------------------------
CMagicSingleDir::CMagicSingleDir()
{
	(__int64&)m_ptTileOffset	= 0;
	(__int64&)m_fOldDir			= 0;
	m_nLen				= 0;
	m_nFramesPerDir		= 0;
	m_nStartFrameIdx	= 0;
	m_fOldDir			= 0;
	m_fDirInc			= 1;
	m_nTimeTile			= 0;
	m_nLife				= 0;
	m_pAttackView		= NULL;
}
//------------------------------------------------------------------------
// 有生命期的魔法
BOOL CMagicFlowText::AddToTimerAxis()
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMagicFlowText::AddToMapView(POINT& ptTile) 
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMagicFlowText::RemoveFromTimerAxis()
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMagicFlowText::RemoveFromMapView()
{
	return TRUE;
}
//------------------------------------------------------------------------
void CMagicFlowText::OnTimer(DWORD dwEventID)
{
	return;
}
//------------------------------------------------------------------------
BOOL CMagicFlowText::OnCreateItem(const char* buf, int nLen)
{
	m_dwResID = *(DWORD*)buf;
	return (RequestRes() != NULL);
}
//------------------------------------------------------------------------
void CMagicFlowText::DrawText(ICanvas* pCanvas, int x, int y, int nFrame, DWORD dwFlag, BYTE nAlpha)
{
	if (!pCanvas || !RequestRes()) return;
	if(nFrame < 0 || nFrame >= m_AniContext.nFrames)
		return;

	m_pPoolNodeObj->Draw(pCanvas,
		x - m_pPoolNodeObj->GetWidth(nFrame) / 2, y - m_pPoolNodeObj->GetHeight(nFrame) / 2,
		nFrame, dwFlag, nAlpha);
}
//------------------------------------------------------------------------
int CMagicFlowText::GetBitmapWidthHalf(int nFrame)
{
	return m_pPoolNodeObj->GetWidth(nFrame) / 2;
}
//------------------------------------------------------------------------
// 新的通用魔法
CMagicGeneral::CMagicGeneral()
{
	m_bLoopPart = TRUE;
	m_bLoop = FALSE;
}
//------------------------------------------------------------------------
CMagicGeneral::~CMagicGeneral()
{
}
//------------------------------------------------------------------------
void CMagicGeneral::OnTimer(DWORD dwEventID)
{
	if(m_bLoop) // 循环运行
	{
		if(m_bLoopPart)
		{
			if (++m_AniContext.nCurFrame >= m_nLoopEndIndex)
				m_AniContext.SetCurFrame(m_nLoopStartIndex);
		}
		else
		{
			if (++m_AniContext.nCurFrame > m_AniContext.nFrames)
				m_AniContext.SetCurFrame(0);
		}
	}
	else
	{
		if (++m_AniContext.nCurFrame >= m_AniContext.nFrames)
		{
			if (m_pMgr)
			{
				m_pMgr->OnPlayDone(this, m_ptTile);
				m_pMgr = NULL;
			}
			
			delete this;
			return;
		}
	}
}
//------------------------------------------------------------------------
BOOL CMagicGeneral::OnCreateItem(const char* buf, int nLen)
{
	if (CMagic::CreateItem(buf, nLen))
	{
		m_nLoopStartIndex = ((SMagicArchive_General*)buf)->nLoopStartIndex - 1;
		m_nLoopEndIndex = ((SMagicArchive_General*)buf)->nLoopEndIndex - 1;
		
		if(m_nLoopStartIndex < 0 || m_nLoopEndIndex <= 0 ||  m_nLoopEndIndex <= m_nLoopStartIndex)
			m_bLoopPart = FALSE;

		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
void CMagicGeneral::SetLoop(BOOL bLoop)
{
	m_bLoop = bLoop;
}
//------------------------------------------------------------------------
// 范围技能的鼠标光效
CMagicMouse::CMagicMouse()
{
	m_pParticle = NULL;
	m_bLoopPart = TRUE;
}
//------------------------------------------------------------------------
CMagicMouse::~CMagicMouse()
{
	SAFE_RELEASE(m_pParticle);
}
//------------------------------------------------------------------------
RECT CMagicMouse::GetShowRect()
{
	RECT rc = {-800,-800,800,800};
	return rc;
}
//------------------------------------------------------------------------
void CMagicMouse::OnTimer(DWORD dwEventID)
{
	POINTS ptTile = g_pPlayer->GetLoc();
	m_ptTile.x = ptTile.x;
	m_ptTile.y = ptTile.y;
	if(m_bLoopPart)
	{
		if (++m_AniContext.nCurFrame >= m_nLoopEndIndex)
			m_AniContext.SetCurFrame(m_nLoopStartIndex);
	}
	else
	{
		if (++m_AniContext.nCurFrame >= m_AniContext.nFrames)
			m_AniContext.SetCurFrame(0);
	}
}
//------------------------------------------------------------------------
BOOL CMagicMouse::OnCreateItem(const char* buf, int nLen)
{
	if (CMagic::CreateItem(buf, nLen))
	{
		m_nLoopStartIndex = ((SMagicArchive_General*)buf)->nLoopStartIndex - 1;
		m_nLoopEndIndex = ((SMagicArchive_General*)buf)->nLoopEndIndex - 1;
		
		if(m_nLoopStartIndex < 0 || m_nLoopEndIndex <= 0 ||  m_nLoopEndIndex <= m_nLoopStartIndex)
			m_bLoopPart = FALSE;

		if(m_AniContext.nDelay <= 0 )
			return FALSE;
		
		g_pTimeAxis->SetTimer(0,
			static_cast<ITimerSink*>(this),
			m_AniContext.nDelay,
			"BasicSys!!CMagicMouse::OnCreateItem");

		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
void CMagicMouse::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if (!pCanvas || !RequestRes())
		return;
	
	XCursor *pCursor = XDesktop::GetInstance()->GetCurrentCursor();
	if(pCursor == NULL)
		return;

	POINT ptMouse;
	pCursor->GetMouseXY(ptMouse);

	int offx, offy;
	m_pPoolNodeObj->GetOffset(m_AniContext.CurFrame(), offx, offy);
	m_pPoolNodeObj->Draw(pCanvas, 
		ptMouse.x - m_ptAnchor.x + offx, 
		ptMouse.y - m_ptAnchor.y + offy,
		m_AniContext.CurFrame(), dwFlag, NULL, m_pParticle);
}
//------------------------------------------------------------------------