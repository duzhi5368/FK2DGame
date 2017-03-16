/**
*	created:		2012-7-2   3:07
*	filename: 		MagicMgrGeneral
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		通用的魔法管理器(由运行态和爆炸态组成)
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Magic.h"
//------------------------------------------------------------------------
class CMagicMgrGeneral : public CMagicMgrBase, public ITimerSink
{
	CMagicGeneral	*m_pRun;
	CMagicGeneral	*m_pExplode;
	int				m_nLoopTime;
	BOOL			m_bIsRunning;
	BOOL			m_bBegin;
	BOOL			m_bCanCtrl;
	BOOL			m_bOnceAttack;		// 是否一次性伤害
public:
	struct SMagicMgrArchive_General
	{
		POINT					ptTile;				// 生物所在TILE
		POINT					ptAttackTile;		// 攻击点所在TILE
		BOOL					bNeedAttackDelay;	// 是否需要攻击延迟
		int						nLoopTime;			// 循环的时间
		BOOL					bCanCtrl;			// 是否能控制
		BOOL					bOnceAttack;		// 是否一次性伤害
		
		SMagicArchive_General	*pRun;				// 运行态魔法的创建缓冲
		SMagicArchive_General	*pExplode;			// 爆炸态魔法的创建缓冲
	};
	CMagicMgrGeneral();
	virtual ~CMagicMgrGeneral();
	
public:
	virtual BOOL Create(void* buf, int nSize);
	virtual void OnPlayDone(void* pThis, POINT Pt);
	virtual void OnTimer(DWORD dwEventID);
	virtual void BeginAttack();
	virtual void Stop();
	virtual BOOL IsRunning();
	virtual void AddDamage(CCreatureView *pView, SFlowParam FlowParam);
};
//------------------------------------------------------------------------