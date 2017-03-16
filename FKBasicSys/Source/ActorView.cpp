/**
*	created:		2012-7-2   3:54
*	filename: 		ActorView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/ActorView.h"
#include "../Include/ResLoader.h"
#include "../Include/PalMgr.h"
#include "../Include/SkillCreate.h"
//------------------------------------------------------------------------
CActorView::CActorView() : CActFSM()
{
	memset(m_pModule, 0, sizeof(m_pModule));
	memset(m_dwModuleID, 0, sizeof(m_dwModuleID));
	m_pGroupInfoImage = NULL;
	AddFlag(flagInfo|flagName|flagPaoPao|flagGroup|flagBloodSlot);
	m_nFootprintLeftRight = 0;
	m_nWeaponStyle = ActWeaponStyleSingle;
	CActFSM::Create(this);
}
//------------------------------------------------------------------------
CActorView::~CActorView()
{
}
//------------------------------------------------------------------------
void CActorView::Release()
{
	Close();
	if (m_pCurrentState)
	{
		m_pCurrentState->End();
		m_pCurrentState = NULL;
	}
	delete this;
}
//------------------------------------------------------------------------
void CActorView::Close()
{
	m_pGroupInfoImage = NULL;
}
//------------------------------------------------------------------------
void CActorView::OnTimer(DWORD dwEventID)
{
	CActFSM::OnTimer(dwEventID);
}
//------------------------------------------------------------------------
// 获取对象的类ID
DWORD CActorView::GetClassID() const
{
	return miActorObj;
}
//------------------------------------------------------------------------
// 保存或读取数据，用于从文件中创建对象
BOOL CActorView::OnSerialize(BOOL bWrite,IArchive* pArchive)
{
	if (!pArchive) return FALSE;
	if (bWrite)
	{
		SActorViewArchive a = 
		{
			m_dwResID,
		};
			
		pArchive->Write((char*)&a, sizeof(a));
		return TRUE;
	}
	else
	{
		Close();
		if (!CreateItem((SActorViewArchive*)pArchive->GetBuffer()))
		{
			TraceLn("ERROR: CActorView::OnSerialize(bRead), create item failed!");
		}
		pArchive->Seek(sizeof(SActorViewArchive));
		return TRUE;
	}
}
//------------------------------------------------------------------------
// 根据数据现场创建对象
BOOL CActorView::OnCreateItem(const char* buf, int nLen)
{
	Close();
	return CreateItem((SActorViewArchive*)(buf));
}
//------------------------------------------------------------------------
// 更新(如：重载脚本数据等)
void CActorView::OnUpdate()
{
	//设置帧延时
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
	CActFSM::SetAllStateDelay(nDelayBuf);
	CActFSM::SetAllStateAttackFrame(nAttackFrameBuf);
	CActFSM::SetAllStateAttackSoundFrame(nAttackSoundFrameBuf);

	int nDelay[3];
	nDelay[0] = g_ResLoader.GetWalkDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
	nDelay[1] = g_ResLoader.GetRunDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
	nDelay[2] = g_ResLoader.GetRideDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
	CActFSM::SetMoveDelay(nDelay);

	m_pCurrentState->Start();
	m_fStepWalk = g_ResLoader.GetStep(m_AniContext.nType, CrtStateWalk);
	m_fStepRun = g_ResLoader.GetStep(m_AniContext.nType, CrtStateRun);
	m_fStepRide = g_ResLoader.GetStep(m_AniContext.nType, CrtStateRideRun);
	g_ResLoader.GetAnchor(m_AniContext.nType, m_ptAnchor);


	SetStepWalk(m_fStepWalk);
	SetStepRun(m_fStepRun);
	SetStepRide(m_fStepRide);

	// 偏移位置相关
	SetHalfHeightOffset(g_ResLoader.GetHalfHeigh(m_AniContext.nType));
	SetHeightOffset(g_ResLoader.GetWholeHeigh(m_AniContext.nType));

	m_nBeHitLoopBeginFrame = g_ResLoader.GetBeHitLoopBegin(m_AniContext.nType);
	m_nBeHitLoopEndFrame = g_ResLoader.GetBeHitLoopEnd(m_AniContext.nType);

	ClearShowRect();
	
	IniTotalFrame();

}
//------------------------------------------------------------------------
// 获取单占位对象的占位值
DWORD CActorView::GetOccupantValue()
{
	if (State() == CrtStateDie || !m_bOccupant)
		return 0x0;
	return CMapItem::GetOccupantValue();
}
//------------------------------------------------------------------------
// 获取对象在地图中的显示层
DWORD CActorView::GetLayerValue()
{
	if (State() == CrtStateDie)
		return milvLowest;
	return milvMidle;
}
//------------------------------------------------------------------------
// 设脏
void CActorView::SetDirty(ICanvas* pCanvas, const POINT& ptViewTop)
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
void CActorView::GetOldDirtyRect(list<RECT> &DirtyRectList)
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
void CActorView::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if(!pCanvas || !m_bVisible)
		return;

	// 画光效(draw order = draw_order_prev)
	vector<IAttachObj*>::iterator it;
	POINT pt;
	pt.x = m_ptTileOffset.x + ptTileCenter.x;
	pt.y = m_ptTileOffset.y + ptTileCenter.y;
	
	for (it = m_vAttachObjs.begin(); it != m_vAttachObjs.end(); ++it)
		for (int ni = 0; ni < (*it)->GetLoopNum(); ++ni)
			if ((*it)->GetDrawOrder(ni) == IAttachObj::draw_order_prev)
				(*it)->OnDraw(pCanvas, pt, dwFlag, ni);

	int nOldState = m_AniContext.nState;
	if(IsChangeShap())
	{
		// 如果怪物没有跑步动作那就用走路动作代替，策划那边需求的，可是感觉这样搞很别扭
		if(GetState() == CrtStateRun && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateRun)))
			m_AniContext.nState = CrtStateWalk;

		// 如果怪物没有走路动作那就用站立动作代替，策划那边需求的，可是感觉这样搞很别扭
		if(GetState() == CrtStateWalk && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateWalk)))
			m_AniContext.nState = CrtStateStand;
		
		// 如果怪物没有坐下动作那就用站立动作代替，策划那边需求的，可是感觉这样搞很别扭
		if(GetState() == CrtStateSit && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateSit)))
			m_AniContext.nState = CrtStateStand;
	}

	// 画角色
	int offx, offy;
	int nModuleType, nCurAni;
	CAniGroup* pCurObj;
	BYTE nDrawOrder[CrtPartMax/2];
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
			if (m_byteAlpha[nModuleType] == 0 || m_byteAlphaAll != 0)
			{
				pCurObj->Draw(pCanvas, 
					m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + offx,
					m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + offy,
					nCurAni, m_AniContext.nCurFrame, dwFlag,
					m_byteAlphaAll, m_Pal[nModuleType].GetPal(m_nCurPalType), m_pParticle[nModuleType]);
			}
			else
			{
				pCurObj->Draw(pCanvas, 
					m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + offx,
					m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + offy,
					nCurAni, m_AniContext.nCurFrame, dwFlag,
					m_byteAlpha[nModuleType], m_Pal[nModuleType].GetPal(m_nCurPalType), m_pParticle[nModuleType]);
			}
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
			if (m_byteAlpha[nModuleType] == 0 || m_byteAlphaAll != 0)
			{
				pCurObj->Draw(pCanvas,
					m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + offx,
					m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + offy,
					nCurAni, m_AniContext.nCurFrame, dwFlag,
					m_byteAlphaAll, m_Pal[nModuleType].GetPal(m_nCurPalType));
			}
			else
			{
				pCurObj->Draw(pCanvas,
					m_ptTileOffset.x + ptTileCenter.x - m_ptAnchor.x + offx,
					m_ptTileOffset.y + ptTileCenter.y - m_ptAnchor.y + offy,
					nCurAni, m_AniContext.nCurFrame, dwFlag,
					m_byteAlpha[nModuleType], m_Pal[nModuleType].GetPal(m_nCurPalType));
			}
		}
	}

	if(IsChangeShap())
	{
		m_AniContext.nState = nOldState;
	}
	
	// 画光效(draw order = draw_order_after)
	for (it=m_vAttachObjs.begin(); it!=m_vAttachObjs.end(); ++it)
		for (int ni = 0; ni < (*it)->GetLoopNum(); ++ni)
			if ((*it)->GetDrawOrder(ni) == IAttachObj::draw_order_after)
				(*it)->OnDraw(pCanvas, pt, dwFlag, ni);
};
//------------------------------------------------------------------------
// 绘制其他一些顶层对象，如血槽、聊天内容、物品名等，这些对象永远放在最上层
void CActorView::OnDrawTopmostObject(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag)
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

	if(GetState() == CrtStateSit)
		ptOffset.y += SIT_HEIGHT_OFFSET;

#ifndef _USE
	// draw chat info
	BOOL bHadDrawGroupInfo = FALSE;
	if (m_pPaoPao != NULL)
	{
		if ((dwFlags & flagGroup) && m_pGroupInfoImage)
		{
			ptOffset.y -= m_pGroupInfoImage->GetHeight();
			m_pGroupInfoImage->Draw(pCanvas, 
				ptOffset.x-m_pGroupInfoImage->GetWidth()/2, 
				ptOffset.y, BLIT_MASK);
			bHadDrawGroupInfo = TRUE;
		}
		// 称号
//		m_ActorTitle.OnDraw(pCanvas, ptOffset);
		// 泡泡
		if (dwFlags & flagPaoPao)
		{
			m_pPaoPao->OnDraw(pCanvas, ptOffset, m_szUserName, m_clrName, g_pFont);
		}
	}
	else // draw user name
	{
		if (dwFlags & flagName)
		{
			DrawName(pCanvas, ptOffset);
			ptOffset.y -= g_pFontWithOutline->GetHeight() + 3;
		}

		if ((dwFlags & flagGroup) && m_pGroupInfoImage && !bHadDrawGroupInfo)
		{
			ptOffset.y -= m_pGroupInfoImage->GetHeight();
			m_pGroupInfoImage->Draw(pCanvas, 
				ptOffset.x-m_pGroupInfoImage->GetWidth()/2, 
				ptOffset.y, BLIT_MASK);
		}
		
		// 称号
		m_ActorTitle.OnDraw(pCanvas, ptOffset);
	}
#endif
/*
#ifdef _USE
	DrawName(pCanvas, ptOffset);
	if (IsSelected())
	{
		WPixel clr = WPixel(255,255,0);
		WPixel clrText = WPixel(0,0,255);
		char buf[MAX_PATH];
		POINT ptTile;
		g_pMapView->ScreenToTile(ptTileCenter, ptTile);
		
		if (dwFlags & flagInfo)
		{
			IResObject* pObj = g_ResLoader.m_pScpBodyPart->GetResObject(m_dwResID);
			if (pObj)
			{
				IResObject* pParentObj = pObj->GetParentObject()->GetParentObject();
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
	if (State() == CrtStateDie)
		return;

	if (dwFlags & flagBloodSlot)
	{
		RECT rc;
		rc.left = ptTileCenter.x + m_ptTileOffset.x - BLOOD_SLOT_WIDTH/2;
		rc.right = rc.left + BLOOD_SLOT_WIDTH + 1;
		rc.top = ptTileCenter.y + m_ptTileOffset.y + GetHeightOffset();

		if(GetState() == CrtStateSit)
			rc.top += SIT_HEIGHT_OFFSET;

		rc.bottom = rc.top + BLOOD_SLOT_HEIGHT + 1;		
		DrawBloodSlot(pCanvas, rc, m_nBloodVol);
	}
/*
	//m_pGroupInfoImage->Draw(pCanvas)
#endif	
	*/
}
//------------------------------------------------------------------------
//换装
//dwParam1:生物部件类型
//dwParam2:部件ID
int CActorView::cmdChangeModule(DWORD dwParam1, DWORD dwParam2)
{
	if(IsChangeShap())
	{
		if(m_dwBakModuleID[dwParam1] == dwParam2)
			return TRUE;
		
		m_dwBakModuleID[dwParam1] = dwParam2;
		return TRUE;
	}
	else
	{
		if(m_dwModuleID[dwParam1] != dwParam2)
			ClearShowRect();

		// 修正资源ID
		DWORD dwID = m_dwModuleID[dwParam1];

		// 释放先前的资源
		if (m_pModule[dwParam1])
		{
			g_ResLoader.ReleaseRes(ioCreature + dwID);
			m_pModule[dwParam1] = NULL;
		}
		
		// 加载新的资源
		m_dwModuleID[dwParam1] = dwParam2;
		if (!RequestRes(dwParam1))
			return FALSE;

		return TRUE;
	}
}
//------------------------------------------------------------------------
//使用默认图包
//dwParam1:生物部件
//dwParam2:生物默认图包
int CActorView::cmdSpecialModule(DWORD dwParam1, DWORD dwParam2)
{
	if (dwParam1 < 0 || dwParam1 >= CrtPartMax || dwParam2 < 0)
		return 0;
	m_dwDefaultResID[dwParam1] = dwParam2;
	return 1;
}
//------------------------------------------------------------------------
//角色更换调色板
//dwParam1:部件类型
//dwParam2:调色板索引
int CActorView::cmdModulePalType(DWORD dwParam1, DWORD dwParam2)
{
	if(IsChangeShap())
	{
		if (dwParam1 >= 0 && dwParam1 < CrtPartMax && dwParam2 >=0 && dwParam2 < MAX_PAL_TYPES)
		{
			m_byteBakPal[dwParam1] = dwParam2;
			return TRUE;
		}
		return FALSE;
	}
	else
	{
		if (dwParam1 >= 0 && dwParam1 < CrtPartMax && dwParam2 >=0 && dwParam2 < MAX_PAL_TYPES)
		{
			m_bytePal[dwParam1] = dwParam2;
			return TRUE;
		}
		return FALSE;
	}
}
//------------------------------------------------------------------------
//角色更换透明度
//dwParam1:部件类型
//dwParam2:半透明值
int CActorView::cmdModuleAlpha(DWORD dwParam1, DWORD dwParam2)
{
	if(IsChangeShap())
	{
		if (dwParam1 >= 0 && dwParam1 < CrtPartMax && dwParam2 >=0 && dwParam2 < 256)
		{
			m_byteBakAlpha[dwParam1] = dwParam2;
			return TRUE;
		}
		return FALSE;
	}
	else
	{
		if (dwParam1 >= 0 && dwParam1 < CrtPartMax && dwParam2 >=0 && dwParam2 < 256)
		{
			m_byteAlpha[dwParam1] = dwParam2;
			return TRUE;
		}
		return FALSE;
	}
}
//------------------------------------------------------------------------
// 角色出生(发此命令可以播放一段出生动画)
// dwParam1: ?
// dwParam2: ?
int CActorView::cmdBorn(DWORD dwParam1, DWORD dwParam2)
{
	m_bDeaded =	FALSE;
	SetState(CrtStateStand);

	SSkillCreateParam scp;
	scp.nAniType = anitypeGenericAni;
	scp.nIndex = aniBorn;
	scp.ptTile = m_ptTile;
	scp.ptAttack = m_ptTile;

	return CSkillCreate::CreateGenericMagic(&scp);
}
//------------------------------------------------------------------------
// 角色升级(发此命令可以播放一段升级动画)
// dwParam1: ?
// dwParam2: ?
int CActorView::cmdUpgrade(DWORD dwParam1, DWORD dwParam2)
{
	IResObject* pScpSkill = (IResObject*)CSkillCreate::getSkillScpObj(anitypeGenericAni, aniUpgrade);
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
			CreateAttachObj(&st);
		}
	}
	Fun_PlaySound(m_ptTile, 567);
	return TRUE;
}
//------------------------------------------------------------------------
// 生物(这里只有主角)行走时，在路上点击鼠标留下的点击动画
// dwParam1: ?
// dwParam2: ?
int CActorView::cmdMouseAni(DWORD dwParam1, DWORD dwParam2)
{
	SSkillCreateParam scp;
	scp.nAniType = anitypeGenericAni;
	scp.nIndex = aniMouse;
	scp.ptTile = m_ptTile;
	scp.ptAttack = *(POINT*)dwParam1;
	
	return CSkillCreate::CreateGenericMagic(&scp);
}
//------------------------------------------------------------------------
// 给生物添加称号
// dwParam1: LPCSTR(格式化后的称号字符串)
// dwParam2: DWORD(称号效果的ID)
int CActorView::cmdAddTitle(DWORD dwParam1, DWORD dwParam2)
{
	return m_ActorTitle.AddTitle((LPCSTR)dwParam1, dwParam2);
}
//------------------------------------------------------------------------
// 清除生物的所有称号
// dwParam1: ?
// dwParam2: ?
int CActorView::cmdClearTitle(DWORD dwParam1, DWORD dwParam2)
{
	m_ActorTitle.ClearTitle();
	return TRUE;
}
//------------------------------------------------------------------------
// 角色携带武器的方式(单手武器?双手武器?弓箭类武器?)
// 设置或返回: int* enActorWeaponStyle类型
void CActorView::propWeaponStyle(BOOL bSetProp, DWORD dwPropVal)
{
	if (!bSetProp)
		*(int*)dwPropVal = m_nWeaponStyle;
	else
	{
		m_nWeaponStyle = *(int*)dwPropVal;
	}
}
//------------------------------------------------------------------------
// 角色武器的攻击音效ID
// 设置或返回: int*
void CActorView::propWeaponSoundID(BOOL bSetProp, DWORD dwPropVal)
{
	if(IsChangeShap())
	{
		if (!bSetProp)
		{
			*(int*)dwPropVal = m_nBakCommonAttackSoundID;
		}
		else
		{
			m_nBakCommonAttackSoundID = *(int*)dwPropVal;
		}
	}
	else
	{
		if (!bSetProp)
		{
			*(int*)dwPropVal = m_nCommonAttackSoundID;
		}
		else
		{
			m_nCommonAttackSoundID = *(int*)dwPropVal;
		}
	}
}
//------------------------------------------------------------------------
// 角色武器的击中目标音效ID
// 设置或返回: int*
void CActorView::propBeHitSoundID(BOOL bSetProp, DWORD dwPropVal)
{
	if(IsChangeShap())
	{
		if (!bSetProp)
		{
			*(int*)dwPropVal = m_nBakCommonAttackBeHitSoundID;
		}
		else
		{
			m_nBakCommonAttackBeHitSoundID = *(int*)dwPropVal;
		}
	}
	else
	{
		if (!bSetProp)
		{
			*(int*)dwPropVal = m_nCommonAttackBeHitSoundID;
		}
		else
		{
			m_nCommonAttackBeHitSoundID = *(int*)dwPropVal;
		}
	}
}
//------------------------------------------------------------------------
// 帮会信息图片
// 设置或返回: IBitmap*
void CActorView::propGroupInfoImage(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
	{
		m_pGroupInfoImage = *(IBitmap**)dwPropVal;
	}
	else
		*(IBitmap**)dwPropVal = m_pGroupInfoImage;
}
//------------------------------------------------------------------------
// 设置生物的当前状态
void CActorView::SetState(int nState)
{
	if(nState < 0 || nState >= CrtStateMax)
		return;
	
	if(nState == CrtStateBehit && GetMoveStyle() == ActMoveStyleRide)
		return;

	if(nState == CrtStateStand && GetMoveStyle() == ActMoveStyleRide)
		nState = CrtStateRideStand;

	if(m_pStates[nState] == NULL)
		return;

	if (CActFSM::GetCurState() == m_pStates[nState])
	{
		m_pStates[nState]->DelAllAttribute();
		return;
	}

	// 如果死了不作其他动作了
	if(IsDeaded() && nState != CrtStateDie)
		return;

	m_AniContext.nState = nState;

	if(IsChangeShap())
	{
		// 如果怪物没有跑步动作那就用走路动作代替，策划那边需求的，可是感觉这样搞很别扭
		if(GetState() == CrtStateRun && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateRun)))
			m_AniContext.nState = CrtStateWalk;

		// 如果怪物没有走路动作那就用站立动作代替，策划那边需求的，可是感觉这样搞很别扭
		if(GetState() == CrtStateWalk && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateWalk)))
			m_AniContext.nState = CrtStateStand;

		// 如果怪物没有该动作就站立吧
		if(!g_ResLoader.GetStateEnable(m_AniContext.nType, m_AniContext.nState))
		{
			m_AniContext.nState = CrtStateStand;
			nState = CrtStateStand;
		}
	}

	CActFSM::GotoState(*m_pStates[nState]);

	SetCurFrame(0);

	IniTotalFrame();
}
//------------------------------------------------------------------------
// 停止当前状态
void CActorView::StopState(int nState)
{
	if(nState < 0 || nState >= CrtStateMax)
		return;
	
	if(m_pStates[nState] == NULL)
		return;

	m_pStates[nState]->Stop();
}
//------------------------------------------------------------------------
// 重启当前状态
void CActorView::RestartCurState()
{
	GetCurState()->End();
	GetCurState()->Start();
}
//------------------------------------------------------------------------
// 获取普通攻击动作
int CActorView::GetNormalAttackActID()
{
	if(IsChangeShap())
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
	else
	{
		switch(m_nWeaponStyle) 
		{
		case ActWeaponStyleSingle:
			return CrtStateSingleAttack1;
		case ActWeaponStyleDouble:
			return CrtStateDoubleAttack1;
		case ActWeaponStyleBow:
			return CrtStateArrowAttack;
		default:
			break;
		}
	}
	return CrtStateSingleAttack1;
}
//------------------------------------------------------------------------
// 获取默认的蓄气动作ID（非玩家有效）
int CActorView::GetJuqiActID()
{
	if(IsChangeShap() && m_AniContext.nType != 0 && m_AniContext.nType != 1)
	{
		return m_nActStateJuqi;
	}
	else
		return -1;
}
//------------------------------------------------------------------------
// 获取默认的技能攻击动作ID（非玩家有效）
int CActorView::GetSkillAttackActID()
{
	if(IsChangeShap() && m_AniContext.nType != 0 && m_AniContext.nType != 1)
	{
		return m_nActStateSkillAttack;
	}
	else
		return -1;
}
//------------------------------------------------------------------------
// 设置对象矩形大小范围
void CActorView::SetShowRect()
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

	if(IsChangeShap())
	{
		// 如果怪物没有跑步动作那就用走路动作代替，策划那边需求的，可是感觉这样搞很别扭
		if(m_AniContext.nState == CrtStateRun && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateRun)))
			m_AniContext.nState = CrtStateWalk;

		// 如果怪物没有走路动作那就用站立动作代替，策划那边需求的，可是感觉这样搞很别扭
		if(m_AniContext.nState == CrtStateWalk && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateWalk)))
			m_AniContext.nState = CrtStateStand;
		
		// 如果怪物没有坐下动作那就用站立动作代替，策划那边需求的，可是感觉这样搞很别扭
		if(m_AniContext.nState == CrtStateSit && !(g_ResLoader.GetStateEnable(m_AniContext.nType, CrtStateSit)))
			m_AniContext.nState = CrtStateStand;
	}

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
void CActorView::SetAllMoveDelay(int* pDelay)
{
	SetMoveDelay(pDelay);
}
//------------------------------------------------------------------------
// 回调(跟随对象销毁时回调)
void CActorView::OnAttachObjDestroy(IAttachObj* pAttachObj)
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
void CActorView::OnFlowObjDestroy(IAttachObj* pAttachObj)
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
void CActorView::RemoveAllAttachObj()
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
// 获取武器类型
int CActorView::GetWeaponStyle()
{
	return m_nWeaponStyle;
}
//------------------------------------------------------------------------
// 设置目前状态下对应的技能ID
void CActorView::SetStatusMagicCheckID(int nCheckID)
{
	GetCurState()->SetMagicCheckID(nCheckID);
}
//------------------------------------------------------------------------
// 设置目前状态下要播放普通攻击声音
void CActorView::SetStatusPlayCommondAttackSound()
{
	GetCurState()->SetPlayCommondAttackSound();
}
//------------------------------------------------------------------------
// 设置目前状态下的蓄气属性
void CActorView::SetStatusPreAttack()
{
	GetCurState()->SetPreAttack();
}
//------------------------------------------------------------------------
// 设置目前状态下的一次性攻击属性
void CActorView::SetStatusAttackOnce()
{
	GetCurState()->SetAttackOnce();
}
//------------------------------------------------------------------------
// 设置目前状态下的持续攻击属性
void CActorView::SetStatusAttackCtrl()
{
	GetCurState()->SetAttackCtrl();
}
//------------------------------------------------------------------------
// 为了残影做的
void CActorView::OnDrawEx(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag, SAniContext *pContext, BYTE nAlpha)		// 为了残影做的
{
	if(!pCanvas || !m_bVisible || m_byteAlphaAll != 0)
		return;
	
	SAniContext OldAniContext;
	OldAniContext.SetData(&m_AniContext);
	m_AniContext.SetData(pContext);

	if(IsChangeShap())
	{
		if(!(g_ResLoader.GetStateEnable(m_AniContext.nType, GetState())))
			m_AniContext.nState = CrtStateStand;
	}

	// 画角色
	int offx, offy;
	int nModuleType, nCurAni;
	CAniGroup* pCurObj;
	BYTE nDrawOrder[CrtPartMax/2];
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
			nCurAni = GetCurAni(nModuleType);
			if (nCurAni == -1)
				continue;
			pCurObj->GetOffset(nCurAni, m_AniContext.nCurFrame, offx, offy);
			pCurObj->Draw(pCanvas, 
				ptTileCenter.x - m_ptAnchor.x + offx,
				ptTileCenter.y - m_ptAnchor.y + offy,
				nCurAni, m_AniContext.nCurFrame, dwFlag, nAlpha, m_Pal[nModuleType].GetPal(m_nCurPalType));
		}
	}
	m_AniContext.SetData(&OldAniContext);
}
//------------------------------------------------------------------------
BOOL CActorView::CreateItem(SActorViewArchive* pArchive)
{
	ASSERT(pArchive);
	struct STemp
	{
		DWORD dwActorType;
		ICreatureSink* pCrtSink;
	};
	m_pCrtSink = ((STemp*)pArchive)->pCrtSink;
	m_AniContext.nType = ((STemp*)pArchive)->dwActorType;
	m_dwType = m_AniContext.nType;

	OnUpdate();

	return TRUE;
}
//------------------------------------------------------------------------
// 创建脚印
int CActorView::CreateFootprint()
{
	SSkillCreateParam scp;
	scp.nAniType = anitypeGenericAni;
	scp.nIndex = aniFootprint;
	scp.ptTile = m_ptTile;
	scp.ptAttack = m_ptTile;
	m_nFootprintLeftRight = (m_nFootprintLeftRight == 0) ? 1 : 0;
	
	return CSkillCreate::CreateFootprintAnimate(&scp, GetDirection(), m_nFootprintLeftRight);	
}
//------------------------------------------------------------------------
// 变身
BOOL CActorView::ChangeShapEX()
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
	CActFSM::SetAllStateDelay(nDelayBuf);
	CActFSM::SetAllStateAttackFrame(nAttackFrameBuf);
	CActFSM::SetAllStateAttackSoundFrame(nAttackSoundFrameBuf);

	int nDelay[3];
	nDelay[0] = g_ResLoader.GetWalkDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
	nDelay[1] = g_ResLoader.GetRunDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
	nDelay[2] = g_ResLoader.GetRideDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
	CActFSM::SetMoveDelay(nDelay);

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
	m_nCommonAttackBeHitSoundID = 0;
	
	m_nSoundIDFallow = mvcp.nSoundIDFallow;
	m_nSoundIDBeHit = mvcp.nSoundIDBeHit;
	m_nSoundIDDie = mvcp.nSoundIDDie;
	
	IniTotalFrame();
	ClearShowRect();
	return TRUE;
}
//------------------------------------------------------------------------