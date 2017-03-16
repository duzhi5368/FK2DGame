/**
*	created:		2012-6-28   2:34
*	filename: 		XItemSentence
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		在聊天框上显示的话语句子对象
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
//------------------------------------------------------------------------
// 在聊天框上显示的话语句子对象
class XItemSentence : public XItemLinkText  
{
public:
	XItemSentence(void);
	virtual ~XItemSentence(void);
	virtual void OnDraw(ICanvas* pCanvas, int x, int y, RECT& rcClip, int alpha);
	void SetBackColor(WPixel clrBack);
public:
	virtual void Emit(void);
private:
	WPixel m_crBackColor;
	BOOL   m_bSetBack;
};
//------------------------------------------------------------------------