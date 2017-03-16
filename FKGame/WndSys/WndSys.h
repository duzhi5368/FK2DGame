/**
*	created:		2012-6-22   0:28
*	filename: 		WndSys
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define _USE_IME_EX
//------------------------------------------------------------------------
#pragma warning( disable : 4251 )
//------------------------------------------------------------------------
#include "../Timer/ITimerAxis.h"
#include "../../FKCanvas/IFKCanvas.h"
#include <set>
#include <list>
#include <hash_map>
#include <string>
#include <vector>
//------------------------------------------------------------------------
using stdext::hash_map;
using std::string;
using std::vector;
using std::list;
//------------------------------------------------------------------------
// 连接方式定义
//#if defined(WNDSYS_STATICLIB) || defined(_LIB)
//	#define WNDSYS_API
//	#if !defined(_LIB)
//		#ifdef _DEBUG
//			#pragma comment (lib,"FKWndSysStatic_D.lib")
//		#else
//			#pragma comment (lib,"FKWndSysStatic.lib")
//		#endif
//	#endif
//#elif defined(WNDSYS_EXPORTS)
#ifdef FKWNDSYS_EXPORTS
	#define WNDSYS_API __declspec(dllexport)
#else
	#define WNDSYS_API __declspec(dllimport)
	#ifdef _DEBUG
		#pragma comment (lib,"../../Bin/BinGame/Debug/FKWndSys_D.lib")
	#else
		#pragma comment (lib,"../../Bin/BinGame/Release/FKWndSys.lib")
	#endif
#endif
//------------------------------------------------------------------------
#ifndef TraceLn
	#ifdef _DEBUG
		#define TraceLn g_DefaultTrace.ThreadSafeDebugPrintLn
		#define TraceColorLn g_DefaultTrace.DebugPrintColorLn
	#else
		#define TraceLn __noop
		#define TraceColorLn __noop
	#endif
#endif

#ifndef	MY_TRACE_COLOR
#define MY_TRACE_COLOR		RGB(146,0,123)
#endif
//------------------------------------------------------------------------
// 窗口系统版本
#ifndef WNDSYS_VERSION
#define WNDSYS_VERSION 101
#endif
//------------------------------------------------------------------------
#define WNDMGR_SCORLLBAR_FILE	"Common\\ScrollBar\\ScrollBar.bmp"
//------------------------------------------------------------------------
/*
 *	信号连接宏
 */
// 无参数信号
#define CONN_0(theClass, memberFxn)							new Connection0<theClass>(this, &theClass::memberFxn)
// 单参数信号(类型:DWORD，所有的带参数信号最终将他的实际参数的地址转换为DWORD传出)
#define CONN_1(theClass, memberFxn)							new Connection1<theClass>(this, &theClass::memberFxn)
// 单击信号，使用于所有窗口(无参数)
#define CONN_clicked(classPtr, theClass, memberFxn)			classPtr->s_clicked.connect(CONN_0(theClass, memberFxn));
// 回车键信号，使用于XEdit(无参数)
#define CONN_returnPressed(classPtr, theClass, memberFxn)	classPtr->s_returnPressed.connect(CONN_0(theClass, memberFxn));
// Radio被选中信号，使用于XRadio(无参数)
#define CONN_selected(classPtr, theClass, memberFxn)		classPtr->s_selected.connect(CONN_0(theClass, memberFxn));
// 鼠标进入信号，使用于所有窗口(参数转换: XWindow* pWnd=(XWindow*)dwParam)
#define CONN_mouseEnter(classPtr, theClass, memberFxn)		classPtr->s_mouseEnter.connect(CONN_1(theClass, memberFxn));
// 鼠标退出信号，使用于所有窗口(参数转换: XWindow* pWnd=(XWindow*)dwParam)
#define CONN_mouseExit(classPtr, theClass, memberFxn)		classPtr->s_mouseExit.connect(CONN_1(theClass, memberFxn));
// 索引选中信号，使用于XRadioGroup(参数转换: int nIndex=*(int*)dwParam)
#define CONN_indexChecked(classPtr, theClass, memberFxn)	classPtr->s_indexChecked.connect(CONN_1(theClass, memberFxn));
// 位置改变信号，使用于XSlideBar,XScrollBar(参数转换: int nPos=*(int*)dwParam)
#define CONN_posChanged(classPtr, theClass, memberFxn)		classPtr->s_posChanged.connect(CONN_1(theClass, memberFxn));
// 选择改变信号，使用于XListBox(参数转换: int nIndex=*(int*)dwParam)
#define CONN_selChanged(classPtr, theClass, memberFxn)		classPtr->s_selChanged.connect(CONN_1(theClass, memberFxn));
// 选中信号，使用于XCheckBox(参数转换: BOOL bChecked=*(BOOL*)dwParam)
#define CONN_checked(classPtr, theClass, memberFxn)			classPtr->s_checked.connect(CONN_1(theClass, memberFxn));
// 文本改变信号，使用于XEdit(参数转换: LPCSTR lpStr=(LPCSTR)dwParam)
#define CONN_textChanged(classPtr, theClass, memberFxn)		classPtr->s_textChanged.connect(CONN_1(theClass, memberFxn));
// URL信号，使用于XLinkText(参数转换: LPCSTR lpURL=(LPCSTR)dwParam)
#define CONN_runURL(classPtr, theClass, memberFxn)			classPtr->s_runURL.connect(CONN_1(theClass, memberFxn));
// 输入法改变信号
#define CONN_imeChanged(classPtr, theClass, memberFxn)		classPtr->s_imeChanged.connect(CONN_1(theClass, memberFxn));
//------------------------------------------------------------------------
// 用于信号定义的关键字
#define Signals	public
//------------------------------------------------------------------------
// 窗口系统没有处理某条消息时返回这个值，供其他系统继续处理
#define NOTHANDLE	(DWORD)(-1)	
//------------------------------------------------------------------------
// 无效窗口
#define INVALID_XWND	((XWindow*)0xFFFFFFFF)
//------------------------------------------------------------------------
// 光标类型(Cursor Type)
#define CT_ARROW	0	// 箭头
#define CT_HAND		1	// 手型
#define CT_BEAN		2	// I字型
#define CT_VSIZE	3	// 垂直变化尺寸
//------------------------------------------------------------------------
// 窗口特性(XWindow Traits)
#define XWT_VISIBLE			0x0001	// 是否可见
#define XWT_ENABLED			0x0002	// 是否使能
#define XWT_MODAL			0x0004	// 是否模态
#define XWT_MASK			0x0008	// 是否不绘制透明色
#define XWT_MOVABLE			0x0010	// 是否可以移动(不能超越父窗口)
#define XWT_TOPMOST			0x0040	// 是否顶层
#define XWT_ACTIVE			0x0080	// 是否激活
#define XWT_ESCAPE			0x0100	// 是否响应ESC键
#define XWT_GENERAL			0x0200	// 是否是通用窗口
//------------------------------------------------------------------------
// 获取某点下面的子窗口时的过滤选项
#define GCWP_ALL             0x0000		// 不跳过任何窗口
#define GCWP_SKIPINVISIBLE   0x0001		// 跳过不可见的窗口
#define GCWP_SKIPDISABLED    0x0002		// 跳过无效状态的窗口
#define GCWP_SKIPTRANSPARENT 0x0004		// 跳过透明的窗口
//------------------------------------------------------------------------
// 对齐方式 (Align Style)
#define AS_LEFT				0
#define AS_CENTER			1
#define AS_RIGHT			2
//------------------------------------------------------------------------
// 鼠标键定义(OnMouseDown等函数中的nButon输出下面的值)
enum MouseKeys
{
	LeftButton=1,	// 鼠标左键
	RightButton=2,	// 鼠标右键
	MiddleButton=4,	// 鼠标中键
};
//------------------------------------------------------------------------
// 鼠标特殊键状态
enum MouseKeyState
{
	LeftButtonDown=1,	// 鼠标左键处于按下状态
	RightButtonDown=2,	// 鼠标右键处于按下状态
	MiddleButtonDown=4,	// 鼠标中键处于按下状态
	ShiftKeyDown=0x10,
	CtrlKeyDown=0x20,
	AltKeyDown=0x40,
};
//------------------------------------------------------------------------
// 特殊键定义(OnMouseDown等函数中的nShift输出下面的值)
enum ShiftKeys
{
	ShiftKey=1,		// Shift键
	CtrlKey=2,		// Ctrl键
	AltKey=4,		// Alt键
};
//------------------------------------------------------------------------
// 目前支持的窗口类型
enum WindowType
{
	WT_UNKNOW=-1,
	WT_DESKTOP,
	WT_MAINWND,
	WT_WINDOW,
	WT_CONTROL,
	WT_BUTTON,
	WT_STATEBUTTON,
	WT_CHAT,
	WT_CHECKBOX,
	WT_EDIT,
	WT_LABEL,
	WT_LISTBOX,
	WT_PROGRESSBAR,
	WT_RADIO,
	WT_SCROLLBAR,
	WT_SLIDEBAR,
	WT_STATICBITMAP,
	WT_TOOLTIP,
	WT_LINKTEXT,
	WT_ITEMCONTAINER,
	WT_HTML,
	WT_TALK,
	WT_ANIMATE,
};
//------------------------------------------------------------------------
// 向前声明
class SlotHolder;
class CImageLoader;
class XFontCreate;
class XCursor;
class XWindow;
class XAccelTable;
class XDesktop;
class XMainWnd;
class XGeneralMainWnd;
class XControl;
class XButton;
class XStateButton;
class XLabel;
class XRadio;
class XRadioGroup;
class XCheckBox;
class XEdit;
class XSlideBar;
class XScrollBar;
class XProgressBar;
class XStaticBitmap;
class XListBox;
class XLinkText;
class XToolTip;
class XItem;
class XItemText;
class XItemLinkText;
class XItemImage;
class XItemLinkImage;
class XItemContainer;
class XLineParser;
class XChat;
class XHtml;
class XAnimate;
class XTalk;
class CWndSysHelper;
//------------------------------------------------------------------------
struct IBitmap;
struct IFileSystem;
struct IResObject;
//------------------------------------------------------------------------
// 信号槽相关
class WNDSYS_API BasicConnection
{
public:
    virtual SlotHolder* getSlotHolder() const = 0;
	virtual void release() = 0;
};
//------------------------------------------------------------------------
class WNDSYS_API BasicConnection0 : public BasicConnection
{
public:
    virtual void trigger() = 0;
};
//------------------------------------------------------------------------
class WNDSYS_API BasicConnection1 : public BasicConnection
{
public:
    virtual void trigger(DWORD) = 0;
};
//------------------------------------------------------------------------
class WNDSYS_API BasicSignal
{
public:
    virtual void disconnectSlot(SlotHolder* slot) = 0;
	virtual void disconnectAll() = 0;
};
//------------------------------------------------------------------------
// 槽拥有者
class WNDSYS_API SlotHolder
{
private:
    typedef std::set<BasicSignal*>		SenderSet;
    typedef SenderSet::const_iterator	const_iterator;
public:
    SlotHolder() {}
    virtual ~SlotHolder();
    void connectTo(BasicSignal* sender);
    void disconnectFrom(BasicSignal* sender);
    void disconnectAll();
private:
    SenderSet _senders;
};
//------------------------------------------------------------------------
template<class TargetType>
class WNDSYS_API Connection0 : public BasicConnection0
{
public:
    Connection0() : _object(0), _voidMemberFunction(0) {}
    Connection0(TargetType* anObject, void (TargetType::*aMemberFunction)()) : _object(anObject), _voidMemberFunction(aMemberFunction) {}
    virtual void trigger()						{(_object->*_voidMemberFunction)();}
    virtual SlotHolder* getSlotHolder() const	{return _object;}
	virtual void release()						{delete this;}
private:
    TargetType* _object;
    void (TargetType::* _voidMemberFunction)();
};
//------------------------------------------------------------------------
template<class TargetType>
class WNDSYS_API Connection1 : public BasicConnection1
{
public:
    Connection1() : _object(0),_voidMemberFunction(0) {}
    Connection1(TargetType* anObject, void (TargetType::*aMemberFunction)(DWORD)) : _object(anObject), _voidMemberFunction(aMemberFunction) {}
    virtual void trigger(DWORD a1)				{(_object->*_voidMemberFunction)(a1);}
    virtual SlotHolder* getSlotHolder() const	{return _object;}
	virtual void release()						{delete this;}
private:
    TargetType* _object;
    void (TargetType::* _voidMemberFunction)(DWORD);
};
//------------------------------------------------------------------------
class WNDSYS_API Signal0 : public BasicSignal
{
protected:
    typedef std::list<BasicConnection0 *>  ConnectionList;
    ConnectionList _connectedSlots;
public:
    Signal0() {}
	virtual ~Signal0();
    virtual void disconnectAll();
    void disconnectSlot(SlotHolder* slot);
    void connect(BasicConnection0* conn);
    void emit();
    void operator()();
};
//------------------------------------------------------------------------
class WNDSYS_API Signal1 : public BasicSignal
{
protected:
	typedef std::list<BasicConnection1*>  ConnectionList;
    ConnectionList _connectedSlots;
public:
    Signal1() {}
	virtual ~Signal1();
    virtual void disconnectAll();
    void disconnectSlot(SlotHolder* slot);
    void connect(BasicConnection1* conn);
    void emit(DWORD dwParam);
    void operator()(DWORD dwParam);
};
//------------------------------------------------------------------------
// 图象加载
// 目前支持bmp,tga的加载，此加载器是一个单实例的对象，通过此对象加载的图片
// 要调用本对象来进行释放
class WNDSYS_API CImageLoader  
{
public:
	struct stCacheImage					// 缓冲图片
	{
		IBitmap*	pImage;				// 缓冲位片的IBitmap接口指针
		int			nRefCount;			// 缓冲位图的引用计数
	};
private:
	typedef hash_map<string, stCacheImage>				CacheImageList;
	typedef hash_map<string, stCacheImage>::iterator	CacheImageListPtr;
	static CImageLoader* _instance;		// 全局图片加载器实例
	ICanvasHelper		m_CanvasHelper;	// 创建图片的辅助类
	CacheImageList		m_vImageList;	// 图片缓冲列表
	IFileSystem*		m_pFS;			// 文件系统指针
public:
	CImageLoader();
	~CImageLoader();
	
