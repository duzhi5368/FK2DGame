/**
*	created:		2012-7-3   1:10
*	filename: 		CChunnel
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		地图通道
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/CChunnel.h"
#include "../Include/CThingClientFacade.h"
#include "../Include/CreatureViewWrapper.h"
//------------------------------------------------------------------------
CChunnel::CChunnel(void)
{
	// 普通属性初始化
	m_uid = INVALID_UID;
	m_pItemView = NULL;
	memset(&m_Property, 0, sizeof(m_Property));

	// 逻辑对像类型初始化
	m_ThingClass.SetThingClass(THINGCLASS_CHUNNEL);
}
//------------------------------------------------------------------------
CChunnel::~CChunnel(void)
{

}
//------------------------------------------------------------------------
void CChunnel::Release(void)
{
	// 发一事件出去,即将删除
	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_RELEASE);
	OnMessage(wMsgID, NULL, 0);

	if(g_pThingClientFacade->GetThingTable() == NULL)
	{
		delete this;
		return;
	}

	if(g_pThingClientFacade->GetMapView() == NULL)
	{
		delete this;
		return;
	}
		
	// 从ThingTable中清除
	if(g_pThingClientFacade->GetThingTable()->Remove(m_uid) == FALSE)
	{
		TraceLn("从ThingTable中清除失败！%s", GetName());
	}

	// 清除MapItem
	if(m_pItemView != NULL)
	{
		POINTS _ptLoc = GetLoc();
		POINT  ptLoc = { _ptLoc.x, _ptLoc.y };
		if(g_pThingClientFacade->GetMapView()->RemoveItem(ptLoc, m_pItemView) == FALSE)
		{
			TraceLn("清除MapItem失败！%s[%d, %d]", GetName(), ptLoc.x, ptLoc.y);
		}

		// ????因为basicsys未用FCL，无法释放
		SAFE_RELEASE(m_pItemView);
	}

	delete this;
}
//------------------------------------------------------------------------
BOOL CChunnel::Create(void)
{
	IItemClassFactory * pItemClassFactory = g_pThingClientFacade->GetItemFactory();
	if(pItemClassFactory == NULL)
	{
		return FALSE;
	}
	
	// 创建地图部件
	m_pItemView = (CMapItemView *)pItemClassFactory->CreateItem(miAniObj_Transprot, (LPCSTR)&m_Property.dwResID, sizeof(DWORD));
	if(m_pItemView == NULL)
	{
		return FALSE;
	}
	m_pItemView->SetItemData((DWORD)(this));

	// 各种外观属性设置
	CCreatureViewWrapper viewwrapper(m_pItemView);
	viewwrapper.AddToMap(m_Property.ptLoc);
	viewwrapper.SetTile(m_Property.ptLoc);
	viewwrapper.AddFlag(CMapItemView::flagCanbeSelected);
	viewwrapper.AddFlag(CMapItemView::flagPaoPao);
	viewwrapper.SetFlagColorOnMinmap(255, 255, 0);

	return TRUE;
}
//------------------------------------------------------------------------
void CChunnel::SetUID(UID uid)
{
	m_uid = uid;
}
//------------------------------------------------------------------------
UID	CChunnel::GetUID(void)
{
	return m_uid;
}
//------------------------------------------------------------------------
DWORD CChunnel::GetMapID(void)
{
	return GETUIDSERVERID(m_uid);
}
//------------------------------------------------------------------------
void CChunnel::SetLoc(POINTS ptLoc)
{
	m_Property.ptLoc = ptLoc;
}
//------------------------------------------------------------------------
POINTS CChunnel::GetLoc(void)
{
	POINTS ptLoc = { 0, 0 };
	if(m_pItemView != NULL)
	{
		POINT _ptLoc = m_pItemView->GetTile();
		ptLoc.x = _ptLoc.x;
		ptLoc.y = _ptLoc.y;
	}

	return ptLoc;
}
//------------------------------------------------------------------------
IThingClass * CChunnel::GetThingClass(void)
{
	return static_cast<IThingClass *>(&m_ThingClass);
}
//------------------------------------------------------------------------
// 私有对象现场管理
// 说明：
// 私有对象现场是指用户自己的角色或装备等的详细信息，其它人是看不到的
// 以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端
// 如：用户登录时，服务器取得人物对象现场
// 客户端根据dwClassID创建对象，调用OnGetPrivateContext()设置对象现场
BOOL CChunnel::OnSetPrivateContext(LPCSTR buf, int nLen)
{
	if(buf == NULL || nLen != sizeof(SCHUNNEL_PROPERTY_LIST))
	{
		TraceLn("CChunnel::OnSetPrivateContext Failed! [buf == NULL]");
		return FALSE;
	}

	memcpy(&m_Property, buf, sizeof(SCHUNNEL_PROPERTY_LIST));

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CChunnel::OnGetPrivateContext(LPSTR buf, int &nLen)
{
	// 客户端不可能这样调的
	return FALSE;
}
//------------------------------------------------------------------------
// 对象公开现场管理
// 说明：
// 公开现场为可以让所有GameWorld中的其它对象查看的对象信息，如角色名、HP、装备形状等
// 服务器取得对象现场，以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端，
// 客户端根据dwThingClassID创建对象，调用OnSetContext()设置对象现场
BOOL CChunnel::OnSetPublicContext(LPCSTR buf, int nLen)
{
	return OnSetPrivateContext(buf, nLen);
}
//------------------------------------------------------------------------
BOOL CChunnel::OnGetPublicContext(LPSTR buf, int &nLen)
{	
	// 客户端不可能这样调的
	return FALSE;
}
//------------------------------------------------------------------------
// 管理thing的数据库现场
// 说明：
// OnGetDBContext() 用于将对象现场保护到数据库
// OnSetDBContext() 用于通过数据库返回的数据创建对象
BOOL CChunnel::OnSetDBContext(LPCSTR buf, int nLen)
{	
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CChunnel::OnGetDBContext(LPSTR buf, int &nLen)
{
	return FALSE;
}
//------------------------------------------------------------------------
int CChunnel::OnCommand(DWORD dwCommand, LPCSTR buf, int nLen)
{
	return 0;
}
//------------------------------------------------------------------------
void CChunnel::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return;
	}

	/*
	// 先判定否决是否通过
	if(pEventServer->FireVote((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_CHUNNEL, (DWORD)this) == FALSE)
	{
		return;
	}

	// 然后行为事件
	pEventServer->FireAction((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_CHUNNEL, (DWORD)this);
	*/
}
//------------------------------------------------------------------------
// 描  述：触发响应
void CChunnel::OnResponse(DWORD dwMsg, LPCSTR buf, int nLen)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return;
	}

	/*
	// 触发响应
	pEventServer->FireResponse((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_CHUNNEL, (DWORD)this);
	*/
}
//------------------------------------------------------------------------
// 描  述: 订阅本对象的否决事件
// 参  数: [输入] DWORD dwMsg - 事件ID
// 参  数: [输入] IEventVoteListener * pListener - 事件否决者
// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CChunnel::SubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_CHUNNEL, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述：取消订阅本对象的否决事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventVoteListener * pListener  - 事件否决者	
// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CChunnel::UnSubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_CHUNNEL, (DWORD)this);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述: 订阅本对象的行为事件
// 参  数: [输入] DWORD dwMsg - 事件ID
// 参  数: [输入] IEventActionListener * pListener - 事件行为执行者
// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CChunnel::SubscibeMessage(DWORD dwMsg, IEventActionListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_CHUNNEL, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventActionListener * pListener  - 事件行为执行者
BOOL CChunnel::UnSubscibeMessage(DWORD dwMsg, IEventActionListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_CHUNNEL, (DWORD)this);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述: 订阅本对象的响应事件
// 参  数: [输入] DWORD dwMsg - 事件ID
// 参  数: [输入] IEventResponseListener * pListener - 事件响应者
// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CChunnel::SubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_CHUNNEL, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventResponseListener * pListener  - 事件响应者
// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CChunnel::UnSubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_CHUNNEL, (DWORD)this);	
	*/
	return TRUE;
}
//------------------------------------------------------------------------
LPCSTR CChunnel::GetName(void)
{
	return m_Property.szName;
}
//------------------------------------------------------------------------
void CChunnel::Accept(IMapItemVisitor &visitor)
{
	visitor.VisitChunnel();
}
//------------------------------------------------------------------------
CMapItemView * CChunnel::GetMapItem(void) const
{
	return m_pItemView;
}
//------------------------------------------------------------------------
void CChunnel::SetMapItem(CMapItemView * pMapItemView)
{
	m_pItemView = pMapItemView;
}
//------------------------------------------------------------------------
DWORD CChunnel::GetTransMode(void)
{
	return m_Property.dwTransMode;
}
//------------------------------------------------------------------------
SCHUNNEL_TARGET * CChunnel::GetTargetInfor(DWORD &dwCount)
{
	dwCount = m_Property.dwTargetCount;
	return m_Property.TargetInfor;
}
//------------------------------------------------------------------------