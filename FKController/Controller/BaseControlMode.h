/**
*	created:		2012-6-25   0:50
*	filename: 		BaseControlMode
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		IController会把收到的各种操作委托过来
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CMapItemView;
//------------------------------------------------------------------------
#include "IControlModel.h"
#include "../Finder/MonsterFinder.h"
#include "../ValidationCode/WndMgrSignal.h"
//------------------------------------------------------------------------
class CBaseControlMode : public IControlMode 
{
private:
	BOOL m_bAutoAttack;                             //是否开启了自动打怪
	CMonsterFinder          m_MonsterFinder;        //怪物搜索器
	BOOL                    m_bPressCtrl;			//记录是否按下了Ctrl
public:
	void __SaveScreen(void);

	/*
	清除附在鼠标上的攻击命令, 
	当鼠标上附的是攻击命令,则把命令取消,并返回TRUE
	当鼠标上附的是其它命令,命令不会取消,并返回FALSE
	*/
	BOOL clearAttackCmd();
	/*
	  当用户选择技能(按下F1,等)后,尝试马上释放技能,不用选对像
	  如果成功,返回TRUE   ,成功的条件, 
								1  该技能不用选对像  
								2  上次的选的对像还没死

	  否则返回FALSE
	*/
	BOOL attack_Immediate(int SkillID); 	
	/*
	  用户数字1, 后等按钮时执行
	*/
	void UseSkill(int SkillID);
	/*
	  用户按下 Alt + 数字 等按钮时执行
	*/
	void UseSkill_Self(int SkillID);
	/*
	  处理鼠标上附着的攻击命令
	  输入 ,  点中的CMapItemView, 被点中的Tile
	  输出 ,  处理了鼠标上的命令,返回TRUE, 否则返回FALSE
	*/
	BOOL ProcessCursorAttackCmd(CMapItemView* pItem, POINT ptTileClicked);
	/*
	  处理鼠标上附着的LUA选择命令
	  输入 ,  点中的CMapItemView, 被点中的Tile
	  输出 ,  处理了鼠标上的命令,返回TRUE, 否则返回FALSE
	 */
	BOOL ProcessLUASelectActorCmd(CMapItemView* pItem, POINT ptTileClicked);
	/*
	  检测某个技能, 是否在冷却状态
	  输入 ,  技能ID
	  输出 ,  该技能是否在冷却 
	*/
	BOOL __SkillInColding(int nSkillID);
	/*
	  拾取身边一圈的宝箱
	*/
	void __PickUpTreasure_Round(void);

	//尝试拾取某一座标上的箱子(在__PickUpTreasure_Round() 中调用 )
	//成功打开了合子, 返回TRUE, 否则FALSE
	BOOL __TryPickup(int Tile_X, int Tile_Y);

	//尝试拾取某一个ITEM  (在__TryPickup(int Tile_X, Tile_Y) 中调用 )
	//成功打开了合子, 返回TRUE, 否则FALSE
	BOOL __TryPickupBox(CMapItemView* pItem);
	/*
	  转化一下用户按下的键
	*/
	void __TranslateKeyCode(int *pKeyCode);
	/*
	  检测一下, 点中的是否宝箱
	*/
	BOOL __isTreasureBox(CMapItemView *pItem);

	CWMConnection<CBaseControlMode> m_SkillConnect;				// 使用技能时,这里可收到消息
	CWMConnection<CBaseControlMode> m_SkillSelfConnect;			// 人物跟踪命令的连接
	CWMConnection<CBaseControlMode> m_GoodsConnect;  
	CWMConnection<CBaseControlMode> m_CommandQueueConnect;		// 人物命令队列的连接
	CWMConnection<CBaseControlMode> m_CommandTraceConnect;		// 人物跟踪命令的连接
	CWMConnection<CBaseControlMode> m_CommandSelectTeamate;		// 人物跟踪命令的连接
	CWMConnection<CBaseControlMode> m_CommandAttackTeamate;		// 向队友施法的连接
	CWMConnection<CBaseControlMode> m_CommandPet;				// 执行宠物指令连接
	CWMConnection<CBaseControlMode> m_CommandSetLockFire;		// 设置自动攻击连接
	CWMConnection<CBaseControlMode> m_CommandCancelTarget;		// 取消当前目标连接
	CWMConnection<CBaseControlMode> m_CommandLongMove;			// 长距离移动
	CWMConnection<CBaseControlMode> m_CommandLoadMapBlock;		// 读取地图占位块

public:
	CBaseControlMode();
	virtual ~CBaseControlMode();
	
	virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnRButtonUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnRButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam);

	//窗口模块发过来,要求使用技能
	void OnSkillClickedConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue);
	void OnGoodsClickedConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue);

	//窗口模块发过来,要求清除玩家命令队列 
	void OnClearCommandQueueConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue);

	//窗口模块发过来,要求跟踪某个玩家
	void OnTraceCommandConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue);

	//窗口模块发过来,要求对自己使用技能
	void OnSelfSkillConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue);
	
	//不知从哪里发过来, 说选中另一目标
	void OnSelectNewTargetCommandConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue);

	//不知从哪里发过来, 说向队友施法
	void OnAttackTeamateCommandConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue);

	//不知从哪里发过来, 取消当前目标环
	void OnCancelTargetCommandConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue);

	//窗口发过来, 执行宠物指令
	void OnExecutePetCommandConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue);

	//窗口发过来, 设置"自动攻击"是否开启
	void OnSetLockFireState( WPARAM wParam, LPARAM lParam,BOOL &bContinue);

	//窗口发过来, 执行长距离移动指令
	void OnLongmoveCommandConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue);

	//窗口发过来, 读取地图占位块信息
	void OnLoadMapBlockCommandConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue);

	void ConnectSingal(void);
	void DisConnectSingal(void);
};
//------------------------------------------------------------------------