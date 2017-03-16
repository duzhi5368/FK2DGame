/**
*	created:		2012-6-29   20:00
*	filename: 		GoodsPropertyDef
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		物品属性定义
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <time.h>
//------------------------------------------------------------------------
#pragma pack(1)	
//------------------------------------------------------------------------
#define GOODS_CLASS_EQUIPMENT				1	// 装备
#define GOODS_CLASS_MEDICAMENT				2	// 药品
#define GOODS_CLASS_WARRANT					3	// 凭证
#define GOODS_CLASS_VALUABLES				4	// 元宝

#define GOODS_EFFECT_MAXCOUNT				10	// 一件物品的最多附加属性个数

#define EQUIPMENT_MAX_SMELTLEVEL			8	// 装备最多精炼等级	

#define EQUIPMENT_MAX_SLOTNUM				3	// 装备最多的插槽数

#define EQUIPMENT_ENDURE_SHOWSPACE			256	// 装备显示耐久 ＝ 实际耐久 / 256
//------------------------------------------------------------------------
// 物品绑定标志位
enum
{
	GOODS_BINDFLAG_SETHOLD	= 0x00000001,		// 是否设了获取绑定
	GOODS_BINDFLAG_SETUSE	= 0x00000002,		// 是否设了使用绑定
	GOODS_BINDFLAG_DIEDROP	= 0x00000004,		// 死亡掉落
	GOODS_BINDFLAG_DOFF		= 0x00000008,		// 丢弃
	GOODS_BINDFLAG_TRADE	= 0x00000010,		// 摆摊交易
	GOODS_BINDFLAG_DESTROY	= 0x00000020,		// 销毁
	GOODS_BINDFLAG_SALE		= 0x00000040,		// 出售给NPC
	GOODS_BINDFLAG_WAREHOUSE= 0x00000080,		// 个人仓库
	GOODS_BINDFLAG_PACKET	= 0x00000100,		// 包裹栏
	GOODS_BINDFLAG_EQUIP	= 0x00000200,		// 装备栏
	GOODS_BINDFLAG_CLANWARE	= 0x00000400,		// 氏族仓库
};
//------------------------------------------------------------------------
// 装备界面序号
enum
{
	EQUIPPOS_INDEX_HEADPIECE = 0,				// 0  = 头盔	
	EQUIPPOS_INDEX_SCAPULAR,					// 1  = 肩甲
	EQUIPPOS_INDEX_ARMATURE,					// 2  = 盔甲
	EQUIPPOS_INDEX_PANTS,						// 3  = 裤子
	EQUIPPOS_INDEX_ACCOUTERMENT,				// 4  = 饰品
	EQUIPPOS_INDEX_RESERVE1,					// 5  = 保留1
	EQUIPPOS_INDEX_FINGERRING1,					// 6  = 戒指1
	EQUIPPOS_INDEX_RIGHT_WEAPON,				// 7  = 右手武器
	EQUIPPOS_INDEX_LEFT_WEAPON,					// 8  = 左手武器	
	EQUIPPOS_INDEX_FINGERRING2,					// 9  = 戒指2
	EQUIPPOS_INDEX_RESERVE2,					// 10 = 保留2
	EQUIPPOS_INDEX_SHOES,						// 11 = 鞋子
	EQUIPPOS_INDEX_CUFF,						// 12 = 护腕
	EQUIPPOS_INDEX_GLOVE,						// 13 = 手套
	EQUIPPOS_INDEX_MANTEAU,						// 14 = 披风
	EQUIPPOS_INDEX_NECKLACE,					// 15 = 项链	
	EQUIPPOS_INDEX_MAX							// 16 = 最多
};
//------------------------------------------------------------------------
// 装备子类
enum
{
	EQUIPMENT_SUBCLASS_HEADPIECE = 1,			// 1  = 头盔	
	EQUIPMENT_SUBCLASS_SCAPULAR,				// 2  = 肩甲
	EQUIPMENT_SUBCLASS_ARMATURE,				// 3  = 盔甲
	EQUIPMENT_SUBCLASS_PANTS,					// 4  = 裤子
	EQUIPMENT_SUBCLASS_ACCOUTERMENT,			// 5  = 饰品
	EQUIPMENT_SUBCLASS_FINGERRING,				// 6  = 戒指
	EQUIPMENT_SUBCLASS_LEFT_WEAPON,				// 7  = 左手武器
	EQUIPMENT_SUBCLASS_RIGHT_WEAPON,			// 8  = 右手武器
	EQUIPMENT_SUBCLASS_SHOES,					// 9  = 鞋子
	EQUIPMENT_SUBCLASS_CUFF,					// 10 = 护腕
	EQUIPMENT_SUBCLASS_GLOVE,					// 11 = 手套
	EQUIPMENT_SUBCLASS_MANTEAU,					// 12 = 披风
	EQUIPMENT_SUBCLASS_NECKLACE,				// 13 = 项链
	EQUIPMENT_SUBCLASS_PAIR_WEAPON				// 14 = 双手武器
};
//------------------------------------------------------------------------
// 装备用途
enum
{
	EQUIPMENT_PURPOSE_WEAPON = 0,				// 0 : 武器
    EQUIPMENT_PURPOSE_ARMOUR,					// 1 : 防具
};
//------------------------------------------------------------------------
// 装备类型
enum
{
	EQUIPMENT_KIND_HEADPIECE = 1,				// 1  = 头盔	
	EQUIPMENT_KIND_SCAPULAR,					// 2  = 肩甲
	EQUIPMENT_KIND_ARMATURE,					// 3  = 盔甲
	EQUIPMENT_KIND_PANTS,						// 4  = 裤子
	EQUIPMENT_KIND_ACCOUTERMENT,				// 5  = 饰品
	EQUIPMENT_KIND_FINGERRING1,					// 6  = 戒指
	EQUIPMENT_KIND_SHOES,						// 7  = 鞋子
	EQUIPMENT_KIND_CUFF,						// 8  = 护腕
	EQUIPMENT_KIND_GLOVE,						// 9  = 手套
	EQUIPMENT_KIND_MANTEAU,						// 10 = 披风
	EQUIPMENT_KIND_NECKLACE,					// 11 = 项链
	EQUIPMENT_KIND_FALCHION,					// 12 = 刀
	EQUIPMENT_KIND_SWORD,						// 13 = 剑
	EQUIPMENT_KIND_SPEAR,						// 14 = 枪
	EQUIPMENT_KIND_AX,							// 15 = 斧
	EQUIPMENT_KIND_BOW,							// 16 = 弓
	EQUIPMENT_KIND_STAFF,						// 17 = 杖
	EQUIPMENT_KIND_SHIELD						// 18 = 盾牌
};
//------------------------------------------------------------------------
// 药品子类
enum
{
	MEDICAMENT_SUBCLASS_UNKNOWN = 0,			// 0  = 未知的
	MEDICAMENT_SUBCLASS_MEDICAMENT,				// 1  = 药品 (续命小还丹, 龙象丸...)
	MEDICAMENT_SUBCLASS_ROULEAU,				// 2  = 卷轴 (回城卷轴...)
	MEDICAMENT_SUBCLASS_SEAL,					// 3  = 封印 (封魔珠...)
	MEDICAMENT_SUBCLASS_BOOK,					// 4  = 天书 (天书, 镶嵌要诀...)
	MEDICAMENT_SUBCLASS_SECRET,					// 5  = 配方 (炼化配方...)
	MEDICAMENT_SUBCLASS_MAGICBOTTLE,			// 6  = 魔瓶 (炼化瓶...)
	MEDICAMENT_SUBCLASS_FACIA,					// 7  = 招牌
	MEDICAMENT_SUBCLASS_VALUABLESGOLD,			// 8  = 金元宝
	MEDICAMENT_SUBCLASS_VALUABLESSILVER,		// 9  = 银元宝
	MEDICAMENT_SUBCLASS_VALUABLESCUPRUM,		// 10 = 铜元宝
};
//------------------------------------------------------------------------
// 药品作用对像
enum
{
	MEDICAMENT_ONTARGET_SELF = 0,				// 0  = 自已
	MEDICAMENT_ONTARGET_SELELOC,				// 1  = 自已位置
	MEDICAMENT_ONTARGET_ASSIGNER,				// 2  = 指定的人
	MEDICAMENT_ONTARGET_AREA,					// 3  = 指定的区域
	MEDICAMENT_ONTARGET_TEAM,					// 4  = 队友
	MEDICAMENT_ONTARGET_MARRIAGE,				// 5  = 婚姻
	MEDICAMENT_ONTARGET_TEACH,					// 6  = 师徒	
};
//------------------------------------------------------------------------
// 药品作用类型
enum
{
	MEDICAMENT_ONTYPE_STATE = 0,				// 0  = 状态
	MEDICAMENT_ONTYPE_SKILL,					// 1  = 技能
	MEDICAMENT_ONTYPE_LUA,						// 2  = lua
	MEDICAMENT_ONTYPE_SPECIAL,					// 3  = 特定的
};
//------------------------------------------------------------------------
enum
{
	GOODS_PROPID_GOODSID = 1,					// 1  = 物品ID
	GOODS_PROPID_NAME,							// 2  = 名字
	GOODS_PROPID_LOC,							// 3  = 位置 （但是用这个宏是无法取得值的）
	GOODS_PROPID_NUMBER,						// 4  = 数量
	GOODS_PROPID_ENDURE_CUR,					// 5  = 当前耐久
	GOODS_PROPID_ENDURE_CURMAX,					// 6  = 当前最大耐久	
	GOODS_PROPID_SMELTLEVEL,					// 7  = 精炼等级
	GOODS_PROPID_SLOTLEFT						// 8  = 剩余插槽数
};
//------------------------------------------------------------------------
// 凭证子类
enum
{
	WARRANT_SUBCLASS_ORE = 1,					// 1  = 矿石
	WARRANT_SUBCLASS_HERBAGE,					// 2  = 草药
	WARRANT_SUBCLASS_MATERIAL,					// 3  = 材料
	WARRANT_SUBCLASS_STONE,						// 4  = 宝石
	WARRANT_SUBCLASS_WARRANT,					// 5  = 任务物品
	WARRANT_SUBCLASS_OTHER,						// 6  = 其他
	WARRANT_SUBCLASS_DEBRIS,					// 7  = 残骸
	WARRANT_SUBCLASS_RARE_ORE,					// 8  = 稀有矿石
	WARRANT_SUBCLASS_RARE_HERBAGE,				// 9  = 稀有草药
	WARRANT_SUBCLASS_RARE_STONE,				// 10 = 稀有晶石
};
//------------------------------------------------------------------------
// 描  述：物品的基本属性
struct SGOODS_PROPERTY_BASE
{
	DWORD	dwGoodsID;							// 1  = 物品ID
	DWORD	dwNumber;							// 4  = 数量
	DWORD	dwUseBind;							//      使用绑定
	DWORD	dwUseLock;							//		使用锁定
};
//------------------------------------------------------------------------
// 描  述：装备
struct SEQUIPMENT_PROPERTY_PUBLIC : public SGOODS_PROPERTY_BASE
{
	int		nEndureCur;							// 4  = 当前耐久
	int		nEndureCurMax;						// 5  = 当前最大耐久
	WORD	wEffect[GOODS_EFFECT_MAXCOUNT];		// 6  = 附加属性
	int		nSmeltLevel;						// 7  = 精炼等级
	int 	nSlotLeft;							// 8  = 剩余插槽数	
};
//------------------------------------------------------------------------
struct SEQUIPMENT_PROPERTY_PRIVATE : public SEQUIPMENT_PROPERTY_PUBLIC
{

};
//------------------------------------------------------------------------
struct SEQUIPMENT_PROPERTY_LIST : public SEQUIPMENT_PROPERTY_PRIVATE
{
	
};
//------------------------------------------------------------------------
// 描  述：药品
struct SMEDICAMENT_PROPERTY_PUBLIC : public SGOODS_PROPERTY_BASE
{
	BYTE	byExtend[20];						// 可扩展的buf，每种子类的所填充的数据不一样

	int		nValidFlag;							// 有效（0 = 无效，1 = 有效）
};
//------------------------------------------------------------------------
struct SMEDICAMENT_PROPERTY_PRIVATE : public SMEDICAMENT_PROPERTY_PUBLIC
{

};
//------------------------------------------------------------------------
struct SMEDICAMENT_PROPERTY_LIST : public SMEDICAMENT_PROPERTY_PRIVATE
{
	
};
//------------------------------------------------------------------------
struct SMEDICAMENT_EXTEND_FOR_MAGICBOTTLE
{
	time_t	tmStartTime;						// 开始炼化时间, 0表示未开始炼化
	time_t	tmCurrentTime;						// 当前时，没什么用，主要用来显示客户端tooltip
	BYTE	btCountOnlineFlag;					// 要不要计算在线时间
	WORD	wOnlineTime;						// 在线时间, 分钟为单位
	WORD	wSecretID;							// 配方ID	
};
//------------------------------------------------------------------------
// 描  述：凭证
struct SWARRAN_PROPERTY_PUBLIC : public SGOODS_PROPERTY_BASE
{
	WORD	wEffect[GOODS_EFFECT_MAXCOUNT];		// 6  = 附加属性
};
//------------------------------------------------------------------------
struct SWARRAN_PROPERTY_PRIVATE : public SWARRAN_PROPERTY_PUBLIC
{

};
//------------------------------------------------------------------------
struct SWARRAN_PROPERTY_LIST : public SWARRAN_PROPERTY_PRIVATE
{
	
};
//------------------------------------------------------------------------
// 描  述：元宝
struct SVALUABLES_PROPERTY_PUBLIC : public SGOODS_PROPERTY_BASE
{
	
};
//------------------------------------------------------------------------
struct SVALUABLES_PROPERTY_PRIVATE : public SVALUABLES_PROPERTY_PUBLIC
{

};
//------------------------------------------------------------------------
struct SVALUABLES_PROPERTY_LIST : public SVALUABLES_PROPERTY_PRIVATE
{
	
};
//------------------------------------------------------------------------
#pragma pack()	
//------------------------------------------------------------------------