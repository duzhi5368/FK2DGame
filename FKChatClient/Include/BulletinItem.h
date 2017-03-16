/**
*	created:		2012-6-28   2:51
*	filename: 		BulletinItem
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		公告项
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKCanvas/IFKCanvas.h"
#include <string>
using std::string;
//------------------------------------------------------------------------
// 鼠标移动后的颜色
#define COLOR_MOUSEOVER		WPixel(255,128,128)
// 鼠标点击的颜色
#define COLOR_CLICKED		WPixel(128,128,255)
//------------------------------------------------------------------------
// 公告项
class CBulletinItem  
{
public:
	CBulletinItem(void);
	virtual ~CBulletinItem(void);

public:
	void OnDraw(ICanvas* pCanvas, int x, int y, RECT& rcClip);
	void Click(void);
	void Scrolled(void);
	void MouseOver(BOOL bOver);
	BOOL IsFinished(void);
	int GetLength(void);
	void SetScrollCount(int nCount);
	void SetUrl(string strUrl);
	void SetText(string strText);
	void SetTextColor(WPixel clrText);
	void SetFont(ITTFont * pFont);

private:
	enum {STATE_NORMAL, STATE_MOUSEOVER, STATE_CLICKED};
	static const int ITEM_SPACE;	// 项间隔
	ITTFont * m_pFont;				// 画文本的字体
	WPixel m_clrText;				// 文本显示的颜色
	string m_strText;				// 公告的文本
	string m_strUrl;				// 链接
	int m_nLength;					// 公告的长度
	BOOL m_bClicked;				// 是不是被点击过
	int m_nScrollCount;				// 总需滚动次数
	int m_nScrolledCount;			// 已经滚动次数
	int m_nState;					// 状态
};
//------------------------------------------------------------------------