	static CImageLoader*	GetInstance();
	static void				ReleaseInstance();
	
	void     Clear();
	IBitmap* LoadImage(const char* szFileName);
	IBitmap* CopyImage(IBitmap* pImage);
	void	 FreeImage(const char* szFileName);
	void	 FreeImage(IBitmap* pImage);
	BOOL	 SetFileSystem(IFileSystem* pFS);
	IFileSystem* GetFileSystem(){return m_pFS;}
	
private:
	IBitmap* _loadImage(const char* szFileName);
	BYTE * __DecodeJpeg(BYTE *pData, int nLen);     //解码一段JPEG数据,返回一段缓冲区
};
//------------------------------------------------------------------------
// 图象加载
// 目前支持bmp,tga的加载，此加载器是一个单实例的对象，通过此对象加载的图片
// 要调用本对象来进行释放
// 这个类跟上面的完全一样(有点变态)
class WNDSYS_API CImageLoaderEx
{
public:
	struct stCacheImage			// 缓冲图片
	{
		IBitmap*	pImage;		// 缓冲位片的IBitmap接口指针
		int			nRefCount;	// 缓冲位图的引用计数
	};
	
	private:
		typedef hash_map<string, stCacheImage>				CacheImageList;
		typedef hash_map<string, stCacheImage>::iterator	CacheImageListPtr;
		static CImageLoaderEx* _instance;		// 全局图片加载器实例
		ICanvasHelper		m_CanvasHelper;	// 创建图片的辅助类
		CacheImageList		m_vImageList;	// 图片缓冲列表
		IFileSystem*		m_pFS;			// 文件系统指针
	public:
		CImageLoaderEx();
		~CImageLoaderEx();
		
		static CImageLoaderEx*	GetInstance();
		static void				ReleaseInstance();
		
		void     Clear();
		IBitmap* LoadImage(const char* szFileName);
		IBitmap* CopyImage(IBitmap* pImage);
		void	 FreeImage(const char* szFileName);
		void	 FreeImage(IBitmap* pImage);
		BOOL	 SetFileSystem(IFileSystem* pFS);
		IFileSystem* GetFileSystem(){return m_pFS;}
		
	private:
		IBitmap* _loadImage(const char* szFileName);
};
//------------------------------------------------------------------------
// 字体创建器，单实例对象
// 通过此对象创建的字体，请通过此对象的相应方法进行释放
class WNDSYS_API XFontCreate
{
public:
	struct stFontInfo	// 字体信息
	{
		enum {max_fontname_len=16};
		char szName[max_fontname_len];	// 字体名
		int  nSize;						// 字体尺寸(字符的真实尺寸)
		int  bBlod;						// 是否粗体
		int  bItalic;					// 是否斜体
		int  bUnderline;				// 是否有下划线
		int  bWithOutline;				// 是否带轮廓
		int  nCacheCharSize;			// 要缓冲该字体的字符数目(目前在比较时忽略)
		
		stFontInfo()		{memset(this,0,sizeof(stFontInfo));}
		BOOL operator == (const stFontInfo& info) const;
	};
	struct stCacheFont			// 缓冲字体
	{
		stFontInfo	info;		// 缓冲字体的信息
		HFONT		hFont;		// 缓冲字体的句柄
		ITTFont*	pFont;		// 缓冲字体的ITTFont接口指针
		int			nRefCount;	// 缓冲字体的引用计数
	};
	typedef vector<stCacheFont>				CacheFontList;
	typedef vector<stCacheFont>::iterator	CacheFontListPtr;

private:
	static XFontCreate* _instance;		// 全局字体创建实例
	ITTFont*			m_pDefFont;		// 系统默认字体
	ICanvasHelper		m_CanvasHelper;	// 创建字体的辅助类
	CacheFontList		m_vFontList;	// 各种字体缓冲列表
	
public:
	XFontCreate();
	~XFontCreate();
	
	static XFontCreate* GetInstance();
	static void			ReleaseInstance();
	
	void     Clear();
	ITTFont* GetDefaultFont()					{return m_pDefFont;}
	ITTFont* CreateFont(const char* szFontName, int nFontSize, BOOL bBlod = FALSE, 
		BOOL bItalic = FALSE, BOOL bUnderLine = FALSE, 
		BOOL bWithOutline = FALSE, int nCacheCharSize = 128);
	void	ReleaseFont(const char* szFontName, int nFontSize, BOOL bBlod = FALSE, 
		BOOL bItalic = FALSE, BOOL bUnderLine = FALSE, 
		BOOL bWithOutline = FALSE, int nCacheCharSize = 128);
	void	ReleaseFont(ITTFont* pFont);
	XFontCreate::stFontInfo* XFontCreate::GetFontInfo(ITTFont* pFont);
private:
	BOOL _createFont(stCacheFont& cachefont);
};
//------------------------------------------------------------------------
// 光标(注，这个类还需要改造，里面有一堆堆的垃圾)
class WNDSYS_API XCursor : public ITimerSink
{
private:
	IBitmap*	m_pWndBitmap;		// 位图指针(大多是背景)
	RECT		m_rcWnd;			// 窗口边框
	int			m_nAlpha;			// Alpha值
	BOOL		m_bMask;			// 是否透底
	BOOL		m_bAnimate;			// 是否是动画光标
	int			m_nInterval;		// 动画间隔时间
	int			m_nFrames;			// 帧数
	POINT		m_ptHot;			// 热点
	int			m_nCurrentFrame;	// 当前帧
	POINT		m_ptCursor;			// 当前帧在源位图上的坐标点

public:
	static IBitmap*		m_pAttachWnd;			// 鼠标吸附的图片，指那些物品之类的东东
	static DWORD		m_dwUserData;			// 对鼠标设置的附加数据
	static BOOL			m_bUseOwnCursor;		// 是否自绘光标
	
public:
	XCursor();
	virtual ~XCursor();
    virtual void Release();

	// ITimeSink
	virtual void OnTimer(DWORD dwEventID);
	virtual IBitmap* SetBitmap(IBitmap* pBmp){
		IBitmap* pBmpOld = m_pWndBitmap;
		m_pWndBitmap = pBmp;
		return pBmpOld;
	}
	virtual IBitmap* GetBitmap()			{return m_pWndBitmap;}
	virtual void SetAlpha(int nAlpha)		{m_nAlpha = nAlpha;}
	virtual void SetMask(BOOL bMask = TRUE)	{m_bMask = bMask;}
	virtual void SetWindowPos(RECT& rc){CopyRect(&m_rcWnd, &rc);}
	virtual void GetWindowRect(LPRECT lpRect) {CopyRect(lpRect, &m_rcWnd);}
	virtual void SetAnimate(BOOL bAnimate)	{m_bAnimate = bAnimate;}
	virtual void SetInterval(int nInterval) {m_nInterval = nInterval;}
	virtual void SetFrames(int nFrames)		{m_nFrames = nFrames;}
	virtual void SetHots(POINT& pt)			{m_ptHot = pt;}
	virtual POINT GetHots()					{return m_ptHot;}
	virtual void InvalidRect(LPRECT lpRect = NULL);
	virtual void GetMouseXY(POINT& pt);
	virtual void SetMouseXY(POINT& pt);
	virtual void SetAttach(IBitmap* pAttachWnd);
	virtual IBitmap* GetAttach() {return XCursor::m_pAttachWnd;}
	virtual void SetUserData(DWORD dwUserData) {XCursor::m_dwUserData=dwUserData;}
	virtual DWORD GetUserData() {return XCursor::m_dwUserData;}
	
	virtual void OnDraw(ICanvas* pCanvas);
	virtual DWORD OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual DWORD MoveTo(POINT pt);
	virtual void Show(BOOL bShow = TRUE);

	void SetCursorOwnDraw(BOOL bOwnDraw);
};
//------------------------------------------------------------------------
// 窗口基类，所有窗口都继承了SlotHolder(槽拥有者)
class WNDSYS_API XWindow : public SlotHolder
{
public:
	typedef list<XWindow*>						_wnd_list;
	typedef	list<XWindow*>::iterator			_wnd_it;
	typedef list<XWindow*>::reverse_iterator	_wnd_rit;
	_wnd_list	m_lstWnds;
protected:
	string			m_strCaption;	// 窗口标题
	IBitmap*		m_pWndBitmap;	// 位图指针(大多是背景)
	RECT			m_rcWnd;		// 窗口边框
	int				m_nAlpha;		// Alpha值
	int				m_nID;			// 用于标识自己的ID号(实际是资源对象的索引号，非ID号)
	
	XWindow*		m_pParent;		// 父窗口
	XWindow*		m_pFocusChild;	// 拥有焦点的子窗口

	ITTFont*		m_pSelfFont;	// 窗口私有字体
	DWORD			m_dwWndTraits;	// 窗口特性

	WPixel			m_cForeColor, m_cBackColor;	// 前景和背景颜色
	int				m_nAlign;		// 对齐方式
	IResObject*		m_pResObj;		// 窗口脚本对象
Signals:
	Signal0			s_clicked;		// 单击信号
	int             m_nToolTipIndex;// ToolTip索引
public:
	XWindow();
	XWindow(XWindow* pParentWnd);
	virtual ~XWindow();

    virtual void Release();

	// 系统属性相关
	XDesktop*  GetDesktop();
	ICanvas*   GetCanvas();
	ITimeAxis* GetTimeAxis();
	ITTFont*   GetSysFont();

	// 属性相关
	ITTFont* SetFont(ITTFont* pNewFont);
	ITTFont* GetFont();

	XWindow* SetFocus();
	XWindow* GetFocus();
	BOOL     IsFocus();
	XWindow* GetGlobalFocus();	// 窗口系统有一个全局的焦点，他实际就是活动主窗口的焦点
	BOOL     IsGlobalFocus();

	XWindow* SetCapture();
	XWindow* GetCapture();
	BOOL     IsCapture();
	void     ReleaseCapture();

	XWindow* SetMouseMove();
	XWindow* GetMouseMove();
	BOOL	 IsMouseMove();

	XWindow* SetActive();
	XWindow* GetActive();
	BOOL	 IsActive();
	
	int      SetCursor(int nType = CT_ARROW);
	XCursor* GetCursor();

	void	WritePoint(POINT pt);
	POINT	ReadPoint();
	void	WriteRect(RECT& rc);
	RECT&	ReadRect();

	BOOL IsMainWindow() {return GetWindowType()==WT_MAINWND;}
	BOOL IsDesktop()	{return GetWindowType()==WT_DESKTOP;}
	BOOL IsControl()	{return GetWindowType()==WT_CONTROL;}

	virtual BOOL IsTransparent(POINT& pt);

	// 窗口特性
	virtual void SetVisible(BOOL bVisible = TRUE);
	virtual void SetEnabled(BOOL bEnabled = TRUE);
	void SetMask(BOOL bMask = TRUE);
	void SetMovable(BOOL bMovable = FALSE);
	void SetModal(BOOL bModal = FALSE);
	void SetTopmost(BOOL bTopmost = FALSE);
	void SetWndTraits(DWORD dwTraits, BOOL bTraits);
	
	BOOL IsVisible()	{return m_dwWndTraits & XWT_VISIBLE;}
	BOOL IsEnabled()	{return m_dwWndTraits & XWT_ENABLED;}
	BOOL IsMask()		{return m_dwWndTraits & XWT_MASK;}
	BOOL IsMovable()	{return m_dwWndTraits & XWT_MOVABLE;}
	BOOL IsModal()		{return m_dwWndTraits & XWT_MODAL;}
	BOOL IsTopmost()	{return m_dwWndTraits & XWT_TOPMOST;}

	int  GetToolTipIndex(void) {return m_nToolTipIndex;}
	void  SetToolTipIndex(int nIndex) {m_nToolTipIndex = nIndex;}
	DWORD GetWndTraits(){return m_dwWndTraits;}

	// 窗口管理相关
	virtual void     AddWindow(XWindow* pWnd);
	virtual void     RemoveWindow(XWindow* pWnd);
	virtual XWindow* FindWindowByID(int nID);
	virtual int      FindWindow(XWindow* pWnd);

