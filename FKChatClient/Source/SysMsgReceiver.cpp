/**
*	created:		2012-6-28   9:11
*	filename: 		SysMsgReceiver
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现系统信息接收器
*/
//------------------------------------------------------------------------
#include "../Include/SysMsgReceiver.h"
#include "../IReminder.h"
#include "../Include/Reminder.h"
#include "../Include/ChatClient.h"
//------------------------------------------------------------------------
CSysMsgReceiver::CSysMsgReceiver(void)
: m_pMiddleReminder(NULL)
, m_pAboveReminder(NULL)
, m_pFloatReminder(NULL)
, m_pUnderReminder(NULL)
, m_bPromptOn(FALSE)
{
}
//------------------------------------------------------------------------
CSysMsgReceiver::~CSysMsgReceiver(void)
{
}

//-------------------------------------------------------------------
// 描  述：接收系统信息 
//-------------------------------------------------------------------
void CSysMsgReceiver::OnAction(IEvent * pEvent)
{
	if (pEvent == NULL || !m_bPromptOn)
	{
		return;
	}

	// 获取数据
	int nLen = 0;
	BYTE * pData = pEvent->GetContext(nLen);
	if (pData == NULL || nLen < sizeof(SSysMessage))
	{
		return;
	}

	const SSysMessage * pMessage = (const SSysMessage *)pData;

	// 检查消息的长度是不是正确
	if (nLen != (sizeof(SSysMessage) + pMessage->nTextLen))
	{
		return;
	}

	// 获取提示信息
	char buf[512] = "";
	memset(buf, 0, 512);
	int nTextLen = (pMessage->nTextLen < 512) ? pMessage->nTextLen : 510;
	strncpy(buf, (char *)pData + sizeof(SSysMessage), nTextLen);
	// 处理各种类型的信息
	ShowMessage(pMessage->nType, buf);
}

//-------------------------------------------------------------------
// 描  述：接收系统信息 
//-------------------------------------------------------------------
void CSysMsgReceiver::ShowMessage(int nType, const char * pszText)
{
	if (pszText == NULL)
	{
		return;
	}
	
	// 处理各种类型的信息
	switch (nType)
	{
	case SMT_FLOAT:
		OnFloatMessage(pszText);
		break;

	case SMT_ABOVE:
		OnAboveMessage(pszText);
		break;	

	case SMT_MIDDLE:
		OnMiddleMessage(pszText);
		break;					
		
	case SMT_UNDER:
		OnUnderMessage(pszText);
		break;

	case SMT_HELP:
		CChatClient::Instance()->m_pHelpInfoWnd->AddMessage(pszText);
		break;

	default:
		break;
	}
}

//-------------------------------------------------------------------
// 描  述：显示在浮动提示栏
//-------------------------------------------------------------------
void CSysMsgReceiver::OnFloatMessage(const char * pszText)
{
	if (m_pFloatReminder != NULL)
	{
		m_pFloatReminder->AddMessage(pszText);
	}
}

//-------------------------------------------------------------------
// 描  述：显示在上方提示栏
//-------------------------------------------------------------------
void CSysMsgReceiver::OnAboveMessage(const char * pszText)
{
	if (m_pAboveReminder != NULL)
	{
		m_pAboveReminder->AddMessage(pszText);
	}
}

//-------------------------------------------------------------------
// 描  述：显示在中间提示栏
//-------------------------------------------------------------------
void CSysMsgReceiver::OnMiddleMessage(const char * pszText)
{
	if (m_pMiddleReminder != NULL)
	{
		m_pMiddleReminder->AddMessage(pszText);
	}
}

//-------------------------------------------------------------------
// 描  述：显示在下方提示栏
//-------------------------------------------------------------------
void CSysMsgReceiver::OnUnderMessage(const char * pszText)
{
	if (m_pUnderReminder != NULL)
	{
		m_pUnderReminder->AddMessage(pszText);
	}
}

