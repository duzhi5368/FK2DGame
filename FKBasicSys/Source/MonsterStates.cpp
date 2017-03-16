/**
*	created:		2012-7-2   3:36
*	filename: 		MonsterStates
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/MonsterView.h"
//------------------------------------------------------------------------
// 通用状态
CMstState::CMstState() 
{
	m_pMst = NULL;
	m_nDelay = 0;
	m_nAttackFrame = 0;
	m_nAttackSoundFrame = 0;
	m_nMagicCheckID = -1;
	m_bPreAttack = FALSE;
	m_bAttackOnce = FALSE;
	m_bAttackCtrl = FALSE;
	m_bPlayCommondAttackSound = FALSE;
}
//------------------------------------------------------------------------
CMstState::~CMstState() 
{
}
//------------------------------------------------------------------------
void CMstState::Start()
{
	m_nMagicCheckID = -1;
	if (m_nDelay > 0)
	{
		g_pTimeAxis->SetTimer(TIME_EVENT_ID_FRAME,
			static_cast<ITimerSink*>(m_pMst),
			m_nDelay,
			"BasicSys!!CMstState::Start");	
	}
}
//------------------------------------------------------------------------
void CMstState::End()
{
	DelAllAttribute();
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_FRAME, static_cast<ITimerSink*>(m_pMst));
}
//------------------------------------------------------------------------
void CMstState::OnTimer(DWORD dwEventID)
{
	if(m_nMagicCheckID >= 0)			// 是否有魔法附加在该动作上
	{
		if(m_pMst->GetCurFrame() >= (m_nAttackFrame))
		{
			m_pMst->MagicBeginAttack(m_nMagicCheckID);
			m_nMagicCheckID = -1;
		}
	}
	if(m_bPlayCommondAttackSound)			// 是否有普通攻击音效附加在该动作上
	{
		if(m_pMst->GetCurFrame() >= (m_nAttackSoundFrame))
		{
			Fun_PlaySound(m_pMst->m_ptTile, m_pMst->m_nCommonAttackSoundID);
			m_bPlayCommondAttackSound = FALSE;
		}
	}
	if (m_pMst->IncAndCheckFrameOverflow())
	{
		m_pMst->SetCurFrame(0);
		if(m_bAttackOnce)
		{
			m_pMst->SetState(CrtStateStand);
		}
	}
}
//------------------------------------------------------------------------
void CMstState::SetDelay(int nDelay)
{
	m_nDelay = nDelay;
}
//------------------------------------------------------------------------
int CMstState::GetDelay()
{
	return m_nDelay;
}
//------------------------------------------------------------------------
void CMstState::DelAllAttribute()
{
	if(m_nMagicCheckID >= 0)		// 是否有魔法附加在该动作上
	{
		m_pMst->MagicBeginAttack(m_nMagicCheckID);
		m_nMagicCheckID = -1;
	}

	if(m_bPlayCommondAttackSound)	// 是否有附加普通攻击音效
	{		
		Fun_PlaySound(m_pMst->m_ptTile, m_pMst->m_nCommonAttackSoundID);
		m_bPlayCommondAttackSound = FALSE;
	}

	if(m_bPreAttack)				// 是否有蓄气附加在该动作上
	{
		m_pMst->StopAllAttackAttach();
		m_pMst->StopAttackSound();
		m_bPreAttack = FALSE;
	}

	if(m_bAttackOnce)				// 是否有一次性攻击附加在该动作上
	{
		m_pMst->StopAllAttackAttach();
		m_pMst->StopAttackSound();
		m_bAttackOnce = FALSE;
	}

	if(m_bAttackCtrl)				// 是否有一次性攻击附加在该动作上
	{
		m_pMst->StopAllAttackAttach();
		m_pMst->StopAttackSound();
		m_pMst->StopMagic();
		m_bAttackCtrl = FALSE;
	}
}
//------------------------------------------------------------------------
void CMstState::SetAttackFrame(int nFrame)
{
	m_nAttackFrame = nFrame > 1 ? (nFrame - 1) : 0;
}
//------------------------------------------------------------------------
void CMstState::SetPlayCommondAttackSound()
{
	m_bPlayCommondAttackSound = TRUE;
}
//------------------------------------------------------------------------
void CMstState::SetAttackSoundFrame(int nFrame)
{
	m_nAttackSoundFrame = nFrame > 1 ? (nFrame - 1) : 0;
}
//------------------------------------------------------------------------
void CMstState::SetMagicCheckID(int nCheckID)
{
	m_nMagicCheckID = nCheckID;
}
//------------------------------------------------------------------------
void CMstState::SetPreAttack()
{
	m_bPreAttack = TRUE;
}
//------------------------------------------------------------------------
void CMstState::SetAttackOnce()
{
	m_bAttackOnce = TRUE;
}
//------------------------------------------------------------------------
void CMstState::SetAttackCtrl()
{
	m_bAttackCtrl = TRUE;
}
//------------------------------------------------------------------------
// 移动状态
void CMstStateMove::Start()
{
	m_pMst->CalcSpeedMove();
	m_pMst->SetCurSpeed(m_pMst->GetSpeedMove(m_pMst->GetDirection()));
	
	g_pTimeAxis->SetTimer(TIME_EVENT_ID_SHORT_STEPS,
		static_cast<ITimerSink*>(m_pMst),
		TIME_SHORT_STEPS,
		"BasicSys!!CMstStateMove::Start");

	CMstState::Start();
}
//------------------------------------------------------------------------
void CMstStateMove::End()
{
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_SHORT_STEPS, static_cast<ITimerSink*>(m_pMst));
	CMstState::End();
}
//------------------------------------------------------------------------
void CMstStateMove::OnTimer(DWORD dwEventID)
{
	if (dwEventID == 0)
	{
		if (m_pMst->IncAndCheckFrameOverflow())
			m_pMst->SetCurFrame(0);
	}
	else
		m_pMst->MoveStep((DWORD)TIME_SHORT_STEPS);
}
//------------------------------------------------------------------------
// 站立状态
void CMstStateStand::OnTimer(DWORD dwEventID)
{
	if(m_nMagicCheckID >= 0)			// 是否有魔法附加在该动作上
	{
		if(m_pMst->GetCurFrame() >= (m_nAttackFrame))
		{
			m_pMst->MagicBeginAttack(m_nMagicCheckID);
			m_nMagicCheckID = -1;
		}
	}
	if(m_bPlayCommondAttackSound)		// 是否有普通攻击音效附加在该动作上
	{
		if(m_pMst->GetCurFrame() >= (m_nAttackSoundFrame))
		{
			Fun_PlaySound(m_pMst->m_ptTile, m_pMst->m_nCommonAttackSoundID);
			m_bPlayCommondAttackSound = FALSE;
		}
	}
	if (m_pMst->IncAndCheckFrameOverflow())
	{
		m_pMst->SetCurFrame(0);
		if((rand() % 100) < 2 && m_pMst->m_nCurPalType != PalType_Stone)
		{
			m_pMst->SetState(CrtStateFallow);
		}
	}
}
//------------------------------------------------------------------------
// 休闲状态
void CMstStateFallow::Start()
{
	// 屏幕外的怪概率叫
	if(g_pPlayer != NULL)
	{
		POINTS ptPlayer = g_pPlayer->GetLoc();
		if(abs(m_pMst->m_ptTile.x - ptPlayer.x) > 10 || abs(m_pMst->m_ptTile.y - ptPlayer.y))		
		{
			if((rand() % 100) < 33)
				Fun_PlaySound(m_pMst->m_ptTile, m_pMst->m_nSoundIDFallow);
		}
		else
		{
			Fun_PlaySound(m_pMst->m_ptTile, m_pMst->m_nSoundIDFallow);
		}
	}
	
	CMstState::Start();
}
//------------------------------------------------------------------------
void CMstStateFallow::OnTimer(DWORD dwEventID)
{
	if(m_nMagicCheckID >= 0)			// 是否有魔法附加在该动作上
	{
		if(m_pMst->GetCurFrame() >= (m_nAttackFrame))
		{
			m_pMst->MagicBeginAttack(m_nMagicCheckID);
			m_nMagicCheckID = -1;
		}
	}
	if(m_bPlayCommondAttackSound)			// 是否有普通攻击音效附加在该动作上
	{
		if(m_pMst->GetCurFrame() >= (m_nAttackSoundFrame))
		{
			Fun_PlaySound(m_pMst->m_ptTile, m_pMst->m_nCommonAttackSoundID);
			m_bPlayCommondAttackSound = FALSE;
		}
	}
	if (m_pMst->IncAndCheckFrameOverflow())
	{
		m_pMst->SetCurFrame(0);
		m_pMst->SetState(CrtStateStand);
	}
}
//------------------------------------------------------------------------
// 死亡状态
void CMstStateDie::Start()
{
	Fun_PlaySound(m_pMst->m_ptTile, m_pMst->m_nSoundIDDie);
	CMstState::Start();
}
//------------------------------------------------------------------------
void CMstStateDie::OnTimer(DWORD dwEventId)
{
	if (m_pMst->GetCurFrame() == -1 || m_pMst->IncAndCheckFrameOverflow())
	{
		int nCurFrame = m_pMst->GetTotalFrame() - 1;
		m_pMst->SetCurFrame(nCurFrame);
	}
}
//------------------------------------------------------------------------
// 后仰状态
void CMstStateBehit::Start()
{
	m_nState = stateCreate;
	m_nBeHitLoopBeginFrame = m_pMst->m_nBeHitLoopBeginFrame - 1;
	m_nBeHitLoopEndFrame = m_pMst->m_nBeHitLoopEndFrame - 1;

	Fun_PlaySound(m_pMst->m_ptTile, m_pMst->m_nSoundIDBeHit);
	CMstState::Start();
}
//------------------------------------------------------------------------
void CMstStateBehit::OnTimer(DWORD dwEventID)
{
	if(m_nBeHitLoopBeginFrame < 0
		|| m_nBeHitLoopEndFrame <= m_nBeHitLoopBeginFrame
		|| m_nBeHitLoopEndFrame >= m_pMst->GetTotalFrame())
	{
		m_nBeHitLoopBeginFrame = 0;
		m_nBeHitLoopEndFrame = m_pMst->GetTotalFrame() - 1;
	}
	
	if (m_nState == stateCreate) // 启动阶段
	{
		if (m_pMst->IncAndCheckFrameOverflow(m_nBeHitLoopBeginFrame)) 
			m_nState = stateLoop;
	}
	else if (m_nState == stateLoop) // 循环运行阶段
	{
		if (m_pMst->IncAndCheckFrameOverflow(m_nBeHitLoopEndFrame)) 
			m_pMst->SetCurFrame(m_nBeHitLoopBeginFrame);
	}
	else if (m_nState == stateRelease) // 销毁阶段
	{ 
		if(m_pMst->IncAndCheckFrameOverflow())
		{
			m_pMst->SetCurFrame(0);
			m_pMst->SetState(CrtStateStand);
		}
	}
}
//------------------------------------------------------------------------
void CMstStateBehit::Stop()
{
	m_nState = stateRelease;
}
//------------------------------------------------------------------------
// 状态机
CMstFSM::CMstFSM()
{
	for (int i = 0; i < CrtStateMax; i++)
	{
		m_pStates[i] = NULL;
	}

	m_pStates[CrtStateStand]			= &m_StateStand;
	m_pStates[CrtStateRideStand]		= &m_StateRideStand;
	m_pStates[CrtStateSit]				= &m_StateSit;

	m_pStates[CrtStateWalk]				= &m_StateWalk;
	m_pStates[CrtStateRun]				= &m_StateRun;
	m_pStates[CrtStateRideRun]			= &m_StateRideRun;
	
	m_pStates[CrtStateBehit]			= &m_StateBehit;
	m_pStates[CrtStateDie]				= &m_StateDie;

	m_pStates[CrtStateFallow]			= &m_StateFallow;
	m_pStates[CrtStateJuqi1]			= &m_StatePre1;				
	m_pStates[CrtStateJuqi2]			= &m_StatePre2;

	m_pStates[CrtStateMagic1]			= &m_StateMagic1;				
	m_pStates[CrtStateMagic2]			= &m_StateMagic2;

	m_pStates[CrtStateSingleAttack1]	= &m_StateSingleAttack1;
	m_pStates[CrtStateSingleAttack2]	= &m_StateSingleAttack2;
	m_pStates[CrtStateSingleAttack3]	= &m_StateSingleAttack3;

	m_pStates[CrtStateDoubleAttack1]	= &m_StateDoubleAttack1;
	m_pStates[CrtStateDoubleAttack2]	= &m_StateDoubleAttack2;
	
	m_pStates[CrtStateArrowAttack]		= &m_StateArrowAttack;

	m_pStates[CrtStateCommonAttack1]	= &m_StateCommonAttack1;
	m_pStates[CrtStateCommonAttack2]	= &m_StateCommonAttack2;
	
	m_pStates[CrtStateSkillAttack1]		= &m_StateSkillAttack1;
	m_pStates[CrtStateSkillAttack2]		= &m_StateSkillAttack2;

	m_pCurrentState = m_pStates[CrtStateStand];
}
//------------------------------------------------------------------------
CMstFSM::~CMstFSM()
{
}
//------------------------------------------------------------------------
bool CMstFSM::Create(CMonsterView* pMst)
{
	for (int i = 0; i < CrtStateMax; i++)
	{
		if (m_pStates[i])
			m_pStates[i]->m_pMst = pMst;
	}
	return TRUE;
}
//------------------------------------------------------------------------
void CMstFSM::OnTimer(DWORD dwEventID)
{
	if (m_pCurrentState)
		m_pCurrentState->OnTimer(dwEventID);
}
//------------------------------------------------------------------------
bool CMstFSM::IsState(CMstState& State)
{
	return (m_pCurrentState == &State);
}
//------------------------------------------------------------------------
CMstState* CMstFSM::GetCurState()
{
	return m_pCurrentState;
}
//------------------------------------------------------------------------
bool CMstFSM::GotoState(CMstState& NewState)
{
	if (m_pCurrentState)
		m_pCurrentState->End();

	m_pCurrentState = &NewState;

	m_pCurrentState->Start();

	return true;
}
//------------------------------------------------------------------------
void CMstFSM::SetAllStateDelay(int* pDelay)
{
	for (int i = 0; i < CrtStateMax; i++)
	{
		if (m_pStates[i])
			m_pStates[i]->SetDelay(pDelay[i]);
	}
}
//------------------------------------------------------------------------
void CMstFSM::SetAllStateAttackFrame(int* pFrame)
{
	for (int i = 0; i < CrtStateMax; i++)
	{
		if (m_pStates[i])
			m_pStates[i]->SetAttackFrame(pFrame[i]);
	}
}
//------------------------------------------------------------------------
void CMstFSM::SetAllStateAttackSoundFrame(int* pFrame)
{
	for (int i = 0; i < CrtStateMax; i++)
	{
		if (m_pStates[i])
			m_pStates[i]->SetAttackSoundFrame(pFrame[i]);
	}
}
//------------------------------------------------------------------------
void CMstFSM::SetMoveDelay(int* pDelay)
{
	if (m_pStates[CrtStateWalk])
		m_pStates[CrtStateWalk]->SetDelay(pDelay[0]);
	
	if (m_pStates[CrtStateRun])
		m_pStates[CrtStateRun]->SetDelay(pDelay[1]);
	
	if (m_pStates[CrtStateRideRun])
		m_pStates[CrtStateRideRun]->SetDelay(pDelay[2]);
}
//------------------------------------------------------------------------