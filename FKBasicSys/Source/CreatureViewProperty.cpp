/**
*	created:		2012-7-2   2:46
*	filename: 		CreatureViewProperty
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/CreatureView.h"
#include "../Include/ResLoader.h"
#include "../Include/SkillCreate.h"
//------------------------------------------------------------------------
// 所有支持的属性
CCreatureView::ProcProperty CCreatureView::m_Property[CrtPropMaxPropCount] =
{
	NULL,
	&CCreatureView::propMoveStype,
	&CCreatureView::propTile,
	&CCreatureView::propUserName,
	&CCreatureView::propShowName,
	&CCreatureView::propNameColor,
	&CCreatureView::propAbsolutePosition,
	&CCreatureView::propWeaponStyle,
	&CCreatureView::propWeaponSoundID,
	&CCreatureView::propBeHitSoundID,
	&CCreatureView::propIsMoving,
	&CCreatureView::propState,
	&CCreatureView::propDirection,
	&CCreatureView::propMoveSpeedLevel,
	&CCreatureView::propAttackSpeedLevel,
	&CCreatureView::propGroupInfoImage,
};
//------------------------------------------------------------------------
// 移动类型
// 设置或返回: DWORD* (ActMoveStyleWalk or ActMoveStyleRun or ActMoveStyleRide)
void CCreatureView::propMoveStype(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
	{
		if(*(DWORD*)dwPropVal != ActMoveStyleWalk
			&& *(DWORD*)dwPropVal != ActMoveStyleRun
			&& *(DWORD*)dwPropVal != ActMoveStyleRide)
			return;

		if(GetMoveStyle() == *(DWORD*)dwPropVal)
			return;

		SetMoveStyle(*(DWORD*)dwPropVal);

		if(IsMoving())
		{
			cmdMove((DWORD)(m_pPathList + m_nCurNodeIndex), m_nPathNodeCount - m_nCurNodeIndex);
		}
		else
		{
			SetState(CrtStateStand);
		}
	}
	else
		*(DWORD*)dwPropVal = GetMoveStyle();
}
//------------------------------------------------------------------------
// 生物的TILE坐标(客户端只有在搜路时调用这个属性，因为他对走路作了优化，
// 其他情况使用propAbsolutePosition然后ScreenToTile)
// 设置或返回: POINT*
void CCreatureView::propTile(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
	{
		m_ptTile = *(POINT*)dwPropVal;
	}
	else
	{
		*(POINT*)dwPropVal = m_ptTile;
	}
}
//------------------------------------------------------------------------
// 生物的名字
// 设置或返回: char*
void CCreatureView::propUserName(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
	{
		LPCSTR pStr = (LPCSTR)dwPropVal;
		int nLen = strlen(pStr);
		lstrcpyn(m_szUserName, (LPCSTR)dwPropVal, nLen + 1);
	}
	else
	{
		int nLen = strlen(m_szUserName);
		lstrcpyn((char*)dwPropVal, m_szUserName, nLen + 1);
	}
}
//------------------------------------------------------------------------
// 是否让生物显示自己的名字
// 设置或返回: BOOL*
void CCreatureView::propShowName(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
		m_bShowUserName = *(BOOL*)dwPropVal;
	else
		*(BOOL*)dwPropVal = m_bShowUserName;
}
//------------------------------------------------------------------------
// 生物名字的颜色
// 设置或返回: WPixel*
void CCreatureView::propNameColor(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
		m_clrName = *(WPixel*)dwPropVal;
	else
		*(WPixel*)dwPropVal = m_clrName;
}
//------------------------------------------------------------------------
// 生物的绝对位置(ptTileCenter+ptOffset)
// 返回: POINT*
void CCreatureView::propAbsolutePosition(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
	{
		TraceLn("CCreatureView::propAbsolutePosition.Set() is not implemented!");
	}
	else
	{
		POINT pt;
		Tile_To_Screen(m_ptTile, pt);
		pt.x += m_ptTileOffset.x;
		pt.y += m_ptTileOffset.y;
		*(POINT*)dwPropVal = pt;
	}
}
//------------------------------------------------------------------------
// 生物是否正在移动
// 返回: BOOL*
void CCreatureView::propIsMoving(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
		TraceLn("CCreatureView::propIsMoving.Set() is not implemented!");
	else
		*(BOOL*)dwPropVal = IsMoving();
}
//------------------------------------------------------------------------
// 生物的状态
// 返回: int*
void CCreatureView::propState(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
		SetState(*(int*)dwPropVal);
	else 
		*(int*)dwPropVal = GetState();

}
//------------------------------------------------------------------------
// 生物的方向
// 设置或返回: int*
void CCreatureView::propDirection(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp) // 当角色走到非敌对NPC旁边时，客户端可以设置NPC的方向来使之面对角色
		SetDirection(*(int*)dwPropVal);
	else 
		*(int*)dwPropVal = GetDirection();
}
//------------------------------------------------------------------------
// 生物的移动速度等级(0->8)
// 设置或返回: int*
void CCreatureView::propMoveSpeedLevel(BOOL bSetProp, DWORD dwPropVal)
{
	if (bSetProp)
	{
		ASSERT(*(BYTE*)dwPropVal > 0 && *(BYTE*)dwPropVal < MAX_DELAY_TYPE);

		if (*(BYTE*)dwPropVal != m_nMoveSpeedLevel)
		{
			m_nMoveSpeedLevel = *(BYTE*)dwPropVal;
			
			int nDelay[3];
			nDelay[0] = g_ResLoader.GetWalkDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
			nDelay[1] = g_ResLoader.GetRunDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
			nDelay[2] = g_ResLoader.GetRideDelayByGrade(m_AniContext.nType, m_nMoveSpeedLevel);
			SetAllMoveDelay(nDelay);
			if(IsMoving())
			{
				CalcSpeedMove();
				SetCurSpeed(GetSpeedMove(GetDirection()));
				RestartCurState();
			}
		}
	}
	else 
	{
		*(int*)dwPropVal = m_nMoveSpeedLevel;
	}
}
//------------------------------------------------------------------------
// 生物的攻击速度等级(0->8)
// 设置或返回: int*
void CCreatureView::propAttackSpeedLevel(BOOL bSetProp, DWORD dwPropVal)
{
/*
	if (bSetProp)
	{
		ASSERT(*(BYTE*)dwPropVal > 0 && *(BYTE*)dwPropVal < MAX_DELAY_TYPE);

		int nOldSpdLvl = GetAttackSpeedLevel();
		SetAttackSpeedLevel(*(int*)dwPropVal);
		if (nOldSpdLvl != GetAttackSpeedLevel() && State() == ActStateAttack)
		{
			GetCurState()->End();
			GetCurState()->Start();
		}
	}
	else 
		*(int*)dwPropVal = GetAttackSpeedLevel();
*/
}
//------------------------------------------------------------------------