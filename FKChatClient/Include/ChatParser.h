/**
*	created:		2012-6-28   7:17
*	filename: 		ChatParser
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		聊天信息解析类
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "HtmlParser.h"
//------------------------------------------------------------------------
#define  SPECIAL_CONTENT (22)
//------------------------------------------------------------------------
typedef __int64 UID;
//------------------------------------------------------------------------
// 聊天信息解析类
class CChatParser
{

public:	
	CChatParser(void);
	virtual ~CChatParser(void);

public:
	CHtmlNode * Parse(const char * lpszData, int nLen);
	CHtmlNode * Parse(LPCSTR lpszText);

private:
	BOOL OnMonsterChat(const char *pData,int nLen);
	BOOL ParseContent(const char * pContent, int nLen);
	BOOL ParseHeader(const char ** ppData, int & nLen);
	BOOL OnSystemChat(const char ** ppData, int & nLen);
	BOOL OnLocalChat(const char ** ppData, int & nLen);
	BOOL OnPrivateChat(const char ** ppData, int & nLen);
	BOOL OnTeamChat(const char ** ppData, int & nLen);
	BOOL OnClanChat(const char ** ppData, int & nLen);
	BOOL OnWorldChat(const char ** ppData, int & nLen);
	BOOL AddChannel(int nChannel);
	BOOL AddSender(string strSender);
	BOOL CChatParser::AddTime(void);
	BOOL AddSentence(string strText, string strColor);
	BOOL AddText(string strText, string strColor);
	void AddGoods(DWORD dwData);
	int ParseIndex(char * pBuf, int nLen, const char * pData);
	BOOL ParseColor(const char ** ppContent);
	BOOL ParseFace(const char ** ppContent);
	BOOL ParseThing(const char ** ppContent);
	void ShowPaoPao(const char * pszText);
	void OnCustomCmd(LPCSTR lpszText);
	void OnPingCmd(void);
	void OnBulletinCmd(LPCSTR lpszText);
	void OnFailCmd(LPCSTR lpszText);
	BOOL ParseSpecialContent(const char *pContent,int nLen,string& str);
   
private:	
	string m_strColor;
	CHtmlNode * m_pChatNode;
	CHtmlParser m_htmlParser;
	int m_nChannel;
	UID m_uidSender;
	int m_nCustomCmd;
};
//------------------------------------------------------------------------