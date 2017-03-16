/**
*	created:		2012-6-23   23:29
*	filename: 		XTalk
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		用在与任务NPC说话时的文字显示
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
void XTalk::XItemTaskText::Emit()
{
	if (m_pXTalk)
	{
		m_pXTalk->OnTaskTextClick(m_strLinkText.c_str());
	}
}
//------------------------------------------------------------------------
BOOL XTalk::CTagParserTaskText::Emit()
{
	XItemContainer* pContainer = m_pLineParser->GetItemContainer();
	if (pContainer)
	{
		ITTFont* pFont = XFontCreate::GetInstance()->CreateFont(context.strFontName.c_str(), context.nFontSize, 
			context.bBlod, context.bItalic, context.bUnderline, FALSE, 1024);
		
		// 新建一个文本条目，并加入条目数组
		XItemTaskText* pItem = new XItemTaskText;
		pItem->SetTalkPtr(m_pXTalk);
		pItem->SetText(context.strText.c_str());
		pItem->SetFont(pFont);

		pItem->SetNotVisitedColor(WPixel(0,255,0));
		pItem->SetVisitedColor(WPixel(255,0,0));
		pItem->SetRolloverColor(WPixel(255,0,0));
		pItem->SetTextColor(pItem->GetNotVisitedColor());
		
		pItem->SetLinkText(context.strLinkText.c_str());
		pItem->SetVisited(FALSE);
		
		if (!pContainer->AddItem(static_cast<XItem*>(pItem), FALSE))
		{
			TraceColorLn(MY_TRACE_COLOR, "CTagParserTaskItem::Parse, AddItem return FALSE");
			return FALSE;
		}
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
XTalk::XTalk()
{
    SIZE size;
	m_pVScrollBar = new XTalkScrollBar(this);
	m_pVScrollBar->Attach(this);
	AddWindow(m_pVScrollBar);
	m_pVScrollBar->SetStyle(XScrollBar::S_VERT);
	size.cx = size.cy = GetDesktop()->GetSysScrollBarBitmap()->GetHeight();
	m_pVScrollBar->SetSize(size);
	m_pVScrollBar->SetScrollRange(0, 0);
	m_pVScrollBar->SetScrollPos(0);

	m_pContainer = new XItemContainer(this);
	m_pContainer->SetVScrollBar(m_pVScrollBar);
	AddWindow(m_pContainer);
	
	m_LineParser.SetItemContainer( m_pContainer) ;
	
	RegisterAllParser();
}
//------------------------------------------------------------------------
XTalk::XTalk(XWindow* pParentWnd) : XWindow(pParentWnd)
{
    SIZE size;
	m_pVScrollBar = new XTalkScrollBar(this);
	m_pVScrollBar->Attach(this);
	AddWindow(m_pVScrollBar);
	m_pVScrollBar->SetVisible(FALSE);
	m_pVScrollBar->SetStyle(XScrollBar::S_VERT);
	size.cx = size.cy = GetDesktop()->GetSysScrollBarBitmap()->GetHeight();
	m_pVScrollBar->SetSize(size);
	m_pVScrollBar->SetScrollRange(0, 0);
	m_pVScrollBar->SetScrollPos(0);

	m_pContainer = new XItemContainer(this);
	m_pContainer->SetVScrollBar(m_pVScrollBar);
	AddWindow(m_pContainer);

	m_LineParser.SetItemContainer( m_pContainer) ;

	RegisterAllParser();
}
//------------------------------------------------------------------------
XTalk::~XTalk()
{

}
//------------------------------------------------------------------------
void XTalk::Clear()
{
	m_pContainer->Clear();
}
//------------------------------------------------------------------------
void XTalk::RegisterAllParser()
{
	m_LineParser.RegisterTagParser(&m_TagParserParagraph);
	m_LineParser.RegisterTagParser(&m_TagParserText);
	m_LineParser.RegisterTagParser(&m_TagParserTaskText);
	m_TagParserTaskText.SetTalkPtr(this);
	m_LineParser.RegisterTagParser(&m_TagParserImage);
}
//------------------------------------------------------------------------
void XTalk::RegisterTagParser(CTagParser* pTagParser)
{
	m_LineParser.RegisterTagParser(pTagParser);
}
//------------------------------------------------------------------------
DWORD XTalk::OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt)
{
	return m_pContainer->OnMouseWheel(nButton, nShift, zDelta, pt);
}
//------------------------------------------------------------------------
// 添加一行xhtm脚本
BOOL XTalk::AddLine(LPCSTR szLineText)
{
	string line = szLineText;
	if ('\n' == (BYTE)line.at(line.length()-1)) // 删除最后一个换行符 '\n'
		line.erase(line.end()-1);
	return m_LineParser.ParseLine(line.c_str());
}
//------------------------------------------------------------------------
void XTalk::SetWindowPos(RECT& rc)
{
	XWindow::SetWindowPos(rc);
	
	RECT rcVScroll, rcContainer;
	CopyRect(&rcVScroll, &rc);
	CopyRect(&rcContainer, &rc);
	SIZE sizeH = {0,0} ;
	SIZE sizeV = {0,0} ;
	if (m_pVScrollBar)
	{
		m_pVScrollBar->GetSize(sizeV);
	}
	
	if (m_pVScrollBar)
	{
		rcVScroll.left = m_rcWnd.left;
		rcVScroll.right = rcVScroll.left + sizeV.cx;
		rcVScroll.top = m_rcWnd.top;
		rcVScroll.bottom = m_rcWnd.bottom;
		m_pVScrollBar->SetWindowPos(rcVScroll);
		m_pVScrollBar->SetSize(sizeV);
		m_pVScrollBar->Refresh();
		rcContainer.left += sizeV.cx;
	}
	m_pContainer->SetWindowPos(rcContainer);
	m_pContainer->SetMaxWidth(rcContainer.right - rcContainer.left);
	m_pContainer->SetMaxHeight(rcContainer.bottom - rcContainer.top);
}
//------------------------------------------------------------------------