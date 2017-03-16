/**
*	created:		2012-6-30   23:17
*	filename: 		CConfigDataCenter
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		通过读取脚本获得各种服务器数值配置, 然后通过全局的实例获取
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IConfigDataCenter.h"
//------------------------------------------------------------------------
#include "CConfigMapInfor.h"
//#include "CConfigProp.h"
//#include "CConfigCreatureNude.h"
//#include "CConfigMonster.h"
//#include "CConfigGoodsProperty.h"
//#include "CConfigActorExp.h"
//#include "CConfigActorHeadRes.h"
//#include "CConfigGoodsSound.h"
//#include "CConfigSecret.h"
//#include "CConfigExpDegression.h"
//#include "CConfigHorse.h"
//#include "CConfigSuitEquip.h"
//#include "CConfigCastellanGown.h"
//------------------------------------------------------------------------
class CConfigDataCenter : public IConfigDataCenter
{
public:
	// 生物属性的配置
	virtual IConfigProp *				GetConfigProp(void);

	// 取得所有地图的信息, IN : pInfor 可容纳的MAP_INFOR个数, OUT : 当前有多少张地图
	virtual BOOL						GetMapInfor(MAP_INFOR * pInfor, DWORD &dwCount);

	// 取得某一张地图的信息
	virtual MAP_INFOR *					GetMapInfor(DWORD dwMapID);

	// 取得当前地图数量
	virtual DWORD						GetMapCount(void);

	// 取得人物某一职业，某一个部件的光身部件资源ID
	// dwOccupation : 职业
	// dwPartID = 部件号
	virtual DWORD						GetNudePartForActor(DWORD dwOccupation, DWORD dwPartID, DWORD dwSex);

	// 取得人物某一职业，某一个部件的光身部件资源ID show
	// dwOccupation : 职业
	// dwPartID = 部件号
	virtual DWORD						GetNudePartForActor_Show(DWORD dwOccupation, DWORD dwPartID, DWORD dwSex);

	// 取得某个怪物的设定
	virtual MONSTER_CONFIG_INFOR &		GetMonsterInfor(long lMonsterID);

	// 取得物品基本属性
	virtual GOODS_BASEPROPERTY &		GetGoodsBaseProperty(long lGoodsID);

	// 取得药品基本属性
	virtual MEDICAMENT_BASEPROPERTY &	GetMedicamentBaseProperty(long lGoodsID);

	// 取得装备基本属性
	virtual EQUIPMENT_BASEPROPERTY &	GetEquipmentBaseProperty(long lGoodsID, long lSmeltLevel);

	// 取得凭证基本属性
	virtual WARRANT_BASEPROPERTY &		GetWarrantBaseProperty(long lGoodsID);

	// 取得元宝基本属性
	virtual VALUABLES_BASEPROPERTY &	GetValuablesBaseProperty(long lGoodsID);

	// 取得人物某一等级的经验
	virtual long						GetActorExp(long lLevel);

	// 取得人物某一职业，空手的攻击和击中声音
	virtual void						GetFreeWeaponSoundID(DWORD dwOccupation, DWORD &dwAttackSoundID, DWORD &dwBeHitSoundID);

	// 通过头像索引号取得各种类型头像ID
	virtual	DWORD						GetHeadResIDByIndex(DWORD dwIndex, DWORD dwType);

	// 通过职业和性别获取可供使用的头像索引号
	virtual vector<DWORD>				GetHeadResIDByOccupation(DWORD dwOccupation, DWORD dwSex);

	// 通过物品ID取得相关的声音
	virtual DWORD						GetGoodsSoundID(DWORD dwGoodsID, DWORD dwSoundType);

	// 取得配方信息
	virtual SECRET_INFOR *				GetSecretInfor(long lSecretID);

	// 取得经验衰减, dwMasterLevel = 人物等级, nLevelDiff = 等级差
	virtual float						GetExpDegression(DWORD dwMasterLevel, int nLevelDiff);

	// 取得座驾的配置参数
	virtual HORSE_CONFIG_INFOR *		GetHorseInfor(long lID);

	// 取得套装附加效果
	virtual SUITEQUIP_EFFECT *			GetSuitEquipEffect(long lSuitEquipID);

	// 获取城主氏族炫耀装备
	virtual SCASTELLANGOWN *			GetCastellanGown(long lCastellanID, long lOccupation);
public:
	// 载入相关脚本
	BOOL								LoadScript(void);

	CConfigDataCenter(void);

	~CConfigDataCenter(void);

private:
	// 地图信息配置
	CConfigMapInfor						m_ConfigMapInfor;

	/*
	// 生物属性配置脚本
	CConfigProp							m_ConfigProp;

	// 人物光身配置
	CConfigCreatureNude					m_ConfigCreatureNude;

	// 怪物信息配置脚本
	CConfigMonster						m_ConfigMonster;

	// 物品配置脚本
	CConfigGoodsProperty				m_ConfigGoodsProperty;

	// 人物经验脚本
	CConfigActorExp						m_ConfigActorExp;

	// 人物头像脚本
	CConfigActorHeadRes					m_ConfigActorHeadRes;	

	// 物品声音脚本
	CConfigGoodsSound					m_ConfigGoodsSound;

	// 配方脚本
	CConfigSecret						m_ConfigSecret;

	// 经验衰减脚本
	CConfigExpDegression				m_ConfigExpDegression;

	// 座驾配置参数
	CConfigHorse						m_ConfigHorse;

	// 套装脚本
	CConfigSuitEquip					m_ConfigSuitEquip;

	// 城主炫耀脚本
	CConfigCastellanGown				m_ConfigCastellanGown;
	*/
};
//------------------------------------------------------------------------