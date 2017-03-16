/**
*	created:		2012-6-23   21:57
*	filename: 		XFontCreate
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
//------------------------------------------------------------------------
XFontCreate* XFontCreate::_instance = NULL;
//------------------------------------------------------------------------
inline BOOL XFontCreate::stFontInfo::operator == (const stFontInfo& info) const
{
	return (nSize==info.nSize && bBlod==info.bBlod && bItalic==info.bItalic &&
		bUnderline==info.bUnderline && bWithOutline==info.bWithOutline &&
		stricmp(szName, info.szName)==0);
}
//------------------------------------------------------------------------
XFontCreate::XFontCreate()
{
	m_vFontList.clear();
	m_pDefFont = CreateFont("宋体", 12);	// 系统默认字体
	XFontCreate::_instance = this;			// 如果直接定义这个类的全局变量，则不需要new出来
}
//------------------------------------------------------------------------
XFontCreate::~XFontCreate()
{
	Clear();
}
//------------------------------------------------------------------------
// 获得字体创建实例(始终保持单实例)
XFontCreate* XFontCreate::GetInstance()
{
	if (XFontCreate::_instance == NULL)
		XFontCreate::_instance = new XFontCreate;
	return XFontCreate::_instance;
}
//------------------------------------------------------------------------
// 释放字体创建实例
void XFontCreate::ReleaseInstance()
{
	if (XFontCreate::_instance)
	{
		XFontCreate::_instance->Clear();
		XFontCreate* p = XFontCreate::_instance;
		XFontCreate::_instance = NULL;
		delete p;
	}
}
//------------------------------------------------------------------------
// 清除资源
void XFontCreate::Clear()
{
	CacheFontListPtr _end = m_vFontList.end();
	for (CacheFontListPtr it=m_vFontList.begin(); it!=_end; ++it)
	{
		(*it).pFont->Release();
		DeleteObject((*it).hFont);
	}
	m_vFontList.clear();
}
//------------------------------------------------------------------------
// 创建某种字体上下文字体，如果缓冲中存在此字体上下文字体，则直接返回
// 缓冲中的字体，同时将该字体引用计数加1
ITTFont* XFontCreate::CreateFont(const char* szFontName, int nFontSize, 
					BOOL bBlod/* = FALSE*/, BOOL bItalic/* = FALSE*/, BOOL bUnderLine/* = FALSE*/, 
					BOOL bWithOutline/* = FALSE*/, int nCacheCharSize/* = 128*/)
{
	stFontInfo info;
	strncpy(info.szName, szFontName, stFontInfo::max_fontname_len);
	info.nSize = nFontSize;
	info.bBlod = bBlod;
	info.bItalic = bItalic;
	info.bUnderline = bUnderLine;
	info.bWithOutline = bWithOutline;
	info.nCacheCharSize = nCacheCharSize;
	
	CacheFontListPtr _end = m_vFontList.end();
	for (CacheFontListPtr it=m_vFontList.begin(); it!=_end; ++it)
	{
		if (info == (*it).info) // 有相同的字体，则引用计数加1
		{
			(*it).nRefCount ++;
			return (*it).pFont;
		}
	}
	// 缓存中没有该字体，创建它
	stCacheFont cachefont;
	cachefont.info = info;
	cachefont.nRefCount = 0;
	cachefont.pFont = NULL;
	cachefont.hFont = 0;
	if (!_createFont(cachefont))
		return NULL;
	
	cachefont.nRefCount = 1;
	m_vFontList.push_back(cachefont);
#ifdef _DEBUG
	if (m_vFontList.size() >= 20)
	{
		OutputDebugString("WARNING: Font count >= 20 !!!!!\r\n");
	}
#endif
	return cachefont.pFont;
}
//------------------------------------------------------------------------
// 释放某种字体上下文字体，如果缓冲中存在此字体上下文字体，则直接将该字体引用计数减1
// 如果计数引用为0，则删除缓冲中的该字体
void XFontCreate::ReleaseFont(const char* szFontName, int nFontSize, 
								 BOOL bBlod/* = FALSE*/, BOOL bItalic/* = FALSE*/, BOOL bUnderLine/* = FALSE*/, 
								 BOOL bWithOutline/* = FALSE*/, int nCacheCharSize/* = 128*/)
{
	stFontInfo info;
	strncpy(info.szName, szFontName, stFontInfo::max_fontname_len);
	info.nSize = nFontSize;
	info.bBlod = bBlod;
	info.bItalic = bItalic;
	info.bUnderline = bUnderLine;
	info.bWithOutline = bWithOutline;
	info.nCacheCharSize = nCacheCharSize;
	
	CacheFontListPtr _end = m_vFontList.end();
	for (CacheFontListPtr it=m_vFontList.begin(); it!=_end; ++it)
	{
		if (info == (*it).info) // 有相同的字体，则引用计数减1
		{
			if (--((*it).nRefCount) == 0)
			{
				(*it).pFont->Release();
				DeleteObject((*it).hFont);
				m_vFontList.erase(it);
#ifdef _DEBUG
				if (m_vFontList.size() <= 1)
				{
					OutputDebugString("WARNING: Font count <= 1 !!!!!\r\n");
				}
#endif
				break;
			}
		}
	}
}
//------------------------------------------------------------------------
void XFontCreate::ReleaseFont(ITTFont* pFont)
{

	CacheFontListPtr _end = m_vFontList.end();
	for (CacheFontListPtr it=m_vFontList.begin(); it!=_end; ++it)
	{
		stCacheFont	CacheFont = (*it);
		if (CacheFont.pFont == pFont) // 有相同的字体，则引用计数减1
		{
			if (--CacheFont.nRefCount == 0)
			{
				CacheFont.pFont->Release();
				DeleteObject(CacheFont.hFont);
				m_vFontList.erase(it);
#ifdef _DEBUG
				if (m_vFontList.size() <= 1)
				{
					OutputDebugString("WARNING: Font count <= 1 !!!!!\r\n");
				}
#endif
			}
			break;
		}
	}
}
//------------------------------------------------------------------------
// 创建一个真正的字体
BOOL XFontCreate::_createFont(stCacheFont& cachefont)
{
	int fnWeight = (cachefont.info.bBlod) ? FW_BOLD : FW_NORMAL;
	HDC hdc = GetDC(NULL);
	cachefont.hFont = ::CreateFont(cachefont.info.nSize, 0, 0, 0,
		fnWeight, cachefont.info.bItalic, cachefont.info.bUnderline, FALSE, 
		DEFAULT_CHARSET, OUT_DEVICE_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_SWISS, cachefont.info.szName);
	if (cachefont.hFont == 0)
	{
		ReleaseDC(NULL, hdc);
		return FALSE;
	}
	
	if (!m_CanvasHelper.CreateITTFont(&cachefont.pFont))
	{
		OutputDebugString("XFontCreate::_createXFont(), CreateITTFont fail!\r\n");
		DeleteObject(cachefont.hFont);
		ReleaseDC(NULL, hdc);
		return FALSE;
	}
	
	if (!cachefont.pFont->Create(cachefont.hFont, cachefont.info.bWithOutline, cachefont.info.nCacheCharSize))
	{
		OutputDebugString("XFontCreate::_createXFont(), ITTFont::Create() fail!\r\n");
		cachefont.pFont->Release();
		DeleteObject(cachefont.hFont);
		ReleaseDC(NULL, hdc);
		return FALSE;
	}
	
	ReleaseDC(NULL, hdc);
	return TRUE;
}
//------------------------------------------------------------------------
XFontCreate::stFontInfo* XFontCreate::GetFontInfo(ITTFont* pFont)
{
	CacheFontListPtr _end = m_vFontList.end();
	for (CacheFontListPtr it=m_vFontList.begin(); it!=_end; ++it)
	{
		stCacheFont&	CacheFont = (*it);
		if (CacheFont.pFont == pFont) // 有相同的字体，则引用计数减1
		{
			return &CacheFont.info;
		}
	}
	return NULL;
}
//------------------------------------------------------------------------