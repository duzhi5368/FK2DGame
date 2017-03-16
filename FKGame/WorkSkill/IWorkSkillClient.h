/**
*	created:		2012-6-29   20:35
*	filename: 		IWorkSkillClient
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../SceneObj/IThingPart.h"
//------------------------------------------------------------------------
#define MAX_JINGLIAN_MATERIAL_NUM		6			//最大精炼材料数目
#define MAX_JINGLIAN_RESULT				(1+8)		//最大精炼结果种类
//炼丹最大材料数
#define MAX_MATERIAL_NUM				9			//最大材料数
//------------------------------------------------------------------------
// 精炼配方关键字
struct SJingLianKey
{
	DWORD dwID;					//装备ID
	DWORD dwJingLianLevel;		//精炼等级
	
	SJingLianKey()
	{
		memset(this, 0, sizeof(SJingLianKey));
	}
	
	bool operator==(const SJingLianKey &Key) const
	{
		return dwID == Key.dwID && dwJingLianLevel == Key.dwJingLianLevel;
	}
};
//------------------------------------------------------------------------
// 精炼材料信息
struct SJingLianMaterial
{
	DWORD dwID;		//材料ID
	DWORD dwNum;	//材料数目
	SJingLianMaterial()
	{
		memset(this, 0, sizeof(SJingLianMaterial));
	}
};
//------------------------------------------------------------------------
// 精炼配方信息
struct SJingLianInfo
{
	DWORD dwMinLevel;		//精炼等级要求
	DWORD dwMaterialNum;	//精炼材料个数
	SJingLianMaterial Materials[MAX_JINGLIAN_MATERIAL_NUM];	
	DWORD dwToolsID;
	SJingLianInfo()
	{
		memset(this, 0, sizeof(SJingLianInfo));
	}
};
//------------------------------------------------------------------------
#define	MAX_HE_CHENG_PROBABILITY_NUM	(1+3)		//合成基本概率
struct SHeChengInfo
{
	DWORD dwID;		//装备ID
	DWORD dwType;	//分类
	DWORD dwLevel;	//装备等级
	DWORD dwHighID;	//高一级ID
	DWORD dwLowID;	//低一级ID
	DWORD dwProbabilitys[MAX_HE_CHENG_PROBABILITY_NUM];//概率
	DWORD dwMinLevel;		//要求最低技能等级
	DWORD dwTime;	//进度条时间
	DWORD dwToolsID;	//工具ID
	DWORD dwExp;		//经验值 
	SHeChengInfo()
	{
		memset(this, 0, sizeof(SHeChengInfo));
	}
};
//------------------------------------------------------------------------
//材料信息
struct SMaterialInfo
{
	DWORD dwMaterialID;		//材料ID
	DWORD dwMaterialNum;	//材料数目
};
//------------------------------------------------------------------------
//炼丹配方信息
struct SPeiFangInfo
{
	DWORD dwPeiFangID;		//配方ID
	DWORD dwLevel;			//需要技能等级
	SMaterialInfo MaterialInfos[MAX_MATERIAL_NUM];		//材料信息
	DWORD dwMaterialNum;		//材料个数
	DWORD dwProductID;		//产品ID
	DWORD dwProductNum;		//产品数量
	DWORD dwToolsID;		//工具ID
	DWORD dwTime;			//进度条时间
	SPeiFangInfo()
	{
		memset(this, 0, sizeof(SPeiFangInfo));
	}
};
//------------------------------------------------------------------------
// 工作技能通用信息
struct SWorkSkillBaseInfo 
{
	BYTE bSkillID;			//技能ID
	BYTE bSkillType;		//技能类型
	char szName[16];		//名字
	char szDes[256];		//描述
	char szTypeDes[256];	//类型描述
	DWORD dwPicID;			//图标ID
	SWorkSkillBaseInfo()
	{
		memset(this, 0, sizeof(SWorkSkillBaseInfo));
	}
};
//------------------------------------------------------------------------
// 工作技能称号信息
struct SWorkSkillTitleInfo
{
	DWORD dwID;				//技能ID
	char szName[16];		//技能名字
	DWORD dwLevel;			//技能级别
	char szTitle[16];		//技能称号
	DWORD dwMaxExp;			//技能最大经验	
	long lNeedActorLevel;	//需要玩家级别
	long lNeedActorMoney;	//需要玩家金钱
	SWorkSkillTitleInfo()
	{
		memset(this, 0, sizeof(SWorkSkillTitleInfo));
	}
};
//------------------------------------------------------------------------
// 合成信息第二期
struct SHeChengV2Info{
	enum
	{
		_MAX_MATERIAL_NUM = 18,
		_MAX_EFFECT_NUM = 7,
		_MAX_SLOT_NUM = 3,
	};
	DWORD GoodsID;     //主装备ID
	char szName[32];   //主装备名
	DWORD MaterialGoodsID[_MAX_MATERIAL_NUM];  //18个材料物品的ID
	DWORD MaterialNeed[_MAX_MATERIAL_NUM];     //18个材料物品的所需的数量
	
	DWORD SuccessProbability;  //成功率
	DWORD Suit1_GoodsID;      //套装1 ID
	DWORD Suit1_Probability;  //套装1 概率
	DWORD Suit2_GoodsID;      //套装2 ID
	DWORD Suit2_Probability;  //套装2 概率
	DWORD NeedSkillPoint;     //使用所需的技能点 , 这个叫LUA配
	DWORD LuckyStoneProb;     //幸运石增加成功的概率
	DWORD WhiteStoneProb;     //白色石增加成功的概率
	DWORD BlackStoneProb;     //黑色石增加成功的概率
	DWORD EffectProbability[_MAX_EFFECT_NUM];  // 7个属性的机率
	DWORD SlotProbability[_MAX_SLOT_NUM];  // 7个属性的机率
	
	SHeChengV2Info()
	{
		memset(this, 0, sizeof(SHeChengV2Info));
	}
};
//------------------------------------------------------------------------
// 熔炼第二期
struct SFusionV2Info{
	enum
	{
		MAX_FUSIONV2_MATERIAL_NUM = 18,
	};
	DWORD MainGoodsID;     //主物品ID
	char szName[32];   //主物品名
	DWORD EquipRank;   //副装备档次
	DWORD InheritedProbability;   //属性继承的机率
	
	DWORD MaterialGoodsID[MAX_FUSIONV2_MATERIAL_NUM];  //18个材料物品的ID
	DWORD MaterialNeed[MAX_FUSIONV2_MATERIAL_NUM];     //18个材料物品的所需的数量
	
	DWORD SuccessProbability;  //成功率
	DWORD ProductGoodsID;      //成品 ID
	DWORD ProductNum;          //成品数量
	
	SFusionV2Info()
	{
		memset(this, 0, sizeof(SFusionV2Info));
	}
};
//------------------------------------------------------------------------
// 制药二期
struct SPharmacyV2Info{
	enum
	{
		MAX_PHARMACYV2_PRODUCE_NUM = 4,    //最多有4种产品
		MAX_PHARMACYV2_MATERIAL_NUM = 18,  //最多有18种材料
	};
	DWORD PrescriptionID;     //配方ID
	DWORD ProductGoodsID[MAX_PHARMACYV2_PRODUCE_NUM];      //成品 ID
	DWORD ProductNum[MAX_PHARMACYV2_PRODUCE_NUM];          //成品数量
	DWORD ProductZone[MAX_PHARMACYV2_PRODUCE_NUM];         //成品产出的区间数
	DWORD MaterialGoodsID[MAX_PHARMACYV2_MATERIAL_NUM];  //18个材料物品的ID
	DWORD MaterialNeed[MAX_PHARMACYV2_MATERIAL_NUM];     //18个材料物品的所需的数量
	
	SPharmacyV2Info()
	{
		memset(this, 0, sizeof(SPharmacyV2Info));
	}
};
//------------------------------------------------------------------------
//提炼2期
typedef SPharmacyV2Info SEpurateV2Info;
//------------------------------------------------------------------------
//镶嵌2期
struct SInlayInfoV2
{
	static enum
	{	
		MAX_INLAYEFFECT_NUM = 3,    //宝石效果的数量
			MAX_EQUIP_TYPES = 16,  //最大装备类型数量
	};
	
	DWORD dwStoneID;			//宝石ID
	char  szName[33];           //宝石名称
	DWORD dwMinLevel;	//要求技能级别
	DWORD dwEffects[MAX_INLAYEFFECT_NUM];		//附加属性
	DWORD dwEffectProbabilitys[MAX_INLAYEFFECT_NUM];//概率
	DWORD dwEquipTypes[MAX_EQUIP_TYPES];			//类型
	DWORD dwTime;		//进度条时间
	SInlayInfoV2()
	{
		memset(this, 0, sizeof(SInlayInfoV2));
	}
	
};
//------------------------------------------------------------------------
//精炼2期
struct SRefineV2Info{
	enum
	{		
		MAX_REFINEV2_MATERIAL_NUM = 6,  //最多有18种材料
			RESULT_ZERO = 0,  //精炼为0  的区间IDX
			RESULT_DECREASE = 1,  //精炼 -1  的区间IDX
			RESULT_NOCHANGE = 2,  //精炼不变  的区间IDX
			RESULT_INCREASE = 3,  //精炼为 +1  的区间IDX
	};
	DWORD dwEquipID;     //装备ID
	char  szName[32 + 1]; //名移
	DWORD dwSkillLevel;   //精炼技能的等级
	DWORD dwSmelt;        // 武器的精炼度
	DWORD dwMaterialID[MAX_REFINEV2_MATERIAL_NUM];
	DWORD dwMaterialNum[MAX_REFINEV2_MATERIAL_NUM];
	DWORD dwProbability[4]; //成功率区间
	DWORD dwTime;   //时间
	SRefineV2Info()
	{
		memset(this, 0, sizeof(SRefineV2Info));
	}
	
	static DWORD MAKEKEY(DWORD dwEquipID, DWORD nSmelt)
	{
		return dwEquipID * 100 + nSmelt;
	}
};
//------------------------------------------------------------------------
struct IActor;
struct IContainer;
struct IWorkSkillRul;
//------------------------------------------------------------------------
struct IWorkSkill
{
	virtual void Release() = NULL;
	// 判断是否可以使用该技能
	virtual BOOL CanUse(LPCSTR pContext, int nLen) = NULL;
	// 使用技能
	virtual void Use(LPCSTR pContext, int nLen) = NULL;
	virtual BYTE GetLevel() = NULL;
	virtual BYTE GetID() = NULL;
	// 获取经验值
	virtual WORD GetExp() = NULL;	
	// 获取当前最大经验值
	virtual DWORD GetMaxExp() = NULL;
	// 获取称号
	virtual LPCSTR GetTitle() = NULL;
	// 获取容器
	virtual IContainer* GetContainer() = NULL;
	// 处理结果
	virtual void DoResult(LPCSTR buf, int nLen) = NULL;
	// 更新
	virtual BOOL OnUpdate(LPCSTR buf, int nLen) = NULL;
};
//------------------------------------------------------------------------
struct IWorkSkillMgr : public IThingPart
{
	// 根据ID 获取技能
	virtual IWorkSkill* GetSkill(BYTE bWorkSkillID) = NULL;
	virtual void RequestShowWorkSkill(UID uidTarget) = NULL;
	virtual void RequestWorkSkillByID(UID uidTarget, BYTE bSkillID) = NULL;
	virtual void RequestOK(UID uidTarget, BYTE bSkillID, DWORD dwMoney, LPCSTR buf = NULL, int nLen = NULL) = NULL;
	virtual void AcceptWorkSkill(UID uidRequestor, BYTE bSkillID, LPCSTR buf = NULL, int nLen = NULL) = NULL;
	virtual void CloseWorkSkill(BYTE bSkillID) = NULL;
	//遗忘生活技能
	virtual void DisrememberSkill(BYTE bSkillID) = NULL;
};
//------------------------------------------------------------------------
struct IWorkSkillSystem
{
	virtual void Release() = NULL;
	// 创建工作技能管理器
	virtual BOOL CreateWorkSkillMgr(IWorkSkillMgr **ppWorkSkillMgr) = NULL;
	// 根据生活技能ID获取基本信息(SWorkSkillBaseInfo)
	virtual SWorkSkillBaseInfo* GetWorkSkillBaseInfo(BYTE bWorkSkillID) = NULL;

	// 获取炼丹配方个数
	virtual int GetLianDanPeifangNum() = NULL;
	// 根据配方序号获取炼丹产品ID (物品ID)
	virtual DWORD GetLianDanProductID(int nIndex) = NULL;
	// 根据产品ID 获取配方信息	
	virtual SPeiFangInfo* GetPeiFangInfo(DWORD dwProductID) = NULL;

	// 根据物品ID和精炼等级获取精炼信息
	virtual SJingLianInfo* GetJingLianInfo(DWORD dwGoodsID, DWORD dwLevel) = NULL;

	virtual SWorkSkillTitleInfo* GetTitleInfo(DWORD dwSkillID, DWORD dwLevel) = NULL;
	virtual IWorkSkillRul* GetRul(BYTE bSkillID) = NULL;
	virtual	SHeChengInfo* GetHeChengInfo(DWORD dwID) = NULL;

	//第二期工作技能
	virtual	SHeChengV2Info* GetHeChengV2Info(DWORD dwID) = NULL;
	virtual SFusionV2Info* GetFusionV2Info(DWORD dwMainGoodsID) = NULL;
	virtual SPharmacyV2Info* GetPharmacyInfo(DWORD dwPrescriptionID) = NULL;
	virtual SRefineV2Info* GetRefineV2Info(DWORD dwEquipID, DWORD dwSmelt) = NULL;
	//传入一个缓冲区和大小, 返回值表示共有多少个产品,产品配方ID存在pBuff;
	virtual DWORD GetPharmacyV2PrescriptionList(DWORD *pBuff, int nLen) = NULL;

	//传入一个缓冲区和大小, 返回值表示共有多少个产品,产品配方ID存在pBuff;
	virtual DWORD GetEpurateV2PrescriptionList(DWORD *pBuff, int nLen) = NULL;
	virtual SEpurateV2Info* GetEpurateV2Info(DWORD dwPrescriptionID) = NULL;

};
//------------------------------------------------------------------------
struct IClientGlobal;
//------------------------------------------------------------------------
class CWorkSkillSystemHelper
{
private:
	typedef BOOL (*ProcCreateWorkSkillSystemObject)(
		IWorkSkillSystem **ppStatusSys, IClientGlobal *pClientGlobal, const char *pFileName );
	
public:
	CWorkSkillSystemHelper()
	{
		m_hDll = NULL;
		m_pWorkSkillSystem = NULL;
		m_szErrMsg[0] = 0;
	}
	~CWorkSkillSystemHelper()
	{
		Close();
		if (m_hDll != NULL)//注意，如果是lib方式，由于m_hDll总是NULL,所以下面的::FreeLibary()不会被调用
		{
			::FreeLibrary(m_hDll);
			m_hDll=NULL;
		}
	}	

	BOOL Create(IClientGlobal *pClientGlobal)
	{
		Close();
		try
		{
			if (m_hDll == NULL)
			{
				//第一次调用时未加载dll,下面进行加载dll
#ifdef _DEBUG
				m_hDll = ::LoadLibrary("FKWorkSkillClient_D.dll");										
				if (m_hDll == NULL)
					throw "无法加载 FKWorkSkillClient_D.dll";
#else
				m_hDll = ::LoadLibrary("FKWorkSkillClient.dll");										
				if (m_hDll == NULL)
					throw "无法加载 FKWorkSkillClient.dll";
#endif
			}
			//下面获得dll的输出函数，即接口对象的创建函数
			ProcCreateWorkSkillSystemObject proc = NULL;
			proc=(ProcCreateWorkSkillSystemObject)::GetProcAddress(m_hDll,
				"CreateWorkSkillSystemObject");
			if (proc == NULL)
				throw "Can't GetProcAddress('CreateWorkSkillSystemObject')";
			if (!proc(&m_pWorkSkillSystem, pClientGlobal, NULL))
				throw "CreateWorkSkillSystemObject() error!";
			
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
		if (m_pWorkSkillSystem != NULL)
		{
			m_pWorkSkillSystem->Release();
			m_pWorkSkillSystem = NULL;			
		}
	}
	//重载->,将helper对象的引用直接转为接口的引用
	IWorkSkillSystem* operator ->()
	{
		return m_pWorkSkillSystem;
	}
	IWorkSkillSystem* GetInterface()
	{
		return m_pWorkSkillSystem;
	}
	//判断接口指针是否有效
	BOOL IsValid()
	{
		return (m_pWorkSkillSystem != NULL);
	}
	char* GetErrMsg()
	{
		return m_szErrMsg;
	}
private:
	IWorkSkillSystem *m_pWorkSkillSystem;					//接口对象
	HINSTANCE m_hDll;									//动态链接库加载与否标志
	char m_szErrMsg[128];								//错误信息
};
//------------------------------------------------------------------------