	// 初始化相关
	IBitmap* SetBitmap(IBitmap* pBmp);
	IBitmap* GetBitmap();
	void     SetAlpha(int nAlpha);
	int      GetAlpha();
	void	SetBackColor(int r, int g, int b)	{m_cBackColor = WPixel(r,g,b);}
	void	SetBackColor(WPixel color)			{m_cBackColor = color;}
	WPixel	GetBackColor()						{return m_cBackColor;}
	void	SetForeColor(int r, int g, int b)	{m_cForeColor = WPixel(r,g,b);}
	void	SetForeColor(WPixel color)			{m_cForeColor = color;}
	WPixel	GetForeColor()						{return m_cForeColor;}
	void	SetAlign(int nAlign)				{m_nAlign = nAlign;}
	int		GetAlign()							{return m_nAlign;}

	// 坐标相关
	virtual	void SetWindowPos(RECT& rc);
	virtual void GetWindowRect(LPRECT lpRect);
	virtual void GetClientRect(LPRECT lpRect);
	virtual void MoveWindow(int dx, int dy);
	virtual int  GetWidth()						{return m_rcWnd.right - m_rcWnd.left;}
	virtual int  GetHeight()					{return m_rcWnd.bottom - m_rcWnd.top;}
	virtual void InvalidRect(LPRECT lpRect = NULL);

	// 文本相关
	virtual void   SetWindowText(LPCSTR lpszString);
	virtual LPCSTR GetWindowText();

	// 文字提示
	virtual void ToolTipSetInfo(IBitmap* pBitmap, LPCSTR szToolTip, WPixel cBackColor, WPixel cForeColor, int nInterval=0);
	virtual void ToolTipMoveTo(int x, int y);

	// 窗口访问
	virtual void	 SetID(int nID);
	virtual int		 GetID();
	virtual XWindow* GetParent();
	virtual XWindow* SetParent(XWindow* pParent);
	virtual int		 GetWindowType() {return WT_WINDOW;}
	XMainWnd*		 GetMainWnd();

	// 加速键(当注册英文字母时，以大写字母作为键码)
	void RegisterAccelKey(UINT nKeyCode, UINT nShift, XWindow* pWnd);
	void UnRegisterAccelKey(UINT nKeyCode, UINT nShift);
	void UnRegisterAccelKey();

	virtual XWindow* GetChildWindowFromPoint(POINT pt, int nFlags);
	XWindow* GetWindowBelowCursor(POINT pt, int nFlags);

	virtual void SetResObject(IResObject* pObj)	{m_pResObj = pObj;}
	virtual IResObject* GetResObject()			{return m_pResObj;}
	virtual void LoadAllImage();
	virtual void ReleaseAllImage();

public:	// 消息相关
	// 鼠标消息
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt){return NOTHANDLE;}

	// 键盘消息
	virtual DWORD OnKeyDown(UINT nKeyCode, UINT nShift)	{return NOTHANDLE;}
	virtual DWORD OnKeyUp(UINT nKeyCode, UINT nShift)	{return NOTHANDLE;}
	virtual DWORD OnChar(UINT nKeyAscii)				{return NOTHANDLE;}
	virtual void  OnAccelKey(UINT nKeyCode, UINT nShift){}

	// 推出消息
	virtual void PostExitMessage();

	// 焦点消息
	virtual void OnSetFocus()	{}
	virtual void OnKillFocus()	{}

	// 鼠标消息产生的事件
	virtual void OnMouseEnter()	;
	virtual void OnMouseExit()	{}

	// 文本改变
	virtual void OnTextChanged() {}

	// 窗口激活
	virtual void OnActivate(BOOL bActive) {}

	// 绘图消息
	virtual void OnPaint(ICanvas* pCanvas);
	virtual void OnDraw(ICanvas* pCanvas, RECT& rcClip);
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip);

	// 消息处理
	virtual DWORD OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual DWORD DefMessage(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	virtual void DestroyWindow();
	
private:
	XWindow* SetFocusHelper();
};
//------------------------------------------------------------------------
// 快捷键表
class WNDSYS_API XAccelTable  
{
public:
	struct stAccelKey
	{
		UINT		nKeyCode;
		UINT		nShift;
		XWindow*	pWnd;
		
		stAccelKey(UINT Key, UINT Shift, XWindow* Wnd)
			: nKeyCode(Key), nShift(Shift), pWnd(Wnd) {}
	};
	typedef	vector<stAccelKey>				AccelKeys;
	typedef	vector<stAccelKey>::iterator	AccelKeysPtr;
protected:
	AccelKeys	m_AccelKeys;
public:
	XAccelTable();
	virtual ~XAccelTable();
	
	void  AddAccelKey(UINT nKeyCode, UINT nShift, XWindow* pWnd);
	void  RemoveAccelKey(UINT nKeyCode, UINT nShift);
	void  RemoveAccelKey(XWindow* pWnd);
	DWORD HandleAccelKey(UINT nKeyCode, UINT nShift);
};
//------------------------------------------------------------------------
// 桌面，他是说有窗口的直接或间接的父窗口，一个窗口只有一个桌面，他不但拥有
// 窗口的特性，还兼有窗口管理器的功能，并接管整个窗口系统的消息处理和分发
class WNDSYS_API XDesktop : public XWindow
{
public:
	struct stMsg
	{
		UINT	uMsgID;
		WPARAM	wParam;
		LPARAM	lParam;
	};
	typedef list<XWindow*>				_wnd_list;
	typedef	list<XWindow*>::iterator	_wnd_it;
	typedef list<stMsg>					_msg_list;
	typedef	list<stMsg>::iterator		_msg_it;
private:
	HWND			m_hWnd;				// 窗口句柄
	ICanvas*		m_pCanvas;			// 系统画布
	ITimeAxis*		m_pTimeAxis;		// 系统时间轴
	ITTFont*		m_pSysFont;			// 系统字体
	SIZE			m_sizeDesktop;		// 桌面大小

	// 光标相关
	XCursor**		m_ppCursors;		// 所有的光标数组
	int				m_nCursorCount;		// 所有光标的数目
	int				m_nCurrentCursorType;// 当前使用的光标类型

	POINT			m_ptBak;			// 用于备份某些点
	RECT			m_rcBak;			// 用于备份某些矩形

	// 窗口相关
	XWindow*		m_pActiveWnd;		// 当前活动的窗口(仅仅支持主窗口)
	XWindow*		m_pFocusWnd;		// 全局焦点窗口
	XWindow*		m_pCaptureWnd;		// 捕捉窗口
	XWindow*		m_pMouseMoveWnd;	// 鼠标移动窗口

	// 系统资源
	IBitmap*		m_pSysScrollBarBitmap;	// 系统滚动条图片
	XWindow*		m_pToolTip;				// 公用的ToolTip窗口

	_wnd_list		m_lstTopmostWnds;		// topmost 窗口列表

	XAccelTable		m_AccelTable;		// 加速键表
	_msg_list		m_lstMsg;			// 消息队列

	HKL				m_hKeyboardLayoutList[32];		// 键盘布局列表(输入法列表)
	int				m_nKeyboardLayoutListCount;		// 键盘布局数目(输入法数目)
	int				m_nEnImeIndex;					// 英文输入法索引
	int				m_nCurImeIndex;					// 当前输入法索引
	BOOL			m_bRecordLayoutChange;			// 是否记录输入法的改变

	DWORD			m_dwMouseKeyState;				// 鼠标、特殊键的状态

Signals:
	Signal1			s_mouseEnter;	// 鼠标进入信号
	Signal1			s_mouseExit;	// 鼠标退出信号
	Signal1			s_imeChanged;
public:
	static XDesktop*	m_pDesktop;		// 游戏桌面
public:
	XDesktop();
	virtual ~XDesktop();
	virtual int GetWindowType()				{return WT_DESKTOP;}
	virtual void Release();	

	void SetHWnd(HWND hWnd)					{m_hWnd = hWnd;}
	void SetCanvas(ICanvas* pCanvas)		{m_pCanvas = pCanvas;}
	void SetTimeAxis(ITimeAxis* pTimeAxis)	{m_pTimeAxis = pTimeAxis;}
	void SetSysFont(ITTFont* pSysFont)		{m_pSysFont = pSysFont;}
	void SetDesktopSize(SIZE size)			{m_sizeDesktop = size;}
	HWND GetHWnd()							{return m_hWnd;}
	ICanvas* GetCanvas()					{return m_pCanvas;}
	ITimeAxis* GetTimeAxis()				{return m_pTimeAxis;}
	ITTFont* GetSysFont()					{return m_pSysFont;}
	SIZE GetDesktopSize()					{return m_sizeDesktop;}
	static XDesktop* GetInstance();

	XWindow* SetActiveWindow(XWindow* pNewWnd);
	XWindow* SetFocusWindow(XWindow* pNewWnd);
	XWindow* SetCaptureWindow(XWindow* pNewWnd);
	XWindow* SetMouseMoveWindow(XWindow* pNewWnd);
	XWindow* GetActiveWindow()				{return m_pActiveWnd;}
	XWindow* GetFocusWindow()				{return m_pFocusWnd;}
	XWindow* GetCaptureWindow()				{return m_pCaptureWnd;}
	XWindow* GetMouseMoveWindow()			{return m_pMouseMoveWnd;}

	int SetCurrentCursorType(int nType);
	int	GetCurrentCursorType()				{return m_nCurrentCursorType;}
	XCursor* GetCursor(int nType);
	XCursor* GetCurrentCursor()				{return GetCursor(GetCurrentCursorType());}
	int  GetCursorCount()					{return m_nCursorCount;}
	void SetAllCursor(XCursor** ppCursors)	{m_ppCursors = ppCursors;}
	void SetCursorCount(int nCount)			{m_nCursorCount = nCount;}
	BOOL IsCursorOwnDraw()					{return XCursor::m_bUseOwnCursor;}
	void SetCursorOwnDraw(BOOL bOwnDraw);
	DWORD GetMouseKeyState()				{return m_dwMouseKeyState;}

	void SetBakPoint(POINT pt)				{m_ptBak = pt;}
	POINT GetBakPoint()						{return m_ptBak;}
	void SetBakRect(RECT& rc)				{CopyRect(&m_rcBak, &rc);}
	RECT& GetBakRect()						{return m_rcBak;}

	IBitmap* SetSysScrollBarBitmap(IBitmap* pNewBitmap);
	IBitmap* GetSysScrollBarBitmap()		{return m_pSysScrollBarBitmap;}
	XWindow* SetSysToolTip(XWindow* pNewWnd);
	XWindow* GetSysToolTip()				{return m_pToolTip;}
	XAccelTable* GetAccelTable()			{return &m_AccelTable;}

	DWORD DispatchMessage(UINT message, WPARAM wParam, LPARAM lParam);
	DWORD DispatchMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);
	DWORD DispatchKeyMessage(UINT message, WPARAM wParam, LPARAM lParam);
	DWORD DispatchOtherMessage(UINT message, WPARAM wParam, LPARAM lParam);
	void  PostMessage(UINT uMsgID, WPARAM wParam, LPARAM lParam);
	void  SetIMEEnable(BOOL bEnable = TRUE);

// 重载
public:
	// 鼠标消息
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt)	{return NOTHANDLE;}
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt)	{return NOTHANDLE;}
	virtual DWORD OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt)	{return NOTHANDLE;}

	virtual void OnPaint(ICanvas* pCanvas);
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip);

	// 主窗口管理
	virtual void	 AddWindow(XWindow* pWnd);
	virtual void	 RemoveWindow(XWindow* pWnd);
	virtual XWindow* FindWindowByID(int nID);
	virtual int      FindWindow(XWindow* pWnd);

	virtual XWindow* GetChildWindowFromPoint(POINT pt, int nFlags);
	virtual XWindow* GetFirstVisibleEnableWindow();
	
protected:
	virtual void DestroyWindow();
};
//------------------------------------------------------------------------
// 主窗口，所有用户编写的自定义主窗口均继承自本类
class WNDSYS_API XMainWnd : public XWindow  
{
public:
	XMainWnd();
	XMainWnd(XWindow* pParentWnd);
	virtual ~XMainWnd();
	
	virtual int GetWindowType()		{return WT_MAINWND;}
	
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt);
	
	// 键盘消息
	virtual DWORD OnKeyDown(UINT nKeyCode, UINT nShift);
	virtual DWORD OnKeyUp(UINT nKeyCode, UINT nShift);
	virtual DWORD OnChar(UINT nKeyAscii);
	
	virtual void SetVisible(BOOL bVisible = TRUE);
	virtual void SetEnabled(BOOL bEnabled = TRUE);
	virtual BOOL CanEscape();
	virtual void OnEscape();
public:	// 消息相关
	// 窗口激活
	virtual void OnActivate(BOOL bActive);
	virtual void BringToTopLevel();	
