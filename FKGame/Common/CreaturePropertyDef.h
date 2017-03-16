/**
*	created:		2012-6-29   21:37
*	filename: 		CreaturePropertyDef
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		所有生物属性ID,不分人物,怪物,NPC
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Macro.h"
//------------------------------------------------------------------------
/*
**
** 添加一属性，需修改以下地方：
** 1、添加属性ID
** 2、在SACTOR_PROPERTY_LIST里添加属性变量。
** 3、在SMONSTER_PROPERTY_LIST里添加属性变量。
** 4、在prop_table.h添加修改方式。
** 5、CActor::CActor添加变量地址索引
** 6、CActor::OnSetDBContext添加存数据库变量
** 7、CActor::OnGetDBContext添加取数据库变量
** 8、CMonster::CMonster添加变量地址索引
** 9、客户端CActor::SetPropNum修改
** 10、客户端CActor::GetPropNum修改
** 11、客户端CActor::SetPropStr修改
** 12、客户端CActor::GetPropStr修改
** 13、客户端CHero::SetPropNum修改
** 14、客户端CHero::GetPropNum修改
** 15、客户端CHero::SetPropStr修改
** 16、客户端CHero::GetPropStr修改
** 17、客户端CMonster::SetPropNum修改
** 18、客户端CMonster::GetPropNum修改
** 19、客户端CMonster::SetPropStr修改
** 20、客户端CMonster::GetPropStr修改
*/
//------------------------------------------------------------------------
#pragma pack(1)	
//------------------------------------------------------------------------
// 属性应用标志
#define PROP_APPFLAG_NOSAVE				0x00000001	// 不存数据库 (默认为存数据库)
#define PROP_APPFLAG_LOCK				0x00000002	// 不允许修改 (默为为非锁定)

// 属性更新标志
#define PROP_UPDATEFLAG_UPDATE			0x00000001	// 单个属性更新 (玩家)
#define PROP_UPDATEFLAG_SYNC			0x00000002	// 公有属性广播 (玩家)(怪物)
#define PROP_UPDATEFLAG_BIG				0x00000004	// 私有属性更新 (玩家)
#define PROP_UPDATEFLAG_SYNC_UPDATE     0x00000008	// 单个属性广播 (玩家)(怪物)

// 解析属性应用标志
#define ISPROPNOSAVE(flag)				((flag & PROP_APPFLAG_NOSAVE) == PROP_APPFLAG_NOSAVE ? TRUE : FALSE)
#define ISPROPLOCK(flag)				((flag & PROP_APPFLAG_LOCK) == PROP_APPFLAG_LOCK ? TRUE : FALSE)

// 一个角色最多包裹物品件数
#define PACKET_OBJECT_INITCOUNT			36
#define PACKET_OBJECT_MAXCOUNT			60

// 一个角色仓库最多包裹个数
#define WAREHOUSE_INCLUDEPACKET_COUNT	7

// 快捷键BUF长度
#define CREATURE_SHORTCUT_BUF_LEN		80
//------------------------------------------------------------------------
// 所有生物属性ID,不分人物,怪物,NPC
enum
{
	CREATURE_PROPID_NAME = 1,			// 1	= 名字
	CREATURE_PROPID_LOC,				// 2	= 位置 
	CREATURE_PROPID_SEX,				// 3	= 性别
	CREATURE_PROPID_LEVEL,				// 4	= 等级
	CREATURE_PROPID_PHYLE,				// 5	= 种族
	CREATURE_PROPID_OCCUPATION,			// 6	= 职业	
	
	CREATURE_PROPID_HPCUR,				// 7	= 当前生命值	
	CREATURE_PROPID_HPMAXCUR,			// 8	= 当前最大生命值
	CREATURE_PROPID_MPCUR,				// 9	= 当前法术值	
	CREATURE_PROPID_MPMAXCUR,			// 10	= 当前最大法术值

	CREATURE_PROPID_P_A_MINCUR,			// 11	= 最小物理攻击力(当前)
	CREATURE_PROPID_P_A_MAXCUR,			// 12	= 最大物理攻击力(当前)
	CREATURE_PROPID_M_A_MINCUR,			// 13	= 最小法术攻击力(当前)
	CREATURE_PROPID_M_A_MAXCUR,			// 14	= 最大法术攻击力(当前)

