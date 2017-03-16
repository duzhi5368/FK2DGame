/**
*	created:		2012-6-20   19:57
*	filename: 		WindowCanvas
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../IFKCanvas.h"
#include "DirtyList.h"
#include "WindowBitmap.h"
//------------------------------------------------------------------------
class WCanvas : public ICanvas  
{
private:
	IBitmap*	m_pBmp;
	CDirtyList	m_rcDirty;
	CDirtyList	m_rcOutDirty;
	WBmpStruct	m_bmpStruct;	// 用于脏矩形处理
	WBitmap		m_bmp;			// 用于脏矩形处理，指向 m_pBmp 上某块子位图
	BOOL		m_bInEqualOut;
private:
	void SetBorder();
	void SubBmpStruct(RECT& rc);
public:
	WCanvas();
	virtual ~WCanvas();

	// 函数重载
	virtual IBitmap* SelectBitmap(IBitmap* pBitmap) ;															//选择一个IBitmap
	virtual IBitmap* GetBitmap(void) const ;

	//与Canvas的交互
	virtual BOOL Draw(int x, int y, ICanvas* pCanvas, DWORD dwStyle = BLIT_NORMAL) ;							//把别的位图绘制到此位图
	virtual BOOL Draw(int dx, int dy, int w, int h, ICanvas* bmp, int sx, int sy, DWORD dwStyle = BLIT_NORMAL); //增强的Draw

	//脏矩形与覆盖矩形管理
	virtual void AddDirtyRect(RECT& area) ;	
	virtual void AddDirtyRect(int x, int y, int w, int h){
		RECT rc;
		rc.left = x;
		rc.top = y;
		rc.right = x + w;
		rc.bottom = y + h;
		AddDirtyRect(rc);
	}

	virtual void AddInOutRect(RECT& area);
	virtual void AddInOutWholeScreen();
	virtual void AddInOutOverlay(RECT& area);
	virtual void AddOverlay(RECT& area) ;
	virtual void ResetDirtyRect(void) ;
	virtual void AddWholeScreen(void);
	virtual void Merge(void);
	virtual void* GetDirtyList();
	virtual void AddDirtyList(void* pList);
	virtual BOOL IsInDirty(RECT& rc) {return m_rcDirty.IsDirty(rc);}
	virtual int	GetDirtyCount() {return m_rcDirty.GetDirtyCount();}					// 获取当前的脏矩形数目

	//出矩形管理
	virtual void AddOutDirtyRect(RECT& area);
	virtual void AddOutDirtyRect(int x, int y, int w, int h);
	virtual void AddOutOverlay(RECT& area);
	virtual void ResetOutDirtyRect(void);
	virtual void AddOutWholeScreen(void);
	virtual void MergeOut(void);

	virtual void* GetOutDirtyList();
	virtual void AddOutDirtyList(void* pList);
	virtual BOOL IsOutDirty(RECT & rc)
	{
		if(m_bInEqualOut)
			return m_rcDirty.IsDirty(rc);
		return m_rcOutDirty.IsDirty(rc);
	}
	virtual void DrawDirtyList(ICanvas* pICanvas,WPixel color);
	virtual void DrawOutDirtyList(ICanvas *pICanvas, WPixel color);

	//实现的IBitmap接口
	//初始化函数
	virtual BOOL Create(int nWidth, int nHeight) ;																				//创建位图
	virtual BOOL Create(int nWidth, int nHeight, void* ptr) ;																	//给定数据区创建位图
	virtual BOOL Create(IBitmap* parent, int x, int y, int nWidth, int nHeight);												//从父位图上创建对象
	virtual IBitmap* SubBitmap(int x, int y, int nWidth, int nHeight) ;															//创建子位图
	virtual int GetWidth() const ;																								//获得宽度
	virtual int GetHeight() const ;																								//获得高度

	//绘图相关
	virtual BOOL Clear(WPixel clr, RECT* pRc = NULL) ;																			//清屏
	virtual BOOL Draw(int x, int y, IBitmap* bmp, DWORD dwStyle = BLIT_NORMAL) ;												//把别的位图绘制到此位图
	virtual BOOL Draw(int dx, int dy, int w, int h, IBitmap* bmp, int sx, int sy, DWORD dwStyle = BLIT_NORMAL);					//增强的Draw
	virtual BOOL DrawAlpha(int dx, int dy, IAlphaBitmap* bmp, DWORD dwStyle = BLIT_COPY);										//把别的位图绘制到此位图
	virtual BOOL DrawAlpha(int dx, int dy, int w, int h, IAlphaBitmap* bmp, int sx, int sy, DWORD dwStyle = BLIT_COPY);			//增强的AlphaDraw
	virtual BOOL DrawRle(int dx, int dy, IRleBitmap* bmp, DWORD dwStyle = BLIT_COPY);											//把别的rle位图绘制到此位图
	virtual BOOL DrawRle(int dx, int dy, int w, int h, IRleBitmap* bmp, int sx, int sy, DWORD dwStyle = BLIT_COPY);				//增强的rle绘制
	virtual BOOL DrawRle(int dx, int dy, IRleBitmap* bmp, BYTE* pPal, DWORD dwStyle = BLIT_COPY);								//把别的rle位图绘制到此位图
	virtual BOOL DrawRle(int dx, int dy, int w, int h, IRleBitmap* bmp, BYTE* pPal, int sx, int sy, DWORD dwStyle = BLIT_COPY); //增强的rle绘制
	virtual void Draw(int x, int y, const char* s, WPixel clr) ;																//打印ASCII字符
	virtual BOOL Update2DC(HDC hDC,int dx,int dy,int x,int y,int w,int h) ;														//BLIT 一个矩形部分到 DC
	virtual BOOL Blit2DC(HDC hDC,int dx,int dy,int x,int y,int w,int h) ;
	virtual BOOL DrawBox(RECT& rc, WPixel clr, DWORD dwStyle = BLIT_NORMAL, int alpha = 255);
	virtual BOOL DrawLine(POINT& ptFrom, POINT& ptTo, WPixel clr, DWORD dwStyle = BLIT_NORMAL, int alpha = 255);

	//其它
	virtual WPixel* operator[](int y) const;																					//返回一个扫描行指针
	virtual void* GetBmpStruct() const;																							//返回数据
	virtual BOOL Load(void *bmpdata) ;																							//从数据区读出位图
	virtual BOOL Save(const char *filename) const ;																				//保存位图
	virtual void SetUserData(DWORD userdata) ;																					//设置用户数据
	virtual DWORD GetUserData() const ;																							//得到用户数据
	virtual BOOL IsValid() const;
	virtual void Release() ;
	virtual void SetData(void* pBmp);
	virtual BOOL Scroll(int x, int y);
	virtual BOOL IsInEqualOut() {return m_bInEqualOut;}
	virtual void SetInEqualOut(BOOL b) {m_bInEqualOut = b;}

	virtual BOOL DrawText(ITTFont*, int x, int y, const char* str, WPixel color, DWORD style=FONT_ALPHA);
	virtual BOOL DrawTextEx(ITTFont*, int x, int y, const char* str, WPixel color, WPixel outlinecolor, DWORD style=FONT_ALPHA);
	virtual BOOL IsTransparent(POINT& ptSelect);
	virtual IBitmap* SubTile(POINT ptLeftTop);
	virtual BOOL LoadEx(void* customdata){return TRUE;}
	virtual DWORD GetSize();																									//得到图片所占内存的大小
	virtual BOOL DrawPixel(int x, int y, WPixel clr, DWORD dwStyle = BLIT_NORMAL, int alpha = 255);
	virtual BOOL DrawText(int x, int y, const char* str, WPixel color, DWORD style=FONT_ALPHA);									//输出点阵文字
	virtual BOOL DrawClipText(ITTFont* pfont, int x, int y, const char* str, WPixel color, RECT rcClip, DWORD style=FONT_ALPHA);//带裁剪框的文字输出.输出TTF文字	
	virtual BOOL Draw(ICanvas* pCanvas, int dx, int dy, DWORD dwStyle = BLIT_NORMAL);											//将IBitmap画向Canvas
	virtual BOOL Draw(ICanvas* pCanvas, int dx, int dy, int w, int h, int sx, int sy, DWORD dwStyle = BLIT_NORMAL);
};
//------------------------------------------------------------------------
