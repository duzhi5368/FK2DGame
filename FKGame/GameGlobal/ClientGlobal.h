/**
*	created:		2012-6-21   0:22
*	filename: 		ClientGlobal
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IClientGlobal.h"
#include "../ClientWorld/CClientWorld.h"
//------------------------------------------------------------------------
class CClientGlobal : public IClientGlobal
{
public:
	CClientGlobal(void);
	~CClientGlobal(void);
public:
	// 创建
	BOOL			Create(HINSTANCE hInstance, LPCSTR szWorkDir, HWND hMainWnd, ICanvas * pScreen, ITTFont * pDefFont, ITTFont * pDefFontWithOutline, ITrace * pTrace);
	// 关闭
	void			Close(void);
	// 聊天模块
	virtual IChatClient *			GetChatClient(void);
	// 关键字过滤
	virtual IKeywordFilter *		GetKeywordFilter(void);
	// 组队
	virtual ITeamClient *			GetTeamClient(void);
	// 效果工厂
	virtual IEffectFactory *		GetEffectFactory(void);
	// 客户端控制器
	virtual IController *			GetController(void);
	// 好友模块
	virtual IFriendClient *			GetFriendClient(void);
	// 声音引擎
	virtual ISound *				GetSoundEngine(void);
public:
	CClientWorld	m_ClientWorld;
};
//------------------------------------------------------------------------