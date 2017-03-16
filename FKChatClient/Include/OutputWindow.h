/**
*	created:		2012-6-28   10:35
*	filename: 		OutputWindow
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		聊天信息输出窗口
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "XChatEx.h"
#include "XMoveButton.h"
#include "XResizeButton.h"
#include "XDivisionButton.h"
#include "ChatItemFactory.h"
#include "ChatParser.h"
#include "ChatDef.h"
//------------------------------------------------------------------------
#define WINDOW_MIN_HEIGHT 100
#define WINDOW_MAX_HEIGHT 450
//------------------------------------------------------------------------
#define HISTORY_MAX 10
//------------------------------------------------------------------------
class CChatClient;
//------------------------------------------------------------------------
// 聊天信息输出窗口
class COutputWindow : public XMainWnd, 
                      public ITimerSink
{
public:
	COutputWindow(void);
	virtual ~COutputWindow(void);

public:
	BOOL Create(IResObject * pResObject);
	virtual void SetWindowPos(RECT & rc);
	virtual void MoveWindow(int dx, int dy);
	virtual BOOL IsTransparent(POINT& pt);
	void ChannelOn(int nChannel, BOOL bOnOff);
	BOOL Display(const char * lpszData, int nLen);
	void SetTransparence(int nLevel);
	void ShowWindow(BOOL bVisible);
	virtual void OnTimer(DWORD dwEventID);
	void SetPromptOn(BOOL bOn);
	virtual DWORD OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void Clear();
private:
	BOOL DisplayMessage(const CHtmlNode * pContent);

private:
	BOOL m_channelState[CHANNEL_COUNT];
	XResizeButton * m_pResizeButton;
	XMoveButton * m_pMoveButton;
	XChatEx * m_pChatOutput;
	XStaticBitmap * m_pPromptBitmap;
	list<CHtmlNode *> m_history;
	CChatItemFactory m_itemFactory;
	CChatParser m_chatParser;
	BOOL m_bPromptOn;
};
//------------------------------------------------------------------------