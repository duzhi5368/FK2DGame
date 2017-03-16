/**
*	created:		2012-6-23   0:19
*	filename: 		IThing
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IThingClass.h"
#include "../Common/Macro.h"
//------------------------------------------------------------------------
// 申明
class  IEventVoteListener;
class  IEventActionListener;
class  IEventResponseListener;
//------------------------------------------------------------------------
// 定义接口
struct IThing
{
	// 释放
	virtual void			Release(void) = NULL;
	// 设置UID
	virtual void			SetUID(UID uid) = NULL;
	// 取得UID
	virtual UID				GetUID(void) = NULL;
	// 取得地图ID, 也就是根据UID解析出来的
	virtual DWORD			GetMapID(void) = NULL;
	// 设置位置
	virtual void			SetLoc(POINTS ptLoc) = NULL;
	// 取得位置
	virtual POINTS			GetLoc(void) = NULL;
	// 取得类型
	virtual IThingClass *	GetThingClass(void) = NULL;

	/* 
	私有对象现场管理
	说明：
	私有对象现场是指用户自己的角色或装备等的详细信息，其它人是看不到的
	以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端
	如：用户登录时，服务器取得人物对象现场
	客户端根据dwClassID创建对象，调用OnGetPrivateContext()设置对象现场
	*/
	virtual BOOL			OnSetPrivateContext(LPCSTR buf, int nLen) = NULL;
	virtual BOOL			OnGetPrivateContext(LPSTR buf, int &nLen) = NULL;
	/*
	对象公开现场管理
	说明：
	公开现场为可以让所有GameWorld中的其它对象查看的对象信息，如角色名、HP、装备形状等
	服务器取得对象现场，以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端，
	客户端根据dwThingClassID创建对象，调用OnSetContext()设置对象现场	
	*/
	virtual BOOL			OnSetPublicContext(LPCSTR buf, int nLen) = NULL;
	virtual BOOL			OnGetPublicContext(LPSTR buf, int &nLen) = NULL;
	
	// 执行命令
	virtual int				OnCommand(DWORD dwCommand, LPCSTR buf = NULL, int nLen = 0) = NULL;
	// 消息通知, 驱使逻辑对像进行运转
	virtual void			OnMessage(DWORD dwMsg, LPCSTR buf, int nLen) = NULL;
	// 触发响应
	virtual void			OnResponse(DWORD dwMsg, LPCSTR buf, int nLen) = NULL;

	// 描  述: 订阅本对象的否决事件
	// 参  数: [输入] DWORD dwMsg - 事件ID
	// 参  数: [输入] IEventVoteListener * pListener - 事件否决者
	// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
	// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
	virtual BOOL			SubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener, const char szDescription[32] = NULL) = NULL;

	// 描  述：取消订阅本对象的否决事件,不会delete pListener指针
	// 参  数：[输入] DWORD dwMsg  - 事件ID
	// 参  数：[输入] IEventVoteListener * pListener  - 事件否决者	
	// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
	virtual BOOL			UnSubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener) = NULL;

	// 描  述: 订阅本对象的行为事件
	// 参  数: [输入] DWORD dwMsg - 事件ID
	// 参  数: [输入] IEventActionListener * pListener - 事件行为执行者
	// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
	// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE	
	virtual BOOL			SubscibeMessage(DWORD dwMsg, IEventActionListener * pListener, const char szDescription[32] = NULL) = NULL;
	
	// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
	// 参  数：[输入] DWORD dwMsg  - 事件ID
	// 参  数：[输入] IEventActionListener * pListener  - 事件行为执行者
	// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息	
	virtual BOOL			UnSubscibeMessage(DWORD dwMsg, IEventActionListener * pListener) = NULL;

	// 描  述: 订阅本对象的响应事件
	// 参  数: [输入] DWORD dwMsg - 事件ID
	// 参  数: [输入] IEventResponseListener * pListener - 事件响应者
	// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
	// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE	
	virtual BOOL			SubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener, const char szDescription[32] = NULL) = NULL;
	
	// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
	// 参  数：[输入] DWORD dwMsg  - 事件ID
	// 参  数：[输入] IEventResponseListener * pListener  - 事件响应者
	// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息	
	virtual BOOL			UnSubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener) = NULL;
};
//------------------------------------------------------------------------