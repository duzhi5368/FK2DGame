/**
*	created:		2012-6-23   23:23
*	filename: 		XHtml
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
inline void XHtml::XHtmlScrollBar::LineLeftUp()
{
	int nPos = GetScrollPos();
	nPos -= 16;
	SetScrollPos(nPos);
}
//------------------------------------------------------------------------
inline void XHtml::XHtmlScrollBar::LineRightDown()
{
	int nPos = GetScrollPos();
	nPos += 16;
	SetScrollPos(nPos);
}
//------------------------------------------------------------------------
bool XHtml::FileHelper::Read(const char* szFileName, Lines& lines)
{
	FILE* hFile = fopen(szFileName, "r");
	if (!hFile)
		return false;
	
	lines.clear();
	char buffer[buffer_size];
	
	while (fgets(buffer, buffer_size, hFile))
		lines.push_back(buffer);
	
	fclose(hFile);
	return true;
}
//------------------------------------------------------------------------
void XHtml::XItemJumpText::Emit()
{
	if (m_pXHtml)
	{
		m_pXHtml->SetJumpVisited(m_strLinkText.c_str(), TRUE);
		m_pXHtml->Navigate(m_strLinkText.c_str());
	}
}
//------------------------------------------------------------------------
BOOL XHtml::CTagParserJump::Emit()
{
	XItemContainer* pContainer = m_pLineParser->GetItemContainer();
	if (pContainer)
	{
		ITTFont* pFont = XFontCreate::GetInstance()->CreateFont(context.strFontName.c_str(), context.nFontSize, 
			context.bBlod, context.bItalic, context.bUnderline, FALSE, 1024);
		
		// 新建一个文本条目，并加入条目数组
		XItemJumpText* pItem = new XItemJumpText;
		pItem->SetXHtmlPtr(m_pXHtml);
		pItem->SetText(context.strText.c_str());
		pItem->SetFont(pFont);
		
		pItem->SetNotVisitedColor(WPixel(0,255,0));
		pItem->SetVisitedColor(WPixel(0,128,0));
		pItem->SetRolloverColor(WPixel(255,0,0));
		
		pItem->SetLinkText(context.strLinkText.c_str());
		BOOL bVisited = FALSE;
		if (m_pXHtml)
			bVisited = m_pXHtml->GetJumpVisited(context.strLinkText.c_str());
		pItem->SetVisited(bVisited);
		if (bVisited)
			pItem->SetTextColor(pItem->GetVisitedColor());
		else
			pItem->SetTextColor(pItem->GetNotVisitedColor());
		
		if (!pContainer->AddItem(static_cast<XItem*>(pItem), FALSE))
		{
			TraceColorLn(MY_TRACE_COLOR, "CTagParserJump::Parse, AddItem return FALSE");
			return FALSE;
		}
		
		if (!bVisited && m_pXHtml)
			m_pXHtml->AddJumpToHashMap(context.strLinkText.c_str(), bVisited);
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
BOOL XHtml::CTagParserBookmarkEx::Emit()
{
	XItemContainer* pContainer = m_pLineParser->GetItemContainer();
	if (pContainer)
	{
		ITTFont* pFont = XFontCreate::GetInstance()->CreateFont(context.strFontName.c_str(), context.nFontSize, 
			context.bBlod, context.bItalic, context.bUnderline, FALSE, 1024);
		
		// 新建一个文本条目，并加入条目数组
		XItemBookmarkText* pItem = new XItemBookmarkText;
		pItem->SetText(context.strText.c_str());
		pItem->SetFont(pFont);
		pItem->SetTextColor(context.clrText);
		
		pItem->SetBookmark(context.strBookmark.c_str());
		if (m_pXHtml)
			m_pXHtml->AddBookmarkToHashmap(context.strBookmark.c_str());
		
		if (!pContainer->AddItem(static_cast<XItem*>(pItem), FALSE))
		{
			TraceColorLn(MY_TRACE_COLOR, "CTagParserBookmarkEx::Parse, AddItem return FALSE");
			return FALSE;
		}
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
XHtml::XHtml()
{
	m_strWorkingDir.clear();
	m_strFileName.clear();
	m_NavigateList.clear();
	m_nCurNavigateIdx = 0;

	m_pHScrollBar = new XHtmlScrollBar(this);
	m_pHScrollBar->Attach(this);
	AddWindow(m_pHScrollBar);
	m_pHScrollBar->SetVisible(FALSE);
	m_pHScrollBar->SetStyle(XScrollBar::S_HORZ);
	SIZE size;
	size.cx = size.cy = GetDesktop()->GetSysScrollBarBitmap()->GetHeight();
	m_pHScrollBar->SetSize(size);
	m_pHScrollBar->SetScrollRange(0, 0);
	m_pHScrollBar->SetScrollPos(0);

	m_pVScrollBar = new XHtmlScrollBar(this);
	m_pVScrollBar->Attach(this);
	AddWindow(m_pVScrollBar);
	m_pVScrollBar->SetStyle(XScrollBar::S_VERT);
	size.cx = size.cy = GetDesktop()->GetSysScrollBarBitmap()->GetHeight();
	m_pVScrollBar->SetSize(size);
	m_pVScrollBar->SetScrollRange(0, 0);
	m_pVScrollBar->SetScrollPos(0);

	m_pContainer = new XItemContainer(this);
	m_pContainer->SetHScrollBar(m_pHScrollBar);
	m_pContainer->SetVScrollBar(m_pVScrollBar);
	AddWindow(m_pContainer);
	
	m_LineParser.SetItemContainer(m_pContainer);
	
	RegisterAllParser();
}
//------------------------------------------------------------------------
XHtml::XHtml(XWindow* pParentWnd) : XWindow(pParentWnd)
{
	m_strWorkingDir.clear();
	m_strFileName.clear();
	m_NavigateList.clear();
	m_nCurNavigateIdx = 0;
	
	m_pHScrollBar = new XHtmlScrollBar(this);
	m_pHScrollBar->Attach(this);
	AddWindow(m_pHScrollBar);
	m_pHScrollBar->SetVisible(FALSE);
	m_pHScrollBar->SetStyle(XScrollBar::S_HORZ);
	SIZE size;
	size.cx = size.cy = GetDesktop()->GetSysScrollBarBitmap()->GetHeight();
	m_pHScrollBar->SetSize(size);
	m_pHScrollBar->SetScrollRange(0, 0);
	m_pHScrollBar->SetScrollPos(0);

	m_pVScrollBar = new XHtmlScrollBar(this);
	m_pVScrollBar->Attach(this);
	AddWindow(m_pVScrollBar);
	m_pVScrollBar->SetVisible(FALSE);
	m_pVScrollBar->SetStyle(XScrollBar::S_VERT);
	size.cx = size.cy = GetDesktop()->GetSysScrollBarBitmap()->GetHeight();
	m_pVScrollBar->SetSize(size);
	m_pVScrollBar->SetScrollRange(0, 0);
	m_pVScrollBar->SetScrollPos(0);

	m_pContainer = new XItemContainer(this);
	m_pContainer->SetHScrollBar(m_pHScrollBar);
	m_pContainer->SetVScrollBar(m_pVScrollBar);
	AddWindow(m_pContainer);

	m_LineParser.SetItemContainer(m_pContainer);

	RegisterAllParser();
}
//------------------------------------------------------------------------
XHtml::~XHtml()
{
	Clear();
}
//------------------------------------------------------------------------
void XHtml::Clear()
{
	m_BookmarkList.clear();
	m_JumpList.clear();
	m_NavigateList.clear();
}
//------------------------------------------------------------------------
void XHtml::SetWorkingDir(LPCSTR szWorkingDir)
{
	if (szWorkingDir == NULL || szWorkingDir[0] == '\0')
		return;
	
	m_strWorkingDir = szWorkingDir;
	unsigned char c = *(m_strWorkingDir.end() - 1);
	if (c != '\\' && c != '//')
		m_strWorkingDir += '\\';
}
//------------------------------------------------------------------------
void XHtml::RegisterAllParser()
{
	m_LineParser.RegisterTagParser(&m_TagParserParagraph);
	m_LineParser.RegisterTagParser(&m_TagParserText);
	m_TagParserJump.SetXHtmlPtr(this);
	m_LineParser.RegisterTagParser(&m_TagParserJump);
	m_LineParser.RegisterTagParser(&m_TagParserBookmark);
	m_TagParserBookmark.SetXHtmlPtr(this);
	m_LineParser.RegisterTagParser(&m_TagParserImage);
}
//------------------------------------------------------------------------
void XHtml::RegisterTagParser(CTagParser* pTagParser)
{
	m_LineParser.RegisterTagParser(pTagParser);
}
//------------------------------------------------------------------------
void XHtml::SetWindowPos(RECT& rc)
{
	XWindow::SetWindowPos(rc);
	
	RECT rcHScroll, rcVScroll, rcContainer;
	CopyRect(&rcHScroll, &rc);
	CopyRect(&rcVScroll, &rc);
	CopyRect(&rcContainer, &rc);
	SIZE sizeH = {0,0} ;
	SIZE sizeV = {0,0} ;
	if (m_pHScrollBar)
	{
		m_pHScrollBar->GetSize(sizeH);
	}
	if (m_pVScrollBar)
	{
		m_pVScrollBar->GetSize(sizeV);
	}

	if (m_pHScrollBar)
	{
		rcHScroll.left = m_rcWnd.left;
		rcHScroll.right = m_rcWnd.right - sizeV.cx;
		rcHScroll.top = m_rcWnd.bottom - sizeH.cy;
		rcHScroll.bottom = rcHScroll.top + sizeH.cy;
		m_pHScrollBar->SetWindowPos(rcHScroll);
		m_pHScrollBar->SetSize(sizeH);
		m_pHScrollBar->Refresh();
		rcContainer.bottom -= sizeH.cy;
	}
	if (m_pVScrollBar)
	{
		rcVScroll.left = m_rcWnd.right - sizeV.cx;
		rcVScroll.right = rcVScroll.left + sizeV.cx;
		rcVScroll.top = m_rcWnd.top;
		rcVScroll.bottom = m_rcWnd.bottom - sizeH.cy;
		m_pVScrollBar->SetWindowPos(rcVScroll);
		m_pVScrollBar->SetSize(sizeV);
		m_pVScrollBar->Refresh();
		rcContainer.right -= sizeV.cx;
	}
	m_pContainer->SetWindowPos(rcContainer);
}
//------------------------------------------------------------------------
DWORD XHtml::OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt)
{
	return m_pContainer->OnMouseWheel(nButton, nShift, zDelta, pt);
}
//------------------------------------------------------------------------
DWORD XHtml::OnKeyDown(UINT nKeyCode, UINT nShift)
{
	return m_pContainer->OnKeyDown(nKeyCode, nShift);
}
//------------------------------------------------------------------------
DWORD XHtml::OnKeyUp(UINT nKeyCode, UINT nShift)
{
	return m_pContainer->OnKeyUp(nKeyCode, nShift);
}
//------------------------------------------------------------------------
// 添加一行xhtm脚本
BOOL XHtml::AddLine(LPCSTR szLineText)
{
	string line = szLineText;
	if ('\n' == (BYTE)line.at(line.length()-1)) // 删除最后一个换行符 '\n'
		line.erase(line.end()-1);
	return m_LineParser.ParseLine(line.c_str());
}
//------------------------------------------------------------------------
BOOL XHtml::CanBack()
{
	return (m_nCurNavigateIdx > 0 && m_nCurNavigateIdx < (int)m_NavigateList.size());
}
//------------------------------------------------------------------------
BOOL XHtml::CanForward()
{
	return (m_nCurNavigateIdx >= 0 && m_nCurNavigateIdx < (int)m_NavigateList.size()-1);
}
//------------------------------------------------------------------------
void XHtml::GoBack()
{
	if (CanBack())
	{
		JumpToUrl(m_NavigateList[--m_nCurNavigateIdx].c_str());
	}
}
//------------------------------------------------------------------------
void XHtml::GoForward()
{
	if (CanForward())
	{
		JumpToUrl(m_NavigateList[++m_nCurNavigateIdx].c_str());
	}
}
//------------------------------------------------------------------------
// 清除forward列表，再导航
void XHtml::Navigate(LPCSTR szUrl)
{
	if (CanForward()) // 删除列表后面的导航地址
	{
		m_NavigateList.erase(m_NavigateList.begin() + m_nCurNavigateIdx + 1, m_NavigateList.end());
	}
	
	m_NavigateList.push_back(szUrl);
	m_nCurNavigateIdx = m_NavigateList.size() - 1;
	JumpToUrl(szUrl);
}
//------------------------------------------------------------------------
// 仅仅导航，不处理导航列表
void XHtml::JumpToUrl(LPCSTR szUrl)
{
	string strUrl = szUrl;
	int nPos = strUrl.find_first_of('#');
	if (nPos == strUrl.npos) // 没有#,说明导航到指定的文件
	{
		LoadFile(strUrl.c_str());
	}
	else
	{
		if (nPos == 0) // 说明导航到本文件中的对应标签
		{
			string strBookmark = strUrl.substr(nPos+1, strUrl.length()-nPos-1);
			LocateBookmark(strBookmark.c_str());
		}
		else // 说明导航到指定文件的对应标签
		{
			string strFileName = strUrl.substr(0, nPos);
			LoadFile(strFileName.c_str());
			string strBookmark = strUrl.substr(nPos+1, strUrl.length()-nPos-1);
			LocateBookmark(strBookmark.c_str());
		}
	}
}
//------------------------------------------------------------------------
void XHtml::LoadFile(LPCSTR szFileName)
{
	if (!szFileName || szFileName[0] == '\0') return;
	string str;
	if (szFileName[1] != ':')
		str = m_strWorkingDir;
	str += szFileName;
	m_strFileName = str;

	Lines	lines;
	if (!FileHelper::Read(str.c_str(), lines))
	{
		TraceLn("FileHelper::Read( %s ) failed!", str.c_str());
		return;
	}

	m_pContainer->Clear();

	LinesPtr _end = lines.end();
	for (LinesPtr it=lines.begin(); it!=_end; ++it)
	{
		if ('\n' == (BYTE)(*it).at((*it).length()-1)) // 删除最后一个换行符 '\n'
			(*it).erase((*it).end()-1);
		m_LineParser.ParseLine((*it).c_str());
	}
	InvalidRect();
}
//------------------------------------------------------------------------
// 为了让标签在hash表中的字符串唯一，书签记录为：path+filename+#+bookmark
// 如：e:\hxonline\xhtm\page1.xhtm#聊天介绍
void XHtml::AddBookmarkToHashmap(LPCSTR szBookmark)
{
	string str = m_strFileName;
	str += "#";
	str += szBookmark;
	hash_map<string, int>::iterator it = m_BookmarkList.find(str);
	if (it != m_BookmarkList.end())
		return;
	int nLineCount = m_pContainer->GetLineCount();
	m_BookmarkList[str] = nLineCount - 1;
}
//------------------------------------------------------------------------
// 定位到某个标签
void XHtml::LocateBookmark(LPCSTR szBookmark)
{
	string str = m_strFileName;
	str += "#";
	str += szBookmark;
	hash_map<string, int>::iterator it = m_BookmarkList.find(str);
	if (it != m_BookmarkList.end())
	{
		int nLineIndex = (*it).second;
		m_pContainer->BringToTopLine(nLineIndex);
	}
}
//------------------------------------------------------------------------
// 添加一个跳转到hash映射表
void XHtml::AddJumpToHashMap(LPCSTR szJump, BOOL bVisited)
{
	string str = m_strFileName;
	str += szJump;
	hash_map<string, BOOL>::iterator it = m_JumpList.find(str);
	if (it != m_JumpList.end())
		return;
	m_JumpList[str] = bVisited;
}
//------------------------------------------------------------------------
// 设置某个跳转的是否已经访问的属性
void XHtml::SetJumpVisited(LPCSTR szJump, BOOL bVisited)
{
	string str = m_strFileName;
	str += szJump;
	hash_map<string, BOOL>::iterator it = m_JumpList.find(str);
	if (it != m_JumpList.end())
	{
		(*it).second = bVisited;
	}
}
//------------------------------------------------------------------------
// 得到某个跳转的是否已经访问的属性
BOOL XHtml::GetJumpVisited(LPCSTR szJump)
{
	string str = m_strFileName;
	str += szJump;
	hash_map<string, BOOL>::iterator it = m_JumpList.find(str);
	if (it != m_JumpList.end())
	{
		return (*it).second;
	}
	return FALSE;
}
//------------------------------------------------------------------------