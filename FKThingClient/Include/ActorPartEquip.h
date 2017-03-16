/**
*	created:		2012-7-4   0:24
*	filename: 		ActorPartEquip
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		别的玩家装备部件
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/Container/IContainerHolder.h"
#include "../../FKGame/SceneObj/IActor.h"
#include "../../FKGame/Event/IEvent.h"
#include "../../FKGame/Common/CreatureViewDef.h"
//------------------------------------------------------------------------
class CActorPart_Equip : public IContainerHolder,
						 public IEventActionListener,
						 public IEventResponseListener
{
public:
	// 创建
	virtual BOOL			Create(IThing * pMaster, char * pContext, int nLen);
	
	// 释放
	virtual void			Release(void);
	
	// 取得部件ID
	virtual DWORD			GetPartID(void);
	
	// 取得本身生物
	virtual IThing *		GetMaster(void);
	
	// 激活
	virtual BOOL			Activation(LPCSTR pContext, int nLen);
	
	// 冻结
	virtual BOOL			Freeze(void);

	// 取得私有对象现场，发给自已的客户端
	virtual BOOL			OnGetPrivateContext(LPSTR buf, int &nLen);
	
	// 取得公用对象现场，广播给客户端	
	virtual BOOL			OnGetPublicContext(LPSTR buf, int &nLen);

	// 取得数据库现场
	virtual BOOL			OnGetDBContext(LPSTR buf, int &nLen);

	// Message
	virtual	int				OnMessage(DWORD dwMsg, LPCSTR buf, int nLen);

	// 初始化客户端
	virtual BOOL			InitClient(UID uidClient);


	virtual IContainer *	GetContainer(void);

	// 是否存在（能通过物品id，容器内是否达到了要求的个数）
	virtual BOOL			IsExistByGoodsID(DWORD dwGoodsID, DWORD dwNum);

	// 获取此物品id容器内的个数
	virtual DWORD			GetNumByGoodsID(DWORD dwGoodsID);

	// 此容器内是否能销毁个数
	virtual BOOL			IsRemoveByGoodsID(DWORD dwGoodsID, DWORD dwNum);

	// 此容器内能销毁的个数
	virtual DWORD			GetRemoveNumByGoodsID(DWORD dwGoodsID);

	virtual void			OnAction(IEvent * pEvent);

	virtual void			OnResponse(IEvent *pEvent);

public:
	CActorPart_Equip(void);
	virtual ~CActorPart_Equip(void);

private:
	IActor *				m_pMaster;	
};
//------------------------------------------------------------------------