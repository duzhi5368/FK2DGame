/**
*	created:		2012-6-19   21:12
*	filename: 		WindowChar
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKTrace/IFKTrace.h"
#include "../IFKCanvas.h"
#include "../Include/StructCommon.h"
#include "../Include/CanvasCommon.h"
#include "../Include/WindowChar.h"
#include "../Include/WindowFont.h"
#include "../asm/asm.h"
//------------------------------------------------------------------------
BLIT_FUNC _font_func[2][2]={
	{
		(BLIT_FUNC)font16_alpha,						// 带 65 级 Alpha 的画字函数
		(BLIT_FUNC)font16_noalpha						// 不带 Alpha 的画字函数
	},
	{
		(BLIT_FUNC)font16_alpha,						// 带 65 级 Alpha 的画字函数
		(BLIT_FUNC)font16_noalpha_with_outline			// 不带 Alpha 的画字函数
	}
};
//------------------------------------------------------------------------
WChar::WChar()
{
	m_bOutline = FALSE;
}
//------------------------------------------------------------------------
WChar::~WChar()
{

}
//------------------------------------------------------------------------
BOOL WChar::Create(ITTFont* pFont, unsigned int c, WPixel color, WPixel outlinecolor)
{
	WFontData* pFontData = (WFontData*)pFont->GetChar(c);

	if(pFontData == 0)
	{
		FKTraceLn("Cannot create char!");
		return FALSE;	
	}

	XFont* font = static_cast<XFont*>(pFont);
	m_bOutline = font->HasOutline();
	m_objdata.w	= pFontData->w;
	m_objdata.h = pFontData->h;
	m_objdata.data = pFontData->data;
	m_objdata.kx = pFontData->kx;
	m_objdata.ky = pFontData->ky;
	m_objdata.width = pFontData->width;
	m_objdata.color = color;
	m_objdata.outline = outlinecolor;

	return TRUE;
}
//------------------------------------------------------------------------
int WChar::GetW() const
{
	return m_objdata.w;
}
//------------------------------------------------------------------------
int WChar::GetH()const
{
	return m_objdata.h;
}
//------------------------------------------------------------------------
int WChar::GetWidth() const
{
	return m_objdata.width;
}
//------------------------------------------------------------------------
BLIT_FUNC WChar::DrawFunc(DWORD style) const
{
	return _font_func[m_bOutline][style];
}		
//------------------------------------------------------------------------