protected:
	virtual void DestroyWindow();
};
//------------------------------------------------------------------------
// 主窗口，所有用户编写的自定义主窗口均继承自本类
class WNDSYS_API XGeneralMainWnd : public XMainWnd  
{
public:
	enum {LeftTop=0,Top,RightTop,Left,Right,
		LeftBottom,Bottom,RightBottom,
		SrcBack,Fill,
		CaptionPos,
		MaxAreaCount};
protected:
	struct AREA
	{
		long x,y,w,h;
	};
	AREA m_Pos[MaxAreaCount];
	/*
	位置信息排列顺序：左上、上，右上、左，右，左下、下，
	右下，填充背景的源图区域，要填充的区域
	//*/
	int				m_nBorderAlpha;				 //窗口边框的Alpha值
	int             m_caption_x, m_caption_y;    //标题显示的相对位移
	void CalculateCaptionPos(void);              //计算出标题栏的相对位移
	
public:
	XGeneralMainWnd();
	XGeneralMainWnd(XWindow* pParentWnd);
	virtual ~XGeneralMainWnd();

	virtual void SetBorderAlpha(int alpha);			//设置边框的Alpha值
	virtual int GetBorderAlpha();					//读取边框的Alpha值
	virtual const AREA *GetGraphicsPos();           //读取原图片的各贴图位置

	virtual BOOL InitParamList(long* posList, int nCount);
	virtual BOOL IsTransparent(POINT& pt);
	virtual	void SetWindowPos(RECT& rc);
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip);
	virtual void OnDraw(ICanvas* pCanvas, RECT& rcClip);
	
	virtual void SetWindowText(LPCSTR lpszString); 
};
//------------------------------------------------------------------------
// 控件类，外部自定义的非窗口系统所具有的控件均要继承此类
class WNDSYS_API XControl : public XWindow  
{
public:
	XControl();
	XControl(XWindow* pParentWnd);
	virtual ~XControl();
	
	virtual int GetWindowType() {return WT_CONTROL;}

	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt);
	
	// 键盘消息
	virtual DWORD OnKeyDown(UINT nKeyCode, UINT nShift);
	virtual DWORD OnKeyUp(UINT nKeyCode, UINT nShift);
	virtual DWORD OnChar(UINT nKeyAscii);
};
//------------------------------------------------------------------------
// 信息提示窗口
class WNDSYS_API XToolTip : public XWindow, public ITimerSink
{
protected:
	int		m_nMargin;		// 边框空白
	int		m_nLineSpace;	// 行间距
	int		m_nInterval;	// 显示的时间长度
public:
	XToolTip();
	XToolTip(XWindow* pParentWnd);
	virtual ~XToolTip();
	virtual int GetWindowType()					{return WT_TOOLTIP;}
	
	virtual void OnTimer(DWORD dwEventID);
	
	void SetText(LPCSTR szTooltip)			{XWindow::SetWindowText(szTooltip);}
	void SetLineSpace(int nSpace)			{m_nLineSpace = max(0, nSpace);}
	int  GetLineSpace()						{return m_nLineSpace;}
	void SetInterval(int nInterval)			{m_nInterval = nInterval;}
	int  GetInterval()						{return m_nInterval;}
	
	void CalcPos(LPPOINT lpptPos);
	
	void SetInfo(IBitmap* pBitmap, LPCSTR szToolTip, WPixel cBackColor, WPixel cForeColor, int nInterval);
	void Show(LPPOINT lpptPos);
	void SetNewPos(int x, int y);
	void Hide();
	
	BOOL IsValid()							{return m_pWndBitmap || !m_strCaption.empty();}
	
	virtual BOOL IsTransparent(POINT& pt)	{return FALSE;}
public:
	virtual void OnDraw(ICanvas* pCanvas, RECT& rcClip);
private:
	void LimitedToScreen(LPRECT lpRect); 
	
	//  这个函数用于解决新的格式描述 
	//  解释字符串中含型如[:255,033,056,r]的格式 (不要字符间不能留空格, 每个颜色值要有3位数)
	//  输入: 字符串, WPIXEL指针, 对齐设置指针(int *)
	//  返回: 格式串的大小
	int __PhraseNewFormat(LPCSTR szStr, WPixel *pColor, int *pAlign);

	//  这个函数用于判断是否需要换行
	//  输入: 字符串
	//  返回: 是否遇到换行
	BOOL __isNewLine(LPCSTR szStr);
};
//------------------------------------------------------------------------
// 按钮类
class WNDSYS_API XButton : public XWindow  
{
protected:
	IBitmap*	m_pBmpDown;		// 按钮按下时的图片
	IBitmap*	m_pBmpUp;		// 按钮释放时的图片
	IBitmap*	m_pBmpHover;	// 鼠标悬停时的图片
	IBitmap*	m_pBmpDisable;	// 按钮无效时的图片
	int			m_nState;		// 按钮当前的状态
	
	WPixel      m_cForeColorHover;   //鼠标悬停时标题颜色
	WPixel      m_cForeColorDown;   //鼠标按下时标题颜色
	WPixel      m_cForeColorDisable;   //鼠标按下时标题颜色
	WPixel      m_cForeColorUp;   //鼠标弹起时标题颜色

public:
	XButton();
	XButton(XWindow* pParentWnd);
	virtual ~XButton();
	virtual int GetWindowType()						{return WT_BUTTON;}
	
	enum STATE{BS_UP=0, BS_ROLLOVER, BS_DOWN};
	
public:
	
	void SetBitmapDown(IBitmap* pBmp)	{m_pBmpDown = pBmp;}
	IBitmap* GetBitmapDown();
	void SetBitmapUp(IBitmap* pBmp)		{m_pBmpUp = pBmp;m_pWndBitmap=pBmp;}
	IBitmap* GetBitmapUp()				{return m_pBmpUp;}
	void SetBitmapHover(IBitmap* pBmp)	{m_pBmpHover = pBmp;}
	IBitmap* GetBitmapHover();
	void SetBitmapDisable(IBitmap* pBmp){m_pBmpDisable = pBmp;}
	IBitmap* GetBitmapDisable();
	void SetState(int bsState)			{m_nState = bsState;}
	int  GetState()						{return m_nState;}
	
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt);

	//设置各种情况下的背景色
	void SetDisableForeColor(long *color);
	void SetHoverForeColor(long *color);
	void SetDownForeColor(long *color);
	void SetUpForeColor(long *color);  

	void OnDraw(ICanvas* pCanvas, RECT& rcClip);
	
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	
	virtual void SetEnabled(BOOL bEnabled = TRUE);
	virtual void LoadAllImage();
	virtual void ReleaseAllImage();
protected:
	virtual void DestroyWindow();
};
//------------------------------------------------------------------------
// 状态按钮类
class WNDSYS_API XStateButton : public XWindow
{
protected:
	IBitmap*	m_pBmpState1;	// 状态1对应的图片
	IBitmap*	m_pBmpState2;	// 状态2对应的图片
	int			m_nState;		// 当前状态
	bool		m_bShowState1;	// 是否正在显示状态1
public:
	XStateButton();
	XStateButton(XWindow* pParentWnd);
	virtual ~XStateButton();
	virtual int GetWindowType()						{return WT_STATEBUTTON;}
	
	enum STATE{BS_NORMAL=0, BS_ROLLOVER, BS_PRESSED};
	
public:
	virtual void SetBitmapState2(IBitmap* pBmp)	{m_pBmpState2=pBmp;}
	virtual void SetBitmapState1(IBitmap* pBmp)	{m_pBmpState1=pBmp;m_pWndBitmap=pBmp;}
	virtual void SetState(int nState);
	virtual int GetState()						{return (m_bShowState1)?0:1;}

	virtual void LoadAllImage();
	virtual void ReleaseAllImage();
	
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	
protected:
	virtual void DestroyWindow();
};
//------------------------------------------------------------------------
// 标签类，也叫静态文本，目前仅仅支持单行
class WNDSYS_API XLabel : public XWindow
{
	int			m_nMargin;	// 边框空白
public:
	XLabel();
	XLabel(XWindow* pParentWnd);
	virtual ~XLabel();
	virtual int GetWindowType()					{return WT_LABEL;}
	
	void SetMargin(int nMargin)				{m_nMargin = nMargin;}
	int  GetMargin()						{return m_nMargin;}
	
	// 虚函数重载
	virtual BOOL IsTransparent(POINT& pt)	{return !IsEnabled();}
	
public:
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip);
	virtual void OnDraw(ICanvas* pCanvas, RECT& rcClip);	
};
//------------------------------------------------------------------------
// 单选钮
class WNDSYS_API XRadio : public XWindow
{
protected:
	int				m_nState;
	IBitmap*		m_pBmp;
	XRadioGroup*	m_pGroup;
	
Signals:
	Signal0			s_selected;
public:
	XRadio();
	XRadio(XWindow* pParentWnd);
	virtual ~XRadio();
	virtual int GetWindowType()					{return WT_RADIO;}
	
	virtual void Release();
	
	enum STATE{RS_UNCHECK=0, RS_CHECK};
	
public:
	void SetState(int cbsState = TRUE)		{m_nState = cbsState;}
	int  GetState()							{return m_nState;}
	void SetStateBitmap(IBitmap* pBmp)		{m_pBmp = pBmp;}
	void SetGroup(XRadioGroup* pGroup)		{m_pGroup = pGroup;}
	XRadioGroup* GetGroup()					{return m_pGroup;}

	virtual BOOL IsTransparent(POINT& pt)	{return FALSE;}

	virtual void LoadAllImage();
	virtual void ReleaseAllImage();
public:
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip);
	virtual void OnDraw(ICanvas* pCanvas, RECT& rcClip);
	
protected:
	virtual void DestroyWindow();
};
//------------------------------------------------------------------------
// 单选钮群管理类
class WNDSYS_API XRadioGroup
{
Signals:
	Signal1		s_indexChecked;

public:
	XRadioGroup();
	virtual ~XRadioGroup();
	
	list<XRadio*>	m_lstRadios;
	
	XRadio* GetSelectedRadio();
	void	SelectRadio(XRadio* pRadio);
	int		GetIndexOfRadio(XRadio* pRadio);
	int		AddRadio(XRadio* pRadio);
	void	RemoveRadio(XRadio* pRadio);
	void	Release();
};
//------------------------------------------------------------------------
// 复选框
class WNDSYS_API XCheckBox : public XWindow  
{
protected:
	int			m_nState;
	IBitmap*	m_pBmp;
	BOOL		m_bUseGray;
	
Signals:
	Signal1		s_checked;
public:
	XCheckBox();
	XCheckBox(XWindow* pParentWnd);
	virtual ~XCheckBox();
	virtual int GetWindowType()							{return WT_CHECKBOX;}
	
	enum STATE{CBS_UNCHECK=0, CBS_CHECK};
	
	void SetState(int cbsState = TRUE)				{m_nState = cbsState;}
	int  GetState()									{return m_nState;}
	void SetStateBitmap(IBitmap* pBmp)				{m_pBmp = pBmp;}

	virtual BOOL IsTransparent(POINT& pt)			{return FALSE;}

	virtual void LoadAllImage();
	virtual void ReleaseAllImage();
public:
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual void  OnMouseEnter();
	virtual void  OnMouseExit();
	
	virtual void  OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip);
	virtual void  OnDraw(ICanvas* pCanvas, RECT& rcClip);
	
protected:
	virtual void  DestroyWindow();
};
//------------------------------------------------------------------------
// 编辑框
class WNDSYS_API XEdit : public XWindow, public ITimerSink
{
public:

	enum {LeftTop=0,Top,RightTop,Left,Right,
		LeftBottom,Bottom,RightBottom,
		SrcBack,Fill,
		MaxAreaCount};

	struct Selection		// 选择结构
    {
        int startIndex;
        int endIndex;
    };
	
protected:
	BOOL		m_bPassword;		// 是否是密码框
	BOOL		m_bReadOnly;		// 是否只读
	BOOL		m_bDigitOnly;		// 仅仅数字
	BOOL		m_bDrawBeam;		// 是否画插入符
	BOOL		m_bMultiLine;		// 是否支持多行
	BOOL		m_bAutoWrap;		// 是否自动回行
	int			m_nCaretPos;		// 插入符在字符串中的位置
	POINT		m_ptCaretPos;		// 插入符的像素位置(偏移值)
	int			m_nMaxLen;			// 可容纳字符的最大长度
	Selection	m_sSel;				// 选择的文本
	int			m_nSelStart;		// 选择时的开始位置
	POINT		m_ptOffset;			// EDIT视区相对于EDIT外框的偏移
	char		m_cPasswordChar;	// 用于屏蔽密码的字符
	int			m_nMargin;			// 边缘间隔
	int			m_nLineSpace;		// 行间距
	int			m_nValidWidth;		// 有效宽度
	int			m_nValidHeight;		// 有效高度
	XScrollBar*	m_pHScrollBar;		// 水平滚动条
	XScrollBar*	m_pVScrollBar;		// 垂直滚动条

	BOOL m_bStretch;                 //是否延伸贴图   
	
	struct AREA
	{
		long x,y,w,h;
	};
	AREA m_Pos[MaxAreaCount];
	/*
	位置信息排列顺序：左上、上，右上、左，右，左下、下，
	右下，填充背景的源图区域，要填充的区域
	*/

Signals:
    Signal0		s_returnPressed;
    Signal1		s_textChanged;
public:
	
	XEdit();
	XEdit(XWindow* pParentWnd);
	virtual ~XEdit();
	virtual int GetWindowType()					{return WT_EDIT;}

