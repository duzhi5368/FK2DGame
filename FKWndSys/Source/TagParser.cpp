/**
*	created:		2012-6-23   23:18
*	filename: 		TagParser
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		各种标签解析器
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
CTagParser::CTagParser() : m_pLineParser(NULL)
{

}
//------------------------------------------------------------------------
CTagParser::~CTagParser()
{
	m_pLineParser = NULL;
}
//------------------------------------------------------------------------
// 字符变为小写
char CTagParser::GetLowerChar(char* c)
{
	if (*c >= 'A' && *c <= 'Z') *c = *c - 'A' + 'a'; // tolower
	return *c;
}
//------------------------------------------------------------------------
// 跳过空格和tab键之类(小于等于32)的字符
void CTagParser::SkipSpace(char* szLine, char*& pBegin, char* pEnd)
{
	for(; pBegin<pEnd; pBegin++)
	{
		unsigned char c = (unsigned char)*pBegin;
		if (c > 32)
			break;
	}
}
//------------------------------------------------------------------------
// 跳过空格和tab键之类(小于等于32)的字符，同时也跳过引号(")
void CTagParser::SkipSpaceAndQuot(char* szLine, char*& pBegin, char* pEnd)
{
	for(; pBegin<pEnd; pBegin++)
	{
		unsigned char c = (unsigned char)*pBegin;
		if (c > 32 && c != '"')
			break;
	}
}
//------------------------------------------------------------------------
// 要处理的部分形如：" face = 宋体 "
BOOL CTagParser::GetPropertyString(char* szLine, char*& pBegin, char* pEnd, char* buffer, int bufsize)
{
	char* pEqu = strstr(pBegin, "="); // 找 '='
	if (pEqu == 0 || pEqu >= pEnd) // 没有等号？？
	{
		*buffer = '\0';
		return FALSE;
	}

	SkipSpaceAndQuot(szLine, ++pEqu, pEnd);
	char* pQuot = strstr(pEqu, "\""); // 找后面的引号
	if (pQuot == 0 || pQuot >= pEnd) // 没有引号,脚本解析出错
	{
		*buffer = '\0';
		return FALSE;
	}

	ASSERT(pQuot - pEqu < bufsize);
	strncpy(buffer, pEqu, pQuot-pEqu);
	buffer[pQuot-pEqu] = '\0';
	pBegin = pQuot + 1;
	return TRUE;
}
//------------------------------------------------------------------------
// 将一个字符串转成对应的整数
int CTagParser::ConvertStringToInt(LPCSTR szString)
{
	return atoi(szString);
}
//------------------------------------------------------------------------
BOOL CTagParser::IsThisTag(LPCSTR szTag)	
{
	return (strnicmp(szTag, GetOpenKey(), GetOpenKeyLen()) == 0);
}
//------------------------------------------------------------------------
char* CTagParser::FindEndTag(char* pBegin)
{
	return strstr(pBegin, GetCloseKey());
}
//------------------------------------------------------------------------
// 段落标签
// "<p align="center" space="2""
BOOL CTagParserParagraph::Parse(char* szLine, char* pBegin, char* pEnd)
{
	char szValue[MAX_PATH];
	pBegin += GetOpenKeyLen();	// 跳过关键字
	context.clear();
	char* pPropertyEnd = strstr(pBegin, ">");
	if (pPropertyEnd == 0 || pPropertyEnd > pEnd) // 没有 '>'，有问题哟
	{
		TraceLn("error: '<' not match; %s", szLine);
		return FALSE;
	}
	int nPropertyEndPos = pPropertyEnd - szLine;
	while (pBegin < pPropertyEnd)
	{
		SkipSpaceAndQuot(szLine, pBegin, pPropertyEnd);
		char c = GetLowerChar(pBegin);
		switch (c) 
		{
		case 'a': // align
			{
				if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
					return FALSE;
				unsigned char c = *szValue;
				if (c == 'l')
					context.nAlign = XLine::LeftAlign;
				else if (c == 'c')
					context.nAlign = XLine::CenterAlign;
				else if (c == 'r')
					context.nAlign = XLine::RightAlign;
				else // 其他不支持的对齐格式，解析失败
					return FALSE;
			}			
			break;
		case 's': // line space
			{
				if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
					return FALSE;
				context.nLineSpace = ConvertStringToInt(szValue);
			}
			break;
		default:
			return FALSE;
		}
	}
	return Emit();
}
//------------------------------------------------------------------------
BOOL CTagParserParagraph::Emit()
{
	XItemContainer* pContainer = m_pLineParser->GetItemContainer();
	if (pContainer)
	{
		return (pContainer->NewLine(context.nAlign, context.nLineSpace) != NULL);
	}
	return FALSE;
}
//------------------------------------------------------------------------
// 文本标签
// "<text face="宋体" size="14" color="255,255,255" bold="1" italic="0" underline="1">文本正文"
// 去掉了关键字前面的"<"和无效字符(如空格、tab),去掉了关闭标签
BOOL CTagParserText::Parse(char* szLine, char* pBegin, char* pEnd)
{
	char szValue[MAX_PATH];
	pBegin += GetOpenKeyLen();	// 跳过关键字
	context.clear();
	char* pPropertyEnd = strstr(pBegin, ">");
	if (pPropertyEnd == 0 || pPropertyEnd > pEnd) // 没有 '>'，有问题哟
	{
		TraceLn("error: '<' not match; %s", szLine);
		return FALSE;
	}
	while (pBegin < pPropertyEnd)
	{
		SkipSpaceAndQuot(szLine, pBegin, pPropertyEnd);
		char c = GetLowerChar(pBegin);
		switch (c) 
		{
		case 'f': // face
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.strFontName = szValue;
			break;
		case 's': // size
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.nFontSize = ConvertStringToInt(szValue);
			break;
		case 'c': // color
			{
				if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
					return FALSE;
				int r,g,b;
				r = g = b = 0;
				sscanf(szValue, "%d,%d,%d", &r, &g, &b);
				context.clrText = WPixel(r, g, b);
			}
			break;
		case 'b': // bold
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.bBlod = ConvertStringToInt(szValue);
			break;
		case 'i': // italic
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.bItalic = ConvertStringToInt(szValue);
			break;
		case 'u': // underline
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.bUnderline = ConvertStringToInt(szValue);
			break;
		default:
			return FALSE;
		}
	}
	
	// 文本正文
	context.strText.insert(context.strText.begin(), pPropertyEnd+1, pEnd);
	return Emit();
}
//------------------------------------------------------------------------
BOOL CTagParserText::Emit()
{
	XItemContainer* pContainer = m_pLineParser->GetItemContainer();
	if (pContainer)
	{
		ITTFont* pFont = XFontCreate::GetInstance()->CreateFont(context.strFontName.c_str(), context.nFontSize, 
			context.bBlod, context.bItalic, context.bUnderline, FALSE, 1024);
		
		// 新建一个文本条目，并加入条目数组
		XItemText* pItem = new XItemText;
		pItem->SetText(context.strText.c_str());
		pItem->SetFont(pFont);
		pItem->SetTextColor(context.clrText);
		
		if (!pContainer->AddItem(static_cast<XItem*>(pItem), FALSE))
		{
			TraceColorLn(MY_TRACE_COLOR, "CTagParserText::Parse, AddItem return FALSE");
			return FALSE;
		}
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
// 链接文本标签
// "<link face="宋体" size="14" color="255,255,255" bold="1" italic="0" underline="1" url="nextpage.xhtm#bookmark1">链接文本正文"
// 去掉了关键字前面的"<"和无效字符(如空格、tab),去掉了关闭标签
BOOL CTagParserLink::Parse(char* szLine, char* pBegin, char* pEnd)
{
	//string str;
	//str.insert(str.begin(), pBegin, pEnd);
	char szValue[MAX_PATH];
	pBegin += GetOpenKeyLen();	// 跳过关键字
	context.clear();
	char* pPropertyEnd = strstr(pBegin, ">");
	if (pPropertyEnd == 0 || pPropertyEnd > pEnd) // 没有 '>'，有问题哟
	{
		TraceLn("error: '<' not match; %s", szLine);
		return FALSE;
	}
	while (pBegin < pPropertyEnd)
	{
		SkipSpaceAndQuot(szLine, pBegin, pPropertyEnd);
		char c = GetLowerChar(pBegin);
		switch (c) 
		{
		case 'f': // face
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.strFontName = szValue;
			break;
		case 's': // size
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.nFontSize = ConvertStringToInt(szValue);
			break;
		case 'c': // color
			{
				if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
					return FALSE;
				int r,g,b;
				r = g = b = 0;
				sscanf(szValue, "%d,%d,%d", &r, &g, &b);
				context.clrText = WPixel(r, g, b);
			}
			break;
		case 'b': // bold
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.bBlod = ConvertStringToInt(szValue);
			break;
		case 'i': // italic
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.bItalic = ConvertStringToInt(szValue);
			break;
		case 'u': // underline;url
			{
				char c = GetLowerChar(pBegin+1);
				if (c == 'n')
				{
					GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH);
					context.bUnderline = ConvertStringToInt(szValue);
				}
				else // url
				{
					GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH);
					context.strLinkText = szValue;
					
				}
			}
			break;
		}
	}
	
	// 文本正文
	context.strText.insert(context.strText.begin(), pPropertyEnd+1, pEnd);
	return Emit();
}
//------------------------------------------------------------------------
BOOL CTagParserLink::Emit()
{
	XItemContainer* pContainer = m_pLineParser->GetItemContainer();
	if (pContainer)
	{
		ITTFont* pFont = XFontCreate::GetInstance()->CreateFont(context.strFontName.c_str(), context.nFontSize, 
			context.bBlod, context.bItalic, context.bUnderline, FALSE, 1024);
		
		// 新建一个文本条目，并加入条目数组
		XItemLinkText* pItem = new XItemLinkText;
		pItem->SetText(context.strText.c_str());
		pItem->SetFont(pFont);
		pItem->SetTextColor(context.clrText);
		
		pItem->SetLinkText(context.strLinkText.c_str());
		pItem->SetVisited(FALSE);
		pItem->SetNotVisitedColor(WPixel(0,0,255));
		pItem->SetVisitedColor(WPixel(128,0,128));
		pItem->SetRolloverColor(WPixel(255,0,0));

		if (!pContainer->AddItem(static_cast<XItem*>(pItem), FALSE))
		{
			TraceColorLn(MY_TRACE_COLOR, "CTagParserLink::Parse, AddItem return FALSE");
			return FALSE;
		}
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
// 书签文本标签
// "<bookmark face="宋体" size="14" color="255,255,255" bold="1" italic="0" underline="1" mark="bookmark2">文本正文"
// 去掉了关键字前面的"<"和无效字符(如空格、tab),去掉了关闭标签
BOOL CTagParserBookmark::Parse(char* szLine, char* pBegin, char* pEnd)
{
	char szValue[MAX_PATH];
	pBegin += GetOpenKeyLen();	// 跳过关键字
	context.clear();
	char* pPropertyEnd = strstr(pBegin, ">");
	if (pPropertyEnd == 0 || pPropertyEnd > pEnd) // 没有 '>'，有问题哟
	{
		TraceLn("error: '<' not match; %s", szLine);
		return FALSE;
	}
	while (pBegin < pPropertyEnd)
	{
		SkipSpaceAndQuot(szLine, pBegin, pPropertyEnd);
		char c = GetLowerChar(pBegin);
		switch (c) 
		{
		case 'f': // face
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.strFontName = szValue;
			break;
		case 's': // size
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.nFontSize = ConvertStringToInt(szValue);
			break;
		case 'c': // color
			{
				if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
					return FALSE;
				int r,g,b;
				r = g = b = 0;
				sscanf(szValue, "%d,%d,%d", &r, &g, &b);
				context.clrText = WPixel(r, g, b);
			}
			break;
		case 'b': // bold
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.bBlod = ConvertStringToInt(szValue);
			break;
		case 'i': // italic
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.bItalic = ConvertStringToInt(szValue);
			break;
		case 'u': // underline
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.bUnderline = ConvertStringToInt(szValue);
			break;
		case 'm': // bookmark
			GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH);
			context.strBookmark = szValue;
			break;
		}
	}
	
	// 文本正文
	context.strText.insert(context.strText.begin(), pPropertyEnd+1, pEnd);
	return Emit();
}
//------------------------------------------------------------------------
BOOL CTagParserBookmark::Emit()
{
	XItemContainer* pContainer = m_pLineParser->GetItemContainer();
	if (pContainer)
	{
		ITTFont* pFont = XFontCreate::GetInstance()->CreateFont(context.strFontName.c_str(), context.nFontSize, 
			context.bBlod, context.bItalic, context.bUnderline, FALSE, 1024);
		
		// 新建一个文本条目，并加入条目数组
		XItemBookmarkText* pItem = new XItemBookmarkText;
		pItem->SetText(context.strText.c_str());
		pItem->SetFont(pFont);
		pItem->SetTextColor(context.clrText);

		pItem->SetBookmark(context.strBookmark.c_str());
		
		if (!pContainer->AddItem(static_cast<XItem*>(pItem), FALSE))
		{
			TraceColorLn(MY_TRACE_COLOR, "CTagParserBookmark::Parse, AddItem return FALSE");
			return FALSE;
		}
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
// 图片标签
// "<img src="e:\face01.bmp" frame="4""
// 去掉了关键字前面的"<"和无效字符(如空格、tab),去掉了关闭标签
BOOL CTagParserImage::Parse(char* szLine, char* pBegin, char* pEnd)
{
	char szValue[MAX_PATH];
	pBegin += GetOpenKeyLen();	// 跳过关键字
	context.clear();
	char* pPropertyEnd = strstr(pBegin, ">");
	if (pPropertyEnd == 0 || pPropertyEnd > pEnd) // 没有 '>'，有问题哟
	{
		TraceLn("error: '<' not match; %s", szLine);
		return FALSE;
	}
	while (pBegin < pPropertyEnd)
	{
		SkipSpaceAndQuot(szLine, pBegin, pPropertyEnd);
		char c = GetLowerChar(pBegin);
		switch (c) 
		{
		case 's': // src
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.strImage = szValue;
			break;
		case 'f': // frame
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.nCount = ConvertStringToInt(szValue);
			break;
		case 'd': // delay
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.nDelay = ConvertStringToInt(szValue);
			break;
		case 't': // type
			if (!GetPropertyString(szLine, pBegin, pPropertyEnd, szValue, MAX_PATH))
				return FALSE;
			context.nType = ConvertStringToInt(szValue);
			break;
		default:
			return FALSE;
		}
	}
	
	return Emit();
}
//------------------------------------------------------------------------
BOOL CTagParserImage::Emit()
{
	XItemContainer* pContainer = m_pLineParser->GetItemContainer();
	if (pContainer)
	{
		// 加载图片
		IBitmap* pImage;
		if (context.nType == 0)
			pImage = CImageLoaderEx::GetInstance()->LoadImage(context.strImage.c_str());
		else
			pImage = CImageLoader::GetInstance()->LoadImage(context.strImage.c_str());
		if (pImage == NULL)
			return FALSE;
		
		// 新建一个图片条目，并加入条目数组
		XItemImage* pItem = new XItemImage;
		pItem->SetBitmap(pImage);
		pItem->SetDelay(context.nDelay);
		pItem->SetFrameCount(context.nCount);
		
		if (!pContainer->AddItem(static_cast<XItem*>(pItem), FALSE))
		{
			TraceColorLn(MY_TRACE_COLOR, "CTagParserImage::Parse, AddItem return FALSE");
			return FALSE;
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------