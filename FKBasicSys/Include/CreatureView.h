/**
*	created:		2012-7-2   2:16
*	filename: 		CreatureView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		生物视图管理
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Magic.h"
#include "AniGroup.h"
#include "AttachObj.h"
#include "Pal.h"
#include "ResLoader.h"
#include "Particle.h"
#include "../../FKChatClient/IPaoPao.h"
#include "../../FKGame/Timer/ITimerAxis.h"
#include "../../FKGame/MapView/MapItemView.h"
#include "../../FKGame/Common/CreatureViewDef.h"
//------------------------------------------------------------------------
class CMagicMgrShadow;
//------------------------------------------------------------------------
#define TIME_EVENT_ID_FRAME			0		// 动画时钟ID
#define TIME_EVENT_ID_SHORT_STEPS	1		// 碎步时钟ID
#define TIME_EVENT_ID_DIE			2		// 怪物尸体保存时钟ID
#define TIME_EVENT_ID_DIE_ANI		3		// 怪物尸体消失动画时钟ID

#define	TIME_SHORT_STEPS			5		// 生物在移动过程中的碎步时间间隔
#define TIME_DEFAULT_CORPER			10000	// 怪物尸体保存时间
#define TIME_DEFAULT_CORPER_ANI		40		// 怪物尸体消失动画时间

#define	SKILL_MAX_COUNT				98		// 技能数目

#define CREATURE_MAX_NAME			36		// 生物最大名字长度(字节)
#define BLOOD_SLOT_WIDTH			40		// 血条宽度
#define BLOOD_SLOT_HEIGHT			3		// 血条高度

#define SIT_HEIGHT_OFFSET			20		// 坐下状态的全高偏移

#define MAX_CHANGE_SHAP_LV			10		// 变身的最多优先级
//------------------------------------------------------------------------
struct SItemShowRect
{
	BOOL	bIni;
	RECT	rc;
};
//------------------------------------------------------------------------
// MonsterView存档格式
struct SMonsterViewArchive
{
	int		nType;
	DWORD	dwResID;
};
//------------------------------------------------------------------------
struct SMonsterViewCreateParam
{
	SMonsterViewCreateParam()
	{
		memset (this, 0, sizeof(SMonsterViewCreateParam));
	}

	int		nHalfHeightOffset;				//生物半高，显示伤害值的位置
	int		nHeightOffset;					//生物全高，显示名字血条的位置
	int		nWalkGrade;						//生物移动等级，实际上是帧延时
	int		nRunGrade;						//生物跑动等级，实际上是帧延时
	int		nAttackGrade;					//生物攻击等级，实际上是帧延时
	int		nShape;							//生物外形，有巨型怪物小型怪物中型怪物
	int		nType;							//生物类型，表明生物属于哪一类
	BYTE	nStateFlag[CrtPartMax];			//生物部件标志，标明该怪物有几个部件
	int		nActStateCommonAttack[3];		//普通攻击动作id
	int		nActStateJuqi;					//蓄气动作
	int		nActStateSkillAttack;			//技能攻击动作
	int		nSoundIDCommonAttack;			//普通攻击音效
	int		nSoundIDFallow;					//休闲音效
	int		nSoundIDBeHit;					//被击音效
	int		nSoundIDDie;					//死亡音效
};
//------------------------------------------------------------------------
class CCreatureView : public CMapItemView, public ITimerSink
{
private:
	struct SStep
	{
		float x, y;
	};

	struct SDblPoint
	{
		POINT ptTile, ptOff;
	};

public:
	CCreatureView();
	virtual ~CCreatureView();
	void Close();

// ITimerSink
public:
	virtual void OnTimer(DWORD dwEventID)	{}

// CMapItemView
public:
	virtual void	SetFlagColorOnMinmap(DWORD dwColor);						// 设置在小地图上的颜色
	virtual DWORD	GetFlagColorOnMinmap();										// 取得在小地图上的颜色
	
	virtual BOOL	GetVisibleOnMinmap();										// 取得是否在小地图上显示

	virtual void	SetItemData(DWORD dwData);									// 设置对象的私有数据
	virtual DWORD	GetItemData() const;										// 获取对象的私有数据

	virtual void	SetTile(POINT& ptTile, IMapView* pMapView);					// 设置在地图上的位置
	virtual POINT	GetTile();													// 获取在地图上的位置

	virtual BOOL	SetSelected(BOOL bSelected = TRUE, BOOL bHighLight = TRUE);	// 设置对象选中状态
	virtual BOOL	SetLocked(int nLockType = 1);								// 设置对象锁定状态

	virtual DWORD	GetResID();													// 获取资源ID
	virtual RECT	GetShowRect();												// 获取对象矩形大小范围
	virtual void	ClearShowRect();											// 清除对象矩形大小范围
	virtual POINT	GetAnchor() const;											// 获取锚点

	virtual BOOL	HitTest(const RECT& rcScreen);								// 测试当用户点中某个小区域时是否选择此对象
	virtual BOOL	OnEnterViewport(BOOL bVisable);								// 当对象进入视口和离开视口时被回调
	
	virtual BOOL	IsSinking();												// 是否潜水，哇哈哈

	virtual int		OnCommand(DWORD dwCmdID, DWORD dwParam1,DWORD dwParam2);	// 通用控制命令
	
public:
	// 生物与外界的交互命令(CreatureViewCommond.cpp里实现)
	typedef int (CCreatureView::*ProcCommand)		(DWORD dwParam1, DWORD dwParam2);
	
	virtual int cmdSetProperty(DWORD dwParam1, DWORD dwParam2);				// 设置属性
	virtual int cmdGetProperty(DWORD dwParam1, DWORD dwParam2);				// 获取属性

	virtual int cmdMove(DWORD dwParam1, DWORD dwParam2);					// 移动
	virtual int cmdStand(DWORD dwParam1, DWORD dwParam2);					// 站立
	virtual int cmdSetting(DWORD dwParam1, DWORD dwParam2);					// 生物坐下(不坐下)
	virtual int cmdFallow(DWORD dwParam1, DWORD dwParam2)		{return 0;}	// 休闲

	virtual int	cmdBorn(DWORD dwParam1, DWORD dwParam2)			{return 0;}	// 角色出生
	virtual int	cmdUpgrade(DWORD dwParam1, DWORD dwParam2)		{return 0;}	// 角色升级

	virtual int cmdChangeModule(DWORD dwParam1, DWORD dwParam2)	{return 0;}	// 换装
	virtual int cmdSpecialModule(DWORD dwParam1, DWORD dwParam2){return 0;}	// 设置角色默认部件
	virtual int	cmdModulePalType(DWORD dwParam1, DWORD dwParam2){return 0;}	// 角色各组件的调色板类型索引
	virtual int	cmdModuleAlpha(DWORD dwParam1, DWORD dwParam2)	{return 0;}	// 改变角色透明度

	virtual int	cmdAppearance(DWORD dwParam1, DWORD dwParam2);				// 外观
	virtual int	cmdBloodInfo(DWORD dwParam1, DWORD dwParam2);				// 显示血量
	virtual int cmdAddTitle(DWORD dwParam1, DWORD dwParam2)		{return 0;}	// 给生物添加称号
	virtual int cmdClearTitle(DWORD dwParam1, DWORD dwParam2)	{return 0;}	// 清除生物的所有称号
	virtual int	cmdChatInfo(DWORD dwParam1, DWORD dwParam2);				// 角色头顶的聊天信息
	virtual int	cmdMouseAni(DWORD dwParam1, DWORD dwParam2)		{return 0;}	// 主角行走时，在路上点击鼠标留下的点击动画

	virtual int	cmdPreAttack(DWORD dwParam1, DWORD dwParam2);				// 蓄气
	virtual int	cmdStopPreAttack(DWORD dwParam1, DWORD dwParam2);			// 停止蓄气
	virtual int	cmdAttack(DWORD dwParam1, DWORD dwParam2);					// 攻击
	virtual int	cmdStopAttack(DWORD dwParam1, DWORD dwParam2);				// 停止攻击
	virtual int	cmdChangeLocForSkill(DWORD dwParam1, DWORD dwParam2);		// 用技能顺移
	virtual int	cmdSkillSelAni(DWORD dwParam1, DWORD dwParam2);				// 范围技能的选择光效

	virtual int cmdBeHit(DWORD dwParam1, DWORD dwParam2);					// 生物被击
	virtual int cmdDie(DWORD dwParam1, DWORD dwParam2);						// 生物死亡

	virtual int cmdSetNpcType(DWORD dwParam1, DWORD dwParam2)	{return 0;}	// 设置NPC类别对应的头顶光效
	virtual int cmdSetTaskType(DWORD dwParam1, DWORD dwParam2)	{return 0;}	// 任务NPC头顶的光效

	virtual int cmdSetClanName(DWORD dwParam1, DWORD dwParam2);				// 设置氏族名称
	virtual int cmdSetCityName(DWORD dwParam1, DWORD dwParam2);				// 设置城池名称

	// 技能的创建函数(CreatureViewSkill.cpp里实现)
	typedef BOOL (CCreatureView::*ProcCreateSkill)	(SSkillCreateParam* pParam);

	BOOL skill_NNNNNNNNNNNN(SSkillCreateParam* pParam);						// 空技能还没做
	BOOL skill_None(SSkillCreateParam* pParam);								// 无魔法效果技能
	BOOL skill_CommonAttack(SSkillCreateParam* pParam);						// 普通攻击
	BOOL skill_SingleDir(SSkillCreateParam* pParam);						// 单向飞行技能(如火球)
	BOOL skill_RainSkill(SSkillCreateParam* pParam);						// 多单元雨类型技能的创建过程(如暴风雪)
	BOOL skill_WanJianSkill(SSkillCreateParam* pParam);					// 万剑朝宗类技能的创建过程
	BOOL skill_MultiUnitSingleTileSkill(SSkillCreateParam* pParam);			// 对同个目标的多个单元魔法（如连冰箭等）
	BOOL skill_MultiDirSkillEx(SSkillCreateParam* pParam);					// 多向飞行技能(如暴烈火球)
	BOOL skill_SingleCirculDirSkill(SSkillCreateParam* pParam);				// 8方向单方向技能的创建过程
	BOOL skill_HalfCirculDirSkill(SSkillCreateParam* pParam);				// 8方向扇形技能的创建过程
	BOOL skill_CirculDirSkill(SSkillCreateParam* pParam);					// 8方向圆形技能的创建过程
	BOOL skill_GenericSkill(SSkillCreateParam* pParam);						// 通用技能创建
	BOOL skill_ChangeLoc(POINT pt);											// 顺移魔法
	
	CMagic* CreateSkillSelMagic(int nSkillID);								// 创建技能选择光效


	// 设置和获取生物属性(CreatureViewProperty.cpp里实现)
	typedef void	(CCreatureView::*ProcProperty)		(BOOL bSetProp, DWORD dwPropVal);

	virtual void propMoveStype(BOOL bSetProp, DWORD dwPropVal);				// 移动类型
	virtual void propTile(BOOL bSetProp, DWORD dwPropVal);					// 生物的TILE坐标
	virtual void propUserName(BOOL bSetProp, DWORD dwPropVal);				// 生物的名字
	virtual void propShowName(BOOL bSetProp, DWORD dwPropVal);				// 是否让生物显示自己的名字
	virtual void propNameColor(BOOL bSetProp, DWORD dwPropVal);				// 生物名字的颜色
	virtual void propAbsolutePosition(BOOL bSetProp, DWORD dwPropVal);		// 生物的绝对位置
	virtual void propWeaponStyle(BOOL bSetProp, DWORD dwPropVal)		{}	// 角色携带武器的方式
	virtual void propWeaponSoundID(BOOL bSetProp, DWORD dwPropVal)		{}	// 角色武器的攻击音效ID
	virtual void propBeHitSoundID(BOOL bSetProp, DWORD dwPropVal)		{}	// 角色武器的击中目标音效ID
	virtual void propIsMoving(BOOL bSetProp, DWORD dwPropVal);				// 生物是否正在移动
	virtual void propState(BOOL bSetProp, DWORD dwPropVal);					// 生物的状态
	virtual void propDirection(BOOL bSetProp, DWORD dwPropVal);				// 生物的方向
	virtual void propMoveSpeedLevel(BOOL bSetProp, DWORD dwPropVal);		// 生物的移动速度等级
	virtual void propAttackSpeedLevel(BOOL bSetProp, DWORD dwPropVal);		// 生物的攻击速度等级
	virtual void propGroupInfoImage(BOOL bSetProp, DWORD dwPropVal)		{};	// 帮会信息图片

	// 生物移动函数(CreatureView.cpp里实现)
	typedef void	(CCreatureView::*ProcMoveStepByDir)	(POINT& ptTileOffsetOld);
	
	void _move_step_north(POINT& ptTileOffsetOld);
	void _move_step_eastnorth(POINT& ptTileOffsetOld);
	void _move_step_east(POINT& ptTileOffsetOld);
	void _move_step_eastsouth(POINT& ptTileOffsetOld);
	void _move_step_south(POINT& ptTileOffsetOld);
	void _move_step_westsouth(POINT& ptTileOffsetOld);
	void _move_step_west(POINT& ptTileOffsetOld);
	void _move_step_westnorth(POINT& ptTileOffsetOld);

public:
	static ProcCommand			m_Command[CrtCmdMaxCommandCount];
	static ProcCreateSkill		m_funcSkill[SKILL_MAX_COUNT + 1];
	static ProcProperty			m_Property[CrtPropMaxPropCount];
	static ProcMoveStepByDir	m_MoveStepByDir[CrtDirMaxDirCount];

public:

	// 状态相关
	virtual void SetState(int nState)							{}		// 设置当前状态
	virtual void StopState(int nState)							{}		// 停止当前状态
	int& State();														// 获取当前状态
	int  GetState();													// 获取当前状态
	virtual void RestartCurState()								{}		// 重启当前状态

	// 动画上下文相关
	void IncCurFrame();													// 下一祯
	void DecCurFrame();													// 上一祯
	bool IncAndCheckFrameOverflow();									// 转到下一祯并判断是否大于最大祯数
	bool IncAndCheckFrameOverflow(int nFrameIndex);						// 转到下一祯并判断是否大于参数
		
	void SetCurFrame(int nFrame);										// 设置当前祯
	int& CurFrame();													// 获取当前祯
	int  GetCurFrame();													// 获取当前祯
		
	void IniTotalFrame();												// 初始化当前状态的最大祯
	void SetTotalFrame(int nFrames);									// 置最大祯
	int  GetTotalFrame();												// 获取最大祯
	
	int  GetCurAni(int nFind);											// 返回当前是第几个动画，参数：第几个部件
	
	int GetFrameDelay(int nGrade = -1);									// 获取祯延迟

	BOOL GetPartNeedChangePal(int nModuleType);							// 获取该部件是否需要改变调色板
	CAniGroup* RequestRes(int nModuleType);								// 读取图包
	
	virtual int GetNormalAttackActID()					{return -1;}	// 获取默认的普通攻击动作ID
	virtual int GetJuqiActID()							{return -1;}	// 获取默认的蓄气动作ID（非玩家有效）
	virtual int GetSkillAttackActID()					{return -1;}	// 获取默认的技能攻击动作ID（非玩家有效）

	void SetVisible(BOOL bVisible);
	BOOL GetVisible();
	
	virtual void SetShowRect()									{}		// 设置对象矩形大小范围

	// 偏移相关
	void SetHalfHeightOffset(int nOffSet);							// 设置半高
	int  GetHalfHeightOffset();										// 获取半高
	void SetHeightOffset(int nOffSet);								// 设置全高
	int  GetHeightOffset();											// 获取全高

	// 移动相关
	virtual void SetStepWalk(float fStep);							// 设置生物的行走步长(以竖直方向为准)
	virtual void SetStepRun(float fStep);							// 设置生物的跑动步长(以竖直方向为准)
	virtual void SetStepRide(float fStep);							// 设置生物的骑马跑动步长(以竖直方向为准)

	void GetMoveStep(int nDir, float& x, float& y);					// 获取移动步长
	void GetMoveStepEx(int nDir, float& x, float& y);				// 获取碎步移动步长

	void SetMoveStyle(int nMoveStyle);								// 设置移动方式
	int  GetMoveStyle();											// 获取移动方式
	int  GetMoveStateByStyle();										// 通过移动方式获取移动状态

	void SetDirection(int nDir);									// 设置当前方向
	int  GetDirection();											// 获取当前方向

	void CalcSpeedMove();											// 矫正当前移动速度
	SStep*	GetSpeedMove(int nDir);									// 获取当前移动速度

	void SetCurSpeed(SStep* pSpeed);								// 设置当前方向下的移动速度
	void GetCurSpeed(float& x, float& y);							// 获取当前方向下的移动速度

	void OnMoveStep(DWORD dwPeriod);								// 正常移动
	void OnMoveStepEx(DWORD dwPeriod);								// 没停在Tile中心点的用这种移动
	void MoveStep(DWORD dwPeriod);									// 根据当前的移动方式移动

	BOOL IsMoving();												// 是否在移动中
	void StopMove();												// 停止移动

	void OnChangeDir();												// 当方向改变时调用
	BOOL OnChangeTile();											// 当Tile改变时调用
	
	void AdjustDirection(SSkillCreateParam* pParam);				// 校正生物的方向

	void ResetMoveInfo();											// 重设移动数据
	void SetOccupant(BOOL bOccupant);								// 设置生物是否占位
	BOOL GetOccupant();												// 获取生物是否占位

	virtual void SetAllMoveDelay(int* pDelay)					{}	// 设置移动的祯延时

	// 吸附对象相关
	CAttachStateObj* CreateAttachObj(SAttachStateObjBuffer* pBuf);	// 创建吸附对象

	virtual void RemoveAllAttachObj()							{}	// 删掉所有吸附对象
	virtual void RemoveAllFlowObj();								// 删掉所有飘血对象

	virtual void OnAttachObjDestroy(IAttachObj* pAttachObj)		{}	// 回调(跟随对象销毁时回调)
	virtual void OnFlowObjDestroy(IAttachObj* pAttachObj)		{}	// 回调(飘字对象销毁时回调)

	virtual void StopAttachObj(IAttachObj* pAttachObj);				// 停止某个跟随对象
	void StopAllAttackAttach();										// 停止所有施法吸附对象

	int ChangePal(DWORD dwPalType);									// 改变调色板类型
	int UnChangePal(DWORD dwPalType);								// 取消改变调色板类型

	int CreateAppearance(DWORD dwAppearanceID);						// 根据外观ID创建吸附光效
	int CancelAppearance(DWORD dwAppearanceID);						// 根据外观ID销毁吸附光效

	// 法术管理相关
	CMagicMgrBase* FindMagic(int nID);								// 寻找当前链接的法术管理器
	void MagicBeginAttack(int nCheckID);							// 让技能开始攻击
	void AddMagic(CMagicMgrBase *pMgr);								// 加入法术管理器链接
	void DelMagic(CMagicMgrBase *pMgr);								// 删掉法术管理器链接
	void StopMagic();												// 停止所有魔法
	void AddDmgStack(SBehitParam *pParam);							// 加入伤害堆栈管理

	void FlowHP(SFlowParam FlowParam,
		CCreatureView* pSourceView = NULL,
		long lMagicIDBeHit = 0,
		int nBeHitSoundID = 0);										// 生物飘血

	void FlowDamageNum(SFlowParam FlowParam, CCreatureView* pSourceView); // 生物飘血
	DWORD GetLastFlowTime();										// 获取上次飘血的时间
	void SetLastFlowTime(DWORD dwTime);								// 设置上次飘血的时间

	virtual int  GetWeaponStyle();					// 获取武器类型

	virtual void SetStatusMagicCheckID(int nCheckID)			{}	// 设置目前状态下对应的技能ID
	virtual void SetStatusPlayCommondAttackSound()				{}	// 设置目前状态下要播放普通攻击声音
	virtual void SetStatusPreAttack()							{}	// 设置目前状态下的蓄气属性
	virtual void SetStatusAttackOnce()							{}	// 设置目前状态下的一次性攻击属性
	virtual void SetStatusAttackCtrl()							{}	// 设置目前状态下的持续攻击属性

	virtual void OnDrawEx(ICanvas* pCanvas, const POINT& ptTileCenter,
		DWORD dwFlag, SAniContext *pContext, BYTE nAlpha = 0)	{}	// 为了残影做的

	// 指针链接相关
	void AddLinkView(CCreatureView **ppView);						// 加入指针链接
	void DelLinkView(CCreatureView **ppView);						// 删掉指针链接

	// 变身相关
	void ChangeShap(DWORD dwShapID, int nLv);						// 变身
	void UnChangeShap(DWORD dwShapID, int nLv);						// 取消变身
	void DoChangeShap();
	BOOL IsChangeShap();											// 当前是否变身了

	virtual BOOL ChangeShapEX()						{return FALSE;}

	// 其他
	void DrawName(ICanvas* pCanvas, const POINT& ptBottomCenter);			// 绘制生物的名字
	void DrawSelectedName(ICanvas* pCanvas, const POINT& ptBottomCenter);	// 绘制选择中的生物的名字
	void DrawBloodSlot(ICanvas* pCanvas, RECT& rc, int nPercent);			// 画血槽
	void StopAttackSound();													// 停止攻击声音
	BOOL IsDeaded();														// 是否死亡了
	void AddParticle(DWORD dwPtcID, int nPart);								// 添加粒子效果
	void UnAddParticle(DWORD dwPtcID, int nPart);							// 取消粒子效果

public:	
	//图包相关
	DWORD			m_dwType;								// 生物Type
	DWORD			m_dwCrtID;								// 生物ID

	CAniGroup*		m_pModule[CrtPartMax];					// 组件图包
	DWORD			m_dwModuleID[CrtPartMax];				// 组件图包ID
	DWORD			m_dwDefaultResID[CrtPartMax];			// 默认图包ID
	BYTE			m_bytePal[CrtPartMax];					// 组件的调色板索引号
	BYTE			m_byteAlpha[CrtPartMax];				// 组件的透明度索引号
	BYTE*			m_PartStateFlag[CrtPartMax];			// 保存各个部件的状态标志

	CPal			m_Pal[CrtPartMax];						// 组件的调色板
	int				m_nCurPalList[PalType_MaxType];			// 当前要求改变的调色板列表
	int				m_nCurPalType;							// 当前调色板类型
		
	BYTE			m_byteAlphaAll;							// 总透明度，生物透明时用到

	DWORD			m_dwItemData;							// 覆盖MapItem的函数使用到
	DWORD			m_dwResID;								// 资源ID，用于向资源加载器请求图象资源，如果此组件存在武器类型，则记录父对象的ID
	POINT			m_ptAnchor;								// 图象锚点
	SAniContext		m_AniContext;							// 动画上下文
	SAniContext		m_AniContextOld;						// 上次画图的动画上下文

	// 生物名字、血条相关
	char			m_szUserName[CREATURE_MAX_NAME];		// 生物的名字
	char			m_szClanName[CREATURE_MAX_NAME + 3];	// 氏族名字
	char			m_szCityName[CREATURE_MAX_NAME + 3];	// 氏族名字

	BOOL			m_bShowClanName;						// 是否显示生物的氏族名字
	BOOL			m_bShowCityName;						// 是否显示生物的城池名字
	BOOL			m_bShowUserName;						// 是否显示生物的名字
	WPixel			m_clrName;								// 绘制生物名字的颜色
	int				m_nBloodVol;							// 当前血量(最大100)
	WPixel			m_wpMinMapFlagColor;					// 小地图的颜色
	IPaoPao*		m_pPaoPao;								// 人头泡泡
	BOOL			m_bVisible;								// 此生物是否可见(隐身)	

	int				m_nHalfHeightOffset;					// 相对锚点的y向半高偏移(这个位置可以用来显示生物的被击动画等)
	int				m_nHeightOffset;						// 相对锚点的y向高度偏移(这个位置用于确定血条、生物名、聊天信息等位置)

	// 移动相关
	BOOL			m_bNormalMove;							// 是否正常移动，有时候起始点不是在Tile中心点的就需要特殊移动，既先移动到中心点再正常移动
	int				m_nMoveSpeedLevel;						// 生物移动的速度等级
	int				m_nAttackSpeedLevel;					// 生物走路的速度等级
	float			m_fStepWalk;							// 走路步长
	float			m_fStepRun;								// 跑步步长
	float			m_fStepRide;							// 骑马步长
	SStep			m_StepWalk[CrtDirMaxDirCount];			// 生物走路8个方向的步长(每秒移动的Tile数)
	SStep			m_StepRun[CrtDirMaxDirCount];			// 角色跑动8个方向的步长(每秒移动的Tile数)
	SStep			m_StepRide[CrtDirMaxDirCount];			// 角色骑马8个方向的步长(每秒移动的Tile数)
	SStep			m_SpeedMove[CrtDirMaxDirCount];			// 生物移动8个方向的速度(每祯移动的Tile数)
	SStep			m_fCurSpeed;							// 当前速度，既当前方向下的m_SpeedMove
	POINTS*			m_pPathList;							// 路径列表
	int				m_nPathNodeCount;						// 路径节点数
	int				m_nMaxPathNodeCount;					// 最大路径节点数
	int				m_nCurNodeIndex;						// 当前路径节点索引
	BOOL			m_bNeedWait;							// 是否因为前面有人而需要等待?
	int				m_nWaitTime;							// 等待的时间
	int				m_nMoveStyle;							// 角色的移动方式(行走?跑步?)
	int				m_nLastMoveDir;							// 最后一次移动方向
	SDblPoint*		m_pPointList;							// 双点列表(用于从非中心点开始移动时)
	int				m_nMaxPointNodeCount;					// 最大双点节点数
	int				m_nPointNodeCount;						// 双点节点数
	int				m_nCurPointNodeIndex;					// 当前双点节点索引
	DWORD			m_dwStartTick;							// 开始tick数
	DWORD			m_dwCurTick;							// 当前的tick数
	POINT			m_ptStartPos;							// 开始处的位置


	BOOL			m_bDeaded;								// 是否死亡了

	ICreatureSink*	m_pCrtSink;								// 生物回调接口

	POINT			m_ptTile;								// 对象在地图上的TILE坐标
	POINT			m_ptTileOffset;							// 角色当前位置与TILE中心点位置的偏移

	BOOL			m_bOccupant;							// 用来设置生物是否占位

	int				m_nCommonAttackSoundID;					// 普通攻击的音效
	int				m_nCommonAttackBeHitSoundID;			// 普通攻击的击中音效
	int				m_nSoundIDFallow;						// 生物休闲音效
	int				m_nSoundIDBeHit;						// 生物被击音效
	int				m_nSoundIDDie;							// 生物死亡音效

	int				m_nAttackSoundChunnelID;				// 攻击的声音频带ID

	int				m_nBeHitLoopBeginFrame;					// 被击动作的循环开始帧
	int				m_nBeHitLoopEndFrame;					// 被击动作的循环结束帧

	SItemShowRect	m_ItemShowRect[CrtStateMax][CrtDirMaxDirCount][30];

	vector<IAttachObj*>			m_vAttachObjs;				// 跟随对象数组
	vector<IAttachObj*>			m_vFlowObjs;				// 飘血对象数组
	vector<IAttachObj*>			m_vAttackAttachObjs;		// 攻击时创建的吸附对象

	CMagicMgrShadow*			m_pMagicShadow;				// 拖影效果

	int				m_nCommonAttackStyleNum;				// 默认了几种普通攻击方式
	int				m_nActStateCommonAttack[3];				// 默认的普通攻击状态ID
	int				m_nActStateJuqi;						// 默认的蓄气状态ID
	int				m_nActStateSkillAttack;					// 默认的技能攻击状态ID

	// 变身相关	
	BOOL			m_bChangeShap;							// 是否变身了
	DWORD			m_dwShapID[MAX_CHANGE_SHAP_LV];			// 变身后的外形ID
	int				m_nShapLv;								// 当前处于哪个优先级的变身效果

	int				m_nBakCommonAttackSoundID;				// 备份的普通攻击声音
	int				m_nBakCommonAttackBeHitSoundID;			// 备份的普通攻击击中声音
	DWORD			m_dwBakModuleID[CrtPartMax];			// 备份组件图包ID
	BYTE			m_byteBakPal[CrtPartMax];				// 备份组件的调色板索引号
	BYTE			m_byteBakAlpha[CrtPartMax];				// 备份组件的透明度索引号

protected:
	vector<CCreatureView**>		m_vppLinkView;				// 外面指向该对象的指针列表，防止对象释放了外面还拿来用
	vector<CMagicMgrBase*>		m_vMagicMgr;				// 之前施放的技能管理器
	IAttachObj*					m_pLockAttachObj;
	DWORD						m_dwLastFlowTime;			// 上次飘血的时间
	CParticle*		m_pParticle[CrtPartMax];				// 粒子效果ID Creature的所有部件上都可附粒子效果
};
//------------------------------------------------------------------------