	virtual BOOL InitParamList(long* posList, int nCount);
public:
	// 属性相关
	const AREA * GetGraphicsPos();
	void SetPassword(BOOL bPassword = TRUE) {m_bPassword = bPassword;}
	BOOL IsPassword()						{return m_bPassword;}
	void SetReadOnly(BOOL bReadOnly = TRUE) {m_bReadOnly = bReadOnly;}
	BOOL IsReadOnly()						{return m_bReadOnly;}
	void SetDigitOnly(BOOL bDigitOnly = TRUE) {m_bDigitOnly = bDigitOnly;}
	BOOL IsDigitOnly()						{return m_bDigitOnly;}
	void SetDrawBeam(BOOL bDrawBeam = TRUE) {m_bDrawBeam = bDrawBeam;}
	BOOL IsDrawBeam()						{return m_bDrawBeam;}
	void SetMultiLine(BOOL bMultiLine = TRUE) {m_bMultiLine = bMultiLine;}
	BOOL IsMultiLine()						{return m_bMultiLine;}
	void SetAutoWrap(BOOL bAutoWrap = TRUE);
	BOOL IsAutoWrap()						{return m_bAutoWrap;}
	void SetCaretPos(int nPos);
	int  GetCaretPos()						{return m_nCaretPos;}
	int  SetMaxLen(int nMaxLen)				{return m_nMaxLen = nMaxLen;}
	int  GetMaxLen()						{return m_nMaxLen;}
	void SetMargin(int nMargin)				{m_nMargin = nMargin;}
	int  GetMargin()						{return m_nMargin;}
	void SetPasswordChar(char c)			{m_cPasswordChar = c;}
	char GetPasswordChar()					{return m_cPasswordChar;}
	void SetHScrollBar(XScrollBar* pBar);
	void SetVScrollBar(XScrollBar* pBar);

	Selection GetSelection()				{return m_sSel;}
	void SetSelection(int nStart, int nEnd);
	void ClearSelection();
	void DeleteSelectedText();
	BOOL IsValidSelection();

	virtual BOOL IsTransparent(POINT& pt)	{return !IsEnabled();}

public: // 操作
	void doSelAll();
	void doCopy();
	void doPaste();
	void doCut();
	void doBackSpace();
	void doDelete();
	void doCrLf();	// 回车换行
	
	void AdjustHScroll();
	void AdjustVScroll();
	
public:
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt);
	virtual void  OnMouseEnter();
	virtual void  OnMouseExit();
	virtual void  OnSetFocus();
	virtual void  OnKillFocus();

	virtual DWORD OnKeyDown(UINT nKeyCode, UINT nShift);
	virtual DWORD OnChar(UINT nKeyAscii);

	virtual void SetWindowText(LPCSTR lpszString);
	virtual void SetWindowPos(RECT& rc);
	
	virtual void OnTimer(DWORD dwEventID);
	virtual void OnDraw(ICanvas* pCanvas, RECT& rcClip);
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip);
	virtual void OnOtherChar(UINT nKeyAscii);

	int  GetValidWidth()	{return m_nValidWidth;}
	void SetValidWidth(int nValidWidth);
	int  GetValidHeight() {return m_nValidHeight;}
	void SetValidHeight(int nValidHeight);

protected:
	int GetCharIndexByOffset(LPCSTR lpStr, POINT pt, BOOL bCmpHalfChar=FALSE, BOOL bCalcCaretPos=FALSE);
	int _GetCharIndexByOffset(char* pFirst, char* pLast, char* pBegin, char* pEnd, POINT& pt, BOOL bCmpHalfChar, BOOL bCalcCaretPos);
	void _CalcOffsetByCharIndex(LPCSTR lpStr, int nIndex, POINT& pt);
	void CalcCaretPos();
	void GetCaretDisc(POINT& pt);

	virtual int GetCharBytes(int nPos);
	void OnLeftArrow(UINT nShift);
	void OnRightArrow(UINT nShift);
	void OnUpArrow(UINT nShift);
	void OnDownArrow(UINT nShift);

protected:
	virtual void DestroyWindow();
	int GetLastSpaceBefore(int pos);
	int GetNextWordStartPosAfter(int pos);
	string stringUpToCaret();
	string stringFromCaret();
	string GetDisplayString(string& str);
	int GetDisplayStringWidth(string& str);
	void DrawString(ICanvas* pCanvas, RECT& rcClip, string& strText, WPixel& clr, POINT& ptCurPos);
	void DrawSelectedString(ICanvas* pCanvas, RECT& rcClip, string& strText, WPixel& clr, POINT& ptCurPos);
	void DrawUnSelectText(ICanvas* pCanvas, RECT& rcClip, POINT& ptCurPos, string& strText);
	void DrawSelectText(ICanvas* pCanvas, RECT& rcClip, POINT& ptCurPos, string& strText);
	void OnHScrollPosChanged(DWORD dwParam);
	void OnVScrollPosChanged(DWORD dwParam);
	void ReDrawCaret();
	void CalcValidSize();
private:
	void CalcValidSize_MultiLine_AutoWrap();
	void CalcValidSize_MultiLine_NoAutoWrap();
	void CalcValidSize_NoMultiLine();
};
//------------------------------------------------------------------------
// 滑动条控件
class WNDSYS_API XSlideBar : public XWindow  
{
protected:
	IBitmap*	m_pBmp;
	int			m_nStyle;
	int			m_nMin;
	int			m_nMax;
	int			m_nPos;
	RECT		m_rcBlock;
	
Signals:
	Signal1		s_posChanged;
public:
	XSlideBar();
	XSlideBar(XWindow* pParentWnd);
	virtual ~XSlideBar();
	virtual int GetWindowType()						{return WT_SLIDEBAR;}
	
	enum STYLE{S_HORZ=0, S_VERT};
	
	virtual void SetBlockBitmap(IBitmap* pBmp)	{m_pBmp = pBmp;}
	virtual void SetStyle(int nStyle)			{m_nStyle = nStyle;}
	virtual int  GetStyle()						{return m_nStyle;}
	virtual void SetRange(int nMin, int nMax, BOOL bRedraw = FALSE);
	virtual void GetRange(int& nMin, int& nMax) {nMin = m_nMin;nMax = m_nMax;}
	virtual void SetBlockRect(LPRECT lpRect)	{CopyRect(&m_rcBlock, lpRect);}
	virtual void SetPos(int nPos);
	virtual int GetPos()						{return m_nPos;}

	virtual void LoadAllImage();
	virtual void ReleaseAllImage();
public:
	virtual BOOL IsTransparent(POINT& pt)		{return FALSE;}
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt);
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void OnDraw(ICanvas* pCanvas, RECT& rcClip);
	
protected:
	virtual void DestroyWindow();
};
//------------------------------------------------------------------------
// 滚动条控件
class WNDSYS_API XScrollBar : public XWindow, public ITimerSink
{
protected:
	int			m_nStyle;			// 滚动条类型(水平？垂直？)
	int			m_nMin;				// 滚动范围的最小值
	int			m_nMax;				// 滚动范围的最大值
	int			m_nPos;				// 当前滚动位置
	SIZE		m_Size;				// 步进按钮的尺寸大小
	RECT		m_rcBlock;			// 滑块的矩形区域
	RECT		m_rcLeftTop;		// 左或上按钮的矩形区域
	RECT		m_rcRightBottom;	// 右或下按钮的矩形区域
	int			m_nPressFlags;		// 滚动条按下标志
	XWindow*	m_pAttach;			// 让滚动条吸附的窗口
	int			m_nVisibleCount;	// 可见区域的条目数,用于计算滑动块的长短
	int			m_nTimerTick;		// 用于时钟计数
	int			m_nLineDelta;		// 按左右(上下)按钮时，位置改变多少，默认为1
	POINT		m_ptPageScrollPos;	// 页滚动的终结处鼠标位置
	
Signals:
	Signal1		s_posChanged;		// 滚动条位置改变
public:
	XScrollBar();
	XScrollBar(XWindow* pParentWnd);
	virtual ~XScrollBar();
	virtual int GetWindowType()		{return WT_SCROLLBAR;}
	
	// 立体效果的表现所占像素的行(列)数
	enum {DELTA_3D=7};
	enum STYLE{S_HORZ=0, S_VERT};
	enum PRESSFLAGS{PF_NONE=0,PF_LEFTTOP,PF_BLOCK,PF_RIGHTBOTTOM,PF_LEFTTOPPAGE,PF_RIGHTBOTTOMPAGE};
	enum ScrollBarBitmapPos// 滚动条各个图片元素在标准滚动条位图中的位置索引
	{
		LeftUp=0, LeftDown, LeftDisable,
		RightUp, RightDown, RightDisable,
		TopUp, TopDown, TopDisable,
		BottomUp, BottomDown, BottomDisable,
		BlockHorzUp, BlockHorzDown, 
		BlockVertUp, BlockVertDown, 
		ScrollBarBackGroundHorz, ScrollBarBackGroundVert,
	};
	
public:
	void SetStyle(int nStyle)					{m_nStyle = nStyle;}
	int  GetStyle()								{return m_nStyle;}
	void SetScrollRange(int nMin, int nMax, BOOL bRedraw = FALSE);
	void GetScrollRange(int& nMin, int& nMax)	{nMin = m_nMin;nMax = m_nMax;}
	int  GetMaxRange()							{return m_nMax;}
	void SetScrollPos(int nPos, BOOL bRedraw = TRUE);
	int  GetScrollPos()							{return m_nPos;}
	void SetVisibleCount(int nCount)			{m_nVisibleCount = nCount;}
	int  GetVisibleCount()						{return m_nVisibleCount;}
	void SetSize(SIZE& size);
	void GetSize(SIZE& size)					{size.cx = m_Size.cx;size.cy = m_Size.cy;}
	void SetLineDelta(int nDelta)				{m_nLineDelta = nDelta;}
	virtual void LineLeftUp();
	virtual void LineRightDown();
	virtual void PageLeftUp();
	virtual void PageRightDown();
	
	XWindow* Attach(XWindow* pWnd);
	void Detach()								{m_pAttach = NULL;}
	void Refresh();

	virtual BOOL IsTransparent(POINT& pt)		{return FALSE;}
public:
	virtual void  OnTimer(DWORD dwEventID);
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip);
	virtual void OnDraw(ICanvas* pCanvas, RECT& rcClip);
	virtual void MoveWindow(int dx, int dy);
	
public:
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt);

	virtual DWORD OnKeyDown(UINT nKeyCode, UINT nShift);
	
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	
	virtual void OnPaint(ICanvas* pCanvas);
protected:
	virtual void DestroyWindow();
	virtual void DrawLeftTopButton(ICanvas* pCanvas, int nScrollBarStyle, RECT& rcLeftTop, RECT& rcClip, DWORD dwBlitStyle);
	virtual void DrawRightBottomButton(ICanvas* pCanvas, int nScrollBarStyle, RECT& rcRightBottom, RECT& rcClip, DWORD dwBlitStyle);
	virtual void DrawTrackBar(ICanvas* pCanvas, int nScrollBarStyle, RECT& rcTraceBar, RECT& rcClip, DWORD dwBlitStyle);
	virtual void DrawSlideBlock(ICanvas* pCanvas, int nScrollBarStyle, RECT& rcSlideBlock, RECT& rcClip, DWORD dwBlitStyle);
	virtual void CheckPageScroll();
};
//------------------------------------------------------------------------
// 进度条控件
class WNDSYS_API XProgressBar : public XWindow  
{
protected:
	int			m_nStyle;	// 水平？垂直？
	int			m_nMin;		// 最小值
	int			m_nMax;		// 最大值
	int			m_nPos;		// 当前位置
public:
	XProgressBar();
	XProgressBar(XWindow* pParentWnd);
	virtual ~XProgressBar();
	virtual int GetWindowType()					{return WT_PROGRESSBAR;}
	
	enum STYLE{S_HORZ=0, S_VERT};
	
	void SetStyle(int nStyle)				{m_nStyle = nStyle;}
	int  GetStyle()							{return m_nStyle;}
	void SetRange(int nMin, int nMax, BOOL bRedraw = FALSE);
	void GetRange(int& nMin, int& nMax)		{nMin = m_nMin;nMax = m_nMax;}
	
	void SetPos(int nPos);
	int  GetPos()							{return m_nPos;}
	virtual BOOL IsTransparent(POINT& pt)	{return FALSE;}
	
public:
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip);
	virtual void OnDraw(ICanvas* pCanvas, RECT& rcClip);
};
//------------------------------------------------------------------------
// 静态图片控件
class WNDSYS_API XStaticBitmap : public XWindow
{
protected:
	BOOL  m_bStretch;                       //是否支持拉伸
public:
	XStaticBitmap();
	XStaticBitmap(XWindow* pParentWnd);
	virtual ~XStaticBitmap();
	virtual int GetWindowType()					{return WT_STATICBITMAP;}
	virtual void SetStretch(BOOL flag);
	virtual BOOL IsTransparent(POINT& pt)		{return !IsEnabled();}
	
public:
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip);
};
//------------------------------------------------------------------------
// 动画控件
class WNDSYS_API XAnimate : public XWindow, public ITimerSink
{
protected:
	enum {statePlay=0,statePause,};
	int		m_nFrameCount;	// 帧数
	int		m_nCurFrame;	// 当前帧
	int		m_nDelay;		// 动画延时
	int		m_nOffX;		// 当前帧的x偏移
	int		m_nFrameWidth;	// 单帧宽度
	int		m_nState;		// 当前播放状态
public:
	XAnimate();
	XAnimate(XWindow* pParentWnd);
	virtual ~XAnimate();
	
