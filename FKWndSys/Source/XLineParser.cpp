/**
*	created:		2012-6-23   23:28
*	filename: 		XLineParser
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
XLineParser::XLineParser()
{
	m_nAlign = XLine::LeftAlign;
}
//------------------------------------------------------------------------
XLineParser::~XLineParser()
{

}
//------------------------------------------------------------------------
BOOL XLineParser::RegisterTagParser(CTagParser* pTagParser)
{
	pTagParser->SetLineParser(this);
	m_vTagParserList.push_back(pTagParser);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL XLineParser::UnregisterTagParser(CTagParser* pTagParser)
{
	vector<CTagParser*>::iterator it;
	vector<CTagParser*>::iterator _end = m_vTagParserList.end();
	for (it=m_vTagParserList.begin(); it!=_end; ++it)
	{
		if ((*it) == pTagParser)
		{
			m_vTagParserList.erase(it);
			return TRUE;
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------
void XLineParser::UnregisterAllTagParser()
{
	m_vTagParserList.clear();
}
//------------------------------------------------------------------------
// "...<key1 attrib list...>...<key2 attrib list...>... ......"
BOOL XLineParser::ParseLine(LPCSTR szLine)
{
	if (szLine == NULL) return TRUE;
	if (!m_pContainer)
	{
		TraceColorLn(MY_TRACE_COLOR, "XLineParser::ParseLine, m_pContainer is NULL");
		return FALSE;
	}

	m_nAlign = -1;
	if (*szLine == '\0') // 一个空行
	{
		TextContext context;
		m_nAlign = XLine::LeftAlign;
		m_pContainer->NewLine(m_nAlign);
		AddText(context);
		return TRUE;
	}

	int   nLen = strlen(szLine);
	char* pBeginTag = (char*)szLine;
	char* pBeginTagNext = 0;
	char* pEndTag = (char*)szLine + nLen;
	BOOL  bFirstTag = TRUE;
	char* pFindBegin = 0;
	char* pFindEnd = 0;
	char* pEnd = (char*)szLine + nLen;

	while (pBeginTag < pEnd)
	{
		// 先查找一个 '<'
		pFindBegin = strstr(pBeginTag, "<"); // 找 '<'
		if (pFindBegin == 0) // 本行从nBeginTagPos位置开始没有任何标签，解析失败
		{
			pEndTag = (char*)szLine + nLen - 1;
			goto ParseFailed;
		}

		// 有标签，检测是否合法
		CTagParser* pTagParser = GetMatchTagParser(pFindBegin, pEnd);
		if (pTagParser == NULL) // 不支持的标签，解析失败，如有'<'，但不是标签的开始
		{
			pEndTag = (char*)szLine + nLen - 1;
			goto ParseFailed;
		}


		// 找到一个合法标签，看看是否有匹配的关闭标签
		pFindEnd = pTagParser->FindEndTag(pFindBegin);
		if (pFindEnd == 0) // 没有匹配的关闭标签，解析失败
		{
			pEndTag = (char*)szLine + nLen - 1;
			goto ParseFailed;
		}

		// 是第一个标签，看看是否是对齐标签，如果不是，则新建一行
		if (bFirstTag)
		{
			bFirstTag = FALSE;
			if (pFindBegin > szLine) // 第一个标签前面还有文字，则作为默认文本添加到本行开头
			{
				if (m_nAlign == -1) // 新建一行
				{
					m_nAlign = XLine::LeftAlign;
					m_pContainer->NewLine(m_nAlign);
				}
				TextContext context;
				context.strText.insert(context.strText.begin(), szLine, pFindBegin);
				AddText(context);
				pBeginTag = pFindBegin;
			}
			if (*(pFindBegin+1) != 'p' && *(pFindBegin+1) != 'P') // 不是对齐标签
			{
				if (m_nAlign == -1) // 新建一行
				{
					m_nAlign = XLine::LeftAlign;
					m_pContainer->NewLine(m_nAlign);
				}
			}
		}

		// 找到合法的标签对，解析他
		if (pBeginTag != pFindBegin) // 可能两个标签之间有些文字没有写标签
		{
			TextContext context;
			context.strText.insert(context.strText.begin(), pBeginTag, pFindBegin);
			AddText(context);
			pBeginTag = pFindBegin;
		}
		pBeginTag = pFindBegin;
		pEndTag = pFindEnd;
		if (!pTagParser->Parse((char*)szLine, pBeginTag, pEndTag))
			goto ParseFailed;
		pBeginTagNext = pEndTag + pTagParser->GetCloseKeyLen();
		pBeginTag = pBeginTagNext;
	}

	return TRUE;

ParseFailed:
	TextContext context;
	if (bFirstTag)
	{
		if (m_nAlign == -1) // 新建一行
		{
			m_nAlign = XLine::LeftAlign;
			m_pContainer->NewLine(m_nAlign);
		}
	}
	// 说明前面的解析成功
	if (pBeginTag != szLine)
		context.strText.insert(context.strText.begin(), pBeginTag, pEndTag+1);
	else
		context.strText	= szLine;
	AddText(context);
	return TRUE;
}
//------------------------------------------------------------------------
CTagParser* XLineParser::GetMatchTagParser(char* pBegin, char* pEnd)
{
	LPCSTR szTag = (LPCSTR)pBegin;
	vector<CTagParser*>::iterator it;
	vector<CTagParser*>::iterator _end = m_vTagParserList.end();
	for (it=m_vTagParserList.begin(); it!=_end; ++it)
	{
		if ((*it)->IsThisTag(szTag))
		{
			return (*it);
		}
	}
	return NULL;
}
//------------------------------------------------------------------------
// 添加一般性文本
BOOL XLineParser::AddText(TextContext context)
{
	if (!m_pContainer)
	{
		TraceColorLn(MY_TRACE_COLOR, "XLineParser::AddText, m_pContainer is NULL");
		return FALSE;
	}

	ITTFont* pFont = XFontCreate::GetInstance()->CreateFont(context.strFontName.c_str(), context.nFontSize, 
		context.bBlod, context.bItalic, context.bUnderline, FALSE, 1024);

	// 新建一个文本条目，并加入条目数组
	XItemText* pItem = new XItemText;
	pItem->SetText(context.strText.c_str());
	pItem->SetFont(pFont);
	pItem->SetTextColor(context.clrText);
	
	if (!m_pContainer->AddItem(static_cast<XItem*>(pItem), FALSE))
	{
		TraceColorLn(MY_TRACE_COLOR, "XLineParser::AddText, AddItem return FALSE");
		return FALSE;
	}
	
	return TRUE;
}
//------------------------------------------------------------------------