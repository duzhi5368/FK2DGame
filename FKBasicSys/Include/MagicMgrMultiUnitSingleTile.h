/**
*	created:		2012-7-2   3:10
*	filename: 		MagicMgrMultiUnitSingleTile
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		多单元类型魔法管理器只是针对一个Tile的
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Magic.h"
#include "../../FKGame/MapInfo/Tile.h"
//------------------------------------------------------------------------
class CMagicMgrMultiUnitSingleTile : public CMagicMgrBase, public ITimerSink
{
	CMagicRain	**m_paUnit;				// 单元数组

	POINT		m_ptAttackTile;			// 攻击点所在TILE
	int			m_nUnitCount;			// 单元数目
	int			m_nCurCreateCount;		// 目前创建个数
	int			m_nCurDestroyCount;		// 目前销毁个数
	int			m_nDelay;				// 创建延时
	int			m_nFlowDamNum;			// 当前还要飘的伤害数

public:
	struct SMagicMgrArchive_MultiUnitSingleTile
	{
		POINT		ptAttackTile;		// 攻击点所在TILE
		int			nDelay;				// 创建单元魔法间隔
		int			nTime;				// 掉落次数
		SMagicArchive_Rain	*pUnit;		// 单元的创建包
	};
public:
	CMagicMgrMultiUnitSingleTile();
	virtual ~CMagicMgrMultiUnitSingleTile();

public:
	virtual BOOL Create(void* buf, int nSize);
	virtual void OnTimer(DWORD dwEventID);
	virtual void OnPlayExplode(void* pThis);
	virtual void OnPlayDone(void* pThis, POINT Pt);
	virtual void BeginAttack();
};
//------------------------------------------------------------------------