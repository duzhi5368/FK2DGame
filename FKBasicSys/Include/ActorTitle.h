/**
*	created:		2012-7-2   3:49
*	filename: 		ActorTitle
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		玩家称号管理
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define COLORSIGN	'#'
#define MAX_TITLE_LENGTH	36
//------------------------------------------------------------------------
// 字符上下文
struct SCharConext
{
	char wChar[3]; // 字符
	WPixel color;
	int	nWidth; // 字符的象素宽度
};
//------------------------------------------------------------------------
typedef vector<SCharConext>	NodeList;
typedef vector<WPixel>		ColorList;
//------------------------------------------------------------------------
// 称号效果
// 基类
class CTitleEffect
{
public:
	virtual BOOL	Create(NodeList& CharList)	{return FALSE;}
	virtual void	Close()		{}
	virtual void	OnDraw(ICanvas* pCanvas, POINT& ptBottomCenter, NodeList& CharList)	{}
	virtual int		GetWidth()	{return 0;}
	virtual int		GetHeight() {return 0;}
	virtual void	Release()	{Close();delete this;}

protected:
	void DrawRect(ICanvas* pCanvas, RECT& rc, WPixel clr);
	void DrawBox(ICanvas* pCanvas, RECT& rc, WPixel clr, int alpha);
};
//------------------------------------------------------------------------
// 正常的无效果称号
class CTitleEffect0 : public CTitleEffect
{
public:
	CTitleEffect0();

	virtual BOOL	Create(NodeList& CharList);
	virtual void	OnDraw(ICanvas* pCanvas, POINT& ptBottomCenter, NodeList& CharList);
	virtual int		GetWidth()	{return m_nWidth;}
	virtual int		GetHeight() {return m_nHeight;}

protected:
	int m_nWidth;
	int m_nHeight;
	int m_nCharCount;
	ITTFont* m_pFont;
};
//------------------------------------------------------------------------
// 矩形效果
class CTitleEffect1 : public CTitleEffect0
{
public:
	CTitleEffect1();

	virtual void OnDraw(ICanvas* pCanvas, POINT& ptBottomCenter, NodeList& CharList);
};
//------------------------------------------------------------------------
// 文字陆续出现效果
class CTitleEffect2 : public CTitleEffect0, public ITimerSink
{
public:
	CTitleEffect2();
	virtual ~CTitleEffect2();
	
	virtual BOOL Create(NodeList& CharList);
	virtual void Close();
	virtual void OnDraw(ICanvas* pCanvas, POINT& ptBottomCenter, NodeList& CharList);
	virtual void OnTimer(DWORD dwEventID);

protected:
	int m_nTimerCount;
	int m_nState;
};
//------------------------------------------------------------------------
// 文字陆续变色效果
class CTitleEffect3 : public CTitleEffect0, public ITimerSink
{
public:
	CTitleEffect3();
	virtual ~CTitleEffect3();

	virtual BOOL Create(NodeList& CharList);
	virtual void Close();
	virtual void OnDraw(ICanvas* pCanvas, POINT& ptBottomCenter, NodeList& CharList);
	virtual void OnTimer(DWORD dwEventID);

protected:
	int m_nTimerCount;
};
//------------------------------------------------------------------------
// 称号对象
class CTitleObj
{
public:
	CTitleObj() : pTitleEffect(NULL) {}

	void Close();
	void OnDraw(ICanvas* pCanvas, POINT& ptBottomCenter);

public:
	NodeList		CharList;
	CTitleEffect*	pTitleEffect;
};
//------------------------------------------------------------------------
// 角色称号
class CActorTitle  
{
public:
	CActorTitle();
	virtual ~CActorTitle();

	int		AddTitle(LPCSTR szFormatStr, DWORD dwEffectID);
	void	OnDraw(ICanvas* pCanvas, POINT& ptBottomCenter);
	void	ClearTitle();
	int		ParseFormatString(LPCSTR szFormatStr, CTitleObj& to);

public:
	vector<CTitleObj>	m_TitleList;
	static ColorList	m_ColorList;

};
//------------------------------------------------------------------------
// 效果工厂
class CTitleEffectFactory
{
public:
	CTitleEffect* CreateTitleEffect(DWORD dwEffectID);
	
	CTitleEffect* CreateEffect0();
	CTitleEffect* CreateEffect1();
	CTitleEffect* CreateEffect2();
	CTitleEffect* CreateEffect3();

public:
	typedef CTitleEffect* (CTitleEffectFactory::*ProcCreator)();
	static ProcCreator m_fnCreateEffect[];
};
//------------------------------------------------------------------------