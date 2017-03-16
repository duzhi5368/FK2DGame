/**
*	created:		2012-6-28   10:29
*	filename: 		WindowCreator
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现窗口创建器
*/
//------------------------------------------------------------------------
#include "../Include/WindowCreator.h"
#include "../Include/ChatClient.h"
//------------------------------------------------------------------------
CWindowCreator::CWindowCreator(void)
{
}
//------------------------------------------------------------------------
CWindowCreator::~CWindowCreator(void)
{
}

//-------------------------------------------------------------------
// 描  述：创建通用窗口 
// 参  数：[输入] XWindow *pParent - 父窗口
// 参  数：[输入] XWindow *pMe - 自己
// 参  数：[输入] IResObject *pResObject - 脚本资源
// 返回值：XWindow * - 创建成功返回TRUE,否则返回FALSE
//-------------------------------------------------------------------
BOOL CWindowCreator::CreateGeneralWnd(XWindow *pParent, XWindow *pMe, IResObject *pResObject)
{
	if (pParent == NULL)
	{
		pParent = g_wndHelper.GetGameDesktop();
	}
	
	if (pParent == NULL || pMe == NULL || pResObject == NULL)
	{
		return FALSE;
	}

	return (g_wndHelper.CreateXWindow(pParent, pResObject, pResObject->GetID(), pMe) != NULL);
}

//-------------------------------------------------------------------
// 描  述：创建窗口控件
// 参  数：[输入] XWindow *pParent - 父窗口
// 参  数：[输入] XWindow *pMe - 自己
// 参  数：[输入] IResObject *pResObject - 脚本资源
// 返回值：XWindow * - 创建成功返回TRUE,否则返回FALSE
//-------------------------------------------------------------------
BOOL CWindowCreator::CreateControl(XWindow *pParent, XWindow *pMe, IResObject *pResObject)
{
	if (pParent == NULL || pResObject == NULL)
	{
		return FALSE;
	}
	
	return (g_wndHelper.CreateXWindow(pParent, pResObject, pResObject->GetID(), pMe) != NULL);
}

//-------------------------------------------------------------------
// 描  述：创建主窗口 
// 参  数：[输入] XWindow *pParent - 父窗口
// 参  数：[输入] XWindow *pMe - 自己
// 参  数：[输入] IResObject *pResObject - 脚本资源
// 返回值：XWindow * - 创建成功返回TRUE,否则返回FALSE
//-------------------------------------------------------------------
BOOL CWindowCreator::CreateMainWnd(XWindow *pParent, XWindow *pMe, IResObject *pResObject)
{
	if (pParent == NULL)
	{
		pParent = g_wndHelper.GetGameDesktop();
	}
	
	if (pParent == NULL || pMe == NULL || pResObject == NULL)
	{
		return FALSE;
	}
	
	pMe->SetParent(pParent);
	g_wndHelper.CreateStockProperty(pMe, pResObject, pResObject->GetID());
	pParent->AddWindow(pMe);	

	return TRUE;
}
//------------------------------------------------------------------------