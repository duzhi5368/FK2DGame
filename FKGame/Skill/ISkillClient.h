/**
*	created:		2012-6-30   1:12
*	filename: 		ISkillClient
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		技能工厂模块接口
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define INVALID_SKILLID				0xFFFF	// 无效技能ID
#define INVALID_SKILLLV 			0xFFFF	// 无效等级ID
//------------------------------------------------------------------------
#define MAX_SKILL_DESC_LEN			512		// 技能说明最大长度
//------------------------------------------------------------------------
#include "../SceneObj/IThingPart.h"
//------------------------------------------------------------------------
// 技能列表里的数据结构
struct SSkillData
{
	WORD	wSkillID;			// 技能ID
	WORD	wLevel;				// 技能等级
};
//------------------------------------------------------------------------
// 使用技能的数据包
struct SUseSkillContext
{
	SSkillData	SkillData;		// 技能ID与等级
	UID			uidTarget;		// 释放技能时鼠标点到的位置上的对象
	POINTS		ptTargetTile;	// 释放技能时鼠标点到的位置
	WORD		wCheckId;		// 技能的较检ID，唯一确定
};
//------------------------------------------------------------------------
// 禁止使用技能的条件类型
enum enForbidSkillType
{
	enFORBID_SKILL_TYPE_ALL = 0,	// 禁止所有技能包括普通攻击施放
	enFORBID_SKILL_TYPE_MAGIC,		// 禁止所有技能施放不包括普通攻击

	enFORBID_SKILL_TYPE_MAX_COUN,	// 最大可能数目
};
//------------------------------------------------------------------------
// 技能信息
enum enSkillInfo
{
	// 基本数值
	enSKILL_INFO_GUNSHOT = 0,			// 攻击距离(单位是Tile,默认为0无距离限制)
	enSKILL_INFO_TESTBLOCK,				// 判断阻挡方式（飞行、行走、蔓延阻挡）
	enSKILL_INFO_CONSUMEMP,				// 此技能消耗的MP(默认为0)
	
	// 学习技能相关
	enSKILL_INFO_STUDYNEED_LEVEL,		// 学习技能－等级需求
	enSKILL_INFO_STUDYNEED_EXP,			// 学习技能－经验需求
	enSKILL_INFO_STUDYNEED_MONEY,		// 学习技能－金钱需求

	// 时间相关
	enSKILL_INFO_COLDTIMEID,			// 冷却时间ID
	enSKILL_INFO_TIMEPREP,				// 蓄气时间
	enSKILL_INFO_TIMEATTACK,			// 攻击时间（有默认值，待定，可以理解为僵直时间，这是正在做攻击动作的时候，无法做其他动作）
	enSKILL_INFO_TIMECTRL,				// 技能引导时间，在该时间内玩家必须引导，并可以停止施法（默认为0）
	
	// 物品需求
	enSKILL_INFO_NEEDGOODS,				// 是否需要物品，0表示不需要，1表示要消耗掉，2表示只要存在就行了
	enSKILL_INFO_GOODSID,				// 物品ID
	enSKILL_INFO_GOODSNUM,				// 物品数目
	
	// 造成效果相关
	enSKILL_INFO_HAVEDAM,				// 是否有伤害
	enSKILL_INFO_DAMTYPE,				// 伤害属性类型
	
	// 动画相关
	enSKILL_INFO_VIEWLEVEL,				// 技能对应的动画等级

	enSKILL_INFO_MAX_COUNT,				// 最大可能信息数目
};
//------------------------------------------------------------------------
// 判断技能能否使用的方式
enum enUseSkillType
{
	enATTACK_BEGIN = 0,			// 收到攻击命令
	enATTACK_PRE_OVER,			// 蓄气结束时的技能能否使用判断
};
//------------------------------------------------------------------------
// 技能类型
enum enSkillType
{
	enSKILL_TYPE_INITIATIVE = 0,// 主动技能
	enSKILL_TYPE_PASSIVE,		// 被动技能

	enSKILL_TYPE_ERROR,			// 错误类型
};
//------------------------------------------------------------------------
// 技能攻击类型
enum enSkillAttackType
{
	enSKILL_ATTACK_TYPE_TARGER_ENEMY = 0,	// 要选择对象，只能选择(非死亡)敌人
	enSKILL_ATTACK_TYPE_TARGER_FRIEND,		// 要选择对象，只能选择(非死亡)友方
	enSKILL_ATTACK_TYPE_TARGER_CORPSE,		// 要选择对象，只能选择尸体
	enSKILL_ATTACK_TYPE_TARGER_SELFBABY,	// 要选择对象，只能选择自己的宠物
	enSKILL_ATTACK_TYPE_TARGER_ALL,			// 要选择对象，所有(非死亡)生物都可以

	enSKILL_ATTACK_TYPE_GROUND,				// 要选择地面

	enSKILL_ATTACK_TYPE_NO_SELECT,			// 不用选择对象，只要用了就自动施放

	enSKILL_ATTACK_TYPE_ERROR,				// 错误类型
};
//------------------------------------------------------------------------
// 技能使用失败的返回值
enum enCanUseSkillInfo
{
	enUSE_SKILL_SUCCEED = 0,				// 可以使用该技能

	enUSE_SKILL_FAIL_TOO_FAR,				// 距离太远
	enUSE_SKILL_FAIL_NO_MAGIC,				// MP不够
	enUSE_SKILL_FAIL_NO_TARGER,				// 没有目标或目标消失
	enUSE_SKILL_FAIL_WEAPEN_ERROR,			// 武器不对
	enUSE_SKILL_FAIL_GOODS_ERROR,			// 物品需求不够
	enUSE_SKILL_FAIL_FORBID,				// 被禁魔等禁止施放了
	enUSE_SKILL_FAIL_STATUS_NEED,			// 状态需求不对
	enUSE_SKILL_FAIL_SKILL_SHAPE,			// 当前形态不对
	enUSE_SKILL_FAIL_HAVE_BLOCK,			// 中间有障碍
	enUSE_SKILL_FAIL_IN_COLD_TIME,			// 技能还未冷却完毕
	enUSE_SKILL_FAIL_LEVEL_NO_ENOUGH,		// 技能等级超过玩家所学会的技能等级

	enUSE_SKILL_FAIL_STATE_IN_PREPARE,		// 在蓄气中，不能施放其他技能
	enUSE_SKILL_FAIL_STATE_IN_ATTACK,		// 在攻击状态中，不能施放其他技能
	enUSE_SKILL_FAIL_STATE_IN_CTRL,			// 在持续施法状态中，不能施放其他技能

	enUSE_SKILL_FAIL_SKILL_TYPE_ERROR,		// 技能使用错误（比如使用了被动技能）
	enUSE_SKILL_FAIL_ATTACK_TYPE_ERROR,		// 技能使用错误（比如对敌方使用有益技能，这个以后要详细分）

	enUSE_SKILL_FAIL_OTHER,					// 其他异常,这种异常是不该发生的却发生了，比如技能等级超过有效值
};
//------------------------------------------------------------------------
// 观察者关心的技能使用信息情况
enum enObserverUseSkillInfo
{
	enOBSERVER_USE_SKILL_SUCCEED = 0,				// 可以使用该技能

	enOBSERVER_USE_SKILL_FAIL_NO_MAGIC,				// MP不够
	enOBSERVER_USE_SKILL_FAIL_WEAPEN_ERROR,			// 武器不对
	enOBSERVER_USE_SKILL_FAIL_FORBID,				// 被禁魔等禁止施放了
	enOBSERVER_USE_SKILL_FAIL_STATUS_NEED,			// 状态需求不对
	enOBSERVER_USE_SKILL_FAIL_SKILL_SHAPE,			// 当前形态不对

	enOBSERVER_USE_SKILL_FAIL_OTHER,				// 其他异常
};
//------------------------------------------------------------------------
// 玩家所处的技能阶段状态
enum enSkillStatus
{
	enSKILL_STATUS_NORMAL,					// 非施放技能状态
	enSKILL_STATUS_PREPARE,					// 蓄气状态
	enSKILL_STATUS_ATTACK_ONCE,				// 施放技能状态
	enSKILL_STATUS_ATTACK_CTRL,				// 持续施放技能状态
};
//------------------------------------------------------------------------
// 判断能否学习技能返回的结果
enum enStudySkillResult
{
	enSTUDY_SKILL_RESULT_SUCCEED,			// 可以学习升级该技能
		
	enSTUDY_SKILL_RESULT_FAIL_MONEY,		// 不能升级该技能、钞票不够
	enSTUDY_SKILL_RESULT_FAIL_EXP,			// 不能升级该技能、经验不够
	enSTUDY_SKILL_RESULT_FAIL_OTHER,		// 不能升级该技能、其他原因，待加
};
//------------------------------------------------------------------------
// 激活技能列表
enum enSkillActive
{
	enSKILL_ACTIVE_NONE = 0,				// 不需激活
	enSKILL_ACTIVE_FUCHOU,					// 激活复仇（逆血搏杀）技能
	
	enSKILL_ACTIVE_MAX,						// 最大激活类型
};
//------------------------------------------------------------------------
// 技能观察者
class ISkillObserver
{
public:
	// 当技能发生改变时调用
	// nInfo	：当前该技能可否使用的信息
	// IconID	：该技能显示的图标
	// UpIconID ：当鼠标移进来的时候显示的图标
	virtual void UpdateSkillInfo(enObserverUseSkillInfo nInfo, int IconID, int UpIconID)	= NULL;
};
//------------------------------------------------------------------------
// 技能控制器,主角绑定
struct ISkillControl : public IThingPart
{
	// 取得该技能的等级(0为未学会)
	virtual WORD GetSkillLevel(WORD wSkillID)									= NULL;

	// 取得玩家所处的技能阶段状态
	virtual enSkillStatus GetSkillStatus()										= NULL;


	// 禁止施放技能
	virtual void ForbidSkill(enForbidSkillType nType)							= NULL;
	
	// 取消禁止施放技能
	virtual void UnForbidSkill(enForbidSkillType nType)							= NULL;
	

	// 激活某技能
	virtual void ActiveSkill(enSkillActive nType)								= NULL;
	
	// 取消激活某技能
	virtual void UnActiveSkill(enSkillActive nType)								= NULL;

	// 获取当前某个技能的激活情况
	virtual BOOL GetCurForbidSkill(enSkillActive nType)							= NULL;
	

	// 改变技能使用形态
	virtual void ChangeSkillShape()												= NULL;
	
	// 取消改变技能使用形态
	virtual void UnChangeSkillShape()											= NULL;
	
	// 获取当前技能使用形态 FALSE为未改变 TURE为改变了
	virtual BOOL GetCurSkillShape()												= NULL;


	// 开始监听某个技能的变化
	virtual void SubscibeSkill(WORD	wSkillID, ISkillObserver *pSkillObserver)	= NULL;

	// 取消监听某个技能的变化
	virtual void UnSubscibeSkill(WORD wSkillID, ISkillObserver *pSkillObserver)	= NULL;
};
//------------------------------------------------------------------------
// 技能管理器,用来管理技能及技能控制器
struct ISkillManager
{
	// 创建技能控制器
	virtual ISkillControl *CreateSkillControl()									= NULL;

	// 取得该技能的最大等级
	virtual int GetSkillMaxLevel(WORD wSkillID)									= NULL;

	// 取得某等级的技能的名字
	virtual void GetSkillName(SSkillData SkillData, LPSTR buf)					= NULL;

	// 取得某等级的技能的说明
	virtual void GetSkillDesc(SSkillData SkillData, LPSTR buf)					= NULL;

	// 取得某等级的技能的相关信息 错误返回-1
	virtual long GetSkillInfo(SSkillData SkillData, enSkillInfo nInfo)			= NULL;

	// 取得该技能是否锁定
	virtual BOOL GetSkillBeLock(WORD wSkillID)									= NULL;

	// 取得该技能类型（主动技能，被动技能，……）
	virtual enSkillType GetSkillType(WORD wSkillID)								= NULL;

	// 取得该技能的攻击方式（要选择对象，对地，……）
	virtual enSkillAttackType GetSkillAttackType(WORD wSkillID)					= NULL;

	// 取得技能图标，这是默认的图标
	virtual int	GetDefaultSkillIcon(WORD wSkillID)								= NULL;

	// 判断能否使用该等级技能
	virtual enCanUseSkillInfo CanUseSkill(SUseSkillContext * pContext,
		enUseSkillType nType = enATTACK_BEGIN, BOOL bNotJudgeDistant = FALSE)	= NULL;

	// 取得玩家某职业所能学的技能列表，nSize返回数组的大小，INVALID_SKILLID表示无效的技能ID
	virtual WORD* GetOccupationSkillList(int nOccupation, int &nSize)			= NULL;

	virtual void Release()														= NULL;
};
//------------------------------------------------------------------------
struct IClientGlobal;					// 服务端全局Global
#define VERSION_QUERY_SKILLCLIENT	1	// 版本号
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) BOOL ProcCreateSkillManager(DWORD dwVersion , ISkillManager** ppManager, IClientGlobal* pGlobal);

// 创建器
class CSkillHelper
{
public:
	ISkillManager	*m_pSM;
private:
	HINSTANCE		m_hdll;
public:
	// 创建类工厂
	typedef BOOL (*ProcCreateSkillManager)(DWORD dwVersion , ISkillManager** ppManager, IClientGlobal* pGlobal);	
	
	CSkillHelper()
	{
		m_hdll = NULL;
		m_pSM = NULL;
	}
	
	~CSkillHelper()
	{
		Close();
		if (m_hdll)
		{
			FreeLibrary(m_hdll);
			m_hdll = NULL;
		}	
	}

	void Close()
	{
		if (m_pSM)
		{
			m_pSM->Release();
			m_pSM = NULL;
		}		
	}

	ISkillManager* operator ->()
	{
		return m_pSM;
	}
	
	BOOL CreateSkillManager(IClientGlobal* pGlobal)
	{
		try
		{
			LoadDLL();
			
			ProcCreateSkillManager proc;
			proc = (ProcCreateSkillManager)GetProcAddress(m_hdll, "CreateSkillManager");
			
			if(proc == NULL)
				throw "Can not GetProAddress('CreateSkillManager')";
			
			if(!proc(VERSION_QUERY_SKILLCLIENT, &m_pSM, pGlobal))
				throw "CreateSkillManager() catch error!";
			return m_pSM != NULL;
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);OutputDebugString("\r\n");
			return NULL;
		}
		catch(...)
		{
			OutputDebugString("CreateSkillManager() catch error\r\n");
			return NULL;
		}
	}
	
	void LoadDLL()
	{
		if(m_hdll == NULL)
		{
#ifdef _DEBUG
			m_hdll = LoadLibrary("FKSkillClient_D.dll");
			if(m_hdll == NULL)
				throw "无法加载 FKSkillClient_D.dll";
#else
			m_hdll = LoadLibrary("FKSkillClient.dll");
			if(m_hdll == NULL)
				throw "无法加载 FKSkillClient.dll";
#endif
		}
	}
};
//------------------------------------------------------------------------