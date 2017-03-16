/**
*	created:		2012-6-30   23:19
*	filename: 		CConfigDataCenter
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "PreInclude.h"
#include "CConfigDataCenter.h"
#include "../GameGlobal/ClientGlobal.h"
//------------------------------------------------------------------------
CConfigDataCenter::CConfigDataCenter(void)
{
	
}
//------------------------------------------------------------------------
CConfigDataCenter::~CConfigDataCenter(void)
{

}
//------------------------------------------------------------------------
// 载入相关脚本
BOOL CConfigDataCenter::LoadScript(void)
{
	if(!m_ConfigMapInfor.Create())
	{
		return FALSE;
	}

	/*
	if(!m_ConfigProp.Create())
	{
		return FALSE;
	}

	if(!m_ConfigCreatureNude.Create())
	{
		return FALSE;
	}

	if(!m_ConfigMonster.Create())
	{
		return FALSE;
	}

	if(!m_ConfigGoodsProperty.Create())
	{
		return FALSE;
	}

	if(!m_ConfigActorExp.Create())
	{
		return FALSE;
	}

	if(!m_ConfigActorHeadRes.Create())
	{
		return FALSE;
	}

	if(!m_ConfigGoodsSound.Create())
	{
		return FALSE;
	}

	if(!m_ConfigSecret.Create())
	{
		return FALSE;
	}

	if(!m_ConfigExpDegression.Create())
	{
		return FALSE;
	}

	if(!m_ConfigHorse.Create())
	{
		return FALSE;
	}

	if(!m_ConfigSuitEquip.Create())
	{
		return FALSE;
	}

	if(!m_ConfigCastellanGown.Create())
	{
		return FALSE;
	}
	*/
	return TRUE;
}
//------------------------------------------------------------------------
// 生物属性的配置
IConfigProp * CConfigDataCenter::GetConfigProp(void)
{
	return NULL;
	// return static_cast<IConfigProp *>(&m_ConfigProp);
}
//------------------------------------------------------------------------
// 取得所有地图的信息, IN : pInfor 可容纳的MAP_INFOR个数, OUT : 当前有多少张地图
BOOL CConfigDataCenter::GetMapInfor(MAP_INFOR * pInfor, DWORD &dwCount)
{
	return FALSE;
	// return m_ConfigMapInfor.GetMapInfor(pInfor, dwCount);
}
//------------------------------------------------------------------------
// 取得某一张地图的信息
MAP_INFOR *	CConfigDataCenter::GetMapInfor(DWORD dwMapID)
{
	return m_ConfigMapInfor.GetMapInfor(dwMapID);
}
//------------------------------------------------------------------------
// 取得当前地图数量
DWORD CConfigDataCenter::GetMapCount(void)
{
	return 0;
	// return m_ConfigMapInfor.GetMapCount();
}
//------------------------------------------------------------------------
// 取得人物某一职业，某一个部件的光身部件资源ID
// dwOccupation : 职业
// dwPartID = 部件号
DWORD CConfigDataCenter::GetNudePartForActor(DWORD dwOccupation, DWORD dwPartID, DWORD dwSex)
{
	return 0;
	//return m_ConfigCreatureNude.GetNudePartForActor(dwOccupation, dwPartID, dwSex);
}
//------------------------------------------------------------------------
// 描  述：取得人物某一职业，某一个部件的光身部件资源ID show
// 参  数：dwOccupation : 职业
// 返回值：dwPartID = 部件号
DWORD CConfigDataCenter::GetNudePartForActor_Show(DWORD dwOccupation, DWORD dwPartID, DWORD dwSex)
{
	return 0;
	// return m_ConfigCreatureNude.GetNudePartForActor_Show(dwOccupation, dwPartID, dwSex);
}
//------------------------------------------------------------------------
// 描  述：取得某个怪物的设定
MONSTER_CONFIG_INFOR & CConfigDataCenter::GetMonsterInfor(long lMonsterID)
{
	MONSTER_CONFIG_INFOR tmp;
	return tmp;
	// return m_ConfigMonster.GetMonsterInfor(lMonsterID);
}
//------------------------------------------------------------------------
// 描  述：取得物品基本属性
GOODS_BASEPROPERTY & CConfigDataCenter::GetGoodsBaseProperty(long lGoodsID)
{
	GOODS_BASEPROPERTY tmp;
	return tmp;
	// return m_ConfigGoodsProperty.GetGoodsBaseProperty(lGoodsID);
}
//------------------------------------------------------------------------
// 描  述：取得药品基本属性
MEDICAMENT_BASEPROPERTY & CConfigDataCenter::GetMedicamentBaseProperty(long lGoodsID)
{
	MEDICAMENT_BASEPROPERTY tmp;
	return tmp;
	// return m_ConfigGoodsProperty.GetMedicamentBaseProperty(lGoodsID);
}
//------------------------------------------------------------------------
// 描  述：取得装备基本属性
EQUIPMENT_BASEPROPERTY & CConfigDataCenter::GetEquipmentBaseProperty(long lGoodsID, long lSmeltLevel)
{
	EQUIPMENT_BASEPROPERTY tmp;
	return tmp;
	// return m_ConfigGoodsProperty.GetEquipmentBaseProperty(lGoodsID, lSmeltLevel);
}
//------------------------------------------------------------------------
// 描  述：取得凭证基本属性
WARRANT_BASEPROPERTY & CConfigDataCenter::GetWarrantBaseProperty(long lGoodsID)
{
	WARRANT_BASEPROPERTY tmp;
	return tmp;
	// return m_ConfigGoodsProperty.GetWarrantBaseProperty(lGoodsID);
}
//------------------------------------------------------------------------
// 描  述：取得元宝基本属性
VALUABLES_BASEPROPERTY & CConfigDataCenter::GetValuablesBaseProperty(long lGoodsID)
{
	VALUABLES_BASEPROPERTY tmp;
	return tmp;
	// return m_ConfigGoodsProperty.GetValuablesBaseProperty(lGoodsID);
}
//------------------------------------------------------------------------
// 描  述：取得人物某一等级的经验
long CConfigDataCenter::GetActorExp(long lLevel)
{
	return 0;
	// return m_ConfigActorExp.GetActorExp(lLevel);
}
//------------------------------------------------------------------------
// 描  述：取得人物某一职业，空手的攻击和击中声音
void CConfigDataCenter::GetFreeWeaponSoundID(DWORD dwOccupation, DWORD &dwAttackSoundID, DWORD &dwBeHitSoundID)
{
	return;
	// m_ConfigCreatureNude.GetFreeWeaponSoundID(dwOccupation, dwAttackSoundID, dwBeHitSoundID);
}
//------------------------------------------------------------------------
// 描  述：通过头像索引号取得各种类型头像ID
DWORD CConfigDataCenter::GetHeadResIDByIndex(DWORD dwIndex, DWORD dwType)
{
	return 0;
	// return m_ConfigActorHeadRes.GetHeadResIDByIndex(dwIndex, dwType);
}
//------------------------------------------------------------------------
// 描  述：通过职业和性别获取可供使用的头像索引号
vector<DWORD> CConfigDataCenter::GetHeadResIDByOccupation(DWORD dwOccupation, DWORD dwSex)
{
	vector< DWORD > tmp;
	return tmp;
	// return m_ConfigActorHeadRes.GetHeadResIDByOccupation(dwOccupation, dwSex);
}
//------------------------------------------------------------------------
// 描  述：通过物品ID取得相关的声音
DWORD CConfigDataCenter::GetGoodsSoundID(DWORD dwGoodsID, DWORD dwSoundType)
{
	return 0;
	// return m_ConfigGoodsSound.GetGoodsSoundID(dwGoodsID, dwSoundType);
}
//------------------------------------------------------------------------
// 描  述：取得配方信息
SECRET_INFOR * CConfigDataCenter::GetSecretInfor(long lSecretID)
{
	return NULL;
	//return m_ConfigSecret.GetSecretInfor(lSecretID);
}
//------------------------------------------------------------------------
// 描  述：取得经验衰减, dwMasterLevel = 人物等级, nLevelDiff = 等级差
float CConfigDataCenter::GetExpDegression(DWORD dwMasterLevel, int nLevelDiff)
{
	return 0.0f;
	//return m_ConfigExpDegression.GetExpDegression(dwMasterLevel, nLevelDiff);
}
//------------------------------------------------------------------------
// 描  述：取得座驾的配置参数
HORSE_CONFIG_INFOR * CConfigDataCenter::GetHorseInfor(long lID)
{
	return NULL;
	//return m_ConfigHorse.GetHorseInfor(lID);
}
//------------------------------------------------------------------------
// 描  述：取得套装附加效果
SUITEQUIP_EFFECT * CConfigDataCenter::GetSuitEquipEffect(long lSuitEquipID)
{
	return NULL;
	//return m_ConfigSuitEquip.GetSuitEquipEffect(lSuitEquipID);
}
//------------------------------------------------------------------------
// 描  述：获取城主氏族炫耀装备
SCASTELLANGOWN * CConfigDataCenter::GetCastellanGown(long lCastellanID, long lOccupation)
{
	return NULL;
	//return m_ConfigCastellanGown.GetCastellanGown(lCastellanID, lOccupation);
}
//------------------------------------------------------------------------