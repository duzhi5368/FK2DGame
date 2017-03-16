/**
*	created:		2012-7-3   1:44
*	filename: 		CHero
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		主角
*/
//------------------------------------------------------------------------
#include <windows.h>
#include <mmsystem.h>
#include "../Include/ThingClientCommon.h"
#include "../Include/CHero.h"
#include "../Include/CThingClientFacade.h"
#include "../Include/CreatureViewWrapper.h"
#include "../../FKGame/MapInfo/Tile.h"
//------------------------------------------------------------------------
CHero::CHero(void)
{
	ZeroMemory(m_ThingPart, sizeof(m_ThingPart));
	ZeroMemory(m_CreatureCommand, sizeof(m_CreatureCommand));
}
//------------------------------------------------------------------------
CHero::~CHero(void)
{
	
}
//------------------------------------------------------------------------
BOOL CHero::Create(void)
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
	int nHPCur = GetPropNum(CREATURE_PROPID_HPCUR);
	int nHPMax = GetPropNum(CREATURE_PROPID_HPMAXCUR);
	viewwrapper.SetBloodInfo(GetPropNum(CREATURE_PROPID_HPCUR), max(nHPCur, nHPMax));
	viewwrapper.SetFlagColorOnMinmap(0, 255, 0);

	viewwrapper.SetMoveStyle(ActMoveStyleRun);
	if(m_Property.nWalkStyle == CREATURE_MOVESTYLE_SIT)
	{			
		viewwrapper.Sit();
	}
	else if(m_Property.nWalkStyle == CREATURE_MOVESTYLE_WALK)
	{
		viewwrapper.SetMoveStyle(ActMoveStyleWalk);
	}

	viewwrapper.SetMoveSpeedLevel((int *)&m_Property.nMoveSpeedLevel);	
	m_CreatureActStateMachine.Create((ICreature *)this);
	
	g_pGlobal->m_pMapView->SetPlayerItem(m_pItemView);
	g_pGlobal->m_pPlayer = this;

	return TRUE;	
}
//------------------------------------------------------------------------
void CHero::Release(void)
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

	if(g_pGlobal->m_pPlayer == this)
	{
		g_pThingClientFacade->GetMapView()->SetPlayerItem(NULL);
		g_pGlobal->m_pPlayer = NULL;
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
// 私有对象现场管理
// 说明：
// 私有对象现场是指用户自己的角色或装备等的详细信息，其它人是看不到的
// 以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端
// 如：用户登录时，服务器取得人物对象现场
// 客户端根据dwClassID创建对象，调用OnGetPrivateContext()设置对象现场
BOOL CHero::OnSetPrivateContext(LPCSTR buf, int nLen)
{
	if(buf == NULL || nLen != sizeof(SACTOR_PROPERTY_PRIVATE))
	{
		TraceLn("CHero::OnSetPrivateContext Failed! [nLen != sizeof(SACTOR_PROPERTY_PRIVATE)]");
		return FALSE;
	}

	memcpy(&m_Property, buf, sizeof(SACTOR_PROPERTY_PRIVATE));
	m_Property.nMoveSpeedLevel = m_Property.nMoveSpeedLevel < 0 ? 0 : m_Property.nMoveSpeedLevel;
	m_Property.nMoveSpeedLevel = m_Property.nMoveSpeedLevel >= MAX_MOVESPEED_LEVEL ? MAX_MOVESPEED_LEVEL - 1 : m_Property.nMoveSpeedLevel;

	return TRUE;
}
//------------------------------------------------------------------------
int	CHero::SetPropNum(DWORD dwPropID, int nValue)
{  	
	switch(dwPropID)
	{
		// 性别 （1 ：男，2 ：女）
	case CREATURE_PROPID_SEX:
		{
			m_Property.nSex = nValue;
		}
		break;
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
		// 力量(当前)
	case CREATURE_PROPID_FORCECUR:
		{
			m_Property.nForceCur = nValue;
		}
		break;
		// 体质(当前)
	case CREATURE_PROPID_CONSTITCUR:
		{
			m_Property.nConstitCur = nValue;
		}
		break;	
		// 敏捷(当前)
	case CREATURE_PROPID_CELERITYCUR:
		{
			m_Property.nCelerityCur = nValue;
		}
		break;
		// 智力(当前)
	case CREATURE_PROPID_MENTALCUR:
		{
			m_Property.nMentalCur = nValue;
		}
		break;
		// 智慧(当前)
	case CREATURE_PROPID_WISDOMCUR:
		{
			m_Property.nWisdomCur = nValue;
		}
		break;
		// 最小物理攻击力(当前)
	case CREATURE_PROPID_P_A_MINCUR:
		{
			m_Property.nP_A_MinCur = nValue;
		}
		break;
		// 最大物理攻击力(当前)
	case CREATURE_PROPID_P_A_MAXCUR:
		{
			m_Property.nP_A_MaxCur = nValue;
		}
		break;
		// 最小法术攻击力(当前)
	case CREATURE_PROPID_M_A_MINCUR:
		{
			m_Property.nM_A_MinCur = nValue;
		}
		break;
		// 最大法术攻击力(当前)
	case CREATURE_PROPID_M_A_MAXCUR:
		{
			m_Property.nM_A_MaxCur = nValue;
		}
		break;
		// 最小火法术攻击力
	case CREATURE_PROPID_F_A_MIN:
		{
			m_Property.nF_A_Min = nValue;
		}
		break;
		// 最大火法术攻击力
	case CREATURE_PROPID_F_A_MAX:
		{
			m_Property.nF_A_Max = nValue;
		}
		break;
		// 最小冰法术攻击力
	case CREATURE_PROPID_I_A_MIN:
		{
			m_Property.nI_A_Min = nValue;
		}
		break;
		// 最大冰法术攻击力
	case CREATURE_PROPID_I_A_MAX:
		{
			m_Property.nI_A_Max = nValue;
		}
		break;
		// 最小电法术攻击力
	case CREATURE_PROPID_E_A_MIN:
		{
			m_Property.nE_A_Min = nValue;
		}
		break;
		// 最大电法术攻击力
	case CREATURE_PROPID_E_A_MAX:
		{
			m_Property.nE_A_Max = nValue;
		}
		break;
		// 最小暗法术攻击力
	case CREATURE_PROPID_N_A_MIN:
		{
			m_Property.nN_A_Min = nValue;
		}
		break;
		// 最大暗法术攻击力
	case CREATURE_PROPID_N_A_MAX:
		{
			m_Property.nN_A_Max = nValue;
		}
		break;
		// 最小物理防御力(当前)
	case CREATURE_PROPID_P_D_MINCUR:
		{
			m_Property.nP_D_MinCur = nValue;
		}
		break;
		// 最大物理防御力(当前)
	case CREATURE_PROPID_P_D_MAXCUR:
		{
			m_Property.nP_D_MaxCur = nValue;
		}
		break;
		// 最小法术防御力(当前)
	case CREATURE_PROPID_M_D_MINCUR:
		{
			m_Property.nM_D_MinCur = nValue;
		}
		break;
		// 最大法术防御力(当前)
	case CREATURE_PROPID_M_D_MAXCUR:
		{
			m_Property.nM_D_MaxCur = nValue;
		}
		break;
		// 最小火法术防御力
	case CREATURE_PROPID_F_D_MIN:
		{
			m_Property.nF_D_Min = nValue;
		}
		break;
		// 最大火法术防御力
	case CREATURE_PROPID_F_D_MAX:
		{
			m_Property.nF_D_Max = nValue;
		}
		break;
		// 最小冰法术防御力
	case CREATURE_PROPID_I_D_MIN:
		{	
			m_Property.nI_D_Min = nValue;
		}
		break;
		// 最大冰法术防御力
	case CREATURE_PROPID_I_D_MAX:
		{
			m_Property.nI_D_Max = nValue;
		}
		break;
		// 最小电法术防御力
	case CREATURE_PROPID_E_D_MIN:
		{
			m_Property.nE_D_Min = nValue;
		}
		break;
		// 最大电法术防御力
	case CREATURE_PROPID_E_D_MAX:
		{
			m_Property.nE_D_Max = nValue;
		}
		break;
		// 最小暗法术防御力
	case CREATURE_PROPID_N_D_MIN:
		{
			m_Property.nN_D_Min = nValue;
		}
		break;
		// 最大暗法术防御力
	case CREATURE_PROPID_N_D_MAX:
		{
			m_Property.nN_D_Max = nValue;
		}
		break;
		// 金钱
	case CREATURE_PROPID_HOLDMONEY_V1:
		{
			m_Property.nHoldMoney = nValue;
		}
		break;
		// 仓库金钱
	case CREATURE_PROPID_WAREMONEY_V1:
		{
			m_Property.nWareMoney = nValue;	
		}
		break;
		// 经验
	case CREATURE_PROPID_EXP_V1:
		{
			m_Property.nExp = nValue;
		}
		break;
		// 当前负重
	case CREATURE_PROPID_BURTHENCUR:
		{
			m_Property.nBurthenCur = nValue;
		}
		break;
		// 最大负重
	case CREATURE_PROPID_BURTHENMAX:
		{
			m_Property.nBurthenMax = nValue;
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
		// 装备栏容器ID
	case CREATURE_PROPID_PACKETCONTAINERID:
		{
			m_Property.nPacketContainerID = nValue;
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
			PrintEventLn("error : CHero::SetPropNum : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return 0;
}
//------------------------------------------------------------------------
int CHero::GetPropNum(DWORD dwPropID)
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
		// 力量(当前)
	case CREATURE_PROPID_FORCECUR:
		{
			return m_Property.nForceCur;
		}
		break;
		// 体质(当前)
	case CREATURE_PROPID_CONSTITCUR:
		{
			return m_Property.nConstitCur;
		}
		break;
		// 敏捷(当前)
	case CREATURE_PROPID_CELERITYCUR:
		{
			return m_Property.nCelerityCur;
		}
		break;
		// 智力(当前)
	case CREATURE_PROPID_MENTALCUR:
		{
			return m_Property.nMentalCur;
		}
		break;
		// 智慧(当前)
	case CREATURE_PROPID_WISDOMCUR:
		{
			return m_Property.nWisdomCur;
		}
		break;
		// 最小物理攻击力(当前)
	case CREATURE_PROPID_P_A_MINCUR:
		{
			return m_Property.nP_A_MinCur;
		}
		break;
		// 最大物理攻击力(当前)
	case CREATURE_PROPID_P_A_MAXCUR:
		{
			return m_Property.nP_A_MaxCur;
		}
		break;
		// 最小法术攻击力(当前)
	case CREATURE_PROPID_M_A_MINCUR:
		{
			return m_Property.nM_A_MinCur;
		}
		break;
		// 最大法术攻击力(当前)
	case CREATURE_PROPID_M_A_MAXCUR:
		{
			return m_Property.nM_A_MaxCur;
		}
		break;
		// 最小火法术攻击力
	case CREATURE_PROPID_F_A_MIN:
		{
			return m_Property.nF_A_Min;
		}
		break;
		// 最大火法术攻击力
	case CREATURE_PROPID_F_A_MAX:
		{
			return m_Property.nF_A_Max;
		}
		break;
		// 最小冰法术攻击力
	case CREATURE_PROPID_I_A_MIN:
		{
			return m_Property.nI_A_Min;
		}
		break;
		// 最大冰法术攻击力
	case CREATURE_PROPID_I_A_MAX:
		{
			return m_Property.nI_A_Max;
		}
		break;
		// 最小电法术攻击力
	case CREATURE_PROPID_E_A_MIN:
		{
			return m_Property.nE_A_Min;
		}
		break;
		// 最大电法术攻击力
	case CREATURE_PROPID_E_A_MAX:
		{
			return m_Property.nE_A_Max;
		}
		break;
		// 最小暗法术攻击力
	case CREATURE_PROPID_N_A_MIN:
		{
			return m_Property.nN_A_Min;
		}
		break;
		// 最大暗法术攻击力
	case CREATURE_PROPID_N_A_MAX:
		{
			return m_Property.nN_A_Max;
		}
		break;

		// 最小物理防御力(当前)
	case CREATURE_PROPID_P_D_MINCUR:
		{
			return m_Property.nP_D_MinCur;
		}
		break;
		// 最大物理防御力(当前)
	case CREATURE_PROPID_P_D_MAXCUR:
		{
			return m_Property.nP_D_MaxCur;
		}
		break;
		// 最小法术防御力(当前)
	case CREATURE_PROPID_M_D_MINCUR:
		{
			return m_Property.nM_D_MinCur;
		}
		break;
		// 最大法术防御力(当前)
	case CREATURE_PROPID_M_D_MAXCUR:
		{
			return m_Property.nM_D_MaxCur;
		}
		break;
		// 最小火法术防御力
	case CREATURE_PROPID_F_D_MIN:
		{
			return m_Property.nF_D_Min;
		}
		break;
		// 最大火法术防御力
	case CREATURE_PROPID_F_D_MAX:
		{
			return m_Property.nF_D_Max;
		}
		break;
		// 最小冰法术防御力
	case CREATURE_PROPID_I_D_MIN:
		{	
			return m_Property.nI_D_Min;
		}
		break;
		// 最大冰法术防御力
	case CREATURE_PROPID_I_D_MAX:
		{
			return m_Property.nI_D_Max;
		}
		break;
		// 最小电法术防御力
	case CREATURE_PROPID_E_D_MIN:
		{
			return m_Property.nE_D_Min;
		}
		break;
		// 最大电法术防御力
	case CREATURE_PROPID_E_D_MAX:
		{
			return m_Property.nE_D_Max;
		}
		break;
		// 最小暗法术防御力
	case CREATURE_PROPID_N_D_MIN:
		{
			return m_Property.nN_D_Min;
		}
		break;
		// 最大暗法术防御力
	case CREATURE_PROPID_N_D_MAX:
		{
			return m_Property.nN_D_Max;
		}
		break;
		// 金钱
	case CREATURE_PROPID_HOLDMONEY_V1:
		{
			return m_Property.nHoldMoney;
		}
		break;
		// 经验
	case CREATURE_PROPID_EXP_V1:
		{
			return m_Property.nExp;
		}
		break;
		// 当前负重
	case CREATURE_PROPID_BURTHENCUR:
		{
			return m_Property.nBurthenCur;
		}
		break;
		// 最大负重
	case CREATURE_PROPID_BURTHENMAX:
		{
			return m_Property.nBurthenMax;
		}
		break;
		// 仓库金钱
	case CREATURE_PROPID_WAREMONEY_V1:
		{
			return m_Property.nWareMoney;	
		}
		break;
		// 装备栏容器ID
	case CREATURE_PROPID_EQUIPCONTAINERID:
		{
			return m_Property.nEquipContainerID;
		}
		break;
		// 背包栏容器ID
	case CREATURE_PROPID_PACKETCONTAINERID:
		{
			return m_Property.nPacketContainerID;
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
			PrintEventLn("error : CHero::GetPropNum : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return 0;
}	
//------------------------------------------------------------------------
LPCSTR CHero::SetPropStr(DWORD dwPropID, LPCSTR strPropValue)
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
LPCSTR CHero::GetPropStr(DWORD dwPropID)
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
			PrintEventLn("error : CHero::GetStrProp : Invalid Propertiy ID[ %d ]!", dwPropID);
		}
		break;
	}

	return "";
}
//------------------------------------------------------------------------
void CHero::OnCreatureMoveToTileCenter(const POINTS &ptCurTile, const POINTS &ptNextTile)
{
	POINT ptsNextTile = { ptNextTile.x, 
						  ptNextTile.y};

	ICreatureActState * pCreatureActState = GetCurActState();
	if((pCreatureActState != NULL) &&
	   ((g_pGlobal->m_pMapView->IsValidTile(ptsNextTile) == FALSE) || 
	   ((g_pGlobal->m_pMapView->GetTile(ptsNextTile)->IsBlock() == TRUE))))
	{
		pCreatureActState->OnMoveItem(this, ptsNextTile);
	}
}
//------------------------------------------------------------------------
void CHero::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
{
	// 状态过滤
	ICreatureActState * pCreatureActState = m_CreatureActStateMachine.GetCurActState();
	if(pCreatureActState == NULL || !pCreatureActState->OnMessage(dwMsg, buf, nLen))
	{
		return;
	}

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
int CHero::OnCommand(DWORD dwCommand, LPCSTR buf, int nLen)
{
	if(dwCommand < 0 || dwCommand >= CREATURE_COMMANDID_MAXID || m_CreatureCommand[dwCommand] == NULL)
	{
		return 0;
	}

	return m_CreatureCommand[dwCommand]->OnCommand(this, buf, nLen);
}
//------------------------------------------------------------------------
BOOL CHero::GotoActState(DWORD dwActStateID, LPCSTR pContext, int nLen)
{
	return m_CreatureActStateMachine.GotoActState(dwActStateID, pContext, nLen);
}
//------------------------------------------------------------------------
ICreatureActState * CHero::GetCurActState(void)
{
	return m_CreatureActStateMachine.GetCurActState();
}
//------------------------------------------------------------------------
BOOL CHero::AddPart(IThingPart * pPart)
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
BOOL CHero::RemovePart(DWORD dwPartID)
{
	if(dwPartID < 0 || dwPartID >= THINGPARTID_THING_MAXID)
	{
		return FALSE;
	}

	m_ThingPart[dwPartID] = NULL;

	return TRUE;
}
//------------------------------------------------------------------------
IThingPart * CHero::GetPart(DWORD dwPartID)
{
	if(dwPartID < 0 || dwPartID >= THINGPARTID_THING_MAXID)
	{
		return NULL;
	}

	return m_ThingPart[dwPartID];
}
//------------------------------------------------------------------------
BOOL CHero::AddCreatureCommand(ICreatureCommand * pCreatureCommand)
{
	if(pCreatureCommand == NULL)
	{
		return FALSE;
	}

	if(pCreatureCommand->GetCommandID() < 0 || pCreatureCommand->GetCommandID() >= CREATURE_COMMANDID_MAXID)
	{
		return FALSE;
	}

	m_CreatureCommand[pCreatureCommand->GetCommandID()] = pCreatureCommand;	

	return TRUE;
}
//------------------------------------------------------------------------
void CHero::Accept(IMapItemVisitor &visitor)
{
	visitor.VisitHero();
}
//------------------------------------------------------------------------
void CHero::SetDying(BOOL bDying)
{

}
//------------------------------------------------------------------------
BOOL CHero::IsDying(void)
{
	return (m_CreatureActStateMachine.GetCurActStateID() == CREATURE_STATEID_DIE) ? TRUE : FALSE;
}
//------------------------------------------------------------------------