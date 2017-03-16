/**
*	created:		2012-7-2   3:08
*	filename: 		MagicMgrMultiDir
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		特殊的多单元类型魔法管理器（象暴烈火球，是多方向的，但是目的地是一个区域）
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Magic.h"
//------------------------------------------------------------------------
#define MAX_DIR_COUNT	20
//------------------------------------------------------------------------
class CMagicMgrMultiDirEx : public CMagicMgrBase, public ITimerSink
{
	CMagicSingleDir		**m_paUnit;				// 单元数组
	CMagicGeneral		**m_paLeftObj;			// 残留物对象

	POINT				m_ptAttackLT;
	int					m_nWidth;
	int					m_nDelay;				// 创建延时

	int					m_nExplodeNum;
	int					m_nUnitCount;			// 单元数目
	int					m_nCurCreateCount;		// 目前创建个数
	int					m_nCurDestroyCount;		// 目前销毁个数
public:
	struct SMagicMgrArchive_MultiDirEx
	{
		POINT	ptAttackTile;					// 攻击点所在TILE
		int		nWidth;							// 正方形区域的边长(TILE个数)
		int		nDelay;							// 创建单元魔法间隔
		SMagicArchive_SingleDir		*pRun;		// 运行
		SMagicArchive_General		*pExplode;	// 爆炸
	};
public:
	CMagicMgrMultiDirEx();
	virtual ~CMagicMgrMultiDirEx();

	virtual BOOL Create(void* buf, int nSize);
	virtual void OnTimer(DWORD dwEventID);
	virtual void OnPlayDone(void* pThis, POINT Pt);
	virtual void BeginAttack();
};
//------------------------------------------------------------------------