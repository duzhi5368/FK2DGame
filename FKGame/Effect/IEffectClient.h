/**
*	created:		2012-6-29   21:40
*	filename: 		IEffectClient
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
enum
{
	en_STRENGTH_EFFECT = 1,			//力量效果
	en_CONSTIT_EFFECT = 2,			//体质效果
	en_MENTAL_EFFECT = 3,			//智力效果
	en_WISDOM_EFFECT = 4,			//智慧效果
	en_CELERITY_EFFECT = 5,			//敏捷效果
	
	en_P_A_EFFECT = 6,				//物理攻击点数
	en_P_A_MIN_EFFECT = 7,			//物理最小攻击点数
	en_P_A_MAX_EFFECT = 8,			//物理最大攻击点数
	en_P_A_PER_EFFECT = 9,			//攻击百分比
	en_P_SUDDENLY_EFFECT = 10,		//物理攻击爆击率
	en_M_SUDDENLY_EFFECT = 11,		//法术攻击暴击率
	
	en_F_A_EFFECT = 12,				//火法术攻击力
	en_I_A_EFFECT = 13,				//冰法术攻击力
	en_E_A_EFFECT = 14,				//电法术攻击力	
	en_N_A_EFFECT = 15,				//暗法术攻击力
	
	en_M_A_EFFECT = 16,				//法术攻击力
	en_M_A_PER_EFFECT = 17,			//法术攻击力百分比
	
	en_CURE_EFFECT = 18,			//治疗效果增加
	en_CURE_PER_EFFECT = 19,		//治疗效果增加百分比
	
	en_F_D_EFFECT = 20,				//火法术防御力
	en_I_D_EFFECT = 21,				//冰法术防御力
	en_E_D_EFFECT = 22,				//电法术防御力
	en_N_D_EFFECT = 23,				//暗法术防御力
	
	en_P_D_EFFECT = 24,				//物理防御力
	en_P_D_PER_EFFECT = 25,			//物理防御力百分比
	
	en_M_D_EFFECT = 26,				//法术防御力
	en_M_D_PER_EFFECT = 27,			//法术防御力百分比
	
	en_P_DAMAGE_EFFECT = 28,		//物理伤害吸收点数	
	en_M_DAMAGE_EFFECT = 29,		//法术伤害吸收点数	
	en_ALL_DAMAGE_EFFECT = 30,		//所有伤害吸收点数	
	
	en_ECHO_DAMAGE_EFFECT = 31,		//点数增加反弹伤害
	
//	en_VERACITY_PER_EFFECT = 32,	//命中率百分比
//	en_HEDGE_PER_EFFECT = 33,		//躲闪率百分比
	
	en_PERSIST_HP_EFFECT = 34,		//持续生命值效果
	en_PERSIST_MP_EFFECT = 35,		//持续法力值效果
	en_PERSIST_HP_PER_EFFECT = 36,	//持续恢复百分比HP
	en_PERSIST_MP_PER_EFFECT = 37,	//持续恢复百分比MP
	
	en_KILL_HP_EFFECT = 38,			//每杀死一个生物概率恢复HP
	en_KILL_MP_EFFECT = 39,			//每杀死一个生物概率恢复MP
	en_DAMAGE_HP_EFFECT = 40,		//每次伤害恢复HP
	en_DAMAGE_MP_EFFECT = 41,		//每次伤害恢复MP	
	
	en_HPMAX_EFFECT = 42,			//最大生命值
	en_MPMAX_EFFECT = 43,			//最大法术值
	en_HPMAX_PER_EFFECT = 44,		//最大生命值百分比
	en_MPMAX_PER_EFFECT = 45,		//最大法术值百分比
	
	en_MOVESPEEDLEVEL_EFFECT = 52,	//加速
	
	//////////////////////////////////////////////////////////////////////////
	//53-63不再使用
	en_INVINCIBILITY1_EFFECT = 53,	//无敌1，不受任何伤害和状态、可以移动，本身可以发起攻击
	en_INVINCIBILITY2_EFFECT = 54,  //无敌2，不受任何伤害和状态、可以移动，本身不可以发起攻击
	en_INVINCIBILITY3_EFFECT = 55,	//无敌3，不受任何伤害和状态、不可以移动，本身不可以发起攻击
	
	en_BLACKOUT_EFFECT = 56,		//眩晕,无法移动、无法攻击、无法使用物品。受到攻击不会醒
	en_LETHARGY_EFFECT = 57,		//昏睡,无法移动、无法攻击、无法使用物品。受到攻击会醒
	en_FIXATION_EFFECT = 58,		//定身，无法移动、可以攻击、可以使用物品。受到攻击不变
	en_FROST_EFFECT = 59,			//冰冻，无法移动、无法攻击、无法使用物品。受到攻击会醒
	
	en_AFFRIGHT_EFFECT = 60,		//惊吓,往一个方向跑，受到伤害不会醒
	en_SWIVET_EFFECT = 61,			//狂乱,在一定范围内乱跑，受到伤害不会醒
	en_SOMNAMBULATE_EFFECT = 62,	//梦游,往一个方向跑，受到伤害会醒
	en_PUZZLE_EFFECT = 63,			//迷惑,在一定范围内乱跑，受到伤害会醒
	//////////////////////////////////////////////////////////////////////////
	
	
	en_VOTE_MOVE_EFFECT = 100,		//玩家无法移动效果
	en_VOTE_ATTACK_EFFECT = 101,	//无法攻击效果
	en_VOTE_USE_GOODS_EFFECT = 102,	//无法使用物品效果
	en_VOTE_DAMAGE_EFFECT = 103,	//不受任何伤害效果
	
	en_P_PERSIST_DAMAGE_EFFECT = 150,		//持续物理伤害
	en_F_PERSIST_DAMAGE_EFFECT = 151,		//持续火伤害
	en_I_PERSIST_DAMAGE_EFFECT = 152,		//持续冰伤害
	en_E_PERSIST_DAMAGE_EFFECT = 153,		//持续电伤害
	en_N_PERSIST_DAMAGE_EFFECT = 154,		//持续暗伤害
	
	en_FORBID_MAGIC_EFFECT = 200,			//禁魔
	en_CHANGE_SKILL_SHAPE_EFFECT = 201,		//改变形态
	en_Add_Enmity_EFFECT = 202,				//增加仇恨值，只对怪物有效
	en_SWIVET2_EFFECT = 203,					//狂乱效果，乱跑	
	en_SKILL_BREAK_EFFECT = 204,					//打断施法的效果
	
	en_DamageToMP_EFFECT = 220,				//伤害转移到法力效果
	en_CRAZYDAMAGE_EFFECT = 221,			//激活暴击效果
	en_ALL_DAMAGE_PER_EFFECT = 222,			//所有伤害吸收百分比
	en_EXP_EFFECT = 223,					//经验系数
	en_ACTIVESKILLL_EFFECT = 224,			//激活技能效果
	en_DAMAGETOPAWN_EFFECT = 225,			//伤害转移到宠物
	en_CHANGESHAP = 226,					//变身效果
};
//------------------------------------------------------------------------
struct IEffect
{
	// 描  述：根据参数创建	
	// 参  数：[输入] long *pParams  - 参数列表
	// 参  数：[输入] long lLen  - 长度
	// 返回值：BOOL - 操作成功则返回TRUE，否则返回FASLE
	virtual BOOL Create(const long *pParams, long lLen) = NULL;
	// 描  述：效果开始启动
	// 参  数：[输入] ICreature *pMaster  - 本效果作用生物
	// 参  数：[输入] UID uidCreator - 产生者UID
	// 返回值：BOOL - 操作成功则返回TRUE，否则返回FASLE
	virtual BOOL Start(ICreature *pMaster, UID uidCreator) = NULL;
	//效果结束
	virtual BOOL End() = NULL;
	//克隆自身
	virtual IEffect* Clone() = NULL;
	virtual LPCSTR GetDescription() = NULL;
	virtual void SetDescription(LPCSTR szDes) = NULL;
	virtual void Release() = NULL;
};
//------------------------------------------------------------------------
struct IEffectFactory
{
	virtual void Release() = NULL;	
	// 创建效果
	virtual BOOL CreateEffect(long lID, IEffect **ppEffect) = NULL;
	// 根据ID获取描述信息
	virtual LPCSTR GetDescription(long lID) = NULL;
};
//------------------------------------------------------------------------
struct IClientGlobal;
//------------------------------------------------------------------------
class CEffectFactoryHelper
{
private:
	typedef BOOL (*ProcCreateEffectFactory)(IEffectFactory **ppEffectFactory, 
		IClientGlobal *pClientGlobal, const char *pFileName);	
	
public:
	CEffectFactoryHelper()
	{
		m_hDll = NULL;
		m_pEffectFactory = NULL;
		m_szErrMsg[0] = 0;
	}
	~CEffectFactoryHelper()
	{
		Close();
		if (m_hDll != NULL)//注意，如果是lib方式，由于m_hDll总是NULL,所以下面的::FreeLibary()不会被调用
		{
			::FreeLibrary(m_hDll);
			m_hDll=NULL;
		}
	}	
	
	// 描  述：创建效果服务器
	// 返回值：BOOL - 创建成功则返回TRUE,否则返回FALSE
	BOOL Create(IClientGlobal *pClientGlobal, const char *pFileName = NULL)
	{
		Close();
		try
		{
			if (m_hDll == NULL)
			{
				//第一次调用时未加载dll,下面进行加载dll
#ifdef _DEBUG
				m_hDll = ::LoadLibrary("FKEffectClient_D.dll");
				if (m_hDll == NULL)
					throw "无法加载 FKEffectClient_D.dll";
#else
				m_hDll = ::LoadLibrary("FKEffectClient.dll");
				if (m_hDll == NULL)
					throw "无法加载 FKEffectClient.dll";
#endif
			}
			//下面获得dll的输出函数，即接口对象的创建函数
			ProcCreateEffectFactory proc = NULL;
			proc=(ProcCreateEffectFactory)::GetProcAddress(m_hDll,
				"CreateEffectFactory");
			if (proc == NULL)
				throw "Can't GetProcAddress('CreateEffectFactory')";
			if (!proc(&m_pEffectFactory, pClientGlobal, pFileName))
				throw "CreateEffectFactory() error!";
			
			return TRUE;//接口对象创建成功
		}
		catch (LPCSTR szMsg) 
		{
			lstrcpyn(m_szErrMsg, szMsg, sizeof(m_szErrMsg));
			return FALSE;
		}
		catch(...)
		{
			lstrcpyn(m_szErrMsg, "Unknown Error!", sizeof(m_szErrMsg));
			return FALSE;
		}
	}
	void Close()//销毁接口对象
	{
		if (m_pEffectFactory != NULL)
		{
			m_pEffectFactory->Release();
			m_pEffectFactory = NULL;			
		}
	}
	//重载->,将helper对象的引用直接转为接口的引用
	IEffectFactory* operator ->()
	{
		return m_pEffectFactory;
	}
	IEffectFactory* GetEffectFactory()
	{
		return m_pEffectFactory;
	}
	//判断接口指针是否有效
	BOOL IsValid()
	{
		return (m_pEffectFactory != NULL);
	}
	char* GetErrMsg()
	{
		return m_szErrMsg;
	}
private:
	IEffectFactory *m_pEffectFactory;					//接口对象
	HINSTANCE m_hDll;									//动态链接库加载与否标志
	char m_szErrMsg[128];								//错误信息
};
//------------------------------------------------------------------------