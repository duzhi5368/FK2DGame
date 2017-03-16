/**
*	created:		2012-6-28   10:23
*	filename: 		TitleWnd
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/TitleWnd.h"
#include "../Include/TitleClient.h"
#include "../Include/ChatClient.h"
//------------------------------------------------------------------------
CTitleWnd::CTitleWnd()
{
	m_bReget = TRUE;
}
//------------------------------------------------------------------------
CTitleWnd::~CTitleWnd()
{

}
//------------------------------------------------------------------------
BOOL CTitleWnd::Create(IResObject *pResObject,int nWndId)
{
	if( pResObject == NULL)
	{
		return FALSE;
	}
	g_wndHelper.CreateXWindow(g_wndHelper.GetGameDesktop(),pResObject,nWndId,this);
	//创建各子窗口
	for(int i =0 ;i<pResObject->GetSubObjectCount()-1;i++ )
	{
		g_wndHelper.CreateXWindow( this, 
			pResObject->GetSubObjectByIndex(i),
			i);
	}

	// 关联指针
	m_pBorderEdit1 = (XEdit *)FindWindowByID(0);
	m_pBorderEdit2 = (XEdit *)FindWindowByID(1);
	m_pUserTitleBtn = (XButton *)FindWindowByID(4);
	m_pDeleteTitleBtn = (XButton *)FindWindowByID(5);
	m_pUnUseTitleBtn = (XButton *)FindWindowByID(6);
	m_pCloseBtn = (XButton *)FindWindowByID(7);
	m_pHideBtn = (XCheckBox *)FindWindowByID(9);

	// 设置边框 Disable
	m_pBorderEdit1->SetEnabled(FALSE);
	m_pBorderEdit2->SetEnabled(FALSE);

	// 重新设置提示信息
	FindWindowByID(8)->SetWindowText("  (还有更多的称号等待你去发掘)");

	// 替换掉ListBox默认的ScrollBar
	m_pScrollBar = new XScrollBar(this);
	// 设置滚动条大小
	SIZE size;
	size.cx = GetDesktop()->GetSysScrollBarBitmap()->GetHeight();
	size.cy = size.cx;
	m_pScrollBar->SetSize(size);
	AddWindow(m_pScrollBar);
	RECT rect;
	m_pBorderEdit1->GetWindowRect(&rect);
	rect.top +=2;
	rect.bottom -=2;
	rect.right -=2;
	rect.left = rect.right-size.cx;
	m_pScrollBar->SetWindowPos(rect);
	// 设置滚动条的属性
	m_pScrollBar->Attach(this);	
	m_pScrollBar->SetVisible(TRUE);
	m_pScrollBar->SetStyle(XScrollBar::S_VERT);
	m_pScrollBar->Refresh();
	// 关联函数
	//CONN_posChanged(m_pScrollBar, CTitleWnd, OnVScrollPosChanged);
	//CONN_clicked(m_pUserTitleBtn, CTitleWnd, OnUseTitle);  
	//CONN_clicked(m_pUnUseTitleBtn, CTitleWnd, OnUnUseTitle);  
	//CONN_clicked(m_pDeleteTitleBtn, CTitleWnd, OnDeleteTitle);  
	//CONN_clicked(m_pCloseBtn, CTitleWnd, OnClose); 
	//CONN_clicked(m_pHideBtn, CTitleWnd, OnHide);

	//创建列表框
	m_pListBox = new XListBoxEx;
	if( m_pListBox == NULL
		|| !m_pListBox->Create(this,pResObject->GetSubObjectByIndex(11),pResObject->GetSubObjectByIndex(11)->GetID()))
	{
		return FALSE;
	}
	//CONN_selChanged(m_pListBox,CTitleWnd,OnSelChange);

	//设置Esc
	// 设置用户按下ESC是关闭当前窗口
	SetWndTraits(XWT_ESCAPE, TRUE);
	ReSetScroll();
	SetVisible(FALSE);

	return TRUE;
}
//------------------------------------------------------------------------
int CTitleWnd::OnEvent(UINT uEventID, WPARAM wParam, LPARAM lParam)
{
	switch(uEventID)
	{
	case 400: // 增加称号
		{
			SActorTitle2 *pActorTitle = (SActorTitle2 *)wParam;
			AddActorTitle(pActorTitle);
			break;
		}
	case 401:
		{
			// 删除称号
			DeleteActorTitle((long)wParam);
			break;
		}
	case 402:
		{
			// 使用称号
			//设置称号状态 
			SActorTitle2Ex *pEx = CTitleClient::Instance()->GetTitleMgr().GetActorTitle(wParam);
			if( pEx != NULL)
			{
				pEx->title.lStatus = USE_TITLE;
			}
			break;
		}
	case 403:
		{
			// 不使用称号
			SActorTitle2Ex *pEx = CTitleClient::Instance()->GetTitleMgr().GetActorTitle(wParam);
			if( pEx != NULL)
			{
				pEx->title.lStatus = HOLD_TITLE;
			}
			break;
		}
	case 404:
		{
			// 小退
			// 清除列表框 
			m_pListBox->Clear();
			m_pBorderEdit2->SetWindowText("");
			CTitleClient::Instance()->GetTitleMgr().IntialActorTitleMap();
			m_bReget = TRUE;
			OnClose();
			break;
		}
	case 405:
		{
			// 玩家点击了称号按钮
			if( m_bReget )
			{
				//获取称号列表
				m_bReget = FALSE;
				// 发送获取称号列表命令
				CTitleClient::Instance()->GetTitleController().GetTitle();
			}
			if(IsVisible())
			{
				SetVisible(FALSE);
			}else{
				SetVisible();
				BringToTopLevel();
			}
			break;
		}
	case 406:
		{
			// 显示称号窗口
			SetVisible();
			break;
		}
	case 407:
		{
			//状态改变
			StatusChange((long)wParam, (int)lParam);
			break;
		}
	default:
		{
			break;
		}

	}
	return 0;
}
//------------------------------------------------------------------------
void CTitleWnd::OnVScrollPosChanged(DWORD dwParam)
{
	int nIndex = *(int *)dwParam;
	m_pListBox->SetVisibleStart(nIndex);
}
//------------------------------------------------------------------------
void CTitleWnd::ReSetScroll()
{
	if(m_pListBox->GetSize() <= m_pListBox->GetVisibleCount()  )
	{
		m_pScrollBar->SetVisible(FALSE);
	}else{
		m_pScrollBar->SetVisible();
	}
	int nRange = m_pListBox->GetSize()-m_pListBox->GetVisibleCount();
	m_pScrollBar->SetScrollRange(0,nRange,TRUE);
	m_pScrollBar->SetScrollPos(0);
	m_pListBox->SetVisibleStart(0);
}
//------------------------------------------------------------------------
void CTitleWnd::OnSelChange(DWORD dwParam)
{
	// 同时填充右边的提示信息
	int nSel = m_pListBox->GetSel();
	if( nSel >= 0)
	{
		SActorTitle2Ex *pEx = (SActorTitle2Ex *)m_pListBox->GetItemData(nSel);
		if( pEx )
		{
			if( pEx->dwData )
			{
				STitle *pTitle = (STitle *)(pEx->dwData);
				LPCSTR lpStr = pTitle->strDes.c_str();
				m_pBorderEdit2->SetWindowText(lpStr);
			}
		}
	}
}
//------------------------------------------------------------------------
void CTitleWnd::OnUseTitle()
{
	int nSel = m_pListBox->GetSel();
	char szText[256];

	if( nSel >= 0)
	{
		SActorTitle2Ex *pEx = (SActorTitle2Ex *)m_pListBox->GetItemData(nSel);
		if( pEx )
		{
			if( pEx->title.lStatus == HOLD_TITLE )
			{
				CTitleClient::Instance()->GetTitleController().UserTitle(pEx->title.lTitleId);			
			}else if(pEx->title.lStatus == USE_TITLE)
			{
				STitle *pTitle = CTitleClient::Instance()->GetTitleMgr().GetTitle(pEx->title.lTitleId);
				if( pTitle != NULL)
				{
					sprintf(szText,"称号:[%s]正在使用中",pTitle->strTitleName.c_str());
					CTitleClient::Instance()->Speak(szText);

				}
			}
			else{
				// 您尚未掌握该称号
				STitle *pTitle = CTitleClient::Instance()->GetTitleMgr().GetTitle(pEx->title.lTitleId);
				if( pTitle != NULL)
				{
					sprintf(szText,"您尚未掌握称号:[%s].",pTitle->strTitleName.c_str());
					CTitleClient::Instance()->Speak(szText);
				}
			}
		}
	} 
}
//------------------------------------------------------------------------
void CTitleWnd::OnDeleteTitle()
{
	int nSel = m_pListBox->GetSel();
	if( nSel >= 0)
	{
		SActorTitle2Ex *pEx = (SActorTitle2Ex *)m_pListBox->GetItemData(nSel);
		if( pEx )
		{
			if( pEx->title.lStatus != USE_TITLE )
			{
				CTitleClient::Instance()->GetTitleController().DeleteTitle(pEx->title.lTitleId);			
			}
		}
	}
}
//------------------------------------------------------------------------
void CTitleWnd::OnUnUseTitle()
{
	int nSel = m_pListBox->GetSel();
	if( nSel >= 0)
	{
		SActorTitle2Ex *pEx = (SActorTitle2Ex *)m_pListBox->GetItemData(nSel);
		if( pEx )
		{
			if( pEx->title.lStatus == USE_TITLE )
			{
				CTitleClient::Instance()->GetTitleController().UnUseTitle(pEx->title.lTitleId);			
			}
		}
	}   
}
//------------------------------------------------------------------------
void CTitleWnd::UpdateScroll()
{
	if(m_pListBox->GetSize() <= m_pListBox->GetVisibleCount()  )
	{
		m_pScrollBar->SetVisible(FALSE);
	}else{
		m_pScrollBar->SetVisible();
	}
	int nRange = m_pListBox->GetSize()-m_pListBox->GetVisibleCount();
	int nPos = m_pScrollBar->GetScrollPos();
	m_pScrollBar->SetScrollRange(0,nRange,TRUE);
	m_pScrollBar->SetScrollPos(nPos);
}
//------------------------------------------------------------------------
void CTitleWnd::OnClose()
{  
	SetVisible(FALSE);
}
//------------------------------------------------------------------------
void CTitleWnd::OnHide()
{
	m_pListBox->Clear();

	// 重新填充列表项
	Actor_Title_Map& mp = CTitleClient::Instance()->GetTitleMgr().GetActorTitleMap();
	Actor_Title_Map_Itr itr = mp.begin();
	for(; itr!= mp.end();itr++)
	{

		SActorTitle2Ex *pEx = (*itr);
		if( pEx== NULL)
		{
			continue;
		}
		if( !m_pHideBtn->GetState() )
		{
			if(pEx->title.lStatus <= UNDERSTAND_TITLE)
			{
				continue;
			}
		}
		string str ;
		STitle *pTitle = CTitleClient::Instance()->GetTitleMgr().GetTitle(pEx->title.lTitleId);
		if( pTitle != NULL)
		{
			str = pTitle->strTitleName;
			m_pListBox->AddItem(str.c_str(),WPixel(255,255,255),(DWORD)pEx);
		}
	}
	ReSetScroll();
}	
//------------------------------------------------------------------------
void CTitleWnd::AddActorTitle(SActorTitle2 *pActorTitle)
{
	// 查找是否已经有了
	if( pActorTitle == NULL)
	{
		return;
	}
	SActorTitle2Ex *pEx = CTitleClient::Instance()->GetTitleMgr().GetActorTitle(pActorTitle->lTitleId);
	if( pEx != NULL)
	{
		return;
	}
	pEx = new SActorTitle2Ex;
	pEx->dwData = 0;
	memcpy(&pEx->title,pActorTitle,sizeof(SActorTitle2));
	CTitleClient::Instance()->GetTitleMgr().AddActorTitle(pEx);

	//获取实际名字
	string str ;
	STitle *pTitle = CTitleClient::Instance()->GetTitleMgr().GetTitle(pEx->title.lTitleId);
	if( pTitle != NULL)
	{
		str = pTitle->strTitleName;
		if( !m_pHideBtn->GetState() ||
			(pEx->title.lStatus>= HOLD_TITLE ))
		{
			m_pListBox->AddItem(str.c_str(),WPixel(255,255,255),(DWORD)pEx);
		}
	}
	UpdateScroll();
}
//------------------------------------------------------------------------
void CTitleWnd::DeleteActorTitle(long lTitleId)
{

	SActorTitle2Ex *pEx = CTitleClient::Instance()->GetTitleMgr().GetActorTitle(lTitleId);
	if( pEx == NULL)
	{
		return;
	}
	int nSize = m_pListBox->GetSize();
	for(int i=0;i<nSize;i++)
	{
		if(m_pListBox->GetItemData(i) == (DWORD)pEx)
		{
			m_pListBox->RemoveItem(i);		  
			break;
		}
	}
	CTitleClient::Instance()->GetTitleMgr().DeleteActorTitle(lTitleId);
}
//------------------------------------------------------------------------
void CTitleWnd::UnUseActorTitle(long lTitleId)
{

}
//------------------------------------------------------------------------
void CTitleWnd::OnEscape()
{
	SetVisible(FALSE); 
}
//------------------------------------------------------------------------
void CTitleWnd::StatusChange(long lTitleId,int nStatus)
{
	SActorTitle2Ex *pEx = CTitleClient::Instance()->GetTitleMgr().GetActorTitle(lTitleId);
	if( pEx == NULL)
	{

		return;
	}
	if( m_pHideBtn->GetState() )
	{
		if( pEx->title.lStatus<= UNDERSTAND_TITLE
			&& nStatus== HOLD_TITLE)
		{
			m_pListBox->AddItem(pEx->title.szName,WPixel(255,255,255),(DWORD)pEx);
		}
	}
	pEx->title.lStatus = nStatus;
}
//------------------------------------------------------------------------