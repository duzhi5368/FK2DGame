/**
*	created:		2012-6-26   0:45
*	filename: 		CClientWorld
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		客户端游戏世界
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IClientWorld.h"
#include "../SceneObj/CThingTable.h"
#include "../Controller/IController.h"
#include "../Event/IEvent.h"
//------------------------------------------------------------------------
class CMainActionDataReceiver : public IDataReceiver
{
public:
	virtual BOOL	OnReceived(const char* pData, int nLen);	
};
//------------------------------------------------------------------------
class CPrivateContextDataReceiver : public IDataReceiver
{
public:
	virtual BOOL	OnReceived(const char* pData, int nLen);
};
//------------------------------------------------------------------------
class CPublicContextDataReceiver : public IDataReceiver
{
public:
	virtual BOOL	OnReceived(const char* pData, int nLen);
};
//------------------------------------------------------------------------
class CChangeMapDataReceiver : public IDataReceiver
{
public:
	virtual BOOL	OnReceived(const char* pData, int nLen);
};
//------------------------------------------------------------------------
class CCountFeeDataReceiver : public IDataReceiver
{
public:
	virtual BOOL	OnReceived(const char* pData, int nLen);
};
//------------------------------------------------------------------------
// 描  述：小退处理
class CReselectHeroActionListener : public IEventActionListener
{
public:
	virtual void OnAction(IEvent * pEvent);
};
//------------------------------------------------------------------------
// 描  述：客户端游戏世界
class CClientWorld : public IClientWorld
{
public:
	// 关闭
	virtual void					Close(void);

	// 消息
	virtual void					OnMessage(DWORD dwKeyMain, LPCSTR buf, int nLen);

	// 取得逻辑对象管理器
	virtual IThingTable *			GetThingTable(void);

	// 取得当前静态地图ID
	virtual int						GetCurStaticMapID(void);

	// 初始化
	BOOL							Init(void);

	// 创建
	BOOL							Create(void);

	CClientWorld(void);

	~CClientWorld(void);

private:
	// 客户端逻辑对像管理列表
	CThingTable						m_ThingTable;
public:
	// 消息通道
	CMainActionDataReceiver			m_MainActionDataReceiver;
	CPrivateContextDataReceiver		m_PrivateContextDataReceiver;
	CPublicContextDataReceiver		m_PublicContextDataReceiver;
	CChangeMapDataReceiver			m_ChangeMapDataReceiver;
	CReselectHeroActionListener		m_ReselectHeroActionListener;
	CCountFeeDataReceiver			m_CountFeeDataReceiver;
};
//------------------------------------------------------------------------