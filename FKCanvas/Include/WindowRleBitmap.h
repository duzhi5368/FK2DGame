/**
*	created:		2012-6-20   18:50
*	filename: 		WindowRleBitmap
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../IFKCanvas.h"
#include "WindowBitmap.h"
//------------------------------------------------------------------------
class WRleBitmap : public WBitmap, public IRleBitmap
{
public:
	WRleBitmap();
	virtual ~WRleBitmap();
public:
	//实现IRleBitmap
	//实现IBitmap
	//初始化函数
	virtual BOOL Create(int nWidth, int nHeight) ;																			//创建位图
	virtual BOOL Create(int nWidth, int nHeight, void* ptr) ;																//给定数据区创建位图
	virtual BOOL Create(IBitmap* parent, int x, int y, int nWidth, int nHeight);											//从父位图上创建对象
	virtual IBitmap* SubBitmap(int x, int y, int nWidth, int nHeight) ;														//创建子位图
	virtual int GetWidth() const ;																							//获得宽度
	virtual int GetHeight() const ;																							//获得高度

	//绘图相关
	virtual BOOL Clear(WPixel clr, RECT* pRc = NULL) ;																			//清屏
	virtual BOOL Draw(int x, int y, IBitmap* bmp, DWORD dwStyle = BLIT_NORMAL) ;												//把别的位图绘制到此位图
	virtual void Draw(int x, int y, const char* s, WPixel clr) ;																//打印ASCII字符
	virtual BOOL Draw(int dx, int dy, int w, int h, IBitmap* bmp, int sx, int sy, DWORD dwStyle = BLIT_NORMAL);
	virtual BOOL DrawAlpha(int dx, int dy, IAlphaBitmap* bmp, DWORD dwStyle = BLIT_COPY);										//把别的位图绘制到此位图
	virtual BOOL DrawAlpha(int dx, int dy, int w, int h, IAlphaBitmap* bmp, int sx, int sy, DWORD dwStyle = BLIT_COPY);			//增强的AlphaDraw
	virtual BOOL DrawRle(int dx, int dy, IRleBitmap* bmp, DWORD dwStyle = BLIT_COPY);											//把别的rle位图绘制到此位图
	virtual BOOL DrawRle(int dx, int dy, int w, int h, IRleBitmap* bmp, int sx, int sy, DWORD dwStyle = BLIT_COPY);				//增强的rle绘制
	virtual BOOL DrawRle(int dx, int dy, IRleBitmap* bmp, BYTE* pPal, DWORD dwStyle = BLIT_COPY);								//把别的rle位图绘制到此位图
	virtual BOOL DrawRle(int dx, int dy, int w, int h, IRleBitmap* bmp, BYTE* pPal, int sx, int sy, DWORD dwStyle = BLIT_COPY); //增强的rle绘制
	
	virtual BOOL Blit2DC(HDC hDC,int dx,int dy,int x,int y,int w,int h)  ;														//BLIT 一个矩形部分到 DC
	virtual BOOL DrawBox(RECT& rc, WPixel clr, DWORD dwStyle = BLIT_NORMAL, int alpha = 255);
	virtual BOOL DrawLine(POINT& ptFrom, POINT& ptTo, WPixel clr, DWORD dwStyle = BLIT_NORMAL, int alpha = 255);
	//其它
	virtual WPixel* operator[](int y)  const;						//返回一个扫描行指针
	virtual void* GetBmpStruct() const {return m_pBmpStruct;}		//返回数据
	virtual BOOL Load(void *bmpdata) ;								//从数据区读出位图
	virtual BOOL Save(const char *filename) const ;					//保存位图
	virtual void SetUserData(DWORD userdata) ;						//设置用户数据
	virtual DWORD GetUserData() const ;								//得到用户数据
	virtual BOOL IsValid() const;
	virtual void Release() ;

	virtual void SetData(void* pBmp){m_pBmpStruct = pBmp;}
	virtual BOOL Scroll(int x, int y);

	virtual BOOL DrawText(ITTFont*, int x, int y, const char* str, WPixel color, DWORD style=FONT_ALPHA);
	virtual BOOL DrawTextEx(ITTFont*, int x, int y, const char* str, WPixel color, WPixel outlinecolor, DWORD style=FONT_ALPHA);
	virtual BOOL IsTransparent(POINT& ptSelect);
	virtual IBitmap* SubTile(POINT ptLeftTop);
	virtual BOOL LoadEx(void* customdata);
	virtual DWORD GetSize();																			//得到图片所占内存的大小
	virtual BOOL DrawPixel(int x, int y, WPixel clr, DWORD dwStyle = BLIT_NORMAL, int alpha = 255);
	virtual BOOL DrawText(int x, int y, const char* str, WPixel color, DWORD style=FONT_ALPHA);			//输出点阵文字
	
	// 将IBitmap画向Canvas
	virtual BOOL Draw(ICanvas* pCanvas, int dx, int dy, DWORD dwStyle = BLIT_NORMAL);
	virtual BOOL Draw(ICanvas* pCanvas, int dx, int dy, int w, int h, int sx, int sy, DWORD dwStyle = BLIT_NORMAL);
	
	// IRleBitmap
	virtual int GetUseColor();							// 得到调色板使用的颜色数
	virtual BYTE* GetRleData();							// 得到rle数据
	virtual void SetRleData(BYTE* pData, int nSize);	// 设置rle数据
	virtual BYTE* GetPalette();							// 得到调色板
	virtual void SetPalette(BYTE* pPal, int nSize);		// 设置新的调色板
	virtual BOOL HaveAlphaChannel();					// 是否具有alpha通道?
	virtual BOOL IsHiColor();							// 是否是16位RLE
protected:
	void Destroy();
};
//------------------------------------------------------------------------