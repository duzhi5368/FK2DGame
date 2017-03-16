/**
*	created:		2012-7-3   0:57
*	filename: 		MsgFormat
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		服务器和客户端通信的消息结构
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "MsgKeyMainSelectActor.h"
#include "../FKGame/Common/GoodsPropertyDef.h"
#include "MsgKeyRoot.h"
#include "MsgKeyMainThing.h"
#include "../FKGame/Common/CreaturePropertyDef.h"
//------------------------------------------------------------------------
#pragma pack(1)	
//------------------------------------------------------------------------
#define SURECODE_DB_MAXLEN		40				// 没有MD5的识别码
#define SURECODE_MD5_MAXLEN		33				// MD5后的识别码，通过网络传送
//------------------------------------------------------------------------
// 描  述：服务器发给客户端的登陆态消息码
	struct SC_LOGIN_HEAD
	{
		WORD    m_wKeyRoot;
		WORD    m_wKeyMain;

		SC_LOGIN_HEAD(WORD wKeyMain)
		{
			m_wKeyRoot = CMD_ROOT_LOGIN;
			m_wKeyMain = wKeyMain;
		}

		SC_LOGIN_HEAD(void)
		{
			m_wKeyRoot = 0;
			m_wKeyMain = 0;
		}
	};	
//------------------------------------------------------------------------
// 描  述：客户端发给服务器的登陆态消息码
	struct CS_LOGIN_HEAD
	{
		WORD    m_wKeyRoot;
		WORD    m_wKeyMain;

		CS_LOGIN_HEAD(WORD wKeyMain)
		{
			m_wKeyRoot = CMD_ROOT_LOGIN;
			m_wKeyMain = wKeyMain;
		}

		CS_LOGIN_HEAD(void)
		{
			m_wKeyRoot = 0;
			m_wKeyMain = 0;
		}
	};
//------------------------------------------------------------------------
// 描  述：服务器发给客户端的选择人物态消息码
	struct SC_SELECTACTOR_HEAD
	{
		WORD    m_wKeyRoot;
		WORD    m_wKeyMain;

		SC_SELECTACTOR_HEAD(void)
		{
			m_wKeyRoot = CMD_ROOT_SELECTACTOR;
		}

		SC_SELECTACTOR_HEAD(WORD wKeyMain)
		{
			m_wKeyRoot = CMD_ROOT_SELECTACTOR;
			m_wKeyMain = wKeyMain;
		}
	};	

	struct SC_SELECTACTOR_ENTERCODE : public SC_SELECTACTOR_HEAD
	{
		DWORD	m_dwEnterCode;

		SC_SELECTACTOR_ENTERCODE(DWORD dwEnterCode)
		{
			m_wKeyRoot = CMD_ROOT_SELECTACTOR;
			m_wKeyMain = ROOTSELECTACTOR_SC_MAIN_ENTERCODE;
			m_dwEnterCode = dwEnterCode;
		}
	};
//------------------------------------------------------------------------
// 描  述：客户端发给服务器的选择人物态消息码
	struct CS_SELECTACTOR_HEAD
	{
		WORD    m_wKeyRoot;
		WORD    m_wKeyMain;

		CS_SELECTACTOR_HEAD(void)
		{
			m_wKeyRoot = CMD_ROOT_SELECTACTOR;
			m_wKeyMain = 0;

		}
	};

	struct CS_SELECTACTOR_SELECT_HEAD : public CS_SELECTACTOR_HEAD
	{
		// 帐号
		char	m_szUserName[32 + 1];
		// 密码
		char	m_szPassword[32 + 1];

		// 选择的人物ActorID
		DWORD	m_dwSelActorID;	
		
		// 网卡序列号
		char	szSerialNO[20];	

		// 登录场景服（主要用于跟切换场景服区分）
		long	lFirstLogin;

		// 识别码
		char	m_szSureCode[SURECODE_MD5_MAXLEN];

		CS_SELECTACTOR_SELECT_HEAD(void)
		{
			memset(this, 0, sizeof(CS_SELECTACTOR_SELECT_HEAD));
			m_wKeyRoot = CMD_ROOT_SELECTACTOR;
			m_wKeyMain = ROOTSELECTACTOR_CS_MAIN_SELECT;
			m_dwSelActorID = 0;
		}

		CS_SELECTACTOR_SELECT_HEAD(DWORD dwActorID)
		{
			memset(this, 0, sizeof(CS_SELECTACTOR_SELECT_HEAD));
			m_wKeyRoot = CMD_ROOT_SELECTACTOR;
			m_wKeyMain = ROOTSELECTACTOR_CS_MAIN_SELECT;
			m_dwSelActorID = dwActorID;
		}
	};

	struct CS_SELECTACTOR_MAC_HEAD : public CS_SELECTACTOR_HEAD
	{
		char	m_szMAC[512];

		CS_SELECTACTOR_MAC_HEAD(char szMac[512])
		{
			memset(this, 0, sizeof(CS_SELECTACTOR_MAC_HEAD));
			m_wKeyRoot = CMD_ROOT_SELECTACTOR;
			m_wKeyMain = ROOTSELECTACTOR_CS_MAIN_MAC;
			memcpy(m_szMAC, szMac, sizeof(m_szMAC));
		}
	};

	struct CS_SELECTACTOR_SELECT_HEAD_QQ : public CS_SELECTACTOR_HEAD
	{
		// QQ号
		char	m_szQQ[32 + 1];

		// QQData长度
		int		m_nQQDataLen;

		// QQData
		char	m_szQQData[256];

		// QQData MD5
		char	m_szQQDataMD5[32 + 1];

		// 选择的人物ActorID
		DWORD	m_dwSelActorID;	
		
		// 网卡序列号
		char	szSerialNO[20];	

		// 登录场景服（主要用于跟切换场景服区分）
		long	lFirstLogin;

		// 识别码
		char	m_szSureCode[SURECODE_MD5_MAXLEN];

		CS_SELECTACTOR_SELECT_HEAD_QQ(void)
		{
			memset(this, 0, sizeof(CS_SELECTACTOR_SELECT_HEAD_QQ));
			m_wKeyRoot = CMD_ROOT_SELECTACTOR;
			m_wKeyMain = ROOTSELECTACTOR_CS_MAIN_SELECT_QQ;
			m_dwSelActorID = 0;
		}

		CS_SELECTACTOR_SELECT_HEAD_QQ(DWORD dwActorID)
		{
			memset(this, 0, sizeof(CS_SELECTACTOR_SELECT_HEAD_QQ));
			m_wKeyRoot = CMD_ROOT_SELECTACTOR;
			m_wKeyMain = ROOTSELECTACTOR_CS_MAIN_SELECT_QQ;
			m_dwSelActorID = dwActorID;
		}
	};
//------------------------------------------------------------------------
// 描  述：服务器发给客户端的运行态消息码
	// 服务器发给客户端的消息头
	struct SC_THING_HEAD
	{
		WORD	m_wKeyRoot;
		WORD	m_wKeyMain;

		SC_THING_HEAD(void)
		{
			m_wKeyRoot = CMD_ROOT_THING;
			m_wKeyMain = 0;
		}
	};

	// 服务器通过ROOTTHING_SC_MAIN_PRIVATECONTEXT发给客户端的消息头
	struct SC_THING_PRIVATE_HEAD : public SC_THING_HEAD
	{	
		UID		m_uidThing;
		WORD	m_wThingClassID;
		WORD	m_wReserved;

		SC_THING_PRIVATE_HEAD(void)
		{
			m_uidThing = INVALID_UID;
			m_wKeyMain = ROOTTHING_SC_MAIN_PRIVATECONTEXT;
			m_wThingClassID = 0;
			m_wReserved = 0;
		}
	};

	// 服务器通过ROOTTHING_SC_MAIN_PUBLICCONTEXT发给客户端的消息头
	struct SC_THING_PUBLIC_HEAD : public SC_THING_HEAD
	{
		UID		m_uidThing;
		WORD	m_wThingClassID;
		WORD	m_wReserved;

		SC_THING_PUBLIC_HEAD(void)
		{
			m_uidThing = INVALID_UID;
			m_wKeyMain = ROOTTHING_SC_MAIN_PUBLICCONTEXT;
			m_wThingClassID = 0;
			m_wReserved = 0;
		}
	};

	// 服务器通过ROOTTHING_SC_MAIN_ACTION发给客户端的消息头
	struct SC_THING_ACTION_HEAD : public SC_THING_HEAD
	{
		UID		m_uidThing;
		WORD	m_wKeySub;

		SC_THING_ACTION_HEAD(void)
		{
			m_uidThing = INVALID_UID;
			m_wKeyMain = ROOTTHING_SC_MAIN_ACTION;
			m_wKeySub = 0;
		}
	};

	// 服务器通过ROOTTHING_SC_MAIN_CHANGEMAP发给客户端的消息头
	struct SC_THING_CHANAGEMAP_HEAD : public SC_THING_HEAD
	{
		DWORD	m_dwChangeMode;		// 切换模式.1 : 本进程切换, 2 ：跨进程切换
		int		m_nMapID;			// 地图ID
		POINT	m_ptWorld;			// 世界坐标			
	};

	// 服务器通过ROOTTHING_SC_MAIN_COUNTFEE发给客户端的消息头
	struct SC_THING_COUNTFEE_HEAD : public SC_THING_HEAD
	{
		DWORD	m_dwCountMode;		// 计费模式
		int		m_nQdot;			// Q点
		int		m_nSecond;			// 剩余时长
		char	m_szDesc[128];		// 描述
	};

	// 服务器通过ROOTTHING_SC_MAIN_GAMEWORLD发给客户端的消息头
	struct SC_THING_GAMEWORLD_HEAD : public SC_THING_HEAD
	{
		WORD	m_wKeySub;			// 子消息码

		SC_THING_GAMEWORLD_HEAD(WORD wKeySub)
		{
			m_wKeyRoot = CMD_ROOT_THING;
			m_wKeyMain = ROOTTHING_SC_MAIN_GAMEWORLD;
			m_wKeySub = wKeySub;
		}		
	};
//------------------------------------------------------------------------
// 描  述：客户端发给服务器的运行态消息码
	// 客户端发给服务器的通用消息头	
	struct CS_THING_HEAD
	{
		WORD	m_wKeyRoot;
		WORD	m_wKeyMain;

		CS_THING_HEAD(void)
		{
			m_wKeyRoot = CMD_ROOT_THING;
			m_wKeyMain = 0;
		}
	};

	// ROOTTHING_CS_MAIN_ACTION通用消息头
	struct CS_ACTION_HEAD : public CS_THING_HEAD
	{
		WORD    m_wKeySub;

		CS_ACTION_HEAD(WORD wKeySub)
		{
			m_wKeyRoot = CMD_ROOT_THING;
			m_wKeyMain = ROOTTHING_CS_MAIN_ACTION;
			m_wKeySub  = wKeySub;
		}

		CS_ACTION_HEAD(void)
		{
			m_wKeyRoot = CMD_ROOT_THING;
			m_wKeyMain = ROOTTHING_CS_MAIN_ACTION;
			m_wKeySub  = 0;
		}
	};

	// ROOTTHING_CS_MAIN_ACTION通用消息头, 需要检测时间戳的数据包用这个发送
	struct CS_ACTION_HEAD_EX : public CS_ACTION_HEAD
	{
		DWORD   m_dwTimeTick;

		CS_ACTION_HEAD_EX(WORD wKeySub, DWORD dwTimeTick)
		{
			m_wKeyRoot	= CMD_ROOT_THING;
			m_wKeyMain	= ROOTTHING_CS_MAIN_ACTION;
			m_wKeySub	= wKeySub;
			m_dwTimeTick= dwTimeTick;
		}

		CS_ACTION_HEAD_EX(void)
		{
			m_wKeyRoot	= CMD_ROOT_THING;
			m_wKeyMain	= ROOTTHING_CS_MAIN_ACTION;
			m_wKeySub	= 0;
			m_dwTimeTick= 0;
		}
	};
//------------------------------------------------------------------------
// 描  述：服务器发给客户端的伤害消息码
	struct SC_DAMAGE_REHEARSAL_HEAD
	{
		UID		uidSource;		// 攻击者ID
		WORD	wCheckId;		// 技能的较检ID，唯一确定
		BOOL	bDoAct;			// 是否要做被击动作
		WORD	wFlowType;		// 飘的类型,0 飘正常数字,1 飘暴击,2 飘抵抗,3 飘躲闪
		int		nDamHP;			// 减的HP值
	};
//------------------------------------------------------------------------
// 描  述：服务器发给客户端的治疗消息码
	struct SC_CURE_REHEARSAL_HEAD
	{
		UID		uidSource;		// 攻击者ID
		WORD	wCheckId;		// 技能的较检ID，唯一确定
		BOOL	bDoAct;			// 是否要做治疗动作
		int		nCureHP;		// 加的HP值
	};
//------------------------------------------------------------------------
// 描  述：使用药品
	struct CS_USEMEDICAMENT_CONTEXT
	{
		UID		uidMedicament;	// 药品
		UID		uidTarget;		// 目标对像
		POINTS	ptTarget;		// 目标tile
	};
//------------------------------------------------------------------------
// 描  述：丢弃物品
	struct CS_THROWGOODS_CONTEXT
	{
		UID		uidGoods;		// 物品id		
		POINTS	ptTile;			// 丢在地图上位置
		long	lContainerID;	// 容器id
	};
//------------------------------------------------------------------------
// 描  述：魔法复活
	struct SC_MAGICRELIVE_CONTEXT
	{
		UID		uidMagicer;		// 施法者
	};

	struct CS_MAGICRELIVE_CONTEXT
	{
		UID		uidMagicer;		// 施法者
		BOOL	bAgree;			// 是否同意
	};
//------------------------------------------------------------------------
// 描  述：怪物归属观察
	struct SC_HOLDPRIZE_OBSERVE_CONTEXT
	{
		UID		uidMonster;		// 观察何个怪
		UID		uidCurAward;	// 当前归属
		DWORD	dwHoldTeamID;	// 归属何个队ID
	};

	struct CS_HOLDPRIZE_OBSERVE_CONTEXT
	{
		BOOL	bObserveFlag;	// 是否观察
		UID		uidMonster;		// 观察何个怪
	};
//------------------------------------------------------------------------
// 描  述：更新复活时间
	struct SC_RELIVE_TIME_CONTEXT
	{
		int		nReliveTimeLeft;// 剩余复活时间
		int		nStaticMapID;	// 重生静态地图ID
		POINT	ptReliveLoc;	// 重生位置
	};
//------------------------------------------------------------------------
// 描  述：信息中心
	struct SC_MSG_CENTER_CONTEXT
	{
		DWORD	dwKey;			// 
		DWORD	dwType;			// 类型; MSG_CENTER_TYPE_TRADE....
		DWORD	dwDataLen;		// 数据长度
								// 数据
	};
//------------------------------------------------------------------------
// 描  述：观察角色位置
	struct SC_MSG_LOOK_ACTOR_DATA_CONTEXT
	{
		UID		uidActor;		// UID
		DWORD	dwStaticMapID;	// 所处静态地图ID
		DWORD	dwDynamicMapID;	// 所处动态地图ID
		SACTOR_PROPERTY_PUBLIC stPublicProp;	// 属性
	};

	struct CS_MSG_LOOK_ACTOR_DATA_CONTEXT
	{
		DWORD	dwActorDBID[64];// 观察的ActorDBID

		CS_MSG_LOOK_ACTOR_DATA_CONTEXT()
		{
			ZeroMemory(this, sizeof(CS_MSG_LOOK_ACTOR_DATA_CONTEXT));
		}
	};
//------------------------------------------------------------------------
// 描  述：玩家下线
	struct SC_MSG_PLAYERLOGOUT_CONTEXT
	{
		DWORD	dwStateFlag;	// 1 = 准备下线，2 = 正在存盘，3 = 存盘完成
		DWORD	dwTimeLeft;		// 剩余准备下线时间
	};
//------------------------------------------------------------------------
// 描  述：召唤
	struct SC_MSG_CONJURE_CONTEXT
	{
		UID		uidMaster;		// 主人
		UID		uidChief;		// 首领
		DWORD	dwPawnCount;	// 爪牙个数
		//.......				// 爪牙UID序列
	};

	struct CS_MSG_CONJURE_CONTEXT
	{
		UID		uidMaster;		// 何个生物
	};
//------------------------------------------------------------------------
// 描  述：上马
	struct SC_MSG_MOUNT_CONTEXT
	{
		int		nIndex;			// index
		int		nHorseID;		// 马匹id
	};
//------------------------------------------------------------------------
// 描  述：取得骑乘数据
	struct SC_MSG_GETRIDEDATA_CONTEXT
	{
		int		nCurMountIndex;	// 当前骑的何匹
		int		nHorseCount;	// 拥有马匹数量

		struct HORSEDATA
		{
			int	nIndex;			// index
			int nID;			// ID
		};
		//.....					// 马匹数据
	};

	struct CS_MSG_GETRIDEDATA_CONTEXT
	{
		UID		uidMaster;		// 何个人物
	};
//------------------------------------------------------------------------
// 描  述：快捷键
	struct CS_MSG_SHORTCUT_CONTEXT
	{
		char	szShortcutBuf[CREATURE_SHORTCUT_BUF_LEN];// 快捷键buf
	};
//------------------------------------------------------------------------
// 描  述：刷新外观
	struct SC_MSG_CLEAN_SHOW_CONTEXT
	{
		struct  SHOWRES
		{
			DWORD	dwGoodsID;	// 物品ID
			char	nSmeltLevel;// 精练等级
		};

		SHOWRES	stShowRes[EQUIPPOS_INDEX_MAX];
	};
//------------------------------------------------------------------------
// 描  述：观察玩家装备
	struct CS_MSG_LOOK_ACTOR_EQUIP_CONTEXT
	{
		UID		uidMaster;		// 观察何人
	};

	struct SC_MSG_LOOK_ACTOR_EQUIP_CONTEXT
	{
		UID		uidMaster;		// 被观察的人
		UID		uidEquip[EQUIPPOS_INDEX_MAX];
	};
//------------------------------------------------------------------------
#pragma pack()	
//------------------------------------------------------------------------