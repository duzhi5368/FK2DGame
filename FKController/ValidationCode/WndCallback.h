/**
*	created:		2012-6-24   22:54
*	filename: 		WndCallback
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/IWndMgr.h"
//------------------------------------------------------------------------
class CWndCallback : public IWndCallback
{
public:
	virtual BOOL OnEvent(UINT uWndID, UINT uEventID, WPARAM wParam, LPARAM lParam);
};
//------------------------------------------------------------------------