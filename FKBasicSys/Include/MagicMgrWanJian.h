/**
*	created:		2012-7-2   3:13
*	filename: 		MagicMgrWanJian
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		万剑朝宗
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Magic.h"
//------------------------------------------------------------------------
class CMagicMgrWanJian : public CMagicMgrBase, public ITimerSink
{
	int				m_nDir;

	int				m_nLoopTime;				// 循环时间
	int				m_nDelay;					// 创建延时
	BOOL			m_bStop;
	POINT			m_ptTile;					// 生物所在TILE
	
public:
	struct SMagicMgrArchive_WanJian
	{
		POINT		ptTile;						// 生物所在TILE
		int			nDelay;						// 创建单元魔法间隔
		int			nLoopTime;					// 循环时间
	};
public:
	CMagicMgrWanJian();
	virtual ~CMagicMgrWanJian();

	virtual BOOL Create(void* buf, int nSize);
	virtual void OnTimer(DWORD dwEventID);
	virtual void OnPlayDone(void* pThis, POINT Pt);
	virtual void Stop();
	virtual BOOL IsRunning();
	virtual void BeginAttack();
};
//------------------------------------------------------------------------