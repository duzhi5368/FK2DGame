/**
*	created:		2012-7-2   3:34
*	filename: 		MonsterView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		怪物视图管理
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "CreatureView.h"
#include "MonsterStates.h"
#include "FrameControl.h"
#include "../../FKGame/Common/MapItemDef.h"
//------------------------------------------------------------------------
class CMonsterView : public CCreatureView, public CMstFSM, public CFrameControl
{
public:
	CMonsterView();
	virtual ~CMonsterView();

	void Close();
	void ClearAll();
public:
	virtual void	OnTimer(DWORD dwEventID);

public:
	virtual DWORD	GetClassID() const;										// 获取对象的类ID

	virtual BOOL	OnSerialize(BOOL bWrite,IArchive* pArchive);			// 保存或读取数据，用于从文件中创建对象
	virtual BOOL	OnCreateItem(const char* buf, int nLen);				// 根据数据现场创建对象
	virtual void	OnUpdate();												// 更新(如：重载脚本数据等)

	virtual DWORD	GetOccupantValue();										// 获取单占位对象的占位值
	virtual COccupantTileList * GetOccupantTileList(void) const;			// 读取占位块列表，默认为无占位位列表
	virtual DWORD	GetLayerValue();										// 获取对象在地图中的显示层
	
	virtual void	SetDirty(ICanvas* pCanvas, const POINT& ptViewTop);		// 设脏
	virtual void	GetOldDirtyRect(list<RECT> &DirtyRectList);				// 获取旧的脏矩阵，主要用在把对象从地图上删除时获取它的脏矩阵

	virtual IBitmap	*GetBitmap() {return NULL;}								// 获取当前显示的图像
	virtual void	OnDraw(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag = 0);				// 绘图循环请求对象绘制到指定的ICanvas上
	virtual void	OnDrawTopmostObject(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag=0);	// 绘制其他一些顶层对象，如血槽、聊天内容、物品名等，这些对象永远放在最上层

public:
	// 生物与外界的交互命令
	virtual int cmdDie(DWORD dwParam1, DWORD dwParam2);						// 生物死亡
	virtual int	cmdBorn(DWORD dwParam1, DWORD dwParam2);					// 角色出生

	virtual int cmdSetNpcType(DWORD dwParam1, DWORD dwParam2);				// 设置NPC类别对应的头顶光效
	virtual int cmdSetTaskType(DWORD dwParam1, DWORD dwParam2);				// 任务NPC头顶的光效
public:
	// 状态相关
	virtual void SetState(int nState);										// 设置当前状态
	virtual void StopState(int nState);										// 停止当前状态
	virtual void RestartCurState();											// 重启当前状态

	// 动画上下文相关
	virtual int  GetNormalAttackActID();									// 获取普通攻击动作ID
	virtual int	 GetJuqiActID();											// 获取默认的蓄气动作ID（非玩家有效）
	virtual int	 GetSkillAttackActID();										// 获取默认的技能攻击动作ID（非玩家有效）

	virtual void SetShowRect();												// 设置对象矩形大小范围

	// 移动相关
	virtual void SetStepWalk(float fStep);									// 设置生物的行走步长(以竖直方向为准)
	virtual void SetStepRun(float fStep);									// 设置生物的跑动步长(以竖直方向为准)
	virtual void SetStepRide(float fStep);									// 设置生物的骑马跑动步长(以竖直方向为准)
	virtual void SetAllMoveDelay(int* pDelay);								// 设置移动的祯延时

	// 吸附对象相关
	virtual void OnAttachObjDestroy(IAttachObj* pAttachObj);				// 回调(跟随对象销毁时回调)
	virtual void OnFlowObjDestroy(IAttachObj* pAttachObj);					// 回调(飘字对象销毁时回调)

	virtual void RemoveAllAttachObj();										// 删掉所有吸附对象							// 删掉所有吸附对象

	// 法术管理相关
	virtual void SetStatusMagicCheckID(int nCheckID);						// 设置目前状态下对应的技能ID
	virtual void SetStatusPlayCommondAttackSound();							// 设置目前状态下要播放普通攻击声音
	virtual void SetStatusPreAttack();										// 设置目前状态下的蓄气属性
	virtual void SetStatusAttackOnce();										// 设置目前状态下的一次性攻击属性
	virtual void SetStatusAttackCtrl();										// 设置目前状态下的持续攻击属性

	virtual void OnDrawEx(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag,
				SAniContext *pContext, BYTE nAlpha = 0);					// 为了残影做的


	// 变身相关
	virtual BOOL ChangeShapEX();											// 变身

	// 其他
	BOOL CreateItem(SMonsterViewArchive* pArchive);							// 根据现场创建生物

private:
	int			m_nShape;						// 怪物外形
	COccupant	m_MstOccInfo;					// 怪物多站位处理
	list<RECT>	m_AttachDirtyList;				// 跟随对象的脏矩阵链表
	int			m_nCorperShowTime;				// 怪物尸体消失动画控制
	int			m_nFallowStateID;				// 怪物的休闲动作ID
	IAttachObj*	m_pHeadAttachObj;				// 头顶光效
	int			m_nNpcType;						// NPC类别对应的头顶光效
	int			m_nTaskType;					// 任务NPC头顶的光效
};
//------------------------------------------------------------------------