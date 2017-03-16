/**
*	created:		2012-7-2   22:43
*	filename: 		CClientThingFactory
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		传入一个逻辑对象的类型，和现场，返回一个逻辑对象指针
					逻辑对象包括地图上的人,怪物,NPC,装备,传送点等
*/
//------------------------------------------------------------------------
#include "../Include/CClientThingFactory.h"
#include "../Include/ThingClientCommon.h"
#include "../Include/CHero.h"
#include "../Include/CActor.h"
#include "../Include/CMonster.h"
#include "../Include/CThingClientFacade.h"
#include "../Include/CreatureCommandBase.h"
#include "../Include/CEquipment.h"
#include "../Include/CMedicament.h"
#include "../Include/CValuables.h"
#include "../Include/CWarrent.h"
#include "../Include/CChunnel.h"
#include "../Include/HeroPartEquip.h"
#include "../Include/ActorPartEquip.h"
#include "../Include/HeroPartPacket.h"
#include "../Include/CBox.h"
#include "../Include/CStall.h"
#include "../Include/CreaturePartConjure.h"
#include "../Include/ActorPartRide.h"
#include "../Include/HeroPartRide.h"
//------------------------------------------------------------------------
typedef IThing * (CThingFactory:: *THING_CREATE_PROC)(UID uid, LPCSTR buf, int nLen);
//------------------------------------------------------------------------
// 每一个逻辑对象你可以选择是用详细的现场创建还是用粗略的现场创建
// 典型应用是游戏中的人物，你可能需要看到自己的攻击力防御力等各种属性
// 包括物品栏的每一件物品，而你看别人只能看到他们的角色类型,性别
// 装备等， 他们的详细属性或者物品栏的物品服务器是不会发下来的
// 所以需要支持两种方式创建
IThing * CThingFactory::CreatePrivateThing(DWORD dwThingClassID, UID uid, const char * buf, int nLen)
{
	static THING_CREATE_PROC createproc[THINGCLASS_MAX] =
	{
		0											,	// THINGCLASS_NONE	
		(THING_CREATE_PROC)&CThingFactory::CreateActor_private		,	// THINGCLASS_ACTOR
		0											,	// THINGCLASS_MONSTER
		0											,	// THINGCLASS_CONTAINER
		(THING_CREATE_PROC)&CThingFactory::CreateGoods_private		,	// THINGCLASS_GOODS
		0											,	// THINGCLASS_CHUNNEL
		0											,   // THINGCLASS_BOX
		0											,	// THINGCLASS_STALL
	};

	if(dwThingClassID < 0 || dwThingClassID >= THINGCLASS_MAX || createproc[dwThingClassID] == NULL)
	{
		TraceLn("CreateThing() : dwThingClassID = %d is Invalid!", dwThingClassID);
		return NULL;
	}

	IThing * pThing = (this->*createproc[dwThingClassID])(uid, buf, nLen);

	if(pThing == NULL)
	{
		TraceLn("CreateThing() : dwThingClassID = %d Failed!", dwThingClassID);
	}
	else
	{
		// 广播一事件出去，告知创建一新的逻辑对象
		WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_BORN);
		pThing->OnMessage(wMsgID, (LPCSTR)buf, nLen);
	}

	return pThing;
}
//------------------------------------------------------------------------
IThing * CThingFactory::CreatePublicThing(DWORD dwThingClassID, UID uid, const char * buf, int nLen)
{
	static THING_CREATE_PROC createproc[THINGCLASS_MAX] =
	{
		0											,	// THINGCLASS_NONE	
		(THING_CREATE_PROC)&CThingFactory::CreateActor_public		,	// THINGCLASS_ACTOR
		(THING_CREATE_PROC)&CThingFactory::CreateMonster_public		,	// THINGCLASS_MONSTER
		0											,	// THINGCLASS_CONTAINER
		(THING_CREATE_PROC)&CThingFactory::CreateGoods_public		,	// THINGCLASS_GOODS
		(THING_CREATE_PROC)&CThingFactory::CreateChunnel_public		,	// THINGCLASS_CHUNNEL
		(THING_CREATE_PROC)&CThingFactory::CreateBox_public         ,   // THINGCLASS_BOX
		(THING_CREATE_PROC)&CThingFactory::CreateStall_public		,	// THINGCLASS_STALL
	};

	if(dwThingClassID < 0 || dwThingClassID >= THINGCLASS_MAX || createproc[dwThingClassID] == NULL)
	{
		TraceLn("CreateThing() : dwThingClassID = %d is Invalid!", dwThingClassID);
		return NULL;
	}

	IThing * pThing = (this->*createproc[dwThingClassID])(uid, buf, nLen);

	if(pThing == NULL)
	{
		TraceLn("CreateThing() : dwThingClassID = %d Failed!", dwThingClassID);
	}
	else
	{		
		// 发布一事件出去，告知创建一新的逻辑对象
		WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_BORN);
		pThing->OnMessage(wMsgID, (LPCSTR)buf, nLen);
	}
		
	return pThing;
}
//------------------------------------------------------------------------
void CThingFactory::Release(void)
{
	delete this;
}
//------------------------------------------------------------------------
IThing * CThingFactory::CreateActor_private(UID uid, LPCSTR buf, int nLen)
{
	if(uid == INVALID_UID || buf == NULL || nLen <= 0)
	{
		return NULL;
	}
	PrintEventLn("开始创建主角! [uid = %08X%08X]", uid);

	// 设置现场
	CHero * pHero = new CHero();
	pHero->SetUID(uid);
	if(pHero->OnSetPrivateContext(buf, nLen) == FALSE)
	{
		PrintEventLn("设置主角属性失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pHero);
		return NULL;
	}

	// 创建
	if(pHero->Create() == FALSE)
	{
		PrintEventLn("创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pHero);
		return NULL;
	}
	/*
	// 创建装备栏部件
	CHeroPart_Equip * pHeroPart_Equip = new CHeroPart_Equip();
	if(!pHeroPart_Equip->Create(pHero, NULL, 0))
	{
		PrintEventLn("创建装备栏部件失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pHeroPart_Equip);
		SAFE_RELEASE(pHero);		
		return NULL;
	}	

	// 创建包裹栏部件
	CHeroPart_Packet * pHeroPart_Packet = new CHeroPart_Packet();
	if(!pHeroPart_Packet->Create(pHero, NULL, 0))
	{
		PrintEventLn("创建包裹栏部件失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pHeroPart_Packet);
		SAFE_RELEASE(pHeroPart_Equip);
		SAFE_RELEASE(pHero);		
		return NULL;
	}

	// 创建冷却系统部件

	IColdTimeMgr * pColdTimeMgr = g_pThingClientFacade->GetColdTimeSystem()->CreateColdTimeMgr();
	if(pColdTimeMgr == NULL)
	{
		PrintEventLn("创建冷却系统部件失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pHeroPart_Packet);
		SAFE_RELEASE(pHeroPart_Equip);
		SAFE_RELEASE(pHero);
		return NULL;
	}
	pColdTimeMgr->Create(pHero, NULL, 0);

	// 创建状态部件
	IStatusClient * pStatusClient = g_pThingClientFacade->GetStatusClient();
	if(pStatusClient == NULL)
	{
		PrintEventLn("取得状态模块失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pColdTimeMgr);
		SAFE_RELEASE(pHeroPart_Packet);
		SAFE_RELEASE(pHeroPart_Equip);
		SAFE_RELEASE(pHero);
		return NULL;
	}

	IStatusMgr * pStatusMgr = pStatusClient->CreateStatusMgr();
	if(pStatusMgr == NULL)
	{
		PrintEventLn("创建状态部件失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pColdTimeMgr);
		SAFE_RELEASE(pHeroPart_Packet);
		SAFE_RELEASE(pHeroPart_Equip);
		SAFE_RELEASE(pHero);
		return NULL;
	}
	pStatusMgr->Create(pHero, NULL, 0);	

	// 创建PK部件
	IClientPKSystem * pClientPKSystem = g_pThingClientFacade->GetPKSystem();
	if(pClientPKSystem == NULL)
	{
		PrintEventLn("取得PK系统失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pStatusMgr);
		SAFE_RELEASE(pColdTimeMgr);
		SAFE_RELEASE(pHeroPart_Packet);
		SAFE_RELEASE(pHeroPart_Equip);
		SAFE_RELEASE(pHero);
		return NULL;
	}
	IClientPKManager * pPKManager = pClientPKSystem->CreatePlayerPKManager(pHero, 0, 0);
	if(pPKManager == NULL)
	{
		PrintEventLn("创建PK部件失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pStatusMgr);
		SAFE_RELEASE(pColdTimeMgr);
		SAFE_RELEASE(pHeroPart_Packet);
		SAFE_RELEASE(pHeroPart_Equip);
		SAFE_RELEASE(pHero);
		return NULL;
	}

	// 创建工作技能部件
	IWorkSkillSystem * pWorkSkillSystem = g_pThingClientFacade->GetWorkSkillSystem();
	if(pWorkSkillSystem == NULL)
	{
		PrintEventLn("取得工作技能系统失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pPKManager);
		SAFE_RELEASE(pStatusMgr);
		SAFE_RELEASE(pColdTimeMgr);
		SAFE_RELEASE(pHeroPart_Packet);
		SAFE_RELEASE(pHeroPart_Equip);
		SAFE_RELEASE(pHero);
		return NULL;
	}
	IWorkSkillMgr * pWorkSkillMgr = NULL;	
	if(!pWorkSkillSystem->CreateWorkSkillMgr(&pWorkSkillMgr) || pWorkSkillMgr == NULL)
	{
		PrintEventLn("创建工作技能部件失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pWorkSkillMgr);
		SAFE_RELEASE(pPKManager);
		SAFE_RELEASE(pStatusMgr);
		SAFE_RELEASE(pColdTimeMgr);
		SAFE_RELEASE(pHeroPart_Packet);
		SAFE_RELEASE(pHeroPart_Equip);
		SAFE_RELEASE(pHero);
		return NULL;
	}
	pWorkSkillMgr->Create(pHero, NULL, 0);	

	// 创建技能控制器
	ISkillManager * pSkillManager = g_pThingClientFacade->GetSkillManager();
	if(pSkillManager == NULL)
	{
		PrintEventLn("取得技能系统失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pWorkSkillMgr);
		SAFE_RELEASE(pPKManager);
		SAFE_RELEASE(pStatusMgr);
		SAFE_RELEASE(pColdTimeMgr);
		SAFE_RELEASE(pHeroPart_Packet);
		SAFE_RELEASE(pHeroPart_Equip);
		SAFE_RELEASE(pHero);
		return NULL;
	}
	ISkillControl * pSkillControl = pSkillManager->CreateSkillControl();
	if(pSkillControl == NULL)
	{
		PrintEventLn("创建技能部件失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pWorkSkillMgr);
		SAFE_RELEASE(pPKManager);
		SAFE_RELEASE(pStatusMgr);
		SAFE_RELEASE(pColdTimeMgr);
		SAFE_RELEASE(pHeroPart_Packet);
		SAFE_RELEASE(pHeroPart_Equip);
		SAFE_RELEASE(pHero);
		return NULL;
	}

	// 氏族部件
	IClanClient * pClanClient = g_pThingClientFacade->GetClanClient();
	IClanClientPate * pClanPart = NULL;
	if(pClanClient == NULL || (pClanPart = pClanClient->GetClanPatePart()) == NULL)
	{
		PrintEventLn("创建氏族部件失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pSkillControl);
		SAFE_RELEASE(pWorkSkillMgr);
		SAFE_RELEASE(pPKManager);
		SAFE_RELEASE(pStatusMgr);
		SAFE_RELEASE(pColdTimeMgr);
		SAFE_RELEASE(pHeroPart_Packet);
		SAFE_RELEASE(pHeroPart_Equip);
		SAFE_RELEASE(pHero);
		return NULL;
	}
	pClanPart->Create(pHero, NULL, 0);
	

	// 召唤部件
	CCreaturePart_Conjure * pConjurePart = new CCreaturePart_Conjure();
	if(!pConjurePart->Create(pHero, NULL, 0))
	{
		PrintEventLn("创建召唤部件失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pConjurePart);
		//SAFE_RELEASE(pClanClient);
		//SAFE_RELEASE(pSkillControl);
		//SAFE_RELEASE(pWorkSkillMgr);
		//SAFE_RELEASE(pPKManager);
		//SAFE_RELEASE(pStatusMgr);
		//SAFE_RELEASE(pColdTimeMgr);
		SAFE_RELEASE(pHeroPart_Packet);
		SAFE_RELEASE(pHeroPart_Equip);
		SAFE_RELEASE(pHero);
		return NULL;
	}

	// 骑乘部件
	CHeroPart_Ride * pRidePart = new CHeroPart_Ride();
	if(!pRidePart->Create(pHero, NULL, 0))
	{
		PrintEventLn("创建骑乘部件失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pRidePart);
		SAFE_RELEASE(pConjurePart);
		//SAFE_RELEASE(pClanClient);
		//SAFE_RELEASE(pSkillControl);
		//SAFE_RELEASE(pWorkSkillMgr);
		//SAFE_RELEASE(pPKManager);
		//SAFE_RELEASE(pStatusMgr);
		//SAFE_RELEASE(pColdTimeMgr);
		SAFE_RELEASE(pHeroPart_Packet);
		SAFE_RELEASE(pHeroPart_Equip);
		SAFE_RELEASE(pHero);
		return NULL;
	}

	// 添加部件
	pHero->AddPart(static_cast<IThingPart *>(pHeroPart_Equip));
	pHero->AddPart(static_cast<IThingPart *>(pHeroPart_Packet));
	//pHero->AddPart(static_cast<IThingPart *>(pColdTimeMgr));
	//pHero->AddPart(static_cast<IThingPart *>(pStatusMgr));
	//pHero->AddPart(static_cast<IThingPart *>(pPKManager));
	//pHero->AddPart(static_cast<IThingPart *>(pWorkSkillMgr));
	//pHero->AddPart(static_cast<IThingPart *>(pSkillControl));
	//pHero->AddPart(static_cast<IThingPart *>(pClanPart));
	pHero->AddPart(static_cast<IThingPart *>(pConjurePart));
	pHero->AddPart(static_cast<IThingPart *>(pRidePart));

	// 添加命令
	pHero->AddCreatureCommand(CCreatureCommand_Stand::GetInstance());
	pHero->AddCreatureCommand(CCreatureCommand_ChanageMoveStyle::GetInstance());
*/
	PrintEventLn("创建主角成功！");
	return pHero;
}
//------------------------------------------------------------------------
#define CREATE_THING_PUBLIC_TEMPLATE(pThing)							\
		if(pThing == NULL)												\
		{																\
			return NULL;												\
		}																\
																		\
		pThing->SetUID(uid);											\
		if(pThing->OnSetPublicContext(buf, nLen) == FALSE)				\
		{																\
			SAFE_RELEASE(pThing);										\
			return NULL;												\
		}																\
																		\
		if(pThing->Create() == FALSE)									\
		{																\
			SAFE_RELEASE(pThing);										\
			return NULL;												\
		}

//------------------------------------------------------------------------
#define CREATE_THING_PRIVATE_TEMPLATE(pThing)							\
		if(pThing == NULL)												\
		{																\
			return NULL;												\
		}																\
																		\
		pThing->SetUID(uid);											\
		if(pThing->OnSetPrivateContext(buf, nLen) == FALSE)				\
		{																\
			SAFE_RELEASE(pThing);										\
			return NULL;												\
		}																\
																		\
		if(pThing->Create() == FALSE)									\
		{																\
			SAFE_RELEASE(pThing);										\
			return NULL;												\
		}

//------------------------------------------------------------------------
IThing * CThingFactory::CreateActor_public(UID uid, LPCSTR buf, int nLen)
{
	CActor * pActor = new CActor();
	pActor->SetUID(uid);
	if(!pActor->OnSetPublicContext(buf, nLen))
	{
		PrintEventLn("设置角色属性失败! [uid = %08X%08X]", uid);
		SAFE_RELEASE(pActor);
		return NULL;
	}
	
	if(!pActor->Create())
	{
		PrintEventLn("创建角色失败! [uid = %08X%08X] [%s]", uid, pActor->GetName());
		SAFE_RELEASE(pActor);
		return NULL;
	}

	// 创建装备栏部件
	CActorPart_Equip * pActorPart_Equip = new CActorPart_Equip();
	if(!pActorPart_Equip->Create(pActor, NULL, 0))
	{
		PrintEventLn("创建装备栏部件失败！[uid = %08X%08X] [%s]", uid, pActor->GetName());		
		SAFE_RELEASE(pActorPart_Equip);
		SAFE_RELEASE(pActor);
		return NULL;
	}
/*
	// 创建状态部件
	IStatusClient * pStatusClient = g_pThingClientFacade->GetStatusClient();
	if(pStatusClient == NULL)
	{
		PrintEventLn("取得状态部件系统失败！[uid = %08X%08X] [%s]", uid, pActor->GetName());		
		SAFE_RELEASE(pActorPart_Equip);
		SAFE_RELEASE(pActor);
		return NULL;
	}
	IStatusMgr * pStatusMgr = pStatusClient->CreateStatusMgr();
	if(pStatusMgr == NULL)
	{
		PrintEventLn("创建状态部件失败！[uid = %08X%08X] [%s]", uid, pActor->GetName());		
		SAFE_RELEASE(pActorPart_Equip);
		SAFE_RELEASE(pActor);
		return NULL;
	}
	pStatusMgr->Create(pActor, NULL, 0);	

	// 氏族部件
	IClanClient * pClanClient = g_pThingClientFacade->GetClanClient();
	IClanClientPate * pClanPart = NULL;
	if(pClanClient == NULL || (pClanPart = pClanClient->GetClanPatePart()) == NULL)
	{
		PrintEventLn("创建氏族部件失败！[uid = %08X%08X] [%s]", uid, pActor->GetName());
		SAFE_RELEASE(pStatusMgr);		
		SAFE_RELEASE(pActorPart_Equip);
		SAFE_RELEASE(pActor);
		return NULL;
	}
	pClanPart->Create(pActor, NULL, 0);
*/
	// 召唤部件
	CCreaturePart_Conjure * pConjurePart = new CCreaturePart_Conjure();
	if(!pConjurePart->Create(pActor, NULL, 0))
	{
		PrintEventLn("创建召唤部件失败！[uid = %08X%08X] [%s]", uid, pActor->GetName());
		SAFE_RELEASE(pConjurePart);
		// SAFE_RELEASE(pStatusMgr);		
		SAFE_RELEASE(pActorPart_Equip);
		SAFE_RELEASE(pActor);
		return NULL;
	}

	// 骑乘部件
	CHeroPart_Ride * pRidePart = new CHeroPart_Ride();
	if(!pRidePart->Create(pActor, NULL, 0))
	{
		PrintEventLn("创建骑乘部件失败！[uid = %08X%08X] [%s]", uid, pActor->GetName());
		SAFE_RELEASE(pRidePart);
		SAFE_RELEASE(pConjurePart);
		// SAFE_RELEASE(pStatusMgr);		
		SAFE_RELEASE(pActorPart_Equip);
		SAFE_RELEASE(pActor);
		return NULL;
	}
/*
	// 创建PK部件
	IClientPKSystem * pClientPKSystem = g_pThingClientFacade->GetPKSystem();
	if(pClientPKSystem == NULL)
	{
		PrintEventLn("取得PK系统失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pRidePart);
		SAFE_RELEASE(pConjurePart);
		SAFE_RELEASE(pStatusMgr);		
		SAFE_RELEASE(pActorPart_Equip);
		SAFE_RELEASE(pActor);
		return NULL;
	}
	IClientPKManager * pPKManager = pClientPKSystem->CreatePlayerPKManager(pActor, 0, 0);
	if(pPKManager == NULL)
	{
		PrintEventLn("创建PK部件失败，创建主角失败！[uid = %08X%08X]", uid);
		SAFE_RELEASE(pRidePart);
		SAFE_RELEASE(pConjurePart);
		SAFE_RELEASE(pStatusMgr);		
		SAFE_RELEASE(pActorPart_Equip);
		SAFE_RELEASE(pActor);
		return NULL;
	}
*/	
	// 添加部件
	pActor->AddPart(static_cast<IThingPart *>(pActorPart_Equip));
	// pActor->AddPart(static_cast<IThingPart *>(pStatusMgr));
	// pActor->AddPart(static_cast<IThingPart *>(pClanPart));
	pActor->AddPart(static_cast<IThingPart *>(pConjurePart));
	pActor->AddPart(static_cast<IThingPart *>(pRidePart));
	// pActor->AddPart(static_cast<IThingPart *>(pPKManager));

	return pActor;
}
//------------------------------------------------------------------------
// 描  述：创建怪物
IThing * CThingFactory::CreateMonster_public(UID uid, LPCSTR buf, int nLen)
{
	CMonster * pMonster = new CMonster();
	CREATE_THING_PUBLIC_TEMPLATE(pMonster);	

/*
	// 创建状态部件
	IStatusClient * pStatusClient = g_pThingClientFacade->GetStatusClient();
	if(pStatusClient == NULL)
	{
		PrintEventLn("取得状态部件系统失败！[uid = %08X%08X] [%s]", uid, pMonster->GetName());
		SAFE_RELEASE(pMonster);
		return NULL;
	}
	IStatusMgr * pStatusMgr = pStatusClient->CreateStatusMgr();
	if(pStatusMgr == NULL)
	{
		PrintEventLn("创建状态部件失败！[uid = %08X%08X] [%s]", uid, pMonster->GetName());
		SAFE_RELEASE(pMonster);
		return NULL;
	}
	pStatusMgr->Create(pMonster, NULL, 0);
*/

	// 召唤部件
	CCreaturePart_Conjure * pConjurePart = new CCreaturePart_Conjure();
	if(!pConjurePart->Create(pMonster, NULL, 0))
	{
		PrintEventLn("创建召唤部件失败！[uid = %08X%08X] [%s]", uid, pMonster->GetName());
		SAFE_RELEASE(pConjurePart);
		// SAFE_RELEASE(pStatusMgr);
		SAFE_RELEASE(pMonster);
		return NULL;
	}

	// 添加部件
	// pMonster->AddPart(static_cast<IThingPart *>(pStatusMgr));
	pMonster->AddPart(static_cast<IThingPart *>(pConjurePart));

	return static_cast<IThing *>(pMonster);;
}
//------------------------------------------------------------------------
// 描  述：创建物品
IThing * CThingFactory::CreateGoods_private(UID uid, LPCSTR buf, int nLen)
{
	if(nLen < sizeof(SGOODS_PROPERTY_BASE))
	{
		return NULL;
	}

	SGOODS_PROPERTY_BASE * pPropertyBase = (SGOODS_PROPERTY_BASE *)(buf);
	// 取得是否为（装备，药品，凭证，元宝）
	IConfigDataCenter * pConfigDataCenter = g_pThingClientFacade->GetConfigDataCenter();
	if(pConfigDataCenter == NULL)
	{
		return NULL;
	}

	GOODS_BASEPROPERTY baseproperty = pConfigDataCenter->GetGoodsBaseProperty(pPropertyBase->dwGoodsID);
	if(baseproperty.lClass == GOODS_CLASS_EQUIPMENT)
	{
		CEquipment * pEquipment = new CEquipment();
		CREATE_THING_PRIVATE_TEMPLATE(pEquipment);

		return static_cast<IThing *>(pEquipment);		
	}
	else if(baseproperty.lClass == GOODS_CLASS_MEDICAMENT)
	{
		CMedicament * pMedicament = new CMedicament();
		CREATE_THING_PRIVATE_TEMPLATE(pMedicament);

		return static_cast<IThing *>(pMedicament);	
	}
	else if(baseproperty.lClass == GOODS_CLASS_WARRANT)
	{
		CWarrant * pWarrant = new CWarrant();
		CREATE_THING_PRIVATE_TEMPLATE(pWarrant);

		return static_cast<IThing *>(pWarrant);	
	}
	else if(baseproperty.lClass == GOODS_CLASS_VALUABLES)
	{
		CValuables * pValuables = new CValuables();
		CREATE_THING_PRIVATE_TEMPLATE(pValuables);

		return static_cast<IThing *>(pValuables);	
	}
	else
	{
		return NULL;
	}

	return NULL;
}
//------------------------------------------------------------------------
IThing * CThingFactory::CreateGoods_public(UID uid, LPCSTR buf, int nLen)
{
	if(nLen < sizeof(SGOODS_PROPERTY_BASE))
	{
		return NULL;		
	}

	SGOODS_PROPERTY_BASE * pPropertyBase = (SGOODS_PROPERTY_BASE *)(buf);
	// 取得是否为（装备，药品，凭证，元宝）
	IConfigDataCenter * pConfigDataCenter = g_pThingClientFacade->GetConfigDataCenter();
	if(pConfigDataCenter == NULL)
	{
		return NULL;
	}

	GOODS_BASEPROPERTY baseproperty = pConfigDataCenter->GetGoodsBaseProperty(pPropertyBase->dwGoodsID);
	if(baseproperty.lClass == GOODS_CLASS_EQUIPMENT)
	{
		CEquipment * pEquipment = new CEquipment();
		CREATE_THING_PUBLIC_TEMPLATE(pEquipment);

		return static_cast<IThing *>(pEquipment);
	}
	else if(baseproperty.lClass == GOODS_CLASS_MEDICAMENT)
	{
		CMedicament * pMedicament = new CMedicament();
		CREATE_THING_PUBLIC_TEMPLATE(pMedicament);

		return static_cast<IThing *>(pMedicament);
	}
	else if(baseproperty.lClass == GOODS_CLASS_WARRANT)
	{
		CWarrant * pWarrant = new CWarrant();
		CREATE_THING_PUBLIC_TEMPLATE(pWarrant);

		return static_cast<IThing *>(pWarrant);
	}
	else if(baseproperty.lClass == GOODS_CLASS_VALUABLES)
	{
		CValuables * pValuables = new CValuables();
		CREATE_THING_PUBLIC_TEMPLATE(pValuables);

		return static_cast<IThing *>(pValuables);
	}
	else
	{
		return NULL;
	}

	return NULL;
}
//------------------------------------------------------------------------
// 描  述：创建地图通道
IThing * CThingFactory::CreateChunnel_public(UID uid, LPCSTR buf, int nLen)
{
	CChunnel * pChunnel = new CChunnel();
	CREATE_THING_PUBLIC_TEMPLATE(pChunnel);

	return static_cast<IThing *>(pChunnel);
}
//------------------------------------------------------------------------
// 描  述：创建盒子
IThing * CThingFactory::CreateBox_public(UID uid, LPCSTR buf, int nLen)
{
	CBox* pBox = new CBox;
	CREATE_THING_PUBLIC_TEMPLATE(pBox);
	return pBox;
}
//------------------------------------------------------------------------
// 描  述：创建摊位
IThing * CThingFactory::CreateStall_public(UID uid, LPCSTR buf, int nLen)
{
	CStall * pStall = new CStall;
	CREATE_THING_PUBLIC_TEMPLATE(pStall);
	return pStall;
}
//------------------------------------------------------------------------