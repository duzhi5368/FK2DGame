/**
*	created:		2012-6-24   23:08
*	filename: 		IControlModel
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		游戏控制器
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
struct  IControlMode
{
	// 如果已经处理此鼠标消息应返回0
	virtual void ConnectSingal(void){};      //连接窗口的信号

	virtual void DisConnectSingal(void){};   //取消连接窗口的信号

	// 参见WM_LBUTTONUP
	virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam){return -1;}
	
	// 参见WM_RBUTTONUP
	virtual LRESULT OnRButtonUp(WPARAM wParam, LPARAM lParam){return -1;}

	// 参见WM_LBUTTONDOWN
	virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam){return -1;}
	
	// 如果已经处理此鼠标消息应返回0
	// 参见WM_LBUTTONDOWN
	virtual LRESULT OnRButtonDown(WPARAM wParam, LPARAM lParam){return -1;}
	
	// 如果已经处理此鼠标消息应返回0
	// 参见WM_MOUSEMOVE
	virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam){return -1;}
	
	// 如果已经处理此键盘消息应返回0
	// 参见WM_KEYDOWN
	virtual LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam){return -1;}
	
	// 如果已经处理此键盘消息应返回0
	// 参见WM_KEYDOWN
	virtual LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam){return -1;}
	
	virtual void Release(void){delete this;}
};
//------------------------------------------------------------------------