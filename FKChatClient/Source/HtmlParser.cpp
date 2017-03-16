/**
*	created:		2012-6-28   8:44
*	filename: 		HtmlParser
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现对HTML脚本的解析
*/
//------------------------------------------------------------------------
#include "../Include/HtmlParser.h"
//------------------------------------------------------------------------
CHtmlParser::CHtmlParser(void)
: m_lpszScript(NULL)
{
}
//------------------------------------------------------------------------
CHtmlParser::~CHtmlParser(void)
{
}
//-----------------------------------------------------------------------------
// 描  述：解析HTML脚本
// 参  数：[输入] const char *lpszScript - HTML脚本
// 返回值：CHtmlNode * - 解析正确返回头节点指针，否则返回NULL
//-----------------------------------------------------------------------------
CHtmlNode * CHtmlParser::Parse(const char * lpszScript)
{
	CHtmlNode * pRootNode = NULL;
	m_lpszScript = lpszScript;
	
	// 从根节点开始递归解析各个节点
	SkipBlank();
	if (*m_lpszScript == '<')
	{
		pRootNode = new CHtmlNode();
		if (pRootNode == NULL)
		{			
			return NULL;
		}
		
		if (!ParseNode(pRootNode))
		{			
			delete pRootNode;
			return NULL;
		}
	}

	return pRootNode;
}
//-----------------------------------------------------------------------------
// 描  述：解析HTML的一个节点，子节点通过递归解析 
// 参  数：[输入] CHtmlNode *pNode - 节点结构的指针
// 返回值：bool - 解析成功返回true,否则返回false
//-----------------------------------------------------------------------------
bool CHtmlParser::ParseNode(CHtmlNode *pNode)
{
	string strTagName;
	
	// 解析标签名称
	m_lpszScript++;
	if(!PickName(strTagName))
	{
		return false;
	}
	pNode->SetName(strTagName);

	// 解析标签的属性和内容
	bool bResult = false;
	if (strTagName == "img" || strTagName == "br"
		  || strTagName == "window")
	{
		bResult = ParseNonregularNode(pNode);
	}
	else
	{
		bResult = ParseRegularNode(strTagName, pNode);
	}

	return bResult;
}

//-----------------------------------------------------------------------------
// 描  述：解析规则节点，就是有关闭部分的节点，如<text></text> 
// 参  数：[输入] string strTagName - 标签的名称
// 参  数：[输出] CHtmlNode *pNode - 标签的节点指针
// 返回值：bool - 解析成功返回true，否则返回false
//-----------------------------------------------------------------------------
bool CHtmlParser::ParseRegularNode(string strTagName, CHtmlNode *pNode)
{				
	// 解析标签属性
	if (!ParseNonregularNode(pNode))
	{
		return false;
	}
	
	// 解析标签的内容,可能包含有子节点
	string strContent;
	while (*m_lpszScript != '<' || *(m_lpszScript + 1) != '/')
	{
		if (*m_lpszScript == '\0')
		{
			return false;
		}

		if (*m_lpszScript == '<')
		{			
			CHtmlNode * pSubNode = new CHtmlNode();
			if (pSubNode == NULL)
			{
				return false;
			}

			// 解析子节点
			if (!ParseNode(pSubNode))
			{
				delete pSubNode;
				return false;
			}
			else
			{
				pNode->AddChild(pSubNode);
			}

			strContent += "<>";
		}
		else
		{		
			strContent += *m_lpszScript++;
		}
	}
	pNode->SetContent(strContent);	
	
	// 检查标签的关闭部分, 如</text>
	string strName;
	m_lpszScript += 2;
	PickName(strName);
	if (strName.compare(strTagName) != 0)
	{
		return false;
	}
	
	return true;
}

//-----------------------------------------------------------------------------
// 描  述：解析不规则节点，就是没有关闭部分的节点，如<br>,<img>等
// 参  数：[输入] string strTagName - 标签的名称
// 参  数：[输出] CHtmlNode *pNode - 标签的节点指针
// 返回值：bool - 解析成功返回true，否则返回false
//-----------------------------------------------------------------------------
bool CHtmlParser::ParseNonregularNode(CHtmlNode *pNode)
{
	// 解析标签属性
	string strAttributeName;
	string strAttributeValue;
	SkipBlank();
	while (*m_lpszScript != '>')
	{
		// 检查是不是脚本结束
		if (*m_lpszScript == '\0')
		{
			return false;
		}
		
		// 提取属性名称
		if (!PickName(strAttributeName))
		{
			return false;
		}
		
		// 提取属性值
		if (!PickValue(strAttributeValue))
		{
			return false;
		}
		
		// 检查属性间有没有分隔
		if (*m_lpszScript != '>' && !iswspace(*m_lpszScript))
		{
			return false;
		}
		
		// 添加到节点
		if (!pNode->AddAttribute(strAttributeName, strAttributeValue))
		{
			return false;
		}
		
		SkipBlank();
	}
	m_lpszScript++;		
	
	return true;
}

//-----------------------------------------------------------------------------
// 描  述：忽略空白字符 
// 参  数：void
// 返回值：void
//-----------------------------------------------------------------------------
void CHtmlParser::SkipBlank(void)
{
	while (iswspace(*m_lpszScript))
	{
		m_lpszScript++;
	}
}

//-----------------------------------------------------------------------------
// 描  述：提取名称文本
// 参  数：[输出] string &strName - 名称文本
// 返回值：bool - 能够正确提取名称文本返回true，否则返回false
//-----------------------------------------------------------------------------
bool CHtmlParser::PickName(string & strName)
{
	strName.clear();

	while ((*m_lpszScript >= 'a' && *m_lpszScript <= 'z')
		|| (*m_lpszScript >= 'A' && *m_lpszScript <= 'Z'))
	{
		strName += *m_lpszScript++;
	}
	
	// 把所有的字母转为小写
	for (string::iterator itr = strName.begin(); itr != strName.end(); itr++)
	{	
		*itr = tolower(*itr);
	}
	
	return (!strName.empty());
}

//-----------------------------------------------------------------------------
// 描  述：提取属性值文本
// 参  数：[输出] string &strValue - 属性值文本
// 返回值：bool - 能够正确提取属性值返回true，否则返回false
//-----------------------------------------------------------------------------
bool CHtmlParser::PickValue(string & strValue)
{
	// 检查开始的部分="
	if (*m_lpszScript != '='     || *(m_lpszScript + 1) != '\"'
		|| *m_lpszScript == '\0' || *(m_lpszScript + 1) == '\0')
	{
		return false;
	}
	m_lpszScript += 2;
	strValue.clear();
	
	// 获取属性值
	while (*m_lpszScript != '\"')
	{
		if (*m_lpszScript == '\0')
		{
			return false;
		}

		strValue += *m_lpszScript++;
	}
	m_lpszScript++;
	
	return true;
}
//------------------------------------------------------------------------