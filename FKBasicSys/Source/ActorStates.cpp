/**
*	created:		2012-7-2   3:46
*	filename: 		ActorsState
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/ActorView.h"
#include "../Include/ActorStates.h"
//------------------------------------------------------------------------
// 通用状态
CActState::CActState() 
{
	m_pAct = NULL;
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
CActState::~CActState() 
{
}
//------------------------------------------------------------------------
void CActState::Start()
{
	m_nMagicCheckID = -1;
	m_bPlayCommondAttackSound = FALSE;
	if (m_nDelay > 0)
	{
		g_pTimeAxis->SetTimer(TIME_EVENT_ID_FRAME,
			static_cast<ITimerSink*>(m_pAct),
			m_nDelay,
			"BasicSys!!CActState::Start");
	}
}
//------------------------------------------------------------------------
void CActState::End()
{
	DelAllAttribute();
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_FRAME, static_cast<ITimerSink*>(m_pAct));
}
//------------------------------------------------------------------------
void CActState::OnTimer(DWORD dwEventID)
{
	if(m_nMagicCheckID >= 0)			// 是否有魔法附加在该动作上
	{
		if(m_pAct->GetCurFrame() >= (m_nAttackFrame))
		{
			m_pAct->MagicBeginAttack(m_nMagicCheckID);
			m_nMagicCheckID = -1;
		}
	}
	if(m_bPlayCommondAttackSound)			// 是否有普通攻击音效附加在该动作上
	{
		if(m_pAct->GetCurFrame() >= (m_nAttackSoundFrame))
		{
			Fun_PlaySound(m_pAct->m_ptTile, m_pAct->m_nCommonAttackSoundID);
			m_bPlayCommondAttackSound = FALSE;
		}
	}
	if (m_pAct->IncAndCheckFrameOverflow())
	{
		m_pAct->SetCurFrame(0);
		if(m_bAttackOnce)
		{
			m_pAct->SetState(CrtStateStand);
		}
	}
}
//------------------------------------------------------------------------
void CActState::SetDelay(int nDelay)
{
	m_nDelay = nDelay;
}
//------------------------------------------------------------------------
int CActState::GetDelay()
{
	return m_nDelay;
}
//------------------------------------------------------------------------
void CActState::DelAllAttribute()
{
	if(m_nMagicCheckID >= 0)		// 是否有魔法附加在该动作上
	{
		m_pAct->MagicBeginAttack(m_nMagicCheckID);
		m_nMagicCheckID = -1;
	}

	if(m_bPlayCommondAttackSound)	// 是否有附加普通攻击音效
	{
		Fun_PlaySound(m_pAct->m_ptTile, m_pAct->m_nCommonAttackSoundID);
		m_bPlayCommondAttackSound = FALSE;
	}

	if(m_bPreAttack)				// 是否有蓄气附加在该动作上
	{
		m_pAct->StopAllAttackAttach();
		m_pAct->StopAttackSound();
		m_bPreAttack = FALSE;
	}

	if(m_bAttackOnce)				// 是否有一次性攻击附加在该动作上
	{
		m_pAct->StopAllAttackAttach();
		m_pAct->StopAttackSound();
		m_bAttackOnce = FALSE;
	}

	if(m_bAttackCtrl)				// 是否有一次性攻击附加在该动作上
	{
		m_pAct->StopAllAttackAttach();
		m_pAct->StopAttackSound();
		m_pAct->StopMagic();
		m_bAttackCtrl = FALSE;
	}
}
//------------------------------------------------------------------------
void CActState::SetAttackFrame(int nFrame)
{
	m_nAttackFrame = nFrame > 1 ? (nFrame - 1) : 0;
}
//------------------------------------------------------------------------
void CActState::SetPlayCommondAttackSound()
{
	m_bPlayCommondAttackSound = TRUE;
}
//------------------------------------------------------------------------
void CActState::SetAttackSoundFrame(int nFrame)
{
	m_nAttackSoundFrame = nFrame > 1 ? (nFrame - 1) : 0;
}
//------------------------------------------------------------------------
void CActState::SetMagicCheckID(int nCheckID)
{
	m_nMagicCheckID = nCheckID;
}
//------------------------------------------------------------------------
void CActState::SetPreAttack()
{
	m_bPreAttack = TRUE;
}
//------------------------------------------------------------------------
void CActState::SetAttackOnce()
{
	m_bAttackOnce = TRUE;
}
//------------------------------------------------------------------------
void CActState::SetAttackCtrl()
{
	m_bAttackCtrl = TRUE;
}
//------------------------------------------------------------------------
// 移动状态
void CActStateMove::Start()
{
	m_pAct->CalcSpeedMove();
	m_pAct->SetCurSpeed(m_pAct->GetSpeedMove(m_pAct->GetDirection()));
	
	g_pTimeAxis->SetTimer(TIME_EVENT_ID_SHORT_STEPS,
		static_cast<ITimerSink*>(m_pAct),
		TIME_SHORT_STEPS,
		"BasicSys!!CActStateMove::Start");

	CActState::Start();
}
//------------------------------------------------------------------------
void CActStateMove::End()
{
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_SHORT_STEPS, static_cast<ITimerSink*>(m_pAct));
	CActState::End();
}
//------------------------------------------------------------------------
void CActStateMove::OnTimer(DWORD dwEventID)
{
	if (dwEventID == 0)
	{
		if (m_pAct->IncAndCheckFrameOverflow())
			m_pAct->SetCurFrame(0);
	}
	else
		m_pAct->MoveStep((DWORD)TIME_SHORT_STEPS);
}
//------------------------------------------------------------------------
// 休闲状态
void CActStateFallow::OnTimer(DWORD dwEventId)
{
	if(m_nMagicCheckID >= 0)			// 是否有魔法附加在该动作上
	{
		if(m_pAct->GetCurFrame() >= (m_nAttackFrame))
		{
			m_pAct->MagicBeginAttack(m_nMagicCheckID);
			m_nMagicCheckID = -1;
		}
	}
	if(m_bPlayCommondAttackSound)			// 是否有普通攻击音效附加在该动作上
	{
		if(m_pAct->GetCurFrame() >= (m_nAttackSoundFrame))
		{
			Fun_PlaySound(m_pAct->m_ptTile, m_pAct->m_nCommonAttackSoundID);
			m_bPlayCommondAttackSound = FALSE;
		}
	}
	if (m_pAct->IncAndCheckFrameOverflow())
	{
		m_pAct->SetCurFrame(0);
		m_pAct->SetState(CrtStateStand);
	}
}
//------------------------------------------------------------------------
// 死亡状态
void CActStateDie::OnTimer(DWORD dwEventId)
{
	if (m_pAct->IncAndCheckFrameOverflow())
	{
		int nCurFrame = m_pAct->GetTotalFrame() - 1;		//假如总帧数是n，那最后一帧肯定是n-1
		m_pAct->SetCurFrame(nCurFrame);
	}
}
//------------------------------------------------------------------------
// 后仰状态
void CActStateBehit::Start()
{
	m_nState = stateCreate;
	m_nBeHitLoopBeginFrame = m_pAct->m_nBeHitLoopBeginFrame - 1;
	m_nBeHitLoopEndFrame = m_pAct->m_nBeHitLoopEndFrame - 1;

	CActState::Start();
}
//------------------------------------------------------------------------
void CActStateBehit::OnTimer(DWORD dwEventID)
{
	if(m_nBeHitLoopBeginFrame < 0
		|| m_nBeHitLoopEndFrame <= m_nBeHitLoopBeginFrame
		|| m_nBeHitLoopEndFrame >= m_pAct->GetTotalFrame())
	{
		m_nBeHitLoopBeginFrame = 0;
		m_nBeHitLoopEndFrame = m_pAct->GetTotalFrame() - 1;
	}

	if (m_nState == stateCreate) // 启动阶段
	{
		if (m_pAct->IncAndCheckFrameOverflow(m_nBeHitLoopBeginFrame)) 
			m_nState = stateLoop;
	}
	else if (m_nState == stateLoop) // 循环运行阶段
	{
		if (m_pAct->IncAndCheckFrameOverflow(m_nBeHitLoopEndFrame)) 
			m_pAct->SetCurFrame(m_nBeHitLoopBeginFrame);
	}
	else if (m_nState == stateRelease) // 销毁阶段
	{ 
		if(m_pAct->IncAndCheckFrameOverflow())
		{
			m_pAct->SetCurFrame(0);
			m_pAct->SetState(CrtStateStand);
		}
	}
}
//------------------------------------------------------------------------
void CActStateBehit::Stop()
{
	m_nState = stateRelease;
}
//------------------------------------------------------------------------
// 攻击状态
void CActStateAttack::OnTimer(DWORD dwEventID)
{
	if(m_nMagicCheckID >= 0)			// 是否有魔法附加在该动作上
	{
		if(m_pAct->GetCurFrame() >= (m_nAttackFrame))
		{
			m_pAct->MagicBeginAttack(m_nMagicCheckID);
			m_nMagicCheckID = -1;
		}
	}
	if(m_bPlayCommondAttackSound)			// 是否有普通攻击音效附加在该动作上
	{
		if(m_pAct->GetCurFrame() >= (m_nAttackSoundFrame))
		{
			Fun_PlaySound(m_pAct->m_ptTile, m_pAct->m_nCommonAttackSoundID);
			m_bPlayCommondAttackSound = FALSE;
		}
	}
	if (m_pAct->IncAndCheckFrameOverflow())
	{
		m_pAct->SetCurFrame(0);
		if(m_bAttackOnce)
		{
			m_pAct->SetState(CrtStateFallow);
		}
	}
}
//------------------------------------------------------------------------
// 状态机
CActFSM::CActFSM()
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

	m_pStates[CrtStateFallow]			= &m_StatePreFallow;		// 暂时用休闲动作代替蓄气
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
CActFSM::~CActFSM()
{
}
//------------------------------------------------------------------------
bool CActFSM::Create(CActorView* pAct)
{
	for (int i = 0; i < CrtStateMax; i++)
	{
		if (m_pStates[i])
			m_pStates[i]->m_pAct = pAct;
	}
	return TRUE;
}
//------------------------------------------------------------------------
void CActFSM::OnTimer(DWORD dwEventID)
{
	if (m_pCurrentState)
		m_pCurrentState->OnTimer(dwEventID);
}
//------------------------------------------------------------------------
bool CActFSM::IsState(CActState& State)
{
	return (m_pCurrentState == &State);
}
//------------------------------------------------------------------------
CActState* CActFSM::GetCurState()
{
	return m_pCurrentState;
}
//------------------------------------------------------------------------
bool CActFSM::GotoState(CActState& NewState)
{
	if (m_pCurrentState)
		m_pCurrentState->End();

	m_pCurrentState = &NewState;

	m_pCurrentState->Start();

	return true;
}
//------------------------------------------------------------------------
void CActFSM::SetAllStateDelay(int* pDelay)
{
	for (int i = 0; i < CrtStateMax; i++)
	{
		if (m_pStates[i])
			m_pStates[i]->SetDelay(pDelay[i]);
	}
}
//------------------------------------------------------------------------
void CActFSM::SetAllStateAttackFrame(int* pFrame)
{
	for (int i = 0; i < CrtStateMax; i++)
	{
		if (m_pStates[i])
			m_pStates[i]->SetAttackFrame(pFrame[i]);
	}
}
//------------------------------------------------------------------------
void CActFSM::SetAllStateAttackSoundFrame(int* pFrame)
{
	for (int i = 0; i < CrtStateMax; i++)
	{
		if (m_pStates[i])
			m_pStates[i]->SetAttackSoundFrame(pFrame[i]);
	}
}
//------------------------------------------------------------------------
void CActFSM::SetMoveDelay(int* pDelay)
{
	if (m_pStates[CrtStateWalk])
		m_pStates[CrtStateWalk]->SetDelay(pDelay[0]);
	
	if (m_pStates[CrtStateRun])
		m_pStates[CrtStateRun]->SetDelay(pDelay[1]);
	
	if (m_pStates[CrtStateRideRun])
		m_pStates[CrtStateRideRun]->SetDelay(pDelay[2]);
}
//------------------------------------------------------------------------