	CREATURE_PROPID_F_A_MIN,			// 15	= 最小火法术攻击力
	CREATURE_PROPID_F_A_MAX,			// 16	= 最大火法术攻击力	
	CREATURE_PROPID_I_A_MIN,			// 17	= 最小冰法术攻击力
	CREATURE_PROPID_I_A_MAX,			// 18	= 最大冰法术攻击力	
	CREATURE_PROPID_E_A_MIN,			// 19	= 最小电法术攻击力
	CREATURE_PROPID_E_A_MAX,			// 20	= 最大电法术攻击力
	CREATURE_PROPID_N_A_MIN,			// 21	= 最小暗法术攻击力
	CREATURE_PROPID_N_A_MAX,			// 22	= 最大暗法术攻击力

	CREATURE_PROPID_P_D_MINCUR,			// 23	= 最小物理防御力(当前)
	CREATURE_PROPID_P_D_MAXCUR,			// 24	= 最大物理防御力(当前)
	CREATURE_PROPID_M_D_MINCUR,			// 25	= 最小法术防御力(当前)
	CREATURE_PROPID_M_D_MAXCUR,			// 26	= 最大法术防御力(当前)

	CREATURE_PROPID_F_D_MIN,			// 27	= 最小火法术防御力
	CREATURE_PROPID_F_D_MAX,			// 28	= 最大火法术防御力
	CREATURE_PROPID_I_D_MIN,			// 29	= 最小冰法术防御力
	CREATURE_PROPID_I_D_MAX,			// 30	= 最大冰法术防御力
	CREATURE_PROPID_E_D_MIN,			// 31	= 最小电法术防御力
	CREATURE_PROPID_E_D_MAX,			// 32	= 最大电法术防御力
	CREATURE_PROPID_N_D_MIN,			// 33	= 最小暗法术防御力
	CREATURE_PROPID_N_D_MAX,			// 34	= 最大暗法术防御力

	CREATURE_PROPID_VERACITY,			// 35	= 命中率
	CREATURE_PROPID_HEDGE,				// 36	= 躲闪率
	CREATURE_PROPID_M_SUDDENLY,			// 37	= 法术暴击率
	CREATURE_PROPID_P_SUDDENLY,			// 38	= 物理暴击率	

	CREATURE_PROPID_WALKSTYLE,			// 39	= 走路方式
	CREATURE_PROPID_MOVESPEEDLEVEL,		// 40	= 移动速度等级
	CREATURE_PROPID_ATTACKSPEEDLEVEL,	// 41	= 攻击速度等级

	CREATURE_PROPID_P_A_EFF,			// 42	= 物理攻击力(附加)
	CREATURE_PROPID_M_A_EFF,			// 43	= 法术攻击力(附加）
	
	CREATURE_PROPID_P_D_EFF,			// 44	= 物理防御力(附加）
	CREATURE_PROPID_M_D_EFF,			// 45	= 法术防御力(附加）

	CREATURE_PROPID_P_SUDDENLY_EFF,		// 46	= 物理攻击暴击率(附加）
	CREATURE_PROPID_M_SUDDENLY_EFF,		// 47	= 法术攻击暴击率(附加）	
	
	CREATURE_PROPID_P_DAMAGE_EFF,		// 48	= 物理伤害吸收点数(附加）	
	CREATURE_PROPID_M_DAMAGE_EFF,		// 49	= 法术伤害吸收点数(附加）	
	CREATURE_PROPID_ALL_DAMAGE_EFF,		// 50	= 所有伤害吸收点数(附加）	
	CREATURE_PROPID_ECHO_DAMAGE_EFF,	// 51	= 点数增加反弹伤害(附加）

	CREATURE_PROPID_VERACITY_EFF,		// 52	= 命中率(附加）
	CREATURE_PROPID_HEDGE_EFF,			// 53	= 躲闪率(附加）

	CREATURE_PROPID_HPMAX_EFF,			// 54	= 最大生命值(附加）
	CREATURE_PROPID_MPMAX_EFF,			// 55	= 最大法术值(附加）

	CREATURE_PROPID_CUREOUTPUT_PER_EFF,	// 56   = 医疗输出加成百分比(附加)
	CREATURE_PROPID_CUREINPUT_PER_EFF,	// 57   = 接受医疗加成百分比(附加)
	CREATURE_PROPID_ALL_DAMAGE_PER_EFF,	// 58	= 所有伤害吸收点数百分比(附加）

	CREATURE_PROPID_DIR,				// 59   = 方向

