/**
*	created:		2012-7-3   1:49
*	filename: 		CMonster
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		怪物
*/
//------------------------------------------------------------------------
#include <windows.h>
#include <mmsystem.h>
#include "../Include/ThingClientCommon.h"
#include "../Include/CMonster.h"
#include "../Include/CThingClientFacade.h"
#include "../Include/CreatureViewWrapper.h"
//------------------------------------------------------------------------
CMonster::CMonster(void)
{
	// 普通属性初始化
	m_uid = INVALID_UID;
	m_pItemView = NULL;
	m_pItemHead = NULL;
	m_bIsDying = FALSE;
	m_nLevel = 0;
	m_nHPMaxCur = 0;
	m_nMPMaxCur = 0;

	memset(&m_Property, 0, sizeof(m_Property));
	ZeroMemory(m_ThingPart, sizeof(m_ThingPart));

	// 逻辑对像类型初始化
	m_ThingClass.SetThingClass(THINGCLASS_MONSTER);
	m_ThingClass.SetCreature();
}
//------------------------------------------------------------------------
CMonster::~CMonster(void)
{

}
//------------------------------------------------------------------------
void CMonster::Release(void)
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
BOOL CMonster::Create(void)
{
	// 创建地图对像
	struct stTemp
	{
		int				nMonsterResID;		// 怪物资源ID	
		ICreatureSink * pCrtSink;			// 
	}tempdata;

	IConfigDataCenter * pConfigDataCenter = g_pThingClientFacade->GetConfigDataCenter();
	if(pConfigDataCenter == NULL)
	{
		TraceLn("%s创建失败，无法取得数据中心", GetName());
		return FALSE;
	}

	if(m_Property.ptLoc.x == 0 || m_Property.ptLoc.y == 0)
	{
		TraceLn("%s创建失败，坐标为零坐标！", GetName());
		return FALSE;
	}

	int nMonsterID = GetMonsterID();
	MONSTER_CONFIG_INFOR monsterinfor = pConfigDataCenter->GetMonsterInfor(GetMonsterID());
	if(monsterinfor.lmonsterid == 0)
	{
		TraceLn("%s创建失败，无法取得配置数值", GetName());
		return FALSE;
	}
	m_nLevel	= monsterinfor.llevel;
	m_nHPMaxCur = monsterinfor.lhp;
	m_nMPMaxCur = monsterinfor.lmp;

	tempdata.nMonsterResID	= monsterinfor.lresid;
	tempdata.pCrtSink		= static_cast<ICreatureSink *>(this);
	IItemClassFactory * pItemClassFactory = g_pThingClientFacade->GetItemFactory();
	if(pItemClassFactory == NULL)
	{
		return FALSE;
	}
	
	int nDataLen = sizeof(tempdata);
	m_pItemView = (CMapItemView *)pItemClassFactory->CreateItem(miNpcObj, (const char *)&tempdata, nDataLen);
	if(m_pItemView == NULL)
	{
		return FALSE;
	}
	
	// 各种外观属性设置
	CCreatureViewWrapper viewwrapper(m_pItemView);
	viewwrapper.SetData((DWORD)this);
	viewwrapper.AddToMap(m_Property.ptLoc);
	viewwrapper.SetTile(m_Property.ptLoc);
	viewwrapper.SetName(GetName());
	viewwrapper.AddFlag(CMapItemView::flagCanbeSelected);
	viewwrapper.SetDirection(m_Property.nDir);
	int nHPCur = GetPropNum(CREATURE_PROPID_HPCUR);
	int nHPMax = GetPropNum(CREATURE_PROPID_HPMAXCUR);
	viewwrapper.SetBloodInfo(GetPropNum(CREATURE_PROPID_HPCUR), max(nHPCur, nHPMax));
	
	viewwrapper.AddFlag(CMapItemView::flagCanbeSelected);
	viewwrapper.SetMoveSpeedLevel((int *)&m_Property.nMoveSpeedLevel);
	viewwrapper.Born();

	switch(monsterinfor.ltype)
	{
	case MONSTER_TYPE_GENERAL:
	case MONSTER_TYPE_EXCELLENT:
	case MONSTER_TYPE_RARE:
	case MONSTER_TYPE_BOSS:
	case MONSTER_TYPE_BEASTIE:
		{
			viewwrapper.SetFlagColorOnMinmap(255, 0, 0);
		}
		break;
	case MONSTER_TYPE_SAFEGUARD:
	case MONSTER_TYPE_HATCHETMAN:
		{
			viewwrapper.SetFlagColorOnMinmap(126, 0, 252);
		}
		break;
	case MONSTER_TYPE_NOATTACKNPC:
	case MONSTER_TYPE_ATTACKNPC:
		{
			viewwrapper.SetNameColor(252, 136, 0);
			viewwrapper.SetFlagColorOnMinmap(253, 167, 1);			
		}
		break;
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
void CMonster::SetUID(UID uid)
{
	m_uid = uid;
}
//------------------------------------------------------------------------
UID	CMonster::GetUID(void)
{
	return m_uid;
}
//------------------------------------------------------------------------
DWORD CMonster::GetMapID(void)
{
	return GETUIDSERVERID(m_uid);
}
//------------------------------------------------------------------------
void CMonster::SetLoc(POINTS ptLoc)
{
	m_Property.ptLoc = ptLoc;
}
//------------------------------------------------------------------------
POINTS CMonster::GetLoc(void)
{
	CCreatureViewWrapper viewwrapper(m_pItemView);
	
	POINT pos = viewwrapper.GetTile();
	POINTS loc = { pos.x, pos.y };

	return loc;
}
//------------------------------------------------------------------------
IThingClass * CMonster::GetThingClass(void)
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
BOOL CMonster::OnSetPrivateContext(LPCSTR buf, int nLen)
{
	if(buf == NULL || nLen != sizeof(SMONSTER_PROPERTY_PUBLIC))
	{
		TraceLn("CMonster::OnSetPrivateContext Failed! [nLen != sizeof(SMONSTER_PROPERTY_PUBLIC)]");
		return FALSE;
	}

	memcpy(&m_Property, buf, sizeof(SMONSTER_PROPERTY_PUBLIC));
	m_Property.nMoveSpeedLevel = m_Property.nMoveSpeedLevel < 0 ? 0 : m_Property.nMoveSpeedLevel;
	m_Property.nMoveSpeedLevel = m_Property.nMoveSpeedLevel >= MAX_MOVESPEED_LEVEL ? MAX_MOVESPEED_LEVEL - 1 : m_Property.nMoveSpeedLevel;

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMonster::OnGetPrivateContext(LPSTR buf, int &nLen)
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
BOOL CMonster::OnSetPublicContext(LPCSTR buf, int nLen)
{
	return OnSetPrivateContext(buf, nLen);
}
//------------------------------------------------------------------------
BOOL CMonster::OnGetPublicContext(LPSTR buf, int &nLen)
{	
	// 客户端不可能这样调的
	return FALSE;
}
//------------------------------------------------------------------------
// 管理thing的数据库现场
// 说明：
// OnGetDBContext() 用于将对象现场保护到数据库
// OnSetDBContext() 用于通过数据库返回的数据创建对象
BOOL CMonster::OnSetDBContext(LPCSTR buf, int nLen)
{	
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CMonster::OnGetDBContext(LPSTR buf, int &nLen)
{
	return FALSE;
}
//------------------------------------------------------------------------
int CMonster::OnCommand(DWORD dwCommand, LPCSTR buf, int nLen)
{
	return 0;
}
//------------------------------------------------------------------------
void CMonster::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return;
	}

	/*
	// 先判定否决是否通过
	if(pEventServer->FireVote((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_MONSTER, (DWORD)this) == FALSE)
	{
		return;
	}

	// 然后行为事件
	pEventServer->FireAction((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_MONSTER, (DWORD)this);
	*/
}
//------------------------------------------------------------------------
void CMonster::OnResponse(DWORD dwMsg, LPCSTR buf, int nLen)
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
BOOL CMonster::SubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_MONSTER, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述：取消订阅本对象的否决事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventVoteListener * pListener  - 事件否决者	
// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CMonster::UnSubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_MONSTER, (DWORD)this);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述: 订阅本对象的行为事件
// 参  数: [输入] DWORD dwMsg - 事件ID
// 参  数: [输入] IEventActionListener * pListener - 事件行为执行者
// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CMonster::SubscibeMessage(DWORD dwMsg, IEventActionListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_MONSTER, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventActionListener * pListener  - 事件行为执行者
// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CMonster::UnSubscibeMessage(DWORD dwMsg, IEventActionListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_MONSTER, (DWORD)this);	
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述: 订阅本对象的响应事件
// 参  数: [输入] DWORD dwMsg - 事件ID
// 参  数: [输入] IEventResponseListener * pListener - 事件响应者
// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CMonster::SubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_MONSTER, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventResponseListener * pListener  - 事件响应者
// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CMonster::UnSubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_MONSTER, (DWORD)this);	
	*/
	return TRUE;
}
//------------------------------------------------------------------------
LPCSTR CMonster::GetName(void)
{
	return GetPropStr(CREATURE_PROPID_NAME);
}
//------------------------------------------------------------------------
int	CMonster::SetPropNum(DWORD dwPropID, int nValue)
{
	switch(dwPropID)
	{
		// 等级
	case CREATURE_PROPID_LEVEL:
		{
			m_nLevel = nValue;
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
			m_nHPMaxCur = nValue;
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
			m_nMPMaxCur = nValue;
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
		// 怪物阵营
	case CREATURE_PROPID_MONSTERCAMP:
		{
			m_Property.nMonsterCamp = nValue;
		}
		break;
	default:
		{
			PrintEventLn("error : CMonster::SetPropNum : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return 0;
}
//------------------------------------------------------------------------
int CMonster::GetPropNum(DWORD dwPropID)
{
	switch(dwPropID)
	{
		// 等级
	case CREATURE_PROPID_LEVEL:
		{
			return m_nLevel;
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
			return m_nHPMaxCur;
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
			return m_nMPMaxCur;
		}
		break;
		// 怪物ID
	case CREATURE_PROPID_MONSTERID:
		{
			return m_Property.nMonsterID;
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
		// 方向
	case CREATURE_PROPID_DIR:
		{
			return m_Property.nDir;
		}
		break;
		// 怪物阵营
	case CREATURE_PROPID_MONSTERCAMP:
		{
			return m_Property.nMonsterCamp;
		}
		break;
	default:
		{
			PrintEventLn("error : CMonster::GetPropNum : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return 0;
}
//------------------------------------------------------------------------
LPCSTR CMonster::SetPropStr(DWORD dwPropID, LPCSTR strPropValue)
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
			PrintEventLn("error : CMonster::SetPropStr : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return "";
}
//------------------------------------------------------------------------
LPCSTR CMonster::GetPropStr(DWORD dwPropID)
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
BOOL CMonster::AddPart(IThingPart * pPart)
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
BOOL CMonster::RemovePart(DWORD dwPartID)
{
	if(dwPartID < 0 || dwPartID >= THINGPARTID_THING_MAXID)
	{
		return FALSE;
	}

	m_ThingPart[dwPartID] = NULL;

	return TRUE;
}
//------------------------------------------------------------------------
IThingPart * CMonster::GetPart(DWORD dwPartID)
{
	if(dwPartID < 0 || dwPartID >= THINGPARTID_THING_MAXID)
	{
		return NULL;
	}

	return m_ThingPart[dwPartID];	
}
//------------------------------------------------------------------------
BOOL CMonster::GotoActState(DWORD dwActStateID, LPCSTR pContext, int nLen)
{
	return FALSE;
}
//------------------------------------------------------------------------
ICreatureActState * CMonster::GetCurActState(void)
{
	return NULL;
}
//------------------------------------------------------------------------
void CMonster::Accept(IMapItemVisitor &visitor)
{
	visitor.VisitMonster();
}
//------------------------------------------------------------------------
CMapItemView * CMonster::GetMapItem(void) const
{
	return m_pItemView;
}
//------------------------------------------------------------------------
void CMonster::SetMapItem(CMapItemView * pMapItemView)
{
	m_pItemView = pMapItemView;
}
//------------------------------------------------------------------------
void CMonster::OnCreatureMoveToTileCenter(const POINTS &ptCurTile, const POINTS &ptNextTile)
{

}
//------------------------------------------------------------------------
BOOL CMonster::AddCreatureCommand(ICreatureCommand * pCreatureCommand)
{
	return FALSE;
}
//------------------------------------------------------------------------
IBitmap * CMonster::GetHeadBitmap(void)
{	
	if(m_pItemHead == NULL)
	{
		IConfigDataCenter * pDataCenter = g_pThingClientFacade->GetConfigDataCenter();
		if(pDataCenter == NULL)
		{
			return NULL;
		}

		MONSTER_CONFIG_INFOR & monsterinfor = pDataCenter->GetMonsterInfor(m_Property.nMonsterID);
		if(monsterinfor.lmonsterid == 0)
		{
			return NULL;
		}

		IItemClassFactory * pItemClassFactory = g_pThingClientFacade->GetItemFactory();
		if(pItemClassFactory == NULL)
		{
			return NULL;
		}

		m_pItemHead = (CMapItemView *)pItemClassFactory->CreateItem(miStaticObj_Picture, (char*)&(monsterinfor.lheadresid), sizeof(DWORD));
		if(m_pItemHead == NULL)
		{
			return NULL;
		}
	}	

	return m_pItemHead->GetBitmap();
}
//------------------------------------------------------------------------
DWORD CMonster::GetMonsterID(void)
{
	return GetPropNum(CREATURE_PROPID_MONSTERID);
}
//------------------------------------------------------------------------
void CMonster::SetDying(BOOL bDying)
{
	m_bIsDying = bDying;
}
//------------------------------------------------------------------------
BOOL CMonster::IsDying(void)
{
	return m_bIsDying;
}
//------------------------------------------------------------------------