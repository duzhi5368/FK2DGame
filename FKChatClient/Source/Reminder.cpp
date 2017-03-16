/**
*	created:		2012-6-28   9:07
*	filename: 		Reminder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现系统信息提示器
*/
//------------------------------------------------------------------------
#include "../Include/Reminder.h"
#include "../Include/ChatClient.h"
//------------------------------------------------------------------------
CReminder::CReminder(void)
: m_nType(REMINDER_ROLL)
, m_nLineHeight(15)
, m_clrText(WPixel(255, 255, 100))
, m_pFont(NULL)
{
	SetRect(&m_rcShow, 0, 0, 0, 0);

	// 创建字体
	m_pFont = XFontCreate::GetInstance()->CreateFont("宋体", 12, FALSE, FALSE, FALSE, TRUE);
}
//------------------------------------------------------------------------
CReminder::~CReminder(void)
{
	// 关闭定时器
	ITimeAxis * pTimeAxis = CChatClient::Instance()->GetTimeAxis();
	if (pTimeAxis != NULL)
	{
		pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));
	}	

	// 销毁字体
	XFontCreate::GetInstance()->ReleaseFont(m_pFont);
}
//------------------------------------------------------------------------
void CReminder::OnTimer(DWORD dwEventID)
{
	ITimeAxis * pTimeAxis = CChatClient::Instance()->GetTimeAxis();
	
	if (m_nType == REMINDER_ADD)
	{
		if (m_messages.size() > 0)
		{
			m_messages.pop_front();
		}

		if (m_messages.size() == 0 && pTimeAxis != NULL)
		{
			pTimeAxis->KillTimer(dwEventID, static_cast<ITimerSink *>(this));
		}
	}
	else if (m_nType == REMINDER_REPLACE)
	{
		m_messages.clear();

		if (pTimeAxis != NULL)
		{
			pTimeAxis->KillTimer(dwEventID, static_cast<ITimerSink *>(this));
		}
	}
	else
	{
		AutoAdjust();

		if (m_messages.size() == 0 && pTimeAxis != NULL)
		{
			pTimeAxis->KillTimer(dwEventID, static_cast<ITimerSink *>(this));
		}
	}	
}
//------------------------------------------------------------------------
void CReminder::OnDraw(ICanvas * pCanvas)
{
	RECT rc;
	
	if (pCanvas == NULL)
	{
		return;
	}
	
	CopyRect(&rc, &m_rcShow);	
	
	// 显示所有的信息
	for (MessageListItr itr = m_messages.begin(); itr != m_messages.end(); itr++)
	{  
		rc.top = (*itr).nPos;
		rc.bottom = rc.top + m_nLineHeight;
		
		int x = m_rcShow.left + 5;
		if (m_nType == REMINDER_ADD || m_nType == REMINDER_REPLACE)
		{
			int nOffset = (m_rcShow.right - m_rcShow.left - m_pFont->GetLength((*itr).strText.c_str())) / 2;
			x = m_rcShow.left + nOffset;
		}

		pCanvas->DrawClipText(m_pFont, x, (*itr).nPos, 
			(*itr).strText.c_str(), (*itr).clrText, rc, FONT_NOALPHA);
		
	}
}
//------------------------------------------------------------------------
void CReminder::SetPos(RECT & rc)
{
	m_rcShow = rc;
}
//------------------------------------------------------------------------
void CReminder::SetType(int nType)
{
	m_nType = nType;
}
//------------------------------------------------------------------------
void CReminder::AddMessage(LPCSTR lpszText)
{
	if (lpszText == NULL)
	{
		return;
	}

	ITimeAxis * pTimeAxis = CChatClient::Instance()->GetTimeAxis();
	if (pTimeAxis == NULL)
	{
		return;
	}

	if (m_nType == REMINDER_ROLL)
	{
		if (m_messages.size() == 0)
		{
			pTimeAxis->SetTimer(0, static_cast<ITimerSink *>(this), 150, "CReminder::AddMessage");
		}		
	}
	else if (m_nType == REMINDER_REPLACE)
	{
		if (m_messages.size() != 0)
		{
			pTimeAxis->KillTimer(0, static_cast<ITimerSink *>(this));
			m_messages.clear();
		}
		
		pTimeAxis->SetTimer(0, static_cast<ITimerSink *>(this), 6000, "CReminder::AddMessage");
	}
	else
	{
		if (m_messages.size() == 0)
		{
			pTimeAxis->SetTimer(0, static_cast<ITimerSink *>(this), 6000, "CReminder::AddMessage");
		}
	}

	// 如果有必要，把信息分成多行
	while (strlen(lpszText) != 0)
	{
		string strLine = GetLine(&lpszText);
		AddLine(strLine);
	} 
}
//------------------------------------------------------------------------
void CReminder::ClearMessage(void)
{
	m_messages.clear();
}

//-------------------------------------------------------------------
// 描  述：获取一行文本
//-------------------------------------------------------------------
string CReminder::GetLine(LPCSTR * ppText)
{	
	WORD wWord;	
	int nLen = 0;
	int nLineLen = m_rcShow.right - m_rcShow.left - 5;
	LPCSTR pText = *ppText;		
	BOOL bChinese = FALSE;
	
	while (*pText != '\0')
	{
		wWord = *pText;

		// 检查是不是汉字
		if (isascii(wWord))
		{
			bChinese = FALSE;
		}
		else
		{
			if (*(pText + 1) == '\0')
			{
				++pText;
				break;
			}
			
			bChinese = TRUE;
			memcpy(&wWord, pText, 2);			
		}	
		
		// 检查是不是已经满一行了
		nLen += m_pFont->GetCharLength(wWord);
		if (nLen > nLineLen)
		{
			break;
		}

		// 下一个字符
		if (bChinese)
		{
			pText += 2;
		}
		else
		{
			pText++;
		}
	}
	
	string strLine = string(*ppText, pText - *ppText);
	*ppText = pText;
	
	return strLine;
}

//-------------------------------------------------------------------
// 描  述：添加一行提示信息
// 参  数：[输入] string strText - 文本
// 返回值：void
//-------------------------------------------------------------------
void CReminder::AddLine(string strText)
{
	// 调整信息显示的位置
	for (MessageListItr itr = m_messages.begin(); itr != m_messages.end();)
	{
		MessageListItr preitr = itr++;

		(*preitr).nPos -= m_nLineHeight;
		if ((*preitr).nPos <= m_rcShow.top)
		{
			m_messages.erase(preitr);
		}
	}
	
	SMessage message;
	message.strText = strText;
	message.clrText = m_clrText;
	message.nPos = m_rcShow.bottom - m_nLineHeight;
	
	m_messages.push_back(message);
}

//-------------------------------------------------------------------
// 描  述：自动调整信息的位置、颜色等
//-------------------------------------------------------------------
void CReminder::AutoAdjust(void)
{
	for (MessageListItr itr = m_messages.begin(); itr != m_messages.end();)
	{
		// 如果信息移到了顶端，则删除
		if ((*itr).nPos <= m_rcShow.top)
		{
			MessageListItr curitr = itr++;
			m_messages.erase(curitr);
		}
		// 否则信息向上移动一格
		else
		{
			(*itr).nPos--;
			
			if ((*itr).nPos - m_rcShow.top < 70)
			{
				// 改变显示的颜色
				int nValue = 255 - (70 - ((*itr).nPos - m_rcShow.top)) * 2;
				(*itr).clrText = WPixel(nValue, nValue, 100);
			}			
			
			itr++;
		}
	}	
}
//------------------------------------------------------------------------