	CREATURE_PROPID_BASEHIGHID,			// 60	= 生物最高基本属性ID		
	
	//////////////////////////////////////////////////////////
	/*********************** 人物 ***************************/	
	CREATURE_PROPID_ACTORLOWID = 1000,	// 1000 = 人物最低专有属性ID
	CREATURE_PROPID_USERID,				// 1001 = UserID
	CREATURE_PROPID_ACTORID,			// 1002 = ActorID
	CREATURE_PROPID_HOLDMONEY_V1,		// 1003 = 身上金钱
	CREATURE_PROPID_WAREMONEY_V1,		// 1004 = 仓库金钱
	CREATURE_PROPID_EXP_V1,				// 1005 = 经验
	CREATURE_PROPID_PKVALUE,			// 1006 = PK值
	CREATURE_PROPID_BURTHENCUR,			// 1007 = 当前负重
	CREATURE_PROPID_BURTHENMAX,			// 1008 = 最大负重

	CREATURE_PROPID_FORCEBASE,			// 1009 = 力量(基础)
	CREATURE_PROPID_FORCECUR,			// 1010 = 力量(当前)	
	CREATURE_PROPID_CONSTITBASE,		// 1011 = 体质(基础)
	CREATURE_PROPID_CONSTITCUR,			// 1012 = 体质(当前)
	CREATURE_PROPID_CELERITYBASE,		// 1013 = 敏捷(基础)
	CREATURE_PROPID_CELERITYCUR,		// 1014 = 敏捷(当前)
	CREATURE_PROPID_MENTALBASE,			// 1015 = 智力(基础)
	CREATURE_PROPID_MENTALCUR,			// 1016 = 智力(当前)
	CREATURE_PROPID_WISDOMBASE,			// 1017 = 智慧(基础)
	CREATURE_PROPID_WISDOMCUR,			// 1018 = 智慧(当前)

	CREATURE_PROPID_HPMAX_BASE,			// 1019 = 最大生命值(基础)
	CREATURE_PROPID_MPMAX_BASE,			// 1020 = 最大魔法值(基础)

	CREATURE_PROPID_P_A_MINBASE,		// 1021 = 最小物理攻击力(基础)
	CREATURE_PROPID_P_A_MAXBASE,		// 1022 = 最大物理攻击力(基础)
	CREATURE_PROPID_M_A_MINBASE,		// 1023 = 最小法术攻击力(基础)
	CREATURE_PROPID_M_A_MAXBASE,		// 1024 = 最大法术攻击力(基础)
	CREATURE_PROPID_P_D_MINBASE,		// 1025 = 最小物理防御力(基础)
	CREATURE_PROPID_P_D_MAXBASE,		// 1026 = 最大物理防御力(基础)
	CREATURE_PROPID_M_D_MINBASE,		// 1027 = 最小法术防御力(基础)
	CREATURE_PROPID_M_D_MAXBASE,		// 1028 = 最大法术防御力(基础)

	CREATURE_PROPID_FORCE_EFF,			// 1029 = 力量(附加)
	CREATURE_PROPID_CONSTIT_EFF,		// 1030 = 体质(附加)
	CREATURE_PROPID_CELERITY_EFF,		// 1031 = 敏捷(附加)
	CREATURE_PROPID_MENTAL_EFF,			// 1032 = 智力(附加)
	CREATURE_PROPID_WISDOM_EFF,			// 1033 = 智慧(附加)

	CREATURE_PROPID_P_A_MIN_EFF,		// 1034 = 最小物理攻击力(附加)
	CREATURE_PROPID_P_A_MAX_EFF,		// 1035 = 最大物理攻击力(附加)

	CREATURE_PROPID_F_A_EFF,			// 1036 = 火法术攻击力(附加)
	CREATURE_PROPID_I_A_EFF,			// 1037 = 冰法术攻击力(附加)
	CREATURE_PROPID_E_A_EFF,			// 1038 = 电法术攻击力(附加)	
	CREATURE_PROPID_N_A_EFF,			// 1039 = 暗法术攻击力(附加)

	CREATURE_PROPID_F_D_EFF,			// 1040 = 火法术防御力(附加)
	CREATURE_PROPID_I_D_EFF,			// 1041 = 冰法术防御力(附加)
	CREATURE_PROPID_E_D_EFF,			// 1042 = 电法术防御力(附加)
	CREATURE_PROPID_N_D_EFF,			// 1043 = 暗法术防御力(附加)

