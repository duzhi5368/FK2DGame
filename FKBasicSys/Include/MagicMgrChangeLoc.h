/**
*	created:		2012-7-2   2:57
*	filename: 		MagicMgrChangeLoc
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		普通攻击魔法管理器
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Magic.h"
//------------------------------------------------------------------------
#define SHANDOW_STEP	15
#define SHANDOW_NUM		30
//------------------------------------------------------------------------
class CMagicMgrChangeLoc : public ITimerSink
{
public:
	struct STile
	{
		POINT		ptTile;
		POINT		ptOffsetTile;
	};
	CMagicMgrChangeLoc();
	virtual ~CMagicMgrChangeLoc();

	BOOL Create(POINT pt, CCreatureView *pMaster);
	void OnTimer(DWORD dwEventID);

public:
	CCreatureView* m_pMasterView;
	int m_nCurNodeIndex;
	STile m_pPathList[SHANDOW_STEP];
};
//------------------------------------------------------------------------