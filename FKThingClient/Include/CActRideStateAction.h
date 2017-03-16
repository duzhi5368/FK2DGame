/**
*	created:		2012-7-4   1:25
*	filename: 		CActRideStateAction
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		响应在此状态下，各种行为事件的发生
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/ICreatureActState.h"
//------------------------------------------------------------------------
class CCreatureActRideState : public ICreatureActState
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

	CCreatureActRideState(void);

	~CCreatureActRideState(void);

private:
	// MAKE_MSGKID(MAINACTION_CIRCULTYPE_IC, MAINACTION_MSGID_MOVE)
	BOOL						OnMove_IC_Message(LPCSTR buf, int nLen);

	// MAKE_MSGKID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_DIE)
	BOOL						OnDie_CSC_Message(LPCSTR buf, int nLen);

private:
	ICreature	*				m_pMaster;

public:
	// 消息过滤函数
	typedef BOOL (CCreatureActRideState::*FILTER_MESSAGE_PROC)(LPCSTR buf, int nLen);
	static  FILTER_MESSAGE_PROC	m_FilterMessageProc[0xFFFF + 1];

	// 初始化消息映射
	static	void				InitFilterMessageProc(void);
};
//------------------------------------------------------------------------