/**
*	created:		2012-7-2   3:39
*	filename: 		MonsterView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/MonsterView.h"
#include "../Include/ResLoader.h"
#include "../Include/Pal.h"
#include "../Include/PalMgr.h"
#include "../Include/SkillCreate.h"
//------------------------------------------------------------------------
CMonsterView::CMonsterView() : CMstFSM()
{
	ClearAll();
	CMstFSM::Create(this);
}
//------------------------------------------------------------------------
CMonsterView::~CMonsterView()
{
	Close();
	if (m_pCurrentState)
	{
		m_pCurrentState->End();
		m_pCurrentState = NULL;
	}
}
//------------------------------------------------------------------------
void CMonsterView::ClearAll()
{
	m_nFallowStateID = 0;
	m_nCorperShowTime = 0;
	m_nShape = 0;
	
	m_pHeadAttachObj = NULL;
	m_nNpcType = NpcTypeNone;
	m_nTaskType = TaskTypeNone;
}
//------------------------------------------------------------------------
void CMonsterView::Close()
{
	if(m_pHeadAttachObj)
	{
		OnAttachObjDestroy(m_pHeadAttachObj);
		m_pHeadAttachObj = NULL;
	}
	for (int i = 0; i < CrtPartMax; i++)
	{
		if (m_pModule[i])
		{
			g_ResLoader.ReleaseRes(ioCreature + m_dwModuleID[i]);
			m_pModule[i] = NULL;
			m_dwModuleID[i] = 0;
		}
	}
}
//------------------------------------------------------------------------
void CMonsterView::OnTimer(DWORD dwEventID)
{
	switch (dwEventID)
	{
	case TIME_EVENT_ID_FRAME:		// 动画时钟
	case TIME_EVENT_ID_SHORT_STEPS:	// 碎步
		CMstFSM::OnTimer(dwEventID);
		break;

	case TIME_EVENT_ID_DIE:			// 尸体消失时间
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_DIE, static_cast<ITimerSink*>(this));

		g_pTimeAxis->SetTimer(TIME_EVENT_ID_DIE_ANI,
			static_cast<ITimerSink*>(this),
			TIME_DEFAULT_CORPER_ANI,
			"BasicSys!!CMonsterView::OnTimer");

		m_nCorperShowTime = 20;
		break;
		
	case TIME_EVENT_ID_DIE_ANI:	// 消失动画时间
		if(--m_nCorperShowTime == 0)
		{
			g_pTimeAxis->KillTimer(TIME_EVENT_ID_DIE_ANI, static_cast<ITimerSink*>(this));
			g_pMapView->RemoveItem(m_ptTile, this);
			delete this;
			return;
		}
		break;
	}
}
//------------------------------------------------------------------------
// 获取对象的类ID
DWORD CMonsterView::GetClassID() const
{
	return miNpcObj;
}
//------------------------------------------------------------------------
// 保存或读取数据，用于从文件中创建对象
BOOL CMonsterView::OnSerialize(BOOL bWrite,IArchive* pArchive)
{
	if (!pArchive) return FALSE;
	if(bWrite)
	{
		SMonsterViewArchive a;
		a.dwResID = m_dwResID;
		a.nType = m_AniContext.nType;

		pArchive->Write((char*)&a, sizeof(a));
		return TRUE;
	}
	else
	{
		Close();
		if (!CreateItem((SMonsterViewArchive*)pArchive->GetBuffer()))
		{
			TraceLn("ERROR: CMonsterView::OnSerialize(bRead), create item failed!");
		}
		pArchive->Seek(sizeof(SMonsterViewArchive));
		return TRUE;
	}
}
//------------------------------------------------------------------------
// 根据数据现场创建对象
BOOL CMonsterView::OnCreateItem(const char* buf, int nLen)
{
	return CreateItem((SMonsterViewArchive*)buf);
}
//------------------------------------------------------------------------
// 更新(如：重载脚本数据等)
void CMonsterView::OnUpdate()
{
	SMonsterViewCreateParam mvcp;
	memset(&mvcp, 0, sizeof(mvcp));
	g_ResLoader.GetMonsterCreateParam(m_dwCrtID, mvcp);
	m_AniContext.nType = mvcp.nType;
	g_ResLoader.GetAnchor(m_AniContext.nType, m_ptAnchor);
	int nDelayBuf[CrtStateMax];
	int nAttackFrameBuf[CrtStateMax];
	int nAttackSoundFrameBuf[CrtStateMax];
	memset (nDelayBuf, 0, sizeof(int) * CrtStateMax);
	memset (nAttackFrameBuf, 0, sizeof(int) * CrtStateMax);
	memset (nAttackSoundFrameBuf, 0, sizeof(int) * CrtStateMax);
	for (int i = 0; i < CrtStateMax; i++)
	{
		nDelayBuf[i] = g_ResLoader.GetFrameDelay(m_AniContext.nType, i);
		nAttackFrameBuf[i] = g_ResLoader.GetAttackFrame(m_AniContext.nType, i);
		nAttackSoundFrameBuf[i] = g_ResLoader.GetAttackSoundFrame(m_AniContext.nType, i);
	}
	CMstFSM::SetAllStateDelay(nDelayBuf);
	CMstFSM::SetAllStateAttackFrame(nAttackFrameBuf);
	CMstFSM::SetAllStateAttackSoundFrame(nAttackSoundFrameBuf);

	int nDelay[3];
	nDelay[0] = g_ResLoader.GetWalkDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
	nDelay[1] = g_ResLoader.GetRunDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
	nDelay[2] = g_ResLoader.GetRideDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
	CMstFSM::SetMoveDelay(nDelay);
	
	m_pCurrentState->Start();
	
	m_fStepWalk = g_ResLoader.GetStep(m_AniContext.nType, CrtStateWalk);
	m_fStepRun = g_ResLoader.GetStep(m_AniContext.nType, CrtStateRun);
	m_fStepRide = g_ResLoader.GetStep(m_AniContext.nType, CrtStateRideRun);

	//设置怪物的属性数据
	if (!g_ResLoader.GetMonsterbuf(m_dwCrtID, m_dwModuleID, m_byteAlpha, m_bytePal))
	{
		PrintEventLn("ERROR: CMonsterView-OnUpdata-SetModulePro");
	}
	
	// 速度等级支持	
	SetStepWalk(m_fStepWalk);
	SetStepRun(m_fStepRun);	
	SetStepRide(m_fStepRide);
	
	// 偏移位置相关
	mvcp.nHalfHeightOffset = g_ResLoader.GetHalfHeigh(m_AniContext.nType);
	mvcp.nHeightOffset = g_ResLoader.GetWholeHeigh(m_AniContext.nType);
	SetHalfHeightOffset(mvcp.nHalfHeightOffset);
	SetHeightOffset(mvcp.nHeightOffset);
	
	m_nBeHitLoopBeginFrame = g_ResLoader.GetBeHitLoopBegin(m_AniContext.nType);
	m_nBeHitLoopEndFrame = g_ResLoader.GetBeHitLoopEnd(m_AniContext.nType);

	memcpy (m_nActStateCommonAttack, mvcp.nActStateCommonAttack, sizeof(m_nActStateCommonAttack));
	m_nCommonAttackStyleNum = 0;
	for(int j = 0; j < 3; j++)
	{
		if(m_nActStateCommonAttack[j] >= 0)
			m_nCommonAttackStyleNum++;
	}
	m_nActStateJuqi = mvcp.nActStateJuqi;
	m_nActStateSkillAttack = mvcp.nActStateSkillAttack;
	m_nCommonAttackSoundID = mvcp.nSoundIDCommonAttack;
	
	m_nSoundIDFallow = mvcp.nSoundIDFallow;
	m_nSoundIDBeHit = mvcp.nSoundIDBeHit;
	m_nSoundIDDie = mvcp.nSoundIDDie;
	
	m_nFallowStateID = g_ResLoader.GetFallowStateID(m_AniContext.nType);

	IniTotalFrame();
	ClearShowRect();
}
//------------------------------------------------------------------------
//获取怪物站位
DWORD CMonsterView::GetOccupantValue()
{
	if (State() == CrtStateDie || !m_bOccupant)
		return 0x0;
	return CMapItem::GetOccupantValue();
}
//------------------------------------------------------------------------
// 读取占位块列表，默认为无占位位列表
COccupantTileList* CMonsterView::GetOccupantTileList() const
{
	return m_MstOccInfo.GetOccupantTileList();
}
//------------------------------------------------------------------------
// 获取对象在地图中的显示层
DWORD CMonsterView::GetLayerValue()
{
	if (m_AniContext.IsState(CrtStateDie))
		return milvLowest-1;
	return milvMidle;
}
//------------------------------------------------------------------------
// 设脏
void CMonsterView::SetDirty(ICanvas* pCanvas, const POINT& ptViewTop)
{
	if(!pCanvas)
		return;

// 先搞删除吸附对象时残留下来的脏矩形
	while(!m_AttachDirtyList.empty())
	{
		RECT rc;
		rc = m_AttachDirtyList.front();
		OffsetRect(&rc,
			-ptViewTop.x,
			-ptViewTop.y);
		pCanvas->AddDirtyRect(rc);
		m_AttachDirtyList.pop_front();
	}

	if(!m_bVisible)
	{
		if(m_bNeedSetOldDirty)
		{
			list<RECT> DirtyRectList;
			GetOldDirtyRect(DirtyRectList);

			RECT rc;
			while(!DirtyRectList.empty())
			{
				rc = DirtyRectList.front();
				OffsetRect(&rc,
					-ptViewTop.x,
					-ptViewTop.y);
				pCanvas->AddDirtyRect(rc);
				DirtyRectList.pop_front();
			}
		}
		return;
	}

	POINT ptNew;
	RECT rcAreaNew;
	Tile_To_World(m_ptTile, ptNew);

// 先搞吸附对象
	
	POINT ptAttachOffset;
	ptAttachOffset.x = m_ptTileOffset.x + ptNew.x;
	ptAttachOffset.y = m_ptTileOffset.y + ptNew.y;
	vector<IAttachObj*>::iterator it;
	for (it = m_vAttachObjs.begin(); it != m_vAttachObjs.end(); ++it)
	{
		for (int ni = 0; ni < (*it)->GetLoopNum(); ++ni)
		{
			if ((*it)->GetDrawOrder(ni) != IAttachObj::draw_order_topmost)
			{
				(*it)->SetDirty(pCanvas, ptViewTop, ptAttachOffset, ni);
			}
		}
	}
// 再搞生物
	rcAreaNew = GetShowRect();
	OffsetRect(&rcAreaNew,
		ptNew.x,
		ptNew.y);

	if(m_bNeedSetOldDirty)
	{
		// 如果还是画的同一祯
		if(m_AniContextOld.Equal(&m_AniContext))
		{
			if(EqualRect(&rcAreaNew, &m_rcAreaOld))	// 如果和上次画的位置一样则不用处理了						
				return;
		}
		else
		{
			m_AniContextOld = m_AniContext;
		}

		// 如果画的不是同一祯则把上次和这次的矩阵设脏
		// 先把上次旧的画图区域设脏
		OffsetRect(&m_rcAreaOld,
			-ptViewTop.x,
			-ptViewTop.y);
		pCanvas->AddDirtyRect(m_rcAreaOld);
	}

	// 保存这次的数据
	m_rcAreaOld = rcAreaNew;

	// 再把新的要画图区域设脏
	OffsetRect(&rcAreaNew,
		-ptViewTop.x,
		-ptViewTop.y);
	pCanvas->AddDirtyRect(rcAreaNew);
// 再搞生物对象 end------------------------------------------------------
	
	// 表示这次这个对象画了，下次要把他画的地方设脏
	m_bNeedSetOldDirty = TRUE;

}
//------------------------------------------------------------------------
// 获取旧的脏矩阵，主要用在把对象从地图上删除时获取它的脏矩阵
void CMonsterView::GetOldDirtyRect(list<RECT> &DirtyRectList)
{
	if(!m_bNeedSetOldDirty)
		return;

	m_bNeedSetOldDirty = FALSE;

	// 生物的
	DirtyRectList.push_back(m_rcAreaOld);

	// 吸附对象的残留物
	while(!m_AttachDirtyList.empty())
	{
		DirtyRectList.push_back(m_AttachDirtyList.front());
		m_AttachDirtyList.pop_front();
	}
	vector<IAttachObj*>::iterator it;
	for (it = m_vAttachObjs.begin(); it != m_vAttachObjs.end(); ++it)
	{
		for (int ni = 0; ni < (*it)->GetLoopNum(); ++ni)
		{
			if ((*it)->GetDrawOrder(ni) != IAttachObj::draw_order_topmost)
			{
				(*it)->GetOldDirtyRect(DirtyRectList, ni);
			}
		}
	}
}
//------------------------------------------------------------------------
// 绘图循环请求对象绘制到指定的ICanvas上
void CMonsterView::OnDraw(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag)
{
	if(!pCanvas || !m_bVisible)
		return;
	
	// 画光效(draw order = draw_order_prev)
	vector<IAttachObj*>::iterator it;
	POINT pt;
	pt.x = m_ptTileOffset.x + ptTileCenter.x;
	pt.y = m_ptTileOffset.y + ptTileCenter.y;
	
	for (it=m_vAttachObjs.begin(); it!=m_vAttachObjs.end(); ++it)
		for (int ni = 0; ni < (*it)->GetLoopNum(); ++ni)
			if ((*it)->GetDrawOrder(ni) == IAttachObj::draw_order_prev)
				(*it)->OnDraw(pCanvas, pt, dwFlag, ni);

	int nOldState = m_AniContext.nState;
	// 如果怪物没有跑步动作那就用走路动作代替，策划那边需求的，可是感觉这样搞很别扭
	if(GetState() == CrtStateRun && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateRun)))
		m_AniContext.nState = CrtStateWalk;

	// 如果怪物没有走路动作那就用站立动作代替，策划那边需求的，可是感觉这样搞很别扭
	if(GetState() == CrtStateWalk && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateWalk)))
		m_AniContext.nState = CrtStateStand;

	// 休闲动作可以指定任一动作
	if(GetState() == CrtStateFallow)
		m_AniContext.nState = m_nFallowStateID;

	int nCurFrame = m_AniContext.nCurFrame;
	if (m_nCurPalType == PalType_Stone)
	{
		nCurFrame = 0;
	}
	// 画角色
	int offx, offy;
	int nModuleType, nCurAni;
	CAniGroup* pCurObj;
	BYTE nDrawOrder[CrtPartMax/2];
	memset (nDrawOrder, 0, sizeof(BYTE) * CrtPartMax/2);
	int nCount = g_ResLoader.GetFrameSort(nDrawOrder, m_AniContext);
	for (int i = 0; i < nCount; i++)
	{
		nModuleType = nDrawOrder[i];
		if(nModuleType < 0 || nModuleType > CrtPartMax - 1)
			continue;
		pCurObj = RequestRes(nModuleType);
	
		if (pCurObj)
		{
			nCurAni = GetCurAni(nModuleType);
			if (nCurAni == -1)
				continue;
			pCurObj->GetOffset(nCurAni, nCurFrame, offx, offy);
			if (m_byteAlpha[nModuleType] == 0 || m_byteAlphaAll != 0)
			{
				if(m_nCorperShowTime > 0)
				{
					pCurObj->Draw(pCanvas, 
						m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + offx,
						m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + offy,
						nCurAni, nCurFrame, dwFlag,
						m_nCorperShowTime * 12, m_Pal[nModuleType].GetPal(m_nCurPalType), m_pParticle[nModuleType]);
				}
				else
				{
					pCurObj->Draw(pCanvas, 
						m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + offx,
						m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + offy,
						nCurAni, nCurFrame, dwFlag,
						m_byteAlphaAll, m_Pal[nModuleType].GetPal(m_nCurPalType), m_pParticle[nModuleType]);
				}
			}
			else
			{
				if(m_nCorperShowTime > 0)
				{
					pCurObj->Draw(pCanvas, 
						m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + offx,
						m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + offy,
						nCurAni, nCurFrame, dwFlag,
						m_nCorperShowTime * 25, m_Pal[nModuleType].GetPal(m_nCurPalType), m_pParticle[nModuleType]);
				}
				else
				{
					pCurObj->Draw(pCanvas, 
						m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + offx,
						m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + offy,
						nCurAni, nCurFrame, dwFlag,
						m_byteAlpha[nModuleType], m_Pal[nModuleType].GetPal(m_nCurPalType), m_pParticle[nModuleType]);
				}
			}
		}
		// 画特效
		nModuleType++;
		pCurObj = RequestRes(nModuleType);
		if (pCurObj)
		{
			nCurAni = GetCurAni(nModuleType);
			if (nCurAni == -1)
				continue;
			pCurObj->GetOffset(nCurAni, nCurFrame, offx, offy);
			if (m_byteAlpha[nModuleType] == 0 || m_byteAlphaAll != 0)
			{
				if(m_nCorperShowTime > 0)
				{
					pCurObj->Draw(pCanvas, 
						m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + offx,
						m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + offy,
						nCurAni, nCurFrame, dwFlag,
						m_nCorperShowTime * 12, m_Pal[nModuleType].GetPal(m_nCurPalType));
				}
				else
				{
					pCurObj->Draw(pCanvas, 
						m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + offx,
						m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + offy,
						nCurAni, nCurFrame, dwFlag,
						m_byteAlphaAll, m_Pal[nModuleType].GetPal(m_nCurPalType));
				}
			}
			else
			{
				if(m_nCorperShowTime > 0)
				{
					pCurObj->Draw(pCanvas, 
						m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + offx,
						m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + offy,
						nCurAni, nCurFrame, dwFlag,
						m_nCorperShowTime * 25, m_Pal[nModuleType].GetPal(m_nCurPalType));
				}
				else
				{
					pCurObj->Draw(pCanvas, 
						m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + offx,
						m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + offy,
						nCurAni, nCurFrame, dwFlag,
						m_byteAlpha[nModuleType], m_Pal[nModuleType].GetPal(m_nCurPalType));
				}
			}
		}
	}

	m_AniContext.nState = nOldState;

	// 画光效(draw order = draw_order_after)
	for (it=m_vAttachObjs.begin(); it!=m_vAttachObjs.end(); ++it)
		for (int ni = 0; ni < (*it)->GetLoopNum(); ++ni)
			if ((*it)->GetDrawOrder(ni) == IAttachObj::draw_order_after)
				(*it)->OnDraw(pCanvas, pt, dwFlag, ni);
};
//------------------------------------------------------------------------
// 绘制其他一些顶层对象，如血槽、聊天内容、物品名等，这些对象永远放在最上层
void CMonsterView::OnDrawTopmostObject(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag)
{
	vector<IAttachObj*>::iterator it;
	vector<IAttachObj*>::iterator end;
	POINT pt;
	pt.x = m_ptTileOffset.x + ptTileCenter.x;
	pt.y = m_ptTileOffset.y + ptTileCenter.y;

	end = m_vFlowObjs.end();
	for (it=m_vFlowObjs.begin(); it!=end; ++it)
		(*it)->OnDraw(pCanvas, pt, BLIT_MASK);

	if(!m_bVisible)
		return;

	end = m_vAttachObjs.end();
	for (it=m_vAttachObjs.begin(); it!=end; ++it)
		if ((*it)->GetDrawOrder() == IAttachObj::draw_order_topmost)
			(*it)->OnDraw(pCanvas, pt, BLIT_MASK);

	DWORD dwFlags = CMapItemView::GetFlag();
	POINT ptOffset = ptTileCenter;
	ptOffset.x += m_ptTileOffset.x;
	ptOffset.y += m_ptTileOffset.y + GetHeightOffset();
	
#ifndef _USE
	if (m_pPaoPao != NULL && State() != CrtStateDie)
	{
		m_pPaoPao->OnDraw(pCanvas, ptOffset, m_szUserName, m_clrName, g_pFont);
	}
	else
	{
		if (m_AniContext.IsState(CrtStateDie))
			return;
		if (!IsSelected())
		{
			DrawName(pCanvas, ptOffset);
		}
		else
		{
			DrawSelectedName(pCanvas, ptOffset);
		}
	}
#endif
/*
#ifdef _USE
	DrawName(pCanvas, ptOffset);
	if (IsSelected())
	{
		WPixel clr = WPixel(255,0,0);
		WPixel clrText = WPixel(255,255,255);
		char buf[MAX_PATH];
		POINT ptTile;
		g_pMapView->ScreenToTile(ptTileCenter, ptTile);
		
		if (dwFlags & flagInfo)
		{
			IResObject* pObj = g_ResLoader.m_pScpCreature->GetResObject(m_dwResID);
			if (pObj)
			{
				IResObject* pParentObj = pObj->GetParentObject();
				wsprintf(buf, "%s\\%s,(%d,%d),CID=%d,RID=%d", 
					pParentObj->GetObjectName(), pObj->GetObjectName(),
					ptTile.x, ptTile.y, GetClassID(), m_dwResID);
			}
			else
				wsprintf(buf, "(%d,%d),CID=%d,RID=%d", ptTile.x, ptTile.y, GetClassID(), m_dwResID);
			
			int nW = g_pFont->GetLength(buf);
			int nH = g_pFont->GetHeight();
			RECT rcTip;
			int offx, offy;
			if (m_pModule[CrtPartHead])					//头
				m_pModule[CrtPartHead]->GetOffset(GetCurAni(CrtPartHead), GetCurFrame(), offx, offy);
			else if (m_pModule[CrtPartUpBody])			//上半身
				m_pModule[CrtPartUpBody]->GetOffset(GetCurAni(CrtPartUpBody), GetCurFrame(), offx, offy);
			else if (m_pModule[CrtPartDownBody])		//下半身
				m_pModule[CrtPartDownBody]->GetOffset(GetCurAni(CrtPartDownBody), GetCurFrame(), offx, offy);
			else if (m_pModule[CrtPartBody])			//身体
				m_pModule[CrtPartBody]->GetOffset(GetCurAni(CrtPartBody), GetCurFrame(), offx, offy);
			else if (m_pModule[CrtPartWholeBody])		//全身体
				m_pModule[CrtPartWholeBody]->GetOffset(GetCurAni(CrtPartWholeBody), GetCurFrame(), offx, offy);
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
#else
*/
	if (m_AniContext.IsState(CrtStateDie))
		return;

	//if (dwFlags & flagBloodSlot)
	{
		RECT rc;
		rc.left = ptOffset.x - BLOOD_SLOT_WIDTH/2;
		rc.right = rc.left + BLOOD_SLOT_WIDTH + 1;
		rc.top = ptOffset.y;
		rc.bottom = rc.top + BLOOD_SLOT_HEIGHT + 1;
		DrawBloodSlot(pCanvas, rc, m_nBloodVol);
	}
	/*
#endif
	*/
}
//------------------------------------------------------------------------
// 生物死亡
// dwParam1: 0表示慢慢死亡，有动作，1表示已经死亡，无动作
// dwParam2: ?
int CMonsterView::cmdDie(DWORD dwParam1, DWORD dwParam2)
{
	m_bDeaded = TRUE;
	
	if (IsMoving())
		StopMove();

	g_pMapView->RemoveItem(m_ptTile, this);
	SetState(CrtStateDie);
	g_pMapView->AddItem(m_ptTile, this);

	if (dwParam1 == 1)
	{
		m_AniContext.nCurFrame = - 1;
	}

	// 设定尸体保存时间
	g_pTimeAxis->SetTimer(TIME_EVENT_ID_DIE,
		static_cast<ITimerSink*>(this),
		TIME_DEFAULT_CORPER,
		"BasicSys!!CMonsterView::cmdDie");

	return TRUE;
}
//------------------------------------------------------------------------
// 角色出生(发此命令可以播放一段出生动画)
// dwParam1: ?
// dwParam2: ?
int CMonsterView::cmdBorn(DWORD dwParam1, DWORD dwParam2)
{
	m_bDeaded = FALSE;
	
	SSkillCreateParam scp;
	scp.nAniType = anitypeGenericAni;
	scp.nIndex = aniBorn;
	scp.ptTile = m_ptTile;
	scp.ptAttack = m_ptTile;

	return CSkillCreate::CreateGenericMagic(&scp);
}
//------------------------------------------------------------------------
// 设置NPC类别对应的头顶光效
// dwParam1: NPC类别对应的头顶光效 参看enNpcType
// dwParam2: ?
int CMonsterView::cmdSetNpcType(DWORD dwParam1, DWORD dwParam2)
{
	m_nNpcType = dwParam1;
	if(m_nTaskType != TaskTypeNone)
		return TRUE;

	if(m_pHeadAttachObj)
	{
		OnAttachObjDestroy(m_pHeadAttachObj);
		m_pHeadAttachObj = NULL;
	}

	if(m_nNpcType != NpcTypeNone)
	{
		IResObject* pScpSkill = (IResObject*)CSkillCreate::getSkillScpObj(anitypeHeadAni, aniNpcTypeTrans + m_nNpcType - 1);
		if (!pScpSkill)
			return FALSE;
		
		DWORD dwMagicID;
		pScpSkill->GetPropertyLong("MagicID", (long&)dwMagicID, 0);	

		if (dwMagicID > 0)
		{
			SAttachStateObjBuffer st;
			memset(&st, 0, sizeof(st));
			st.dwResID = dwMagicID;

			IResObject* pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(st.dwResID);
			if (pMagicObj)
			{
				g_ResLoader.ReadAttachMagic(pMagicObj, &st);
				
				st.dwAppearanceID = -1;
				
				m_pHeadAttachObj = CreateAttachObj(&st);
			}
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 任务NPC头顶的光效
// dwParam1: 任务NPC头顶的光效 参看enTaskType
// dwParam2: ?
int CMonsterView::cmdSetTaskType(DWORD dwParam1, DWORD dwParam2)
{
	m_nTaskType = dwParam1;
	
	if(m_pHeadAttachObj)
	{
		OnAttachObjDestroy(m_pHeadAttachObj);
		m_pHeadAttachObj = NULL;
	}

	if(m_nTaskType != TaskTypeNone)
	{
		IResObject* pScpSkill = (IResObject*)CSkillCreate::getSkillScpObj(anitypeHeadAni, aniTaskType_Get_Can + m_nTaskType - 1);
		if (!pScpSkill)
			return FALSE;
		
		DWORD dwMagicID;
		pScpSkill->GetPropertyLong("MagicID", (long&)dwMagicID, 0);	

		if (dwMagicID > 0)
		{
			SAttachStateObjBuffer st;
			memset(&st, 0, sizeof(st));
			st.dwResID = dwMagicID;

			IResObject* pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(st.dwResID);
			if (pMagicObj)
			{
				g_ResLoader.ReadAttachMagic(pMagicObj, &st);
				
				st.dwAppearanceID = -1;
				
				m_pHeadAttachObj = CreateAttachObj(&st);
			}
		}
	}
	else if(m_nNpcType != NpcTypeNone)
	{
		IResObject* pScpSkill = (IResObject*)CSkillCreate::getSkillScpObj(anitypeHeadAni, aniNpcTypeTrans + m_nNpcType - 1);
		if (!pScpSkill)
			return FALSE;
		
		DWORD dwMagicID;
		pScpSkill->GetPropertyLong("MagicID", (long&)dwMagicID, 0);	

		if (dwMagicID > 0)
		{
			SAttachStateObjBuffer st;
			memset(&st, 0, sizeof(st));
			st.dwResID = dwMagicID;

			IResObject* pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(st.dwResID);
			if (pMagicObj)
			{
				g_ResLoader.ReadAttachMagic(pMagicObj, &st);
				
				st.dwAppearanceID = -1;
				
				m_pHeadAttachObj = CreateAttachObj(&st);
			}
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 设置生物的当前状态
void CMonsterView::SetState(int nState)
{
	if(nState < 0 || nState >= CrtStateMax)
		return;
	
	if(nState == CrtStateBehit && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateBehit)))
		nState = CrtStateStand;

	if(nState == CrtStateStand && GetMoveStyle() == ActMoveStyleRide)
		nState = CrtStateRideStand;

	if(m_pStates[nState] == NULL)
		return;

	if (CMstFSM::GetCurState() == m_pStates[nState])
	{
		m_pStates[nState]->DelAllAttribute();
		return;
	}

	// 如果死了不作其他动作了
	if(IsDeaded() && nState != CrtStateDie)
		return;

	m_AniContext.nState = nState;

	// 如果怪物没有跑步动作那就用走路动作代替，策划那边需求的，可是感觉这样搞很别扭
	if(GetState() == CrtStateRun && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateRun)))
		m_AniContext.nState = CrtStateWalk;

	// 如果怪物没有走路动作那就用站立动作代替，策划那边需求的，可是感觉这样搞很别扭
	if(GetState() == CrtStateWalk && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateWalk)))
		m_AniContext.nState = CrtStateStand;

	// 休闲动作可以指定任一动作
	if(GetState() == CrtStateFallow)
		m_AniContext.nState = m_nFallowStateID;

	if(IsChangeShap())
	{
		// 如果怪物没有该动作就站立吧
		if(!g_ResLoader.GetStateEnable(m_AniContext.nType, m_AniContext.nState))
		{
			m_AniContext.nState = CrtStateStand;
			nState = CrtStateStand;
		}
	}
	CMstFSM::GotoState(*m_pStates[nState]);	
	
	SetCurFrame(0);
	IniTotalFrame();
}
//------------------------------------------------------------------------
// 停止当前状态
void CMonsterView::StopState(int nState)
{
	if(nState < 0 || nState >= CrtStateMax)
		return;
	
	if(m_pStates[nState] == NULL)
		return;

	m_pStates[nState]->Stop();
}
//------------------------------------------------------------------------
// 重启当前状态
void CMonsterView::RestartCurState()
{
	GetCurState()->End();
	GetCurState()->Start();
}
//------------------------------------------------------------------------
// 获取普通攻击动作
int CMonsterView::GetNormalAttackActID()
{
	if(m_nCommonAttackStyleNum > 0)
	{
		if(m_nCommonAttackStyleNum == 1)
			return m_nActStateCommonAttack[0];
		else
		{
			return m_nActStateCommonAttack[rand() % m_nCommonAttackStyleNum];
		}
	}

	return 0;
}
//------------------------------------------------------------------------
// 获取默认的蓄气动作ID（非玩家有效）
int CMonsterView::GetJuqiActID()
{
	return m_nActStateJuqi;
}
//------------------------------------------------------------------------
// 获取默认的技能攻击动作ID（非玩家有效）
int CMonsterView::GetSkillAttackActID()
{
	return m_nActStateSkillAttack;
}
//------------------------------------------------------------------------
// 设置对象矩形大小范围
void CMonsterView::SetShowRect()
{
	// 如果以前已经设置了就不用设置了
	if(m_ItemShowRect[m_AniContext.nState][m_AniContext.nDir][m_AniContext.nCurFrame].bIni)
		return;
	
	// 设为已经设置
	m_ItemShowRect[m_AniContext.nState][m_AniContext.nDir][m_AniContext.nCurFrame].bIni = TRUE;

	// 没有该状态
	if(m_pStates[m_AniContext.nState] == NULL)
	{
		memset(&m_ItemShowRect[m_AniContext.nState][m_AniContext.nDir][m_AniContext.nCurFrame].rc, 0, sizeof(RECT));
		return;
	}

	int nOldState = m_AniContext.nState;

	// 如果怪物没有跑步动作那就用走路动作代替，策划那边需求的，可是感觉这样搞很别扭
	if(m_AniContext.nState == CrtStateRun && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateRun)))
		m_AniContext.nState = CrtStateWalk;

	// 如果怪物没有走路动作那就用站立动作代替，策划那边需求的，可是感觉这样搞很别扭
	if(m_AniContext.nState == CrtStateWalk && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateWalk)))
		m_AniContext.nState = CrtStateStand;

	// 休闲动作可以指定任一动作
	if(GetState() == CrtStateFallow)
		m_AniContext.nState = m_nFallowStateID;

	RECT rc;
	RECT rcResult = {800,600,-800,-600};

	BYTE nDrawOrder[CrtPartMax/2];
	int nCount = g_ResLoader.GetFrameSort(nDrawOrder, m_AniContext);

	int offx, offy;
	int nModuleType;
	int nCurAni;
	CAniGroup* pCurObj = NULL;
	for (int i = 0; i < nCount; i++)
	{
		nModuleType = nDrawOrder[i];
		pCurObj = RequestRes(nModuleType);
		if (pCurObj)
		{
			nCurAni = GetCurAni(nModuleType);
			if (nCurAni == -1)
				continue;

			pCurObj->GetOffset(nCurAni, m_AniContext.nCurFrame, offx, offy);
			if(offx == 0 && offy == 0)// 这个东西主要是防止脚本那边出现宽度和高度都为1偏移都为0的东东（这个确实存在，原因还没空去查）
			{
				if(pCurObj->GetWidth(nCurAni, m_AniContext.nCurFrame) <= 1 && pCurObj->GetHeight(nCurAni, m_AniContext.nCurFrame) <= 1)
					continue;
			}

			rc.left = offx - m_ptAnchor.x;
			rc.top = offy - m_ptAnchor.y;
			rc.right = rc.left + pCurObj->GetWidth(nCurAni, GetCurFrame());
			rc.bottom = rc.top + pCurObj->GetHeight(nCurAni, GetCurFrame());
			if (rcResult.bottom < rc.bottom)
				rcResult.bottom = rc.bottom;
			if (rcResult.left > rc.left)
				rcResult.left = rc.left;
			if (rcResult.right < rc.right)
				rcResult.right = rc.right;
			if (rcResult.top > rc.top)
				rcResult.top = rc.top;
		}

		// 特效
		nModuleType++;
		pCurObj = RequestRes(nModuleType);
		if (pCurObj)
		{
			nCurAni = GetCurAni(nModuleType);
			if (nCurAni == -1)
				continue;

			pCurObj->GetOffset(nCurAni, m_AniContext.nCurFrame, offx, offy);
			if(offx == 0 && offy == 0)// 这个东西主要是防止脚本那边出现宽度和高度都为1偏移都为0的东东（这个确实存在，原因还没空去查）
			{
				if(pCurObj->GetWidth(nCurAni, m_AniContext.nCurFrame) <= 1 && pCurObj->GetHeight(nCurAni, m_AniContext.nCurFrame) <= 1)
					continue;
			}

			rc.left = offx - m_ptAnchor.x;
			rc.top = offy - m_ptAnchor.y;
			rc.right = rc.left + pCurObj->GetWidth(nCurAni, GetCurFrame());
			rc.bottom = rc.top + pCurObj->GetHeight(nCurAni, GetCurFrame());
			if (rcResult.bottom < rc.bottom)
				rcResult.bottom = rc.bottom;
			if (rcResult.left > rc.left)
				rcResult.left = rc.left;
			if (rcResult.right < rc.right)
				rcResult.right = rc.right;
			if (rcResult.top > rc.top)
				rcResult.top = rc.top;
		}
	}
	m_ItemShowRect[nOldState][m_AniContext.nDir][m_AniContext.nCurFrame].rc = rcResult;

	m_AniContext.nState = nOldState;
}
//------------------------------------------------------------------------
// 设置移动的祯延时
void CMonsterView::SetAllMoveDelay(int* pDelay)
{
	SetMoveDelay(pDelay);
}
//------------------------------------------------------------------------
// 回调(跟随对象销毁时回调)
void CMonsterView::OnAttachObjDestroy(IAttachObj* pAttachObj)
{
	vector<IAttachObj*>::iterator it;
	for (it = m_vAttachObjs.begin(); it != m_vAttachObjs.end(); it++)
	{
		if ((*it) == pAttachObj) // 存在
		{
			// 把残留的脏矩形留下才放你走
			for (int ni = 0; ni < pAttachObj->GetLoopNum(); ++ni)
			{
				if (pAttachObj->GetDrawOrder(ni) != IAttachObj::draw_order_topmost)
				{
					pAttachObj->GetOldDirtyRect(m_AttachDirtyList, ni);
				}
			}
			
			pAttachObj->Release();
			m_vAttachObjs.erase(it);
			break;
		}
	}
}
//------------------------------------------------------------------------
// 回调(飘字对象销毁时回调)
void CMonsterView::OnFlowObjDestroy(IAttachObj* pAttachObj)
{
	vector<IAttachObj*>::iterator it;
	for (it = m_vFlowObjs.begin(); it != m_vFlowObjs.end(); it++)
	{
		if ((*it) == pAttachObj) // 存在
		{
			pAttachObj->Release();
			m_vFlowObjs.erase(it);
			break;
		}
	}
}
//------------------------------------------------------------------------
// 删掉所有吸附对象
void CMonsterView::RemoveAllAttachObj()
{
	vector<IAttachObj*>::iterator it;
	for (it = m_vAttachObjs.begin(); it != m_vAttachObjs.end(); it++)
	{
		// 把残留的脏矩形留下才放你走
		for (int ni = 0; ni < (*it)->GetLoopNum(); ++ni)
		{
			if ((*it)->GetDrawOrder(ni) != IAttachObj::draw_order_topmost)
			{
				(*it)->GetOldDirtyRect(m_AttachDirtyList, ni);
			}
		}
		(*it)->Release();
	}
	m_vAttachObjs.clear();

	m_vAttackAttachObjs.clear();
}
//------------------------------------------------------------------------
// 设置目前状态下对应的技能ID
void CMonsterView::SetStatusMagicCheckID(int nCheckID)
{
	GetCurState()->SetMagicCheckID(nCheckID);
}
//------------------------------------------------------------------------
// 设置目前状态下要播放普通攻击声音
void CMonsterView::SetStatusPlayCommondAttackSound()
{
	GetCurState()->SetPlayCommondAttackSound();
}
//------------------------------------------------------------------------
// 设置目前状态下的蓄气属性
void CMonsterView::SetStatusPreAttack()
{
	GetCurState()->SetPreAttack();
}
//------------------------------------------------------------------------
// 设置目前状态下的一次性攻击属性
void CMonsterView::SetStatusAttackOnce()
{
	GetCurState()->SetAttackOnce();
}
//------------------------------------------------------------------------
// 设置目前状态下的持续攻击属性
void CMonsterView::SetStatusAttackCtrl()
{
	GetCurState()->SetAttackCtrl();
}
//------------------------------------------------------------------------
// 为了残影做的
void CMonsterView::OnDrawEx(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag, SAniContext *pContext, BYTE nAlpha)
{
	if(!pCanvas || !m_bVisible || m_byteAlphaAll != 0)
		return;
	
	SAniContext OldAniContext;
	OldAniContext.SetData(&m_AniContext);
	m_AniContext.SetData(pContext);

	int nOldState = m_AniContext.nState;
	// 如果怪物没有跑步动作那就用走路动作代替，策划那边需求的，可是感觉这样搞很别扭
	if(GetState() == CrtStateRun && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateRun)))
		m_AniContext.nState = CrtStateWalk;

	// 如果怪物没有走路动作那就用站立动作代替，策划那边需求的，可是感觉这样搞很别扭
	if(GetState() == CrtStateWalk && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateWalk)))
		m_AniContext.nState = CrtStateStand;

	// 休闲动作可以指定任一动作
	if(GetState() == CrtStateFallow)
		m_AniContext.nState = m_nFallowStateID;

	// 画角色
	int offx, offy;
	int nModuleType, nCurAni;
	CAniGroup* pCurObj;
	BYTE nDrawOrder[CrtPartMax/2];
	memset (nDrawOrder, 0, sizeof(BYTE) * CrtPartMax/2);
	int nCount = g_ResLoader.GetFrameSort(nDrawOrder, m_AniContext);
	for (int i = 0; i < nCount; i++)
	{
		nModuleType = nDrawOrder[i];
		if(nModuleType < 0 || nModuleType > CrtPartMax - 1)
			continue;
		pCurObj = RequestRes(nModuleType);
	
		if (pCurObj)
		{
			nCurAni = GetCurAni(nModuleType);
			if (nCurAni == -1)
				continue;
			pCurObj->GetOffset(nCurAni, m_AniContext.nCurFrame, offx, offy);
			pCurObj->Draw(pCanvas, 
				ptTileCenter.x - m_ptAnchor.x + offx,
				ptTileCenter.y - m_ptAnchor.y + offy,
				nCurAni, m_AniContext.nCurFrame, dwFlag, nAlpha, m_Pal[nModuleType].GetPal(m_nCurPalType));
		}

		// 画特效
		nModuleType++;
		pCurObj = RequestRes(nModuleType);
		if (pCurObj)
		{
			int nCurAni = GetCurAni(nModuleType);
			if (nCurAni == -1)
				continue;
			pCurObj->GetOffset(nCurAni, m_AniContext.nCurFrame, offx, offy);
			if (m_byteAlpha[nModuleType] == 0 || dwFlag == (BLIT_MASK | BLIT_TRANS))
			{
				pCurObj->Draw(pCanvas,
					m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + offx,
					m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + offy,
					nCurAni, m_AniContext.nCurFrame, dwFlag, 0, m_Pal[nModuleType].GetPal(m_nCurPalType));
			}
			else
			{
				pCurObj->Draw(pCanvas,
					m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + offx,
					m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + offy,
					nCurAni, m_AniContext.nCurFrame, dwFlag, m_byteAlpha[nModuleType], m_Pal[nModuleType].GetPal(m_nCurPalType));
			}
		}
	}

	m_AniContext.SetData(&OldAniContext);
};
//------------------------------------------------------------------------
BOOL CMonsterView::CreateItem(SMonsterViewArchive* pArchive)
{
	ASSERT(pArchive);
	struct STemp 
	{
		DWORD dwResID;						//怪物ID。通过这个ID可以获取到怪物的组成
		ICreatureSink* pCrtSink;
	};

	m_dwCrtID = ((STemp*)pArchive)->dwResID;//怪物ID
	m_pCrtSink = ((STemp*)pArchive)->pCrtSink;
	
	if(m_dwCrtID <= 0 || m_pCrtSink == NULL)
	{
		TraceLn("ERROR: CMonsterView::CreateItem FAILE ID = %d", m_dwCrtID);
		return FALSE;
	}

	OnUpdate();
	
	return TRUE;
}
//------------------------------------------------------------------------
/*

//读取站位块信息
BOOL CMonsterView::ReadOccupantInfo()
{
	char buffer[4096];
	unsigned char buf[2048];
	COccupantTileList *pL = (COccupantTileList*)buffer;
	BOOL nLen = g_ResLoader.GetNpcOccupantInfo(m_dwCrtID, buf, 2048);
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
//		pL->Init(0);
//		pL->GetPreData().nOffAnchorX = 0;
//		pL->GetPreData().nOffAnchorY = 0;
//		pL->GetPreData().wWidth = 0;
//		pL->GetPreData().wHeight = 0;

		return FALSE;
	}
	return m_MstOccInfo.SetOccupantTileList(pL);
}*/

