/**
*	created:		2012-6-28   9:04
*	filename: 		PromptWnd
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现系统提示信息窗口
*/
//------------------------------------------------------------------------
#include "../Include/PromptWnd.h"
#include "../include/WindowCreator.h"
//------------------------------------------------------------------------
CPromptWnd::CPromptWnd(void)
{
	m_nType = REMINDER_ADD;
	m_clrText = WPixel(255, 255, 255);
	
	// 销毁字体
	if (m_pFont != NULL)
	{
		XFontCreate::GetInstance()->ReleaseFont(m_pFont);
	}	

	// 创建字体
	m_pFont = XFontCreate::GetInstance()->CreateFont("黑体", 14, TRUE, FALSE, FALSE, TRUE);	

	if (m_pFont != NULL)
	{
		m_nLineHeight = m_pFont->GetHeight();
	}
}
//------------------------------------------------------------------------
CPromptWnd::~CPromptWnd(void)
{
}
//------------------------------------------------------------------------
void CPromptWnd::SetWindowPos(RECT& rc)
{	
	XMainWnd::SetWindowPos(rc);
	CReminder::SetPos(rc);
}

//-------------------------------------------------------------------
// 描  述：画图 
//-------------------------------------------------------------------
void CPromptWnd::OnDraw(ICanvas * pCanvas, RECT & rcClip)
{	
	if (pCanvas == NULL)
	{
		return;
	}

	RECT rc = m_rcWnd;
	int nPos = m_rcWnd.top;	
	
	// 显示所有的信息项
	for (MessageListItr itr = m_messages.begin(); itr != m_messages.end(); itr++)
	{  
		rc.top = nPos;
		rc.bottom = rc.top + m_nLineHeight;

		pCanvas->DrawClipText(m_pFont, m_rcWnd.left + 3, nPos, 
			(*itr).strText.c_str(), (*itr).clrText, rc, FONT_NOALPHA);

		nPos += m_nLineHeight;
	}
}

//-------------------------------------------------------------------
// 描  述：鼠标键按下 
//-------------------------------------------------------------------
DWORD CPromptWnd::OnMouseDown(UINT nButton, UINT nShift, POINT pt)
{
	if (m_messages.size() > 0)
	{
		//ClearMessage();
		return 0;
	}
	else
	{
		return NOTHANDLE;
	}	
}

//-------------------------------------------------------------------
// 描  述：鼠标键释放 
//-------------------------------------------------------------------
DWORD CPromptWnd::OnMouseUp(UINT nButton, UINT nShift, POINT pt)
{
	return NOTHANDLE;
}

//-------------------------------------------------------------------
// 描  述：是否透明
//-------------------------------------------------------------------
BOOL CPromptWnd::IsTransparent(POINT & pt)
{
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：创建系统提示信息窗口 
//-------------------------------------------------------------------
BOOL CPromptWnd::Create(IResObject * pResObject)
{
	return CWindowCreator::CreateMainWnd(NULL, this, pResObject);
}

//-------------------------------------------------------------------
// 描  述：添加一条提示信息
// 参  数：[输入] LPCSTR lpszText - 文本
// 返回值：void
//-------------------------------------------------------------------
void CPromptWnd::AddMessage(LPCSTR lpszText)
{
	if (lpszText == NULL)
	{
		return;
	}

	m_messages.clear();

	// 如果有必要，把信息分成多行
	while (strlen(lpszText) != 0)
	{
		string strLine = GetLine(&lpszText);
		AddLine(strLine);
	} 
	
	if (m_messages.size() > 0)
	{
		m_rcWnd.bottom = m_rcWnd.top + m_nLineHeight * m_messages.size();
		SetAlpha(50);
		SetWindowPos(m_rcWnd);
	}
	else
	{
		SetAlpha(0);
	}

}
//------------------------------------------------------------------------
void CPromptWnd::ClearMessage(void)
{
	m_messages.clear();
	SetAlpha(0);
}

//-------------------------------------------------------------------
// 描  述：添加一行提示信息
// 参  数：[输入] string strText - 文本
// 返回值：void
//-------------------------------------------------------------------
void CPromptWnd::AddLine(string strText)
{
	// 调整信息显示的位置
	for (MessageListItr itr = m_messages.begin(); itr != m_messages.end(); itr++)
	{
		(*itr).nPos -= m_nLineHeight;
	}
	
	SMessage message;
	message.strText = strText;
	message.clrText = m_clrText;
	message.nPos = m_rcShow.bottom - m_nLineHeight;
	
	m_messages.push_back(message);
}

//-------------------------------------------------------------------
// 描  述：显示窗口 
//-------------------------------------------------------------------
void CPromptWnd::ShowWindow(BOOL bShow)
{
	SetVisible(bShow);
}
//------------------------------------------------------------------------