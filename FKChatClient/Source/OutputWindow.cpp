/**
*	created:		2012-6-28   10:39
*	filename: 		OutputWindow
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现创建和维护聊天信息的输出窗口
*/
//------------------------------------------------------------------------
#include "../Include/ChatClient.h"
#include "../Include/OutputWindow.h"
#include "../Include/WindowCreator.h"
//------------------------------------------------------------------------
COutputWindow::COutputWindow(void)
: m_pChatOutput(NULL)
, m_pMoveButton(NULL)
, m_pResizeButton(NULL)
, m_pPromptBitmap(NULL)
, m_bPromptOn(FALSE)
{
	m_nAlpha = 100;
	
	// 设置所有的频道都是开通的
	for (int i = 0; i <= CHANNEL_MAX; i++)
	{
		m_channelState[i] = TRUE;
	}
}
//------------------------------------------------------------------------
COutputWindow::~COutputWindow(void)
{
	// 清理历史记录	
	while (m_history.begin() != m_history.end())
	{
		delete *(m_history.begin());
		m_history.pop_front();
	}	

	delete m_pChatOutput;
	delete m_pMoveButton;
	delete m_pResizeButton;
	delete m_pPromptBitmap;
}
//------------------------------------------------------------------------
BOOL COutputWindow::Create(IResObject * pResObject)
{
	int i = 0;	

	// 创建输出主窗口
	if (!CWindowCreator::CreateMainWnd(NULL, this, pResObject))
	{
		return FALSE;
	}

	// 创建聊天信息输出框
	m_pChatOutput = new XChatEx(this);
	if (!CWindowCreator::CreateControl(this, m_pChatOutput, pResObject->GetSubObjectByIndex(i++)))
	{
		return FALSE;
	}
	m_pChatOutput->SetItemFactory(static_cast<IItemFactory *>(&m_itemFactory));

	
	// 创建改变大小按钮
	m_pResizeButton = new XResizeButton(this);
	if (!CWindowCreator::CreateControl(this, m_pResizeButton, pResObject->GetSubObjectByIndex(i++)))
	{
		return FALSE;
	}
	
	// 创建移动按钮
	m_pMoveButton = new XMoveButton(this);
	if (!CWindowCreator::CreateControl(this, m_pMoveButton, pResObject->GetSubObjectByIndex(i++)))
	{
		return FALSE;
	}	
	
	// 创建提示位图
	m_pPromptBitmap = new XStaticBitmap(this);
	if (!CWindowCreator::CreateControl(this, m_pPromptBitmap, pResObject->GetSubObjectByIndex(i++)))
	{
		return FALSE;
	}
	m_pPromptBitmap->SetEnabled(FALSE);

	SetWindowPos(m_rcWnd);
	SetVisible(FALSE);

	TraceLn("ChatClient - COutputWindow::Create OK");
	return TRUE;
}
//------------------------------------------------------------------------
void COutputWindow::ShowWindow(BOOL bVisible)
{
	SetVisible(bVisible);
}

//-------------------------------------------------------------------
// 描  述：设置窗口的透明度 
// 参  数：[输入] int nLevel - 透明度
// 返回值：void
//-------------------------------------------------------------------
void COutputWindow::SetTransparence(int nLevel)
{
	m_nAlpha = nLevel;
}

//-------------------------------------------------------------------
// 描  述：显示聊天信息，同时保存记录 
// 参  数：[输入] const char * lpszData - 聊天数据
// 参  数：[输入] int nLen - 数据长度
// 返回值：BOOL - 如果可以显示则返回TRUE，否则返回FALSE
//-------------------------------------------------------------------
BOOL COutputWindow::Display(const char * lpszData, int nLen)
{	
	// 对数据进行解析
	CHtmlNode * pContentNode = m_chatParser.Parse(lpszData, nLen);
	if (pContentNode == NULL)
	{
		return FALSE;
	}
	
	// 检查显示的结果，如果正确，保存记录
	if (DisplayMessage(pContentNode))
	{
		// 如果记录的数目已达到限制数，则删除第一条记录
		if (m_history.size() >= HISTORY_MAX)
		{
			delete *(m_history.begin());
			m_history.pop_front();
		}
		
		m_history.push_back(pContentNode);
		return TRUE;
	}
	else
	{
		PrintEventLn("ChatClient (FAIL) - COutputWindow::Display");
		delete pContentNode;
		return FALSE;
	}
}

