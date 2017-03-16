/**
*	created:		2012-7-3   23:11
*	filename: 		CWarrent
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		凭证
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/CWarrent.h"
#include "../Include/CThingClientFacade.h"
//------------------------------------------------------------------------
CWarrant::CWarrant(void)
{
	// 普通属性初始化
	m_uid = INVALID_UID;
	ZeroMemory(&m_Property, sizeof(SWARRAN_PROPERTY_PRIVATE));
	ZeroMemory(m_szName, sizeof(m_szName));
	ZeroMemory(m_Effect, sizeof(m_Effect));

	// 逻辑对像类型初始化
	m_ThingClass.SetThingClass(THINGCLASS_GOODS);
	m_ThingClass.SetWarrant();
}
//------------------------------------------------------------------------
CWarrant::~CWarrant(void)
{

}
//------------------------------------------------------------------------
void CWarrant::Release(void)
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
BOOL CWarrant::Create(void)
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
void CWarrant::SetUID(UID uid)
{
	m_uid = uid;
}
//------------------------------------------------------------------------
UID	CWarrant::GetUID(void)
{
	return m_uid;
}
//------------------------------------------------------------------------
DWORD CWarrant::GetMapID(void)
{
	return GETUIDSERVERID(m_uid);
}
//------------------------------------------------------------------------
void CWarrant::SetLoc(POINTS ptLoc)
{

}
//------------------------------------------------------------------------
POINTS CWarrant::GetLoc(void)
{
	POINTS ptLoc = { 0, 0 };
	return ptLoc;
}
//------------------------------------------------------------------------
IThingClass * CWarrant::GetThingClass(void)
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

BOOL CWarrant::OnSetPrivateContext(LPCSTR buf, int nLen)
{
	if(nLen != sizeof(SWARRAN_PROPERTY_PRIVATE))
	{
		return FALSE;
	}

	memcpy(&m_Property, buf, sizeof(SWARRAN_PROPERTY_PRIVATE));

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CWarrant::OnGetPrivateContext(LPSTR buf, int &nLen)
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
BOOL CWarrant::OnSetPublicContext(LPCSTR buf, int nLen)
{
	if(nLen != sizeof(SWARRAN_PROPERTY_PUBLIC))
	{
		return FALSE;
	}

	memcpy(&m_Property, buf, sizeof(SWARRAN_PROPERTY_PUBLIC));

	// 清除所有效果
	ClearEffect();

	// 创建效果
	IEffectFactory * pEffectFactory = g_pThingClientFacade->GetEffectFactory();
	if(pEffectFactory == NULL)
	{
		return FALSE;
	}
	
	for(int i = 0; i < GOODS_EFFECT_MAXCOUNT; i++)
	{
		if(m_Property.wEffect[i] != 0)
		{
			if(pEffectFactory->CreateEffect(m_Property.wEffect[i], &m_Effect[i]) == FALSE)
			{
				char szTraceBuf[256];
				sprintf(szTraceBuf, "无法创建%d效果，%s装备创建失败！", m_Property.wEffect[i], GetName());
				TraceLn(szTraceBuf);				
			}
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CWarrant::OnGetPublicContext(LPSTR buf, int &nLen)
{
	if(nLen < sizeof(SWARRAN_PROPERTY_PUBLIC))
	{
		return FALSE;
	}

	memcpy(buf, &m_Property, sizeof(SWARRAN_PROPERTY_PUBLIC));
	nLen = sizeof(SWARRAN_PROPERTY_PUBLIC);

	return TRUE;
}
//------------------------------------------------------------------------
//// 管理thing的数据库现场
//// 说明：
//// OnGetDBContext() 用于将对象现场保护到数据库
//// OnSetDBContext() 用于通过数据库返回的数据创建对象
BOOL CWarrant::OnSetDBContext(LPCSTR buf, int nLen)
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CWarrant::OnGetDBContext(LPSTR buf, int &nLen)
{	
	return TRUE;
}
//------------------------------------------------------------------------
//// 消息通知, 驱使逻辑对像进行运转
void CWarrant::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
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
//// 描  述：触发响应（FireResponse）
void CWarrant::OnResponse(DWORD dwMsg, LPCSTR buf, int nLen)
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
BOOL CWarrant::SubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener, const char szDescription[32])
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
BOOL CWarrant::UnSubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener)
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
BOOL CWarrant::SubscibeMessage(DWORD dwMsg, IEventActionListener * pListener, const char szDescription[32])
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
BOOL CWarrant::UnSubscibeMessage(DWORD dwMsg, IEventActionListener * pListener)
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
BOOL CWarrant::SubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener, const char szDescription[32])
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
BOOL CWarrant::UnSubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener)
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
int CWarrant::OnCommand(DWORD dwCommand, LPCSTR buf, int nLen)
{
	return 0;
}
//------------------------------------------------------------------------
DWORD CWarrant::GetGoodsID(void)
{
	return m_Property.dwGoodsID;
}
//------------------------------------------------------------------------
LPCSTR CWarrant::GetName(void)
{
	return (LPCSTR)m_szName;
}
//------------------------------------------------------------------------
int CWarrant::GetPropNum(DWORD dwPropID)
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
			PrintEventLn("error : CWarrant::GetPropNum : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return 0;	
}
//------------------------------------------------------------------------
LPCSTR CWarrant::GetPropStr(DWORD dwPropID)
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
			PrintEventLn("error : CWarrant::GetStrProp : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return "";
}
//------------------------------------------------------------------------
DWORD CWarrant::GetNumber(void)
{
	return m_Property.dwNumber;
}
//------------------------------------------------------------------------
void CWarrant::Accept(IMapItemVisitor &visitor)
{
	visitor.VisitGoods();
}
//------------------------------------------------------------------------
CMapItemView * CWarrant::GetMapItem(void) const
{
	return NULL;
}
//------------------------------------------------------------------------
void CWarrant::SetMapItem(CMapItemView * pMapItemView)
{

}
//------------------------------------------------------------------------
DWORD CWarrant::GetUseLock(void)
{
	return m_Property.dwUseLock;
}
//------------------------------------------------------------------------
BOOL CWarrant::GetBindFlag(DWORD dwFlag)
{
	return (m_Property.dwUseBind & dwFlag) == dwFlag ? TRUE : FALSE;
}
//------------------------------------------------------------------------
void CWarrant::ClearEffect(void)
{
	for(int i = 0; i < GOODS_EFFECT_MAXCOUNT; i++)
	{
		SAFE_RELEASE(m_Effect[i]);
	}
}
//------------------------------------------------------------------------
IEffect * CWarrant::GetEffect(int nIndex)
{
	if(nIndex < 0 || nIndex >= GOODS_EFFECT_MAXCOUNT)
	{
		return NULL;
	}

	return m_Effect[nIndex];	
}
//------------------------------------------------------------------------