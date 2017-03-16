/**
*	created:		2012-7-2   3:11
*	filename: 		MagicMgrShadow
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		普通攻击魔法管理器
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Magic.h"
//------------------------------------------------------------------------
class CMagicMgrShadow : public ITimerSink
{
public:
	CMagicMgrShadow();
	virtual ~CMagicMgrShadow();

	BOOL Create(CCreatureView *pMaster);
	void OnTimer(DWORD dwEventID);

public:
	CCreatureView* m_pMasterView;
};
//------------------------------------------------------------------------