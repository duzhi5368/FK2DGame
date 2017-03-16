/**
*	created:		2012-6-30   5:13
*	filename: 		Tool
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/Tool.h"
#include "../Include/ToolTipBuilder.h"
#include "../../FKGameModule/Include/AccountSpace.h"
#include "../Include/SysMessageDef.h"
#include "../../FKGame/Sound/ISound.h"
#include "../Include/ChatClient.h"
#include "../../FKGame/ClientWorld/IClientWorld.h"
#include "../../FKGame/ClientWorld/IThingTable.h"
#include "../../FKGame/SceneObj/ThingDef.h"
#include "../../FKGame/SceneObj/IItemClassFactory.h"
#include "../../FKGame/Common/MapItemDef.h"
#include "../../FKGame/MapView/MapItemView.h"
#include <time.h>
//------------------------------------------------------------------------
CTool* CTool::s_pInstance = NULL;
//------------------------------------------------------------------------
CTool::CTool()
{

}
//------------------------------------------------------------------------
CTool::~CTool()
{

}
//------------------------------------------------------------------------
LPCSTR CTool::GetNameByUID(UID uid)
{
	if( g_pGlobal->m_pClientWorld && g_pGlobal->m_pClientWorld->GetThingTable() )
	{
		IThing *pThing = g_pGlobal->m_pClientWorld->GetThingTable()->Find(uid);
		if( pThing )
		{
			DWORD dwType = pThing->GetThingClass()->GetThingClass();
			if( dwType == THINGCLASS_GOODS )
			{
				IGoods *pGoods = static_cast<IGoods *>(pThing);
				return pGoods->GetName();
			}else if( dwType == THINGCLASS_MONSTER ||
				dwType == THINGCLASS_ACTOR	)
			{
				ICreature *pCure = static_cast<ICreature *>(pThing);
				return pCure->GetName();				
			}
		}
	}
	return NULL;
}
//------------------------------------------------------------------------
ICreature* CTool::GetCreatureThingByUID(UID uid)
{

	if( g_pGlobal->m_pClientWorld && g_pGlobal->m_pClientWorld->GetThingTable() )
	{
		IThing *pThing = g_pGlobal->m_pClientWorld->GetThingTable()->Find(uid);
		if( pThing )
		{
			DWORD dwType = pThing->GetThingClass()->GetThingClass();
			if( dwType == THINGCLASS_MONSTER || dwType == THINGCLASS_ACTOR)
			{
				ICreature *pCure = static_cast<ICreature *>(pThing);
				return pCure;				
			}
		}
	}
	return NULL;
}
//------------------------------------------------------------------------
IThing* CTool::GetThing(UID uid)
{
	IThing *pThing = NULL;
	if( g_pGlobal->m_pClientWorld && g_pGlobal->m_pClientWorld->GetThingTable() )
	{
		pThing = g_pGlobal->m_pClientWorld->GetThingTable()->Find(uid);
	}
	return pThing;
}
//------------------------------------------------------------------------
CTool* CTool::Instance()
{
	if( s_pInstance == NULL )
	{
		s_pInstance = new CTool;
	}
	return s_pInstance;
}
//------------------------------------------------------------------------
void CTool::Speak(char *lpszFormat,...)
{
	char szMsg[256];
	va_list va;
	va_start(va,lpszFormat);
	vsprintf(szMsg,lpszFormat,va);
	va_end(va);
	g_pGlobal->GetChatClient()->DisplayFloatMessage(SMT_FLOAT,szMsg);
}
//------------------------------------------------------------------------
void CTool::IntMoneyToStr(int nMoney,char *lpsz)
{
	int m = nMoney/10000;
	int n = (nMoney - m*10000)/100;
	int z = nMoney-m*10000-n*100;

	int nMask = 0;
	nMask |= (z!=0);
	nMask |= ((n!=0)<<1);
	nMask |= ((m!=0)<<2);


	static char *szFormat[]  = 
	{
		"%d金%d银%d铜",                   // 000
		"%d铜",             // 001
		"%d银",              // 010
		"%d银%d铜",       // 011
		"%d金",             // 100
		"%d金%d铜",       // 101
		"%d金%d银",       // 110
		"%d金%d银%d铜"  // 111
	};

	switch(nMask)
	{
	case 0:
		{
			sprintf(lpsz,szFormat[nMask],m,n,z);
			break;
		}
	case 1:
		{
			sprintf(lpsz,szFormat[nMask],z);
			break;
		}
	case 2:
		{
			sprintf(lpsz,szFormat[nMask],n);
			break;
		}
	case 3:
		{
			sprintf(lpsz,szFormat[nMask],n,z);
			break;
		}
	case 4:
		{

			sprintf(lpsz,szFormat[nMask],m);
			break;
		}
	case 5:
		{
			sprintf(lpsz,szFormat[nMask],m,z);			
			break;
		}
	case 6:
		{
			sprintf(lpsz,szFormat[nMask],m,n);	
			break;
		}
	case 7:
		{
			sprintf(lpsz,szFormat[nMask],m,n,z);	
			break;
		}
	}
}
//------------------------------------------------------------------------
string CTool::GetGoodNameByGoodID(DWORD dwID)
{
	GOODS_BASEPROPERTY& prop = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(dwID);
	return prop.szName;	 
}
//------------------------------------------------------------------------
BOOL CTool::RemoveAddGoods( IContainerGoods *pGoods,IContainer *pContanier,int &nLocation)
{   
	if( pGoods == NULL)
	{
		return FALSE;
	}
	IContainer *pContanier1 = pGoods->GetContainer();
	if( pContanier1 == NULL || pContanier == NULL)
	{
		return FALSE;	
	}
	if(nLocation == -1)
	{
		int i = 0;
		for( i = 0;i<pContanier->GetSize();i++ )
		{
			if( pContanier->GetGoods(i) == NULL)
			{
				nLocation = i;
				break;
			}
		}
	}
	if(pContanier1->CanRemove(pGoods,pGoods->GetLocation())
		&& pContanier->CanAdd(pGoods,nLocation))
	{
		pContanier->AddGoods(pGoods,nLocation,pGoods->GetNumber());
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CTool::ReleaseContainer(IContainer *pContainer,BOOL bReleaseGoods)
{
	if(pContainer)
	{
		pContainer->Release();
	}
	return TRUE;
}
//------------------------------------------------------------------------
int CTool::CouputeDistance(UID uid)
{
	static CAccountSpace<> space;
	IThing *pThing = GetThing(uid);
	if(pThing == NULL)
	{
		return 10000;
	}
	POINTS ps1 = pThing->GetLoc();
	if( g_pGlobal && g_pGlobal->m_pPlayer )
	{
		POINTS ps2 =  g_pGlobal->m_pPlayer->GetLoc();
		return space.Distance(ps1,ps2);
	}
	return 10000;
}
//------------------------------------------------------------------------
BOOL CTool::PlaySound(int nSoundId, BOOL bLoop)
{
	ISound *pSound = g_pGlobal->GetSoundEngine();
	if(pSound!=NULL)
	{
		pSound->PlaySound(nSoundId,bLoop);
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
void CTool::Random(BYTE by[],int nLen,int nMin,int nMax)
{
	::srand((int)time(NULL));
	for( int i = 0; i< nLen;i++)
	{
		by[i] = rand()%(nMax-nMin)+nMin;
	}
}
//------------------------------------------------------------------------
string& CTool::BuilderToolTip(UID uid)
{
	IGoods *pGoods = (IGoods *)GetThing(uid);
	m_strToolTip = "";
	if( pGoods == NULL)
	{
		return m_strToolTip;
	}
	GOODS_BASEPROPERTY& prop = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(pGoods->GetGoodsID());	
	if (CToolTipBuilder::Instance()->SetObject(pGoods->GetGoodsID(), uid) == FALSE)
	{
		return m_strToolTip;
	}
	//分类别显示各种特殊属性
	switch(prop.lClass) {
	case GOODS_CLASS_EQUIPMENT:    //物品为装备
		{
			//名字 + 精炼等级
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enName);
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enBlank);
			//装备部位
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enEquipPosition);
			//档次
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enRank);
			//使用绑定
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enBind);
			//唯一	
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enUnify);

			//无法出售
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enCanSold);

			//攻击速度(策划说不要了)
			//CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enAttackSpeed);

			//物理攻击力
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enPhysicalAttack);

			//物理防御
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enPhysicalDefence);

			//法术攻击力
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enMagicalAttack);

			//法术防御
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enMagicalDefence);

			//重量(策划说不要了)
			//CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enWeight);

			//耐久		
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enDur);

			//所要等级
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enNeedLevel);

			//使用职业
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enVocation);

			//附加属性
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enExtraAttribute);

			//插孔数
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enSlotNum);

			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enSuitEffectDesc);			
			//描述
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enDescription);

			m_strToolTip = CToolTipBuilder::Instance()->GetToolTip();
		}
		break;
	case GOODS_CLASS_WARRANT:
	case GOODS_CLASS_MEDICAMENT:    //物品为药品 
		{

			//名字
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enName);
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enBlank);

			//物品子类型
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enGoodsType);

			//档次
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enRank);

			//使用等级
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enNeedLevel);

			//物品描述	
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enDescription);		

			m_strToolTip = m_strToolTip+CToolTipBuilder::Instance()->GetToolTip();

		}
		break;

	default:
		{
			//名字
			//名字
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enName);

			//物品描述	
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enDescription);	
			m_strToolTip += CToolTipBuilder::Instance()->GetToolTip();
		}		
		break;
	} //switch
	return m_strToolTip;
}
//------------------------------------------------------------------------
string& CTool::BuilderToolTip(IGoods *pGoods)
{
	m_strToolTip = "";
	if( pGoods == NULL)
	{
		return m_strToolTip;
	}
	GOODS_BASEPROPERTY& prop = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(pGoods->GetGoodsID());	
	if (CToolTipBuilder::Instance()->SetObject(pGoods->GetGoodsID(), pGoods) == FALSE)
	{
		return m_strToolTip;
	}
	//分类别显示各种特殊属性
	switch(prop.lClass) {
	case GOODS_CLASS_EQUIPMENT:    //物品为装备
		{
			//名字 + 精炼等级
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enName);
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enBlank);
			//装备部位
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enEquipPosition);
			//档次
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enRank);
			//使用绑定
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enBind);
			//唯一	
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enUnify);

			//无法出售
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enCanSold);

			//攻击速度(策划说不要了)
			//CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enAttackSpeed);

			//物理攻击力
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enPhysicalAttack);

			//物理防御
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enPhysicalDefence);

			//法术攻击力
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enMagicalAttack);

			//法术防御
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enMagicalDefence);

			//重量(策划说不要了)
			//CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enWeight);

			//耐久		
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enDur);

			//所要等级
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enNeedLevel);

			//使用职业
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enVocation);

			//附加属性
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enExtraAttribute);

			//插孔数
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enSlotNum);

			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enSuitEffectDesc);
			//描述
			CToolTipBuilder::Instance()->AddItem(CEquipToolTipBuilder::enDescription);

			m_strToolTip = CToolTipBuilder::Instance()->GetToolTip();
		}
		break;
	case GOODS_CLASS_WARRANT:
	case GOODS_CLASS_MEDICAMENT:    //物品为药品 
		{

			//名字
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enName);
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enBlank);

			//物品子类型
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enGoodsType);

			//档次
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enRank);

			//使用等级
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enNeedLevel);

			//物品描述	
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enDescription);		

			m_strToolTip = m_strToolTip+CToolTipBuilder::Instance()->GetToolTip();

		}
		break;

	default:
		{
			//名字
			//名字
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enName);

			//物品描述	
			CToolTipBuilder::Instance()->AddItem(CGoodsToolTipBuilder::enDescription);	
			m_strToolTip += CToolTipBuilder::Instance()->GetToolTip();
		}		
		break;
	} //switch
	return m_strToolTip;
}
//------------------------------------------------------------------------
IBitmap* CTool::GetGoodsBitmap(DWORD dwGoodsId)
{

	GOODS_BASEPROPERTY& Prop = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(dwGoodsId);
	IBitmap *pBitmap = NULL;
	CMapItemView * pItem = (CMapItemView *)g_pGlobal->m_pItemClassFactory->CreateItem(
		miStaticObj_Picture, 
		(char*)&(Prop.lArticleID_z), 
		sizeof(DWORD));
	if(pItem )
	{
		pBitmap = pItem->GetBitmap();
	}
	return pBitmap;
}
//------------------------------------------------------------------------
long CTool::GetGoodsLevelByGoodsId(DWORD dwGoodsId)
{
	GOODS_BASEPROPERTY& Prop = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(dwGoodsId);
	return Prop.lAllowLevel;
}
//------------------------------------------------------------------------
long CTool::GetGoodsLevelByUID(UID UId)
{
	IGoods *pGoods = (IGoods *)GetThing(UId);
	if( pGoods != NULL)
	{
		return GetGoodsLevelByGoodsId(pGoods->GetGoodsID());
	}
	return 0;
}
//------------------------------------------------------------------------
int CTool::GetEquipmentEndUse(UID uid)
{
	int n = -1;
	IGoods *pGoods = (IGoods *)GetThing(uid); 
	if( pGoods )
	{
		GOODS_BASEPROPERTY& Prop = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(pGoods->GetGoodsID());
		if(Prop.lClass == GOODS_CLASS_EQUIPMENT)
		{
			return pGoods->GetPropNum(GOODS_PROPID_ENDURE_CUR);
		}
	}
	return n;
}
//------------------------------------------------------------------------
WPixel CTool::GetGoodsNameColor(IGoods *pGoods)
{
	WPixel w = 0;
	static const  WPixel wGoodsColor[] = 
	{
		WPixel(255,255,255),
		WPixel(35,255,0),
		WPixel(10,101,251),
		WPixel(247,006,233),    
		WPixel(255,132,000),   
		WPixel(255,000,000)	   
	};
	static const int MapTable[GOODS_EFFECT_MAXCOUNT + 1] = 
	{0, 1,1,1, 2,2, 3,3,4,5,5};
	if( pGoods == NULL )
	{
		return w;
	}
	int nIndex = MapTable[GetEffectCount(pGoods)];
	w = wGoodsColor[nIndex];
	return w;
}
//------------------------------------------------------------------------
int CTool::GetEffectCount(IGoods *pGoods)
{
	int nCount = 0;
	if( pGoods== NULL)
	{
		return 0;
	}
	GOODS_BASEPROPERTY& Prop = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(pGoods->GetGoodsID());
	IEquipment *pEquipment = (IEquipment *)pGoods;
	if(Prop.lClass == GOODS_CLASS_EQUIPMENT)
	{
		if (pEquipment)
		{
			for (int i = 0; i < GOODS_EFFECT_MAXCOUNT; ++i)
			{
				IEffect *pEffect = pEquipment->GetEffect(i);
				if (pEffect)
				{
					nCount ++;			
				}					
			}
		}	
	}	
	return nCount;
}
//------------------------------------------------------------------------