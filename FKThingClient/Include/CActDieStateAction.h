/**
*	created:		2012-7-4   1:19
*	filename: 		CActDieStateAction
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		生物死亡走路状态，响应事件
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/ICreatureActState.h"
//------------------------------------------------------------------------
class CCreatureActDieState : public ICreatureActState
{
	
public:
	// 创建
	virtual BOOL				Create(ICreature * pCreature);
	
	// 取得本状态ID
	virtual DWORD				GetStateID(void);

	// 消息过滤
	virtual BOOL				OnMessage(DWORD dwMsg, LPCSTR buf, int nLen);

	// 状态开始,订阅事件
	virtual void				Start(LPCSTR pContext, int nContextLen);
	
	// 状态结束,取消订阅事件
	virtual void				End(LPCSTR pContext, int nContextLen);

	// 显示层事件
	virtual BOOL				OnMoveItem(ICreature * pCreature, POINT ptTile);

	CCreatureActDieState(void);

	~CCreatureActDieState(void);

private:
	// MAKE_MSGID(MAINACTION_CIRCULTYPE_XXX, MAINACTION_MSGID_XXX)
	BOOL						OnGeneral_Vote_Message(LPCSTR buf, int nLen);

private:
	ICreature	*				m_pMaster;

public:
	// 消息过滤函数
	typedef BOOL (CCreatureActDieState::*FILTER_MESSAGE_PROC)(LPCSTR buf, int nLen);
	static  FILTER_MESSAGE_PROC	m_FilterMessageProc[0xFFFF + 1];

	// 初始化消息映射
	static	void				InitFilterMessageProc(void);
};
//------------------------------------------------------------------------