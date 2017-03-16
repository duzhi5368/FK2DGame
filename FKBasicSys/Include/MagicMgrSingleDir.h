/**
*	created:		2012-7-2   3:12
*	filename: 		MagicMgrSingleDir
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		单向魔法管理器(单向魔法由3个阶段组成：创建、运行、爆炸)
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Magic.h"
//------------------------------------------------------------------------
class CMagicMgrSingleDir : public CMagicMgrBase, public ITimerSink
{
public:
	CMagicSingleDir		*m_pRun;
	CMagicGeneral		*m_pExplode;

public:
	struct SMagicMgrArchive_SingleDir
	{
		SMagicArchive_SingleDir		*pRun;		// 运行
		SMagicArchive_General		*pExplode;	// 爆炸
	};
	CMagicMgrSingleDir();
	virtual ~CMagicMgrSingleDir();

	virtual BOOL Create(void* buf, int nSize);
	virtual void OnTimer(DWORD dwEventID);
	virtual void OnPlayDone(void* pThis, POINT Pt);
	virtual void BeginAttack();
};
//------------------------------------------------------------------------