/**
*	created:		2012-7-3   0:39
*	filename: 		CBox
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		盒子
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IBox.h"
#include "CThingClass.h"
#include "../../FKGame/Common/BoxProp.h"
#include <vector>
using std::vector;
//------------------------------------------------------------------------
class CBox : public IBox
{
public:
	// 释放
	virtual void Release(void);

	// 设置UID
	virtual void SetUID(UID uid);

	// 取得UID	
	virtual UID GetUID(void);

	// 取得地图ID, 也就是根据UID解析出来的
	virtual DWORD GetMapID(void);

	// 设置位置
	virtual void SetLoc(POINTS ptLoc);

	// 取得位置
	virtual POINTS GetLoc(void);

	// 取得类型
	virtual IThingClass* GetThingClass(void);

	/* 
	私有对象现场管理
	说明：
	私有对象现场是指用户自己的角色或装备等的详细信息，其它人是看不到的
	以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端
	如：用户登录时，服务器取得人物对象现场
	客户端根据dwClassID创建对象，调用OnGetPrivateContext()设置对象现场
	*/
	virtual BOOL OnSetPrivateContext(LPCSTR buf, int nLen);
	virtual BOOL OnGetPrivateContext(LPSTR buf, int &nLen);
	
	/*
	对象公开现场管理
	说明：
	公开现场为可以让所有GameWorld中的其它对象查看的对象信息，如角色名、HP、装备形状等
	服务器取得对象现场，以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端，
	客户端根据dwThingClassID创建对象，调用OnSetContext()设置对象现场	
	*/
	virtual BOOL OnSetPublicContext(LPCSTR buf, int nLen);
	virtual BOOL OnGetPublicContext(LPSTR buf, int &nLen);	

	// 执行命令
	virtual int OnCommand(DWORD dwCommand, LPCSTR buf, int nLen);

	// 消息通知, 驱使逻辑对像进行运转（只发FireVote和FireAction)
	virtual void OnMessage(DWORD dwMsg, LPCSTR buf, int nLen);

	// 触发响应
	virtual void OnResponse(DWORD dwMsg, LPCSTR buf, int nLen);

	// 描  述: 订阅本对象的否决事件
	// 参  数: [输入] DWORD dwMsg - 事件ID
	// 参  数: [输入] IEventVoteListener * pListener - 事件否决者
	// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
	// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
	virtual BOOL SubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener, const char szDescription[32] = NULL);

	// 描  述：取消订阅本对象的否决事件,不会delete pListener指针
	// 参  数：[输入] DWORD dwMsg  - 事件ID
	// 参  数：[输入] IEventVoteListener * pListener  - 事件否决者	
	// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
	virtual BOOL UnSubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener);

	// 描  述: 订阅本对象的行为事件
	// 参  数: [输入] DWORD dwMsg - 事件ID
	// 参  数: [输入] IEventActionListener * pListener - 事件行为执行者
	// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
	// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE	
	virtual BOOL SubscibeMessage(DWORD dwMsg, IEventActionListener * pListener, const char szDescription[32] = NULL);
	
	// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
	// 参  数：[输入] DWORD dwMsg  - 事件ID
	// 参  数：[输入] IEventActionListener * pListener  - 事件行为执行者
	// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息	
	virtual BOOL UnSubscibeMessage(DWORD dwMsg, IEventActionListener * pListener);

	// 描  述: 订阅本对象的响应事件
	// 参  数: [输入] DWORD dwMsg - 事件ID
	// 参  数: [输入] IEventResponseListener * pListener - 事件响应者
	// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
	// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE	
	virtual BOOL SubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener, const char szDescription[32] = NULL);
	
	// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
	// 参  数：[输入] DWORD dwMsg  - 事件ID
	// 参  数：[输入] IEventResponseListener * pListener  - 事件响应者
	// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息	
	virtual BOOL UnSubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener);

	virtual void Accept(IMapItemVisitor &visitor);	
	
	// 取得地图物件指针
	virtual CMapItemView* GetMapItem() const;
	
	// 设置视图
	virtual void SetMapItem(CMapItemView * pMapItemView);	

	// 得到名字
	virtual LPCSTR GetName() const;
	virtual BOOL Open();	
	virtual void Close();	
	virtual void Respond(LPCSTR pBuf, int nLen);
	virtual DWORD GetType() const;
	virtual DWORD GetID() const;
	virtual void SetRight(BOOL bRight);		
	virtual BOOL IsCanOpen(void);

	// 构造
	CBox();

	// 析构
	~CBox();

	// 创建
	BOOL Create();

private:
	void PrepareOpen(DWORD dwOpenTime);
	void OpenBox(LPCSTR pBuf, int nLen);
	void CloseBox();
	void ClearBox();
	void Command(LPCSTR pBuf, int nLen);

private:
	UID								m_uid;
	// 属性表
	SBOX_PROPERTY_LIST		        m_Property;
	// 地图物件
	CMapItemView *					m_pMapItemView;
	// 逻辑对像分类
	CThingClass						m_ThingClass;
	// 盒子里的容器
	vector<DWORD>                   m_objects;
};
//------------------------------------------------------------------------