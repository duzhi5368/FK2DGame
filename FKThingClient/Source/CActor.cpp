/**
*	created:		2012-7-3   1:22
*	filename: 		CActor
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		别的玩家
*/
//------------------------------------------------------------------------
#include <windows.h>
#include <mmsystem.h>
#include "../Include/ThingClientCommon.h"
#include "../Include/CActor.h"
#include "../Include/CThingClientFacade.h"
#include "../Include/CreatureViewWrapper.h"
//------------------------------------------------------------------------
CActor::CActor(void)
{
	// 普通属性初始化
	m_uid = INVALID_UID;
	m_pItemView = NULL;
	m_pItemHead = NULL;
	m_bIsDying = FALSE;
	m_dwOldHeadResID = 0;
	ZeroMemory(m_ThingPart, sizeof(m_ThingPart));
	ZeroMemory(&m_Property, sizeof(SACTOR_PROPERTY_PRIVATE));

	// 逻辑对像类型初始化
	m_ThingClass.SetThingClass(THINGCLASS_ACTOR);
	m_ThingClass.SetCreature();
}
//------------------------------------------------------------------------
CActor::~CActor(void)
{

}
//------------------------------------------------------------------------
void CActor::Release(void)
{	
	// 发一事件出去,即将删除
	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_RELEASE);
	OnMessage(wMsgID, NULL, 0);

	// 释放部件
	for(int i = 0; i < THINGPARTID_THING_MAXID; i++)
	{
		SAFE_RELEASE(m_ThingPart[i]);
	}

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

	// 释放头像
	SAFE_RELEASE(m_pItemHead);

	// 清除MapItem
	if(m_pItemView != NULL)
	{
		POINTS _ptLoc = GetLoc();
		POINT  ptLoc = { _ptLoc.x, _ptLoc.y };
		if(g_pThingClientFacade->GetMapView()->RemoveItem(ptLoc, m_pItemView) == FALSE)
		{
			TraceLn("清除MapItem失败！%s[%d, %d]", GetName(), ptLoc.x, ptLoc.y);
		}

		SAFE_RELEASE(m_pItemView);
	}

	delete this;
}
//------------------------------------------------------------------------
BOOL CActor::Create(void)
{
	// 创建地图对像
	struct stTemp
	{
		int				nType;		// 身形ID	
		ICreatureSink * pCrtSink;	// 
	};

	// 取得性别，以便确定身形（标准男人形或标准女人形）
	int nSex = GetPropNum(CREATURE_PROPID_SEX);	
	stTemp tempdata;
	tempdata.nType = nSex == ACTOR_SEX_MAN ? MAN_CREATURE_TYPE : FEMALE_CREATURE_TYPE;
	tempdata.pCrtSink = static_cast<ICreatureSink *>(this);
	IItemClassFactory * pItemClassFactory = g_pThingClientFacade->GetItemFactory();
	if(pItemClassFactory == NULL)
	{
		return FALSE;
	}

	if(m_Property.ptLoc.x == 0 || m_Property.ptLoc.y == 0)
	{
		TraceLn( "%s创建失败，坐标为零坐标！", GetName());
		return FALSE;
	}
	
	int nDataLen = sizeof(tempdata);
	m_pItemView = (CMapItemView *)pItemClassFactory->CreateItem(miActorObj, (const char *)&tempdata, nDataLen);
	if(m_pItemView == NULL)
	{
		return FALSE;
	}

	// 指定人物在部件找不到图包时，用特定的图包代替
	IConfigDataCenter * pConfigDataCenter = g_pThingClientFacade->GetConfigDataCenter();
	if(pConfigDataCenter == NULL)
	{
		return FALSE;
	}

	int nOccupation = GetPropNum(CREATURE_PROPID_OCCUPATION);
	CCreatureViewWrapper viewwrapper(m_pItemView);
	for(int i = 0; i < CrtPartMax; i++)
	{
		DWORD dwResID = pConfigDataCenter->GetNudePartForActor(nOccupation, i, nSex);
		viewwrapper.SpecialModule(i, dwResID);
	}

	// 刷新形像
	UpdateAppearance();
	
	// 各种外观属性设置
	viewwrapper.SetData((DWORD)this);
	viewwrapper.AddToMap(m_Property.ptLoc);
	viewwrapper.SetTile(m_Property.ptLoc);
	viewwrapper.SetName(GetName());
	viewwrapper.SetNameColor(m_Property.nNameColor);
	viewwrapper.AddFlag(CMapItemView::flagCanbeSelected);
	viewwrapper.SetDirection(m_Property.nDir);
	viewwrapper.SetFlagColorOnMinmap(0, 255, 255);

	int nHPCur = GetPropNum(CREATURE_PROPID_HPCUR);
	int nHPMax = GetPropNum(CREATURE_PROPID_HPMAXCUR);
	viewwrapper.SetBloodInfo(GetPropNum(CREATURE_PROPID_HPCUR), max(nHPCur, nHPMax));

	viewwrapper.AddFlag(CMapItemView::flagCanbeSelected);
	viewwrapper.SetMoveSpeedLevel((int *)&m_Property.nMoveSpeedLevel);
	
	return TRUE;
}
//------------------------------------------------------------------------
void CActor::SetUID(UID uid)
{
	m_uid = uid;
}
//------------------------------------------------------------------------
UID	CActor::GetUID(void)
{
	return m_uid;
}
//------------------------------------------------------------------------
DWORD CActor::GetMapID(void)
{
	return GETUIDSERVERID(m_uid);
}
//------------------------------------------------------------------------
void CActor::SetLoc(POINTS ptLoc)
{
	m_Property.ptLoc = ptLoc;
}
//------------------------------------------------------------------------
POINTS CActor::GetLoc(void)
{
	CCreatureViewWrapper viewwrapper(m_pItemView);
	
	POINT pos = viewwrapper.GetTile();
	POINTS loc = { pos.x, pos.y };

	return loc;
}
//------------------------------------------------------------------------
IThingClass * CActor::GetThingClass(void)
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
BOOL CActor::OnSetPrivateContext(LPCSTR buf, int nLen)
{
	if(buf == NULL || nLen != sizeof(SACTOR_PROPERTY_PUBLIC))
	{
		TraceLn("CActor::OnSetPrivateContext Failed! [nLen != sizeof(SACTOR_PROPERTY_PUBLIC)]");
		return FALSE;
	}

	memcpy(&m_Property, buf, sizeof(SACTOR_PROPERTY_PUBLIC));
	
	m_Property.nMoveSpeedLevel = m_Property.nMoveSpeedLevel < 0 ? 0 : m_Property.nMoveSpeedLevel;
	m_Property.nMoveSpeedLevel = m_Property.nMoveSpeedLevel >= MAX_MOVESPEED_LEVEL ? MAX_MOVESPEED_LEVEL - 1 : m_Property.nMoveSpeedLevel;

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CActor::OnGetPrivateContext(LPSTR buf, int &nLen)
{
	return FALSE;
}
//------------------------------------------------------------------------
// 对象公开现场管理
// 说明：
// 公开现场为可以让所有GameWorld中的其它对象查看的对象信息，如角色名、HP、装备形状等
// 服务器取得对象现场，以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端，
// 客户端根据dwThingClassID创建对象，调用OnSetContext()设置对象现场
BOOL CActor::OnSetPublicContext(LPCSTR buf, int nLen)
{
	return OnSetPrivateContext(buf, nLen);
}
//------------------------------------------------------------------------
BOOL CActor::OnGetPublicContext(LPSTR buf, int &nLen)
{	
	return FALSE;
}
//------------------------------------------------------------------------
// 管理thing的数据库现场
// 说明：
// OnGetDBContext() 用于将对象现场保护到数据库
// OnSetDBContext() 用于通过数据库返回的数据创建对象
BOOL CActor::OnSetDBContext(LPCSTR buf, int nLen)
{	
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CActor::OnGetDBContext(LPSTR buf, int &nLen)
{
	return FALSE;
}
//------------------------------------------------------------------------
int CActor::OnCommand(DWORD dwCommand, LPCSTR buf, int nLen)
{
	return 0;
}
//------------------------------------------------------------------------
void CActor::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return;
	}

	/*
	// 先判定否决是否通过
	if(pEventServer->FireVote((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_ACTOR, (DWORD)this) == FALSE)
	{
		return;
	}

	// 然后行为事件
	pEventServer->FireAction((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_ACTOR, (DWORD)this);
	*/
}
//------------------------------------------------------------------------
// 描  述：触发响应
void CActor::OnResponse(DWORD dwMsg, LPCSTR buf, int nLen)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return;
	}

	/*
	// 触发响应
	pEventServer->FireResponse((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_ACTOR, (DWORD)this);
	*/
}
//------------------------------------------------------------------------
// 描  述: 订阅本对象的否决事件
// 参  数: [输入] DWORD dwMsg - 事件ID
// 参  数: [输入] IEventVoteListener * pListener - 事件否决者
// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CActor::SubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_ACTOR, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述：取消订阅本对象的否决事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventVoteListener * pListener  - 事件否决者	
// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CActor::UnSubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_ACTOR, (DWORD)this);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述: 订阅本对象的行为事件
// 参  数: [输入] DWORD dwMsg - 事件ID
// 参  数: [输入] IEventActionListener * pListener - 事件行为执行者
// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CActor::SubscibeMessage(DWORD dwMsg, IEventActionListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_ACTOR, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventActionListener * pListener  - 事件行为执行者
// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CActor::UnSubscibeMessage(DWORD dwMsg, IEventActionListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_ACTOR, (DWORD)this);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述: 订阅本对象的响应事件
// 参  数: [输入] DWORD dwMsg - 事件ID
// 参  数: [输入] IEventResponseListener * pListener - 事件响应者
// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CActor::SubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_ACTOR, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventResponseListener * pListener  - 事件响应者
// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CActor::UnSubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_ACTOR, (DWORD)this);	
	*/
	return TRUE;
}
//------------------------------------------------------------------------
LPCSTR CActor::GetName(void)
{
	return GetPropStr(CREATURE_PROPID_NAME);
}
//------------------------------------------------------------------------
int	CActor::SetPropNum(DWORD dwPropID, int nValue)
{
	switch(dwPropID)
	{
		// 等级
	case CREATURE_PROPID_LEVEL:
		{
			m_Property.nLevel = nValue;
		}
		break;
		// 种族
	case CREATURE_PROPID_PHYLE:
		{
			m_Property.nPhyle = nValue;
		}
		break;
		// 职业
	case CREATURE_PROPID_OCCUPATION:
		{
			m_Property.nOccupation = nValue;
		}
		break;
		// 当前生命值
	case CREATURE_PROPID_HPCUR:
		{
			m_Property.nHPCur = nValue;
		}
		break;
		// 当前最大生命值
	case CREATURE_PROPID_HPMAXCUR:
		{
			m_Property.nHPMaxCur = nValue;
		}
		break;
		// 当前法术值	
	case CREATURE_PROPID_MPCUR:
		{
			m_Property.nMPCur = nValue;
		}
		break;
		// 当前最大法术值
	case CREATURE_PROPID_MPMAXCUR:
		{
			m_Property.nMPMaxCur = nValue;
		}
		break;
		// PK值
	case CREATURE_PROPID_PKVALUE:
		{
			m_Property.nPKValue = nValue;
		}
		break;
		// 当前走路方式
	case CREATURE_PROPID_WALKSTYLE:
		{
			m_Property.nWalkStyle = nValue;
		}
		break;
		// 移动速度等级
	case CREATURE_PROPID_MOVESPEEDLEVEL:
		{
			m_Property.nMoveSpeedLevel = nValue;
			m_Property.nMoveSpeedLevel = m_Property.nMoveSpeedLevel < 0 ? 0 : m_Property.nMoveSpeedLevel;
			m_Property.nMoveSpeedLevel = m_Property.nMoveSpeedLevel >= MAX_MOVESPEED_LEVEL ? MAX_MOVESPEED_LEVEL - 1 : m_Property.nMoveSpeedLevel;	
		}
		break;
		// 装备栏容器ID
	case CREATURE_PROPID_EQUIPCONTAINERID:
		{
			m_Property.nEquipContainerID = nValue;
		}
		break;
		// 人物头像
	case CREATURE_PROPID_HEADRESID:
		{
			m_Property.nHeadResID = nValue;
		}
		break;
		// 名字颜色
	case CREATURE_PROPID_NAMECOLOR:
		{
			m_Property.nNameColor = nValue;
		}
		break;
		// 氏族ID
	case CREATURE_PROPID_CLANID:
		{
			m_Property.nClanID = nValue;
		}
		break;
	default:
		{
			PrintEventLn("error : CActor::GetPropNum : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return 0;
}
//------------------------------------------------------------------------
int CActor::GetPropNum(DWORD dwPropID)
{
	switch(dwPropID)
	{
		// 性别 （1 ：男，2 ：女）
	case CREATURE_PROPID_SEX:
		{
			return m_Property.nSex;
		}
		break;
		// 等级
	case CREATURE_PROPID_LEVEL:
		{
			return m_Property.nLevel;
		}
		break;
		// 种族
	case CREATURE_PROPID_PHYLE:
		{
			return m_Property.nPhyle;
		}
		break;
		// 职业
	case CREATURE_PROPID_OCCUPATION:
		{
			return m_Property.nOccupation;
		}
		break;
		// 当前生命值
	case CREATURE_PROPID_HPCUR:
		{
			return m_Property.nHPCur;
		}
		break;
		// 当前最大生命值
	case CREATURE_PROPID_HPMAXCUR:
		{
			return m_Property.nHPMaxCur;
		}
		break;
		// 当前法术值	
	case CREATURE_PROPID_MPCUR:
		{
			return m_Property.nMPCur;
		}
		break;
		// 当前最大法术值
	case CREATURE_PROPID_MPMAXCUR:
		{
			return m_Property.nMPMaxCur;
		}
		break;
		// UserID
	case CREATURE_PROPID_USERID:
		{
			return m_Property.nUserDBID;
		}
		break;
		// ActorID
	case CREATURE_PROPID_ACTORID:
		{
			return m_Property.nActorDBID;
		}
		break;
		// 装备栏容器ID
	case CREATURE_PROPID_EQUIPCONTAINERID:
		{
			return m_Property.nEquipContainerID;
		}
		break;
		// PK值
	case CREATURE_PROPID_PKVALUE:
		{
			return m_Property.nPKValue;
		}
		break;
		// 当前走路方式
	case CREATURE_PROPID_WALKSTYLE:
		{
			return m_Property.nWalkStyle;
		}
		break;
		// 移动速度等级
	case CREATURE_PROPID_MOVESPEEDLEVEL:
		{
			return  m_Property.nMoveSpeedLevel;
		}
		break;
		// 人物头像
	case CREATURE_PROPID_HEADRESID:
		{
			return m_Property.nHeadResID;
		}
		break;
		// 名字颜色
	case CREATURE_PROPID_NAMECOLOR:
		{
			return m_Property.nNameColor;
		}
		break;
		// 氏族ID
	case CREATURE_PROPID_CLANID:
		{
			return m_Property.nClanID;
		}
		break;
		// 方向
	case CREATURE_PROPID_DIR:
		{
			return m_Property.nDir;
		}
		break;
	default:
		{
			PrintEventLn("error : CActor::GetPropNum : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return 0;
}
//------------------------------------------------------------------------
LPCSTR CActor::SetPropStr(DWORD dwPropID, LPCSTR strPropValue)
{
	switch(dwPropID)
	{
		// 1  = 名字
	case CREATURE_PROPID_NAME:
		{
			lstrcpyn(m_Property.szName, strPropValue, sizeof(m_Property.szName));
			return m_Property.szName;
		}
		break;
	default:
		{
			PrintEventLn("error : CActor::GetStrProp : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return "";
}
//------------------------------------------------------------------------
LPCSTR CActor::GetPropStr(DWORD dwPropID)
{
	switch(dwPropID)
	{
		// 1  = 名字
	case CREATURE_PROPID_NAME:
		{
			return m_Property.szName;
		}
		break;
	default:
		{
			PrintEventLn("error : CActor::GetStrProp : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return "";
}
//------------------------------------------------------------------------
BOOL CActor::AddPart(IThingPart * pPart)
{
	if(pPart == NULL)
	{
		return FALSE;
	}

	if(pPart->GetPartID() < 0 || pPart->GetPartID() >= THINGPARTID_THING_MAXID)
	{
		return FALSE;
	}

	if(m_ThingPart[pPart->GetPartID()] != NULL)
	{
		return FALSE;
	}

	m_ThingPart[pPart->GetPartID()] = pPart;

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CActor::RemovePart(DWORD dwPartID)
{
	if(dwPartID < 0 || dwPartID >= THINGPARTID_THING_MAXID)
	{
		return FALSE;
	}

	m_ThingPart[dwPartID] = NULL;

	return TRUE;
}
//------------------------------------------------------------------------
IThingPart * CActor::GetPart(DWORD dwPartID)
{
	if(dwPartID < 0 || dwPartID >= THINGPARTID_THING_MAXID)
	{
		return NULL;
	}

	return m_ThingPart[dwPartID];	
}
//------------------------------------------------------------------------
DWORD CActor::GetUserDBID(void)
{
	return m_Property.nUserDBID;
}
//------------------------------------------------------------------------
DWORD CActor::GetActorDBID(void)
{
	return m_Property.nActorDBID;
}
//------------------------------------------------------------------------
DWORD CActor::GetActorClanID(void)
{
	return m_Property.nClanID;
}
//------------------------------------------------------------------------
BOOL CActor::GotoActState(DWORD dwActStateID, LPCSTR pContext, int nLen)
{
	return FALSE;
}
//------------------------------------------------------------------------
ICreatureActState * CActor::GetCurActState(void)
{
	return NULL;
}
//------------------------------------------------------------------------
void CActor::Accept(IMapItemVisitor &visitor)
{
	visitor.VisitActor();
}
//------------------------------------------------------------------------
CMapItemView * CActor::GetMapItem(void) const
{
	return m_pItemView;
}
//------------------------------------------------------------------------
void CActor::SetMapItem(CMapItemView * pMapItemView)
{
	m_pItemView = pMapItemView;
}
//------------------------------------------------------------------------
void CActor::OnCreatureMoveToTileCenter(const POINTS &ptCurTile, const POINTS &ptNextTile)
{

}
//------------------------------------------------------------------------
BOOL CActor::UpdateAppearance(void)
{
	IConfigDataCenter * pConfigDataCenter = g_pThingClientFacade->GetConfigDataCenter();
	if(pConfigDataCenter == NULL)
	{
		return FALSE;
	}

	int nOccupation = GetPropNum(CREATURE_PROPID_OCCUPATION);
	int nSex = GetPropNum(CREATURE_PROPID_SEX);
	CCreatureViewWrapper viewwrapper(m_pItemView);
	for(int i = 0; i < CrtPartMax; i++)
	{
		DWORD dwResID = pConfigDataCenter->GetNudePartForActor(nOccupation, i, nSex);
		viewwrapper.ChangeModule(i, dwResID);
	}

	return TRUE;	
}
//------------------------------------------------------------------------
BOOL CActor::AddCreatureCommand(ICreatureCommand * pCreatureCommand)
{
	return FALSE;
}
//------------------------------------------------------------------------
IBitmap * CActor::GetHeadBitmap(void)
{	
	if(m_pItemHead == NULL || m_dwOldHeadResID != m_Property.nHeadResID)
	{
		// 释放旧头像
		SAFE_RELEASE(m_pItemHead);

		// 创建新头像
		IItemClassFactory * pItemClassFactory = g_pThingClientFacade->GetItemFactory();
		if(pItemClassFactory == NULL)
		{
			return NULL;
		}

		IConfigDataCenter * pDataCenter = g_pThingClientFacade->GetConfigDataCenter();
		if(pDataCenter == NULL)
		{
			return NULL;
		}
		DWORD dwTargetResID = pDataCenter->GetHeadResIDByIndex(m_Property.nHeadResID, ACTORHEAD_RESTYPE_TARGET);

		m_pItemHead = (CMapItemView *)pItemClassFactory->CreateItem(miStaticObj_Picture, (char*)&dwTargetResID, sizeof(DWORD));
		if(m_pItemHead == NULL)
		{
			return NULL;
		}

		m_dwOldHeadResID = m_Property.nHeadResID;
	}	

	return m_pItemHead->GetBitmap();
}
//------------------------------------------------------------------------
void CActor::SetDying(BOOL bDying)
{
	m_bIsDying = bDying;
}
//------------------------------------------------------------------------
BOOL CActor::IsDying(void)
{
	return m_bIsDying;
}
//------------------------------------------------------------------------