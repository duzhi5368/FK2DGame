/**
*	created:		2012-6-28   2:29
*	filename: 		ChatClient
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		聊天模块
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <string>
#include "../../FKTrace/IFKTrace.h"
#include "../../FKGame/GameGlobal/IClientGlobal.h"
#include "../../FKGame/WndSys/WndSys.h"
#include "../IChatClient.h"
#include "../../FKGame/Controller/IController.h"
#include "../IPaoPao.h"
using std::string;
//------------------------------------------------------------------------
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  {if(p) { delete p;p = NULL; }}
#endif
//------------------------------------------------------------------------
extern CFKTrace					g_DefaultTrace;
extern CWndSysHelper			g_wndHelper;
extern IClientGlobal *			g_pGlobal;
//------------------------------------------------------------------------
#include "ChatController.h"
#include "OutputWindow.h"
#include "InfoWindow.h"
#include "ChatMenu.h"
#include "BulletinWnd.h"
#include "SysMsgReceiver.h"
#include "PromptWnd.h"
#include "InvalidThingMgr.h"
//------------------------------------------------------------------------
#define OUTPUTWINDOW_ID		451		// 聊天信息显示窗口
#define INFOWINDOW_ID		452		// 属性显示窗口
#define PROMPTWINDOW_ID		453		// 系统提示窗口
#define BULLETINWINDOW_ID	454		// 系统公告窗口
#define HELPINFOWINDOW_ID	455		// 帮助提示栏
#define CHAT_MENU_WND_ID    472
#define FRIEND_WND			470
#define PROPERTY_WND		473
//------------------------------------------------------------------------
// 菜单项
struct SMenuItem
{
	int	nId;                 // 菜单ID
	const char * pszName;    // 菜单名字
};
//------------------------------------------------------------------------
// 菜单项ID定义
enum CHAT_MENU_ITEM
{
	CHAT_PRIVATE = 100,      // 悄悄话
	CHAT_TEAM ,              // 邀请组队
	CHAT_FRIEND, 			 // 加为好友
	CHAT_INFO				 // 资料信息	
};
//------------------------------------------------------------------------
// 队长点自由玩家的菜单
const SMenuItem g_menu[] = 
{
	{CHAT_PRIVATE,	"悄悄话"	},
	{CHAT_TEAM,		"邀请组队"	},
	{CHAT_FRIEND,	"加为好友"  },
	{CHAT_INFO,		"资料信息"  },
	0	
};
//------------------------------------------------------------------------
// 聊天模块
class CChatClient : public IChatClient, 
                    public IDataReceiver, 
					public IEventActionListener,
					public IMenuEventReceiver
{
	friend class CChatController;
	friend class CChatItemFactory;
	friend class CReminder;
	friend class XItemPlayer;
	friend class XItemSentence;
	friend class XItemThing;
	friend class CPromptWnd;
	friend class CReminder;
	friend class CChatParser;
	
protected:
	CChatClient(void);
	virtual ~CChatClient(void);

public:
	static CChatClient * Instance(void);
	BOOL Create(IClientGlobal * pGlobal);
	virtual void Release(void);
	virtual BOOL OnReceived(const char * pData, int nLen);
	virtual void SetWindowTrans(int nLevel);
	virtual void ShowWindow(int nType);
	virtual void ChannelOn(int nChannel, BOOL bOnOff);
	virtual BOOL Speak(int nChannel, const char * pszReceiver, const char * pszText);
	virtual void DisplaySysMessage(const char * pszText);
	virtual void OnAction(IEvent *pEvent);
	virtual IPaoPao * CreatePaoPao(UID uid);
	virtual void OnSysMsgDraw(ICanvas * pCanvas);
	virtual void DisplayFloatMessage(int nType, const char * lpszText);
	virtual void ClearFloatMessage(int nType);
	ITimeAxis * GetTimeAxis(void);
	IActor * GetPlayer(UID uidPlayer);
	ICreature * GetCreature(UID uidCreature);
	void PingStart(void);
	DWORD PingStop(void);
	virtual void Clicked(int nId);
    void SetMenu(const SMenuItem menu[]);
	void ShowMenu(POINT pt,LPCSTR lpszPlayer);
	void ShowTitleWnd(BOOL bTrue,BOOL bTurn = TRUE);					// 显示称号窗口
	int GetDistance(UID uid);											// 获取玩家之间的距离
     
	IThing *GetThing(UID uid);
	IThing *CreateGoods(char *buf,int nLen,UID uid);
public:
	CPromptWnd *			m_pHelpInfoWnd;
private:	
	BOOL m_bCreated;
	IClientGlobal *			m_pGlobal;	
	COutputWindow *			m_pOutputWindow;
	CInfoWindow *			m_pInfoWindow;
	IResScript *			m_pResScript;
	CChatController			m_controller;
	CSysMsgReceiver			m_sysMsgReceiver;
	DWORD					m_dwPingStartTime;
	CBulletinWnd *			m_pBulletinWnd;
	CChatMenu *				m_pMenu;
	CInvalidThingMgr		m_InvlidThingMgr;
	string					m_strMenu;
private:
	static CChatClient *	s_pModule;
};
//------------------------------------------------------------------------