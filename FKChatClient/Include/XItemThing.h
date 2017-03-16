/**
*	created:		2012-6-28   2:41
*	filename: 		XItemThing
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		在聊天框上显示的物品条目
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../../FKGame/SceneObj/IGoods.h"
//------------------------------------------------------------------------
// 在聊天框上显示的物品条目
class XItemThing : public XItemLinkText  
{
public:
	XItemThing(void);
	virtual ~XItemThing(void);
public:
	void SetGoods(IGoods *pGoods);
public:
	virtual void Emit(void);
private:
	IGoods *m_pGoods;
};
//------------------------------------------------------------------------