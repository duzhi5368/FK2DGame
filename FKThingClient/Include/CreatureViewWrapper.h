/**
*	created:		2012-7-2   23:57
*	filename: 		CreatureViewWrapper
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		生物显示包装类
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/Timer/ITimerAxis.h"
#include "../../FKGame/Common/CreatureViewDef.h"
#include "../../FKGame/MapView/IMapView.h"
#include "../../FKGame/MapView/MapItemView.h"
#include "../../FKCanvas/IFKCanvas.h"
//------------------------------------------------------------------------
// 申明
struct stBehitParam;
//------------------------------------------------------------------------
// 定义
class CCreatureViewWrapper
{
public:	
	CMapItemView * operator ->(void);

	BOOL				IsValid(void);
	BOOL				AddToMap(POINTS ptTile);

	CMapItemView *		GetItemView(void);

	void				SetData(DWORD dwData);
	void				AddFlag(DWORD dwFlag);
	void				RemoveFlag(DWORD dwFlag);

	// 移动类型
	DWORD				GetMoveStyle(void);
	void				SetMoveStyle(DWORD dwStyle);

	// 生物的TILE坐标
	POINT				GetTile(void);
	void				SetTile(POINTS ptTile);

	// 生物的名字
	void				GetName(LPSTR szBuf);
	void				SetName(LPCSTR szName);

	// 名字的颜色
	void				SetNameColor(int r, int g, int b);
	void				SetNameColor(DWORD dwColor);

	// 生物状态
	int					GetState(void);
	void				SetState(int nState);

	// 是否让生物显示自己的名字
	BOOL				GetPropShowName(void);
	void				SetPropShowName(BOOL bShow);

	// 生物的绝对位置(ptTileCenter+ptOffset)
	POINT				GetPosition(void);
	void				SetPosition(POINT ptPos);

	// 生物的方向
	int					GetDirection(void);
	void				SetDirection(int nDir);

	// 角色携带武器的方式(空手武器?单手武器?双手武器?)
	DWORD				GetPropWeaponStyle(void);
	void				SetPropWeaponStyle(DWORD dwStyle);

	// 生物是否正在移动?
	BOOL				IsMoving(void);
	// 移动命令,以当前的移动方式移动
	void				Move(POINTS * pPath, int nPathLen);
	// 停止移动站立在某个Tile上
	void				Stand(POINT ptTile);	
	// 用技能顺移到某个Tile上
	void				ChangeLocForSkill(POINT ptTile);	
	// 坐下
	void				Sit(void);
	// 生物被击
	void				BeHit(SBehitParam * pParam);
	// 生物死亡
	void				Die(DWORD dwParam1, DWORD dwParam2);
	// 生物出生
	void				Born(void);
	// 人物升级
	void				Upgrade(void);
	// 生物死亡
	void				Fallow(void);
	// 换装，变换生物的身体
	void				ChangeModule(DWORD dwModule, DWORD dwResID);
	// 改变各组件的调色板类型索引
	void				ChangeModulePal(DWORD dwModule, DWORD dwPalType);
	// 指定在部件找不到图包时，用特定的图包代替
	void				SpecialModule(DWORD dwModule, DWORD dwResID);
	// 停止攻击
	void				StopAttack(void);
	// 使用魔法(技能)
	void				Attack(DWORD pParam);	
	// 停止蓄气
	void				StopPrepAttack(void);
	// 蓄气
	void				PrepAttack(DWORD pParam);
	// 切换到一个状态
	void				ChangeToState(int nState, BOOL bFlag);
	// 取消魔法状态
	void				DestroyMagic(DWORD dwParam);

	// 设置移动速度等级(0->8)
	void				SetMoveSpeedLevel(int * pSpeed);
	int                 GetMoveSpeedLevel(void);
	// 设置攻击速度
	void				SetAttackSpeed(int * pSpeed);
	// 设置血量
	void				SetBloodInfo(int nNowBlood, int nTotalBlood);
	// 设置聊天文字
	void				SetChatInfo(LPCSTR szMsg, WPixel * pColor);
	// 设置帮派信息
	void				SetTribeTitle(IBitmap * pBitmap);
	// 设置称号
	void				SetTitle(LPCSTR szMsg, DWORD dwEffectID);
	// 设置武器的攻击音效ID
	void				SetWeaponAttackSoundID(int nSoundID);
	// 设置武器的击中音效ID
	void				SetWeaponBeHitSoundID(int nSoundID);
	// 设置小地图颜色
	void				SetFlagColorOnMinmap(DWORD dwColor);
	void				SetFlagColorOnMinmap(int r, int g, int b);

	CCreatureViewWrapper(CMapItemView * pItemView);

private:
	CMapItemView *			m_pItemView;
};
//------------------------------------------------------------------------