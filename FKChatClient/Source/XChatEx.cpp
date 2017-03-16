/**
*	created:		2012-6-28   12:23
*	filename: 		XChatEx
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现聊天信息框扩展，满足位置改变等的需求
*/
//------------------------------------------------------------------------
#include "../Include/XChatEx.h"
//------------------------------------------------------------------------
XChatEx::XChatEx(XWindow * pParent)
: XChat(pParent)
, m_pItemFactory(NULL)
{
	m_nAlpha = 0;
	m_bAutoSroll = TRUE;

	if (m_pVScrollBar != NULL)
	{
		//CONN_posChanged(m_pVScrollBar, XChatEx, OnVScrollBarChanged);
	}	
}
//------------------------------------------------------------------------
XChatEx::~XChatEx(void)
{
}
//------------------------------------------------------------------------
void XChatEx::SetWindowPos(RECT& rc)
{
	XWindow::SetWindowPos(rc);

	if (m_pVScrollBar != NULL)
	{	
		m_pVScrollBar->SetWindowPos(m_rcScrollBar);
		m_pVScrollBar->SetScrollPos(m_pVScrollBar->GetMaxRange(), FALSE);
		m_pVScrollBar->Refresh();
	}
	
	if (m_pContainer != NULL)
	{
		m_pContainer->SetWindowPos(m_rcContainer);
	}
}
//------------------------------------------------------------------------
void XChatEx::SetWindowPos(RECT & rcScrollBar, RECT & rcContainer)
{
	m_rcScrollBar = rcScrollBar;
	m_rcContainer = rcContainer;

	// 确定整个客户端的区域
	RECT rcClient;
	CopyRect(&rcClient, &rcContainer);
	rcClient.left = rcScrollBar.left;

	SetWindowPos(rcClient);
}

//-------------------------------------------------------------------
// 描  述：添加显示元素 
// 参  数：[输入] XItem * pItem - 要显示的条目元素
// 参  数：[输入] BOOL bNewline - 是否换行后再添加
// 返回值：BOOL - 可以添加返回TRUE，否则返回FALSE
//-------------------------------------------------------------------
BOOL XChatEx::AddItem(XItem * pItem, BOOL bNewline)
{
	if (pItem == NULL)
	{
		return FALSE;
	}

	if (!m_pContainer->AddItem(pItem, bNewline))
	{
		delete pItem;
		return FALSE;
	}

	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：显示聊天信息 
// 参  数：[输入] const CHtmlNode * pContent - 信息内容
// 返回值：BOOL - 可以显示返回TRUE,否则返回FALSE
//-------------------------------------------------------------------
BOOL XChatEx::DisplayMessage(const CHtmlNode * pContent)
{
	string strNodeName;
	XItem * pItem = NULL;
	BOOL bNewline = TRUE;

	if (m_pItemFactory == NULL)
	{
		return FALSE;
	}
	
	// 确定是否要自动滚动
	int nPos = m_pVScrollBar->GetScrollPos();
	if (nPos == m_pVScrollBar->GetMaxRange())
	{
		m_bAutoSroll = TRUE;
	}
	else
	{
		m_bAutoSroll = FALSE;
	}

	CHtmlNode::NodeList children = pContent->GetChildren();
	CHtmlNode::NodeList::const_iterator itr = children.begin();
	for (; itr != children.end(); itr++)
	{
		strNodeName = (*itr)->GetName();
		
		if (strNodeName == "text") // 文本
		{			
			pItem = m_pItemFactory->CreateTextItem(static_cast<void *>(*itr));
		}
		else if (strNodeName == "a") // 链接
		{
			pItem = m_pItemFactory->CreateLinkItem2(static_cast<void *>(*itr));			
		}
		else if (strNodeName == "br") // 换行
		{
			pItem = m_pItemFactory->CreateNewlineItem(static_cast<void *>(*itr));
			bNewline = TRUE;
		}
		else if (strNodeName == "img") // 图片
		{
			pItem = m_pItemFactory->CreateImageItem(static_cast<void *>(*itr));			
		}else if(strNodeName == "goods") // 装备
		{
            pItem =  m_pItemFactory->CreateLinkItem2(static_cast<void *>(*itr));	
		}
		else
		{
			return FALSE;
		}
		
		if (AddItem(pItem, bNewline))
		{
			bNewline = FALSE;
		}
		else
		{
			return FALSE;
		}
	} 
	return TRUE;
}
//------------------------------------------------------------------------
void XChatEx::SetItemFactory(IItemFactory * pFactory)
{
	m_pItemFactory = pFactory;
}

//-------------------------------------------------------------------
// 描  述：检查是不是在最后，如果是则自行滚动
//-------------------------------------------------------------------
void XChatEx::AutoScroll(void)
{
	if (m_bAutoSroll)
	{	
		m_pVScrollBar->SetScrollPos(m_pVScrollBar->GetMaxRange());
		m_pParent->OnMessage(0, FALSE, 0);
	}
	else
	{
		m_pParent->OnMessage(0, TRUE, 0);
	}
}
//------------------------------------------------------------------------
RECT XChatEx::GetContainerRect(void) const
{
	return m_rcContainer;
}
//------------------------------------------------------------------------
RECT XChatEx::GetScrollBarRect(void) const
{
	return m_rcScrollBar;
}
//------------------------------------------------------------------------
void XChatEx::OnVScrollBarChanged(DWORD dwParam)
{
	int nPos = *((int *)dwParam);
	if (nPos == m_pVScrollBar->GetMaxRange())
	{
		// 停止提示
		m_pParent->OnMessage(0, FALSE, 0);
	}
}
//------------------------------------------------------------------------