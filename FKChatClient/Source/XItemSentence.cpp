/**
*	created:		2012-6-28   2:34
*	filename: 		XItemSentence
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现聊天框上说话话语的显示元素
*/
//------------------------------------------------------------------------
#include "../Include/ChatClient.h"
#include "../Include/XItemSentence.h"
//------------------------------------------------------------------------
XItemSentence::XItemSentence(void)
:m_bSetBack(FALSE)
{
}
//------------------------------------------------------------------------
XItemSentence::~XItemSentence(void)
{
}
//------------------------------------------------------------------------
void XItemSentence::Emit(void)
{
	// 把这里的句子加到聊天输入框
	CChatClient::Instance()->m_controller.SetChatString(m_strText);
	m_bVisited = FALSE;
}
//------------------------------------------------------------------------
void XItemSentence::OnDraw(ICanvas* pCanvas, int x, int y, RECT& rcClip, int alpha)
{
	if( m_bSetBack )
	{
		RECT rcClient;
		CopyRect(&rcClient, &rcClip);

		if (rcClient.top < (rcClient.bottom - GetHeight()))
		{
			rcClient.top = rcClient.bottom - GetHeight();
		}	

		rcClient.left = x;
		rcClient.right = x + GetWidth() + 1;
		pCanvas->DrawBox(rcClient, m_crBackColor, BLIT_NORMAL, 255);
	}
	XItemLinkText::OnDraw(pCanvas,x,y,rcClip,alpha);
}
//------------------------------------------------------------------------
void XItemSentence::SetBackColor(WPixel clrBack)
{
	m_crBackColor = clrBack;
	m_bSetBack = TRUE;
}
//------------------------------------------------------------------------