//-------------------------------------------------------------------
// 描  述：获取区域 
//-------------------------------------------------------------------
RECT CSysMsgReceiver::GetRect(IResObject * pResObject)
{
	RECT rc;
	SetRect(&rc, 0, 0, 0, 0);

	if (pResObject == NULL)
	{
		return rc;
	}

	// 获取属性值
	LPCSTR pProperty = pResObject->GetPropertyString("rect");
	if (pProperty != NULL)
	{
		int l, t, w, h;
		sscanf(pProperty, "%d,%d,%d,%d", &l, &t, &w, &h);
		SetRect(&rc, l, t, l + w, t + h);
	}

	return rc;
}

//-------------------------------------------------------------------
// 描  述：初始化 
//-------------------------------------------------------------------
BOOL CSysMsgReceiver::Init(IResObject * pResObject)
{
	int i = 0;
	RECT rc;

	if (pResObject == NULL)
	{
		return FALSE;
	}

	// 中间提示栏
	if ((m_pMiddleReminder = new CReminder()) == NULL)
	{
		return FALSE;
	}
	rc = GetRect(pResObject->GetSubObjectByIndex(i++));
	m_pMiddleReminder->SetPos(rc);	
	m_pMiddleReminder->SetType(REMINDER_REPLACE);

	// 上方提示栏
	if ((m_pAboveReminder = new CReminder()) == NULL)
	{
		return FALSE;
	}
	rc = GetRect(pResObject->GetSubObjectByIndex(i++));
	m_pAboveReminder->SetPos(rc);
	m_pAboveReminder->SetType(REMINDER_ADD);

	// 浮动提示栏
	if ((m_pFloatReminder = new CReminder()) == NULL)
	{
		return FALSE;
	}
	rc = GetRect(pResObject->GetSubObjectByIndex(i++));
	m_pFloatReminder->SetPos(rc);

	// 下方提示栏
	if ((m_pUnderReminder = new CReminder()) == NULL)
	{
		return FALSE;
	}
	rc = GetRect(pResObject->GetSubObjectByIndex(i++));
	m_pUnderReminder->SetPos(rc);
	m_pUnderReminder->SetType(REMINDER_REPLACE);

	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：重设系统消息接收器
//-------------------------------------------------------------------
void CSysMsgReceiver::Reset(void)
{
	m_pMiddleReminder->ClearMessage();
	m_pAboveReminder->ClearMessage();
	m_pFloatReminder->ClearMessage();
	m_pUnderReminder->ClearMessage();
	m_bPromptOn = FALSE;
}

//-------------------------------------------------------------------
// 描  述：开启提示信息 
//-------------------------------------------------------------------
void CSysMsgReceiver::SetPromptOn(void)
{
	m_bPromptOn = TRUE;
}

//-------------------------------------------------------------------
// 描  述：画系统信息
//-------------------------------------------------------------------
void CSysMsgReceiver::OnSysMsgDraw(ICanvas * pCanvas)
{
	m_pMiddleReminder->OnDraw(pCanvas);
	m_pAboveReminder->OnDraw(pCanvas);
	m_pFloatReminder->OnDraw(pCanvas);
	m_pUnderReminder->OnDraw(pCanvas);
}

//------------------------------------------------------------------------
void CSysMsgReceiver::Clear(int nType)
{
	switch(nType)
	{
	case -1:
		{
           if( m_pFloatReminder )
		   {
		      m_pFloatReminder->ClearMessage();
		   }
           if( m_pAboveReminder )
		   {
			   m_pAboveReminder->ClearMessage();
		   }
           if( m_pMiddleReminder )
		   {
			   m_pMiddleReminder->ClearMessage();
		   }
           if( m_pUnderReminder )
		   {
			   m_pUnderReminder->ClearMessage();
		   }
           break;
		}
	default:
		{
           break;
		}
	}
}
//------------------------------------------------------------------------