	CREATURE_PROPID_P_A_PER_EFF,		// 1044 = 物理攻击力百分比(附加)
	CREATURE_PROPID_M_A_PER_EFF,		// 1045 = 法术攻击力百分比(附加)
	CREATURE_PROPID_CURE_PER_EFF,		// 1046 = 治疗效果增加百分比(附加)
	CREATURE_PROPID_CURE_EFF,			// 1047 = 治疗效果增加(附加)
	CREATURE_PROPID_P_D_PER_EFF,		// 1048 = 物理防御力百分比(附加)
	CREATURE_PROPID_M_D_PER_EFF,		// 1049 = 法术防御力百分比(附加)	
	CREATURE_PROPID_HPMAX_PER_EFF,		// 1050 = 最大生命值百分比(附加)
	CREATURE_PROPID_MPMAX_PER_EFF,		// 1051 = 最大法术值百分比(附加)

	CREATURE_PROPID_P_A_MIN_EQUIP,		// 1052 = 装备最小物理攻击
	CREATURE_PROPID_P_A_MAX_EQUIP,		// 1053 = 装备最大物理攻击
	CREATURE_PROPID_M_A_MIN_EQUIP,		// 1054 = 装备最小法术攻击
	CREATURE_PROPID_M_A_MAX_EQUIP,		// 1055 = 装备最大法术攻击
	CREATURE_PROPID_P_D_MIN_EQUIP,		// 1056 = 装备最小物理攻击
	CREATURE_PROPID_P_D_MAX_EQUIP,		// 1057 = 装备最大物理攻击
	CREATURE_PROPID_M_D_MIN_EQUIP,		// 1058 = 装备最小法术攻击
	CREATURE_PROPID_M_D_MAX_EQUIP,		// 1059 = 装备最大法术攻击

	CREATURE_PROPID_EQUIPCONTAINERID,	// 1060 = 装备栏容器ID
	CREATURE_PROPID_PACKETCONTAINERID,	// 1061 = 包裹栏容器ID	

	CREATURE_PROPID_HEADRESID,			// 1062 = 人物头像
	CREATURE_PROPID_EXP_EFF,			// 1063 = 经验附加（经验系数）
	CREATURE_PROPID_NAMECOLOR,			// 1064 = 名字颜色
	CREATURE_PROPID_CLANID,				// 1065 = 氏族ID
	CREATURE_PROPID_GMPOWER,			// 1066 = GM权限
	CREATURE_PROPID_WARELEVEL,			// 1067 = 仓库等级

	CREATURE_PROPID_HIDE,				// 1068 = 是否为隐身(0:非隐身，1:隐身)
	CREATURE_PROPID_CR_HIDE,			// 1069 = 是否为反隐身(0:没反隐身，1:有反隐身)

	CREATURE_PROPID_ACTORHIGHID,		// 人物最高专有属性ID

	/*********************** 怪物 ***************************/	
	CREATURE_PROPID_MONSTERLOWID = 2000,// 2000 = 怪物最低专有属性ID
	CREATURE_PROPID_MONSTERID,			// 2001 = 怪物ID
	CREATURE_PROPID_MONSTEREYESHOT,		// 2002 = 怪物视野
	CREATURE_PROPID_MONSTERENMITY,		// 2003 = 怪物仇恨值
	CREATURE_PROPID_MONSTERTRAILRADIUS,	// 2004 = 怪物跟踪半径
	CREATURE_PROPID_MONSTERCAMP,		// 2005 = 怪物阵营值
	CREATURE_PROPID_MONSTERHIGHID,		// 怪物最高专有属性ID
	
	CREATURE_PROPID_CHUNKLOWID = 10000,	// 10000 = DataChunk最低专有属性ID
	CREATURE_PROPID_CHUNKHIGHID,		// DataChunk最高专有属性ID         
};
//------------------------------------------------------------------------
#define ACTOR_PRIVATE_PROP_INTERZONE	(CREATURE_PROPID_ACTORHIGHID - CREATURE_PROPID_ACTORLOWID)
#define MONSTER_PRIVATE_PROP_INTERZONE	(CREATURE_PROPID_MONSTERHIGHID - CREATURE_PROPID_MONSTERLOWID)
//------------------------------------------------------------------------
// 描  述：ACTOR的属性表
struct SACTOR_PROPERTY_PUBLIC
{
	char	szName[MAX_NAME_LEN - 15];	// 名字，去掉15个字节是为了优化
	POINTS  ptLoc;						// 位置 
	int		nSex;						// 性别
	int		nLevel;						// 等级
	int		nOccupation;				// 职业
	
