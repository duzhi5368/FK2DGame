/**
*	created:		2012-7-4   0:48
*	filename: 		CreaturePartConjure
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		召唤部件
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/IConjurePart.h"
#include "../../FKGame/Event/IEvent.h"
//------------------------------------------------------------------------
class CCreaturePart_Conjure : public IConjurePart,
							  public IEventActionListener
{
public:
	// 创建
	virtual BOOL				Create(IThing * pMaster, char * pContext, int nLen);

	// 释放
	virtual void				Release(void);

	// 取得部件ID
	virtual DWORD				GetPartID(void);

	// 取得本身生物
	virtual IThing *			GetMaster(void);

	// 激活
	virtual BOOL				Activation(LPCSTR pContext, int nLen);

	// 冻结
	virtual BOOL				Freeze(void);

	// 取得私有对象现场
	virtual BOOL				OnGetPrivateContext(LPSTR buf, int &nLen);

	// 取得公用对象现场
	virtual BOOL				OnGetPublicContext(LPSTR buf, int &nLen);	

	// 取得数据库现场
	virtual BOOL				OnGetDBContext(LPSTR buf, int &nLen);

	// Message
	virtual	int					OnMessage(DWORD dwMsg, LPCSTR buf, int nLen);

	// 初始化客户端
	virtual BOOL				InitClient(UID uidClient);

	// 取得首领
	virtual UID					GetChief(void);

	// 能否找到
	virtual BOOL				FindPawn(UID uidPawm);
	
	// 取得爪牙列表
	virtual list< UID >			GetPawn(void);
 
	virtual void				OnAction(IEvent *pEvent);
public:
	CCreaturePart_Conjure(void);
	~CCreaturePart_Conjure(void);

private:
	// 首领
	UID							m_uidChief;
	// 爪牙列表
	list< UID >					m_listPawn;
	// 主人
	ICreature *					m_pMaster;
};
//------------------------------------------------------------------------