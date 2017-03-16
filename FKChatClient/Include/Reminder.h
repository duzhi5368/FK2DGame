/**
*	created:		2012-6-28   9:06
*	filename: 		Reminder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		系统信息提示器
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <list>
#include <string>
#include "../IReminder.h"
//------------------------------------------------------------------------
using std::list;
using std::string;
//------------------------------------------------------------------------
// 系统信息提示器
class CReminder : public IReminder, 
                  public ITimerSink
{
public:
	CReminder(void);
	virtual ~CReminder(void);
	
public:
	virtual void OnTimer(DWORD dwEventID);
	virtual void OnDraw(ICanvas * pCanvas);	
	virtual void SetType(int nType);
	virtual void SetPos(RECT & rc);
	virtual void AddMessage(LPCSTR lpszText);	
	virtual void ClearMessage(void);

protected:
	string GetLine(LPCSTR * ppText);
	void AddLine(string strText);
	void AutoAdjust(void);

protected:	
	struct SMessage
	{
		string strText;		// 提示文本
		WPixel clrText;		// 文本颜色
		int nPos;			// 显示位置
	};	
	typedef list<SMessage> MessageList;
	typedef list<SMessage>::iterator MessageListItr;
	typedef list<SMessage>::const_iterator MessageListCItr;
	
	int m_nType;						// 提示器的类型
	RECT m_rcShow;						// 显示的区域
	int m_nLineHeight;					// 信息行的高度
	WPixel m_clrText;					// 文本的开始颜色
	ITTFont * m_pFont;					// 显示提示的字体
	MessageList m_messages;				// 信息列表
};
//------------------------------------------------------------------------
