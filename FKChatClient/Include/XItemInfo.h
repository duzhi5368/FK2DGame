/**
*	created:		2012-6-28   2:23
*	filename: 		XItemInfo
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		在聊天框显示的系统信息条目
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
//------------------------------------------------------------------------
// 在聊天框显示的系统信息条目
class XItemInfo : public XItemText  
{
public:
	virtual void OnDraw(ICanvas* pCanvas, int x, int y, RECT& rcClip, int alpha);
	void SetBackColor(WPixel clrBack);
	XItemInfo();
	virtual ~XItemInfo();

private:
	WPixel m_clrBack; // 背景颜色
};
//------------------------------------------------------------------------