	virtual int GetWindowType()							{return WT_ANIMATE;}
	
	virtual void SetFrameCount(int nCount)			{m_nFrameCount = nCount;}
	virtual int  GetFrameCount()					{return m_nFrameCount;}
	virtual void SetDelay(int nDelay);
	virtual int  GetDelay()							{return m_nDelay;}
	virtual void Play();
	virtual void Pause();
	virtual void Resume();
	virtual void SetCurFrame(int nFrame);
	virtual BOOL IsPlaying()						{return m_nState == statePlay;}
	virtual BOOL IsTransparent(POINT& pt)		{return !IsEnabled();}
public:
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip);
	virtual void OnTimer(DWORD dwEventID);
	
protected:
	virtual void DestroyWindow();
};
//------------------------------------------------------------------------
// 列表框控件
class WNDSYS_API XListBox : public XWindow  
{
protected:
	struct ItemData		// 条目结构
	{
		DWORD dwItemData;
		string str;
		BOOL operator < (const ItemData& itemdata)
		{
			return str < itemdata.str;
		}
	};
	
	typedef list<ItemData>				ItemList;
	typedef list<ItemData>::iterator	ItemListPtr;
	
	int				m_nSelectIndex;		// 选择条目的索引
	ItemList		m_lstItem;			// 条目列表
	XScrollBar*		m_pVScrollBar;		// 垂直滚动条
	int				m_nVisibleCount;	// 可见的条目数
	int				m_nItemHeight;		// 条目的高度，我们这里仅仅支持所有的条目都是统一的高度
	BOOL			m_bAutoSort;		// 是否自动排序
	
Signals:
	Signal1			s_selChanged;
public:
	XListBox();
	XListBox(XWindow* pParentWnd);
	virtual ~XListBox();
	virtual int GetWindowType()		{return WT_LISTBOX;}
	
	virtual void SetAutoSort(BOOL bSort)	{m_bAutoSort = bSort;}
	virtual BOOL IsAutoSort()				{return m_bAutoSort;}
	
	// 一般操作
	virtual int GetCount();
	virtual int ItemFromPoint(POINT pt);
	virtual void SetItemHeight(int nHeight);
	virtual int GetItemHeight();
	virtual int SetTopIndex(int nIndex);
	virtual int GetTopIndex();
	
	// 条目操作
	virtual int AddItem(LPCSTR lpszItem);
	virtual int AddItem(LPCSTR lpszItem, DWORD dwItemData);
	virtual int RemoveItem(int nIndex);
	virtual int InsertItem(int nIndex, LPCSTR lpszItem);
	virtual void RemoveAll();
	virtual int FindItem(int nStartAfter, LPCSTR lpszItem);
	virtual int SelectItem(int nStartAfter, LPCSTR lpszItem);
	virtual LPCSTR GetText(int nIndex);
	virtual int SetItemData(int nIndex, DWORD dwItemData);
	virtual DWORD GetItemData(int nIndex);
	
	// 单选
	virtual int SetCurSel(int nSelect);
	virtual int GetCurSel();
	
	virtual void OnDraw(ICanvas* pCanvas, RECT& rcClip);
	virtual void SetWindowPos(RECT& rc);
	virtual void SetScrollBar(XWindow* pVScroll);
	
	virtual void ParseStringToListBox(LPCSTR lpszContext, char cSeparator = ',');
	virtual BOOL IsTransparent(POINT& pt)		{return FALSE;}

public:
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt);
	
	virtual DWORD OnKeyDown(UINT nKeyCode, UINT nShift);
	
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	
	virtual void OnSetFocus();
	virtual void OnKillFocus();
private:
	void OnScrollPosChanged(DWORD dwParam);
};
//------------------------------------------------------------------------
// 链接控件
class WNDSYS_API XLinkText : public XWindow  
{
protected:
	WPixel	m_cNotVisitedColor;	// 未访问过的链接颜色
	WPixel	m_cVisitedColor;	// 访问过的链接颜色
	WPixel	m_cRollOverColor;	// 悬停时的颜色
	BOOL	m_bVisited;			// 是否访问过
	WPixel	m_cCurColor;		// 当前应用颜色
	string	m_strURL;			// 链接地址
	
Signals:
	Signal1	s_runURL;
public:
	XLinkText();
	XLinkText(XWindow* pParentWnd);
	virtual ~XLinkText();
	virtual int GetWindowType()									{return WT_LINKTEXT;}
	
	void SetNotVisitedColor(int red, int green, int blue)	{m_cNotVisitedColor = WPixel(red, green, blue);}
	void SetVisitedColor(int red, int green, int blue)		{m_cVisitedColor = WPixel(red, green, blue);}
	void SetRollOverColor(int red, int green, int blue)		{m_cRollOverColor = WPixel(red, green, blue);}
	void SetURL(LPCSTR szURL)								{m_strURL = szURL;}
	LPCSTR GetURL()											{return m_strURL.c_str();}
	void SetVisited(BOOL bVisited = TRUE)					{m_bVisited = bVisited;}
	BOOL IsVisited()										{return m_bVisited;}
	
	virtual BOOL IsTransparent(POINT& pt)					{return FALSE;}
public:
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	
	virtual void OnDraw(ICanvas* pCanvas, RECT& rcClip);
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip);
};
//------------------------------------------------------------------------
// Item和Item容器相关
class XItem;
struct IItemContainer
{
	virtual void SetDirty(XItem* pItem) = 0;
};
class XLine;
struct ILineContainer
{
	virtual void SetDirty(XLine* pLine, XItem* pItem) = 0;
};
// Item基类,所有希望放入ItemContainer中的条目都必须实现这个接口
class WNDSYS_API XItem : public ITimerSink
{
public:
	enum {NotParent=-1,};
	enum LinkStyle {lsNoLink=0,lsLinkPrev=1,lsLinkNext=2};
	int				m_nLinkStyle;	// item与前后行的连接类型
	int				m_nOffsetX;		// item在行中的x偏移
	int				m_nWidth;		// item的大小
	int				m_nHeight;
	int				m_nParent;		// 父item索引
	IItemContainer* m_pContainer;	// item容器
	DWORD			m_dwUserData;	// 用户数据
public:
	enum {InvalidPos=-1,};
	XItem() : m_nLinkStyle(lsNoLink), m_nOffsetX(0), 
		m_nWidth(0), m_nHeight(0), m_nParent(NotParent), m_pContainer(NULL), m_dwUserData(0) {}
	XItem(XItem* pItem);
	virtual ~XItem()	{}
	// 是否有效
	virtual BOOL IsValid() = 0;
	// 是否能够被分割
	virtual BOOL CanSplit()	= 0;
	// 得到宽度(最大宽度，如动画需返回最大宽度)
	virtual int  GetWidth() = 0;
	// 得到高度(最大高度，如动画需返回最大高度)
	virtual int  GetHeight() = 0;
	// 计算分割位置(从nFromPos开始距离为xDisc的位置,这里位置的概念是抽象的，既可以是
	// 文本中字符的索引，也可以是图片中的像素偏移等，如果xDisc=-1,则表示从nFromPos到
	// 条目末尾)
	virtual int  CalcSplitPos(int nFromPos, int xDisc) {return InvalidPos;}
	// 分割一个条目为两个子条目
	virtual void SplitItem(int nLimitedWidth, XItem*& pItem1, XItem*& pItem2){};
	// 设置连接类型
	virtual void SetLinkStyle(int nStyle)		{m_nLinkStyle|=nStyle;}
	// 得到连接类型
	virtual int  GetLinkStyle()					{return m_nLinkStyle;}
	// 设置x向偏移
	virtual void SetOffsetX(int nOffX)			{m_nOffsetX = nOffX;}
	// 得到x向偏移
	virtual int  GetOffsetX()					{return m_nOffsetX;}
	// 设置父item
	virtual void SetParent(int nParent)			{m_nParent = nParent;}
	// 得到父item
	virtual int  GetParent()					{return m_nParent;}
	// 用户数据
	virtual void  SetUserData(DWORD dwUserData)	{m_dwUserData = dwUserData;}
	virtual DWORD GetUserData()					{return m_dwUserData;}
	// 得到光标类型(表明光标在本条目上的形状)
	virtual int  GetCursorType()				{return CT_ARROW;}
	// 释放
	virtual void Release()						{delete this;}
	// 绘图事件
	virtual void OnDraw(ICanvas* pCanvas, int x, int y, RECT& rcClip, int alpha)	{}
	// 执行本条目的回调函数
	virtual void Emit()							{}
	// 鼠标事件
	virtual void OnMouseEnter()	{}
	virtual void OnMouseExit()	{}
	//virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt)	{return 0;}
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt)	{return 0;}
	// 时钟事件
	virtual void OnTimer(DWORD dwEventID) {}
	// 属性
	void SetContainer(IItemContainer* pContainer)	{m_pContainer = pContainer;}
	IItemContainer* GetContainer()					{return m_pContainer;}
	void SetDirty()									{if (m_pContainer) m_pContainer->SetDirty(this);}
	virtual BOOL SaveToHtml(FILE* hFile)	{return TRUE;}
};
//------------------------------------------------------------------------
// 文本条目
class WNDSYS_API XItemText : public XItem
{
protected:
	WPixel		m_clrText;		// 文本的颜色
	ITTFont*	m_pFont;		// 绘制文本时使用的字体
	string		m_strText;		// 文本字符串
public:
	XItemText() : m_clrText(0), m_pFont(0) {m_strText.clear();}
	XItemText(XItemText* pItem);
	virtual ~XItemText() {}
	
	virtual BOOL IsValid()			{return (m_pFont && !m_strText.empty());}
	virtual BOOL CanSplit()			{return TRUE;}
	virtual int  GetWidth()			{if(m_nWidth==0)CalcSize();return m_nWidth;}
	virtual int  GetHeight()		{if(m_nHeight==0)CalcSize();return m_nHeight;}
	virtual void OnDraw(ICanvas* pCanvas, int x, int y, RECT& rcClip, int alpha);
	virtual int  CalcSplitPos(int nFromPos, int xDisc);
	virtual void SplitItem(int nLimitedWidth, XItem*& pItem1, XItem*& pItem2);
	
	void		SetFont(ITTFont* pFont)	{m_pFont = pFont;}
	ITTFont*	GetFont()				{return m_pFont;}
	void		SetText(LPCSTR szText)	{m_strText = szText;}
	LPCSTR		GetText()				{return m_strText.c_str();}
	void		SetTextColor(WPixel clr){m_clrText = clr;}
	WPixel		GetTextColor()			{return m_clrText;}
	int			GetTextLength()			{return m_strText.length();}
	virtual BOOL SaveToHtml(FILE* hFile);
protected:
	void CalcSize();
};
//------------------------------------------------------------------------
// 带链接的文本条目
class WNDSYS_API XItemLinkText : public XItemText
{
protected:
	string		m_strLinkText;		// 链接的内容
	WPixel		m_clrNotVisited;	// 未访问过的链接颜色
	WPixel		m_clrVisited;		// 访问过的链接颜色
	WPixel		m_clrRollOver;		// 悬停时的颜色
	BOOL		m_bVisited;			// 是否访问过
	BOOL		m_bRollOver;		// 当前是否悬停
public:
	XItemLinkText() : m_strLinkText(""), m_clrNotVisited(0),
		m_clrVisited(0), m_clrRollOver(0), m_bVisited(FALSE), m_bRollOver(FALSE) {}
	XItemLinkText(XItemLinkText* pItem);
	void	SetLinkText(LPCSTR szLink)		{m_strLinkText = szLink;}
	LPCSTR	GetLinkText()					{return m_strLinkText.c_str();}
	void	SetNotVisitedColor(WPixel clr)	{m_clrNotVisited = clr;}
	WPixel  GetNotVisitedColor()			{return m_clrNotVisited;}
	void	SetVisitedColor(WPixel clr)		{m_clrVisited = clr;}
	WPixel  GetVisitedColor()				{return m_clrVisited;}
	void	SetRolloverColor(WPixel clr)	{m_clrRollOver = clr;}
	WPixel  GetRolloverColor()				{return m_clrRollOver;}
	void	SetVisited(BOOL visited = TRUE) {m_bVisited = visited;}
	BOOL	IsVisited()						{return m_bVisited;}
	
