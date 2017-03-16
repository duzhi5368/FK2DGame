/**
*	created:		2012-7-2   2:24
*	filename: 		CreatureView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		基本生物类
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/CreatureView.h"
#include "../Include/ResLoader.h"
#include "../Include/SkillCreate.h"
#include "../../FKGame/MapInfo/Tile.h"
#include "../Include/MagicMgrShadow.h"
//------------------------------------------------------------------------
CCreatureView::ProcMoveStepByDir CCreatureView::m_MoveStepByDir[CrtDirMaxDirCount] =
{
	&CCreatureView::_move_step_north,
	&CCreatureView::_move_step_eastnorth,
	&CCreatureView::_move_step_east,
	&CCreatureView::_move_step_eastsouth,
	&CCreatureView::_move_step_south,
	&CCreatureView::_move_step_westsouth,
	&CCreatureView::_move_step_west,
	&CCreatureView::_move_step_westnorth,
};
//------------------------------------------------------------------------
CCreatureView::CCreatureView()
{
	memset (this, 0, sizeof(CCreatureView));
	m_clrName = WPixel(255,255,255);

	m_nShapLv = -1;
	m_nActStateJuqi = -1;
	m_nActStateSkillAttack = -1;
	memset(m_nActStateCommonAttack, -1, sizeof(m_nActStateCommonAttack));
	m_nCurPalType = -1;
	m_wpMinMapFlagColor = WPixel(255,255,255);
	m_nMoveStyle = ActMoveStyleWalk;
	m_bShowUserName = TRUE;
	m_bNormalMove = TRUE;
	m_bVisible = TRUE;
	m_bOccupant = TRUE;
	m_nCurNodeIndex = -1;
	m_nLastMoveDir = -1;
	m_nBloodVol = 100;
	m_nAttackSoundChunnelID = -1;
	for(int i = 0; i < CrtPartMax; i++)
	{
		m_pParticle[i] = NULL;
	}
}
//------------------------------------------------------------------------
CCreatureView::~CCreatureView()
{
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_FRAME, static_cast<ITimerSink*>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_SHORT_STEPS, static_cast<ITimerSink*>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_DIE, static_cast<ITimerSink*>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_DIE_ANI, static_cast<ITimerSink*>(this));
	Close();
	RemoveAllAttachObj();
	RemoveAllFlowObj();
	
	SAFE_DELETE(m_pMagicShadow);

	vector<CMagicMgrBase*>::iterator it;
	for(it = m_vMagicMgr.begin(); it != m_vMagicMgr.end(); it++)
	{
		if((*it)->IsRunning())
			(*it)->Stop();
		(*it)->SetMasterView(NULL);
	}

	vector<CCreatureView**>::iterator it1;
	for (it1 = m_vppLinkView.begin(); it1 != m_vppLinkView.end(); it1++)
	{
		if(*it1 != NULL)
			*(*it1) = NULL;
	}
	m_vppLinkView.clear();
	
	for(int i = 0; i < CrtPartMax; i++)
	{
		SAFE_RELEASE(m_pParticle[i]);
	}
}
//------------------------------------------------------------------------
void CCreatureView::Close()
{
	for (int i = 0; i < CrtPartMax; i++)
	{
		if (m_pModule[i])
		{
			DWORD dwID = m_dwModuleID[i];
			if (dwID != 0)
				g_ResLoader.ReleaseRes(ioCreature + dwID);
			m_pModule[i] = NULL;	
		}
	}
	//删除所有指针
#ifndef _USE_BINSCRIPT		//这个是为了满足Debug版本的需求。两个版本的脚本读取不一样
	for (int j = 0; j < CrtPartMax; j++)
	{
		SAFE_DELETE_ARRAY(m_PartStateFlag[j]);
	}
#endif
	SAFE_DELETE_ARRAY(m_pPathList);
	SAFE_DELETE_ARRAY(m_pPointList);
}
//------------------------------------------------------------------------
// 各种指令的执行
int CCreatureView::OnCommand(DWORD dwCmdID, DWORD dwParam1,DWORD dwParam2)
{
	if (dwCmdID < 0 || dwCmdID >= CrtCmdMaxCommandCount)
	{
		TraceColorLn(MY_TRACE_COLOR, "CCreatureView::OnCommand, Unknow Command ID = %d", dwCmdID);
		return FALSE;
	}
	if (m_Command[dwCmdID] == NULL)
	{
		TraceColorLn(MY_TRACE_COLOR, "CCreatureView::OnCommand(), Command is not implemented, ID = %d", dwCmdID);
		return FALSE;
	}
	
	return (this->*m_Command[dwCmdID])(dwParam1, dwParam2);
}
//------------------------------------------------------------------------
// 设置在小地图上的颜色
void CCreatureView::SetFlagColorOnMinmap(DWORD dwColor)
{
	m_wpMinMapFlagColor = WPixel(dwColor);
}
//------------------------------------------------------------------------
// 取得在小地图上的颜色
DWORD CCreatureView::GetFlagColorOnMinmap()
{
	return m_wpMinMapFlagColor;
}
//------------------------------------------------------------------------
// 取得是否在小地图上显示
BOOL CCreatureView::GetVisibleOnMinmap()
{
	return TRUE;
}
//------------------------------------------------------------------------
// 设置对象的私有数据
void CCreatureView::SetItemData(DWORD dwData)
{
	m_dwItemData = dwData;
}
//------------------------------------------------------------------------
// 获取对象的私有数据
DWORD CCreatureView::GetItemData() const
{
	return m_dwItemData;
}
//------------------------------------------------------------------------
// 设置在地图上的位置
void CCreatureView::SetTile(POINT& ptTile, IMapView* pMapView)
{
		m_ptTile = ptTile;
		
		COccupantTileList* potl = GetOccupantTileList();
		if (potl == NULL)
		{
			m_rcArea.right = m_rcArea.left = m_ptTile.x;
			m_rcArea.bottom = m_rcArea.top = m_ptTile.y;	// 是单点对象，右上角跟左下角一样
		}
		else
		{
			COccupantTileListHeader& octh = potl->GetPreData();
			m_rcArea.left = m_ptTile.x + octh.nOffAnchorX;
			m_rcArea.top = m_ptTile.y + octh.nOffAnchorY;
			m_rcArea.right = m_rcArea.left + octh.wWidth - 1;		// rcArea.right为平视右端点
			m_rcArea.bottom = m_rcArea.top + octh.wHeight - 1;
		}
		
		pMapView->TileToWorld((POINT&)m_rcArea, m_ptLeft);
		
		POINT ptRightTile = {m_rcArea.right, m_rcArea.bottom};
		pMapView->TileToWorld(ptRightTile, m_ptRight);
		
		pMapView->TileToWorld(m_ptTile, m_ptWold);
}
//------------------------------------------------------------------------
// 获取在地图上的位置
POINT CCreatureView::GetTile()
{
	return m_ptTile;
}
//------------------------------------------------------------------------
// 设置对象选中状态
BOOL CCreatureView::SetSelected(BOOL bSelected, BOOL bHighLight)
{
	return TRUE;
}
//------------------------------------------------------------------------
// 设置对象锁定状态
BOOL CCreatureView::SetLocked(int nLockType)
{
	if(m_pLockAttachObj)
	{
		OnAttachObjDestroy(m_pLockAttachObj);
		m_pLockAttachObj = NULL;
	}

	if(nLockType != 0)
	{
		IResObject* pScpSkill = (IResObject*)CSkillCreate::getSkillScpObj(anitypeGenericAni, aniBeLock_Red + nLockType - 1);
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
				
				m_pLockAttachObj = CreateAttachObj(&st);
			}
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 获取资源ID
DWORD CCreatureView::GetResID()
{
	return m_dwResID;
}
//------------------------------------------------------------------------
// 清除对象矩形大小范围
void CCreatureView::ClearShowRect()
{
	memset(m_ItemShowRect, 0, sizeof(m_ItemShowRect));
}
//------------------------------------------------------------------------
// 获取对象矩形大小范围
RECT CCreatureView::GetShowRect()
{
	RECT rc = {0,0,0,0};

	SetShowRect();
	rc = m_ItemShowRect[m_AniContext.nState][m_AniContext.nDir][m_AniContext.nCurFrame].rc;
	OffsetRect(&rc,
		m_ptTileOffset.x,
		m_ptTileOffset.y);
	return rc;
}
//------------------------------------------------------------------------
// 获取锚点
POINT CCreatureView::GetAnchor() const
{
	return m_ptAnchor;
}
//------------------------------------------------------------------------
// 测试当用户点中某个小区域时是否选择此对象
BOOL CCreatureView::HitTest(const RECT& rcScreen)
{
	if(!m_bVisible && this->GetItemData() != (DWORD)(g_pPlayer))
		return FALSE;

	if(m_bDeaded && GetClassID() == miNpcObj)
		return FALSE;

	if (!m_pModule[CrtPartHead] 
		&& !m_pModule[CrtPartUpBody] 
		&& !m_pModule[CrtPartDownBody] 
		&& !m_pModule[CrtPartBody] 
		&& !m_pModule[CrtPartWholeBody]) 
		return FALSE;

	RECT rcIntersect;
	RECT rcShowRect = GetShowRect();
	RECT rcSelect = rcScreen;
	if (!IntersectRect(&rcIntersect, &rcSelect, &rcShowRect))
		return FALSE;
	
	for (int i = 0; i < CrtPartMax; i++)
	{
		if (m_pModule[i])
		{
			IBitmap* pBitmap = m_pModule[i]->GetBitmap(GetCurAni(i), GetCurFrame());
			if (!pBitmap)
				continue;
			
			int offx, offy;
			m_pModule[i]->GetOffset(GetCurAni(i), GetCurFrame(), offx, offy);
			
			rcSelect = rcScreen;
			OffsetRect(&rcSelect, m_ptAnchor.x - offx - m_ptTileOffset.x,
								  m_ptAnchor.y - offy - m_ptTileOffset.y);
			RECT rcBmp = {0, 0, pBitmap->GetWidth(), pBitmap->GetHeight()};
			rcIntersect;
			if (!IntersectRect(&rcIntersect, &rcSelect, &rcBmp))
				continue;

			POINT ptSelect;
			for (int i = rcIntersect.top; i < rcIntersect.bottom; i++)
			{
				ptSelect.y = i;
				for (int j = rcIntersect.left; j < rcIntersect.right; j++)
				{
					ptSelect.x = j;
					if (!pBitmap->IsTransparent(ptSelect))
						return TRUE;
				}
			}
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------
// 当对象进入视口和离开视口时被回调
BOOL CCreatureView::OnEnterViewport(BOOL bVisable)
{
	if (bVisable)
	{
		//资源图包全部预读
		for (int i = 0; i < CrtPartMax; i++)
		{
			RequestRes(i);
		}
		return TRUE;
	}
	else		//释放所有资源
	{
		// 丢掉身上可能还有的吸附物
		RemoveAllAttachObj();

		DWORD dwID = 0;
		for (int i = 0; i < CrtPartMax; i++)
		{
			if (m_pModule[i])
			{
				dwID = m_dwModuleID[i];
				if (!g_ResLoader.ReleaseRes(ioCreature + dwID))
					return FALSE;
				m_pModule[i] = NULL;	
			}
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------
// 是否潜水，哇哈哈
BOOL CCreatureView::IsSinking()
{
	return !m_bVisible;
}
//------------------------------------------------------------------------
// 获取当前状态
int& CCreatureView::State()
{
	return m_AniContext.nState;
}
//------------------------------------------------------------------------
// 获取当前状态
int CCreatureView::GetState()
{
	return m_AniContext.nState;
}
//------------------------------------------------------------------------
// 下一祯
void CCreatureView::IncCurFrame()
{
	m_AniContext.nCurFrame++;
}
//------------------------------------------------------------------------
// 上一祯
void CCreatureView::DecCurFrame()
{
	m_AniContext.nCurFrame--;
}
//------------------------------------------------------------------------
// 转到下一祯并判断是否大于最大祯数
bool CCreatureView::IncAndCheckFrameOverflow()
{
	return (++m_AniContext.nCurFrame >= m_AniContext.nFrames);
}
//------------------------------------------------------------------------
// 转到下一祯并判断是否大于参数
bool CCreatureView::IncAndCheckFrameOverflow(int nFrameIndex)
{
	return (++m_AniContext.nCurFrame >= nFrameIndex + 1);
}
//------------------------------------------------------------------------
// 设置当前祯
void CCreatureView::SetCurFrame(int nFrame)
{
	m_AniContext.nCurFrame = nFrame;
}
//------------------------------------------------------------------------
// 获取当前祯
int& CCreatureView::CurFrame()
{
	return m_AniContext.nCurFrame;
}
//------------------------------------------------------------------------
// 获取当前祯
int CCreatureView::GetCurFrame()
{
	return m_AniContext.nCurFrame;
}
//------------------------------------------------------------------------
// 初始化当前状态的最大祯
void CCreatureView::IniTotalFrame()
{
	//设定最大帧数
	if (m_pModule[CrtPartHead])
		SetTotalFrame(m_pModule[CrtPartHead]->GetTotalFrames(GetCurAni(CrtPartHead)));
	else if (m_pModule[CrtPartBody])
		SetTotalFrame(m_pModule[CrtPartBody]->GetTotalFrames(GetCurAni(CrtPartBody)));
	else if (m_pModule[CrtPartUpBody])
		SetTotalFrame(m_pModule[CrtPartUpBody]->GetTotalFrames(GetCurAni(CrtPartUpBody)));
	else if (m_pModule[CrtPartWholeBody])
		SetTotalFrame(m_pModule[CrtPartWholeBody]->GetTotalFrames(GetCurAni(CrtPartWholeBody)));
}
//------------------------------------------------------------------------
// 设最大祯
void CCreatureView::SetTotalFrame(int nFrames)
{
	m_AniContext.nFrames = nFrames;
}
//------------------------------------------------------------------------
// 获取最大祯
int CCreatureView::GetTotalFrame()
{
	return m_AniContext.nFrames;
}
//------------------------------------------------------------------------
// 返回当前是第几个动画，参数：第几个部件
int CCreatureView::GetCurAni(int nFind)
{
	if(nFind < 0 || nFind >= CrtPartMax)
		return -1;

	if(m_PartStateFlag[nFind] == NULL)
		return -1;

	int nState = GetState();
	if (m_PartStateFlag[nFind][nState] == 1)
	{

		int nPos = -1;
		for (int i = 0; i <= m_AniContext.nState; i++)
		{
			if (m_PartStateFlag[nFind][i] == 1)
			{
				nPos++;
			}
		}
		return nPos * 8 + m_AniContext.nDir;
	}
	return -1;
}
//------------------------------------------------------------------------
// 获取祯延迟
int CCreatureView::GetFrameDelay(int nGrade)
{
	if (nGrade == -1)
	{
		return g_ResLoader.GetFrameDelay(m_AniContext.nType, m_AniContext.State());
	}
	else
	{
		return g_ResLoader.GetDelayByGrade(m_AniContext.nType, nGrade);
	}
}
//------------------------------------------------------------------------
// 获取该部件是否需要改变调色板
BOOL CCreatureView::GetPartNeedChangePal(int nModuleType)
{
	if(m_dwModuleID[CrtPartWholeBody] != 0 && nModuleType != CrtPartWholeBody)
		return FALSE;

	if(m_dwModuleID[CrtPartWHoleBodyEffect] != 0 && nModuleType != CrtPartWHoleBodyEffect)
		return FALSE;

	return TRUE;
}
//------------------------------------------------------------------------
// 读取图包
CAniGroup* CCreatureView::RequestRes(int nModuleType)
{
	if (m_pModule[nModuleType])
		return m_pModule[nModuleType];
		
	DWORD dwID = m_dwModuleID[nModuleType];
	if (dwID != 0)
		m_pModule[nModuleType] = static_cast<CAniGroup*>(g_ResLoader.RequestCreature(dwID));	

	if (dwID == 0 || m_pModule[nModuleType] == NULL)
	{
		if (m_dwDefaultResID[nModuleType] == 0)
		{
			return NULL;
		}
		else
		{
			m_pModule[nModuleType] = static_cast<CAniGroup*>(g_ResLoader.RequestCreature(m_dwDefaultResID[nModuleType]));
			if (m_pModule[nModuleType] == NULL)
			{
				return NULL;
			}
			m_dwModuleID[nModuleType] = m_dwDefaultResID[nModuleType];
			dwID = m_dwModuleID[nModuleType];
		}
	}
	if (m_pModule[nModuleType]->GetUserPal(0) == NULL) // 加载用户调色板
	{
		int nCount = ptMaxPalTypeCount;
		BYTE* pPal = m_pModule[nModuleType]->GetPal();
		int nUseColor = m_pModule[nModuleType]->GetUseColor();
		m_pModule[nModuleType]->SetMaxUserPalCount(nUseColor);
		//设置原始调色板
		m_Pal[nModuleType].SetPal(pPal, nUseColor);
		//如果使用调色板，改变调色板
		if (m_bytePal[nModuleType] != 0)
		{
			int *p = g_ResLoader.GetHSLpal(dwID, m_bytePal[nModuleType]);
			if (!p)
				return NULL;
			m_Pal[nModuleType].OnHSLChanged(p[0], p[1], p[2]);
#ifndef _USE_BINSCRIPT		//这个是为了满足Debug版本的需求。两个版本的脚本读取不一样
			SAFE_DELETE_ARRAY(p);
#endif
		}
	}
	//加载部件状态标志
#ifndef _USE_BINSCRIPT		//这个是为了满足Debug版本的需求。两个版本的脚本读取不一样
	SAFE_DELETE_ARRAY(m_PartStateFlag[nModuleType]);
#endif
	if (NULL == (m_PartStateFlag[nModuleType] = g_ResLoader.GetStateFlag(nModuleType, dwID)))
		return NULL;

	//记录生物该状态共有几帧
	if (nModuleType == CrtPartHead || nModuleType == CrtPartBody
		|| nModuleType == CrtPartWholeBody || nModuleType == CrtPartUpBody)
	{
		int nCurAni = GetCurAni(nModuleType);
		if (nCurAni == -1)//不用Release()，因为每次使用前都会先Release()掉
			return NULL;
		m_AniContext.nFrames = m_pModule[nModuleType]->GetTotalFrames(nCurAni);
	}
	return m_pModule[nModuleType];
}
//------------------------------------------------------------------------
// 设置是否可见
void CCreatureView::SetVisible(BOOL bVisible)
{
	if(!bVisible && this->GetItemData() == (DWORD)(g_pPlayer))
	{
		m_bVisible = TRUE;
		m_byteAlphaAll = 100;
	}
	else
	{
		m_bVisible = bVisible;
		if(!m_bVisible && m_pLockAttachObj != NULL)
		{
			SetLocked(FALSE);
			gp->m_pWndMgr->PostWindowMessage(WND_TARGETRIM, WE_WC_TARGET_CANCELTARGET, 0, 0);
		}
	}
}
//------------------------------------------------------------------------
// 设置是否可见
BOOL CCreatureView::GetVisible()
{
	return m_bVisible;
}
//------------------------------------------------------------------------
// 设置半高
void CCreatureView::SetHalfHeightOffset(int nOffSet)
{
	m_nHalfHeightOffset = nOffSet;
}
//------------------------------------------------------------------------
// 获取半高
int CCreatureView::GetHalfHeightOffset()
{
	return m_nHalfHeightOffset;
}
//------------------------------------------------------------------------
// 设置全高
void CCreatureView::SetHeightOffset(int nOffSet)
{
	m_nHeightOffset = nOffSet;
}
//------------------------------------------------------------------------
// 获取全高
int CCreatureView::GetHeightOffset()
{
	return m_nHeightOffset;
}
//------------------------------------------------------------------------
void CCreatureView::_move_step_north(POINT& ptTileOffsetOld)
{
	bool bChangeDir1 = (ptTileOffsetOld.y > 0 && m_ptTileOffset.y <= 0);
	if (bChangeDir1)
		OnChangeDir();
	bool bChangeTile = (m_ptTileOffset.y <= -16);
	bool bChangeDir2 = (ptTileOffsetOld.y <= 0 && m_ptTileOffset.y <= -32);
	if (bChangeTile)
	{
		if(OnChangeTile())
		{
			m_ptTileOffset.y = m_ptTileOffset.y + 32;
		}
		else
		{
			m_ptTileOffset = ptTileOffsetOld;
		}
	}
	if (bChangeDir2)
		OnChangeDir();
}
//------------------------------------------------------------------------
void CCreatureView::_move_step_eastnorth(POINT& ptTileOffsetOld)
{
	bool bChangeDir1 = (ptTileOffsetOld.x < 0 && m_ptTileOffset.x >= 0);
	if (bChangeDir1)
		OnChangeDir();
	bool bChangeTile = (m_ptTileOffset.x >= 16);
	bool bChangeDir2 = (ptTileOffsetOld.x >= 0 && m_ptTileOffset.x >= 32);
	if (bChangeTile)
	{
		if(OnChangeTile())
		{
			m_ptTileOffset.x = m_ptTileOffset.x - 32;
			m_ptTileOffset.y = m_ptTileOffset.y + 16;
		}
		else
		{
			m_ptTileOffset = ptTileOffsetOld;
		}
	}
	if (bChangeDir2)
		OnChangeDir();
}
//------------------------------------------------------------------------
void CCreatureView::_move_step_east(POINT& ptTileOffsetOld)
{
	bool bChangeDir1 = (ptTileOffsetOld.x < 0 && m_ptTileOffset.x >= 0);
	if (bChangeDir1)
		OnChangeDir();
	bool bChangeTile = (m_ptTileOffset.x >= 32);
	bool bChangeDir2 = (ptTileOffsetOld.x >= 0 && m_ptTileOffset.x >= 64);
	if (bChangeTile)
	{
		if(OnChangeTile())
		{
			m_ptTileOffset.x = m_ptTileOffset.x - 64;
		}
		else
		{
			m_ptTileOffset = ptTileOffsetOld;
		}
	}
	if (bChangeDir2)
		OnChangeDir();
}
//------------------------------------------------------------------------
void CCreatureView::_move_step_eastsouth(POINT& ptTileOffsetOld)
{
	bool bChangeDir1 = (ptTileOffsetOld.x < 0 && m_ptTileOffset.x >= 0);
	if (bChangeDir1)
		OnChangeDir();
	bool bChangeTile = (m_ptTileOffset.x >= 16);
	bool bChangeDir2 = (ptTileOffsetOld.x >= 0 && m_ptTileOffset.x >= 32);
	if (bChangeTile)
	{
		if(OnChangeTile())
		{
			m_ptTileOffset.x = m_ptTileOffset.x - 32;
			m_ptTileOffset.y = m_ptTileOffset.y - 16;
		}
		else
		{
			m_ptTileOffset = ptTileOffsetOld;
		}
	}
	if (bChangeDir2)
		OnChangeDir();
}
//------------------------------------------------------------------------
void CCreatureView::_move_step_south(POINT& ptTileOffsetOld)
{
	bool bChangeDir1 = (ptTileOffsetOld.y < 0 && m_ptTileOffset.y >= 0);
	if (bChangeDir1)
		OnChangeDir();
	bool bChangeTile = (m_ptTileOffset.y >= 16);
	bool bChangeDir2 = (ptTileOffsetOld.y >= 0 && m_ptTileOffset.y >= 32);
	if (bChangeTile)
	{
		if(OnChangeTile())
		{
			m_ptTileOffset.y = m_ptTileOffset.y - 32;
		}
		else
		{
			m_ptTileOffset = ptTileOffsetOld;
		}
	}
	if (bChangeDir2)
		OnChangeDir();
}
//------------------------------------------------------------------------
void CCreatureView::_move_step_westsouth(POINT& ptTileOffsetOld)
{
	bool bChangeDir1 = (ptTileOffsetOld.x > 0 && m_ptTileOffset.x <= 0);
	if (bChangeDir1)
		OnChangeDir();
	bool bChangeTile = (m_ptTileOffset.x <= -16);
	bool bChangeDir2 = (ptTileOffsetOld.x <= 0 && m_ptTileOffset.x <= -32);
	if (bChangeTile)
	{
		if(OnChangeTile())
		{
			m_ptTileOffset.x = m_ptTileOffset.x + 32;
			m_ptTileOffset.y = m_ptTileOffset.y - 16;
		}
		else
		{
			m_ptTileOffset = ptTileOffsetOld;
		}
	}
	if (bChangeDir2)
		OnChangeDir();
}
//------------------------------------------------------------------------
void CCreatureView::_move_step_west(POINT& ptTileOffsetOld)
{
	bool bChangeDir1 = (ptTileOffsetOld.x > 0 && m_ptTileOffset.x <= 0);
	if (bChangeDir1)
		OnChangeDir();
	bool bChangeTile = (m_ptTileOffset.x <= -32);
	bool bChangeDir2 = (ptTileOffsetOld.x <= 0 && m_ptTileOffset.x <= -64);
	if (bChangeTile)
	{
		if(OnChangeTile())
		{
			m_ptTileOffset.x = m_ptTileOffset.x + 64;
		}
		else
		{
			m_ptTileOffset = ptTileOffsetOld;
		}
	}
	if (bChangeDir2)
		OnChangeDir();
}
//------------------------------------------------------------------------
void CCreatureView::_move_step_westnorth(POINT& ptTileOffsetOld)
{
	bool bChangeDir1 = (ptTileOffsetOld.x > 0 && m_ptTileOffset.x <= 0);
	if (bChangeDir1)
		OnChangeDir();
	bool bChangeTile = (m_ptTileOffset.x <= -16);
	bool bChangeDir2 = (ptTileOffsetOld.x <= 0 && m_ptTileOffset.x <= -32);
	if (bChangeTile)
	{
		if(OnChangeTile())
		{
			m_ptTileOffset.x = m_ptTileOffset.x + 32;
			m_ptTileOffset.y = m_ptTileOffset.y + 16;
		}
		else
		{
			m_ptTileOffset = ptTileOffsetOld;
		}
	}
	if (bChangeDir2)
		OnChangeDir();
}
//------------------------------------------------------------------------
// 设置生物的行走步长(以竖直方向为准)
void CCreatureView::SetStepWalk(float fStep)
{
	const float fRateX = 1.414f;
	const float fRateY = 0.707f;
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
void CCreatureView::SetStepRun(float fStep)
{
	const float fRateX = 1.414f;
	const float fRateY = 0.707f;
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
void CCreatureView::SetStepRide(float fStep)
{
	const float fRateX = 1.414f;
	const float fRateY = 0.707f;
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
// 获取移动步长
void CCreatureView::GetMoveStep(int nDir, float& x, float& y)
{
	switch(m_nMoveStyle)
	{
	case ActMoveStyleWalk:
		x = m_StepWalk[nDir].x;
		y = m_StepWalk[nDir].y;
		break;
	case ActMoveStyleRun:
		x = m_StepRun[nDir].x;
		y = m_StepRun[nDir].y;
		break;
	case ActMoveStyleRide:
		x = m_StepRide[nDir].x;
		y = m_StepRide[nDir].y;
		break;
	default:
		break;
	}
}
//------------------------------------------------------------------------
// 获取碎步移动步长
void CCreatureView::GetMoveStepEx(int nDir, float& x, float& y)
{
	int nFrameDelay;
	switch(m_nMoveStyle)
	{
	case ActMoveStyleWalk:
		nFrameDelay = g_ResLoader.GetWalkDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
		x = (float)m_StepWalk[nDir].x * (float)TIME_SHORT_STEPS / (float)nFrameDelay;
		y = (float)m_StepWalk[nDir].y * (float)TIME_SHORT_STEPS / (float)nFrameDelay;
		break;
	case ActMoveStyleRun:
		nFrameDelay = g_ResLoader.GetRunDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
		x = (float)m_StepRun[nDir].x * (float)TIME_SHORT_STEPS / (float)nFrameDelay;
		y = (float)m_StepRun[nDir].y * (float)TIME_SHORT_STEPS / (float)nFrameDelay;
		break;
	case ActMoveStyleRide:
		nFrameDelay = g_ResLoader.GetRideDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
		x = (float)m_StepRide[nDir].x * (float)TIME_SHORT_STEPS / (float)nFrameDelay;
		y = (float)m_StepRide[nDir].y * (float)TIME_SHORT_STEPS / (float)nFrameDelay;
		break;
	default:
		break;
	}
}
//------------------------------------------------------------------------
// 设置移动方式
void CCreatureView::SetMoveStyle(int nMoveStyle)
{
	// 如果改成骑马的生物全高要增加
	if(m_nMoveStyle != ActMoveStyleRide
		&& nMoveStyle == ActMoveStyleRide)
	{
		m_nHeightOffset -= 18;
		m_nHalfHeightOffset -= 9;
	}
	// 如果骑马改成不骑马生物全高
	else if(m_nMoveStyle == ActMoveStyleRide
		&& nMoveStyle != ActMoveStyleRide)
	{
		m_nHeightOffset += 18;
		m_nHalfHeightOffset += 9;
	}
	m_nMoveStyle = nMoveStyle;
}
//------------------------------------------------------------------------
// 获取移动方式
int CCreatureView::GetMoveStyle()
{
	return m_nMoveStyle;
}
//------------------------------------------------------------------------
// 通过移动方式获取移动状态
int CCreatureView::GetMoveStateByStyle()
{
	if (m_nMoveStyle == ActMoveStyleWalk)
	{
		return CrtStateWalk;
	}
	else if (m_nMoveStyle == ActMoveStyleRun)
	{
		return CrtStateRun;
	}
	else if (m_nMoveStyle == ActMoveStyleRide)
	{
		return CrtStateRideRun;
	}
	return 0;
}
//------------------------------------------------------------------------
// 设置当前方向
void CCreatureView::SetDirection(int nDir)
{
	ASSERT(nDir >= 0 && nDir < CrtDirMaxDirCount);
	if (m_AniContext.nDir == nDir) return;
	m_AniContext.nDir = nDir;
	
	IniTotalFrame();
}
//------------------------------------------------------------------------
// 获取当前方向
int CCreatureView::GetDirection()
{
	return m_AniContext.nDir;
}
//------------------------------------------------------------------------
// 矫正当前移动速度
void CCreatureView::CalcSpeedMove()
{
	int nFrameDelay;
	int i;
	switch(m_nMoveStyle)
	{
	case ActMoveStyleWalk:
		for (i = 0; i < CrtDirMaxDirCount; i++)
		{
			nFrameDelay = g_ResLoader.GetWalkDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
			m_SpeedMove[i].x = m_StepWalk[i].x / (float)nFrameDelay;
			m_SpeedMove[i].y = m_StepWalk[i].y / (float)nFrameDelay;
		}
		return;
	case ActMoveStyleRun:
		for (i = 0; i < CrtDirMaxDirCount; i++)
		{
			nFrameDelay = g_ResLoader.GetRunDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
			m_SpeedMove[i].x = m_StepRun[i].x / (float)nFrameDelay;
			m_SpeedMove[i].y = m_StepRun[i].y / (float)nFrameDelay;
		}
		return;
	case ActMoveStyleRide:
		for (i = 0; i < CrtDirMaxDirCount; i++)
		{
			nFrameDelay = g_ResLoader.GetRideDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
			m_SpeedMove[i].x = m_StepRide[i].x / (float)nFrameDelay;
			m_SpeedMove[i].y = m_StepRide[i].y / (float)nFrameDelay;
		}
		return;
	default:
		return;
	}
}
//------------------------------------------------------------------------
// 获取当前移动速度
CCreatureView::SStep* CCreatureView::GetSpeedMove(int nDir)
{
	return &m_SpeedMove[nDir];
}
//------------------------------------------------------------------------
// 设置当前方向下的移动速度
void CCreatureView::SetCurSpeed(SStep* pSpeed)
{
	if (pSpeed)
		m_fCurSpeed = *pSpeed;
}
//------------------------------------------------------------------------
// 获取当前方向下的移动速度
void CCreatureView::GetCurSpeed(float& x, float& y)
{
	x = m_fCurSpeed.x;
	y = m_fCurSpeed.y;
}
//------------------------------------------------------------------------
// 正常移动
void CCreatureView::OnMoveStep(DWORD dwPeriod)
{
	if (m_bNeedWait)
	{
		POINT pt;
		pt.x = m_pPathList[m_nCurNodeIndex+1].x;
		pt.y = m_pPathList[m_nCurNodeIndex+1].y;
		CTile* pTile = g_pMapView->GetTile((const POINT&)pt);
		if (pTile->IsBlock())
		{
			m_nWaitTime += dwPeriod;
			if(m_nWaitTime >= 500)
				StopMove();

			return;
		}
		else
		{
			m_bNeedWait = FALSE;
			m_nWaitTime = 0;
		}
	}

	POINT ptTileOffsetOld = m_ptTileOffset;
	float x, y;
	GetMoveStep(GetDirection(), x, y);

	DWORD dwLast = m_dwCurTick;

	m_dwCurTick = dwLast + dwPeriod;
	DWORD dwTickDelta = m_dwCurTick - m_dwStartTick;
	POINT pt;
	Tile_To_World(m_ptTile, pt);
	pt.x += m_ptTileOffset.x - m_ptStartPos.x;
	pt.y += m_ptTileOffset.y - m_ptStartPos.y;
	GetCurSpeed(x, y);
	x *= dwTickDelta;
	y *= dwTickDelta;

	x -= pt.x;
	y -= pt.y;

	m_ptTileOffset.x += (int)x;
	m_ptTileOffset.y += (int)y;
	(this->*m_MoveStepByDir[GetDirection()])(ptTileOffsetOld);

}
//------------------------------------------------------------------------
// 没停在Tile中心点的用这种移动
void CCreatureView::OnMoveStepEx(DWORD dwPeriod)
{
	if (m_nCurPointNodeIndex == -1) // 到了终点
	{
		TraceLn("small path is over: %d/%d",m_nCurPointNodeIndex, m_nPointNodeCount);
		StopMove();
		ResetMoveInfo();
		return;
	}

	m_ptTileOffset.x = m_pPointList[m_nCurPointNodeIndex].ptOff.x;
	m_ptTileOffset.y = m_pPointList[m_nCurPointNodeIndex].ptOff.y;

	if (m_ptTile.x != m_pPointList[m_nCurPointNodeIndex].ptTile.x ||
		m_ptTile.y != m_pPointList[m_nCurPointNodeIndex].ptTile.y)
	{
		if (g_pMapView->MoveItem(m_ptTile, m_pPointList[m_nCurPointNodeIndex].ptTile, this))
		{
			//SetTile(m_pPointList[m_nCurPointNodeIndex].ptTile, g_pMapView);
		}
		else
		{
			return;
		}
	}
	if (++m_nCurPointNodeIndex >= m_nPointNodeCount)
	{
		m_nPointNodeCount = 0;
		m_nCurPointNodeIndex = -1;
		m_bNormalMove = TRUE;
		m_nCurNodeIndex ++;
		OnChangeDir();
		ResetMoveInfo();
	}
}
//------------------------------------------------------------------------
// 根据当前的移动方式移动
void CCreatureView::MoveStep(DWORD dwPeriod)
{
	if(m_bNormalMove)
		OnMoveStep(dwPeriod);
	else
		OnMoveStepEx(dwPeriod);
}
//------------------------------------------------------------------------
// 是否在移动中
BOOL CCreatureView::IsMoving()
{
	return (GetState()==CrtStateWalk || GetState() == CrtStateRun || GetState() == CrtStateRideRun);
}
//------------------------------------------------------------------------
// 停止移动
void CCreatureView::StopMove()
{
	m_nCurNodeIndex = -1;
	m_bNeedWait = FALSE;
	m_nWaitTime = 0;
	m_nPathNodeCount = 0;
	m_nPointNodeCount = 0;
	m_nCurPointNodeIndex = -1;
	m_bNormalMove = TRUE;
	m_dwStartTick = m_dwCurTick = 0;
	Tile_To_World(m_ptTile, m_ptStartPos);
	m_ptStartPos.x += m_ptTileOffset.x;
	m_ptStartPos.y += m_ptTileOffset.y;

	SetState(CrtStateStand);
}
//------------------------------------------------------------------------
// 当方向改变时调用度
void CCreatureView::OnChangeDir()
{
	if (m_nCurNodeIndex >= m_nPathNodeCount-1) // 到了终点
	{
		m_ptTileOffset.x = m_ptTileOffset.y = 0;
		m_nLastMoveDir = -1;
		m_bNeedWait = FALSE;
		StopMove();
	}
	else
	{
		POINT pt;
		pt.x = m_pPathList[m_nCurNodeIndex+1].x;
		pt.y = m_pPathList[m_nCurNodeIndex+1].y;
		CTile* pTile = g_pMapView->GetTile((const POINT&)pt);
		if (pTile->IsBlock())
		{
			m_bNeedWait = TRUE;
		}
		int nNewDir = Fun_CalcDirection8_tile(&m_pPathList[m_nCurNodeIndex], &m_pPathList[m_nCurNodeIndex+1]);
		if (nNewDir != m_nLastMoveDir)//GetDirection()) // 如果方向确实改变
		{
			m_ptTileOffset.x = m_ptTileOffset.y = 0;
			m_nLastMoveDir = nNewDir;
			SetDirection(nNewDir);
			ResetMoveInfo();
		}
		else
			SetDirection(nNewDir);
#ifndef _DEBUG
		if (m_pCrtSink)
		{
			if (this->GetItemData() == (DWORD)(g_pPlayer))
				m_pCrtSink->OnCreatureMoveToTileCenter((const POINTS&)m_pPathList[m_nCurNodeIndex], 
				(const POINTS&)m_pPathList[m_nCurNodeIndex+1]);
		}
#endif
	}
}
//------------------------------------------------------------------------
// 当Tile改变时调用
BOOL CCreatureView::OnChangeTile()
{
	if (!m_pPathList) return TRUE;
	static int ppp=0;
	
	if (m_nCurNodeIndex >= m_nPathNodeCount-1) // 到了终点
	{
		TraceColorLn(MY_TRACE_COLOR, "CCreatureView::OnChangeTile, error");
		StopMove();
		m_bNeedWait = FALSE;
		return TRUE;
	}
	POINT ptNewTile;
	ptNewTile.x = m_pPathList[m_nCurNodeIndex+1].x;
	ptNewTile.y = m_pPathList[m_nCurNodeIndex+1].y;
	
	POINT pt;
	pt.x = m_pPathList[m_nCurNodeIndex+1].x;
	pt.y = m_pPathList[m_nCurNodeIndex+1].y;
	CTile* pTile = g_pMapView->GetTile((const POINT&)pt);
	if (pTile->IsBlock())
	{
		m_bNeedWait = TRUE;
		return FALSE;
	}	

	BOOL bRtn = g_pMapView->MoveItem(m_ptTile, ptNewTile, this);
	if (bRtn)
	{
		m_nCurNodeIndex ++;
	}
	return TRUE;
}
//------------------------------------------------------------------------
// 校正生物的方向
void CCreatureView::AdjustDirection(SSkillCreateParam* pParam)
{
	POINT ptTileCenter,ptAttackCenter;
	Tile_To_Screen(pParam->ptTile, ptTileCenter);
	Tile_To_Screen(pParam->ptAttack, ptAttackCenter);

	if (ptTileCenter.x == ptAttackCenter.x && ptTileCenter.y == ptAttackCenter.y)
		return;

	int nDir = Fun_CalcDirection8(&ptTileCenter, &ptAttackCenter);
	SetDirection(nDir);
}
//------------------------------------------------------------------------
// 重设移动数据
void CCreatureView::ResetMoveInfo()
{
	m_dwStartTick = m_dwCurTick = 0;//GetTickCount();
	Tile_To_World(m_ptTile, m_ptStartPos);
	m_ptStartPos.x += m_ptTileOffset.x;
	m_ptStartPos.y += m_ptTileOffset.y;
	SetCurSpeed(GetSpeedMove(GetDirection()));
}
//------------------------------------------------------------------------
// 设置生物是否占位
void CCreatureView::SetOccupant(BOOL bOccupant)
{
	m_bOccupant = bOccupant;
}
//------------------------------------------------------------------------
// 获取生物是否占位
BOOL CCreatureView::GetOccupant()
{
	return m_bOccupant;
}
//------------------------------------------------------------------------
// 回调(跟随对象销毁时回调)
void CCreatureView::StopAttachObj(IAttachObj* pAttachObj)
{
	vector<IAttachObj*>::iterator it;
	for (it = m_vAttachObjs.begin(); it != m_vAttachObjs.end(); it++)
	{
		if ((*it) == pAttachObj) // 存在
		{
			pAttachObj->SetState(stateRelease);
			return;
		}
	}
}
//------------------------------------------------------------------------
// 创建吸附对象
CAttachStateObj* CCreatureView::CreateAttachObj(SAttachStateObjBuffer* pBuf)
{
	// 创建
	CAttachStateObj* pAttachObj = new CAttachStateObj(this);
	if (pAttachObj)
	{
		if (pAttachObj->Create((const char*)pBuf, sizeof(SAttachStateObjBuffer)))
		{
			m_vAttachObjs.push_back(pAttachObj);
			return pAttachObj;
		}
		else
		{
			delete pAttachObj;
		}
	}
	return NULL;
}
//------------------------------------------------------------------------
// 删掉所有飘血对象
void CCreatureView::RemoveAllFlowObj()
{
	vector<IAttachObj*>::iterator it;
	for (it = m_vFlowObjs.begin(); it != m_vFlowObjs.end(); it++)
	{
		(*it)->Release();
	}
	m_vFlowObjs.clear();
}
//------------------------------------------------------------------------
// 删掉预所有施法吸附对象
void CCreatureView::StopAllAttackAttach()
{
	vector<IAttachObj*>::iterator it;
	for (it = m_vAttackAttachObjs.begin(); it != m_vAttackAttachObjs.end(); it++)
	{
		StopAttachObj(*it);
	}

	m_vAttackAttachObjs.clear();
}
//------------------------------------------------------------------------
// 改变调色板类型
int CCreatureView::ChangePal(DWORD dwPalType)
{
	m_nCurPalList[dwPalType]++;

	if(m_nCurPalList[PalType_Stone] > 0)
		m_nCurPalType = PalType_Stone;
	else if(m_nCurPalList[PalType_Frozen] > 0)
		m_nCurPalType = PalType_Frozen;
	else if(m_nCurPalList[PalType_Toxic] > 0)
		m_nCurPalType = PalType_Toxic;
	else if(m_nCurPalList[PalType_Berserk] > 0)
		m_nCurPalType = PalType_Berserk;
	else
		m_nCurPalType = -1;

	return TRUE;
}
//------------------------------------------------------------------------
// 取消改变调色板类型
int CCreatureView::UnChangePal(DWORD dwPalType)
{
	m_nCurPalList[dwPalType]--;

	if(m_nCurPalList[PalType_Stone] > 0)
		m_nCurPalType = PalType_Stone;
	else if(m_nCurPalList[PalType_Frozen] > 0)
		m_nCurPalType = PalType_Frozen;
	else if(m_nCurPalList[PalType_Toxic] > 0)
		m_nCurPalType = PalType_Toxic;
	else if(m_nCurPalList[PalType_Berserk] > 0)
		m_nCurPalType = PalType_Berserk;
	else
		m_nCurPalType = -1;
	
	return TRUE;
}
//------------------------------------------------------------------------
// 根据外观ID创建吸附光效
int CCreatureView::CreateAppearance(DWORD dwAppearanceID)
{
	SSkillCreateParam scp;
	scp.nAniType = anitypeAppearanceAni;
	scp.nIndex = dwAppearanceID;
	scp.ptTile = m_ptTile;
	scp.ptAttack = m_ptTile;

	IResObject* pScpSkill = (IResObject*)CSkillCreate::getSkillScpObj(scp.nAniType, scp.nIndex);
	if (!pScpSkill)
		return FALSE;
	
	// 粒子效果
	int nLen = pScpSkill->GetArrayLong("Particle");
	if(nLen > 1)
	{
		DWORD dwParticle[17];
		memset(dwParticle, CrtPartMax, sizeof(dwParticle));
		pScpSkill->GetArrayLong("Particle", (long*)dwParticle, nLen);
		for(int i = 1; i < 17; i++)
		{
			if(dwParticle[i] >= 0 && dwParticle[i] < CrtPartMax)
				AddParticle(dwParticle[0], dwParticle[i]);
			else
				break;
		}
	}

	// 变身效果
	nLen = pScpSkill->GetArrayLong("ChangeShap");
	if(nLen > 0 && nLen < 3)
	{
		DWORD dwShapChange[] = {0,0};
		pScpSkill->GetArrayLong("ChangeShap", (long*)dwShapChange, nLen);
		ChangeShap(dwShapChange[0], dwShapChange[1]);
	}

	long lTemp;

	// 隐身透明效果
	pScpSkill->GetPropertyLong("Alpha", (long&)lTemp, -1);
	if(lTemp >= 0 && lTemp < 255)
	{
		if(lTemp == 0)
		{
			SetVisible(FALSE);
		}
		else
		{
			m_byteAlphaAll = lTemp;
		}
	}

	// 拖影效果
	pScpSkill->GetPropertyLong("bNeedShadow", (long&)lTemp, 0);
	if(lTemp)
	{
		if(m_pMagicShadow == NULL)
		{
			CSkillCreate::CreateShadow(this);
		}
	}

	// 做被击（晕）动作
	pScpSkill->GetPropertyLong("bNeedDoBeHit", (long&)lTemp, 0);
	if(lTemp)
	{
		SetState(CrtStateBehit);
	}

	// 改变调色板
	pScpSkill->GetPropertyLong("ChangePal", (long&)lTemp, 0);
	if(lTemp > 0 && lTemp < PalType_MaxType)
		ChangePal(lTemp);

	// 创建外观
	SAttachStateObjBuffer st;
	memset(&st, 0, sizeof(st));
	pScpSkill->GetPropertyLong("MagicID", (long&)st.dwResID, 0);
	if(st.dwResID <= 0)
		return FALSE;

	IResObject* pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(st.dwResID);
	if (!pMagicObj)
		return FALSE;

	g_ResLoader.ReadAttachMagic(pMagicObj, &st);
	st.dwAppearanceID = dwAppearanceID;
	
	pMagicObj->GetPropertyLong("LoopNum", (long &)st.m_nLoopNum, 1);
	pMagicObj->GetPropertyLong("NoShowBegin", (long &)st.m_nNoShowBegin, -1);
	pMagicObj->GetPropertyLong("NoShowEnd", (long &)st.m_nNoShowEnd, -1);
	
	if (!CreateAttachObj(&st))
		return FALSE;
	
	return TRUE;
}
//------------------------------------------------------------------------
// 根据外观ID销毁吸附光效
int CCreatureView::CancelAppearance(DWORD dwAppearanceID)
{
	SSkillCreateParam scp;
	scp.nAniType = anitypeAppearanceAni;
	scp.nIndex = dwAppearanceID;
	scp.ptTile = m_ptTile;
	scp.ptAttack = m_ptTile;

	IResObject* pScpSkill = (IResObject*)CSkillCreate::getSkillScpObj(scp.nAniType, scp.nIndex);
	if (!pScpSkill)
		return FALSE;

	// 粒子效果
	int nLen = pScpSkill->GetArrayLong("Particle");
	if(nLen > 1)
	{
		DWORD dwParticle[17];
		memset(dwParticle, 0, sizeof(dwParticle));
		pScpSkill->GetArrayLong("Particle", (long*)dwParticle, nLen);
		for(int i = 1; i < 17; i++)
		{
			if(dwParticle[i] > 0)
				UnAddParticle(dwParticle[0], dwParticle[i]);
			else
				break;
		}
	}

	// 变身效果
	nLen = pScpSkill->GetArrayLong("ChangeShap");
	if(nLen > 0 && nLen < 3)
	{
		DWORD dwShapChange[] = {0,0};
		pScpSkill->GetArrayLong("ChangeShap", (long*)dwShapChange, nLen);
		UnChangeShap(dwShapChange[0], dwShapChange[1]);
	}

	long lTemp;
	
	// 隐身透明效果
	pScpSkill->GetPropertyLong("Alpha", (long&)lTemp, -1);
	if(lTemp >= 0 && lTemp < 255)
	{
		m_byteAlphaAll = 0;
		SetVisible(TRUE);
	}
	
	// 拖影效果
	pScpSkill->GetPropertyLong("bNeedShadow", (long&)lTemp, 0);
	if(lTemp)
	{
		SAFE_DELETE(m_pMagicShadow);
	}

	// 停止被击（晕）动作
	pScpSkill->GetPropertyLong("bNeedDoBeHit", (long&)lTemp, 0);
	if(lTemp)
	{
		StopState(CrtStateBehit);
	}

	// 改变调色板
	pScpSkill->GetPropertyLong("ChangePal", (long&)lTemp, 0);
	if(lTemp > 0 && lTemp < PalType_MaxType)
		UnChangePal(lTemp);

	// 销毁外观
	vector<IAttachObj*>::iterator it;
	for (it=m_vAttachObjs.begin(); it!=m_vAttachObjs.end(); it++)
	{
		if ((*it)->GetAppearanceID() == dwAppearanceID
			&& IAttachObj::stateRelease != (*it)->GetState()) // 存在
		{
			(*it)->SetState(IAttachObj::stateRelease); // 设置成销毁状态
			
			return TRUE;
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------
// 寻找当前链接的法术管理器
CMagicMgrBase* CCreatureView::FindMagic(int nID)
{
	vector<CMagicMgrBase*>::iterator it;
	for(it = m_vMagicMgr.begin(); it != m_vMagicMgr.end(); it++)
	{
		if((*it)->GetCheckID() == nID)
			return (*it);
	}
	return NULL;
}
//------------------------------------------------------------------------
// 让技能开始攻击
void CCreatureView::MagicBeginAttack(int nCheckID)
{
	CMagicMgrBase* pMagicMgr = FindMagic(nCheckID);
	if(pMagicMgr != NULL)
		pMagicMgr->BeginAttack();
}
//------------------------------------------------------------------------
// 加入法术管理器链接
void CCreatureView::AddMagic(CMagicMgrBase *pMgr)
{
	m_vMagicMgr.push_back(pMgr);
}
//------------------------------------------------------------------------
// 删掉法术管理器链接
void CCreatureView::DelMagic(CMagicMgrBase *pMgr)
{
	vector<CMagicMgrBase*>::iterator it;
	for (it = m_vMagicMgr.begin(); it != m_vMagicMgr.end(); it++)
	{
		if ((*it) == pMgr) // 存在
		{
			m_vMagicMgr.erase(it);
			break;
		}
	}
}
//------------------------------------------------------------------------
// 停止所有魔法
void CCreatureView::StopMagic()
{
	vector<CMagicMgrBase*>::iterator it;
	while(!m_vMagicMgr.empty())
	{
		it = m_vMagicMgr.begin();
		m_vMagicMgr.erase(it);
		if((*it)->IsRunning())
			(*it)->Stop();
	}
}
//------------------------------------------------------------------------
// 加入伤害堆栈管理
void CCreatureView::AddDmgStack(SBehitParam *pParam)
{
	if(pParam->pSourceView != NULL)
	{
		CMagicMgrBase* pMgr = pParam->pSourceView->FindMagic(pParam->nCheckId);
		if(pMgr != NULL)
		{
			pMgr->AddDamage(this, pParam->FlowParam);
			return;
		}
	}

	FlowHP(pParam->FlowParam, pParam->pSourceView);
}
//------------------------------------------------------------------------
// 生物飘血
void CCreatureView::FlowHP(SFlowParam FlowParam, CCreatureView* pSourceView, long lMagicIDBeHit, int nBeHitSoundID)
{
	// 创建被击光效
	if(lMagicIDBeHit > 0)
	{
		SAttachStateObjBuffer st;
		memset(&st, 0, sizeof(st));
		st.dwResID = lMagicIDBeHit;

		IResObject* pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(st.dwResID);
		if (pMagicObj)
		{
			g_ResLoader.ReadAttachMagic(pMagicObj, &st);
			
			st.dwAppearanceID = -1;
			
			CreateAttachObj(&st);
		}
	}
	if(FlowParam.nFlowType != FlowTypeHedge)
		Fun_PlaySound(m_ptTile, nBeHitSoundID);

	FlowDamageNum(FlowParam, pSourceView);
}
//------------------------------------------------------------------------
// 生物飘血
void CCreatureView::FlowDamageNum(SFlowParam FlowParam, CCreatureView* pSourceView)
{
	CFlowText::SFlowTextBuffer buf;
	POINT ptCenter;

	Tile_To_World(m_ptTile, ptCenter);
	buf.nDelay = 30;
	buf.ptOffset.x = 0;
	buf.ptOffset.y = -2;
	buf.ptPos.x = ptCenter.x + m_ptTileOffset.x;
	buf.ptPos.y = ptCenter.y + m_ptTileOffset.y + GetHeightOffset() + 20;

	buf.nFlowType = FlowParam.nFlowType;
	if(FlowParam.nFlowType == FlowTypeCommon || FlowParam.nFlowType == FlowTypeCrazy)
	{
		if(FlowParam.nHP == 0)
			buf.nFlowType = FlowTypeResist;
	}

	buf.nNum = FlowParam.nHP;
	
	if(buf.nNum > 0)
	{
		buf.nNumType = enNUM_TYPE_GREEN;
	}
	else
	{
		if(this->GetItemData() == (DWORD)(g_pPlayer))
		{
			buf.nNumType = enNUM_TYPE_RED;
		}
		else if(pSourceView != NULL && pSourceView->GetItemData() == (DWORD)(g_pPlayer))
		{
			buf.nNumType = enNUM_TYPE_YELLOW;
		}
		else
		{
			buf.nNumType = enNUM_TYPE_ORANGE;
		}
	}

	CFlowText* pFlowText = new CFlowText(this);
	ASSERT(pFlowText);
	if (!pFlowText->Create((const char*)&buf, sizeof(buf)))
		return;
	
	m_vFlowObjs.push_back(pFlowText);
}
//------------------------------------------------------------------------
// 获取上次飘血的时间
DWORD CCreatureView::GetLastFlowTime()
{
	return m_dwLastFlowTime;
}
//------------------------------------------------------------------------
// 设置上次飘血的时间
void CCreatureView::SetLastFlowTime(DWORD dwTime)
{
	m_dwLastFlowTime = dwTime;
}
//------------------------------------------------------------------------
// 获取武器类型
int CCreatureView::GetWeaponStyle()
{
	return ActWeaponStyleSingle;
}
//------------------------------------------------------------------------
// 加入指针链接
void CCreatureView::AddLinkView(CCreatureView **ppView)
{
	m_vppLinkView.push_back(ppView);
}
//------------------------------------------------------------------------
// 删掉指针链接
void CCreatureView::DelLinkView(CCreatureView **ppView)
{
	vector<CCreatureView**>::iterator it;
	for (it = m_vppLinkView.begin(); it != m_vppLinkView.end(); it++)
	{
		if ((*it) == ppView) // 存在
		{
			*ppView = NULL;
			m_vppLinkView.erase(it);
			break;
		}
	}
}
//------------------------------------------------------------------------
// 变身
void CCreatureView::ChangeShap(DWORD dwShapID, int nLv)
{
	if(dwShapID <= 0 || nLv < 0 || nLv >= MAX_CHANGE_SHAP_LV)
		return;

	if(m_dwShapID[nLv] > 0)
		return;
	
	m_dwShapID[nLv] = dwShapID;
	
	if(m_nShapLv < nLv)
	{
		if(m_nShapLv < 0)
		{
			m_nBakCommonAttackSoundID = m_nCommonAttackSoundID;
			m_nBakCommonAttackBeHitSoundID = m_nCommonAttackBeHitSoundID;
			memcpy(m_dwBakModuleID, m_dwModuleID, sizeof(m_dwModuleID));
			memcpy(m_byteBakPal, m_bytePal, sizeof(m_bytePal));
			memcpy(m_byteBakAlpha, m_byteAlpha, sizeof(m_byteAlpha));
		}
		m_nShapLv = nLv;
		DoChangeShap();
	}
}
//------------------------------------------------------------------------
// 取消变身
void CCreatureView::UnChangeShap(DWORD dwShapID, int nLv)
{
	if(dwShapID <= 0 || nLv < 0 || nLv >= MAX_CHANGE_SHAP_LV)
		return;

	if(m_dwShapID[nLv] != dwShapID)
		return;

	m_dwShapID[nLv] = 0;
	
	if(m_nShapLv == nLv)
	{
		for(int i = m_nShapLv; i >= 0; i--)
		{
			if(m_dwShapID[i] > 0)
			{
				m_nShapLv = i;
				DoChangeShap();
				return;
			}
		}
		m_nShapLv = -1;
		DoChangeShap();
	}
}
//------------------------------------------------------------------------
// 开始变身
void CCreatureView::DoChangeShap()
{
	for (int i = 0; i < CrtPartMax; i++)
	{
		if (m_pModule[i])
		{
			DWORD dwID = m_dwModuleID[i];
			if (dwID != 0)
				g_ResLoader.ReleaseRes(ioCreature + dwID);
			m_pModule[i] = NULL;
		}
	}
	memset(m_dwModuleID, 0, sizeof(m_dwModuleID));
//删除所有指针
#ifndef _USE_BINSCRIPT		//这个是为了满足Debug版本的需求。两个版本的脚本读取不一样
	for (int j = 0; j < CrtPartMax; j++)
	{
		SAFE_DELETE_ARRAY(m_PartStateFlag[j]);
	}
#endif

	if(m_nShapLv < 0)
	{
		m_nCommonAttackSoundID = m_nBakCommonAttackSoundID;
		m_nCommonAttackBeHitSoundID = m_nBakCommonAttackBeHitSoundID;

		memcpy(m_dwModuleID, m_dwBakModuleID, sizeof(m_dwBakModuleID));
		memcpy(m_bytePal, m_byteBakPal, sizeof(m_byteBakPal));
		memcpy(m_byteAlpha, m_byteBakAlpha, sizeof(m_byteBakAlpha));
		m_AniContext.nType = m_dwType;

		OnUpdate();
	}
	else
	{
		if(!ChangeShapEX())
		{
			UnChangeShap(m_dwShapID[m_nShapLv], m_nShapLv);
		}
	}
}
//------------------------------------------------------------------------
// 当前是否变身了
BOOL CCreatureView::IsChangeShap()
{
	return m_nShapLv >= 0;
}
//------------------------------------------------------------------------
// 绘制生物的名字
void CCreatureView::DrawName(ICanvas* pCanvas, const POINT& ptBottomCenter)
{
	// draw user name
	if (m_bShowUserName)
	{
		int nWidth = 0;
		int nClanWidth = 0;
		int nCityWidth = 0;

		nWidth = g_pFontWithOutline->GetLength(m_szUserName);
		
		if(m_bShowCityName)
		{
			nCityWidth = g_pFontWithOutline->GetLength(m_szCityName);
			nWidth += nCityWidth;
		}
		if(m_bShowClanName)
		{
			nClanWidth = g_pFontWithOutline->GetLength(m_szClanName);
			nWidth += nClanWidth;
		}

		int x = ptBottomCenter.x - nWidth / 2;
		int y = ptBottomCenter.y - g_pFontWithOutline->GetHeight() - 1;

		if(m_bShowCityName)
		{
			pCanvas->DrawText(g_pFontWithOutline, x, y, m_szCityName, WPixel(255,128,64), FONT_NOALPHA);
			x += nCityWidth;
		}
		if(m_bShowClanName)
		{
			pCanvas->DrawText(g_pFontWithOutline, x, y, m_szClanName, WPixel(255, 215, 0), FONT_NOALPHA);
			x += nClanWidth;
		}
		pCanvas->DrawText(g_pFontWithOutline, x, y, m_szUserName, m_clrName, FONT_NOALPHA);
	}
}
//------------------------------------------------------------------------
// 绘制生物的名字
void CCreatureView::DrawSelectedName(ICanvas* pCanvas, const POINT& ptBottomCenter)
{/*
	// draw user name
	if (m_bShowUserName)
	{
		const char* c = "◆";
		int nSelCharWidth = (int)g_pFontWithOutline->GetLength(c);
		int nWidth = (int)g_pFontWithOutline->GetLength(m_szUserName);
		int x = ptBottomCenter.x - (nWidth + 2*nSelCharWidth) / 2;
		int y = ptBottomCenter.y - g_pFontWithOutline->GetHeight() - 1;
		pCanvas->DrawText(g_pFontWithOutline, x, y, c, WPixel(0,255,0), FONT_NOALPHA);
		pCanvas->DrawText(g_pFontWithOutline, x+nSelCharWidth, y, m_szUserName, m_clrName, FONT_NOALPHA);
		pCanvas->DrawText(g_pFontWithOutline, x+nSelCharWidth+nWidth, y, c, WPixel(0,255,0), FONT_NOALPHA);
	}*/
}
//------------------------------------------------------------------------
// 画血槽
void CCreatureView::DrawBloodSlot(ICanvas* pCanvas, RECT& rc, int nPercent)
{
	const WPixel clr = WPixel(0,0,0);
	const WPixel clrpixel = WPixel(50,50,50);
	WPixel bloodclr1, bloodclr2, bloodclr3;
	if (nPercent < 0) nPercent = 0;
	if (nPercent <= 20)
	{
		bloodclr1 = WPixel(247,0,0);
		bloodclr2 = WPixel(150,0,0);
		bloodclr3 = WPixel(105,0,0);
	}
	else if (nPercent <= 60)
	{
		bloodclr1 = WPixel(247,247,0);
		bloodclr2 = WPixel(150,150,0);
		bloodclr3 = WPixel(105,105,0);
	}
	else
	{
		bloodclr1 = WPixel(0,247,0);
		bloodclr2 = WPixel(0,150,0);
		bloodclr3 = WPixel(0,105,0);
	}
	// draw rectangle
	POINT ptFrom,ptTo;
	ptFrom.x	= rc.left + 1;
	ptFrom.y	= rc.top;
	ptTo.x		= rc.right;
	ptTo.y		= rc.top;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 上横
	ptFrom.x	= rc.right;
	ptFrom.y	= rc.top + 1;
	ptTo.x		= rc.right;
	ptTo.y		= rc.bottom;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 右竖
	ptFrom.x	= rc.left + 1;
	ptFrom.y	= rc.bottom;
	ptTo.x		= rc.right;
	ptTo.y		= rc.bottom;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 下横
	ptFrom.x	= rc.left;
	ptFrom.y	= rc.top + 1;
	ptTo.x		= rc.left;
	ptTo.y		= rc.bottom;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 左竖
	pCanvas->DrawPixel(rc.left, rc.top, clrpixel);
	pCanvas->DrawPixel(rc.right, rc.top, clrpixel);
	pCanvas->DrawPixel(rc.left, rc.bottom, clrpixel);
	pCanvas->DrawPixel(rc.right, rc.bottom, clrpixel);
	
	
	// draw solid rectangle
	if (nPercent > 0)
	{
		InflateRect(&rc, -1, -1);
		float fRate = (100.0f/BLOOD_SLOT_WIDTH);
		rc.right = rc.left + (LONG)(nPercent/fRate);
		
		ptFrom.x=rc.left, ptFrom.y=rc.top, ptTo.x=rc.right, ptTo.y=rc.top;
		pCanvas->DrawLine(ptFrom, ptTo, bloodclr1);
		
		ptFrom.x=rc.left, ptFrom.y=rc.top + 1, ptTo.x=rc.right, ptTo.y=rc.top + 1;
		pCanvas->DrawLine(ptFrom, ptTo, bloodclr2);
		
		ptFrom.x=rc.left, ptFrom.y=rc.top + 2, ptTo.x=rc.right, ptTo.y=rc.top + 2;
		pCanvas->DrawLine(ptFrom, ptTo, bloodclr3);
		//pCanvas->DrawBox(rc, bloodclr);
	}
}
//------------------------------------------------------------------------
// 停止攻击声音
void CCreatureView::StopAttackSound()
{
	Fun_StopSound(m_nAttackSoundChunnelID);
}
//------------------------------------------------------------------------
// 停止攻击声音
BOOL CCreatureView::IsDeaded()
{
	return m_bDeaded;
}
//------------------------------------------------------------------------
// 添加粒子效果
void CCreatureView::AddParticle(DWORD dwPtcID, int nPart)
{
	if(nPart < 0 || nPart >= CrtPartMax)
		return;

	if(m_pParticle[nPart] != NULL)
		return;
	
	m_pParticle[nPart] = CParticle::Create(dwPtcID);
}
//------------------------------------------------------------------------
// 取消粒子效果
void CCreatureView::UnAddParticle(DWORD dwPtcID, int nPart)
{
	if(nPart < 0 || nPart >= CrtPartMax)
		return;

	if(m_pParticle[nPart] && m_pParticle[nPart]->m_dwResID == dwPtcID)
		SAFE_RELEASE(m_pParticle[nPart]);
}
//------------------------------------------------------------------------