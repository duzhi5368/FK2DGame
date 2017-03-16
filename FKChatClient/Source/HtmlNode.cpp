/**
*	created:		2012-6-28   8:41
*	filename: 		HtmlNode
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现HTML的节点，维护节点的信息
*/
//------------------------------------------------------------------------
#include "../Include/HtmlNode.h"
//------------------------------------------------------------------------
CHtmlNode::CHtmlNode(void)
:m_pParentNode(NULL)
{
}
//------------------------------------------------------------------------
CHtmlNode::~CHtmlNode(void)
{
	// 销毁子节点
	while (m_children.begin() != m_children.end())
	{
		delete *(m_children.begin());
		m_children.pop_front();
	}
}
//------------------------------------------------------------------------
void CHtmlNode::SetName(string strName)
{
	m_strName = strName;
}
//------------------------------------------------------------------------
void CHtmlNode::SetContent(string strContent)
{
	m_strContent = strContent;
}
//------------------------------------------------------------------------
void CHtmlNode::SetAttribute(string strName, string strValue)
{
	m_attributes[strName] = strValue;
}
//------------------------------------------------------------------------
void CHtmlNode::SetAttribute(string strName, int nValue)
{
	char buf[100] = "";

	sprintf(buf, "%d", nValue);
	m_attributes[strName] = buf;
}

//-----------------------------------------------------------------------------
// 描  述：添加一个属性 
// 参  数：[输入] string strName - 属性名称
// 参  数：[输入] string strValue - 属性值
// 返回值：bool - 添加成功返回true，否则返回false
//-----------------------------------------------------------------------------
bool CHtmlNode::AddAttribute(string strName, string strValue)
{
	if (m_attributes.find(strName) == m_attributes.end())
	{
		m_attributes[strName] = strValue;
		return true;
	}
	else
	{
		return false;
	}
}
//-----------------------------------------------------------------------------
// 描  述：设置父结点 
// 参  数：[输入] CHtmlNode * pParent - 子节点指针
// 返回值：void
//-----------------------------------------------------------------------------

void CHtmlNode::SetParent(CHtmlNode *pParent)
{
	m_pParentNode = pParent;
}
//-----------------------------------------------------------------------------
// 描  述：获取父结点
// 返回值：CHtmlNode
//-----------------------------------------------------------------------------
CHtmlNode* CHtmlNode::GetParent()
{
   return m_pParentNode;
}
//-----------------------------------------------------------------------------
// 描  述：获取第一个父结点
// 返回值：CHtmlNode
//-----------------------------------------------------------------------------
CHtmlNode* CHtmlNode::GetFirstParent()
{
   CHtmlNode *pParent = m_pParentNode;
   while(pParent)
   {
	   if( pParent->GetParent() == NULL )
	   {
		   break;
	   }
	   pParent = pParent->GetParent();

   }
   return pParent;
}
//-----------------------------------------------------------------------------
// 描  述：添加一个子节点 
// 参  数：[输入] CHtmlNode * pNode - 子节点指针
// 返回值：void
//-----------------------------------------------------------------------------
void CHtmlNode::AddChild(CHtmlNode * pNode)
{
	pNode->SetParent(this);
	m_children.push_back(pNode);
}
//------------------------------------------------------------------------
string CHtmlNode::GetName(void) const
{
	return m_strName;
}
//------------------------------------------------------------------------
string CHtmlNode::GetContent(void) const
{
	return m_strContent;
}
//-----------------------------------------------------------------------------
// 描  述：获取属性值 
// 参  数：[输入] string strName - 属性名称
// 参  数：[输出] string & strValue - 属性值
// 返回值：bool - 如果能够获取属性值返回true，否则返回false
//-----------------------------------------------------------------------------
bool CHtmlNode::GetAttribute(string strName, string & strValue) const
{
	hash_map<string, string>::const_iterator itr = m_attributes.find(strName);
	if (itr != m_attributes.end())
	{
		strValue = (*itr).second;
		return true;
	}
	else
	{	
		strValue = "";
		return false;
	}
}
//-----------------------------------------------------------------------------
// 描  述：获取属性值 
// 参  数：[输入] string strName - 属性名称
// 参  数：[输出] string & strValue - 属性值
// 参  数：[输出] int nDefault - 默认属性值
// 返回值：bool - 如果能够获取属性值返回true，否则返回false
//-----------------------------------------------------------------------------
bool CHtmlNode::GetAttribute(string strName, int & nValue, int nDefault) const
{
	hash_map<string, string>::const_iterator itr = m_attributes.find(strName);
	if (itr != m_attributes.end())
	{
		nValue = atoi((*itr).second.c_str());
		return true;
	}
	else
	{	
		nValue = nDefault;
		return false;
	}
}
//------------------------------------------------------------------------
const CHtmlNode::NodeList & CHtmlNode::GetChildren(void) const
{
	return m_children;
}
//------------------------------------------------------------------------
DWORD CHtmlNode::GetData(void) const
{
    return m_dwData;
}
//------------------------------------------------------------------------
void CHtmlNode::SetData(DWORD dwData)
{
    m_dwData = dwData;
}
//-----------------------------------------------------------------------------
// 描  述：检查是不是有子节点 
// 参  数：void
// 返回值：bool - 如果有返回true，否则返回false
//-----------------------------------------------------------------------------
bool CHtmlNode::HasChildren(void) const
{
	return (m_children.size() > 0);
}
//------------------------------------------------------------------------