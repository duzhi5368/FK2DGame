/**
*	created:		2012-6-28   2:25
*	filename: 		XItemPlayer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		在聊天框显示的玩家对象条目
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
//------------------------------------------------------------------------
// 在聊天框显示的玩家对象条目
class XItemPlayer : public XItemLinkText  
{
public:
	XItemPlayer(void);
	virtual ~XItemPlayer(void);

public:
	virtual void Emit(void);
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
};
//------------------------------------------------------------------------