/**
*	created:		2012-7-2   2:30
*	filename: 		CreatureViewCommond
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/CreatureView.h"
#include "../Include/ResLoader.h"
#include "../Include/PalMgr.h"
#include "../Include/SkillCreate.h"
#include "../../FKGame/MapInfo/Tile.h"
//------------------------------------------------------------------------
extern CMagic* g_pSkillSelMagic;
//------------------------------------------------------------------------
// 所有支持的命令
CCreatureView::ProcCommand CCreatureView::m_Command[CrtCmdMaxCommandCount] =
{
	NULL,
	&CCreatureView::cmdSetProperty,
	&CCreatureView::cmdGetProperty,
	&CCreatureView::cmdMove,
	&CCreatureView::cmdBeHit,
	&CCreatureView::cmdDie,
	&CCreatureView::cmdFallow,
	&CCreatureView::cmdChangeModule,
	&CCreatureView::cmdSpecialModule,
	&CCreatureView::cmdBorn,
	&CCreatureView::cmdUpgrade,
	&CCreatureView::cmdStand,
	&CCreatureView::cmdAppearance,
	&CCreatureView::cmdBloodInfo,
	&CCreatureView::cmdChatInfo,
	&CCreatureView::cmdMouseAni,
	&CCreatureView::cmdModulePalType,
	&CCreatureView::cmdModuleAlpha,
	&CCreatureView::cmdSetting,
	&CCreatureView::cmdAddTitle,
	&CCreatureView::cmdClearTitle,

	&CCreatureView::cmdPreAttack,
	&CCreatureView::cmdStopPreAttack,
	&CCreatureView::cmdAttack,
	&CCreatureView::cmdStopAttack,
	&CCreatureView::cmdChangeLocForSkill,
	&CCreatureView::cmdSkillSelAni,

	&CCreatureView::cmdSetNpcType,
	&CCreatureView::cmdSetTaskType,
	&CCreatureView::cmdSetClanName,
	&CCreatureView::cmdSetCityName,
};
//------------------------------------------------------------------------
// 角色的属性设置
// dwParam1: 属性类型
// dwParam2: 设置值(参看角色的属性定义)
int CCreatureView::cmdSetProperty(DWORD dwParam1, DWORD dwParam2)
{
	if (dwParam1 >= CrtPropMaxPropCount)
	{
		TraceColorLn(MY_TRACE_COLOR, "CCreatureView::cmdSetProperty(), Unknow Property ID = %d", dwParam1);
		return FALSE;
	}
	if (m_Property[dwParam1] == NULL)
		return FALSE;
	
	(this->*m_Property[dwParam1])(TRUE, dwParam2);
	return TRUE;
}
//------------------------------------------------------------------------
// 获取角色的属性
// dwParam1: 属性类型
// dwParam2: 返回值(参看角色的属性定义)
int CCreatureView::cmdGetProperty(DWORD dwParam1, DWORD dwParam2)
{
	if (dwParam1 >= CrtPropMaxPropCount)
	{
		TraceColorLn(MY_TRACE_COLOR, "CCreatureView::cmdSetProperty(), Unknow Property ID = %d", dwParam1);
		return FALSE;
	}
	if (m_Property[dwParam1] == NULL)
		return FALSE;
	
	(this->*m_Property[dwParam1])(FALSE, dwParam2);
	return TRUE;
}
//------------------------------------------------------------------------
// 角色沿着指定的路径移动
// dwParam1: 路径列表指针(POINTS*);
// dwParam2: 路径列表节点数目。
int CCreatureView::cmdMove(DWORD dwParam1, DWORD dwParam2)
{
	m_bNeedWait = FALSE;
	m_nWaitTime = 0;
	POINTS* pList = (POINTS*)dwParam1;


	int dwNodeCount=dwParam2;

	if (dwNodeCount < 2)
		return FALSE;
	
	// 路径列表是否空间足够
	if (dwNodeCount > m_nMaxPathNodeCount)
	{
		m_nMaxPathNodeCount = dwNodeCount;
		if (m_pPathList) delete[] m_pPathList;
		m_pPathList = new POINTS[dwNodeCount];
	}

	m_nPathNodeCount = dwNodeCount;
	m_nCurNodeIndex = 0;

	memcpy(m_pPathList, (POINTS*)dwParam1, dwNodeCount*sizeof(POINTS));

	POINT ptNewTile = {m_pPathList[0].x, m_pPathList[0].y};
	if (m_ptTile.x != ptNewTile.x || m_ptTile.y != ptNewTile.y)
	{
		if (g_pMapView->MoveItem(m_ptTile, ptNewTile, this))
		{
			TraceColorLn(MY_TRACE_COLOR, "拉人。。。");
			m_ptTile.x = m_pPathList[0].x;
			m_ptTile.y = m_pPathList[0].y;
			m_ptTileOffset.x = m_ptTileOffset.y = 0;
		}
		else
		{
		}
	}

	if (m_ptTileOffset.x != 0 || m_ptTileOffset.y != 0) // 没有正常停下来，没有站在TILE中间
	{
		POINT ptFrom, ptTo;
		POINT ptTileNext, ptTileCenter;
		ptTileNext.x = m_pPathList[m_nCurNodeIndex+1].x;
		ptTileNext.y = m_pPathList[m_nCurNodeIndex+1].y;
		int nTileDir = Fun_CalcDirection8_tile(&m_ptTile, &ptTileNext);
		{
			Tile_To_World(m_ptTile, ptTileCenter);
			ptFrom.x = ptTileCenter.x + m_ptTileOffset.x;
			ptFrom.y = ptTileCenter.y + m_ptTileOffset.y;
			Tile_To_World(ptTileNext, ptTo);
			
			int nDir = Fun_CalcDirection8_45(&ptFrom, &ptTo);
			
			float nStepX, nStepY;
			GetMoveStepEx(nDir, nStepX, nStepY);
			int dx = ptTo.x - ptFrom.x;
			int dy = ptTo.y - ptFrom.y;
			int nMaxCountX=0, nMaxCountY=0;
			int nCount = 0;
			float fStepX=0.0f, fStepY=0.0f;
			if (dx == 0)
			{
				nCount = dy / nStepY;
				fStepY = (float)nStepY;
			}
			else if (dy == 0)
			{
				nCount = dx / nStepX;
				fStepX = (float)nStepX;
			}
			else
			{
				if (nStepX != 0)
					nMaxCountX = dx / nStepX;
				if (nStepY != 0)
					nMaxCountY = dy / nStepY;
				if (nMaxCountX > nMaxCountY)
				{
					nCount = nMaxCountX;
					fStepX = nStepX;
					fStepY = fStepX * ((float)dy) / ((float)dx);
				}
				else
				{
					nCount = nMaxCountY;
					fStepY = nStepY;
					fStepX = fStepY * ((float)dx) / ((float)dy);
				}
			}

			if (nCount == 0) // 距离tile的中心点太近
			{
				if (m_nMaxPointNodeCount <= 0)
				{
					m_nMaxPointNodeCount = 1;
					if (m_pPointList) delete[] m_pPointList;
					m_pPointList = new SDblPoint[1];
				}
				m_pPointList[0].ptTile = m_ptTile;
				m_pPointList[0].ptOff.x = 0;
				m_pPointList[0].ptOff.y = 0;
				
				m_nCurPointNodeIndex = 0;
				m_nPointNodeCount = 0;	
			}
			else
			{
				if (nCount > m_nMaxPointNodeCount)
				{
					m_nMaxPointNodeCount = nCount;
					if (m_pPointList) delete[] m_pPointList;
					m_pPointList = new SDblPoint[nCount];
				}
				
				POINT ptOldTile = m_ptTile;
				for (int i=0; i<nCount-1; i++)
				{
					POINT pt,ptTile,ptCenter;
					pt.x = ptFrom.x + fStepX*(i+1);
					pt.y = ptFrom.y + fStepY*(i+1);
					World_To_Tile(pt, ptTile);
					if (g_pMapView->GetTile(ptTile)->IsBlock()) // 如果占位，用先前的那个TILE
					{
						ptTile = ptOldTile;
					}
					ptOldTile = ptTile;
					Tile_To_World(ptTile, ptCenter);
					pt.x -= ptCenter.x;
					pt.y -= ptCenter.y;
					m_pPointList[i].ptTile = ptTile;
					m_pPointList[i].ptOff = pt;
				}
				m_pPointList[nCount-1].ptTile = ptTileNext;
				m_pPointList[nCount-1].ptOff.x = 0;
				m_pPointList[nCount-1].ptOff.y = 0;

				m_nCurPointNodeIndex = 0;
				m_nPointNodeCount = nCount;				
			}

			m_bNormalMove = FALSE;		
			SetDirection(nDir);
			m_nLastMoveDir = nDir;
		}
	}
	else
	{
		OnChangeDir();
	}

	ResetMoveInfo();
	
	SetState(GetMoveStateByStyle());
	
	return TRUE;
}
//------------------------------------------------------------------------
// 角色站立
// dwParam1: ?
// dwParam2: ?
int CCreatureView::cmdStand(DWORD dwParam1, DWORD dwParam2)
{
	
	// if is moving, stop move
	if (IsMoving()) StopMove();

	POINT ptNewTile = *(POINT*)dwParam1;
	if (m_ptTile.x != ptNewTile.x || m_ptTile.y != ptNewTile.y)
	{
		TraceLn("cmdStand: 先前(%d,%d),传入(%d,%d)", m_ptTile.x, m_ptTile.y, ptNewTile.x, ptNewTile.y);
		if (g_pMapView->MoveItem(m_ptTile, ptNewTile, this))
			SetTile(ptNewTile, g_pMapView);

		if(GetState() >= CrtStateJuqi1 && GetState() <= CrtStateArrowAttack)
			return TRUE;
	}
	SetState(CrtStateStand);
 
	return TRUE;
}
//------------------------------------------------------------------------
// 生物坐下
// dwParam1: BOOL(标识坐下还是不坐下)
// dwParam2: ?
int	CCreatureView::cmdSetting(DWORD dwParam1, DWORD dwParam2)
{
	if (IsMoving())
		StopMove();
	
	if(dwParam1 == 1)
	{
		m_ptTileOffset.x = 0;
		m_ptTileOffset.y = 0;
		SetState(CrtStateSit);
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
// 生物的外观(主要是一些跟随物)
// dwParam1: 是设置还是取消(TRUR or FALSE)
// dwParam2: 外观ID (DWORD)
int CCreatureView::cmdAppearance(DWORD dwParam1, DWORD dwParam2)
{
	if ((BOOL)dwParam1)
		return CreateAppearance(dwParam2);
	return CancelAppearance(dwParam2);
}
//------------------------------------------------------------------------
// 生物改变血量(如升级或穿上增加血量的装备等)
// dwParam1: 生物当前还有多少血(int*)
// dwParam2: 生物当前总共能容纳多少血(int*)
int CCreatureView::cmdBloodInfo(DWORD dwParam1, DWORD dwParam2)
{
	ASSERT(dwParam1 && dwParam2);
	int nCurBlood = *(int*)dwParam1;
	int nTotalBlood = *(int*)dwParam2;
	if (nTotalBlood <= 0)
		nTotalBlood = 1;
	m_nBloodVol = nCurBlood*100/nTotalBlood;
	
	return TRUE;
}
//------------------------------------------------------------------------
// 角色头顶的聊天信息
// dwParam1: 聊天内容(LPSTR)
// dwParam2: 文字颜色(WPixel*)
int	CCreatureView::cmdChatInfo(DWORD dwParam1, DWORD dwParam2)
{
	typedef IPaoPao** MyType;

	*(MyType*)dwParam1 = (MyType)&m_pPaoPao;
	return TRUE;
}
//------------------------------------------------------------------------
// 蓄气(转到蓄气状态，蓄气光效启动)
// dwParam1: 魔法(技能)的创建参数(SSkillCreateParam*)
// dwParam2: ?
int	CCreatureView::cmdPreAttack(DWORD dwParam1, DWORD dwParam2)
{
	// 停下来
	if (IsMoving())
	{
		StopMove();
	}

	SetState(CrtStateStand);
	// 获取技能参数
	SSkillCreateParam* pParam = (SSkillCreateParam*)dwParam1;
	pParam->nAniType = anitypeSkillAni;
	pParam->ptTile = m_ptTile;
	if(pParam->pAttackView != NULL)
		pParam->ptAttack = pParam->pAttackView->GetTile();

	// 参数校验
	if (pParam->nIndex < 1 || pParam->nIndex > SKILL_MAX_COUNT)
	{
		TraceLn("ERROR: CCreatureView::cmdPreAttack(): invalid skill, ID = %d", pParam->nIndex);
		return FALSE;
	}

	// 读脚本
	IResObject* pScpSkill = (IResObject*)CSkillCreate::getSkillLevelScpObj(pParam->nAniType, pParam->nIndex, pParam->nLevel);
	if (!pScpSkill) 
	{
		return TRUE;
	}
	long lTemp;

	// 校正方向
	pScpSkill->GetPropertyLong("Base_NotAdjustDir", lTemp, 0);
	if(lTemp == 0)
	{
		POINT ptTileCenter,ptAttackCenter;
		Tile_To_Screen(m_ptTile, ptTileCenter);
		ptTileCenter.x += m_ptTileOffset.x;
		ptTileCenter.y += m_ptTileOffset.y;

		Tile_To_Screen(pParam->ptAttack, ptAttackCenter);
		if(pParam->pAttackView != NULL)
		{
			ptAttackCenter.x += pParam->pAttackView->m_ptTileOffset.x;
			ptAttackCenter.y += pParam->pAttackView->m_ptTileOffset.y;
		}

		if (ptTileCenter.x != ptAttackCenter.x || ptTileCenter.y != ptAttackCenter.y)
			SetDirection(Fun_CalcDirection8_45(&ptTileCenter, &ptAttackCenter));
	}

	// 做蓄气动作
	pScpSkill->GetPropertyLong("Base_ActIDPreAttackUseType", lTemp, 0);
	if(lTemp == 0)		// 如果creature脚本配了生物的蓄气动作则用那个，否则用技能脚本的
	{
		lTemp = GetJuqiActID();
		if(lTemp < 0)
		{
			pScpSkill->GetPropertyLong("Base_ActIDPreAttack", lTemp, 0);
		}
	}
	else				// 如果skill脚本配了蓄气动作则用这个，否则用creature脚本的
	{
		pScpSkill->GetPropertyLong("Base_ActIDPreAttack", lTemp, -1);
		if(lTemp <= 0)
		{
			lTemp = GetJuqiActID();
		}
	}

	SetState(lTemp);
	SetStatusPreAttack();

	// 蓄气阶段的声音
	pScpSkill->GetPropertyLong("Base_SoundIDPreAttack", lTemp, 0);
	if(lTemp > 0)
	{
		long lSoundLoop;
		pScpSkill->GetPropertyLong("Base_bSoundLoopPreAttack", lSoundLoop, 0);
		m_nAttackSoundChunnelID = Fun_PlaySound(m_ptTile, lTemp,lSoundLoop);
	}

	// 蓄气阶段的技能光效
	DWORD dwMagicID[] = {0,0,0};
	int nLen = pScpSkill->GetArrayLong("Base_MagicIDPreAttack");
	if (nLen != pScpSkill->GetArrayLong("Base_MagicIDPreAttack", (long*)dwMagicID, nLen))
		return FALSE;

	int nTime = 0;
	while(dwMagicID[nTime] > 0 && nTime <= 3)
	{
		SAttachStateObjBuffer st;
		memset(&st, 0, sizeof(st));
		st.dwResID = dwMagicID[nTime];

		IResObject* pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(st.dwResID);
		if (pMagicObj)
		{
			g_ResLoader.ReadAttachMagic(pMagicObj, &st);
			
			pMagicObj->GetPropertyLong("LoopNum", (long &)st.m_nLoopNum, 1);
			pMagicObj->GetPropertyLong("NoShowBegin", (long &)st.m_nNoShowBegin, -1);
			pMagicObj->GetPropertyLong("NoShowEnd", (long &)st.m_nNoShowEnd, -1);

			st.dwAppearanceID = -1;
			
			CAttachStateObj* pObj = CreateAttachObj(&st);
			if(pObj != NULL)
				m_vAttackAttachObjs.push_back(pObj);
		}
		nTime++;
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 攻击(转到攻击状态，包括瞬间施法、持续施法和普通攻击，施法光效启动，法术动画启动)
// dwParam1: 魔法(技能)的创建参数(SSkillCreateParam*)
// dwParam2: ?
int	CCreatureView::cmdAttack(DWORD dwParam1, DWORD dwParam2)
{
	if (IsMoving())
		StopMove();
	
	SetState(CrtStateStand);
	// 取消以前的技能魔法
	StopMagic();

	SSkillCreateParam* pParam = (SSkillCreateParam*)dwParam1;
	pParam->nAniType = anitypeSkillAni;
	pParam->ptTile = m_ptTile;
	if(pParam->pAttackView != NULL)
		pParam->ptAttack = pParam->pAttackView->GetTile();
	
	if (pParam->nIndex < 0 || pParam->nIndex > SKILL_MAX_COUNT || m_funcSkill[pParam->nIndex] == NULL)
	{
		TraceLn("ERROR: CCreatureView::cmdAttack(): invalid skill, ID = %d", pParam->nIndex);
		return FALSE;
	}

	// 普通攻击校正
	if(pParam->nIndex == 0)
		pParam->nLevel = 1;

	// 读脚本
	IResObject* pScpSkill = (IResObject*)CSkillCreate::getSkillLevelScpObj(pParam->nAniType, pParam->nIndex, pParam->nLevel);
	if (!pScpSkill) 
	{
		return TRUE;
	}
	long lTemp;

	// 校正方向
	pScpSkill->GetPropertyLong("Base_NotAdjustDir", lTemp, 0);
	if(lTemp == 0)
	{
		POINT ptTileCenter,ptAttackCenter;
		Tile_To_Screen(m_ptTile, ptTileCenter);
		ptTileCenter.x += m_ptTileOffset.x;
		ptTileCenter.y += m_ptTileOffset.y;

		Tile_To_Screen(pParam->ptAttack, ptAttackCenter);
		if(pParam->pAttackView != NULL)
		{
			ptAttackCenter.x += pParam->pAttackView->m_ptTileOffset.x;
			ptAttackCenter.y += pParam->pAttackView->m_ptTileOffset.y;
		}

		if (ptTileCenter.x != ptAttackCenter.x || ptTileCenter.y != ptAttackCenter.y)
			SetDirection(Fun_CalcDirection8_45(&ptTileCenter, &ptAttackCenter));
	}

	// 技能创建
	if(!(this->*m_funcSkill[pParam->nIndex])(pParam))
	{
		TraceLn("ERROR: CFashiView::cmdUseMagic(): skill create failed, ID = %d", pParam->nIndex);
		SetState(CrtStateStand);
		return FALSE;
	}

	// 做攻击动作
	if(pParam->nIndex == 0)	// 普通攻击无光效，有指定攻击动作
	{
		lTemp = GetNormalAttackActID();
		if (lTemp < 0)
			return TRUE;

		SetState(lTemp);
		SetStatusMagicCheckID(pParam->nCheckId);
		SetStatusAttackOnce();
		SetStatusPlayCommondAttackSound();
		return TRUE;
	}
	
	int nLen;

	// 做攻击动作
	pScpSkill->GetPropertyLong("Base_ActIDAttackUseType", lTemp, 0);
	if(lTemp == 0)		// 如果creature脚本配了生物的攻击动作则用那个，否则用技能脚本的
	{
		lTemp = GetSkillAttackActID();
		if(lTemp < 0)
		{
			DWORD dwActAttackID[] = {0,0};
			
			nLen = pScpSkill->GetArrayLong("Base_ActIDAttack");
			if(nLen < 1 || nLen > 2)
				return FALSE;

			if (nLen != pScpSkill->GetArrayLong("Base_ActIDAttack", (long*)dwActAttackID, nLen))
				return FALSE;

			if(nLen == 2 && GetWeaponStyle() == ActWeaponStyleDouble)
			{
				lTemp = dwActAttackID[1];
			}
			else
			{
				lTemp = dwActAttackID[0];
			}
		}
	}
	else				// 如果skill脚本配了攻击动作则用这个，否则用creature脚本的
	{
		DWORD dwActAttackID[] = {0,0};
		
		nLen = pScpSkill->GetArrayLong("Base_ActIDAttack");
		if(nLen < 1 || nLen > 2
			|| nLen != pScpSkill->GetArrayLong("Base_ActIDAttack", (long*)dwActAttackID, nLen))
		{
			lTemp = GetSkillAttackActID();
		}
		else
		{
			if(nLen == 2 && GetWeaponStyle() == ActWeaponStyleDouble)
			{
				lTemp = dwActAttackID[1];
			}
			else
			{
				lTemp = dwActAttackID[0];
			}
		}
	}
	
	SetState(lTemp);
	SetStatusMagicCheckID(pParam->nCheckId);

	pScpSkill->GetPropertyLong("Base_AttackOnceOrCtrl", lTemp, 0);
	if(lTemp == 0)
	{
		SetStatusAttackOnce();
	}
	else
	{
		SetStatusAttackCtrl();
	}

	// 攻击阶段的声音
	pScpSkill->GetPropertyLong("Base_SoundIDAttack", lTemp, 0);
	if(lTemp > 0)
	{
		long lSoundLoop;
		pScpSkill->GetPropertyLong("Base_bSoundLoopeAttack", lSoundLoop, 0);
		m_nAttackSoundChunnelID = Fun_PlaySound(m_ptTile, lTemp,lSoundLoop);
	}

	// 攻击阶段的技能光效
	DWORD dwMagicID[] = {0,0,0};
	nLen = pScpSkill->GetArrayLong("Base_MagicIDAttack");
	if (nLen != pScpSkill->GetArrayLong("Base_MagicIDAttack", (long*)dwMagicID, nLen))
		return FALSE;

	int nTime = 0;
	while(dwMagicID[nTime] > 0 && nTime <= 3)
	{
		SAttachStateObjBuffer st;
		memset(&st, 0, sizeof(st));
		st.dwResID = dwMagicID[nTime];

		IResObject* pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(st.dwResID);
		if (pMagicObj)
		{
			g_ResLoader.ReadAttachMagic(pMagicObj, &st);
			
			pMagicObj->GetPropertyLong("LoopNum", (long &)st.m_nLoopNum, 1);
			pMagicObj->GetPropertyLong("NoShowBegin", (long &)st.m_nNoShowBegin, -1);
			pMagicObj->GetPropertyLong("NoShowEnd", (long &)st.m_nNoShowEnd, -1);

			st.dwAppearanceID = -1;
			
			CAttachStateObj* pObj = CreateAttachObj(&st);
			if(pObj != NULL)
				m_vAttackAttachObjs.push_back(pObj);
		}
		nTime++;
	}

	return TRUE;
	
}
//------------------------------------------------------------------------
// 停止蓄气(转到站立状态，蓄气光效停止)
// dwParam1: ?
// dwParam2: ?
int	CCreatureView::cmdStopPreAttack(DWORD dwParam1, DWORD dwParam2)
{
	SetState(CrtStateStand);
	return TRUE;
}
//------------------------------------------------------------------------
// 停止攻击（只对持续施法有效，转到站立状态，施法光效停止，法术动画停止）
// dwParam1: ?
// dwParam2: ?
int	CCreatureView::cmdStopAttack(DWORD dwParam1, DWORD dwParam2)
{
	SetState(CrtStateStand);
	return TRUE;
}
//------------------------------------------------------------------------
// 停止攻击（只对持续施法有效，转到站立状态，施法光效停止，法术动画停止）
// dwParam1: ?
// dwParam2: ?
int	CCreatureView::cmdChangeLocForSkill(DWORD dwParam1, DWORD dwParam2)
{
	skill_ChangeLoc(*(POINT*)dwParam1);
	return TRUE;
}
//------------------------------------------------------------------------
// 范围技能的选择光效
// dwParam1: BOOL(标识设置还是取消)
// dwParam2: 技能ID
int	CCreatureView::cmdSkillSelAni(DWORD dwParam1, DWORD dwParam2)
{
	if(dwParam1 == 1)
	{
		g_pSkillSelMagic = CreateSkillSelMagic(dwParam2);
	}
	else
	{
		SAFE_RELEASE(g_pSkillSelMagic);
	}
	return TRUE;
}
//------------------------------------------------------------------------
// 生物死亡
// dwParam1: 0表示慢慢死亡，有动作，1表示已经死亡，无动作
// dwParam2: ?
int CCreatureView::cmdDie(DWORD dwParam1, DWORD dwParam2)
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

	return TRUE;
}
//------------------------------------------------------------------------
// 生物被击
// dwParam1: 生物被击时传入的相关参数(SBehitParam*)
// dwParam2: ?
int CCreatureView::cmdBeHit(DWORD dwParam1, DWORD dwParam2)
{
	SBehitParam* pParam = (SBehitParam*)dwParam1;

	// 做被击动作
	if(pParam->bDoAct)
	{
		if(IsMoving())
			StopMove();
		SetState(CrtStateBehit);
	}

	// 头上飘血压进伤害堆栈
	AddDmgStack(pParam);
	return TRUE;
}
//------------------------------------------------------------------------
// 给生物设置氏族名称
// dwParam1: LPCSTR(氏族名称)
// dwParam2: ?
int CCreatureView::cmdSetClanName(DWORD dwParam1, DWORD dwParam2)
{
	LPCSTR pStr = (LPCSTR)dwParam1;
	int nLen = strlen(pStr);
	if(nLen > 0)
	{
		m_bShowClanName = TRUE;
		char szTemp[CREATURE_MAX_NAME];
		lstrcpyn(szTemp, (LPCSTR)dwParam1, nLen + 1);
		sprintf(m_szClanName, "[%s] ", szTemp);
	}
	else
	{
		m_bShowClanName = FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 给生物设置城池名称
// dwParam1: LPCSTR(城池名称)
// dwParam2: ?
int CCreatureView::cmdSetCityName(DWORD dwParam1, DWORD dwParam2)
{
	LPCSTR pStr = (LPCSTR)dwParam1;
	int nLen = strlen(pStr);
	if(nLen > 0)
	{
		m_bShowCityName = TRUE;
		char szTemp[CREATURE_MAX_NAME];
		lstrcpyn(szTemp, (LPCSTR)dwParam1, nLen + 1);
		sprintf(m_szCityName, "【%s】", szTemp);
	}
	else
	{
		m_bShowCityName = FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------