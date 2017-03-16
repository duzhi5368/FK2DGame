/**
*	created:		2012-7-2   1:24
*	filename: 		Magic
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		魔法管理
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/MapView/MapItemView.h"
#include "../Include/Ani.h"
#include "../../FKGame/Timer/ITimerAxis.h"
#include "../Include/ResLoader.h"
#include "../Include/AniObj.h"
#include "../Include/PalMgr.h"
#include "../../FKGame/Common/CreatureViewDef.h"
#include "../Include/Color.h"
#include "../Include/Particle.h"
//------------------------------------------------------------------------
#define max_pal_color								8
//------------------------------------------------------------------------
// 检测的占位块数
#define CHECK_DISTANCE								3
#define DEFAULT_FLY_SPEED							15
#define PI											3.1416f
#define MAX_LINE_PATH_LENGTH						100
//------------------------------------------------------------------------
// 魔法管理器相关定义
#define TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY		1
#define TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE		2
#define TIME_EVENT_ID_MAGIC_MGR_ATTACK_LOOP			3
#define TIME_EVENT_ID_MAGIC_MGR_DESTROY				10
//------------------------------------------------------------------------
#define MAGIC_MGR_DEFAULT_ATTACK_DELAY_TIME			2000
#define MAGIC_MGR_DESTROY_TIME						5000
//------------------------------------------------------------------------
int		line(int x1, int y1, int x2, int y2, POINT* pptArray, int& nLen, int nMaxLen=0,
			 bool bTestTile=true, int nBlockTestMode = 0);
int		_test_tile_spread(POINT& ptTile);
int		_test_tile_block(POINT& ptTile, int nBlockTestMode);
//------------------------------------------------------------------------
class CCreatureView;
//------------------------------------------------------------------------
// Magic存档格式
struct SDamageData
{
	SFlowParam		FlowParam;
	CCreatureView	*pView;
};
//------------------------------------------------------------------------
// Magic存档格式
struct SMagicArchive
{
	DWORD	dwResID;
	POINT	ptAnchor;
	POINT	ptOffset;
	int		nDelay;
	int		nLayerValue;
	DWORD	dwSoundResID;
	BOOL	bSoundLoop;
};
//------------------------------------------------------------------------
// 单向魔法存档格式
struct SMagicArchive_SingleDir : public SMagicArchive
{
	POINT	ptStartTile;	// 魔法开始点(TILE坐标)
	POINT	ptAttackTile;	// 魔法攻击点(TILE坐标)
	int		nDir;			// 方向数
	int		nFramesPerDir;	// 每个方向上的帧数
	POINT	ptOffset;		// 魔法距离地面的偏移
	POINT	ptTileOffset;	// 魔法距离Tile中心点的偏移
	float	fDir;			// 开始时的方向
	float	fDirInc;		// 方向变换率
	int		nTimeTile;		// 飞过一个Tile的时间
	int		nLife;			// 飞行生命
	CCreatureView	*pAttackView;
};
//------------------------------------------------------------------------
// 有生命期的魔法存档格式
struct SMagicArchive_Life : public SMagicArchive
{
	int	nLifeLine;			// 魔法生命期
};
//------------------------------------------------------------------------
// 环形魔法存档格式
struct SMagicArchive_Ring : public SMagicArchive_Life
{
	int	nFramesPerDir;		// 魔法每个方向上的帧数
};
//------------------------------------------------------------------------
// 地上角色的脚印存档格式
struct SMagicArchive_FootPrint : public SMagicArchive_Life
{
	int nDirection;
};
//------------------------------------------------------------------------
// 只播放八个方向中某个方向的魔法存档格式
struct SMagicArchive_EightDir : public SMagicArchive_Life
{
	int		nFramesPerDir;	// 每个方向上有多少帧
	POINT	ptTile;
	int		nDir;
};
//------------------------------------------------------------------------
// 带有3个状态的魔法存档格式
struct SMagicArchive_ThreeState : public  SMagicArchive_Life
{
	int   nLoopStartIndex;	// 动画循环所对应的帧索引,注：这样的动画是将他启动、循环、销毁三个状态的动画打在一起
	int   nLoopEndIndex;	// 动画结束所对应的帧索引
};
//------------------------------------------------------------------------
// 象流星雨一样斜着下坠的魔法存档格式
struct SMagicArchive_Rain : public SMagicArchive_ThreeState
{
	BOOL  bNeedRandomOffset;// 是否需要随机生成偏移
	POINT ptStepOffset;		// 每次偏移量
};
//------------------------------------------------------------------------
// 通用魔法存档格式
struct SMagicArchive_General : public  SMagicArchive
{
	int   nLoopStartIndex;	// 动画循环所对应的帧索引,注：这样的动画是将他启动、循环、销毁三个状态的动画打在一起
	int   nLoopEndIndex;	// 动画结束所对应的帧索引
};
//------------------------------------------------------------------------
// 范围技能的鼠标光效存档格式
struct SMagicArchive_Mouse : public  SMagicArchive
{
	int   nLoopStartIndex;	// 动画循环所对应的帧索引,注：这样的动画是将他启动、循环、销毁三个状态的动画打在一起
	int   nLoopEndIndex;	// 动画结束所对应的帧索引
};
//------------------------------------------------------------------------
// 用于文件
struct SMagicCreateParam 
{
	POINTS	ptAnchor;
	WORD	nDelay;
	BYTE	nLife;
	POINTS	ptOffset;
	BYTE	nLoopStartIndex;
	BYTE	nLoopEndIndex;
	int		nLayerValue;

};
//------------------------------------------------------------------------
// 魔法管理器基类
class CMagicMgrBase
{
public:
	CMagicMgrBase();
	virtual ~CMagicMgrBase();

	virtual BOOL	Create(void* buf, int nSize)					{return FALSE;}

	// 当一个魔法单元爆炸之后会回调这个函数
	virtual void	OnPlayExplode(void* pThis)						{}
	// 当一个魔法单元运行完之后会回调这个函数
	virtual void	OnPlayDone(void* pThis, POINT Pt)				{}

	// 告诉魔法管理器开始攻击或者释放魔法
	virtual void	BeginAttack()									{}
	// 设置为已经释放攻击
	virtual void	SetBeginAttack()								{m_bBeginAttack = TRUE;}
	// 是否已经释放攻击了
	virtual BOOL	IsBeginAttack()									{return m_bBeginAttack;}
	// 停止施法
	virtual void	Stop()											{}
	// 魔法是否还在运行
	virtual BOOL	IsRunning()										{return FALSE;}

	// 设值较检ID
	virtual void	SetCheckID(int nID)								{m_nCheckID = nID;}
	// 获取较检ID
	virtual int		GetCheckID()									{return m_nCheckID;}

	// 设值攻击延迟，既魔法动画延迟
	virtual void	SetAttackDelay(int nDelay)						{m_nAttackDelay = nDelay;}
	// 获取攻击延迟
	virtual int		GetAttackDelay()								{return m_nAttackDelay;}

	// 设置被击魔法ID
	virtual void	SetMagicIDBeHit(int nMagicIDBeHit)				{m_nMagicIDBeHit = nMagicIDBeHit;}

	// 设置被击魔法ID
	virtual void	SetSoundIDBeHit(int nSoundIDBeHit)				{m_nSoundIDBeHit = nSoundIDBeHit;}

	// 添加进一个伤害包
	virtual void	AddDamage(CCreatureView *pView, SFlowParam FlowParam);
	// 飘多少个伤害
	virtual void	FlowDamage(int nNum);
	// 飘伤害
	virtual void	FlowAllDamage();
	// 返回当前飘了多少个伤害
	virtual int		GetFlowDamageNum();
	// 表示魔法可以伤害了，以后添加的伤害都马上飘伤害
	virtual void	BeginDamage();
	// 获取最前面的伤害数据
	virtual SDamageData* GetFrontDamageData();
	// 删掉最前面的一个伤害
	virtual void	DelFrontDamage();

	// 设置魔法管理器的主人
	virtual void 	SetMasterView(CCreatureView	*pView);
	// 设置魔法要攻击的对象
	virtual void 	SetAttackView(CCreatureView	*pView);

protected:
	CCreatureView	*m_pMasterView;			// 使用该魔法的生物指针
	CCreatureView	*m_pAttackView;			// 被击者的生物指针
private:
	BOOL	m_bBeginAttack;					// 魔法或者伤害是否开始释放了
	BOOL	m_bBeginDamage;					// 魔法是否可以伤害了
	int		m_nMagicIDBeHit;				// 被击时的光效
	int		m_nSoundIDBeHit;				// 被击时的音效
	int		m_nAttackDelay;					// 攻击延时
	int		m_nCheckID;						// 魔法的较检ID，唯一确定
	int		m_FlowDamNum;					// 当前飘了多少次伤害了
	list<SDamageData *> m_vDamageData;		// 伤害栈
};
//------------------------------------------------------------------------
// 魔法基类
class CMagic  : public CMapItemView, public ITimerSink
{
public:
	// 图片资源相关
	DWORD			m_dwResID;			// 资源ID，用于向资源加载器请求图象资源
	CAni*			m_pPoolNodeObj;		// 图片内存，当此图片移出可见区域时将被丢进缓存里

	// 动画相关
	SSimpleAniContext	m_AniContext;	// 动画上下文

	// 位置相关
	POINT			m_ptOffset;			// 相对于地面中心点的偏移
	POINT			m_ptAnchor;			// 图象锚点
	POINT			m_ptTile;			// 在地图上的Tile坐标
	int				m_nLayerValue;		// 地图上的层次

	// 父魔法
	CMagicMgrBase*	m_pMgr;				// 魔法管理器，主要用于回调

	DWORD			m_dwSoundResID;		// 音效ID
	BOOL			m_bSoundLoop;		// 音效是否循环
	int				m_nSoundChunnelID;	// 声音的频道ID
public:
	CMagic();
	CMagic(CMagicMgrBase* pMgr);
	virtual ~CMagic();
	virtual void	Close();

	virtual void	OnTimer(DWORD dwEventID);
	
	// CMapItem ,暂时用来设置TILE坐标
	virtual void	SetItemData(DWORD dwData);
	// CMapItemView
	virtual void	SetTile(POINT& ptTile, IMapView* pMapView);

	virtual DWORD	GetLayerValue();
	virtual DWORD	GetOccupantValue();
	virtual RECT	GetShowRect();
	virtual POINT	GetTile();
	virtual DWORD	GetResID();
	virtual POINT	GetAnchor() const { return m_ptAnchor; }
	virtual DWORD	GetClassID() const{ return miMagicObj; }
	virtual IBitmap	*GetBitmap();

	virtual BOOL	OnEnterViewport(BOOL bVisable);
	virtual void	GetOldDirtyRect(list<RECT> &DirtyRectList);
	virtual void	SetDirty(ICanvas* pCanvas, const POINT& ptViewTop);
	virtual void	OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0);
	virtual void	OnDrawTopmostObject(ICanvas* pCanvas,const POINT& ptTileCenter, DWORD dwFlag);
	virtual BOOL	OnSerialize(BOOL bWrite,IArchive* pArchive);
	virtual BOOL	OnCreateItem(const char* buf, int nLen);

	virtual	BOOL	HitTest(const RECT& rcScreen);
	virtual BOOL	AddToTimerAxis();
	virtual BOOL	AddToMapView(POINT& ptTile);
	virtual BOOL	RemoveFromTimerAxis();
	virtual BOOL	RemoveFromMapView();

	void	UpdatePosInfo(IMapView* pMapView);
	CAni	*RequestRes();
	BOOL	CreateItem(const char* buf, int nLen);
	
	// 回调
	void	SetMgr(CMagicMgrBase* pMgr);
};
//------------------------------------------------------------------------
// 单向射击类魔法，如火球等
class CMagicSingleDir : public CMagic
{
public:
	int		m_nLen;				// 数组长度
	int		m_nFramesPerDir;	// 每个方向的帧数(<8)
	int		m_nStartFrameIdx;	// 起始帧索引
	POINT	m_ptTileOffset;		// 魔法偏移(相对当前tile中心点的)
	float	m_fOldDir;			// 起始方向
	float	m_fDirInc;			// 每次方向改变幅度
	int		m_nDirBI;			// 方向比
	POINT	m_ptTileAttack;		// 攻击的位置
	CCreatureView	*m_pAttackView;	// 攻击对象的指针
	int		m_nTimeTile;		// 飞过一个TILE的时间
	int		m_nLife;
	
	CMagicSingleDir();
	virtual ~CMagicSingleDir();

	virtual void Close();
	virtual BOOL AddToMapView(POINT& ptTile);
	virtual BOOL AddToTimerAxis();
	virtual void OnTimer(DWORD dwEventID);
	virtual void OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0);
	virtual RECT GetShowRect();
	virtual BOOL OnCreateItem(const char* buf, int nLen);
};
//------------------------------------------------------------------------
// 有生命期的魔法
class CMagicLife : public CMagic
{
public:
	// 生命期相关
	int				m_nCurLife;			// 当前生命
	int				m_nLifeLine;		// 生命期

	CMagicLife() : m_nCurLife(0),m_nLifeLine(-1) {}
	virtual ~CMagicLife() {}

	virtual void OnTimer(DWORD dwEventID);
	virtual BOOL OnCreateItem(const char* buf, int nLen);
};
//------------------------------------------------------------------------
// 环形魔法
class CMagicRing : public CMagicLife
{
public:
	int		m_nFramesPerDir;	// 魔法每个方向上的帧数
	int		m_nNoBlockDirs;		// 没有遮挡的方向数
	int		m_aDirs[32];		// 没有遮挡的方向列表(最大支持32个方向)
	int		m_nDirs;
	
	CMagicRing() : m_nFramesPerDir(0),m_nNoBlockDirs(0) {}
	virtual ~CMagicRing() {}

	virtual void	OnTimer(DWORD dwEventID);
	virtual DWORD	GetLayerValue() {return milvLowest;}
	virtual BOOL	OnCreateItem(const char* buf, int nLen);
	virtual void	OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0);

	void SetDirsInfo(int* nDirList, int nDirCount);
};
//------------------------------------------------------------------------
// 只播放八个方向中某个方向的魔法
class CMagicEightDir : public CMagicLife
{
public:
	int	m_nFramesPerDir;	// 魔法每个方向上的帧数
	int m_nStartFrameIndex;
	int m_nEndFrameIndex;

	CMagicEightDir() : m_nFramesPerDir(0),m_nStartFrameIndex(0),m_nEndFrameIndex(0)	{}
	virtual ~CMagicEightDir() {}

	virtual void	OnTimer(DWORD dwEventID);
	virtual DWORD	GetLayerValue();
	virtual BOOL	OnCreateItem(const char* buf, int nLen);
};
//------------------------------------------------------------------------
// 地上物品上闪烁的星星
class CMagicStar : public CMagicLife
{
	int		m_nCount;	// 用于动画一个轮回之间的间隔
public:
	enum
	{
		MAX_COUNT = 0
	};

	CMagicStar() { m_nCount = MAX_COUNT - 10 + rand() % 10; }

	virtual void OnTimer(DWORD dwEventID);
	virtual void SetDirty(ICanvas* pCanvas, const POINT& ptViewTop);
	virtual void OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0);
};
//------------------------------------------------------------------------
// 地上角色的脚印
class CMagicFootprint : public CMagicLife
{
	int		m_nDirection; // 脚印的方向

public:
	CMagicFootprint() : m_nDirection(0) {}

	virtual BOOL OnCreateItem(const char* buf, int nLen);
	virtual void OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0);
	virtual RECT GetShowRect();
	virtual DWORD GetLayerValue();

	void	SetDirection(int nDir);
};
//------------------------------------------------------------------------
// 地上残留物
class CMagicLife_LeftObj :public CMagicLife
{
public:
	virtual void OnTimer(DWORD dwEventID);
};
//------------------------------------------------------------------------
// 带有3个状态的魔法
class CMagicThreeState : public CMagicLife
{
public:
	int		m_nLoopStartIndex,m_nLoopEndIndex;	// 循环的开始和结束所对应的帧索引
	int		m_nState;

	CMagicThreeState() : m_nLoopStartIndex(-1),m_nLoopEndIndex(-1),m_nState(0) {}
	virtual ~CMagicThreeState() {}
	virtual void OnTimer(DWORD dwEventID);
	virtual BOOL OnCreateItem(const char* buf, int nLen);
};
//------------------------------------------------------------------------
// 象流星雨一样斜着下坠的魔法
class CMagicRain : public CMagicThreeState
{
	POINT	m_ptStepOffset; // 每次偏移量
	int		m_nOffX;
	int		m_nOffY;
	BOOL	m_bMove;
	int		m_nMoveTime;
	POINT	m_ptOldOffset;
public:
	CMagicRain() { (__int64&)m_ptStepOffset = 0; (__int64&)m_ptOldOffset = 0; m_bMove = FALSE; m_nMoveTime = 0;}
	virtual ~CMagicRain(){
		g_pTimeAxis->KillTimer(1, static_cast<ITimerSink*>(this));}

	virtual void OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0);
	virtual RECT GetShowRect();
	virtual void OnTimer(DWORD dwEventID);
	virtual BOOL OnCreateItem(const char* buf, int nLen);
	virtual BOOL AddToTimerAxis();
};
//------------------------------------------------------------------------
// 头上飘的字
class CMagicFlowText :public CMagic
{
public:
	virtual BOOL AddToTimerAxis();
	virtual BOOL AddToMapView(POINT& ptTile);
	virtual BOOL RemoveFromTimerAxis();
	virtual BOOL RemoveFromMapView();
	virtual void OnTimer(DWORD dwEventID);
	virtual BOOL OnCreateItem(const char* buf, int nLen);
	void DrawText(ICanvas* pCanvas, int x, int y, int nFrame, DWORD dwFlag, BYTE nAlpha = 0);
	
	int GetBitmapWidthHalf(int nFrame);
};
//------------------------------------------------------------------------
// 新的通用魔法
class CMagicGeneral : public CMagic
{
public:
	CMagicGeneral();
	virtual ~CMagicGeneral();

	virtual void OnTimer(DWORD dwEventID);
	virtual BOOL OnCreateItem(const char* buf, int nLen);
	void	SetLoop(BOOL bLoop);							// 设置是否要循环

	int		m_nLoopStartIndex;	// 循环的开始帧索引
	int		m_nLoopEndIndex;	// 循环的结束帧索引
	BOOL	m_bLoopPart;		// 是否循环局部
	BOOL	m_bLoop;			// 是否循环
};
//------------------------------------------------------------------------
// 范围技能的鼠标光效
class CMagicMouse : public CMagic
{
public:
	CMagicMouse();
	virtual ~CMagicMouse();

	virtual void OnTimer(DWORD dwEventID);
	virtual DWORD GetLayerValue() {return milvLowest-1;}
	virtual RECT GetShowRect();
	virtual BOOL OnCreateItem(const char* buf, int nLen);
	virtual void OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0);

	int		m_nLoopStartIndex;	// 循环的开始帧索引
	int		m_nLoopEndIndex;	// 循环的结束帧索引
	BOOL	m_bLoopPart;		// 是否循环局部
	
	CParticle*	m_pParticle;	// 粒子效果
};
//------------------------------------------------------------------------