	virtual int  GetCursorType()			{return CT_HAND;}
	virtual void OnDraw(ICanvas* pCanvas, int x, int y, RECT& rcClip, int alpha);
	virtual void SplitItem(int nLimitedWidth, XItem*& pItem1, XItem*& pItem2);
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual BOOL SaveToHtml(FILE* hFile);
};
//------------------------------------------------------------------------
class WNDSYS_API XItemBookmarkText : public XItemText
{
protected:
	string	m_strBookmark;	// 书签名
public:
	XItemBookmarkText() : XItemText()		{m_strBookmark.clear();}
	XItemBookmarkText(XItemBookmarkText* pItem)	: XItemText(pItem),
		m_strBookmark(pItem->m_strBookmark) {}
	void SetBookmark(LPCSTR szBookmark)		{m_strBookmark = szBookmark;}
	LPCSTR GetBookmark()					{return m_strBookmark.c_str();}
	virtual BOOL SaveToHtml(FILE* hFile);
};
//------------------------------------------------------------------------
// 图片条目
// 注：默认动画图片的每一帧横向无间隔排列(默认每帧大小相等)
class WNDSYS_API XItemImage : public XItem
{
protected:
	IBitmap*	m_pImage;		// 只引用，不删除
	int			m_nFrameCount;	// 帧数(如果是动画则m_nFrameCount>1，否则m_nFrameCount=1)
	int			m_nCurFrame;	// 当前帧
	int			m_nDelay;		// 动画延时
public:
	XItemImage() : m_pImage(NULL), m_nFrameCount(0), m_nCurFrame(0), m_nDelay(200) {}
	virtual ~XItemImage(){}
	virtual BOOL IsValid()			{return (m_pImage!=NULL && m_nFrameCount!=0);}
	virtual BOOL CanSplit()			{return FALSE;}
	virtual int  GetWidth()			{if(m_nWidth==0)m_nWidth=m_pImage->GetWidth()/m_nFrameCount;return m_nWidth;}
	virtual int  GetHeight()		{if(m_nHeight==0)m_nHeight=m_pImage->GetHeight();return m_nHeight;}
	virtual void OnDraw(ICanvas* pCanvas, int x, int y, RECT& rcClip, int alpha);
	virtual void OnTimer(DWORD dwEventID);
	virtual void Release();
	
	void SetBitmap(IBitmap* pBmp)	{m_pImage = pBmp;}
	IBitmap* GetBitmap()			{return m_pImage;}
	void SetFrameCount(int nCount);
	int  GetFrameCount()			{return m_nFrameCount;}
	void SetDelay(int nDelay)		{m_nDelay = nDelay;}
	int  GetDelay()					{return m_nDelay;}
	virtual BOOL SaveToHtml(FILE* hFile);
};
//------------------------------------------------------------------------
class WNDSYS_API XItemLinkImage : public XItemImage
{
protected:
	string	m_strLinkText;
public:
	XItemLinkImage()					{m_strLinkText.clear();}
	void	SetLinkText(LPCSTR szLink)	{m_strLinkText = szLink;}
	LPCSTR	GetLinkText()				{return m_strLinkText.c_str();}

	virtual int  GetCursorType()		{return CT_HAND;}
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual BOOL SaveToHtml(FILE* hFile);
};
//------------------------------------------------------------------------
// 行管理器
class WNDSYS_API XLine : public IItemContainer
{
protected:
	typedef vector<XItem*>					Items;
	typedef vector<XItem*>::iterator		ItemsPtr;
	Items			m_vItems;		// item* 数组
	int				m_nAlign;		// 对齐方式(左、中、右)
	int				m_nWidth;		// 当前item实际占有的宽度
	int				m_nHeight;		// 本行最大高度
	int				m_nOffsetY;		// 本行在整个容器的y向偏移，加速定位
	int				m_nLineSpace;	// 本行与下行之间的行间距
	ILineContainer* m_pContainer;	// 容器指针,这里指ILineContainer*
	
public:
	enum Align{LeftAlign=0,CenterAlign,RightAlign};
	XLine() : m_nAlign(LeftAlign), m_nWidth(0), m_nHeight(0), m_nOffsetY(0), m_nLineSpace(0), m_pContainer(0) {m_vItems.clear();}
	XLine(int nAlign, ILineContainer* pContainer)
	{
		m_nAlign = nAlign;
		m_pContainer = pContainer;
		m_nWidth = m_nHeight = m_nOffsetY = m_nLineSpace = 0;
		m_vItems.clear();
	}
	void SetAlign(int nAlign)			{m_nAlign = nAlign;}
	int  GetAlign()						{return m_nAlign;}
	int  GetHeight()					{return m_nHeight;}
	void SetHeight(int h)				{m_nHeight = h;}
	int  GetWidth()						{return m_nWidth;}
	void SetWidth(int w)				{m_nWidth = w;}
	BOOL IsEmpty()						{return m_vItems.empty();}
	int  GetItemCount()					{return m_vItems.size();}
	XItem* GetItem(int nItem)			{return m_vItems[nItem];}
	void SetOffsetY(int nOffsetY)		{m_nOffsetY = nOffsetY;}
	int  GetOffsetY()					{return m_nOffsetY;}
	void SetLineSpace(int nSpace)		{m_nLineSpace = nSpace;}
	int  GetLineSpace()					{return m_nLineSpace;}
	virtual void OnDraw(ICanvas* pCanvas, int x, int y, RECT& rcClip, int alpha);
	void AddItem(XItem* pItem);
	void Clear();
	virtual void SetDirty(XItem* pItem);
	BOOL SaveToHtml(FILE* hFile);
};
//------------------------------------------------------------------------
// item容器基类，不处理鼠标、键盘事件，没有自带滚动条，但可以支持滚动条
class WNDSYS_API XItemContainer : public XWindow, public ILineContainer
{
protected:
	typedef vector<XItem*>					Items;
	typedef vector<XItem*>::iterator		ItemsPtr;
	typedef vector<XLine*>					Lines;
	typedef vector<XLine*>::iterator		LinesPtr;
	
	Items		m_vSplitItems;	// 对于被分割的条目应该建立一个列表,让分割后的条目记住自己的父条目的索引
	Lines		m_vLines;		// 行数据数组
	int			m_nMouseHover;	// 鼠标停留的 Line(高字),item(低字)
	BOOL		m_bLineWrap;	// 是否需要自动折行
	int			m_nMaxWidth;	// 容器的最大宽度
	int			m_nMaxHeight;	// 容器的最大高度
	POINT		m_ptOffset;		// 由于滚动导致的x、y方向的偏移
	XScrollBar*	m_pHScrollBar;	// 水平滚动条
	XScrollBar*	m_pVScrollBar;	// 垂直滚动条
private:
	int			m_nMargin;		// 边框空白区域大小
	
public:
	XItemContainer();
	XItemContainer(XWindow* pParentWnd);
	virtual ~XItemContainer();
	
	// 属性
	void SetMargin(int nMargin)		{m_nMargin = max(0, nMargin);}
	int  GetMargin()				{return m_nMargin;}
	void SetAutoWrap(BOOL bWrap)	{m_bLineWrap = bWrap;}
	BOOL IsAutoWrap()				{return m_bLineWrap;}
	int  GetMaxWidth()				{return m_nMaxWidth;}
	void SetMaxWidth(int nWidth)	{m_nMaxWidth = nWidth;}
	int  GetMaxHeight()				{return m_nMaxHeight;}
	void SetMaxHeight(int nHeight)	{m_nMaxHeight = nHeight;}
	void SetOffset(POINT ptOff)		{m_ptOffset = ptOff;}
	void SetOffset(int x, int y)	{m_ptOffset.x = x;m_ptOffset.y = y;}
	POINT GetOffset()				{return m_ptOffset;}
	POINT* GetOffsetPtr()			{return &m_ptOffset;}
	int  GetLineCount()				{return m_vLines.size();}
	XLine* GetLine(int nLine)		{return m_vLines[nLine];}
	void SetHScrollBar(XScrollBar* pBar);
	void SetVScrollBar(XScrollBar* pBar);
	
	// 操作
	virtual void DeleteLines(int nLines);
	virtual void Clear();
	virtual XLine* NewLine(int nAlign = XLine::LeftAlign, int nLineSpace = 2);
	virtual BOOL AddItem(XItem* pItem, BOOL bNewLine, int nAlign = XLine::LeftAlign, int nLineSpace = 2);
	virtual void BringToTopLine(int nLine);
	virtual void SetDirty(XLine* pLine, XItem* pItem);	
	void AdjustHScroll();
	void AdjustVScroll();
	BOOL SaveToHtml(LPCSTR szFileName, LPCSTR szTitle=NULL, WPixel backcolor=0);
protected:
	virtual int	GetMaxValidWidth();
	int  GetItemByOffset(int nLine, int nOffsetX);
	void OnItemChanged(int nLine, int nItem);
	void OnHScrollPosChanged(DWORD dwParam);
	void OnVScrollPosChanged(DWORD dwParam);
	void OnClick()	{this->SetFocus();}
	
public:
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt);
	virtual DWORD OnKeyDown(UINT nKeyCode, UINT nShift);
	virtual void OnDraw(ICanvas* pCanvas, RECT& rcClip);
	virtual BOOL IsTransparent(POINT& pt)		{return FALSE;}
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip) {}
	virtual	void SetWindowPos(RECT& rc);
protected:
	virtual void DestroyWindow();
private:
	int  GetAlignOffset(XLine* pLine);
};
//------------------------------------------------------------------------
// 解析器基类，行解析器相关
class WNDSYS_API XParserBase
{
protected:
	XItemContainer* m_pContainer;	// item容器
public:
	XParserBase() : m_pContainer(NULL) {}
	virtual ~XParserBase() {}
	
	void SetItemContainer(XItemContainer* pContainer)	{m_pContainer = pContainer;}
	XItemContainer* GetItemContainer()					{return m_pContainer;}
	virtual BOOL ParseLine(LPCSTR szLine)				{return FALSE;}
};
//------------------------------------------------------------------------
// 各种标签的上下文
struct Context
{
};
//------------------------------------------------------------------------
struct ParagraphContext : public Context
{
	int			nAlign;
	int			nLineSpace;
	ParagraphContext() {clear();}
	void clear() {nAlign=0;nLineSpace=2;}
};
//------------------------------------------------------------------------
struct TextContext : public Context
{
	string		strText;		// 正文
	WPixel		clrText;		// 文本颜色
	string		strFontName;	// 字体名
	int			nFontSize;		// 字体大小
	BOOL		bBlod;			// 粗体
	BOOL		bItalic;		// 斜体
	BOOL		bUnderline;		// 下划线
	TextContext() {clear();}
	void clear(){
		strText=" ",clrText=WPixel(255,255,255),strFontName="宋体",nFontSize=12;
		bBlod=0,bItalic=0,bUnderline=0;
	}
};
//------------------------------------------------------------------------
struct LinkTextContext : public TextContext
{
	string		strLinkText;	// 链接的内容
	LinkTextContext() {clear();}
	void clear(){TextContext::clear();strLinkText=strText;bUnderline=0;}
};
//------------------------------------------------------------------------
struct BookmarkTextContext : public TextContext
{
	string		strBookmark;	// 书签名
	BookmarkTextContext() {clear();}
	void clear(){TextContext::clear();strBookmark=strText;}
};
struct ImageContext : public Context
{
	string		strImage;		// 图片名
	int			nCount;			// 帧数
	int			nDelay;			// 动画延时
	int			nType;			// 动画类型，外部文件(0)还是包内文件(1)？
	ImageContext() {clear();}
	void clear() {strImage="",nCount=1,nDelay=200,nType=0;}
};
//------------------------------------------------------------------------
// 各种标签解析器
class WNDSYS_API CTagParser
{
protected:
	XLineParser*		m_pLineParser;
public:
	CTagParser();
	virtual ~CTagParser();

	virtual LPCSTR	GetOpenKey()		{return "";}
	virtual int		GetOpenKeyLen()		{return 0;}
	virtual LPCSTR	GetCloseKey()		{return "";}
	virtual int		GetCloseKeyLen()	{return 0;}
	virtual BOOL	HasCloseTag()		{return TRUE;}
	virtual BOOL	Parse(char* szLine, char* pBegin, char* pEnd)	{return FALSE;}
	virtual BOOL	Emit() {return FALSE;}

public:
	void SetLineParser(XLineParser* pLineParser)	{m_pLineParser = pLineParser;}
	XLineParser* GetLineParser()					{return m_pLineParser;}

