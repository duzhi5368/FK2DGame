/**
*	created:		2012-7-2   3:09
*	filename: 		MagicMgrMultiUnit
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		多单元类型魔法管理器
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Magic.h"
#include "../../FKGame/MapInfo/Tile.h"
//------------------------------------------------------------------------
#define MAX_ATTACK_TIME		10000
//------------------------------------------------------------------------
class CMagicMgrMultiUnit : public CMagicMgrBase, public ITimerSink
{
	SMagicArchive_Rain	m_pUnit;

	POINT			m_ptAttackLT;
	int				m_nWidth;
	int				m_nLoopTime;		// 循环时间
	int				m_nDelay;			// 创建延时
	BOOL			m_bStop;


public:
	struct SMagicMgrArchive_MultiUnit
	{
		POINT		ptAttackTile;		// 攻击点所在TILE
		int			nWidth;				// 正方形区域的边长(TILE个数)
		int			nDelay;				// 创建单元魔法间隔
		int			nLoopTime;			// 循环时间
		SMagicArchive_Rain	*pUnit;		// 单元的创建包
	};
public:
	CMagicMgrMultiUnit();
	virtual ~CMagicMgrMultiUnit();

public:
	virtual BOOL Create(void* buf, int nSize);
	virtual void OnTimer(DWORD dwEventID);
	virtual void OnPlayDone(void* pThis, POINT Pt);
	virtual void Stop();
	virtual BOOL IsRunning();
	virtual void BeginAttack();
};
//------------------------------------------------------------------------