/**
*	created:		2012-7-2   3:35
*	filename: 		MonsterStates
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		怪物状态管理
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CMonsterView;
//------------------------------------------------------------------------
// 通用状态
class CMstState
{
public:
	CMstState();
	virtual ~CMstState();
	
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
	CMonsterView	*m_pMst;
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
class CMstStateMove : public CMstState
{
public:
	virtual void Start();
	virtual void End();
	virtual void OnTimer(DWORD dwEventID);
};
//------------------------------------------------------------------------
// 站立状态
class CMstStateStand : public CMstState
{
public:
	virtual void OnTimer(DWORD dwEventID);
};
//------------------------------------------------------------------------
// 休闲状态
class CMstStateFallow : public CMstState
{
public:
	virtual void Start();
	virtual void OnTimer(DWORD dwEventID);
};
//------------------------------------------------------------------------
// 死亡状态
class CMstStateDie : public CMstState
{
public:
	virtual void Start();
	virtual void OnTimer(DWORD dwEventID);
};
//------------------------------------------------------------------------
// 后仰状态
class CMstStateBehit : public CMstState
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
// 状态机
class CMstFSM
{
protected:
	CMstState*			m_pCurrentState;
	CMstState*			m_pStates[CrtStateMax];

	CMstStateStand		m_StateStand;			//0		站立
	CMstState			m_StateRideStand;		//17	骑马站立			
	CMstState			m_StateSit;				//6		坐下

	CMstStateBehit		m_StateBehit;			//5		被击
	CMstStateDie		m_StateDie;				//3		死亡

	CMstStateMove		m_StateWalk;			//1		行走
	CMstStateMove		m_StateRun;				//2		奔跑
	CMstStateMove		m_StateRideRun;			//18	骑马奔跑

	CMstStateFallow		m_StateFallow;			//4		休闲
	CMstState			m_StatePre1;			//7		聚气1
	CMstState			m_StatePre2;			//8		聚气2

	CMstState			m_StateMagic1;			//9		施法1
	CMstState			m_StateMagic2;			//10	施法2
	CMstState			m_StateSingleAttack1;	//11	单手武器攻击1
	CMstState			m_StateSingleAttack2;	//12	单手武器攻击2
	CMstState			m_StateSingleAttack3;	//13	单手武器攻击3
	CMstState			m_StateDoubleAttack1;	//14	双手武器攻击1
	CMstState			m_StateDoubleAttack2;	//15	双手武器攻击2
	CMstState			m_StateArrowAttack;		//16	弓箭武器攻击
	CMstState			m_StateCommonAttack1;	//19	普通攻击1
	CMstState			m_StateCommonAttack2;	//20	普通攻击2
	CMstState			m_StateSkillAttack1;	//21	技能攻击1
	CMstState			m_StateSkillAttack2;	//22	技能攻击2

public:
	CMstFSM();
	virtual ~CMstFSM();
	bool Create(CMonsterView* pMst);

	bool IsState(CMstState& State);
	bool GotoState(CMstState& NewState);
	void SetAllStateDelay(int* pDelay);
	void SetAllStateAttackFrame(int* pFrame);
	void SetAllStateAttackSoundFrame(int* pFrame);
	void SetMoveDelay(int* pDelay);
	CMstState* GetCurState();

	virtual void OnTimer(DWORD dwEventID);
};
//------------------------------------------------------------------------