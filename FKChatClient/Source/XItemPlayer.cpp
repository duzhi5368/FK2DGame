/**
*	created:		2012-6-28   2:27
*	filename: 		XItemPlayer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现聊天框上的玩家显示
*/
//------------------------------------------------------------------------
#include "../Include/ChatClient.h"
#include "../Include/XItemPlayer.h"
//------------------------------------------------------------------------
XItemPlayer::XItemPlayer(void)
{
}
//------------------------------------------------------------------------
XItemPlayer::~XItemPlayer(void)
{
}
//------------------------------------------------------------------------
void XItemPlayer::Emit(void)
{
	// 把该玩家设为私聊对象
	CChatClient::Instance()->m_controller.SetChatPlayer(m_strText);
	m_bVisited = FALSE;
}
//------------------------------------------------------------------------
DWORD XItemPlayer::OnMouseUp(UINT nButton, UINT nShift, POINT pt)
{
	// 如果点击了右健
	if( nButton == RightButton )
	{
		if( m_strText == "系统" )
		{
			return XItemLinkText::OnMouseUp(nButton, nShift, pt);
		}
		CChatClient::Instance()->ShowMenu(pt,m_strText.c_str());  
		return 0;
	}
	return XItemLinkText::OnMouseUp(nButton, nShift, pt);
}
//------------------------------------------------------------------------