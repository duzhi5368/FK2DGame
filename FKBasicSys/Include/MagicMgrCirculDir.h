/**
*	created:		2012-7-2   2:58
*	filename: 		MagicMgrCirculDir
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		8方向的圆形和扇形技能管理器
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Magic.h"
//------------------------------------------------------------------------
class CMagicMgrCirculDir : public CMagicMgrBase, public ITimerSink
{
protected:
	CMagicEightDir	**m_paRun;
	int				m_nDirNum;			// 创建的方向数，8为圆形，5为扇形
	int				m_nCurDestroyCount;	// 目前销毁个数
	POINT			m_ptTile;			// 生物所在TILE
	BOOL			m_bRealTime;		// 运行光效是否不受攻击帧控制
public:
	struct SMagicMgrArchive_CirculDir
	{
		int			nDirNum;		// 创建的方向数，8为圆形，5为扇形
		POINT		ptTile;			// 生物所在TILE
		POINT		ptAttackTile;	// 攻击点所在TILE
		SMagicArchive_EightDir		*pRun;		// 运行
		BOOL		bRealTime;		// 运行光效是否不受攻击帧控制
	};

	CMagicMgrCirculDir();
	virtual ~CMagicMgrCirculDir();

	virtual BOOL Create(void* buf, int nSize);
	virtual void OnTimer(DWORD dwEventID);
	virtual void OnPlayDone(void* pThis, POINT Pt);
	virtual void BeginAttack();
};
//------------------------------------------------------------------------