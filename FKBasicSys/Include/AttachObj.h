/**
*	created:		2012-7-2   1:59
*	filename: 		AttachObj
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		吸附对象(能够跟着生物一起移动，就像吸附在生物身上一样)
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/Timer/ITimerAxis.h"
#include "../../FKGame/Common/CreatureViewDef.h"
#include "Ani.h"
//------------------------------------------------------------------------
#define MAX_DRAW_FRAMES 5
//------------------------------------------------------------------------
// 吸附对象接口
struct IAttachObj
{
	// 绘图顺序：先画、后画、置顶画
	enum {draw_order_prev=0, draw_order_after, draw_order_topmost};
	enum {stateCreate=0, stateLoop, stateRelease};
	virtual int  GetDrawOrder(int nLoopNumIndex = 0) = NULL;
	virtual RECT GetShowRect(int nLoopNumIndex = 0)	{RECT rc = {0,0,0,0};	return rc;};
	virtual void SetState(int nNewState) = NULL;
	virtual DWORD GetID() = NULL;
	virtual void GetOldDirtyRect(list<RECT> &DirtyRectList, int nLoopNumIndex = 0)	{}
	virtual void SetDirty(ICanvas* pCanvas, const POINT& ptViewTop, const POINT& ptOffset, int nLoopNumIndex = 0) {}
	virtual void OnDraw(ICanvas* pCanvas,
		const POINT& ptTileCenter, DWORD dwFlag = 0, int nLoopNumIndex = 0) = NULL;
	virtual void Release() = NULL;
	virtual DWORD GetAppearanceID() = NULL;
	virtual int GetState() = NULL;
	virtual int GetLoopNum()	{return 1;}
};
//------------------------------------------------------------------------
// 创建缓冲
struct  SAttachObjBuffer
{
	DWORD	dwResID;		// 资源ID
	POINT	ptAnchor;		// 锚点
	int		nDelay;			// 动画延时
	POINT	ptOffset;		// 吸附物所处位置的偏移(脚下为(0,0))
	BOOL	bAddCrtHeight;	// 是否加入生物全高
	int		nDrawOrder;		// 绘图顺序(0-在生物之前绘制，1－在生物之后绘制)
	DWORD	dwAppearanceID;	// 外观ID

	DWORD	dwSoundResID;	// 音效ID
	BOOL	bSoundLoop;		// 音效是否循环
};
//------------------------------------------------------------------------
class CCreatureView;
//------------------------------------------------------------------------
class CAttachObj : public IAttachObj, public ITimerSink
{
public:
	DWORD			m_dwResID;			// 资源ID
	DWORD			m_dwAppearanceID;	// 外观ID
	int				m_nState;			// 当前状态，如启动、循环、销毁等
	int				m_nFrame;			// 当前帧
	int				m_nFrameCount;		// 总帧数
	POINT			m_ptOffset;			// 偏移，踩在脚下时高度为(0,0)
	int				m_nDrawOrder[MAX_DRAW_FRAMES];	// 绘制顺序（相对生物而言）
	int				m_nDrawOrderSave;
	POINT			m_ptAnchor;			// 锚点
	CAni*			m_pPoolNodeObj;		// 包对象
	CCreatureView*	m_pParent;			// 被吸附的生物
	BOOL			m_bNeedSetOldDirty[MAX_DRAW_FRAMES];// 是否需要把旧的矩阵设脏
	int				m_nFrameOld[MAX_DRAW_FRAMES];		// 旧祯
	RECT			m_rcAreaOld[MAX_DRAW_FRAMES];		// 上次画图的矩阵位置
	
	DWORD			m_dwSoundResID;		// 音效ID
	BOOL			m_bSoundLoop;		// 音效是否循环
	int				m_nSoundChunnelID;	// 声音的频道ID
public:
	CAttachObj(CCreatureView* pParent);
	virtual ~CAttachObj();
	void Close();

	// ITimerSink
	virtual void	OnTimer(DWORD dwEventID)  = NULL;

	virtual void	Release()							{delete this;}
	virtual DWORD	GetAppearanceID()					{return m_dwAppearanceID;}
	virtual void	SetState(int nNewState)				{m_nState = nNewState;}
	virtual int		GetState()							{return m_nState;}
	virtual int		GetDrawOrder(int nLoopNumIndex)		{return m_nDrawOrder[nLoopNumIndex];}
	virtual RECT	GetShowRect(int nLoopNumIndex = 0);
	virtual void	GetOldDirtyRect(list<RECT> &DirtyRectList, int nLoopNumIndex = 0);
	virtual void	SetDirty(ICanvas* pCanvas, const POINT& ptViewTop, const POINT& ptOffset, int nLoopNumIndex = 0);
	virtual void	OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0, int nLoopNumIndex = 0);

	CAni*	RequestRes();
	BOOL	Create(const char* buf, int nLen);
	void	SetDrawOrder(int nOrder)					{m_nDrawOrder[0] = nOrder;}
	DWORD	GetID()										{return m_dwResID;}
};
//------------------------------------------------------------------------
// 创建缓冲(3状态吸附对象)
struct SAttachStateObjBuffer : public SAttachObjBuffer
{
	SAttachStateObjBuffer()
	{
		m_nNoShowBegin = m_nNoShowEnd = -1;
		m_nLoopNum = 1;
	}
	int		nLoopTime;			// 循环次数
	int		nLoopStartIndex;	// 动画循环所对应的帧索引,注：这样的动画是将他启动、循环、销毁三个状态的动画打在一起
	int		nLoopEndIndex;		// 动画结束所对应的帧索引

	int		m_nLoopNum;			// 要循环几部分(如火盾通常3部分)默认1部分
	int		m_nNoShowBegin, m_nNoShowEnd;		// 不做显示的开始、结束帧(例如火盾转到人身后变为不可见)
};
//------------------------------------------------------------------------
// 带3个状态的吸附对象(启动、循环、销毁，3个状态的图片位于同一个包中)
class CAttachStateObj : public CAttachObj  
{
public:
	int		m_nLoopTime;						// 循环次数
	int		m_nLoopStartIndex,m_nLoopEndIndex;	// 循环的开始和结束所对应的帧索引
	int		m_nLoopNum;							// 要循环几部分(如火盾通常3部分)
												// 默认1部分

	int		m_nNoShowBegin, m_nNoShowEnd;		// 不做显示的开始、结束帧(例如火盾转到人身后变为不可见)
	int		m_nLoopNumIndex[MAX_DRAW_FRAMES];

public:
	CAttachStateObj(CCreatureView* pParent);
	virtual ~CAttachStateObj();
	
	virtual void OnTimer(DWORD dwEventID);
	BOOL Create(const char* buf, int nLen);

	virtual int  GetDrawOrder(int nLoopNumIndex = 0) {
		return m_nDrawOrder[nLoopNumIndex];}
	virtual RECT GetShowRect(int nLoopNumIndex = 0);
	virtual void SetDirty(ICanvas* pCanvas, const POINT& ptViewTop, const POINT& ptOffset, int nLoopNumIndex = 0);
	virtual void OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0, int nLoopNumIndex = 0);

	virtual int GetLoopNum()	{return m_nLoopNum;}

private:
	int m_nLoopNumSave;
};
//------------------------------------------------------------------------
class CFlowText : public IAttachObj, public ITimerSink
{
private:
	POINT			m_ptBeginPos;		// 起始位置
	POINT			m_ptPos;			// 当前位置
	POINT			m_ptOffset;			// 每次的偏移量
	enFlowType		m_nFlowType;		// 飘的类型
	int				m_nFlowHeight;		// 飘流高度
	CCreatureView*	m_pParent;			// 被吸附的生物
	float			m_a;
	int				m_t;
	BYTE			m_nAlpha;
	int				m_nCurFrame;
	enNumType		m_nNumType;
	int				m_nSign;
	int				m_nDelay;
	BOOL			m_bBeginDraw;

	int				m_nNumSize;			// 数字的位数
	int				m_Num[32];			// 要飘的数字
public:
	// 创建缓冲
	struct  SFlowTextBuffer
	{
		POINT		ptPos;				// 初始创建位置
		int			nDelay;				// 动画延时
		POINT		ptOffset;			// 动画每次的偏移量
		int			nNum;				// 文字内容
		enFlowType	nFlowType;			// 飘的类型
		enNumType	nNumType;			// 飘的类型
	};
	
	CFlowText(CCreatureView* pParent);
	virtual ~CFlowText();
	void Close();

	BOOL Create(const char* buf, int nLen);
	// ITimerSink
	virtual void OnTimer(DWORD dwEventID);
	virtual int  GetDrawOrder(int nLoopNumIndex = 0)	{return IAttachObj::draw_order_topmost;}
	virtual void SetState(int nNewState){};
	virtual DWORD GetID()				{return 0;}
	virtual DWORD GetAppearanceID()		{return -1;}
	virtual int GetState()				{return -1;}
	virtual void OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0, int nLoopNumIndex = 0);
	virtual void Release()				{delete this;}
};
//------------------------------------------------------------------------
class CBehitMagic : public CAttachObj
{
public:
	CBehitMagic();
	CBehitMagic(CCreatureView* pParent);
	
	BOOL Create(const char* buf, int nLen);
	virtual void OnTimer(DWORD dwEventID);
};
//------------------------------------------------------------------------