	int		nHPCur;						// 当前生命值
	int		nHPMaxCur;					// 当前最大生命值
	int		nMPCur;						// 当前法术值
	int		nMPMaxCur;					// 当前最大法术值
	
	int		nWalkStyle;					// 走路方式
	int		nMoveSpeedLevel;			// 移动速度等级	
	
	int		nUserDBID;					// UserDBID
	int		nActorDBID;					// ActorDBID
	int		nPKValue;					// PK值	
	
	int		nHeadResID;					// 人物头像
	int		nCurActState;				// 当前状态	
	int		nNameColor;					// 名字颜色
	
	int		nClanID;					// 氏族ID
	
	int		nDir;						// 方向
};
//------------------------------------------------------------------------
struct SACTOR_PROPERTY_PRIVATE : public SACTOR_PROPERTY_PUBLIC
{	
	int     nPhyle;						// 种族

	int		nForceCur;					// 力量(当前)	
	int		nConstitCur;				// 体质(当前)	
	int		nCelerityCur;				// 敏捷(当前)	
	int		nMentalCur;					// 智力(当前)	
	int		nWisdomCur;					// 智慧(当前)

	int		nP_A_MinCur;				// 最小物理攻击力(当前)
	int		nP_A_MaxCur;				// 最大物理攻击力(当前)
	int		nM_A_MinCur;				// 最小法术攻击力(当前)
	int		nM_A_MaxCur;				// 最大法术攻击力(当前)	
	int		nF_A_Min;					// 最小火法术攻击力
	int		nF_A_Max;					// 最大火法术攻击力
	int		nI_A_Min;					// 最小冰法术攻击力
	int		nI_A_Max;					// 最大冰法术攻击力
	int		nE_A_Min;					// 最小电法术攻击力
	int		nE_A_Max;					// 最大电法术攻击力
	int		nN_A_Min;					// 最小暗法术攻击力
	int		nN_A_Max;					// 最大暗法术攻击力

	int		nP_D_MinCur;				// 最小物理防御力(当前)
	int		nP_D_MaxCur;				// 最大物理防御力(当前)
	int		nM_D_MinCur;				// 最小法术防御力(当前)
	int		nM_D_MaxCur;				// 最大法术防御力(当前)
	int		nF_D_Min;					// 最小火法术防御力
	int		nF_D_Max;					// 最大火法术防御力
	int		nI_D_Min;					// 最小冰法术防御力
	int		nI_D_Max;					// 最大冰法术防御力
	int		nE_D_Min;					// 最小电法术防御力
	int		nE_D_Max;					// 最大电法术防御力
	int		nN_D_Min;					// 最小暗法术防御力
	int		nN_D_Max;					// 最大暗法术防御力

	int		nHoldMoney;					// 身上金钱
	int		nWareMoney;					// 仓库金钱
	int		nExp;						// 经验
	int		nBurthenCur;				// 当前负重
	int		nBurthenMax;				// 最大负重
	
	int		nEquipContainerID;			// 装备栏容器ID	
	int		nPacketContainerID;			// 包裹栏容器ID

	char	szShortcutBuf[CREATURE_SHORTCUT_BUF_LEN];// 快捷栏buffer
};
//------------------------------------------------------------------------
struct SACTOR_PROPERTY_LIST : public SACTOR_PROPERTY_PRIVATE
{
	int		nForceBase;					// 力量(基础)
	int		nConstitBase;				// 体质(基础)
	int		nCelerityBase;				// 敏捷(基础)
	int		nMentalBase;				// 智力(基础)
	int		nWisdomBase;				// 智慧(基础)

	int		nP_A_MinBase;				// 最小物理攻击力(基础)
	int		nP_A_MaxBase;				// 最大物理攻击力(基础)
	int		nM_A_MinBase;				// 最小法术攻击力(基础)
	int		nM_A_MaxBase;				// 最大法术攻击力(基础)
	int		nP_D_MinBase;				// 最小物理防御力(基础)
	int		nP_D_MaxBase;				// 最大物理防御力(基础)
	int		nM_D_MinBase;				// 最小法术防御力(基础)
	int		nM_D_MaxBase;				// 最大法术防御力(基础)

