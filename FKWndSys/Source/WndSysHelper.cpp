/**
*	created:		2012-6-23   0:53
*	filename: 		WndSysHelper
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../../FKResManager/IFKResManager.h"
#include "../../FKTrace/IFKTrace.h"
//------------------------------------------------------------------------
CFKTrace g_DefaultTrace;
//------------------------------------------------------------------------
CWndSysHelper::CWndSysHelper()
{
	m_pFileSystem = NULL;
	m_pResObject = NULL;
}
//------------------------------------------------------------------------
CWndSysHelper::~CWndSysHelper()
{
}
//------------------------------------------------------------------------
void CWndSysHelper::CloseWndSys()
{
	GetGameDesktop()->Release();
	CImageLoader::ReleaseInstance();
	XFontCreate::ReleaseInstance();
}
//------------------------------------------------------------------------
XWindow* CWndSysHelper::CreateWndSys(HWND hWnd, ICanvas* pCanvas, 
									 ITimeAxis* pTimeAxis, 
									 IFileSystem* pFileSystem, IResObject* pResObject, 
									 ITTFont* pSysFont)
{
	m_pFileSystem = pFileSystem;
	m_pResObject = pResObject;

	// 创建游戏桌面
	XDesktop* pDesktop = XDesktop::GetInstance();
	pDesktop->SetHWnd(hWnd);
	pDesktop->SetCanvas(pCanvas);
	pDesktop->SetTimeAxis(pTimeAxis);

	//long lRetval;
	LPCSTR pString;
	IBitmap* pBmp;	
	RECT rc;
	SetRectEmpty(&rc);
	rc.right = pCanvas->GetWidth();
	rc.bottom = pCanvas->GetHeight();
	pDesktop->SetWindowPos(rc);

	// src (桌面图片)
	pString = pResObject->GetPropertyString("src");
	if (pString && NULL != (pBmp = CImageLoader::GetInstance()->LoadImage(pString)) )
		pDesktop->SetBitmap(pBmp);

	// caption (标题，文本)
	if (pString = pResObject->GetPropertyString("caption"))
		pDesktop->SetWindowText(pString);
	else
	{
		if (pString = pResObject->GetObjectName())
		pDesktop->SetWindowText(pString);
	}

	// 系统字体
	if (!pSysFont)
	{
		long lRetval = 0;
		if ((pString = pResObject->GetPropertyString("fontname")) && 
			pResObject->GetPropertyLong("fontsize", lRetval, 12) && lRetval != 0)
			pSysFont = XFontCreate::GetInstance()->CreateFont(pString, lRetval, FALSE, FALSE, FALSE, FALSE, 256);
		else
			pSysFont = XFontCreate::GetInstance()->GetDefaultFont();
	}
	pDesktop->SetSysFont(pSysFont);

	// 窗口特性
	pDesktop->SetMovable(FALSE);

	// 创建所有的光标
	IResObject* pCursorObject = m_pResObject->GetSubObjectByName("CursorType");
	int nCursorCount = pCursorObject->GetSubObjectCount();
	XCursor** ppCursors = NULL;
	ppCursors = new XCursor*[nCursorCount];
	for (int i = 0; i < nCursorCount; i++)
	{
		ppCursors[i] = CreateXCursor(pCursorObject->GetSubObjectByIndex(i));
		if (!ppCursors[i])
			return NULL;
	}

	pDesktop->SetCursorCount(nCursorCount);
	pDesktop->SetAllCursor(ppCursors);
	pDesktop->SetCursor(CT_ARROW);
	pDesktop->GetCurrentCursor()->Show(TRUE);
	pDesktop->GetCurrentCursor()->InvalidRect();

	// 加载用于创建滚动条的背景图片
	pDesktop->SetSysScrollBarBitmap(CImageLoader::GetInstance()->LoadImage(WNDMGR_SCORLLBAR_FILE));

	// 创建公用ToolTip控件
	XToolTip* pToolTip = new XToolTip(pDesktop);
	pToolTip->SetVisible(FALSE);
	pToolTip->SetMovable(FALSE);
	pToolTip->SetEnabled(FALSE);
	pToolTip->SetTopmost(TRUE);
	pDesktop->SetSysToolTip((XWindow*)pToolTip);

	return pDesktop;
}
//------------------------------------------------------------------------
XCursor* CWndSysHelper::CreateXCursor(IResObject* pResObject)
{
	XCursor* pCursors = new XCursor;
	// Create
	LPCSTR pString;
	long lRetval;
	int nAlpha;
	int nFrames;
	IBitmap* pCursor = NULL;

	pString = pResObject->GetPropertyString("src");
	if (pString && (NULL!=(pCursor=CImageLoader::GetInstance()->LoadImage(pString))))
		pCursors->SetBitmap(pCursor);
	else
	{
		delete pCursors;
		return NULL;
	}
	
	pResObject->GetPropertyLong("alpha", (long&)nAlpha, 256);
	pCursors->SetAlpha(nAlpha);

	pResObject->GetPropertyLong("mask", lRetval, TRUE);
	pCursors->SetMask(lRetval);

	pResObject->GetPropertyLong("animate", lRetval, FALSE);
	pCursors->SetAnimate(lRetval);

	pResObject->GetPropertyLong("interval", lRetval, 0);
	pCursors->SetInterval(lRetval);

	POINT pt;
	pResObject->GetArrayLong("hot", (long*)&pt, 0);
	pCursors->SetHots(pt);

	pResObject->GetPropertyLong("frames", (long&)nFrames, 0);
	pCursors->SetFrames(nFrames);

	RECT rc;
	SetRect(&rc, 0, 0, pCursors->GetBitmap()->GetWidth()/nFrames, pCursors->GetBitmap()->GetHeight());
	OffsetRect(&rc, pt.x, pt.y);
	pCursors->SetWindowPos(rc);

	return pCursors;
}
//------------------------------------------------------------------------
XWindow* CWndSysHelper::CreateDialogByResObject(XWindow* pParentWnd, IResObject* pResObject, int nID)
{
	if (!pResObject) return NULL;
	IResObject* pSubResObject;
	XWindow* pChildWnd;

	// 创建窗口
	XWindow* pWindow = CreateXWindow(pParentWnd, pResObject, nID);

	// 创建子窗口
	for(int i = 0; i < pResObject->GetSubObjectCount(); i ++)
	{
		pSubResObject = pResObject->GetSubObjectByIndex(i);
		if(!pSubResObject)
			goto err;

		
		if(pChildWnd = CreateXWindow(pWindow, pSubResObject, i))
		{
			//pWindow->AddWindow(pChildWnd);
		}
		
	}
	return pWindow;
	
err:
	TraceLn("CWndSysHelper::CreateDialogByResObject() Create failed!");
	return NULL;
}
//------------------------------------------------------------------------
XWindow* CWndSysHelper::CreateXWindow(XWindow* pParentWnd, IResObject* pResObject, int nID, XWindow* pSubclassWnd)
{
	if (!pParentWnd || !pResObject)
		return NULL;

	long lRetval;
	LPCSTR pString;
	IBitmap* pBmp = NULL;

	// 类型不明，不能创建
	LPCSTR szType = pResObject->GetTypeName();
	if (!szType || *szType == '\0')
		szType = pResObject->GetPropertyString("type");
	if (!szType)
	{
		//TraceColorLn(MY_TRACE_COLOR, "CWndSysHelper::CreateXWindow() object(\"%s\") not find 'type' property!");
		return NULL;
	}

	// 创建窗口
	if (stricmp(szType, "window") == 0)
	{
		XMainWnd* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XMainWnd(pParentWnd);
		else
			pWnd = (XMainWnd*)pSubclassWnd;
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		pWnd->SetMovable(TRUE);

		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}

	// 创建通用窗口
	if (stricmp(szType, "generalwindow") == 0)
	{
		XGeneralMainWnd* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XGeneralMainWnd(pParentWnd);
		else
			pWnd = (XGeneralMainWnd*)pSubclassWnd;
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		pWnd->SetWndTraits(XWT_GENERAL, TRUE);
		// 读取坐标列表，一共9个坐标，每个坐标4个参数
#define LONG_COUNT	XGeneralMainWnd::MaxAreaCount*4
		long pos[LONG_COUNT];
		if (LONG_COUNT > pResObject->GetArrayLong("paramlist"))
			return NULL;

		int nLen = pResObject->GetArrayLong("paramlist", (long*)&pos, LONG_COUNT);
		if (!pWnd->InitParamList(pos, LONG_COUNT))
			return NULL;
		
		long lBorderAlpha;
		if (pResObject->GetPropertyLong("borderalpha", lBorderAlpha))
		{
			pWnd->SetBorderAlpha(lBorderAlpha);
		}


		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	// 创建控件
	if (stricmp(szType, "control") == 0)
	{
		XControl* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XControl(pParentWnd);
		else
			pWnd = (XControl*)pSubclassWnd;
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	// 创建按钮
	else if (stricmp(szType, "button") == 0)
	{
		XButton* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XButton(pParentWnd);
		else
			pWnd = (XButton*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		// 加载按下，弹起等状态的图片
		pString = pResObject->GetPropertyString("upimg");
		if (pString && NULL != (pBmp = GetBitmap(pString)) )
			pWnd->SetBitmapUp(pBmp);

		pString = pResObject->GetPropertyString("downimg");
		if (pString && NULL != (pBmp = GetBitmap(pString)) )
			pWnd->SetBitmapDown(pBmp);

		pString = pResObject->GetPropertyString("hoverimg");
		if (pString && NULL != (pBmp = GetBitmap(pString)) )
			pWnd->SetBitmapHover(pBmp);
		
		pString = pResObject->GetPropertyString("disableimg");
		if (pString && NULL != (pBmp = GetBitmap(pString)) )
			pWnd->SetBitmapDisable(pBmp);
		
		//读取几种前景色
		long color[3];
		if (pResObject->GetArrayLong("downforecolor", color, 3))
		{			
			pWnd->SetDownForeColor(color);
			
		};

		if (pResObject->GetArrayLong("hoverforecolor", color, 3))
		{
			pWnd->SetHoverForeColor(color);				
		}		

		if (pResObject->GetArrayLong("disableforecolor", color, 3))
		{					
			pWnd->SetDisableForeColor(color);
		};

		if (pResObject->GetArrayLong("forecolor", color, 3))
		{					
			pWnd->SetUpForeColor(color);
		};


		pParentWnd->AddWindow(pWnd);
		return pWnd;	
	}
	// 创建标签
	else if (stricmp(szType, "label") == 0)
	{
		XLabel* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XLabel(pParentWnd);
		else
			pWnd = (XLabel*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);

		// 边框空白
		pResObject->GetPropertyLong("margin", lRetval, 2);
		pWnd->SetMargin(lRetval);
		
		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}

	// 创建单选钮
	else if (stricmp(szType, "radio") == 0)
	{
		XRadio* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XRadio(pParentWnd);
		else
			pWnd = (XRadio*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		// 单选钮各状态图标
		pString = pResObject->GetPropertyString("stateimg");
		if (pString && NULL != (pBmp = GetBitmap(pString)) )
			pWnd->SetStateBitmap(pBmp);
		
		// forecolor 前景(文字)颜色
		long color[3];
		pResObject->GetArrayLong("forecolor", color, 0);
		pWnd->SetForeColor(color[0], color[1], color[2]);
		
		pParentWnd->AddWindow(pWnd);
		return pWnd;	
	}
	// 创建单选钮群组
	else if (stricmp(szType, "radiogroup") == 0)
	{
		XRadioGroup* pWnd = new XRadioGroup();

		int nCheckIndex;
		pResObject->GetPropertyLong("checkindex", (long&)nCheckIndex, 0);
		
		// 创建子窗口
		XWindow* pChildWnd;
		IResObject* pSubResObject;
		for(int i = 0; i < pResObject->GetSubObjectCount(); i ++)
		{
			pSubResObject = pResObject->GetSubObjectByIndex(i);
			if (!pSubResObject)
				return NULL;
			
			if(pChildWnd = CreateXWindow(pParentWnd, pSubResObject, nID*10+i))
			{
				((XRadio*)pChildWnd)->SetGroup(pWnd);
				pWnd->AddRadio((XRadio*)pChildWnd);
				if (nCheckIndex == i)
					((XRadio*)pChildWnd)->SetState(XRadio::RS_CHECK);
				else
					((XRadio*)pChildWnd)->SetState(XRadio::RS_UNCHECK);
			}
		}
	}
	// 创建复选框
	else if (stricmp(szType, "checkbox") == 0)
	{
		XCheckBox* pWnd = NULL; 
		if (pSubclassWnd == NULL)
			pWnd = new XCheckBox(pParentWnd);
		else
			pWnd = (XCheckBox*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		// 复选框各状态图标
		pString = pResObject->GetPropertyString("stateimg");
		if (pString && NULL != (pBmp = GetBitmap(pString)) )
			pWnd->SetStateBitmap(pBmp);
		
		// forecolor 前景(文字)颜色
		long color[3];
		pResObject->GetArrayLong("forecolor", color, 0);
		pWnd->SetForeColor(color[0], color[1], color[2]);
		
		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	// 创建文本框
	else if (stricmp(szType, "edit") == 0)
	{
		XEdit* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XEdit(pParentWnd);
		else
			pWnd = (XEdit*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		// 读取坐标列表，一共9个坐标，每个坐标4个参数
		if (pResObject->GetArrayLong("paramlist") >= XEdit::MaxAreaCount * 4)
		{
			long pos[XEdit::MaxAreaCount*4];
			
			int nLen = pResObject->GetArrayLong("paramlist", (long*)&pos);
			if (!pWnd->InitParamList(pos, nLen))
				return NULL;
		}
		 
 
		// 是否支持多行
		pResObject->GetPropertyLong("multiline", lRetval, 0);
		pWnd->SetMultiLine(lRetval);
		
		// 是否支持自动换行
		pResObject->GetPropertyLong("autowrap", lRetval, 0);
		pWnd->SetAutoWrap(lRetval);
					
		// password，是否是密码框
		pResObject->GetPropertyLong("password", lRetval, FALSE);
		pWnd->SetPassword(lRetval);
		
		// readonly 是否只读状态
		pResObject->GetPropertyLong("readonly", lRetval, FALSE);
		pWnd->SetReadOnly(lRetval);
		
		// readonly 是否只读状态
		pResObject->GetPropertyLong("digitonly", lRetval, FALSE);
		pWnd->SetDigitOnly(lRetval);
		
		// 文本最大限制长度(1024)
		pResObject->GetPropertyLong("maxlen", lRetval, 1024);
		pWnd->SetMaxLen(lRetval);
		
		// 文本框边框空白
		pResObject->GetPropertyLong("margin", lRetval, 2);
		pWnd->SetMargin(lRetval);

		// 密码屏蔽字符
		pResObject->GetPropertyLong("passwordchar", lRetval, '*');
		pWnd->SetPasswordChar((char)lRetval);

		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	// 创建滑动条
	else if (stricmp(szType, "slidebar") == 0)
	{
		XSlideBar* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XSlideBar(pParentWnd);
		else
			pWnd = (XSlideBar*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		// 滑动块
		pString = pResObject->GetPropertyString("blockimg");
		if (pString && NULL != (pBmp = GetBitmap(pString)) )
			pWnd->SetBlockBitmap(pBmp);
		
		// 滑块矩形
		RECT rc;
		if (!pResObject->GetArrayLong("blockrect", (long*)&rc))
		{
			SetRect(&rc, 0, 0, pBmp->GetWidth(), pBmp->GetHeight());
		}
		pWnd->SetBlockRect(&rc);
		
		// 滑动条类型(水平，垂直等)
		pResObject->GetPropertyLong("style", lRetval, 0);
		pWnd->SetStyle(lRetval);
		
		// 范围
		long range[2];
		if (!pResObject->GetArrayLong("range", range))
		{
			range[0] = 0;
			range[1] = 99;
		}
		pWnd->SetRange(range[0], range[1]);
		
		// 当前位置
		pResObject->GetPropertyLong("pos", lRetval, 0);
		pWnd->SetPos(lRetval);
		
		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	// 创建滚动条
	else if (stricmp(szType, "scrollbar") == 0)
	{
		// 下面的顺序不能颠倒
		XScrollBar* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XScrollBar(pParentWnd);
		else
			pWnd = (XScrollBar*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		// 滑动条类型(水平，垂直等)
		pResObject->GetPropertyLong("style", lRetval, 0);
		pWnd->SetStyle(lRetval);
		 
		// 范围
		long range[2];
		if (!pResObject->GetArrayLong("range", range))
		{
			range[0] = 0;
			range[1] = 99;
		}
		pWnd->SetScrollRange(range[0], range[1]);
		
		// 滑块矩形
		SIZE size;
		if (!pResObject->GetArrayLong("size", (long*)&size))
		{
			size.cx = size.cy = GetGameDesktop()->GetSysScrollBarBitmap()->GetHeight();
		}
		pWnd->SetSize(size);
		
		// 当前位置
		pResObject->GetPropertyLong("pos", lRetval, 0);
		pWnd->SetScrollPos(lRetval, FALSE);

		// 可见条目数
		pResObject->GetPropertyLong("visiblecount", lRetval, 0);
		pWnd->SetVisibleCount(lRetval);

		pWnd->Refresh();
		
		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	else if (stricmp(szType, "progressbar") == 0)
	{
		XProgressBar* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XProgressBar(pParentWnd);
		else
			pWnd = (XProgressBar*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		// 进度条类型(水平，垂直等)
		pResObject->GetPropertyLong("style", lRetval, 0);
		pWnd->SetStyle(lRetval);
		
		// 范围
		long range[2];
		if (!pResObject->GetArrayLong("range", range))
		{
			range[0] = 0;
			range[1] = 99;
		}
		pWnd->SetRange(range[0], range[1]);
		
		// 当前位置
		pResObject->GetPropertyLong("pos", lRetval, 0);
		pWnd->SetPos(lRetval);
		
		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	else if (stricmp(szType, "staticbitmap") == 0)
	{
		XStaticBitmap* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XStaticBitmap(pParentWnd);
		else
			pWnd = (XStaticBitmap*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		long stretchrc;		
		if (pResObject->GetPropertyLong("stretch", stretchrc))			  
		{			
			pWnd->SetStretch(stretchrc);
		}
		

		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	else if (stricmp(szType, "animate") == 0)
	{
		XAnimate* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XAnimate(pParentWnd);
		else
			pWnd = (XAnimate*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		pResObject->GetPropertyLong("frame", lRetval, 1);
		pWnd->SetFrameCount(lRetval);
		pResObject->GetPropertyLong("delay", lRetval, 200);
		pWnd->SetDelay(lRetval);

		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	else if (stricmp(szType, "itemcontainer") == 0)
	{
		XItemContainer* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XItemContainer(pParentWnd);
		else
			pWnd = (XItemContainer*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		// 聊天框边框空白
		pResObject->GetPropertyLong("margin", lRetval, 2);
		pWnd->SetMargin(lRetval);
		
		// 是否自动回行
		pResObject->GetPropertyLong("autowrap", lRetval, 0);
		pWnd->SetAutoWrap(lRetval);

		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	else if (stricmp(szType, "chat") == 0)
	{
		XChat* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XChat(pParentWnd);
		else
			pWnd = (XChat*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		// 聊天框边框空白
		pResObject->GetPropertyLong("margin", lRetval, 2);
		pWnd->SetMargin(lRetval);
		
		// 是否自动回行
		pResObject->GetPropertyLong("autowrap", lRetval, 0);
		pWnd->SetAutoWrap(lRetval);
		
		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	else if (stricmp(szType, "html") == 0)
	{
		XHtml* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XHtml(pParentWnd);
		else
			pWnd = (XHtml*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		// 聊天框边框空白
		pResObject->GetPropertyLong("margin", lRetval, 2);
		pWnd->SetMargin(lRetval);
		
		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	else if (stricmp(szType, "talk") == 0)
	{
		XTalk* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XTalk(pParentWnd);
		else
			pWnd = (XTalk*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		// 对话框边框空白
		pResObject->GetPropertyLong("margin", lRetval, 2);
		pWnd->SetMargin(lRetval);
		
		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	else if (stricmp(szType, "listbox") == 0)
	{
		XListBox* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XListBox(pParentWnd);
		else
			pWnd = (XListBox*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		if (pResObject->GetSubObjectCount())
		{
			XWindow* pScroll = CreateXWindow(pWnd, pResObject->GetSubObjectByIndex(0), 0);
			pWnd->SetScrollBar(pScroll);
			static_cast<XScrollBar*>(pScroll)->Attach(static_cast<XWindow*>(pWnd));
		}
		
		// 列表框内容
		if (pString = pResObject->GetPropertyString("context"))
			pWnd->ParseStringToListBox(pString);
		
		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	// 创建链接文本
	else if (stricmp(szType, "linktext") == 0)
	{
		XLinkText* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XLinkText(pParentWnd);
		else
			pWnd = (XLinkText*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		long color[3];
		pResObject->GetArrayLong("notvisitedcolor", color, 0); // 未访问过的链接颜色
		pWnd->SetNotVisitedColor(color[0], color[1], color[2]);
		pResObject->GetArrayLong("visitedcolor", color, 0); // 访问过的链接颜色
		pWnd->SetVisitedColor(color[0], color[1], color[2]);
		pResObject->GetArrayLong("rollovercolor", color, 0); // 悬停时的颜色
		pWnd->SetRollOverColor(color[0], color[1], color[2]);
		pWnd->SetURL(pResObject->GetPropertyString("url"));
		
		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	// 某些情况下可能会创建自己私有的tooltip
	else if (stricmp(szType, "tooltip") == 0)
	{
		XToolTip* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XToolTip(pParentWnd);
		else
			pWnd = (XToolTip*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		pParentWnd->AddWindow(pWnd);
		return pWnd;
	}
	// 创建状态按钮
	else if (stricmp(szType, "statebutton") == 0)
	{
		XStateButton* pWnd = NULL;
		if (pSubclassWnd == NULL)
			pWnd = new XStateButton(pParentWnd);
		else
			pWnd = (XStateButton*)pSubclassWnd;
		
		pWnd->SetParent(pParentWnd);
		CreateStockProperty(pWnd, pResObject, nID);
		
		// 加载按下，弹起等状态的图片
		pString = pResObject->GetPropertyString("state2img");
		if (pString && NULL != (pBmp = GetBitmap(pString)) )
			pWnd->SetBitmapState2(pBmp);

		pString = pResObject->GetPropertyString("state1img");
		if (pString && NULL != (pBmp = GetBitmap(pString)) )
			pWnd->SetBitmapState1(pBmp);

		pParentWnd->AddWindow(pWnd);
		return pWnd;	
	}

	return NULL;
}
//------------------------------------------------------------------------
// 创建常规属性
void CWndSysHelper::CreateStockProperty(XWindow* pWnd, IResObject* pResObject, int nID)
{
	long lRetval;
	LPCSTR pString;
	int nAlpha;
	IBitmap* pBmp;
	RECT rc;
	SetRectEmpty(&rc);
	XWindow* pParentWnd = pWnd->GetParent();
	if (!pParentWnd)
	{
		TraceLn( "CWndSysHelper::CreateStockProperty,Parent window is NULL");
		return;
	}
	pWnd->SetResObject(pResObject);

	pWnd->SetID(nID);

	// src (背景图片)
	pString = pResObject->GetPropertyString("src");
	if (pString && NULL != (pBmp = GetBitmap(pString)) )
		pWnd->SetBitmap(pBmp);

	// rect (left,top,width,height)
	if (!pResObject->GetArrayLong("rect", (long*)&rc)) // 不存在
	{
		if (pBmp) // 有背景图片，则以背景图片的大小构建矩形
		{
			SetRect(&rc, 0, 0, pBmp->GetWidth(), pBmp->GetHeight());
		}
		else
		{
			TraceLn("CWndSysHelper::CreateXWindow() object('%s') not find 'rect' property!", pResObject->GetObjectName());
			OutputDebugString("not find 'rect' property!\r\n");
		}
	}
	rc.right += rc.left;
	rc.bottom += rc.top;
	pWnd->SetWindowPos(rc);

	// caption (标题，文本)
	if (pString = pResObject->GetPropertyString("caption"))
		pWnd->SetWindowText(pString);
	else
	{
		//if (pString = pResObject->GetObjectName())
		pString = "";
		pWnd->SetWindowText(pString);
	}

	// forecolor 前景(文字)颜色
	long color[3];
	if (pResObject->GetPropertyString("forecolor"))
	{
		pResObject->GetArrayLong("forecolor", color, 3);
		pWnd->SetForeColor(WPixel(color[0], color[1], color[2]));
	}
	
	// backcolor 背景颜色
	if (pResObject->GetPropertyString("backcolor"))
	{
		pResObject->GetArrayLong("backcolor", color, 3);
		pWnd->SetBackColor(WPixel(color[0], color[1], color[2]));
	}

	// align
	pResObject->GetPropertyLong("align", lRetval, 0);
	pWnd->SetAlign(lRetval);

	// alpha
	pResObject->GetPropertyLong("alpha", (long&)nAlpha, 256);
	pWnd->SetAlpha(nAlpha);

	// mask
	pResObject->GetPropertyLong("mask", lRetval, TRUE);
	pWnd->SetMask(lRetval);

	// modal
	pResObject->GetPropertyLong("modal", lRetval, FALSE);
	pWnd->SetModal(lRetval);

	// enabled
	if (pParentWnd->IsEnabled())
	{
		pResObject->GetPropertyLong("enabled", lRetval, TRUE);
		pWnd->SetWndTraits(XWT_ENABLED, lRetval);
	}
	else
	{
		pWnd->SetWndTraits(XWT_ENABLED, FALSE);
	}

	// movable
	pResObject->GetPropertyLong("movable", lRetval, FALSE);
	pWnd->SetMovable(lRetval);

	// visible
	if (pParentWnd->IsVisible())
	{
		pResObject->GetPropertyLong("visible", lRetval, TRUE);
		pWnd->SetWndTraits(XWT_VISIBLE, lRetval);
		// 2005.4.29 by PeakGao 为内存优化而修改
		//if (lRetval == 1)
		//	pWnd->LoadAllImage();
	}
	else
	{
		pWnd->SetWndTraits(XWT_VISIBLE, FALSE);
	}

	//ToolTip索引
	if (pResObject->GetPropertyLong("tooltip", lRetval, -1) == TRUE)
	pWnd->SetToolTipIndex(lRetval);

	if (pWnd->IsMainWindow())
	{
		pResObject->GetPropertyLong("escape", lRetval, 0);
		pWnd->SetWndTraits(XWT_ESCAPE, lRetval);
	}
	
	// topmost
	pResObject->GetPropertyLong("topmost", lRetval, FALSE);
	pWnd->SetTopmost(lRetval);
}
//------------------------------------------------------------------------
IBitmap* CWndSysHelper::GetBitmap(LPCSTR szFile)
{
	//return NULL;
	return CImageLoader::GetInstance()->LoadImage(szFile);
}
//------------------------------------------------------------------------
// 整个窗口系统的消息过程
DWORD CWndSysHelper::OnMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	DWORD dwReturn = NOTHANDLE;
	XDesktop* pGameDesktop = GetGameDesktop();
	if (pGameDesktop) // 窗口系统已创建好，桌面存在
		dwReturn = pGameDesktop->DispatchMessage(message, wParam, lParam);
	return dwReturn;
}
//------------------------------------------------------------------------
void CWndSysHelper::OnPaint(ICanvas* pCanvas)
{
	GetGameDesktop()->OnPaint(pCanvas);
}
//------------------------------------------------------------------------