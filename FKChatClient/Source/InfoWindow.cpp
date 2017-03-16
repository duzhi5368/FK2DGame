/**
*	created:		2012-6-28   8:46
*	filename: 		InfoWindow
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现装备属性显示窗口
*/
//------------------------------------------------------------------------
#include "../Include/ChatClient.h"
#include "../Include/InfoWindow.h"
#include <assert.h>
//------------------------------------------------------------------------
CInfoWindow::CInfoWindow(void)
: m_pHtml(NULL)
{
}
//------------------------------------------------------------------------
CInfoWindow::~CInfoWindow(void)
{
}
//------------------------------------------------------------------------
bool CInfoWindow::Create(IResObject * pResObject)
{
	CWndSysHelper wndHelper;
	IResObject * pSubResObject = NULL;
	XWindow * pDesktop = wndHelper.GetGameDesktop();
	assert(pDesktop != NULL);

	if (pResObject == NULL)
	{
		PrintEventLn("ChatClient (FAIL) - pResObject == NULL");
		return false;
	}	

	// 创建属性显示窗口
	SetParent(pDesktop);
	wndHelper.CreateStockProperty(this, pResObject, INFOWINDOW_ID);
	pDesktop->AddWindow(this);	
	
	// 创建HTML控件	
	m_pHtml = new XHtml(this);
	if (m_pHtml == NULL)
	{
		PrintEventLn("ChatClient (FAIL) - new XHtml(this)");
		return false;
	}
	pSubResObject = pResObject->GetSubObjectByIndex(0);
	if (pSubResObject == NULL)
	{
		PrintEventLn("ChatClient (FAIL) - GetSubObjectByIndex(0)");
		return false;
	}
	if(wndHelper.CreateXWindow(this, pSubResObject, 0, m_pHtml) == NULL)
	{
		PrintEventLn("ChatClient (FAIL) - CreateXWindow(m_pHtml)");
		return false;
	}

	m_pHtml->m_pHScrollBar->SetVisible(FALSE);
	m_pHtml->m_pVScrollBar->SetVisible(FALSE);
	
	SetVisible(FALSE);
	
	return true;
}
//------------------------------------------------------------------------
void CInfoWindow::SetWindowPos(RECT & rc)
{
	XMainWnd::SetWindowPos(rc);

	if (m_pHtml != NULL)
	{
		m_pHtml->m_pContainer->SetWindowPos(rc);
	}
}

//-------------------------------------------------------------------
// 描  述：清除属性信息 
// 参  数：void
// 返回值：void
//-------------------------------------------------------------------
void CInfoWindow::ClearInfo(void)
{
	m_pHtml->m_pContainer->Clear();
	m_pHtml->Clear();
}

//-------------------------------------------------------------------
// 描  述：设置要显示的属性信息 
// 参  数：[输入] string strInfo - 属性信息
// 返回值：bool - 可以显示返回true,否则返回false
//-------------------------------------------------------------------
bool CInfoWindow::SetInfo(string strInfo)
{
	ClearInfo();

	if (m_pHtml->m_LineParser.ParseLine(strInfo.c_str()))
	{
		m_pHtml->m_pHScrollBar->SetVisible(FALSE);
		m_pHtml->m_pVScrollBar->SetVisible(FALSE);
		return true;
	}
	else
	{
		return false;
	}
}
//------------------------------------------------------------------------
void CInfoWindow::OnActivate(BOOL bActive)
{
	SetVisible(bActive);
}
//------------------------------------------------------------------------
void CInfoWindow::SetVisible(BOOL bVisible)
{
	if (bVisible)
	{
		XMainWnd::SetActive();
		XMainWnd::SetVisible(bVisible);
	}
	else
	{
		XMainWnd::SetVisible(bVisible);
	}
}
//------------------------------------------------------------------------