	int		nHPMax_Base;				// 最大生命值(基础)
	int		nMPMax_Base;				// 最大魔法值(基础)	

	int		nVeracity;					// 命中率
	int		nHedge;						// 躲闪率
	int		nP_Suddenly;				// 物理暴击率	
	int		nM_Suddenly;				// 法术暴击率

	int     nForce_Eff;					// 力量(附加)
	int     nConstit_Eff;				// 体质(附加)
	int     nCelerity_Eff;				// 敏捷(附加)
	int     nMental_Eff;				// 智力(附加)
	int     nWisdom_Eff;				// 智慧(附加)

	int     nP_A_Eff;					// 物理攻击力点数(附加)
	int     nP_A_Per_Eff;				// 物理攻击力百分比(附加)

	int     nP_A_Min_Eff;				// 最小物理攻击力(附加)
	int     nP_A_Max_Eff;				// 最大物理攻击力(附加)	

	int     nP_Suddenly_Eff;			// 物理攻击暴击率(附加)
	int     nM_Suddenly_Eff;			// 法术攻击暴击率(附加)

	int     nF_A_Eff;					// 火法术攻击力(附加)
	int     nI_A_Eff;					// 冰法术攻击力(附加)
	int     nE_A_Eff;					// 电法术攻击力(附加)	
	int     nN_A_Eff;					// 暗法术攻击力(附加)
	
	int     nM_A_Eff;					// 法术攻击力(附加)
	int     nM_A_Per_Eff;				// 法术攻击力百分比(附加)

	int     nCure_Eff;					// 治疗效果增加(附加)
	int     nCure_Per_Eff;				// 治疗效果增加百分比(附加)

	int     nF_D_Eff;					// 火法术防御力(附加)
	int     nI_D_Eff;					// 冰法术防御力(附加)
	int     nE_D_Eff;					// 电法术防御力(附加)
	int     nN_D_Eff;					// 暗法术防御力(附加)

	int     nP_D_Eff;					// 物理防御力(附加)
	int     nP_D_Per_Eff;				// 物理防御力百分比(附加)

	int     nM_D_Eff;					// 法术防御力(附加)
	int     nM_D_Per_Eff;				// 法术防御力百分比(附加)

	int     nP_Damage_Eff;				// 物理伤害吸收点数(附加)	
	int     nM_Damage_Eff;				// 法术伤害吸收点数(附加)	
	int     nAll_Damage_Eff;			// 所有伤害吸收点数(附加)	

	int     nEcho_Damage_Eff;			// 点数增加反弹伤害(附加)

	int     nVeracity_Eff;				// 命中率百分比(附加)
	int     nHedge_Eff;					// 躲闪率百分比(附加)

	int     nHPMax_Eff;					// 最大生命值(附加)
	int     nMPMax_Eff;					// 最大法术值(附加)
	int     nHPMax_Per_Eff;				// 最大生命值百分比(附加)
	int     nMPMax_Per_Eff;				// 最大法术值百分比(附加)

	int		nP_A_Min_Equip;				// 装备最小物理攻击
	int		nP_A_Max_Equip;				// 装备最大物理攻击
	int		nM_A_Min_Equip;				// 装备最小法术攻击
	int		nM_A_Max_Equip;				// 装备最大法术攻击
	int		nP_D_Min_Equip;				// 装备最小物理防御
	int		nP_D_Max_Equip;				// 装备最大物理防御
	int		nM_D_Min_Equip;				// 装备最小法术防御
	int		nM_D_Max_Equip;				// 装备最大法术防御

	int		nExp_Eff;					// 经验附加（经验系数）

	int		nCureOutput_Per_Eff;		// 医疗输出加成百分比(附加)
	int		nCureInput_Per_Eff;			// 接受医疗加成百分比(附加)
	int		nAll_Damage_Per_Eff;		// 所有伤害吸收点数百分比(附加）

	int		nGMPower;					// GM权限

	int		nAttackSpeedLevel;			// 攻击速度等级（无用的）
	int		nWareLevel;					// 仓库等级

	int		nHide;						// 是否为隐身
	int		nCr_Hide;					// 反隐身
};
//------------------------------------------------------------------------
// 描  述：MONSTER的属性表
struct SMONSTER_PROPERTY_PUBLIC
{
	char	szName[MAX_NAME_LEN];		// 名字
	POINTS  ptLoc;						// 位置 	

