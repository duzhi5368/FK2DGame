/**
*	created:		2012-7-2   2:58
*	filename: 		MagicMgrCommonAttack
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		普通攻击魔法管理器
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Magic.h"
//------------------------------------------------------------------------
class CMagicMgrCommonAttack : public CMagicMgrBase, public ITimerSink
{
public:
	CMagicMgrCommonAttack();
	virtual ~CMagicMgrCommonAttack();

public:
	virtual BOOL Create(void* buf, int nSize);
	virtual void OnTimer(DWORD dwEventID);
	virtual void BeginAttack();
};
//------------------------------------------------------------------------