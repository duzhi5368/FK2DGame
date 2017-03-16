/**
*	created:		2012-6-28   8:59
*	filename: 		MiscFacade
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/MiscFacade.h"
#include "../../FKGame/ClientWorld/IThingTable.h"
#include "../../FKGame/SceneObj/IMonster.h"
#include "../../FKGame/SceneObj/IConjurePart.h"
#include "../../FKGame/Common/IConfigDataCenter.h"
#include "../Include/ChatClient.h"
#include "../../FKGame/ClientWorld/IClientWorld.h"
#include "../../FKGame/Skill/ISkillClient.h"
#include "../../FKGame/SceneObj/ThingDef.h"
#include "../../FKGame/SceneObj/IEquipment.h"
//------------------------------------------------------------------------
LPCSTR MISCFACADE::GetGoodsName(DWORD dwGoodsID)
{
	GOODS_BASEPROPERTY &prop = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(dwGoodsID);
	if (prop.lGoodsID != 0)
	{
		return prop.szName;
	}
	else
	{
		return "";
	}
}
//------------------------------------------------------------------------
int MISCFACADE::GetGoodsNumFromPackage(DWORD dwGoodsID)
{
	if (g_pGlobal->m_pPlayer == NULL)
	{
		return 0;
	}

	IContainerHolder *pPocket = (IContainerHolder *)g_pGlobal->m_pPlayer->GetPart(THINGPARTID_ACTOR_PACKET);
	if (pPocket == NULL)
	{
		return 0;
	}

	return pPocket->GetRemoveNumByGoodsID(dwGoodsID);
} 
//------------------------------------------------------------------------
IThing *MISCFACADE::GetThingFromUID(UID uidThing)
{
	if (g_pGlobal->m_pClientWorld == NULL)
	{
		return NULL;
	}

	IThingTable *pThingTable = g_pGlobal->m_pClientWorld->GetThingTable();
	if (pThingTable == NULL)
	{
		return NULL;
	}
	return pThingTable->Find(uidThing);
}
//------------------------------------------------------------------------
int MISCFACADE::GetPlayerSkillLevel(int nSkillID)
{
	if (g_pGlobal->m_pPlayer == NULL)
	{
		return 0;
	}

	ISkillControl *pSkillControl = (ISkillControl *)g_pGlobal->m_pPlayer->GetPart(THINGPARTID_CREATURE_SKILLCONTROL);
	if (pSkillControl == NULL)
	{
		return 0;
	}

	return pSkillControl->GetSkillLevel(nSkillID);
}
//------------------------------------------------------------------------
int MISCFACADE::GetEmptyBottle(int nBottleID)
{
	int nCountBottle = 0;
	if (g_pGlobal->m_pPlayer == NULL)
	{
		return nCountBottle;
	}

	IContainerHolder *pPocket = (IContainerHolder *)g_pGlobal->m_pPlayer->GetPart(THINGPARTID_ACTOR_PACKET);
	if (pPocket == NULL)
	{
		return nCountBottle;
	}

	IContainer *pContainer = pPocket->GetContainer();
	if (pContainer == NULL)
	{
		return nCountBottle;
	}

	for (int i = 0; i < pContainer->GetSize(); ++i)
	{
		IContainerGoods *pContainerGoods = pContainer->GetGoods(i);
		if (pContainerGoods)
		{
			GOODS_BASEPROPERTY &prop = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(pContainerGoods->GetGoodsID());	
			if (prop.lClass == GOODS_CLASS_MEDICAMENT
				&& prop.lSubClass == MEDICAMENT_SUBCLASS_MAGICBOTTLE
				&& prop.lGoodsID == nBottleID)
			{    //是瓶子
				IMedicament *pMedicament = (IMedicament *)MISCFACADE::GetThingFromUID(pContainerGoods->GetUID());
				if (pMedicament == NULL)
				{
					continue;
				}

				int nBufLen;
				BYTE *pBuff = pMedicament->GetExtendBuf(nBufLen);
				if (nBufLen < sizeof(SMEDICAMENT_EXTEND_FOR_MAGICBOTTLE))
				{
					continue;
				}
				SMEDICAMENT_EXTEND_FOR_MAGICBOTTLE BottleInfo;
				memcpy(&BottleInfo, pBuff, sizeof(SMEDICAMENT_EXTEND_FOR_MAGICBOTTLE));
				if (BottleInfo.tmStartTime == 0
					&& BottleInfo.tmCurrentTime == 0)
				{  //瓶子未开始炼化
					nCountBottle++;
				}
			}
		}
	}

	return nCountBottle;
}
//------------------------------------------------------------------------
int MISCFACADE::GetThingClass(UID uidThing)
{
	IThing *pThing = GetThingFromUID(uidThing);
	if (pThing == NULL)
	{
		return THINGCLASS_NONE;
	}

	IThingClass *pThingClass = pThing->GetThingClass();
	if (pThingClass == NULL)
	{
		return THINGCLASS_NONE;
	}

	return pThingClass->GetThingClass();
}
//------------------------------------------------------------------------
IWorkSkill * MISCFACADE::GetWorkSkill(int nSkillID)
{
	if (g_pGlobal->m_pPlayer == NULL)
	{
		return NULL;
	}

	IWorkSkillMgr *pWorkSkillMgr = (IWorkSkillMgr *)g_pGlobal->m_pPlayer->GetPart(THINGPARTID_ACTOR_WORKSKILL) ;
	if (pWorkSkillMgr == NULL)
	{
		return NULL;
	}

	return pWorkSkillMgr->GetSkill(nSkillID);
}
//------------------------------------------------------------------------
BOOL MISCFACADE::IsDead(UID uidCreature)
{
	IThing *pThing = GetThingFromUID(uidCreature);
	if (pThing == NULL)
	{
		return TRUE;
	}

	IThingClass *pThingClass = pThing->GetThingClass();
	if (pThingClass == NULL
		|| !pThingClass->IsCreature())
	{
		return TRUE;
	}

	ICreature *pCreature = (ICreature *)pThing;
	return pCreature->IsDying();
}
//------------------------------------------------------------------------
void *MISCFACADE::GetThingPart(ICreature *pCreature, int nPartIndex)
{
	if (pCreature == NULL)
	{
		return NULL;
	}

	return pCreature->GetPart(nPartIndex);
}
//------------------------------------------------------------------------
BOOL MISCFACADE::MasteredSkill(int nSkillID)
{	
	ISkillControl *pController = (ISkillControl *)GetThingPart(g_pGlobal->m_pPlayer, THINGPARTID_CREATURE_SKILLCONTROL);
	if (pController == NULL)
	{
		return FALSE;
	}

	return (pController->GetSkillLevel(nSkillID) != 0);
}
//------------------------------------------------------------------------
BOOL MISCFACADE::isSummonPet(UID uidCreature)
{
	IThing *pThing = GetThingFromUID(uidCreature);
	if (pThing == NULL)
	{
		return FALSE;
	}

	if (GetThingClass(uidCreature) != THINGCLASS_MONSTER)
	{
		return FALSE;
	}

	IMonster *pMonster = (IMonster *)pThing;	
	MONSTER_CONFIG_INFOR &MonsterInfo = g_pGlobal->m_pDataCenter->GetMonsterInfor(pMonster->GetMonsterID());

	if (MonsterInfo.ltype == MONSTER_TYPE_SAFEGUARD
		|| MonsterInfo.ltype == MONSTER_TYPE_HATCHETMAN)
	{
		return TRUE;
	}

	return FALSE;	
}
//------------------------------------------------------------------------
IContainer *MISCFACADE::GetActorPackage(void)
{
	if (g_pGlobal->m_pPlayer == NULL)
	{
		return NULL;
	}

	IContainerHolder *pPocket = (IContainerHolder *)g_pGlobal->m_pPlayer->GetPart(THINGPARTID_ACTOR_PACKET);
	if (pPocket == NULL)
	{
		return NULL;
	}

	return pPocket->GetContainer();
}
//------------------------------------------------------------------------
BOOL MISCFACADE::isBrokenEquipment(UID uidThing)
{
	if (GetThingClass(uidThing) != THINGCLASS_GOODS)
	{
		return FALSE;	
	}

	IGoods *pGoods = (IGoods *)GetThingFromUID(uidThing);
	if (pGoods == NULL)
	{
		return FALSE;
	}

	GOODS_BASEPROPERTY prop = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(pGoods->GetGoodsID());	
	if (prop.lGoodsID == 0
		|| prop.lClass != GOODS_CLASS_EQUIPMENT)
	{
		return FALSE;
	}

	int nCurrentDur = ((IEquipment *)pGoods)->GetPropNum(GOODS_PROPID_ENDURE_CUR);
	return nCurrentDur == 0;
}
//------------------------------------------------------------------------
int MISCFACADE::GetPetCount(IActor *pActor)
{
	if (pActor == NULL)
	{
		return 0;
	}

	IConjurePart *pPart = (IConjurePart *)GetThingPart(pActor, THINGPARTID_CREATURE_CONJURE);
	if (pPart == NULL)
	{
		return 0;
	}

	std::list<UID> lsPet = pPart->GetPawn();
	int nCount = 0;
	for (std::list<UID>::iterator itor = lsPet.begin(); itor != lsPet.end();
		++ itor)
	{
		if (GetThingFromUID(*itor))
		{
			nCount++;
		}
	}
	return nCount;	
}
//------------------------------------------------------------------------
//判断一个UID是否炼化瓶
BOOL MISCFACADE::isBottle(UID uidThing)
{
	if (GetThingClass(uidThing) != THINGCLASS_GOODS)
	{
		return FALSE;
	}

	IGoods *pGoods = static_cast<IGoods *>(GetThingFromUID(uidThing));	
	GOODS_BASEPROPERTY &prop = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(pGoods->GetGoodsID());	
	if (prop.lClass == GOODS_CLASS_MEDICAMENT
		&& prop.lSubClass == MEDICAMENT_SUBCLASS_MAGICBOTTLE)	
	{
		return TRUE;
	}

	return FALSE;
}
//------------------------------------------------------------------------
//判断一个瓶子是否正在炼化
BOOL MISCFACADE::isBottleRefining(IMedicament *pMedicament)
{
	if (pMedicament == NULL)
	{
		return FALSE;
	}

	int nBufLen;
	BYTE *pBuff = pMedicament->GetExtendBuf(nBufLen);
	if (nBufLen < sizeof(SMEDICAMENT_EXTEND_FOR_MAGICBOTTLE))
	{
		return FALSE;
	}

	SMEDICAMENT_EXTEND_FOR_MAGICBOTTLE *pBottleInfo = (SMEDICAMENT_EXTEND_FOR_MAGICBOTTLE *)(pBuff);	
	if (pBottleInfo->tmStartTime == 0
		&& pBottleInfo->tmCurrentTime == 0)
	{  
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------
//判断一个物品能否丢弃
BOOL MISCFACADE::GoodsCanDiscard(UID uidGoods)
{
	if (GetThingClass(uidGoods) != THINGCLASS_GOODS)
	{
		return FALSE;
	}

	IGoods *pGoods = static_cast<IGoods *>(GetThingFromUID(uidGoods));	
	if (pGoods)
	{
		return pGoods->GetBindFlag(GOODS_BINDFLAG_DOFF) 
			&& !pGoods->GetBindFlag(GOODS_BINDFLAG_SETUSE);
	}

	return FALSE;
}
//------------------------------------------------------------------------
int MISCFACADE::CorrectRange(int val, int low, int high)
{
	if (val < low)
	{
		return low;
	}

	if (val > high)
	{
		return high;
	}

	return val;
}
//------------------------------------------------------------------------
void MISCFACADE::StringInsert(char *szDst, char *szSrc, int nPos)
{
	int nLenSrc = strlen(szSrc);
	int nLenDst = strlen(szDst);
	memmove(szDst + nPos + nLenSrc, szDst + nPos, nLenDst - nPos + 1);
	memcpy(szDst + nPos, szSrc, nLenSrc);
}
//------------------------------------------------------------------------
BOOL MISCFACADE::isPlayTimeTreasure(UID uidThing)
{
	if (GetThingClass(uidThing) != THINGCLASS_GOODS)
	{
		return FALSE;
	}

	IGoods *pGoods = (IGoods *)GetThingFromUID(uidThing);
	if (pGoods == NULL)
	{
		return FALSE;
	}

	GOODS_BASEPROPERTY prop = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(pGoods->GetGoodsID());
	if (prop.lGoodsID != pGoods->GetGoodsID())
	{
		return FALSE;
	}

	if (prop.lClass != GOODS_CLASS_MEDICAMENT)
	{
		return FALSE;
	}

	if (prop.lSubClass != MEDICAMENT_SUBCLASS_VALUABLESGOLD
		&& prop.lSubClass != MEDICAMENT_SUBCLASS_VALUABLESSILVER
		&& prop.lSubClass != MEDICAMENT_SUBCLASS_VALUABLESCUPRUM )		
	{
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------
int MISCFACADE::GetSuitEquipedCount(int nSuitID)
{
	int nCount = 0;

	if (g_pGlobal->m_pPlayer == NULL
		|| g_pGlobal->m_pDataCenter == NULL)
	{
		return nCount;
	}

	//取人物装备栏
	IContainerHolder *pContainerHolder = (IContainerHolder*)(g_pGlobal->m_pPlayer->GetPart(THINGPARTID_ACTOR_EQUIP));
	if (pContainerHolder == NULL) 
	{
		return nCount;
	}
	IContainer *pContainer = pContainerHolder->GetContainer();;
	if (pContainer == NULL)
	{
		return nCount;
	}

	for(int i = 0; i < pContainer->GetSize(); i++)
	{
		IContainerGoods * pContainerGoods = pContainer->GetGoods(i);
		if(pContainerGoods == NULL)
		{
			continue;
		}

		IThing *pThing = g_pGlobal->m_pClientWorld->GetThingTable()->Find(pContainerGoods->GetUID());
		if (pThing == NULL || !pThing->GetThingClass()->IsEquipment())
		{
			continue;
		}

		IEquipment * pGoods = (IEquipment *)pThing;
		const EQUIPMENT_BASEPROPERTY &prop = g_pGlobal->m_pDataCenter->GetEquipmentBaseProperty(pGoods->GetGoodsID(), pGoods->GetSmeltLevel());
		if (prop.lGoodsID != pGoods->GetGoodsID())
		{
			continue;
		}

		if (prop.lSuitEquipID == nSuitID)
		{
			nCount++;
		}

	}

	return nCount;
}
//------------------------------------------------------------------------