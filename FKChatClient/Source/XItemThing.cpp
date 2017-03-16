/**
*	created:		2012-6-28   2:44
*	filename: 		XItemThing
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/ChatClient.h"
#include "../Include/XItemThing.h"
#include "../Include/Tool.h"
//------------------------------------------------------------------------
XItemThing::XItemThing(void)
:m_pGoods(NULL)
{
}
//------------------------------------------------------------------------
XItemThing::~XItemThing(void)
{

}
//------------------------------------------------------------------------
void XItemThing::Emit(void)
{
	if(m_pGoods!=NULL)
	{
		string str = CTool::Instance()->BuilderToolTip(m_pGoods);
		XToolTip *pToolTip = (XToolTip *)g_wndHelper.GetGameDesktop()->GetSysToolTip();
		pToolTip->SetText(str.c_str());
		POINT pt;
		::GetCursorPos(&pt);
		::ScreenToClient(g_pGlobal->m_hMainWnd,&pt);
		pToolTip->Show(&pt);
		RECT rect;
		pToolTip->GetWindowRect(&rect);
		int h = rect.bottom-rect.top;
		pt.y = 500-h;
		pToolTip->MoveWindow(0,pt.y-rect.top);
	}
}
//------------------------------------------------------------------------
void XItemThing::SetGoods(IGoods *pGoods)
{
	m_pGoods = pGoods;

	if( m_pGoods!= NULL)
	{
		WPixel w =  CTool::Instance()->GetGoodsNameColor(pGoods);
		SetTextColor(w);
		SetNotVisitedColor(w);
		SetVisitedColor(w);
		SetRolloverColor(WPixel(255,255,0));
		SetText(m_pGoods->GetName());
	}
}
//------------------------------------------------------------------------