	int		nHPCur;						// 当前生命值
	int		nMPCur;						// 当前法术值

	int		nMonsterID;					// 怪物ID
	
	int		nWalkStyle;					// 走路方式
	int		nMoveSpeedLevel;			// 移动速度等级	

	int		nCurActState;				// 当前状态

	int		nDir;						// 方向

	int		nMonsterCamp;				// 怪物阵营
};
//------------------------------------------------------------------------
struct SMONSTER_PROPERTY_PRIVATE : public SMONSTER_PROPERTY_PUBLIC
{

};
//------------------------------------------------------------------------
struct SMONSTER_PROPERTY_LIST : public SMONSTER_PROPERTY_PRIVATE
{
	int		nLevel;						// 等级
	int		nHPMaxCur;					// 当前最大生命值
	int		nMPMaxCur;					// 当前最大法术值

	int		nSex;						// 性别
	int     nPhyle;						// 种族
	int		nOccupation;				// 职业

	int		nP_A_MinCur;				// 最小物理攻击力(当前)
	int		nP_A_MaxCur;				// 最大物理攻击力(当前)
	int		nM_A_MinCur;				// 最小法术攻击力(当前)
	int		nM_A_MaxCur;				// 最大法术攻击力(当前)
	
	int		nF_A_Min;					// 最小火法术攻击力
	int		nF_A_Max;					// 最大火法术攻击力
	int		nI_A_Min;					// 最小冰法术攻击力
	int		nI_A_Max;					// 最大冰法术攻击力
	int		nE_A_Min;					// 最小电法术攻击力
	int		nE_A_Max;					// 最大电法术攻击力
	int		nN_A_Min;					// 最小暗法术攻击力
	int		nN_A_Max;					// 最大暗法术攻击力

	int		nP_D_MinCur;				// 最小物理防御力(当前)
	int		nP_D_MaxCur;				// 最大物理防御力(当前)
	int		nM_D_MinCur;				// 最小法术防御力(当前)
	int		nM_D_MaxCur;				// 最大法术防御力(当前)

	int		nF_D_Min;					// 最小火法术防御力
	int		nF_D_Max;					// 最大火法术防御力
	int		nI_D_Min;					// 最小冰法术防御力
	int		nI_D_Max;					// 最大冰法术防御力
	int		nE_D_Min;					// 最小电法术防御力
	int		nE_D_Max;					// 最大电法术防御力
	int		nN_D_Min;					// 最小暗法术防御力
	int		nN_D_Max;					// 最大暗法术防御力

	int		nVeracity;					// 命中率
	int		nHedge;						// 躲闪率
	int		nP_Suddenly;				// 物理暴击率
	int		nM_Suddenly;				// 法术暴击率
	
	int     nP_A_Eff;					// 物理攻击力(附加)
	int     nM_A_Eff;					// 法术攻击力(附加)

	int     nP_D_Eff;					// 物理防御力(附加)
	int     nM_D_Eff;					// 法术防御力(附加)

	int     nP_Suddenly_Eff;			// 物理攻击暴击率(附加)
	int     nM_Suddenly_Eff;			// 法术攻击暴击率(附加)

	int     nP_Damage_Eff;				// 物理伤害吸收点数(附加)	
	int     nM_Damage_Eff;				// 法术伤害吸收点数(附加)	
	int     nAll_Damage_Eff;			// 所有伤害吸收点数(附加)
	int     nEcho_Damage_Eff;			// 点数增加反弹伤害(附加)

	int     nVeracity_Eff;				// 命中率(附加)
	int     nHedge_Eff;					// 躲闪率(附加)

	int     nHPMax_Eff;					// 最大生命值(附加)
	int     nMPMax_Eff;					// 最大法术值(附加)

	int		nEyeShot;					// 怪物视野
	int		nEnmity;					// 怪物仇恨值
	int     nTrailRadius;				// 怪物跟踪半径

	int		nCureOutput_Per_Eff;		// 医疗输出加成百分比(附加)
	int		nCureInput_Per_Eff;			// 接受医疗加成百分比(附加)
	int		nAll_Damage_Per_Eff;		// 所有伤害吸收点数百分比(附加）

	int		nAttackSpeedLevel;			// 攻击速度等级（无用的）
};
//------------------------------------------------------------------------
#pragma pack()
//------------------------------------------------------------------------