	char* FindEndTag(char* pBegin);
	BOOL IsThisTag(LPCSTR szTag);

protected:
	char GetLowerChar(char* c);
	void SkipSpace(char* szLine, char*& pBegin, char* pEnd);
	void SkipSpaceAndQuot(char* szLine, char*& pBegin, char* pEnd);
	int  ConvertStringToInt(LPCSTR szString);
	BOOL GetPropertyString(char* szLine, char*& pBegin, char* pEnd, char* buffer, int bufsize);
};
//------------------------------------------------------------------------
class WNDSYS_API CTagParserParagraph : public CTagParser
{
protected:
	ParagraphContext	context;
public:
	virtual LPCSTR GetOpenKey()			{return "<p";}
	virtual int	   GetOpenKeyLen()		{return 2;}
	virtual LPCSTR GetCloseKey()		{return ">";}
	virtual int  GetCloseKeyLen()		{return 1;}
	virtual BOOL HasCloseTag()			{return FALSE;}
	virtual BOOL Parse(char* szLine, char* pBegin, char* pEnd);
	virtual BOOL Emit();
};
//------------------------------------------------------------------------
class WNDSYS_API CTagParserText : public CTagParser
{
protected:
	TextContext		context;
public:
	virtual LPCSTR GetOpenKey()			{return "<text";}
	virtual int	   GetOpenKeyLen()		{return 5;}
	virtual LPCSTR GetCloseKey()		{return "</text>";}
	virtual int  GetCloseKeyLen()		{return 7;}
	virtual BOOL Parse(char* szLine, char* pBegin, char* pEnd);
	virtual BOOL Emit();
};
//------------------------------------------------------------------------
class WNDSYS_API CTagParserLink : public CTagParser
{
protected:
	LinkTextContext	context;
public:
	virtual LPCSTR GetOpenKey()			{return "<link";}
	virtual int	   GetOpenKeyLen()		{return 5;}
	virtual LPCSTR GetCloseKey()		{return "</link>";}
	virtual int  GetCloseKeyLen()		{return 7;}
	virtual BOOL Parse(char* szLine, char* pBegin, char* pEnd);
	virtual BOOL Emit();
};
//------------------------------------------------------------------------
class WNDSYS_API CTagParserBookmark : public CTagParser
{
protected:
	BookmarkTextContext	context;
public:
	virtual LPCSTR GetOpenKey()			{return "<bookmark";}
	virtual int	   GetOpenKeyLen()		{return 9;}
	virtual LPCSTR GetCloseKey()		{return "</bookmark>";}
	virtual int  GetCloseKeyLen()		{return 11;}
	virtual BOOL Parse(char* szLine, char* pBegin, char* pEnd);
	virtual BOOL Emit();
};
//------------------------------------------------------------------------
class WNDSYS_API CTagParserImage : public CTagParser
{
protected:
	ImageContext	context;
public:
	virtual LPCSTR GetOpenKey()			{return "<img";}
	virtual int	   GetOpenKeyLen()		{return 4;}
	virtual LPCSTR GetCloseKey()		{return ">";}
	virtual int  GetCloseKeyLen()		{return 1;}
	virtual BOOL HasCloseTag()			{return FALSE;}
	virtual BOOL Parse(char* szLine, char* pBegin, char* pEnd);
	virtual BOOL Emit();
};
//------------------------------------------------------------------------
// 行解析器
#define TAG_MARK_BEGIN	'<'
#define TAG_MARK_END	'>'
class WNDSYS_API XLineParser : public XParserBase
{
private:
	int						m_nAlign;
	vector<CTagParser*>		m_vTagParserList;	// 各种标签解析器列表
public:
	XLineParser();
	virtual ~XLineParser();
	
	BOOL RegisterTagParser(CTagParser* pTagParser);
	BOOL UnregisterTagParser(CTagParser* pTagParser);
	void UnregisterAllTagParser();
	CTagParser* GetMatchTagParser(char* pBegin, char* pEnd);
	virtual BOOL ParseLine(LPCSTR szLine);
protected:
	BOOL AddText(TextContext context);
};
//------------------------------------------------------------------------
// 聊天框
class WNDSYS_API XChat : public XWindow
{
protected:
	class XChatScrollBar : public XScrollBar
	{
	public:
		XChatScrollBar(XWindow* pParentWnd) : XScrollBar(pParentWnd) {}
		virtual void LineLeftUp();
		virtual void LineRightDown();
	};
	BOOL				m_bAutoSroll;	// 是否自动滚屏
	int					m_nMaxLines;	// 允许的最大行数
	XChatScrollBar*		m_pVScrollBar;	// 垂直滚动条
	XItemContainer*		m_pContainer;	// item容器
	XParserBase*		m_pParser;		// 格式文本解析器
	
public:
	XChat();
	XChat(XWindow* pParentWnd);
	virtual ~XChat();
	virtual int GetWindowType()				{return WT_CHAT;}
	
	// for item container
	void SetMargin(int nMargin);
	void SetAutoWrap(BOOL bWrap);
	
	// parser
	void SetParser(XParserBase* pParser);
	XParserBase* GetParser()			{return m_pParser;}
	
	// item container
	void SetItemContainer(XItemContainer* pContainer)	{m_pContainer = pContainer;}
	XItemContainer* GetItemContainer()	{return m_pContainer;}
	
	XScrollBar* GetVScrollBar()			{return m_pVScrollBar;}
	
	void SetAutoScroll(BOOL bAuto=TRUE)	{m_bAutoSroll = bAuto;}
	BOOL IsAutoScroll()					{return m_bAutoSroll;}
	void SetMaxLines(int nLines)		{m_nMaxLines = max(10, nLines);}
	int  GetMaxLines()					{return m_nMaxLines;}
	
	void Clear();
	BOOL ParseString(LPCSTR szUnFormatText);
	
public:
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip) {}
	virtual void SetWindowPos(RECT& rc);
	virtual BOOL IsTransparent(POINT& pt)		{return !IsEnabled();}
	virtual DWORD OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt);
	virtual DWORD OnKeyDown(UINT nKeyCode, UINT nShift);
	virtual DWORD OnKeyUp(UINT nKeyCode, UINT nShift);
	
public:
	void AdjustScroll();
};
//------------------------------------------------------------------------
// 类HTML控件
class WNDSYS_API XHtml : public XWindow  
{
public:
	class XHtmlScrollBar : public XScrollBar
	{
	public:
		XHtmlScrollBar(XWindow* pParentWnd) : XScrollBar(pParentWnd) {}
		virtual void LineLeftUp();
		virtual void LineRightDown();
	};
	// 跳转文本(用于页面浏览)
	class XItemJumpText : public XItemLinkText
	{
		XHtml*		m_pXHtml;
	public:
		virtual void Emit();	
		void SetXHtmlPtr(XHtml* pXHtml) {m_pXHtml = pXHtml;}
	};
	class CTagParserJump : public CTagParserLink
	{
		XHtml*		m_pXHtml;
	public:
		virtual BOOL Emit();
		void SetXHtmlPtr(XHtml* pXHtml) {m_pXHtml = pXHtml;}
	};
	class CTagParserBookmarkEx : public CTagParserBookmark
	{
		XHtml*		m_pXHtml;
	public:
		virtual BOOL Emit();
		void SetXHtmlPtr(XHtml* pXHtml) {m_pXHtml = pXHtml;}
	};	
	typedef string		Line;
	typedef list<Line>	Lines;
	typedef list<Line>::iterator	LinesPtr;
	// 文件操作辅助类
	class FileHelper
	{
	private:
		enum {buffer_size = 2048};
	public:
		static bool Read(const char* szFileName, Lines& lines);
	};
public:
	typedef hash_map<string, int>				BookmarkList;
	typedef hash_map<string, int>::iterator		BookmarkListPtr;
	typedef hash_map<string, BOOL>				JumpList;
	typedef hash_map<string, BOOL>::iterator	JumpListPtr;
	typedef vector<string>						NavigateList;
	typedef vector<string>::iterator			NavigateListPtr;

	XHtmlScrollBar*	m_pHScrollBar;	// 水平滚动条
	XHtmlScrollBar*	m_pVScrollBar;	// 垂直滚动条
	XItemContainer* m_pContainer;	// item容器

	// 各类解析器
	XLineParser				m_LineParser;			// 行解析器
	CTagParserParagraph		m_TagParserParagraph;	// 段落标签解析器
	CTagParserText			m_TagParserText;		// 一般文本标签解析器
	CTagParserJump			m_TagParserJump;		// 跳转标签解析器
	CTagParserBookmarkEx	m_TagParserBookmark;	// 书签标签解析器
	CTagParserImage			m_TagParserImage;		// 图片标签解析器

	// 页面信息记录
	BookmarkList			m_BookmarkList;			// 书签列表,int指行索引
	JumpList				m_JumpList;				// 跳转列表,BOOL表明是否已经跳转过

	// 当前xhtm的工作目录,指向保存xhtm脚本的位置
	string					m_strWorkingDir;		// 当前xhtm控件的工作目录
	string					m_strFileName;			// 当前xhtm控件加载的脚本文件名
	NavigateList			m_NavigateList;			// 用于“前进”和“后退”功能
	int						m_nCurNavigateIdx;		// 当前导航索引
public:
	XHtml();
	XHtml(XWindow* pParentWnd);
	virtual ~XHtml();
	virtual int GetWindowType()			{return WT_HTML;}
	
	void SetMargin(int nMargin)		{m_pContainer->SetMargin(nMargin);}
	
	void Clear();
	void SetWorkingDir(LPCSTR szWorkingDir);
	XItemContainer* GetItemContainer()	{return m_pContainer;}
	
	// xhtm导航操作
	virtual void GoBack();
	virtual void GoForward();
	virtual BOOL CanBack();
	virtual BOOL CanForward();
	virtual void Navigate(LPCSTR strUrl);
	BOOL AddLine(LPCSTR szLineText);
	void RegisterTagParser(CTagParser* pTagParser);

	// 跳转与书签操作
	virtual void SetJumpVisited(LPCSTR strJump, BOOL bVisited);
	virtual void AddBookmarkToHashmap(LPCSTR strBookmark);
	virtual void AddJumpToHashMap(LPCSTR strJump, BOOL bVisited);
	virtual BOOL GetJumpVisited(LPCSTR strJump);

public:
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip) {}
	virtual void SetWindowPos(RECT& rc);
	virtual BOOL IsTransparent(POINT& pt)		{return FALSE;}
	virtual DWORD OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt);
	virtual DWORD OnKeyDown(UINT nKeyCode, UINT nShift);
	virtual DWORD OnKeyUp(UINT nKeyCode, UINT nShift);

protected:
	void JumpToUrl(LPCSTR strUrl);

private:
	void LocateBookmark(LPCSTR strBookmark);
	void LoadFile(LPCSTR szFileName);
	void RegisterAllParser();
};
//------------------------------------------------------------------------
// Talk控件
class WNDSYS_API XTalk : public XWindow
{
public:
	class XItemTaskText : public XItemLinkText
	{
	protected:
		XTalk*			m_pXTalk;
		int				m_nTaskID;
	public:
		
		void SetTaskID(int nTaskID)		{m_nTaskID = nTaskID;}	// 设置任务索引
		long GetTaskID()				{ return m_nTaskID;}	// 取得任务索引
		void SetTalkPtr(XTalk* pXTalk)	{m_pXTalk = pXTalk;}
		virtual void Emit();	
	};
	class CTagParserTaskText : public CTagParserLink
	{
	protected:
		XTalk*		m_pXTalk;
	public:
		void SetTalkPtr(XTalk* pXTalk)	{m_pXTalk = pXTalk;}
		virtual BOOL Emit();
	};
	typedef XHtml::XHtmlScrollBar	XTalkScrollBar;
public:
	XTalkScrollBar*	m_pVScrollBar;	// 垂直滚动条
	XItemContainer* m_pContainer;	// item容器
	
	// 各类解析器
	XLineParser				m_LineParser;			// 行解析器
	CTagParserParagraph		m_TagParserParagraph;	// 段落标签解析器
	CTagParserText			m_TagParserText;		// 一般文本标签解析器
	CTagParserTaskText		m_TagParserTaskText;	// 任务文本标签解析器
	CTagParserImage			m_TagParserImage;		// 图片标签解析器
	
public:
	XTalk();
	XTalk(XWindow* pParentWnd);
	virtual ~XTalk();
	
	virtual int GetWindowType()			{return WT_TALK;}
	void SetMargin(int nMargin)		{m_pContainer->SetMargin(nMargin);}
	void Clear();
	XItemContainer* GetItemContainer()	{return m_pContainer;}
	
	BOOL AddLine(LPCSTR szLineText);
	void RegisterTagParser(CTagParser* pTagParser);
public:
	virtual void OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip) {}
	virtual BOOL IsTransparent(POINT& pt)		{return FALSE;}
	virtual DWORD OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt);
	virtual void OnTaskTextClick(LPCSTR szText)	{}
	virtual void SetWindowPos(RECT& rc);
private:
	void RegisterAllParser();
};
//------------------------------------------------------------------------
//增加TOOLTIP类, 令在本模块中可以调用到TOOLTIP
class WNDSYS_API CToolTipLoader  
{
private:
	stdext::hash_map<int, std::string> m_hmapToolTip;
	static CToolTipLoader __instance;
public:
	void AddToolTip(int nID, LPCSTR szStr);
	LPCSTR GetToolTip(int nID);	
	static CToolTipLoader &GetInstance(void);
};
//------------------------------------------------------------------------
// 窗口系统辅助类
class WNDSYS_API CWndSysHelper  
{
public:
	CWndSysHelper();
	virtual ~CWndSysHelper();
	
protected:
	IFileSystem*	m_pFileSystem;
	IResObject*		m_pResObject;
	ICanvasHelper	m_ICanvasHelper;
	
public:
	XWindow* CreateWndSys(HWND hWnd, ICanvas* pCanvas, ITimeAxis* pTimeAxis, IFileSystem* pFileSystem, IResObject* pResObject, ITTFont* pSysFont);
	XWindow* CreateDialogByResObject(XWindow* pParentWnd, IResObject* pResObject, int nID);
	XWindow* CreateXWindow(XWindow* pParentWnd, IResObject* pResObject, int nID, XWindow* pSubclassWnd = NULL);
	IBitmap* GetBitmap(LPCSTR szFile);
	XDesktop* GetGameDesktop(){return XDesktop::GetInstance();}
	DWORD OnMessage(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint(ICanvas* pCanvas);
	void CreateStockProperty(XWindow* pWnd, IResObject* pResObject, int nID);
	void CloseWndSys();
	
private:
	XCursor* CreateXCursor(IResObject* pResObject);
};
//------------------------------------------------------------------------