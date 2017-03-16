/**
*	created:		2012-6-29   19:49
*	filename: 		CreatureViewDef
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		生物相关的接口、定义等信息
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define		MAX_CREATURE_TYPES		40	//最大生物类型
#define		MAX_PAL_TYPES			15	//最大调色板类型

#define		MAN_CREATURE_TYPE		0	//标准人型男的type
#define		FEMALE_CREATURE_TYPE	1	//标准人型女的type

#define		MAX_ACTOR_NUM			3	//一个账号能创建的最大角色数
//------------------------------------------------------------------------
class CCreatureView;
//------------------------------------------------------------------------
struct SSkillCreateParam
{
	SSkillCreateParam()
	{
		memset(this, 0, sizeof(SSkillCreateParam));
	}

	// 视图模块填充的变量
	POINT	ptTile;						// 技能起始点(填入生物的脚底TILE或陷阱的TILE等)
	int		nAniType;					// 动画类型，参考enum AniType
	
	// 逻辑模块需要填充的变量
	int		nIndex;						// 索引号(技能ID)
	int		nLevel;						// 技能等级(技能动画用到)
	int		nCheckId;					// 技能的较检ID，唯一确定
	POINT	ptAttack;					// 攻击点(有的是鼠标点击处的TILE，有的是鼠标点击处生物的TILE)

	CCreatureView	*pAttackView;		// 被攻击者的指针

	DWORD	dwReserve1;					// 保留1
	DWORD	dwReserve2;					// 保留2
	DWORD	dwReserve3;					// 保留3
	DWORD	dwReserve4;					// 保留4
	
};
//------------------------------------------------------------------------
// 飘血类型
enum enFlowType
{
	FlowTypeCommon = 0,					// 飘正常数字
	FlowTypeCrazy,						// 飘暴击
	FlowTypeResist,						// 飘抵抗
	FlowTypeHedge,						// 飘躲闪
};
//------------------------------------------------------------------------
// 飘血和魔法的数据
struct SFlowParam
{
	SFlowParam()
	{
		memset(this, 0, sizeof(SFlowParam));
	}
	enFlowType	nFlowType;				// 飘的类型
	int			nHP;					// 飘HP
};
//------------------------------------------------------------------------
// 生物被击时传入的参数
struct SBehitParam
{
	SBehitParam()
	{
		memset(this, 0, sizeof(SBehitParam));
	}

	int				nCheckId;			// 技能的较检ID，唯一确定
	BOOL			bDoAct;				// 是否要做被击动作
	SFlowParam		FlowParam;			// 飘血和魔法的数据
	CCreatureView	*pSourceView;		// 攻击者的指针
};
//------------------------------------------------------------------------
// 角色移动方式
enum enActorMoveStyle
{
	ActMoveStyleWalk = 0,				// 行走
	ActMoveStyleRun,					// 跑步
	ActMoveStyleRide,					// 骑马跑
};
// 角色携带武器的方式
enum enActorWeaponStyle
{
	ActWeaponStyleSingle = 0,			// 单手武器（包括空手）
	ActWeaponStyleDouble,				// 双手武器
	ActWeaponStyleBow,					// 弓箭类
	
	ActWeaponStyleMax,
};
//------------------------------------------------------------------------
// NPC类别对应的头顶光效
enum enNpcType
{
	NpcTypeNone = 0,					// 无需顶光效
	NpcTypeTrans,						// 魔法门	（传送员）
	NpcTypeSkill,						// 魔法球	（技能学习师）
	NpcTypeMedicinal,					// 大药瓶	（药品商）
	NpcTypeWeapon,						// 刀剑		（武器商）
	NpcTypeArmor,						// 铠甲		（装备商）
	NpcTypeRide,						// 马		（骑乘商）
	NpcTypeSmith,						// 铁锤		（铁匠）
	NpcTypeGrocer,						// 包裹		（杂货商）
	
	NpcTypeMax,
};

// 任务NPC头顶的光效
enum enTaskType
{
	TaskTypeNone = 0,					// 无需顶光效
	TaskType_Get_Can,					// 小卷轴-亮		（有任务，并满足接受条件）
	TaskType_Get_NoCanGet,				// 小卷轴-灰		（有任务，但未满足接受条件）
	TaskType_Give_Complete,				// 闭合小卷轴-亮	（有可交付任务，并已完成）
	TaskType_Give_NoComplete,			// 闭合小卷轴-灰	（有可交付任务，但还未完成）
	
	TaskTypeMax,
};
//------------------------------------------------------------------------
// 角色组件类型(适用于角色、怪物、NPC)
enum enCreaturePart
{
	CrtPartHead = 0,					//0		头
	CrtPartHeadEffect,					//1		头特效
	CrtPartUpBody,						//2		上半身
	CrtPartUpBodyEffect,				//3		上半身特效
	CrtPartDownBody,					//4		下半身
	CrtPartDownBodyEffect,				//5		下半身特效
	CrtPartBody,						//6		身体（要注意指的是腰带）
	CrtPartBodyEffect,					//7		身体特效（要注意指的是腰带）
	CrtPartLeftShoulder,				//8		左肩
	CrtPartLeftShoulderEffect,			//9		左肩特效
	CrtPartRightShoulder,				//10	右肩
	CrtPartRightShoulderEffect,			//11	右肩特效
	CrtPartLeftHand,					//12	左手
	CrtPartLeftHandEffect,				//13	左手特效
	CrtPartRightHand,					//14	右手
	CrtPartRightHandEffect,				//15	右手特效
	CrtPartLeftEquip,					//16	左手装备
	CrtPartLeftEquipEffect,				//17	左手装备特效
	CrtPartRightEquip,					//18	右手装备
	CrtPartRightEquipEffect,			//19	右手装备特效
	CrtPartBothHandsEquip,				//20	双手装备
	CrtPartBothHandsEquipEffect,		//21	双手装备特效
	CrtPartBackEquip,					//22	背部装备
	CrtPartBackEquipEffect,				//23	背部装备特效
	CrtPartTail,						//24	尾巴
	CrtPartTailEffect,					//25	尾巴特效
	CrtPartWing,						//26	翅膀
	CrtPartWingEffect,					//27	翅膀特效
	CrtPartWholeBody,					//28	全身体
	CrtPartWHoleBodyEffect,				//29	全身体特效
	CrtPartExternsion,					//30	附件
	CrtPartExternsionEffect,			//31	附件特效

	CrtPartMax = 32,					//32	生物最大部件数
};
//------------------------------------------------------------------------
// 角色状态
enum enCreatureState
{
	CrtStateStand = 0,					//0		站立
	CrtStateWalk,						//1		行走
	CrtStateRun,						//2		奔跑
	CrtStateDie,						//3		死亡
	CrtStateFallow,						//4		休闲
	CrtStateBehit,						//5		被击
	CrtStateSit,						//6		坐下
	CrtStateJuqi1,						//7		聚气1
	CrtStateJuqi2,						//8		聚气2
	CrtStateMagic1,						//9		施法1
	CrtStateMagic2,						//10	施法2
	CrtStateSingleAttack1,				//11	单手武器攻击1
	CrtStateSingleAttack2,				//12	单手武器攻击2
	CrtStateSingleAttack3,				//13	单手武器攻击3
	CrtStateDoubleAttack1,				//14	双手武器攻击1
	CrtStateDoubleAttack2,				//15	双手武器攻击2
	CrtStateArrowAttack,				//16	弓箭武器攻击
	CrtStateRideStand,					//17	骑马站立
	CrtStateRideRun,					//18	骑马奔跑
	CrtStateCommonAttack1,				//19	普通攻击1
	CrtStateCommonAttack2,				//20	普通攻击2
	CrtStateSkillAttack1,				//21	技能攻击1
	CrtStateSkillAttack2,				//22	技能攻击2

	CrtStateMax = 23,					//23	生物最大状态数
};
//------------------------------------------------------------------------
// 生物方向
enum enCreatureDir
{
	CrtDirNorth = 0,					// 北
	CrtDirEastNorth,					// 东北
	CrtDirEast,							// 东
	CrtDirEastSouth,					// 东南
	CrtDirSouth,						// 南
	CrtDirWestSouth,					// 西南
	CrtDirWest,							// 西
	CrtDirWestNorth,					// 西北

	CrtDirMaxDirCount = 8,				// 生物的最大方向数目
};
//------------------------------------------------------------------------
// 生物的属性定义(下面的"设置或返回"表示当调用OnCommand时dwParam2参数的设置或返回的数据类型)
enum enCreatureProp
{
	CrtPropNull = 0,					// 空属性
	CrtPropMoveStyle,					// 移动类型						设置或返回: DWORD* (ActMoveStyleWalk or ActMoveStyleRun or ActMoveStyleRide)
	CrtPropTile,						// 生物的TILE坐标				设置或返回: POINT*
	CrtPropUserName,					// 生物的名字					设置或返回: char*
	CrtPropShowName,					// 是否让生物显示自己的名字		设置或返回: BOOL*
	CrtPropNameColor,					// 生物名字的颜色				设置或返回: WPixel*
	CrtPropAbsolutePosition,			// 生物的绝对位置				返回: POINT*
	CrtPropWeaponStyle,					// 角色携带武器的方式(单手武器?双手武器?弓箭类武器?)	设置或返回: int* enActorWeaponStyle 类型
	CrtPropWeaponSoundID,				// 角色武器的攻击音效ID			设置或返回: int*
	CrtPropWeaponBeHitSoundID,			// 角色武器的击中目标音效ID		设置或返回: int*
	CrtPropIsMoving,					// 生物是否正在移动?			返回: BOOL*
	CrtPropState,						// 生物的状态					设置或返回: int*
	CrtPropDirection,					// 生物的方向					设置或返回: int*
	CrtPropMoveSpeedLevel,				// 生物的移动速度等级(0->8)		设置或返回: int*
	CrtPropAttackSpeedLevel,			// 生物的攻击速度等级(0->8)		设置或返回: int
	CrtPropGroupInfoImage,  			// 帮会信息图片					设置或返回: IBitmap**

	CrtPropMaxPropCount, 				// 最大可能属性数目
};
//------------------------------------------------------------------------
// 生物与外界的交互命令及其参数说明,其中?表示此参数没有使用(请传入0)
enum enCreatureCommand
{
	CrtCmdNull = 0,						// 空命令
										// dwParam1: ?
										// dwParam2: ?

	CrtCmdSetProperty,					// 设置某些属性的命令
										// dwParam1: 属性类型
										// dwParam2: 设置值(参看角色的属性定义)

	CrtCmdGetProperty,					// 得到某些属性的命令
										// dwParam1: 属性类型
										// dwParam2: 返回值(参看角色的属性定义)
				
	CrtCmdMove,							// 移动命令,以当前的移动方式移动
										// dwParam1: 路径列表指针(POINTS*);
										// dwParam2: 路径列表节点数目
				
	CrtCmdBeHit,						// 生物被击
										// dwParam1: 生物被击时传入的相关参数(SBehitParam*)
										// dwParam2: ?
					
	CrtCmdDie,							// 生物死亡
										// dwParam1: 0表示慢慢死亡，有动作，1表示已经死亡，无动作
										// dwParam2: ?
					
	CrtCmdFallow,						// 生物休闲
										// dwParam1: ?
										// dwParam2: ?

	CrtCmdChangeModule,					// 换装，变换生物的身体
										// dwParam1: 物品ID
										// dwParam2: 
				
	CrtCmdSpecialModule,				// 设置角色默认部件,如果角色图包不存在,使用默认图包
										// dwParam1: 部件类型,头、身体、手等
										// dwParam2: 部件默认ID						
				
	CrtCmdBorn,							// 角色出生(发此命令可以播放一段出生动画)
										// dwParam1: ?
										// dwParam2: ?
				
	CrtCmdUpgrade,						// 角色升级(发此命令可以播放一段升级动画)
										// dwParam1: ?
										// dwParam2: ?
				
	CrtCmdStand,						// 角色站立(服务器可用来强行拉人;如果生物正在移动，则停止移动)
										// dwParam1: 站立到新的TILE位置(POINT*)
										// dwParam2: ?
				
	CrtCmdAppearance,					// 生物的外观(主要是一些跟随物)
										// dwParam1: 是设置还是取消(TRUR or FALSE)
										// dwParam2: 外观ID (DWORD)
				
	CrtCmdBloodInfo,					// 生物改变血量(如升级或穿上增加血量的装备等)
										// dwParam1: 生物当前还有多少血(int*)
										// dwParam2: 生物当前总共能容纳多少血(int*)

	CrtCmdChatInfo,						// 角色头顶的聊天信息
										// dwParam1: 聊天内容(LPSTR)
										// dwParam2: 文字颜色(WPixel*)
				
	CrtCmdMouseAni,						// 生物(这里只有主角)行走时，在路上点击鼠标留下的点击动画
										// dwParam1: 点击点TILE坐标(POINT*)
										// dwParam2: ?
				
	CrtCmdModulePalType,				// 生物(这里只有角色)各组件的调色板类型索引
										// dwParam1: 组件类型(int), 详细请参考生物部件脚本
										// dwParam2: 调色板类型索引(int),1-type1,2-type2,...,0表示用图包固有的调色板
				
	CrtCmdModuleAlpha,					// 改变角色透明度
										// dwParam1: 组件的类型(int), 详细请参考生物部件脚本
										// dwParam2: 组件的透明度, 详细请参考生物编辑器
					
	CrtCmdSetting,						// 生物坐下(不坐下)
										// dwParam1: BOOL(标识坐下还是不坐下)
										// dwParam2: ?
				
	CrtCmdAddTitle,						// 给生物添加称号
										// dwParam1: LPCSTR(格式化后的称号字符串)
										// dwParam2: DWORD(称号效果的ID)
					
	CrtCmdClearTitle,					// 清除生物的所有称号
										// dwParam1: ?
										// dwParam2: ?

// 技能相关的 >>
	CrtCmdPreAttack,					// 蓄气(转到蓄气状态，蓄气光效启动)
										// dwParam1: 魔法(技能)的创建参数(SSkillCreateParam*)
										// dwParam2: ?
				
	CrtCmdStopPreAttack,				// 停止蓄气(转到站立状态，蓄气光效停止)
										// dwParam1: ?
										// dwParam2: ?
				
	CrtCmdAttack,						// 攻击(转到攻击状态，包括瞬间施法、持续施法和普通攻击，施法光效启动，法术动画启动)
										// dwParam1: 魔法(技能)的创建参数(SSkillCreateParam*)
										// dwParam2: ?

	CrtCmdStopAttack,					// 停止攻击（只对持续施法有效，转到站立状态，施法光效停止，法术动画停止）
										// dwParam1: ?
										// dwParam2: ?
						
	CrtCmdChangeLocForSkill,			// 角色用技能顺移到某个Tile上
										// dwParam1: 顺移到新的TILE位置(POINT*)
										// dwParam2: ?

	CrtCmdSkillSelAni,					// 范围技能的选择光效
										// dwParam1: BOOL(标识设置还是取消)
										// dwParam2: 技能ID

	CrtCmdSetNpcType,					// 设置NPC类别对应的头顶光效
										// dwParam1: NPC类别对应的头顶光效 参看enNpcType
										// dwParam2: ?
										
	CrtCmdSetTaskType,					// 任务NPC头顶的光效
										// dwParam1: 任务NPC头顶的光效 参看enTaskType
										// dwParam2: ?

	CrtCmdSetClanName,					// 给生物设置氏族名称
										// dwParam1: LPCSTR(氏族名称)
										// dwParam2: ?

	CrtCmdSetCityName,					// 给生物设置城池名称
										// dwParam1: LPCSTR(城池名称)
										// dwParam2: ?

	CrtCmdMaxCommandCount,				// 最大可能的命令数
};
//------------------------------------------------------------------------
// 这里加入一些地上物的定义，以后考虑要不要独立出去
enum enArticleProp
{
	AtcPropNull = 0,					// 空属性
	AtcPropName,						// 物品的名字属性					设置或返回: char*				
	AtcPropShowName,					// 是否让地上物品显示自己的名字		设置或返回: BOOL*
	AtcPropNameColor,					// 物品的名字显示颜色				设置或返回: WPixel*
	AtcPropBkImage,						// 地上物品的背景图片(用于摆摊的文字下面的背景图片)		设置或返回: IBitmap**
	AtcPropShowStar,					// 是否让地上物品显示星星			设置或返回: BOOL*

	AtcPropMaxPropCount,				// 最大可能属性数目
};
//------------------------------------------------------------------------
enum enArticleCommand
{
	AtcCmdNull = 0,						// 空命令

	AtcCmdSetProperty,					// 设置某些属性的命令
										// dwParam1: 属性类型
										// dwParam2: 设置值(参看地上物的属性定义)

	AtcCmdGetProperty,					// 得到某些属性的命令
										// dwParam1: 属性类型
										// dwParam2: 返回值(参看地上物的属性定义)

	AtcCmdChangePal,					// 改变对象的调色板
										// dwParam1: 是设置新的调色板(TRUE)还是恢复原来的调色板(FALSE)?
										// dwParam2: 调色板类型

	AtcCmdThrow,						// 投掷物品
										// dwParam1: Tile位置(POINT*)
										// dwParam2: 物品大小类别(小1,中2,大3)(int*)

	AtcCmdMaxCommandCount,				// 最大可能的命令数
};
//------------------------------------------------------------------------
// 这里加入一些地上物的定义，以后考虑要不要独立出去
enum enAniProp
{
	AniPropNull = 0,					// 空属性
	AniPropName,						// 动画的名字属性		设置或返回: char*
	AniPropNameColor,					// 动画的名字显示颜色	设置或返回: WPixel*
					
	AniPropMaxPropCount,				// 最大可能属性数目
};				
				
enum enAniCommand				
{
	AniCmdNull = 0,						// 空命令
						
	AniCmdSetProperty,					// 设置某些属性的命令
										// dwParam1: 属性类型
										// dwParam2: 设置值(参看动画对象的属性定义)
					
	AniCmdGetProperty,					// 得到某些属性的命令
										// dwParam1: 属性类型
										// dwParam2: 返回值(参看动画对象的属性定义)
					
	AniCmdMaxCommandCount,				// 最大可能的命令数
};
//------------------------------------------------------------------------
enum enTrapCommand
{
	TrpCmdNull = 0,						// 空命令
				
	TrpCmdTrigger,						// 陷阱触发
										// dwParam1: 陷阱触发时的创建参数(SSkillCreateParam*)
										// dwParam2: ?
				
	TrpCmdDestroy,						// 陷阱销毁
										// dwParam1: ?
										// dwParam2: ?
				
	TrpCmdSetTile,						// 设置陷阱的tile坐标
										// dwParam1: POINT*
										// dwParam1: ?

	TrpCmdMaxCommandCount,				// 最大可能的命令数
};
//------------------------------------------------------------------------
// 所有的陷阱类型
enum enTrapType
{
	TrpTypeHanBingXianJing		= 0,	// 寒冰陷阱
	TrpTypeBingKu				= 1,	// 冰窟
	TrpTypeCiRen				= 2,	// 刺刃
	TrpTypeHuoKeng				= 3,	// 火坑
	TrpTypeYinLei				= 4,	// 隐雷
	TrpTypeLeiGuang				= 5,	// 雷光
	TrpTypeFire					= 6,	// 火(像暗黑一团火在烧，上面的人会受伤)
	
	TrpTypeMaxCount				,		// 陷阱总数
};
//------------------------------------------------------------------------
struct ICreatureSink
{
	virtual void OnCreatureMoveToTileCenter(const POINTS& ptCurTile, const POINTS& ptNextTile) = 0;
};
//------------------------------------------------------------------------
