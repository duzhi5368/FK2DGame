/**
*	created:		2012-6-29   20:02
*	filename: 		IConfigDataCenter
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		数据配置中心,通过读取脚本获得各种服务器数值配置, 然后通过全局的实例获取	
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "GoodsPropertyDef.h"
#include "Macro.h"
#include <vector>
using std::vector;
//------------------------------------------------------------------------
// 申明
struct IConfigProp;
//------------------------------------------------------------------------
// 人物头像类型
enum
{
	ACTORHEAD_RESTYPE_TARGET = 0,					// 目标环头像
	ACTORHEAD_RESTYPE_RESUME,						// 自我介绍头像
	ACTORHEAD_RESTYPE_TEAM_COLOR,					// 组队头像（彩色）
	ACTORHEAD_RESTYPE_TEAM_INVALID,					// 组队头像（灰色）
	ACTORHEAD_RESTYPE_FRIEND_COLOR,					// 好友头像（彩色）
	ACTORHEAD_RESTYPE_FRIEND_INVALID,				// 好友头像（灰色）
};
//------------------------------------------------------------------------
// 物品音效类型
enum
{
	GOODSSOUND_TYPE_SNATCH = 0,						// 提起声音
	GOODSSOUND_TYPE_DROP,							// 放下声音
	GOODSSOUND_TYPE_THROW,							// 丢弃声音
	GOODSSOUND_TYPE_USE,							// 使用声音
	GOODSSOUND_TYPE_ATTACK,							// 攻击声音
	GOODSSOUND_TYPE_BEHIT,							// 击中声音
	GOODSSOUND_TYPE_MANGLE,							// 损坏声音
};
//------------------------------------------------------------------------
// "scp\\mapinfor.scp"
struct MAP_INFOR
{	
	long			napplyserver;					// 应用何个服务器
	long			nopen;							// 要不要开放
	long			nmapid;							// 地图ID	
	char			szmapname[MAX_PATH];			// 地图名字
	char			szmapfile[MAX_PATH];			// .map文件名
	char			szmaifile[MAX_PATH];			// .mai文件名
	char			szminimapfile[MAX_PATH];		// 小地图名
	char			szworldmapfile[MAX_PATH];		// 世界地图名
	long			lusedynamic;					// 客户端是否要动态载入[默认 = 1]
	long			lminimaprate;					// 小地图的缩小比例, [默认 = 20]
	long			lstaticrelivepoint[3];			// 在本静态地图死亡重生点(staticmap, loc.x, loc.y)
	long			ldynamicrelivepoint[3];			// 在本动态地图死亡重生点(staticmap, loc.x, loc.y)
	long			lredstaticrelivepoint[3];		// 红名在本静态地图死亡重生点(staticmap, loc.x, loc.y)
	long			lreddynamicrelivepoint[3];		// 红名在本动态地图死亡重生点(staticmap, loc.x, loc.y)
	char			szmonstercreaterfile[MAX_PATH];	// 怪物生成器脚本名（包括传统的NPC）
	char			szchunnelcreaterfile[MAX_PATH];	// 传送门生成器脚本名
	char			szboxcreaterfile[MAX_PATH];		// 宝箱生成器脚本名
	long			lbackgroundmusic;				// 背景音乐
	long			lmapsizetileX;					// 地图大小(TILE)
	long			lmapsizetileY;					// 地图大小(TILE)
	long			llandfallpoint[2];				// 着陆点，在任何时候，都是有效的，主要用于如果玩家上线，位置出错，则在此上线
};
//------------------------------------------------------------------------
// "scp\\monster.csv"
// 怪物最多支持的技能
#define MONSTER_SUPPORT_SKILL		4
struct MONSTER_CONFIG_INFOR
{
	char			szmonstername[MAX_NAME_LEN];	// 名字
	long			lmonsterid;						// 怪物ID
	long			lresid;							// 资源ID	
	long			lheadresid;						// 头像图片
	long			lpalette;						// 调色板	
	long			ltype;							// 类型, 1 ＝ 普通怪, 2 ＝ 精英怪, 3 ＝ 稀有怪, 4 ＝ BOSS。。。见DThingGlobal.h
	long			lphyle;							// 种族, 1 = 人族, 2 = 妖族, 3 = 亡灵, 4 = 自然, 5 = 机关, 6 = 野兽 
	long			lcamp;							// 阵营, 
	long			llevel;							// 等级
	long			lhp;							// 生命
	long			lmp;							// 魔法
	long			lexp;							// 经验值
	long			lexpcoefficie;					// 经验修正系数
	long			lmovespeed;						// 移动速度
	long			lattackspeed;					// 攻击速度
	long			lmindam;						// 最小物理攻击
	long			lmaxdam;						// 最大物理攻击
	long			lminmagdam;						// 最小法术攻击
	long			lmaxmagdam;						// 最大法术攻击
	long			lmindefend;						// 最小物理防御
	long			lmaxdefend;						// 最大物理防御
	long			lminmagdefend;					// 最小法术防御
	long			lmaxmagdefend;					// 最大法术防御
	int				lP_Suddenly;					// 物理暴击率
	int				lM_Suddenly;					// 法术暴击率
	long			leyeshot;						// 视野
	long			lhit;							// 命中
	long			ldodge;							// 躲闪
	long			lfireresist;					// 火法术防御
	long			lfrozenresist;					// 冰法术防御
	long			llightresist;					// 电法术防御
	long			lpoisonresist;					// 暗法术防御
	long			luseskill[MONSTER_SUPPORT_SKILL * 3];// 使用技能 [技能id, 使用概率, 使用时间]
	long			lprizeholdstrategy;				// 奖励拥有策略
};
//------------------------------------------------------------------------
// 每个物品都必须的基本属性
struct GOODS_BASEPROPERTY
{
	long			lGoodsID;						// 物品ID
	char			szName[MAX_NAME_LEN];			// 物品名字
	long			lClass;							// 物品分类（1 = 装备 2 = 药品 3 = 凭证 4 = 元宝）
	long			lSubClass;						// 物品子类
	long			lAllowPhyle;					// 使用种族
	long			lAllowOccupation;				// 使用职业
	long			lAllowLevel;					// 使用等级
	long			lWeight;						// 重量
	long			lPileNum;						// 叠放数量
	long			lBuyPrice;						// 买入基础价格
	long			lSellPrice;						// 卖出基础价格	
	long			lHoldNum;						// 最多拥有个数0：无数个
	long			lRank;							// 基础物品档次
	long			lIsShortcut;					// 能否放快捷栏
	long			lHoldBind;						// 获取绑定
	long			lUseBind;						// 使用绑定
	long			lBindFlag;						// 绑定标志位
	char			szDesc[256];					// 描述
	long			lArticleID_z;					// 物品栏图片（正常）
	long			lArticleID_g;					// 物品栏图片（高亮）
	long			lArticleID_w;					// 物品栏图片（无效）
	long			lPopedomTime;					// 掉在地上的权限时间
	long			lDestroyTime;					// 掉在地上的消毁时间
	long			lShowToTeam;					// 拾到后同队伍广播(0：不广播，1：广播)
	long			lShowToMap;						// 拾到后同地图广播(0：不广播，1：广播)
	long			lShowToWorld;					// 拾到后同游戏广播(0：不广播，1：广播)
	long			lShowToClan;					// 拾到后同氏族广播(0：不广播，1：广播)
};
//------------------------------------------------------------------------
// "scp\\Equipment.cvs"
struct EQUIPMENT_BASEPROPERTY : public GOODS_BASEPROPERTY
{
	long			lEquipKind;						// 装备种类
	long			lActionMode;					// 攻击动作
	long			lEquipPos1;						// 可装备的位置1，无效填-1
	long			lEquipPos2;						// 可装备的位置2，无效填-1
	long			lEquipPos3;						// 可装备的位置3，无效填-1
	long			lAppraisePrice;					// 鉴定费用
	long			lAttackSpeed;					// 攻击速度
	long			lP_AttackMin;					// 增加物理攻击力最小值
	long			lP_AttackMax;					// 增加物理攻击力最大值
	long			lP_DefendMin;					// 增加物理防御力最小值
	long			lP_DefendMax;					// 增加物理防御力最大值
	long			lM_AttackMin;					// 增加法术攻击力最小值
	long			lM_AttackMax;					// 增加法术攻击力最大值
	long			lM_DefendMin;					// 增加法术防御力最小值
	long			lM_DefendMax;					// 增加法术防御力最大值
	long			lEndureMax;						// 最大耐久上限
	long			lEffectMax;						// 附加属性最多项
	long			lAniID1_partid;					// 部件ID
	long			lAniID1_m;						// 男外形1
	long			lAniID1_w;						// 女外形1
	long			lAniID2_partid;					// 部件ID
	long			lAniID2_m;						// 男外形2
	long			lAniID2_w;						// 女外形2
	long			lAniID3_partid;					// 部件ID
	long			lAniID3_m;						// 男外形3
	long			lAniID3_w;						// 女外形3
	long			lAniID4_partid;					// 部件ID
	long			lAniID4_m;						// 男外形4
	long			lAniID4_w;						// 女外形4

	long			lAniID1_show_partid;			// 部件ID
	long			lAniID1_show_m;					// 男外形1
	long			lAniID1_show_w;					// 女外形1
	long			lAniID2_show_partid;			// 部件ID
	long			lAniID2_show_m;					// 男外形2
	long			lAniID2_show_w;					// 女外形2
	long			lAniID3_show_partid;			// 部件ID
	long			lAniID3_show_m;					// 男外形3
	long			lAniID3_show_w;					// 女外形3
	long			lAniID4_show_partid;			// 部件ID
	long			lAniID4_show_m;					// 男外形4
	long			lAniID4_show_w;					// 女外形4

	long			lPalette_m_s[EQUIPMENT_MAX_SMELTLEVEL];	// 精炼 男调色板
	long			lPalette_w_s[EQUIPMENT_MAX_SMELTLEVEL];	// 精炼 女调色板

	long			lPurposeFlag;					// 用途, 武器还是防具

	long			lSuitEquipID;					// 套装ID
};
//------------------------------------------------------------------------
// "scp\\Medicament.cvs"
struct MEDICAMENT_BASEPROPERTY : public GOODS_BASEPROPERTY
{
	long			lUseNum;						// 使用次数0：无限次使用
	long			lReactID;						// 作用ID
	long			lSecretID;						// 配方ID
};
//------------------------------------------------------------------------
// "scp\\Warrant.cvs"
struct WARRANT_BASEPROPERTY : public GOODS_BASEPROPERTY
{

};
//------------------------------------------------------------------------
// "scp\\Valuables.cvs"
struct VALUABLES_BASEPROPERTY : public GOODS_BASEPROPERTY
{

};
//------------------------------------------------------------------------
// "scp\\secret.res"
struct SECRET_INFOR
{
	struct STUFF_INFOR
	{
		long		lGoodsID;						// 物品id
		long		lNum;							// 所需数量
		char		szSource[128];					// 获取方式
	};

	long			lSecretID;						// 配方id
	char			szCondition[256];				// 条件lua函数
	
	long			lMagicBottleGoodsID;			// 炼化瓶物品ID
	char			szMagicBottleSource[128];		// 炼化瓶获取方式

	long			lStuffTotal;					// 原材料种类总数
	STUFF_INFOR		StuffInfor[20];					// 原材料信息
	
	long			lEpurateTime;					// 炼化时间，分钟
	char			szEpurateResult[128];			// 炼化结果说明
	char			szEpurateLuaFunction[256];		// 炼化成功执行的lua函数

	long			lCountOnlinePrecision;			// 统计在线时间精度
};
//------------------------------------------------------------------------
// "scp\\horse.cse"
struct HORSE_CONFIG_INFOR
{
	long			lid;							// ID
	char			szhorsename[MAX_NAME_LEN];		// 名字
	long			lallowlevel;					// 需要等级
	long			lmovespeed;						// 移动速度档次
	long			lpart1_no;						// 部件1_号，决定装在何个位置
	long			lpart1_resid;					// 部件1_资源id
	long			lpart1_pal;						// 部件1_调色板
	long			lpart2_no;						// 部件2_号，决定装在何个位置
	long			lpart2_resid;					// 部件2_资源id
	long			lpart2_pal;						// 部件2_调色板
	char			szdesc[256];					// 描述
};
//------------------------------------------------------------------------
// "scp\\suitequip.res"
struct SUITEQUIP_EFFECT
{
	long			lID;							// 套装ID
	char			szName[MAX_NAME_LEN];			// 套装名字
	long			lMaxPiece;						// 套装最大件数
	long			lEffect[EQUIPPOS_INDEX_MAX][12];// 套装附加属性
};
//------------------------------------------------------------------------
// "data\\scp\\CastellanGown.cse"
struct SCASTELLANGOWN
{
	long			lManteauPartID;					// 披风部件id
	long			lManManteauResID;				// 男城主氏族成员披风资源id
	long			lManManteauPalID;				// 男城主氏族成员披风调色板id
	long			lWomanManteauResID;				// 女城主氏族成员披风资源id
	long			lWomanManteauPalID;				// 女城主氏族成员披风调色板id
	long			lHorsePartID1;					// 城主马的部件1id
	long			lManHorsePartResID1;			// 男城主马部件1的资源id
	long			lManHorsePartPalID1;			// 男城主马部件1的调色板id
	long			lWomanHorsePartResID1;			// 女城主马部件1的资源id
	long			lWomanHorsePartPalID1;			// 女城主马部件1的调色板id
	long			lHorsePartID2;					// 城主马的部件2id
	long			lManHorsePartResID2;			// 男城主马部件2的资源id 
	long			lManHorsePartPalID2;			// 男城主马部件2的调色板id
	long			lWomanHorsePartResID2;			// 女城主马部件2的资源id
	long			lWomanHorsePartPalID2;			// 女城主马部件2的调色板id
};
//------------------------------------------------------------------------
// 定义
struct IConfigDataCenter
{
	// 生物属性的配置
	virtual IConfigProp *				GetConfigProp(void) = NULL;

	// 取得所有地图的信息, IN : pInfor 可容纳的MAP_INFOR个数, OUT : 当前有多少张地图
	virtual BOOL						GetMapInfor(MAP_INFOR * pInfor, DWORD &dwCount) = NULL;
	
	// 取得某一张地图的信息
	virtual MAP_INFOR *					GetMapInfor(DWORD dwMapID) = NULL;
	
	// 取得当前地图数量
	virtual DWORD						GetMapCount(void) = NULL;

	// 取得人物某一职业，某一个部件的光身部件资源ID
	// dwOccupation : 职业
	// dwPartID = 部件号
	virtual DWORD						GetNudePartForActor(DWORD dwOccupation, DWORD dwPartID, DWORD dwSex) = NULL;

	// 取得人物某一职业，某一个部件的光身部件资源ID show
	// dwOccupation : 职业
	// dwPartID = 部件号
	virtual DWORD						GetNudePartForActor_Show(DWORD dwOccupation, DWORD dwPartID, DWORD dwSex) = NULL;

	// 取得某个怪物的设定
	virtual MONSTER_CONFIG_INFOR &		GetMonsterInfor(long lMonsterID) = NULL;

	// 取得物品基本属性
	virtual GOODS_BASEPROPERTY &		GetGoodsBaseProperty(long lGoodsID) = NULL;

	// 取得药品基本属性
	virtual MEDICAMENT_BASEPROPERTY &	GetMedicamentBaseProperty(long lGoodsID) = NULL;

	// 取得装备基本属性
	virtual EQUIPMENT_BASEPROPERTY &	GetEquipmentBaseProperty(long lGoodsID, long lSmeltLevel) = NULL;

	// 取得凭证基本属性
	virtual WARRANT_BASEPROPERTY &		GetWarrantBaseProperty(long lGoodsID) = NULL;

	// 取得元宝基本属性
	virtual VALUABLES_BASEPROPERTY &	GetValuablesBaseProperty(long lGoodsID) = NULL;

	// 取得人物某一等级的经验
	virtual long						GetActorExp(long lLevel) = NULL;

	// 取得人物某一职业，空手的攻击和击中声音
	virtual void						GetFreeWeaponSoundID(DWORD dwOccupation, DWORD &dwAttackSoundID, DWORD &dwBeHitSoundID) = NULL;

	// 通过头像索引号取得各种类型头像ID
	virtual	DWORD						GetHeadResIDByIndex(DWORD dwIndex, DWORD dwType) = NULL;

	// 通过职业和性别获取可供使用的头像索引号
	virtual vector<DWORD>				GetHeadResIDByOccupation(DWORD dwOccupation, DWORD dwSex) = NULL;

	// 通过物品ID取得相关的声音
	virtual DWORD						GetGoodsSoundID(DWORD dwGoodsID, DWORD dwSoundType) = NULL;

	// 取得配方信息
	virtual SECRET_INFOR *				GetSecretInfor(long lSecretID) = NULL;

	// 取得经验衰减, dwMasterLevel = 人物等级, nLevelDiff = 等级差
	virtual float						GetExpDegression(DWORD dwMasterLevel, int nLevelDiff) = NULL;

	// 取得座驾的配置参数
	virtual HORSE_CONFIG_INFOR *		GetHorseInfor(long lID) = NULL;

	// 取得套装附加效果
	virtual SUITEQUIP_EFFECT *			GetSuitEquipEffect(long lSuitEquipID) = NULL;

	// 获取城主氏族炫耀装备
	virtual SCASTELLANGOWN *			GetCastellanGown(long lCastellanID, long lOccupation) = NULL;
};
//------------------------------------------------------------------------