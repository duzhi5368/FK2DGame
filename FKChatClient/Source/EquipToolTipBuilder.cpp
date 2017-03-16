/**
*	created:		2012-6-28   8:00
*	filename: 		EquipToolTipBuilder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/EquipToolTipBuilder.h"
#include "../Include/MiscFacade.h"
#include "../Include/ChatClient.h"
#include "../../FKGame/ClientWorld/IClientWorld.h"
#include "../../FKGame/ClientWorld/IThingTable.h"
#include "../../FKGame/Common/CreaturePropertyDef.h"
#include "../../FKGame/Effect/IEffectClient.h"
//------------------------------------------------------------------------
CEquipToolTipBuilder::CEquipToolTipBuilder()
{
	m_pEquipment = NULL;
	functions[0] = &CEquipToolTipBuilder::AddSoldPrice ;      			//售价
	functions[1] = &CEquipToolTipBuilder::AddName ;           			//名称
	functions[2] = &CEquipToolTipBuilder::AddBind ;           			//绑定
	functions[3] = &CEquipToolTipBuilder::AddUnify ;          			//唯一
	functions[4] = &CEquipToolTipBuilder::AddCanSold ;        			//能否出售
	functions[5] = &CEquipToolTipBuilder::AddAttackSpeed ;    			//攻击速度
	functions[6] = &CEquipToolTipBuilder::AddPhysicalAttack ; 			//物理攻击
	functions[7] = &CEquipToolTipBuilder::AddMagicalAttack ;			//魔法防御
	functions[8] = &CEquipToolTipBuilder::AddPhysicalDefence ;			//物理防御
	functions[9] = &CEquipToolTipBuilder::AddMagicalDefence ;			//魔法防御  
	functions[10] = &CEquipToolTipBuilder::AddWeight ;					//重量
	functions[11] = &CEquipToolTipBuilder::AddDur ;               		//耐久
	functions[12] = &CEquipToolTipBuilder::AddNeedLevel ;         		//需要等级
	functions[13] = &CEquipToolTipBuilder::AddNeedTrible ;				//种族
	functions[14] = &CEquipToolTipBuilder::AddExtraAttribute ;    		//额外属性
	functions[15] = &CEquipToolTipBuilder::AddInlay ;             		//镶嵌
	functions[16] = &CEquipToolTipBuilder::AddVocation ;          		//需要职业
	functions[17] = &CEquipToolTipBuilder::AddRemain ;            		//剩余次数
	functions[18] = &CEquipToolTipBuilder::AddDescription ;       		//描述
	functions[19] = &CEquipToolTipBuilder::AddEquipPosition;      		//装备位置
	functions[20] = &CEquipToolTipBuilder::AddRank;               		//档次
	functions[21] = &CEquipToolTipBuilder::AddSlotNum;            		//添加插孔数
	functions[22] = &CEquipToolTipBuilder::AddBlank;					//添加空行	
	functions[23] = &CEquipToolTipBuilder::AddRepairPrice;				//添加修理价格
	functions[24] = &CEquipToolTipBuilder::AddSuitEffect;				//套装相关
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddRepairPrice(void)
{
	int nCur = m_pEquipment->GetPropNum(GOODS_PROPID_ENDURE_CUR);
	int nCurMax = m_pEquipment->GetPropNum(GOODS_PROPID_ENDURE_CURMAX);
	int nPrice;
	if( nCur >= m_GoodsProp.lEndureMax || nCur <0 || m_GoodsProp.lBuyPrice <=0 || nCurMax<=0 )
	{
		nPrice = 0; 		
	}
	else
	{
		nPrice  = static_cast<int>( ((float)(nCurMax-nCur))/((float)m_GoodsProp.lEndureMax)*((float)m_GoodsProp.lBuyPrice) ); 
	}

	if (nPrice == 0)
	{
		return ;
	}

	int nGold = nPrice / 10000;
	int nSilver = (nPrice % 10000) / 100;
	int nCopper = nPrice % 100;

	char temp[64];
	sprintf(temp, "[:255,252,000,c]修理价格: ");	
	strToolTip = temp;

	if (nGold > 0)
	{
		sprintf(temp, "%d金", nGold);
		strToolTip = strToolTip + temp;		
	}

	if (nSilver > 0)
	{
		sprintf(temp, "%d银", nSilver);
		strToolTip = strToolTip + temp;		
	}

	if (nCopper > 0)
	{
		sprintf(temp, "%d铜", nCopper);
		strToolTip = strToolTip + temp ;		
	}	
	strToolTip = strToolTip + "\r\n";

}
//------------------------------------------------------------------------
CEquipToolTipBuilder::~CEquipToolTipBuilder()
{

}
//------------------------------------------------------------------------
BOOL CEquipToolTipBuilder::SetObject(DWORD dwGoodsID, UID uid)
{
	strToolTip.clear();	
	m_pEquipment = (IEquipment *)g_pGlobal->m_pClientWorld->GetThingTable()->Find(uid);
	if (m_pEquipment == NULL)
	{
		return FALSE;
	}
	m_GoodsProp = g_pGlobal->m_pDataCenter->GetEquipmentBaseProperty(dwGoodsID, m_pEquipment->GetPropNum( GOODS_PROPID_SMELTLEVEL));	

	return TRUE;
}
//------------------------------------------------------------------------
//用一个新对象重置TOOLTIP生成器
BOOL CEquipToolTipBuilder::SetObject(DWORD dwGoodsID, IGoods *pGoods)
{
	strToolTip.clear();	
	m_pEquipment = (IEquipment *)pGoods;
	m_GoodsProp = g_pGlobal->m_pDataCenter->GetEquipmentBaseProperty(dwGoodsID, m_pEquipment->GetPropNum( GOODS_PROPID_SMELTLEVEL));	
	return TRUE;
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddItem(int nItemID)
{
	if (m_pEquipment == NULL)
	{
		return;
	}

	if (nItemID >= 0 && nItemID < enUnknown)
	{		
		(this->*functions[nItemID])();
	}
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddSoldPrice(void)      //售价
{	
	int nGold = m_GoodsProp.lSellPrice / 10000;
	int nSilver = (m_GoodsProp.lSellPrice % 10000) / 100;
	int nCopper = m_GoodsProp.lSellPrice % 100;

	char temp[64];
	sprintf(temp, "[:255,252,000,c]卖出价格: ");	
	strToolTip = temp;

	if (nGold > 0)
	{
		sprintf(temp, "%d金", nGold);
		strToolTip = strToolTip + temp;		
	}

	if (nSilver > 0)
	{
		sprintf(temp, "%d银", nSilver);
		strToolTip = strToolTip + temp;		
	}

	if (nCopper > 0)
	{
		sprintf(temp, "%d铜", nCopper);
		strToolTip = strToolTip + temp ;		
	}	
	strToolTip = strToolTip + "\r\n";
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddName(void)            //名称
{	
	static const char *formatstr[6] = {
		"[wc]",
		"[:035,255,000,c]",
		"[:010,101,251,c]",
		"[:247,006,233,c]",
		"[:255,132,000,c]",
		"[:255,000,000,c]",
	};

	static const int MapTable[GOODS_EFFECT_MAXCOUNT + 1] = 
	{0, 1,1,1, 2,2, 3,3,4,5,5};		

	LPCSTR szFormat = formatstr[MapTable[__GetEffectCount()]];

	strToolTip = strToolTip + szFormat + m_GoodsProp.szName ;

	//精炼等级
	int nSmeltLevel = m_pEquipment->GetPropNum( GOODS_PROPID_SMELTLEVEL);
	if (nSmeltLevel > 0)
	{
		char temp[4];		
		sprintf(temp, "%d", nSmeltLevel);
		strToolTip = strToolTip + " +" + temp +"\r\n";
	}
	else
	{
		strToolTip = strToolTip + "\r\n";
	}
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddBind(void)            //绑定
{
	if (m_GoodsProp.lHoldBind)
	{
		if (m_pEquipment->GetBindFlag(GOODS_BINDFLAG_SETHOLD))
		{
			strToolTip = strToolTip + "[:255,153,000,c]已绑定" + "\r\n";			
		}
		else
		{
			strToolTip = strToolTip + "[:255,153,000,c]拾取后绑定" + "\r\n";
		}
		return;
	}

	if (m_GoodsProp.lUseBind)
	{
		if (m_pEquipment->GetBindFlag(GOODS_BINDFLAG_SETUSE))
		{
			strToolTip = strToolTip + "[:255,153,000,c]已绑定" + "\r\n";	
		}
		else
		{
			strToolTip = strToolTip + "[:255,153,000,c]使用后绑定" + "\r\n";
		}
		return;
	}	
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddUnify(void)           //唯一
{
	//唯一	
	if (m_GoodsProp.lHoldNum == 1)
	{
		strToolTip = strToolTip + "[wc]唯一" + "\r\n";
	}
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddCanSold(void)         //能否出售
{	
	//无法出售
	if (m_pEquipment->GetBindFlag(GOODS_BINDFLAG_SALE) == FALSE)
	{
		strToolTip = strToolTip + "[wc]无法出售" + "\r\n";
	}
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddAttackSpeed(void)     //攻击速度
{
	//攻击速度
	static const char *szSleeptable[] = {"极慢","很慢","较慢","慢","一般","快","较快","很快","极快"};						
	if (m_GoodsProp.lAttackSpeed >= 0 && m_GoodsProp.lAttackSpeed < 9) 
	{
		strToolTip = strToolTip + "[wc]" + "攻击速度: " + szSleeptable[m_GoodsProp.lAttackSpeed] + "\r\n";
	}
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddPhysicalAttack(void)  //物理攻击
{ 
	//物理攻击力
	char temp[100];
	int nMinAtt = m_GoodsProp.lP_AttackMin;
	int nMaxAtt = m_GoodsProp.lP_AttackMax;
	if (nMaxAtt > 0)
	{
		sprintf(temp, "[wc]物理攻击: %d - %d", nMinAtt, nMaxAtt);
		strToolTip = strToolTip + temp + "\r\n";
	}	
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddMagicalAttack(void)         //魔法防御
{
	char temp[100];
	int nMinAtt = m_GoodsProp.lM_AttackMin;
	int nMaxAtt = m_GoodsProp.lM_AttackMax;
	if (nMaxAtt > 0)
	{
		sprintf(temp, "[wc]法术攻击: %d - %d", nMinAtt, nMaxAtt);
		strToolTip = strToolTip + temp + "\r\n";
	}	
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddPhysicalDefence(void)     //物理防御
{
	char temp[100];
	int nMinDef = m_GoodsProp.lP_DefendMin;
	int nMaxDef = m_GoodsProp.lP_DefendMax;
	if (nMaxDef > 0)
	{
		sprintf(temp, "[wc]物理防御: %d - %d", nMinDef, nMaxDef);
		strToolTip = strToolTip + temp + "\r\n";
	}	
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddMagicalDefence(void)        //魔法防御  
{
	char temp[100];
	int nMinDef = m_GoodsProp.lM_DefendMin;
	int nMaxDef = m_GoodsProp.lM_DefendMax;
	if (nMaxDef > 0)
	{
		sprintf(temp, "[wc]法术防御: %d - %d", nMinDef, nMaxDef);
		strToolTip = strToolTip + temp + "\r\n";
	}	
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddWeight(void) 		     //重量
{
	char temp[32];
	sprintf(temp, "[wc]重量: %d\r\n", m_GoodsProp.lWeight);
	strToolTip = strToolTip + temp;
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddDur(void)                //耐久
{
	//耐久	
	char temp[48];
	int nMaxDup  = m_pEquipment->GetPropNum(GOODS_PROPID_ENDURE_CURMAX);
	int nNowDup  = m_pEquipment->GetPropNum(GOODS_PROPID_ENDURE_CUR);			
	if (nNowDup == 0)
	{
		sprintf(temp, "[rc]耐久: %d / %d", 0, nMaxDup / EQUIPMENT_ENDURE_SHOWSPACE);	
	}	
	else
	{
		int nPlus = 1;
		if (nMaxDup == nNowDup)
		{
			nPlus = 0;
		}
		sprintf(temp, "[wc]耐久: %d / %d", nNowDup / EQUIPMENT_ENDURE_SHOWSPACE + nPlus, nMaxDup / EQUIPMENT_ENDURE_SHOWSPACE);
	}

	strToolTip = strToolTip + temp + "\r\n";
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddNeedLevel(void)          //需要等级
{
	char temp[48];
	if (m_GoodsProp.lAllowLevel > 0)
	{
		int nCurrentLevel = 0;
		if (g_pGlobal->m_pPlayer)
		{
			nCurrentLevel = g_pGlobal->m_pPlayer->GetPropNum(CREATURE_PROPID_LEVEL);
		}

		if (nCurrentLevel < m_GoodsProp.lAllowLevel)
		{
			sprintf(temp, "[:255,000,000,c]需要等级: %d", m_GoodsProp.lAllowLevel + 1);		
		}
		else
		{
			sprintf(temp, "[wc]需要等级: %d", m_GoodsProp.lAllowLevel + 1);			
		}

		strToolTip = strToolTip + temp + "\r\n";
	}	
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddNeedTrible(void)        //种族
{

}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddExtraAttribute(void)     //额外属性
{
	//附加属性
	LPCSTR szStr = NULL;
	for (int i = 0; i < GOODS_EFFECT_MAXCOUNT; ++i)
	{//对每个属性查询一下
		IEffect *pEffect = m_pEquipment->GetEffect(i);
		if (pEffect && strlen(szStr = pEffect->GetDescription()) > 0)
		{//有特殊效果	
			switch(i) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				strToolTip = strToolTip + "[:252,248,144,c]" + szStr + "\r\n";
				break;
			case 6:
				strToolTip = strToolTip + "[:255,132,000,c]" + szStr + "\r\n";
				break;
			default:
				strToolTip = strToolTip + "[:088,208,243,c]" + szStr + "\r\n";
			}
		}	
	}
}
//------------------------------------------------------------------------
int  CEquipToolTipBuilder::__GetEffectCount(void)
{
	int nCount = 0;
	if (m_pEquipment)
	{
		for (int i = 0; i < GOODS_EFFECT_MAXCOUNT; ++i)
		{
			IEffect *pEffect = m_pEquipment->GetEffect(i);
			if (pEffect)
			{
				nCount ++;			
			}					
		}
	}	
	return nCount;
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddInlay(void)              //镶嵌
{

}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddVocation(void)           //需要职业
{
	// 使用职业
	char temp[64];
	char *szOccupationTable[] = {"战士", "法师", "幻师", "暗巫"};			
	if (m_GoodsProp.lAllowOccupation != 0 && m_GoodsProp.lAllowOccupation <= 4)
	{
		int nCurrentVocation = 0;
		if (g_pGlobal->m_pPlayer)
		{
			nCurrentVocation = g_pGlobal->m_pPlayer->GetPropNum(CREATURE_PROPID_OCCUPATION);
		}

		if (nCurrentVocation == m_GoodsProp.lAllowOccupation)
		{
			sprintf(temp, "[wc]需要职业: %s", szOccupationTable[m_GoodsProp.lAllowOccupation - 1]);
		}
		else
		{
			sprintf(temp, "[:255,000,000,c]需要职业: %s", szOccupationTable[m_GoodsProp.lAllowOccupation - 1]);	
		}

		strToolTip = strToolTip + temp + "\r\n";
	}
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddRemain(void)             //剩余次数
{

}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddDescription(void)        //描述
{
	AddBlank();
	//物品描述	
	strToolTip = strToolTip + "[:035,255,000,c]" + m_GoodsProp.szDesc;
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddEquipPosition(void)
{	
	//装备位置
	const static char *szPartName[] = {
		"头部"
		, "肩部"
		, "胸部"
		, "腿部"			
		, "饰品" 
		, "保留" 
		, "手指" 
		, "武器" 
		, "盾牌" 
		, "手指"
		, "保留" 
		, "脚部" 
		, "手腕" 
		, "腰部" 
		, "背部" 
		, "颈部" 
	};

	if (m_GoodsProp.lEquipPos1 >= 0 && m_GoodsProp.lEquipPos1 <= 15)
	{
		strToolTip = strToolTip + "[wc]" + szPartName[m_GoodsProp.lEquipPos1]+ "\r\n";
	}
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddRank(void)
{
	char temp[32];
	sprintf(temp, "档次: %d\r\n", m_GoodsProp.lRank);
	strToolTip = strToolTip + "[wc]" + temp;
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddSlotNum(void)
{
	if (m_pEquipment)
	{
		int nSlotNum = m_pEquipment->GetSlotLeft();
		if (nSlotNum)
		{
			char temp[32];
			sprintf(temp, "剩余 %d 个插槽\r\n", nSlotNum);
			strToolTip = strToolTip + "[:088,208,243,c]" + temp;	
		}		
	}		
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddBlank(void)
{
	strToolTip = strToolTip + "\r\n";
}
//------------------------------------------------------------------------
void CEquipToolTipBuilder::AddSuitEffect(void)
{
	if (g_pGlobal->m_pDataCenter == NULL)
	{
		return;
	}

	SUITEQUIP_EFFECT *pSuitEffect = g_pGlobal->m_pDataCenter->GetSuitEquipEffect(m_GoodsProp.lSuitEquipID);
	if (pSuitEffect == NULL
		|| pSuitEffect->lMaxPiece < 2)  
	{
		return;
	}

	//查找穿上了多少件装备
	int nEquipCount =  MISCFACADE::GetSuitEquipedCount( m_GoodsProp.lSuitEquipID );

	char temp[255];
	sprintf(temp, "[:035,255,000,c]%s (%d/%d)\r\n", pSuitEffect->szName, nEquipCount, pSuitEffect->lMaxPiece);
	strToolTip = strToolTip + temp;

	IEffectFactory *pEffectFactory = g_pGlobal->GetEffectFactory();
	if (nEquipCount > 0
		&& nEquipCount < EQUIPPOS_INDEX_MAX
		&& pEffectFactory)
	{
		for (int i = 0; i < 12; ++i)
		{
			int nEffectID = pSuitEffect->lEffect[nEquipCount][i];
			if (nEffectID != 0)
			{
				strToolTip = strToolTip + "套装:" + g_pGlobal->GetEffectFactory()->GetDescription(nEffectID)
					+ "\r\n";
			}				
		}			
	}
}
//------------------------------------------------------------------------