//------------------------------------------------------------------------
// 设置生物的行走步长(以竖直方向为准)
void CMonsterView::SetStepWalk(float fStep)
{
	const float fRateX = 1.0f;
	const float fRateY = 0.5f;
	if (fStep > 32.0f) fStep = 32.0f;
	if (fStep <= 0.0f) fStep = 1.0f;
	
	m_StepWalk[CrtDirNorth].x = 0;
	m_StepWalk[CrtDirNorth].y = -fStep;
	
	m_StepWalk[CrtDirSouth].x = 0;
	m_StepWalk[CrtDirSouth].y = fStep;
	
	m_StepWalk[CrtDirEast].x = fStep*2;
	m_StepWalk[CrtDirEast].y = 0;
	
	m_StepWalk[CrtDirWest].x = -fStep*2;
	m_StepWalk[CrtDirWest].y = 0;
	
	m_StepWalk[CrtDirEastNorth].x = fRateX*fStep;
	m_StepWalk[CrtDirEastNorth].y = -fRateY*fStep;
	
	m_StepWalk[CrtDirEastSouth].x = fRateX*fStep;
	m_StepWalk[CrtDirEastSouth].y = fRateY*fStep;
	
	m_StepWalk[CrtDirWestSouth].x = -fRateX*fStep;
	m_StepWalk[CrtDirWestSouth].y = fRateY*fStep;
	
	m_StepWalk[CrtDirWestNorth].x = -fRateX*fStep;
	m_StepWalk[CrtDirWestNorth].y = -fRateY*fStep;
}
//------------------------------------------------------------------------
// 设置生物的跑动步长(以竖直方向为准)
void CMonsterView::SetStepRun(float fStep)
{
	const float fRateX = 1.0f;
	const float fRateY = 0.5f;
	if (fStep > 32.0f) fStep = 32.0f;
	if (fStep <= 0.0f) fStep = 1.0f;
	
	m_StepRun[CrtDirNorth].x = 0;
	m_StepRun[CrtDirNorth].y = -fStep;
	
	m_StepRun[CrtDirSouth].x = 0;
	m_StepRun[CrtDirSouth].y = fStep;
	
	m_StepRun[CrtDirEast].x = fStep*2;
	m_StepRun[CrtDirEast].y = 0;
	
	m_StepRun[CrtDirWest].x = -fStep*2;
	m_StepRun[CrtDirWest].y = 0;
	
	m_StepRun[CrtDirEastNorth].x = fRateX*fStep;
	m_StepRun[CrtDirEastNorth].y = -fRateY*fStep;
	
	m_StepRun[CrtDirEastSouth].x = fRateX*fStep;
	m_StepRun[CrtDirEastSouth].y = fRateY*fStep;
	
	m_StepRun[CrtDirWestSouth].x = -fRateX*fStep;
	m_StepRun[CrtDirWestSouth].y = fRateY*fStep;
	
	m_StepRun[CrtDirWestNorth].x = -fRateX*fStep;
	m_StepRun[CrtDirWestNorth].y = -fRateY*fStep;
}
//------------------------------------------------------------------------
// 设置生物的骑马跑动步长(以竖直方向为准)
void CMonsterView::SetStepRide(float fStep)
{
	const float fRateX = 1.0f;
	const float fRateY = 0.5f;
	if (fStep > 32.0f) fStep = 32.0f;
	if (fStep <= 0.0f) fStep = 1.0f;
	
	m_StepRide[CrtDirNorth].x = 0;
	m_StepRide[CrtDirNorth].y = -fStep;
	
	m_StepRide[CrtDirSouth].x = 0;
	m_StepRide[CrtDirSouth].y = fStep;
	
	m_StepRide[CrtDirEast].x = fStep*2;
	m_StepRide[CrtDirEast].y = 0;
	
	m_StepRide[CrtDirWest].x = -fStep*2;
	m_StepRide[CrtDirWest].y = 0;
	
	m_StepRide[CrtDirEastNorth].x = fRateX*fStep;
	m_StepRide[CrtDirEastNorth].y = -fRateY*fStep;
	
	m_StepRide[CrtDirEastSouth].x = fRateX*fStep;
	m_StepRide[CrtDirEastSouth].y = fRateY*fStep;
	
	m_StepRide[CrtDirWestSouth].x = -fRateX*fStep;
	m_StepRide[CrtDirWestSouth].y = fRateY*fStep;
	
	m_StepRide[CrtDirWestNorth].x = -fRateX*fStep;
	m_StepRide[CrtDirWestNorth].y = -fRateY*fStep;
}
//------------------------------------------------------------------------
// 变身
BOOL CMonsterView::ChangeShapEX()
{
	SMonsterViewCreateParam mvcp;
	memset(&mvcp, 0, sizeof(mvcp));

	if(!g_ResLoader.GetMonsterCreateParam(m_dwShapID[m_nShapLv], mvcp))
		return FALSE;

	m_AniContext.nType = mvcp.nType;
	g_ResLoader.GetAnchor(m_AniContext.nType, m_ptAnchor);
	int nDelayBuf[CrtStateMax];
	int nAttackFrameBuf[CrtStateMax];
	int nAttackSoundFrameBuf[CrtStateMax];
	memset (nDelayBuf, 0, sizeof(int) * CrtStateMax);
	memset (nAttackFrameBuf, 0, sizeof(int) * CrtStateMax);
	memset (nAttackSoundFrameBuf, 0, sizeof(int) * CrtStateMax);
	for (int i = 0; i < CrtStateMax; i++)
	{
		nDelayBuf[i] = g_ResLoader.GetFrameDelay(m_AniContext.nType, i);
		nAttackFrameBuf[i] = g_ResLoader.GetAttackFrame(m_AniContext.nType, i);
		nAttackSoundFrameBuf[i] = g_ResLoader.GetAttackSoundFrame(m_AniContext.nType, i);
	}
	CMstFSM::SetAllStateDelay(nDelayBuf);
	CMstFSM::SetAllStateAttackFrame(nAttackFrameBuf);
	CMstFSM::SetAllStateAttackSoundFrame(nAttackSoundFrameBuf);

	int nDelay[3];
	nDelay[0] = g_ResLoader.GetWalkDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
	nDelay[1] = g_ResLoader.GetRunDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
	nDelay[2] = g_ResLoader.GetRideDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
	CMstFSM::SetMoveDelay(nDelay);

	m_pCurrentState->Start();

	m_fStepWalk = g_ResLoader.GetStep(m_AniContext.nType, CrtStateWalk);
	m_fStepRun = g_ResLoader.GetStep(m_AniContext.nType, CrtStateRun);
	m_fStepRide = g_ResLoader.GetStep(m_AniContext.nType, CrtStateRideRun);

	//设置怪物的属性数据
	if (!g_ResLoader.GetMonsterbuf(m_dwShapID[m_nShapLv], m_dwModuleID, m_byteAlpha, m_bytePal))
	{
		return FALSE;
	}

	// 速度等级支持	
	SetStepWalk(m_fStepWalk);
	SetStepRun(m_fStepRun);
	SetStepRide(m_fStepRide);

	// 偏移位置相关
	mvcp.nHalfHeightOffset = g_ResLoader.GetHalfHeigh(m_AniContext.nType);
	mvcp.nHeightOffset = g_ResLoader.GetWholeHeigh(m_AniContext.nType);
	SetHalfHeightOffset(mvcp.nHalfHeightOffset);
	SetHeightOffset(mvcp.nHeightOffset);
	
	m_nBeHitLoopBeginFrame = g_ResLoader.GetBeHitLoopBegin(m_AniContext.nType);
	m_nBeHitLoopEndFrame = g_ResLoader.GetBeHitLoopEnd(m_AniContext.nType);

	memcpy (m_nActStateCommonAttack, mvcp.nActStateCommonAttack, sizeof(m_nActStateCommonAttack));
	m_nCommonAttackStyleNum = 0;
	for(int j = 0; j < 3; j++)
	{
		if(m_nActStateCommonAttack[j] >= 0)
			m_nCommonAttackStyleNum++;
	}
	m_nActStateJuqi = mvcp.nActStateJuqi;
	m_nActStateSkillAttack = mvcp.nActStateSkillAttack;
	m_nCommonAttackSoundID = mvcp.nSoundIDCommonAttack;
	
	m_nSoundIDFallow = mvcp.nSoundIDFallow;
	m_nSoundIDBeHit = mvcp.nSoundIDBeHit;
	m_nSoundIDDie = mvcp.nSoundIDDie;
	
	m_nFallowStateID = g_ResLoader.GetFallowStateID(m_AniContext.nType);

	IniTotalFrame();
	ClearShowRect();
	return TRUE;
}
//------------------------------------------------------------------------