/**
*	created:		2012-6-19   21:11
*	filename: 		WindowChar
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
extern BLIT_FUNC _font_func[2][2];
//------------------------------------------------------------------------
class WChar  
{
private:
	WCharStruct m_objdata;
	int m_bOutline;

public:
	WChar();
	virtual ~WChar();
	BOOL Create(ITTFont* pFont, unsigned int c, WPixel color, WPixel outlinecolor=0);	// 轮廓颜色默认为黑色

	int GetW() const;																	// 取宽度
	int GetH() const;																	// 取高度
	int GetWidth() const;																// 取字宽
	BLIT_FUNC DrawFunc(DWORD style) const;												//获得draw函数指针
	WCharStruct* GetData(){return &m_objdata;}

};
//------------------------------------------------------------------------