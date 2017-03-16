/**
*	created:		2012-7-2   3:52
*	filename: 		ActorView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		玩家视图管理
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "CreatureView.h"
#include "FrameControl.h"
#include "ActorStates.h"
#include "ActorTitle.h"
//------------------------------------------------------------------------
// ActorView存档格式
struct SActorViewArchive
{
	DWORD	dwActTypeID;
};
//------------------------------------------------------------------------
class CActorView : public CCreatureView, public CFrameControl, public CActFSM
{
public:
	CActorView();
	virtual ~CActorView();

	void Close();
	virtual void	Release();

public:
	virtual void	OnTimer(DWORD dwEventID);

public:
	virtual DWORD	GetClassID() const;										// 获取对象的类ID

	virtual BOOL	OnSerialize(BOOL bWrite,IArchive* pArchive);			// 保存或读取数据，用于从文件中创建对象
	virtual BOOL	OnCreateItem(const char* buf, int nLen);				// 根据数据现场创建对象
	virtual void	OnUpdate();												// 更新(如：重载脚本数据等)

	virtual DWORD	GetOccupantValue();										// 获取单占位对象的占位值
	virtual DWORD	GetLayerValue();										// 获取对象在地图中的显示层
	
	virtual void	SetDirty(ICanvas* pCanvas, const POINT& ptViewTop);		// 设脏
	virtual void	GetOldDirtyRect(list<RECT> &DirtyRectList);				// 获取旧的脏矩阵，主要用在把对象从地图上删除时获取它的脏矩阵

	virtual IBitmap	*GetBitmap() {return NULL;}								// 获取当前显示的图像
	virtual void	OnDraw(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag = 0);				// 绘图循环请求对象绘制到指定的ICanvas上
	virtual void	OnDrawTopmostObject(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag=0);	// 绘制其他一些顶层对象，如血槽、聊天内容、物品名等，这些对象永远放在最上层

public:
	// 生物与外界的交互命令
	virtual int	cmdChangeModule(DWORD dwParam1, DWORD dwParam2);			// 换装
	virtual int cmdSpecialModule(DWORD dwPam1, DWORD dwParam2);				// 设置角色默认部件
	virtual int	cmdModulePalType(DWORD dwParam1, DWORD dwParam2);			// 角色各组件的调色板类型索引
	virtual int	cmdModuleAlpha(DWORD dwParam1, DWORD dwParam2);				// 改变角色透明度

	virtual int	cmdBorn(DWORD dwParam1, DWORD dwParam2);					// 角色出生
	virtual int	cmdUpgrade(DWORD dwParam1, DWORD dwParam2);					// 角色升级

	virtual int	cmdMouseAni(DWORD dwParam1, DWORD dwParam2);				// 主角行走时，在路上点击鼠标留下的点击动画

	virtual int cmdAddTitle(DWORD dwParam1, DWORD dwParam2);				// 给生物添加称号
	virtual int cmdClearTitle(DWORD dwParam1, DWORD dwParam2);				// 清除生物的所有称号

	// 设置和获取生物属性
	virtual void propWeaponStyle(BOOL bSetProp, DWORD dwPropVal);			// 角色携带武器的方式
	virtual void propWeaponSoundID(BOOL bSetProp, DWORD dwPropVal);			// 角色武器的攻击音效ID
	virtual void propBeHitSoundID(BOOL bSetProp, DWORD dwPropVal);			// 角色武器的击中目标音效ID
	virtual void propGroupInfoImage(BOOL bSetProp, DWORD dwPropVal);		// 帮会信息图片

public:

	// 状态相关
	virtual void SetState(int nState);										// 设置当前状态
	virtual void StopState(int nState);										// 停止当前状态
	virtual void RestartCurState();											// 重启当前状态

	// 动画上下文相关
	virtual int  GetNormalAttackActID();									// 获取普通攻击动作ID
	virtual int	 GetJuqiActID();											// 获取默认的蓄气动作ID（变身后有效）
	virtual int	 GetSkillAttackActID();										// 获取默认的技能攻击动作ID（变身后有效）
	virtual void SetShowRect();												// 设置对象矩形大小范围

	// 移动相关
	virtual void SetAllMoveDelay(int* pDelay);								// 设置移动的祯延时

	// 吸附对象相关
	virtual void OnAttachObjDestroy(IAttachObj* pAttachObj);				// 回调(跟随对象销毁时回调)
	virtual void OnFlowObjDestroy(IAttachObj* pAttachObj);					// 回调(飘字对象销毁时回调)

	virtual void RemoveAllAttachObj();										// 删掉所有吸附对象

	// 法术管理相关
	virtual int	 GetWeaponStyle();											// 获取武器类型

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
	BOOL CreateItem(SActorViewArchive* pArchive);							// 根据现场创建生物
	virtual int CreateFootprint();											// 创建脚印

public:
	IBitmap*		m_pGroupInfoImage;				// 帮会，团体信息图片(外部释放)
	CActorTitle		m_ActorTitle;					// 角色称号
	int				m_nFootprintLeftRight;			// 左右脚印
	int				m_nWeaponStyle;					// 武器类型
	list<RECT>		m_AttachDirtyList;				// 跟随对象的脏矩阵链表
};
//------------------------------------------------------------------------