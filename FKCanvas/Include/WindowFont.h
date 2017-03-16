/**
*	created:		2012-6-19   21:14
*	filename: 		WindowFont
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../IFKCanvas.h"
//------------------------------------------------------------------------
class XFont  : public ITTFont  
{
	struct Data{
		unsigned int c;
		int freq;
		unsigned short idx;
	};
private:
	Data* m_Data;							// 各个字符数据的顺序列表
	int m_nCacheSize;						// 缓存大小
	int m_nCacheNum;						// 已经缓存的数目
	WFontData* m_FontData;					// 缓存的字符数据
	BYTE*	m_CharBuf;						// 字符数据缓存
	HDC m_FontDC;							// 字体绘制DC
	HBITMAP m_FontBmp;						// alpha=0时的字体位图
	WORD* m_Buffer;							// 字缓存
	int m_Height;							// 字体高度
	int m_Locked;							// 
	int m_nFreq;							// 频率
	int m_bOutline;							// 是否绘制轮廓
	int m_MaxWidth;							// 绘制字符的位图的最大宽度
private:
	int SearchChar(unsigned int c) const;	// 在 Cache 里搜索一个字符
	void ExpandCacheSize(int newsize);		// 扩展 CacheSize
	void Destroy();
public:
	XFont() : m_nCacheSize(0), m_Height(0), m_FontDC(0), m_bOutline(0), m_nFreq(0), m_CharBuf(0) {}
	virtual ~XFont()			{Destroy();}
	
	virtual BOOL Create(HFONT font, int alpha = 16, int cachesize = 128);
	virtual void* GetChar(unsigned int c);
	virtual void Release()		{delete this;}
	virtual DWORD GetLength(const char* str);
	virtual DWORD GetCharLength(const WORD c);
	virtual int GetHeight() const	{return m_Height;}
	virtual void Lock()			{m_Locked = m_nFreq + 1;}
	virtual void UnLock()		{m_Locked = 0;}
	BOOL HasOutline()			{return m_bOutline;}
	int GetCachePos(unsigned int c, int pos);
};
//------------------------------------------------------------------------