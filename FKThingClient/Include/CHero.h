/**
*	created:		2012-7-3   1:42
*	filename: 		CHero
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		主角
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "CActor.h"
#include "../../FKGame/Common/CreatureViewDef.h"
#include "../../FKGame/SceneObj/IThingPart.h"
#include "CCreatureActStateMachine.h"
//------------------------------------------------------------------------
class CHero : public CActor
{
public:
	// 释放
	virtual void				Release(void);

	// 	私有对象现场管理
	virtual BOOL				OnSetPrivateContext(LPCSTR buf, int nLen);

	// 消息通知, 驱使逻辑对像进行运转
	virtual void				OnMessage(DWORD dwMsg, LPCSTR buf, int nLen);

	// 执行命令
	virtual int					OnCommand(DWORD dwCommand, LPCSTR buf = NULL, int nLen = 0);

	// 添加生物部件, 如果已存在,则会添加失败
	virtual BOOL				AddPart(IThingPart * pPart);
	
	// 删除生物部件, 并不释放
	virtual BOOL				RemovePart(DWORD dwPartID);
	
	// 取得生物部件
	virtual IThingPart *		GetPart(DWORD dwPartID);

	// 设置当前行动状态
	virtual BOOL				GotoActState(DWORD dwActStateID, LPCSTR pContext = NULL, int nLen = 0);

	// 取得当前行动状态
	virtual ICreatureActState * GetCurActState(void);

	// 设置生物的数字型属性	
	virtual int					SetPropNum(DWORD dwPropID, int nValue);

	// 取得生物的数字型属性
	virtual int					GetPropNum(DWORD dwPropID);

	// 设置生物的字符型属性	
	virtual LPCSTR				SetPropStr(DWORD dwPropID, LPCSTR strPropValue);

	// 取得生物的字符型属性
	virtual LPCSTR				GetPropStr(DWORD dwPropID);

	// 增加一生物命令
	virtual BOOL				AddCreatureCommand(ICreatureCommand * pCreatureCommand);

	// visitor
	virtual void				Accept(IMapItemVisitor &visitor);

	// 设置处在死亡状态
	virtual void				SetDying(BOOL bDying);

	// 是否正处在死亡
	virtual BOOL				IsDying(void);


	virtual void				OnCreatureMoveToTileCenter(const POINTS &ptCurTile, 
														   const POINTS &ptNextTile);
public:	
	// 创建
	BOOL						Create(void);

	CHero(void);
	~CHero(void);

private:
	ICreatureCommand *			m_CreatureCommand[CREATURE_COMMANDID_MAXID];
	CCreatureActStateMachine	m_CreatureActStateMachine;	
};
//------------------------------------------------------------------------