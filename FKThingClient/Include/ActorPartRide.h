/**
*	created:		2012-7-4   0:38
*	filename: 		ActorPartRide
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		人物骑乘部件
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IRidePart.h"
#include "IHorse.h"
#include "CHorse.h"
#include "../../FKGame/Event/IEvent.h"
#include <list>
using std::list;
//------------------------------------------------------------------------
class CActorPart_Ride : public IRidePart,
						public IEventActionListener
{
	struct HORSEINFOR
	{
		int		nIndex;
		CHorse	horse;
	};
	typedef list< HORSEINFOR >	TLIST_HORSE;
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

	// 取得马匹
	virtual IHorse *		GetHorse(int nIndex);

	// 是否正在骑马
	virtual BOOL			IsRiding(void);


	virtual void			OnAction(IEvent * pEvent);

public:
	CActorPart_Ride(void);
	virtual ~CActorPart_Ride(void);

protected:
	// 骑
	virtual void			Mount(SC_MSG_GETRIDEDATA_CONTEXT::HORSEDATA horse);
	// 下
	virtual void			Dismount(void);

private:
	// 上马
	virtual void			OnAction_Mount(IEvent * pEvent);
	// 下马
	virtual void			OnAction_Dismount(IEvent * pEvent);
	// 取得骑乘数据
	virtual void			OnAction_GetRideData(IEvent * pEvent);

	virtual void			OnMessage_Show(LPCSTR buf, int nLen);
protected:
	IActor *				m_pMaster;
	TLIST_HORSE				m_listHorse;		// 拥有马匹列表	
	int						m_nCurMountIndex;	// 当前骑的index, 为0表示未骑
	int						m_nOldPart1_no;
	int						m_nOldPart2_no;
};
//------------------------------------------------------------------------