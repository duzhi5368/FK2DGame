/**
*	created:		2012-7-3   22:52
*	filename: 		CEquipment
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		装备品
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/IGoods.h"
#include "../../FKGame/SceneObj/IEquipment.h"
#include "../../FKGame/Common/GoodsPropertyDef.h"
#include "../Include/CThingClass.h"
//------------------------------------------------------------------------
class CEquipment : public IEquipment
{
public:
	// 释放
	virtual void				Release(void);

	// 设置UID
	virtual void				SetUID(UID uid);

	// 取得UID
	virtual UID					GetUID(void);

	// 取得地图ID, 也就是根据UID解析出来的
	virtual DWORD				GetMapID(void);

	// 设置位置
	virtual void				SetLoc(POINTS ptLoc);

	// 取得位置
	virtual POINTS				GetLoc(void);

	// 取得类型
	virtual IThingClass *		GetThingClass(void);

	/* 
	私有对象现场管理
	说明：
	私有对象现场是指用户自己的角色或装备等的详细信息，其它人是看不到的
	以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端
	如：用户登录时，服务器取得人物对象现场
	客户端根据dwClassID创建对象，调用OnGetPrivateContext()设置对象现场
	*/
	virtual BOOL				OnSetPrivateContext(LPCSTR buf, int nLen);
	virtual BOOL				OnGetPrivateContext(LPSTR buf, int &nLen);
	
	/*
	对象公开现场管理
	说明：
	公开现场为可以让所有GameWorld中的其它对象查看的对象信息，如角色名、HP、装备形状等
	服务器取得对象现场，以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端，
	客户端根据dwThingClassID创建对象，调用OnSetContext()设置对象现场	
	*/
	virtual BOOL				OnSetPublicContext(LPCSTR buf, int nLen);
	virtual BOOL				OnGetPublicContext(LPSTR buf, int &nLen);
	
	/*
	管理thing的数据库现场
	说明：
	OnGetDBContext() 用于将对象现场保护到数据库
	OnSetDBContext() 用于通过数据库返回的数据创建对象	
	*/
	virtual BOOL				OnSetDBContext(LPCSTR buf, int nLen);
	virtual BOOL				OnGetDBContext(LPSTR buf, int &nLen);

	// 执行命令
	virtual int					OnCommand(DWORD dwCommand, LPCSTR buf, int nLen);

	// 消息通知, 驱使逻辑对像进行运转
	virtual void				OnMessage(DWORD dwMsg, LPCSTR buf, int nLen);

	// 触发响应（FireResponse）
	virtual void				OnResponse(DWORD dwMsg, LPCSTR buf, int nLen);

	// 描  述: 订阅本对象的否决事件
	// 参  数: [输入] DWORD dwMsg - 事件ID
	// 参  数: [输入] IEventVoteListener * pListener - 事件否决者
	// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
	// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
	virtual BOOL				SubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener, const char szDescription[32] = NULL);
	
	// 描  述：取消订阅本对象的否决事件,不会delete pListener指针
	// 参  数：[输入] DWORD dwMsg  - 事件ID
	// 参  数：[输入] IEventVoteListener * pListener  - 事件否决者	
	// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
	virtual BOOL				UnSubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener);
	
	// 描  述: 订阅本对象的行为事件
	// 参  数: [输入] DWORD dwMsg - 事件ID
	// 参  数: [输入] IEventActionListener * pListener - 事件行为执行者
	// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
	// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE	
	virtual BOOL				SubscibeMessage(DWORD dwMsg, IEventActionListener * pListener, const char szDescription[32] = NULL);
	
	// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
	// 参  数：[输入] DWORD dwMsg  - 事件ID
	// 参  数：[输入] IEventActionListener * pListener  - 事件行为执行者
	// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息	
	virtual BOOL				UnSubscibeMessage(DWORD dwMsg, IEventActionListener * pListener);
	
	// 描  述: 订阅本对象的响应事件
	// 参  数: [输入] DWORD dwMsg - 事件ID
	// 参  数: [输入] IEventResponseListener * pListener - 事件响应者
	// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
	// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE	
	virtual BOOL				SubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener, const char szDescription[32] = NULL);
	
	// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
	// 参  数：[输入] DWORD dwMsg  - 事件ID
	// 参  数：[输入] IEventResponseListener * pListener  - 事件响应者
	// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息	
	virtual BOOL				UnSubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener);


	virtual void				Accept(IMapItemVisitor &visitor);
	
	// 取得地上物对象的视图接口
	virtual CMapItemView *		GetMapItem(void) const;

	// 设置视图
	virtual void				SetMapItem(CMapItemView * pMapItemView);

	// 取得物品ID
	virtual DWORD				GetGoodsID(void);

	// 取得物品名字
	virtual LPCSTR				GetName(void);

	// 取得物品的数字型属性
	virtual int					GetPropNum(DWORD dwPropID);

	// 取得物品的字符型属性
	virtual LPCSTR				GetPropStr(DWORD dwPropID);

	// 取得物品数量
	virtual DWORD				GetNumber(void);

	// 取得物品的使用锁定
	virtual DWORD				GetUseLock(void);

	// 取得绑定标志，单独查看绑定标志
	virtual  BOOL				GetBindFlag(DWORD dwFlag);

	// 取得耐久相关数值
	virtual void				GetEndure(int &nCur, int &nCurMax);

	// 取得效果
	virtual IEffect *			GetEffect(int nIndex);

	// 检查是否能装备该项装备
	virtual	BOOL				CanEquip(ICreature * pCreature, int nEquipPos);

	// 取得精练等级
	virtual int					GetSmeltLevel(void);

	// 取得装备的插槽数
	virtual DWORD				GetSlotLeft(void);

	// 创建
	BOOL						Create(void);

	CEquipment(void);

	~CEquipment(void);
	
private:
	// 清除所有效果
	void						ClearEffect(void);

private:
	UID							m_uid;
	// 属性表
	SEQUIPMENT_PROPERTY_PRIVATE	m_Property;
	// 名字
	char						m_szName[MAX_NAME_LEN];
	// 逻辑对像分类
	CThingClass					m_ThingClass;
	// 效果
	IEffect *					m_Effect[GOODS_EFFECT_MAXCOUNT];
};

//------------------------------------------------------------------------

