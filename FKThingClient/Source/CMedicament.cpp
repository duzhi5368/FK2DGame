/**
*	created:		2012-7-4   1:38
*	filename: 		CMedicament
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		药品
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/CMedicament.h"
#include "../Include/CThingClientFacade.h"
//------------------------------------------------------------------------
CMedicament::CMedicament(void)
{
	// 普通属性初始化
	m_uid = INVALID_UID;
	ZeroMemory(&m_Property, sizeof(SMEDICAMENT_PROPERTY_PRIVATE));
	ZeroMemory(m_szName, sizeof(m_szName));

	// 逻辑对像类型初始化
	m_ThingClass.SetThingClass(THINGCLASS_GOODS);
	m_ThingClass.SetMedicament();	
}
//------------------------------------------------------------------------
CMedicament::~CMedicament(void)
{

}
//------------------------------------------------------------------------
void CMedicament::Release(void)
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
	if(!g_pThingClientFacade->GetThingTable()->Remove(m_uid))
	{
		TraceLn("从ThingTable中清除失败！%s", GetName());
	}

	delete this;
}
//------------------------------------------------------------------------
BOOL CMedicament::Create(void)
{
	IConfigDataCenter * pDataCenter = g_pThingClientFacade->GetConfigDataCenter();
	if(pDataCenter == NULL)
	{
		return FALSE;
	}

    GOODS_BASEPROPERTY & goodsbase = pDataCenter->GetGoodsBaseProperty(m_Property.dwGoodsID);
	if(goodsbase.lGoodsID == 0)
	{
		TraceLn("无法取得物品信息，创建物品失败！%d", m_Property.dwGoodsID);
		return FALSE;
	}
	lstrcpyn(m_szName, goodsbase.szName, sizeof(m_szName));

	return TRUE;	
}
//------------------------------------------------------------------------
void CMedicament::SetUID(UID uid)
{
	m_uid = uid;
}
//------------------------------------------------------------------------
UID	CMedicament::GetUID(void)
{
	return m_uid;
}
//------------------------------------------------------------------------
DWORD CMedicament::GetMapID(void)
{
	return GETUIDSERVERID(m_uid);
}
//------------------------------------------------------------------------
void CMedicament::SetLoc(POINTS ptLoc)
{

}
//------------------------------------------------------------------------
POINTS CMedicament::GetLoc(void)
{
	POINTS ptLoc = { 0, 0 };
	return ptLoc;
}
//------------------------------------------------------------------------
IThingClass * CMedicament::GetThingClass(void)
{
	return static_cast<IThingClass *>(&m_ThingClass);
}
//------------------------------------------------------------------------
//// 私有对象现场管理
//// 说明：
//// 私有对象现场是指用户自己的角色或装备等的详细信息，其它人是看不到的
//// 以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端
//// 如：用户登录时，服务器取得人物对象现场
//// 客户端根据dwClassID创建对象，调用OnGetPrivateContext()设置对象现场
BOOL CMedicament::OnSetPrivateContext(LPCSTR buf, int nLen)
{
	if(nLen != sizeof(SMEDICAMENT_PROPERTY_PRIVATE))
	{
		return FALSE;
	}

	memcpy(&m_Property, buf, sizeof(SMEDICAMENT_PROPERTY_PRIVATE));

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMedicament::OnGetPrivateContext(LPSTR buf, int &nLen)
{
	// 客户端不可能这样调的
	return FALSE;
}
//------------------------------------------------------------------------
//// 对象公开现场管理
//// 说明：
//// 公开现场为可以让所有GameWorld中的其它对象查看的对象信息，如角色名、HP、装备形状等
//// 服务器取得对象现场，以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端，
//// 客户端根据dwThingClassID创建对象，调用OnSetContext()设置对象现场
BOOL CMedicament::OnSetPublicContext(LPCSTR buf, int nLen)
{
	if(nLen != sizeof(SMEDICAMENT_PROPERTY_PUBLIC))
	{
		return FALSE;
	}

	memcpy(&m_Property, buf, sizeof(SMEDICAMENT_PROPERTY_PUBLIC));

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMedicament::OnGetPublicContext(LPSTR buf, int &nLen)
{
	if(nLen < sizeof(SMEDICAMENT_PROPERTY_PUBLIC))
	{
		return FALSE;
	}

	memcpy(buf, &m_Property, sizeof(SMEDICAMENT_PROPERTY_PUBLIC));
	nLen = sizeof(SMEDICAMENT_PROPERTY_PUBLIC);

	return TRUE;
}
//------------------------------------------------------------------------
//// 管理thing的数据库现场
//// 说明：
//// OnGetDBContext() 用于将对象现场保护到数据库
//// OnSetDBContext() 用于通过数据库返回的数据创建对象
BOOL CMedicament::OnSetDBContext(LPCSTR buf, int nLen)
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMedicament::OnGetDBContext(LPSTR buf, int &nLen)
{	
	return TRUE;
}
//------------------------------------------------------------------------
//// 消息通知, 驱使逻辑对像进行运转
void CMedicament::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return;
	}

	/*
	// 先判定否决是否通过
	if(pEventServer->FireVote((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_GOODS, (DWORD)this) == FALSE)
	{
		return;
	}

	// 然后行为事件
	pEventServer->FireAction((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_GOODS, (DWORD)this);
	*/
}
//------------------------------------------------------------------------
void CMedicament::OnResponse(DWORD dwMsg, LPCSTR buf, int nLen)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return;
	}

	/*
	// 触发响应
	pEventServer->FireResponse((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_GOODS, (DWORD)this);
	*/
}
//------------------------------------------------------------------------
//// 描  述: 订阅本对象的否决事件
//// 参  数: [输入] DWORD dwMsg - 事件ID
//// 参  数: [输入] IEventVoteListener * pListener - 事件否决者
//// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
//// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CMedicament::SubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_GOODS, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
//// 描  述：取消订阅本对象的否决事件,不会delete pListener指针
//// 参  数：[输入] DWORD dwMsg  - 事件ID
//// 参  数：[输入] IEventVoteListener * pListener  - 事件否决者	
//// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CMedicament::UnSubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_GOODS, (DWORD)this);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
//// 描  述: 订阅本对象的行为事件
//// 参  数: [输入] DWORD dwMsg - 事件ID
//// 参  数: [输入] IEventActionListener * pListener - 事件行为执行者
//// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
//// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE	
BOOL CMedicament::SubscibeMessage(DWORD dwMsg, IEventActionListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_GOODS, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
//// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
//// 参  数：[输入] DWORD dwMsg  - 事件ID
//// 参  数：[输入] IEventActionListener * pListener  - 事件行为执行者
//// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CMedicament::UnSubscibeMessage(DWORD dwMsg, IEventActionListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_GOODS, (DWORD)this);	
	*/
	return TRUE;
}
//------------------------------------------------------------------------
//// 描  述: 订阅本对象的响应事件
//// 参  数: [输入] DWORD dwMsg - 事件ID
//// 参  数: [输入] IEventResponseListener * pListener - 事件响应者
//// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
//// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CMedicament::SubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_GOODS, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
//// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
//// 参  数：[输入] DWORD dwMsg  - 事件ID
//// 参  数：[输入] IEventResponseListener * pListener  - 事件响应者
//// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CMedicament::UnSubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_GOODS, (DWORD)this);	
	*/
	return TRUE;
}
//------------------------------------------------------------------------
int CMedicament::OnCommand(DWORD dwCommand, LPCSTR buf, int nLen)
{
	return 0;
}
//------------------------------------------------------------------------
DWORD CMedicament::GetGoodsID(void)
{
	return m_Property.dwGoodsID;
}
//------------------------------------------------------------------------
LPCSTR CMedicament::GetName(void)
{
	return (LPCSTR)m_szName;
}
//------------------------------------------------------------------------
int CMedicament::GetPropNum(DWORD dwPropID)
{
	switch(dwPropID)
	{
	case GOODS_PROPID_NUMBER:
		{
			return m_Property.dwNumber;
		}
		break;
	default:
		{
			PrintEventLn("error : CMedicament::GetPropNum : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return 0;	
}
//------------------------------------------------------------------------
LPCSTR CMedicament::GetPropStr(DWORD dwPropID)
{
	switch(dwPropID)
	{
	case GOODS_PROPID_NAME:
		{
			return m_szName;
		}
		break;
	default:
		{
			PrintEventLn("error : CMedicament::GetStrProp : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return "";
}
//------------------------------------------------------------------------
DWORD CMedicament::GetNumber(void)
{
	return m_Property.dwNumber;
}
//------------------------------------------------------------------------
void CMedicament::Accept(IMapItemVisitor &visitor)
{
	visitor.VisitGoods();
}
//------------------------------------------------------------------------
CMapItemView * CMedicament::GetMapItem(void) const
{
	return NULL;
}
//------------------------------------------------------------------------
void CMedicament::SetMapItem(CMapItemView * pMapItemView)
{

}
//------------------------------------------------------------------------
BYTE * CMedicament::GetExtendBuf(int &nLen)
{
	nLen = sizeof(m_Property.byExtend);
	return m_Property.byExtend;
}
//------------------------------------------------------------------------
DWORD CMedicament::GetUseLock(void)
{
	return m_Property.dwUseLock;
}
//------------------------------------------------------------------------
BOOL CMedicament::GetBindFlag(DWORD dwFlag)
{
	return (m_Property.dwUseBind & dwFlag) == dwFlag ? TRUE : FALSE;
}
//------------------------------------------------------------------------