//-------------------------------------------------------------------
// 描  述：设置频道是否显示，并显示不屏蔽的内容
// 参  数：[输入] int nChannel - 指定频道
// 参  数：[输入] BOOL bOnOff - 是否显示
// 返回值：void
//-------------------------------------------------------------------
void COutputWindow::ChannelOn(int nChannel, BOOL bOnOff)
{
	// 防止越界
	if (!IS_VALID_CHANNEL(nChannel))
	{
		return;
	}

	m_channelState[nChannel] = bOnOff;
	m_pChatOutput->Clear();

	// 只显示不屏蔽的记录
	list<CHtmlNode *>::iterator itr = m_history.begin();
	for (; itr != m_history.end(); itr++)
	{
		DisplayMessage(*itr);
	}
}
//------------------------------------------------------------------------
void COutputWindow::SetWindowPos(RECT & rc)
{
	int nSize;		
	RECT rcButton;
	
	XMainWnd::SetWindowPos(rc);

	nSize = GetDesktop()->GetSysScrollBarBitmap()->GetHeight();

	// 设置信息输出框的位置
	if (m_pChatOutput != NULL) 
	{		
		// 确定滚动条的区域		
		RECT rcScrollBar;
		rcScrollBar.left = rc.left;
		rcScrollBar.right = rcScrollBar.left + nSize;
		rcScrollBar.top = rc.top + nSize;
		rcScrollBar.bottom = rc.bottom - nSize;
		
		// 确定容器的区域
		RECT rcContainer;
		CopyRect(&rcContainer, &rc);
		rcContainer.left = rcScrollBar.right;		
				
		m_pChatOutput->SetWindowPos(rcScrollBar, rcContainer);
	}

	// 设置改变大小按钮的位置
	if (m_pResizeButton != NULL) 
	{	
		rcButton.left = rc.left+2;
		rcButton.right = rcButton.left + nSize;
		rcButton.top = rc.top+2;
		rcButton.bottom = rcButton.top + nSize;
		
		m_pResizeButton->SetWindowPos(rcButton);
	}

	// 设置移动按钮的位置
	if (m_pMoveButton != NULL) 
	{
		rcButton.left = rc.left+2;
		rcButton.right = rcButton.left + nSize;
		rcButton.top = rc.bottom - nSize+2;
		rcButton.bottom = rc.bottom;
		
		m_pMoveButton->SetWindowPos(rcButton);
	}

	// 设置提示图标的位置
	if (m_pPromptBitmap != NULL)
	{
		rcButton.left = rc.left;
		rcButton.right = rcButton.left + nSize;
		rcButton.bottom = rc.bottom - nSize;
		rcButton.top = rcButton.bottom - nSize;

		m_pPromptBitmap->SetWindowPos(rcButton);
	}
}
//------------------------------------------------------------------------
void COutputWindow::MoveWindow(int dx, int dy)
{
	XMainWnd::MoveWindow(dx, dy);

	// 同时要调整控件的位置
	SetWindowPos(m_rcWnd);
}

//-------------------------------------------------------------------
// 描  述： 显示一条聊天信息
// 参  数：[输入] const CHtmlNode * pContent - 内容节点
// 返回值：BOOL - 显示正确返回TRUE，否则返回FALSE
//-------------------------------------------------------------------
BOOL COutputWindow::DisplayMessage(const CHtmlNode * pContent)
{
	// 获取频道
	int nChannel;
	pContent->GetAttribute("channel", nChannel, 1);

	// 确定这个频道的内容是否要显示
	if (!m_channelState[nChannel])
	{
		return TRUE;
	}

	// 显示信息的内容
	return m_pChatOutput->DisplayMessage(pContent);
}

//-------------------------------------------------------------------
// 描  述：是不是透明的 
//-------------------------------------------------------------------
BOOL COutputWindow::IsTransparent(POINT& pt)
{
	// 如果SHIFT键按下，则不透明
	if (GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT))
	{
		return FALSE;
	}
	
	if (m_pChatOutput != NULL && PtInRect(&(m_pChatOutput->GetContainerRect()), pt))
	{
		POINT ptTmp;
		ptTmp.x = 0;
		ptTmp.y = 0;
		OnMouseMove(0, 0, ptTmp);		
		
		return TRUE;
	}
	
	return FALSE;	
}

//-------------------------------------------------------------------
// 描  述：有新信息的提示 
//-------------------------------------------------------------------
void COutputWindow::OnTimer(DWORD dwEventID)
{
	if (m_pPromptBitmap->IsVisible())
	{
		m_pPromptBitmap->SetVisible(FALSE);
	}
	else
	{
		m_pPromptBitmap->SetVisible(TRUE);
	}
}

//-------------------------------------------------------------------
// 描  述：开始提示 
//-------------------------------------------------------------------
void COutputWindow::SetPromptOn(BOOL bOn)
{
	ITimeAxis * pTimeAxis = GetTimeAxis();
	if (pTimeAxis == NULL)
	{
		return;
	}

	if (bOn && !m_bPromptOn)
	{
		pTimeAxis->SetTimer(0, static_cast<ITimerSink *>(this), 500, "COutputWindow::SetPromptOn");
		m_bPromptOn = TRUE;
	}
	else if (!bOn && m_bPromptOn)
	{
		pTimeAxis->KillTimer(0, static_cast<ITimerSink *>(this));
		m_pPromptBitmap->SetVisible(FALSE);
		m_bPromptOn = FALSE;
	}
}
//------------------------------------------------------------------------
DWORD COutputWindow::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == 0)
	{	
		SetPromptOn(wParam);
	}

	return 0;
}
//------------------------------------------------------------------------
void COutputWindow::Clear()
{
	m_pChatOutput->Clear();
	// 清理历史记录	
	while (m_history.begin() != m_history.end())
	{
		delete *(m_history.begin());
		m_history.pop_front();
	}
}
//------------------------------------------------------------------------