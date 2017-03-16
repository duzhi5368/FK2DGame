/**
*	created:		2012-6-28   2:24
*	filename: 		XItemInfo
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/XItemInfo.h"
//------------------------------------------------------------------------
XItemInfo::XItemInfo(void)
{
	m_clrBack = WPixel(255, 0, 0);
}
//------------------------------------------------------------------------
XItemInfo::~XItemInfo(void)
{

}
//-------------------------------------------------------------------
// 描  述：设置文本的背景颜色 
// 参  数：[输入] WPixel clrBack - 指定背景颜色
// 返回值：void
//-------------------------------------------------------------------
void XItemInfo::SetBackColor(WPixel clrBack)
{
	m_clrBack = clrBack;
}
//------------------------------------------------------------------------
void XItemInfo::OnDraw(ICanvas *pCanvas, int x, int y, RECT &rcClip, int alpha)
{
	RECT rcClient;
	CopyRect(&rcClient, &rcClip);

	if (rcClient.top < (rcClient.bottom - GetHeight()))
	{
		rcClient.top = rcClient.bottom - GetHeight();
	}	
	
	rcClient.left = x;
	rcClient.right = x + GetWidth() + 1;
	pCanvas->DrawBox(rcClient, m_clrBack, BLIT_NORMAL, 255);
	pCanvas->DrawClipText(m_pFont, x, y, m_strText.c_str(), m_clrText, rcClip, FONT_NOALPHA);
}
//------------------------------------------------------------------------