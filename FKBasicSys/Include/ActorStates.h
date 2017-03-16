/**
*	created:		2012-7-2   3:45
*	filename: 		ActorsState
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		玩家状态管理
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CActorView;
//------------------------------------------------------------------------
// 通用状态
class CActState
{
public:
	CActState();
	virtual ~CActState();

	virtual void Start();
	virtual void End();
	virtual void Stop()	{};

	virtual void OnTimer(DWORD dwEventID);

	void SetDelay(int nDelay);
	int  GetDelay();

	void SetAttackFrame(int nFrame);	
	void SetAttackSoundFrame(int nFrame);

	void DelAllAttribute();				// 删除所有附加属性

	void SetPlayCommondAttackSound();	// 附加普通攻击音效在该动作上
	void SetMagicCheckID(int nCheckID);	// 附加魔法在该动作上
	void SetPreAttack();				// 设置蓄气属性
	void SetAttackOnce();				// 设置一次性攻击属性
	void SetAttackCtrl();				// 设置持续攻击属性

public:
	CActorView		*m_pAct;
	int				m_nDelay;			// 每个状态的动画延时
	int				m_nAttackFrame;		// 攻击祯
	int				m_nAttackSoundFrame;// 攻击音效祯

	// 附加属性
	int				m_nMagicCheckID;	// 当前状态对应的技能较检ID
	BOOL			m_bPreAttack;		// 蓄气属性
	BOOL			m_bAttackOnce;		// 一次性攻击属性
	BOOL			m_bAttackCtrl;		// 持续攻击属性
	
	BOOL			m_bPlayCommondAttackSound;	// 是否播放普通攻击音效
};
//------------------------------------------------------------------------
// 移动状态
class CActStateMove : public CActState
{
public:
	virtual void Start();
	virtual void End();
	virtual void OnTimer(DWORD dwEventID);
};
//------------------------------------------------------------------------
// 休闲状态
class CActStateFallow : public CActState
{
public:
	virtual void OnTimer(DWORD dwEventID);
};
//------------------------------------------------------------------------
// 死亡状态
class CActStateDie : public CActState
{
public:
	virtual void OnTimer(DWORD dwEventID);
};
//------------------------------------------------------------------------
// 后仰状态
class CActStateBehit : public CActState
{
public:
	virtual void Start();
	virtual void OnTimer(DWORD dwEventID);
	virtual void Stop();
	
public:
	int m_nState;
	int m_nBeHitLoopBeginFrame;
	int m_nBeHitLoopEndFrame;
};
//------------------------------------------------------------------------
// 攻击状态
class CActStateAttack : public CActState
{
public:
	virtual void OnTimer(DWORD dwEventID);
};
//------------------------------------------------------------------------
// 状态机
class CActFSM
{
protected:
	CActState*			m_pCurrentState;
	CActState*			m_pStates[CrtStateMax];

	CActState			m_StateStand;				//0		站立	
	CActState			m_StateRideStand;			//17	骑马站立			
	CActState			m_StateSit;					//6		坐下

	CActStateBehit		m_StateBehit;				//5		被击
	CActStateDie		m_StateDie;					//3		死亡

	CActStateMove		m_StateWalk;				//1		行走
	CActStateMove		m_StateRun;					//2		奔跑
	CActStateMove		m_StateRideRun;				//18	骑马奔跑

	CActStateFallow		m_StatePreFallow;			//4		休闲
	CActState			m_StatePre1;				//7		聚气1
	CActState			m_StatePre2;				//8		聚气2
	
	CActState			m_StateMagic1;				//9		施法1
	CActState			m_StateMagic2;				//10	施法2
	CActStateAttack		m_StateSingleAttack1;		//11	单手武器攻击1
	CActStateAttack		m_StateSingleAttack2;		//12	单手武器攻击2
	CActStateAttack		m_StateSingleAttack3;		//13	单手武器攻击3
	CActStateAttack		m_StateDoubleAttack1;		//14	双手武器攻击1
	CActStateAttack		m_StateDoubleAttack2;		//15	双手武器攻击2
	CActStateAttack		m_StateArrowAttack;			//16	弓箭武器攻击
	CActStateAttack		m_StateCommonAttack1;		//19	普通攻击1
	CActStateAttack		m_StateCommonAttack2;		//20	普通攻击2
	CActStateAttack		m_StateSkillAttack1;		//21	技能攻击1
	CActStateAttack		m_StateSkillAttack2;		//22	技能攻击2

public:
	CActFSM();
	virtual ~CActFSM();
	bool Create(CActorView* pAct);

	bool IsState(CActState& State);
	bool GotoState(CActState& NewState);
	void SetAllStateDelay(int* pDelay);
	void SetAllStateAttackFrame(int* pFrame);
	void SetAllStateAttackSoundFrame(int* pFrame);
	void SetMoveDelay(int* pDelay);
	CActState* GetCurState();

	virtual void OnTimer(DWORD dwEventID);
};
//------------------------------------------------------------------------