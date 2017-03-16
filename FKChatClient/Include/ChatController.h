/**
*	created:		2012-6-28   4:32
*	filename: 		ChatController
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		聊天控制器
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <string>
#include "../../FKCanvas/IFKCanvas.h"
#include "../Include/ChatFilter.h"
#include "../Include/ChatParser2.h"
//------------------------------------------------------------------------
using std::string;
//------------------------------------------------------------------------
#define BUFFER_LEN	1024
#define CONTENT_LEN 512
//------------------------------------------------------------------------
typedef  __int64 UID;
const int WORLD_CHAT_SAME_MAX = 3;
//------------------------------------------------------------------------
// 聊天控制器
class CChatController  
{
public:
	CChatController(void);
	virtual ~CChatController(void);

public:
	WPixel GetColor(int nIndex) const;
	IBitmap * GetFace(int nIndex) const;
	void SetChatString(string strText);
	void SetChatPlayer(string strPlayer);
	BOOL Speak(int nChannel, const char * pszReceiver, const char * pszText);
	int  GetDistance(UID uid);
	BOOL PackSpecial(LPCSTR lpszData,int nLen,char *buf,int &nBufLen);
	BOOL PackGoods(const char *lpsz,char *buf,int& nLen);
private:
	BOOL SendToServer(const char * pData, int nLen);
	BOOL CheckColdTime(BOOL bWorld = FALSE);
private:
	CChatFilter m_chatFilter;
	string m_strWorldString;
	int m_nWorldCount;
	CChatParse m_ChatContentParse;
};
//------------------------------------------------------------------------