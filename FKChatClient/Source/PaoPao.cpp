/**
*	created:		2012-6-28   9:03
*	filename: 		PaoPao
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现生物头上的泡泡
*/
//------------------------------------------------------------------------
#include "../Include/PaoPao.h"
#include "../Include/ChatClient.h"
#include "../../FKGame/Common/CreatureViewDef.h"
#include "../../FKGame/MapView/MapItemView.h"
//------------------------------------------------------------------------
CPaoPao::CPaoPao(UID uid)
: m_uidCreature(uid)
{
	RECT rc = {0, 0, PAOPAO_MAX_WIDTH, 100};
	m_container.SetWindowPos(rc);
	m_container.SetAutoWrap(TRUE);
	m_container.SetMaxWidth(PAOPAO_MAX_WIDTH);
	m_container.SetMaxHeight(PAOPAO_MAX_HEIGHT);	
	m_container.SetVisible(TRUE);

	m_msgBuilder.SetContainer(&m_container);
}
//------------------------------------------------------------------------
CPaoPao::~CPaoPao(void)
{
	CChatClient::Instance()->GetTimeAxis()->KillTimer(0, static_cast<ITimerSink*>(this));
}

//-------------------------------------------------------------------
// 描  述：设置泡泡要显示的文本
//-------------------------------------------------------------------
BOOL CPaoPao::SetText(LPCSTR lpszText)
{	
	CChatClient::Instance()->GetTimeAxis()->KillTimer(0, static_cast<ITimerSink*>(this));	

	// 构造要显示的内容
	m_container.Clear();
	m_msgBuilder.Build(lpszText);
	
	CChatClient::Instance()->GetTimeAxis()->SetTimer(0, static_cast<ITimerSink*>(this), PAOPAO_TIMEOUT, "CPaoPao::SetText"); 

	return TRUE;
}

void CPaoPao::OnDraw(ICanvas * pCanvas, const POINT & ptBottomCenter, const char * szName, const WPixel & clrName, ITTFont * pNameFont)
{
	RECT rcContainer;
	rcContainer.left = ptBottomCenter.x - PAOPAO_MAX_WIDTH / 2;
	rcContainer.right = rcContainer.left + PAOPAO_MAX_WIDTH;
	rcContainer.bottom = ptBottomCenter.y;
	rcContainer.top = rcContainer.bottom - m_container.GetMaxHeight();

	RECT rcBox = rcContainer;
	rcBox.top = rcContainer.top - pNameFont->GetHeight();

	// 绘制背景
	pCanvas->DrawBox(rcBox, PAOPAO_BACK_COLOR, BLIT_TRANS, PAOPAO_BACK_ALPHA);

	// 绘制名字
	pCanvas->DrawText(pNameFont, rcBox.left, rcBox.top, szName, clrName, FONT_NOALPHA);
	pCanvas->DrawText(pNameFont, rcBox.left + pNameFont->GetLength(szName), rcBox.top, "：", 
		              clrName, FONT_NOALPHA);	

	// 绘制内容
	m_container.SetWindowPos(rcContainer);
	m_container.OnDraw(pCanvas, rcContainer);
}

//-------------------------------------------------------------------
// 描  述：销毁泡泡 
//-------------------------------------------------------------------
void CPaoPao::Release(void)
{
	delete this;
}

void CPaoPao::OnTimer(DWORD dwEventID)
{
	CChatClient::Instance()->GetTimeAxis()->KillTimer(dwEventID, static_cast<ITimerSink*>(this));
	m_container.Clear();

	ICreature * pCreature = CChatClient::Instance()->GetCreature(m_uidCreature);
	if (pCreature != NULL && pCreature->GetMapItem() != NULL)
	{		
		CMapItemView * pMapItemView = pCreature->GetMapItem();
		
		// 清除泡泡
		IPaoPao ** ppPaoPao = NULL;
		pMapItemView->OnCommand(CrtCmdChatInfo, (DWORD)&ppPaoPao, 0);
		if (ppPaoPao != NULL)
		{
			*ppPaoPao = NULL;
			Release();
		}
	}				
}
//------------------------------------------------------------------------