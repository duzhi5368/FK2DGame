/**
*	created:		2012-6-28   8:57
*	filename: 		MessagBuilder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现聊天信息构造器
*/
//------------------------------------------------------------------------
#include "../Include/MessageBuilder.h"
//------------------------------------------------------------------------
CMessageBuilder::CMessageBuilder(void)
: m_pContainer(NULL)
{
}
//------------------------------------------------------------------------
CMessageBuilder::~CMessageBuilder(void)
{
}

//-------------------------------------------------------------------
// 描  述：设置信息容器 
//-------------------------------------------------------------------
void CMessageBuilder::SetContainer(XItemContainer * pContainer)
{
	m_pContainer = pContainer;
}

//-------------------------------------------------------------------
// 描  述： 构造聊天内容
//-------------------------------------------------------------------
BOOL CMessageBuilder::Build(LPCSTR lpszText)
{
	// 解析要显示的内容
	CHtmlNode * pRootNode = m_chatParser.Parse(lpszText);
	if (pRootNode == NULL)
	{
		return FALSE;
	}

	// 把内容添加到显示容器
	BOOL bRet = AddToContainer(pRootNode);

	delete pRootNode;	
	return bRet;
}

//-------------------------------------------------------------------
// 描  述：把信息内容添加到显示容器 
//-------------------------------------------------------------------
BOOL CMessageBuilder::AddToContainer(const CHtmlNode * pContent)
{
	string strNodeName;
	XItem * pItem = NULL;
	BOOL bNewline = FALSE;
	
	// 遍历所有的节点
	CHtmlNode::NodeList children = pContent->GetChildren();
	CHtmlNode::NodeList::const_iterator itr = children.begin();
	for (; itr != children.end(); itr++)
	{
		strNodeName = (*itr)->GetName();
		
		if (strNodeName == "text") // 文本
		{			
			pItem = m_itemFactory.CreateTextItem(static_cast<void *>(*itr));
		}
		else if (strNodeName == "a") // 链接
		{
			pItem = m_itemFactory.CreateLinkItem(static_cast<void *>(*itr));			
		}
		else if (strNodeName == "br") // 换行
		{
			pItem = m_itemFactory.CreateNewlineItem(static_cast<void *>(*itr));
			bNewline = TRUE;
		}
		else if (strNodeName == "img") // 图片
		{
			pItem = m_itemFactory.CreateImageItem(static_cast<void *>(*itr));			
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

//-------------------------------------------------------------------
// 描  述：添加显示元素 
//-------------------------------------------------------------------
BOOL CMessageBuilder::AddItem(XItem * pItem, BOOL bNewline)
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
//------------------------------------------------------------------------