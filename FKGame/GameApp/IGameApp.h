/**
*	created:		2012-6-21   0:41
*	filename: 		IGameApp
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
enum
{
	gcmdFullScreen	= 50000,		// 请求全屏
	gcPlayerMovie	= 50001,		// 通知目前正在播动画
	gcmdChangMap	= 50002,		// 请求更换地图,dwParam1指向新的地图名
	gcmdExit		= 50003			// 退出游戏
};
//------------------------------------------------------------------------
// 申明
struct ITrace;
struct ICanvas;
//------------------------------------------------------------------------
// 定义
struct IGameApp
{
	// 一般在WM_INITDIALOG这样的消息处理里调用此函数，表示主窗口创建成功
	// 游戏可以在此创建游戏对象
	// pParam1、pParam2为附加参数，为未来可能有扩充数据需要传递
	virtual BOOL		InitGame(HINSTANCE hInstance = NULL, HWND hWnd = NULL, ICanvas* pScreen = NULL,ITrace* pTrace = NULL, void* pParam1 = NULL, void*pParam2 = NULL) { return FALSE; }

	// 游戏主循环，一般在OnIdle函数里调用
	// 注意：
	//		MFC和ATL默认行为是：在无消息处理时只调用OnIdle一次
	//		因此要重载OnIdle的实现，使之在空闲时即无限次地调用OnIdle()
	// 返回：当返回FALSE时，要求主程序退出
	virtual BOOL		Loop(void) { return 0; }

	// 消息过滤，接管鼠标、键盘等控制消息
	// 返回：返回TRUE表示此消息已经被接管，主窗口不必再处理此消息
	virtual BOOL		PreTranslateMessage(MSG * pMsg) { return FALSE; }

	// 自定义命令码
	virtual LRESULT		OnCommand(DWORD dwCmd,DWORD dwParam1,DWORD dwParam2) { return -1; }
};
//------------------------------------------------------------------------