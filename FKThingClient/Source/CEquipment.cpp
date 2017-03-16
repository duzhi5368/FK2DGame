/**
*	created:		2012-7-3   22:54
*	filename: 		CEquipment
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		装备
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/CEquipment.h"
#include "../Include/CThingClientFacade.h"
#include "../../FKGame/Effect/IEffectClient.h"
//------------------------------------------------------------------------
CEquipment::CEquipment(void)
{
	// 普通属性初始化
	m_uid = INVALID_UID;
	ZeroMemory(&m_Property, sizeof(SEQUIPMENT_PROPERTY_PRIVATE));
	ZeroMemory(m_Effect, sizeof(m_Effect));
	ZeroMemory(m_szName, sizeof(m_szName));

	// 逻辑对像类型初始化
	m_ThingClass.SetThingClass(THINGCLASS_GOODS);
	m_ThingClass.SetEquipment();	
}
//------------------------------------------------------------------------
CEquipment::~CEquipment(void)
{

}
//------------------------------------------------------------------------
void CEquipment::Release(void)
{
	// 发一事件出去,即将删除
	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_RELEASE);
	OnMessage(wMsgID, NULL, 0);

	// 清除所有效果
	ClearEffect();

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

	delete this;
}
//------------------------------------------------------------------------
BOOL CEquipment::Create(void)
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
void CEquipment::SetUID(UID uid)
{
	m_uid = uid;
}
//------------------------------------------------------------------------
UID	CEquipment::GetUID(void)
{
	return m_uid;
}
//------------------------------------------------------------------------
DWORD CEquipment::GetMapID(void)
{
	return GETUIDSERVERID(m_uid);
}
//------------------------------------------------------------------------
void CEquipment::SetLoc(POINTS ptLoc)
{

}
//------------------------------------------------------------------------
POINTS CEquipment::GetLoc(void)
{
	POINTS ptLoc = { 0, 0 };
	return ptLoc;
}
//------------------------------------------------------------------------
IThingClass * CEquipment::GetThingClass(void)
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
BOOL CEquipment::OnSetPrivateContext(LPCSTR buf, int nLen)
{
	if(nLen != sizeof(SEQUIPMENT_PROPERTY_PRIVATE))
	{
		return FALSE;
	}

	memcpy(&m_Property, buf, sizeof(SEQUIPMENT_PROPERTY_PRIVATE));

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CEquipment::OnGetPrivateContext(LPSTR buf, int &nLen)
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
BOOL CEquipment::OnSetPublicContext(LPCSTR buf, int nLen)
{
	if(nLen != sizeof(SEQUIPMENT_PROPERTY_PUBLIC))
	{
		return FALSE;
	}

	memcpy(&m_Property, buf, sizeof(SEQUIPMENT_PROPERTY_PUBLIC));

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
				TraceLn( szTraceBuf);				
			}
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CEquipment::OnGetPublicContext(LPSTR buf, int &nLen)
{
	if(nLen < sizeof(SEQUIPMENT_PROPERTY_PUBLIC))
	{
		return FALSE;
	}

	memcpy(buf, &m_Property, sizeof(SEQUIPMENT_PROPERTY_PUBLIC));
	nLen = sizeof(SEQUIPMENT_PROPERTY_PUBLIC);

	return TRUE;
}
//------------------------------------------------------------------------
// 管理thing的数据库现场
// 说明：
// OnGetDBContext() 用于将对象现场保护到数据库
// OnSetDBContext() 用于通过数据库返回的数据创建对象
BOOL CEquipment::OnSetDBContext(LPCSTR buf, int nLen)
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CEquipment::OnGetDBContext(LPSTR buf, int &nLen)
{	
	return TRUE;
}
//------------------------------------------------------------------------
// 消息通知, 驱使逻辑对像进行运转
void CEquipment::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
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
// 描  述：触发响应（FireResponse）
void CEquipment::OnResponse(DWORD dwMsg, LPCSTR buf, int nLen)
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
// 描  述: 订阅本对象的否决事件
// 参  数: [输入] DWORD dwMsg - 事件ID
// 参  数: [输入] IEventVoteListener * pListener - 事件否决者
// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CEquipment::SubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener, const char szDescription[32])
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
// 描  述：取消订阅本对象的否决事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventVoteListener * pListener  - 事件否决者	
// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CEquipment::UnSubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener)
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
// 描  述: 订阅本对象的行为事件
// 参  数: [输入] DWORD dwMsg - 事件ID
// 参  数: [输入] IEventActionListener * pListener - 事件行为执行者
// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CEquipment::SubscibeMessage(DWORD dwMsg, IEventActionListener * pListener, const char szDescription[32])
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
// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventActionListener * pListener  - 事件行为执行者
// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CEquipment::UnSubscibeMessage(DWORD dwMsg, IEventActionListener * pListener)
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
// 描  述: 订阅本对象的响应事件
// 参  数: [输入] DWORD dwMsg - 事件ID
// 参  数: [输入] IEventResponseListener * pListener - 事件响应者
// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CEquipment::SubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener, const char szDescription[32])
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
// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventResponseListener * pListener  - 事件响应者
// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CEquipment::UnSubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener)
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
// 描  述：执行命令
int CEquipment::OnCommand(DWORD dwCommand, LPCSTR buf, int nLen)
{
	return 0;
}
//------------------------------------------------------------------------
// 描  述：取得物品ID
DWORD CEquipment::GetGoodsID(void)
{
	return m_Property.dwGoodsID;
}
//------------------------------------------------------------------------
// 取得生物名字
LPCSTR CEquipment::GetName(void)
{
	return (LPCSTR)m_szName;
}
//------------------------------------------------------------------------
// 取得生物的数字型属性
int CEquipment::GetPropNum(DWORD dwPropID)
{
	switch(dwPropID)
	{
	case GOODS_PROPID_NUMBER:
		{
			return m_Property.dwNumber;
		}
		break;
		// 当前耐久
	case GOODS_PROPID_ENDURE_CUR:
		{
			return m_Property.nEndureCur;
		}
		break;
		// 当前最大耐久
	case GOODS_PROPID_ENDURE_CURMAX:
		{
			return m_Property.nEndureCurMax;
		}
		break;
		// 精炼等级
	case GOODS_PROPID_SMELTLEVEL:
		{
			return m_Property.nSmeltLevel;
		}
		break;
	default:
		{
			PrintEventLn("error : CEquipment::GetPropNum : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return 0;	
}
//------------------------------------------------------------------------
// 取得生物的字符型属性
LPCSTR CEquipment::GetPropStr(DWORD dwPropID)
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
			PrintEventLn("error : CEquipment::GetStrProp : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return "";
}
//------------------------------------------------------------------------
// 描  述：取得物品数量
DWORD CEquipment::GetNumber(void)
{
	return m_Property.dwNumber;
}
//------------------------------------------------------------------------
// 描  述：取得效果
IEffect * CEquipment::GetEffect(int nIndex)
{
	if(nIndex < 0 || nIndex >= GOODS_EFFECT_MAXCOUNT)
	{
		return NULL;
	}

	return m_Effect[nIndex];	
}
//------------------------------------------------------------------------
// 描  述：取得精练等级
int	CEquipment::GetSmeltLevel(void)
{
	return m_Property.nSmeltLevel;
}
//------------------------------------------------------------------------
// 描  述：取得装备的插槽数
DWORD CEquipment::GetSlotLeft(void)
{
	return m_Property.nSlotLeft;
}
//------------------------------------------------------------------------
// 描  述：检查是否能装备该项装备
BOOL CEquipment::CanEquip(ICreature * pCreature, int nEquipPos)
{
	if(pCreature == NULL)
	{
		return FALSE;
	}

	IConfigDataCenter * pConfigDataCenter = g_pThingClientFacade->GetConfigDataCenter();
	if(pConfigDataCenter == NULL)
	{
		return FALSE;
	}

	EQUIPMENT_BASEPROPERTY baseproperty = pConfigDataCenter->GetEquipmentBaseProperty(m_Property.dwGoodsID, m_Property.nSmeltLevel);
	if(baseproperty.lGoodsID == 0)
	{
		return FALSE;
	}

	// 人物
	if(pCreature->GetThingClass()->GetThingClass() == THINGCLASS_ACTOR)
	{
		IActor * pActor = (IActor *)(pCreature);

		char szSystemMsg[256];
		// 能否装备
		BOOL bEquipFlag = GetBindFlag(GOODS_BINDFLAG_EQUIP);	
		if(!bEquipFlag)
		{
			sprintf(szSystemMsg, "%s无法装备！", baseproperty.szName);

			WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_SYSTEMMESSAGE);
			pActor->OnMessage(wMsgID, szSystemMsg, sizeof(szSystemMsg));
			return FALSE;
		}

		// 匹配等级
		int nLevel = pActor->GetPropNum(CREATURE_PROPID_LEVEL);
		if(nLevel < baseproperty.lAllowLevel)
		{
			sprintf(szSystemMsg, "您等级不够，%s装备失败！", baseproperty.szName);

			WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_SYSTEMMESSAGE);
			pActor->OnMessage(wMsgID, szSystemMsg, sizeof(szSystemMsg));
			return FALSE;
		}

		// 匹配职业
		if(baseproperty.lAllowOccupation != 0)
		{
			int nOccupation = pActor->GetPropNum(CREATURE_PROPID_OCCUPATION);
			if(nOccupation != baseproperty.lAllowOccupation)
			{
				sprintf(szSystemMsg, "无法匹配职业，%s装备失败！", baseproperty.szName);

				WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_SYSTEMMESSAGE);
				pActor->OnMessage(wMsgID, szSystemMsg, sizeof(szSystemMsg));
				return FALSE;
			}			
		}

		// 匹配位置
		if(baseproperty.lEquipPos1 != nEquipPos && baseproperty.lEquipPos2 != nEquipPos && baseproperty.lEquipPos3 != nEquipPos)
		{
			sprintf(szSystemMsg, "装备位置错误，%s装备失败！", baseproperty.szName);

			WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_SYSTEMMESSAGE);
			pActor->OnMessage(wMsgID, szSystemMsg, sizeof(szSystemMsg));
			return FALSE;
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 描  述：visitor
void CEquipment::Accept(IMapItemVisitor &visitor)
{
	visitor.VisitGoods();
}
//------------------------------------------------------------------------
// 取得地图物件指针
CMapItemView * CEquipment::GetMapItem(void) const
{
	return NULL;
}
//------------------------------------------------------------------------
// 描  述：设置视图接口
void CEquipment::SetMapItem(CMapItemView * pMapItemView)
{

}
//------------------------------------------------------------------------
// 描  述：取得耐久相关数值
void CEquipment::GetEndure(int &nCur, int &nCurMax)
{
	nCur = m_Property.nEndureCur;
	nCurMax = m_Property.nEndureCurMax;
}
//------------------------------------------------------------------------
// 描  述：清除所有效果
void CEquipment::ClearEffect(void)
{
	for(int i = 0; i < GOODS_EFFECT_MAXCOUNT; i++)
	{
		SAFE_RELEASE(m_Effect[i]);
	}
}
//------------------------------------------------------------------------
// 描  述：取得物品的使用锁定
DWORD CEquipment::GetUseLock(void)
{
	return m_Property.dwUseLock;
}
//------------------------------------------------------------------------
// 描  述：取得绑定标志，单独查看绑定标志
BOOL CEquipment::GetBindFlag(DWORD dwFlag)
{
	return (m_Property.dwUseBind & dwFlag) == dwFlag ? TRUE : FALSE;
}
//------------------------------------------------------------------------