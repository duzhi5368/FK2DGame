/**
*	created:		2012-6-19   12:31
*	filename: 		IFKCanvas
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <windows.h>
//------------------------------------------------------------------------
struct ITrace;
//------------------------------------------------------------------------
///	Blit 样式
/////////////////////////
#define BLIT_NORMAL		0			//正常的BLIT，不考虑透明色与Alpha
#define BLIT_COPY		0			//同上
#define BLIT_ADDITIVE	0x1			//色彩迭加
#define BLIT_SUBTRACT	0x2			//色彩衰减
#define BLIT_TRANS		0x3			//alpha BLIT
#define BLIT_MASK		0x4			//透明色BLIT
#define BLIT_BACK		0x5			//反向COPY 即目标到源
#define BLIT_XOR		0x4			

#define FONT_ALPHA		0
#define FONT_NOALPHA	1
//------------------------------------------------------------------------
#pragma pack (push)
#pragma pack (1)

//16位点结构
struct WPixel 
{
	union {
		WORD color;								// 16bit 高彩
		struct {
			WORD blue:5;						// 5bit 兰色
			WORD green:6;						// 6bit 绿色
			WORD red:5;							// 5bit 红色
		};
	};
	WPixel() {}
	WPixel(DWORD c) {color=(WORD)((c>>8&0xf800)|(c>>5&0x7e0)|(c>>3&0x1f));}	//从DWORD转换
	WPixel(int r,int g,int b) : red(r>>3),green(g>>2),blue(b>>3) {}
	operator DWORD() const {return (color<<5&0x7fc00)|(color<<8&0xf80000)|((color<<3|(color&7))&0x3ff);}
};

#pragma pack (pop)
//------------------------------------------------------------------------
struct ITTFont;
struct IAlphaBitmap;
struct IRleBitmap;
struct ICanvas;
//------------------------------------------------------------------------
struct IBitmap
{
	virtual BOOL Create(int nWidth, int nHeight) = NULL;																				//创建位图
	virtual BOOL Create(int nWidth, int nHeight, void* ptr) = NULL;																		//给定数据区创建位图
	virtual BOOL Create(IBitmap* parent, int x, int y, int nWidth, int nHeight) = NULL; 												//从父位图上创建对象
	virtual IBitmap* SubBitmap(int x, int y, int nWidth, int nHeight) = NULL;															//创建子位图
	virtual int GetWidth() const = NULL;																								//获得宽度
	virtual int GetHeight() const = NULL;																								//获得高度
	virtual BOOL Clear(WPixel clr, RECT* pRc = NULL) = NULL;																			//清屏
	virtual BOOL Draw(int dx, int dy, IBitmap* bmp, DWORD dwStyle = BLIT_NORMAL) = NULL;												//把别的位图绘制到此位图
	virtual BOOL Draw(int dx, int dy, int w, int h, IBitmap* bmp, int sx, int sy, DWORD dwStyle = BLIT_NORMAL) = NULL;					//增强的Draw
	virtual BOOL DrawAlpha(int dx, int dy, IAlphaBitmap* bmp, DWORD dwStyle = BLIT_COPY) = NULL;										//把别的位图绘制到此位图
	virtual BOOL DrawAlpha(int dx, int dy, int w, int h, IAlphaBitmap* bmp, int sx, int sy, DWORD dwStyle = BLIT_COPY) = NULL;			//增强的AlphaDraw
	virtual BOOL DrawRle(int dx, int dy, IRleBitmap* bmp, DWORD dwStyle = BLIT_COPY) = NULL;											//把别的rle位图绘制到此位图
	virtual BOOL DrawRle(int dx, int dy, int w, int h, IRleBitmap* bmp, int sx, int sy, DWORD dwStyle = BLIT_COPY) = NULL;				//增强的rle绘制
	virtual void Draw(int x, int y, const char* s, WPixel clr) = NULL;																	//打印ASCII字符
	virtual BOOL DrawText(ITTFont*,int x, int y, const char* str, WPixel color, DWORD style=FONT_ALPHA) = NULL;							//输出TTF文字
	virtual BOOL DrawBox(RECT& rc, WPixel clr, DWORD dwStyle = BLIT_NORMAL, int alpha = 255) = NULL;									//绘制一个矩形盒
	virtual BOOL DrawLine(POINT& ptFrom, POINT& ptTo, WPixel clr, DWORD dwStyle = BLIT_NORMAL, int alpha = 255) = NULL;					//绘制一条线
	virtual BOOL Blit2DC(HDC hDC,int dx,int dy,int x,int y,int w,int h)  = NULL;														//BLIT 一个矩形部分到 DC
	virtual WPixel* operator[](int y)  const = NULL;																					//返回一个扫描行指针
	virtual void* GetBmpStruct() const = NULL;																							//返回BMP数据
	virtual BOOL Load(void *bmpdata) = NULL;																							//从数据区读出位图
	virtual BOOL Save(const char *filename) const = NULL;																				//保存位图
	virtual void SetUserData(DWORD userdata) = NULL;																					//设置用户数据
	virtual DWORD GetUserData() const = NULL;																							//得到用户数据
	virtual BOOL IsValid() const = NULL;																								//是否为有效
	virtual void Release() = NULL;																										//释放当前位图
	virtual void SetData(void*p) = NULL;																								//更换内部的数据
	virtual BOOL Scroll(int x, int y) = NULL;																							//整个Bitmap滚动
	virtual BOOL IsTransparent(POINT& ptSelect) = NULL;																					//测试某个点是否透明
	virtual IBitmap* SubTile(POINT ptLeftTop) = NULL;																					//从本位图上创建一个独立的Tile子位图(即菱形格位图, ptLeftTop为菱形格的左顶点)
	virtual BOOL LoadEx(void* customdata) = NULL;																						//加载位图数据（注意：仅支持带Alpha通道的rle格式）
	virtual DWORD GetSize() = NULL;																										//得到图片所占内存的大小
	virtual BOOL DrawPixel(int x, int y, WPixel clr, DWORD dwStyle = BLIT_NORMAL, int alpha = 255) = NULL;								//绘制一个像素点 
	virtual BOOL DrawText(int x, int y, const char* str, WPixel color, DWORD style=FONT_ALPHA) = NULL;									//输出点阵文字
	virtual BOOL DrawRle(int dx, int dy, IRleBitmap* bmp, BYTE* pPal, DWORD dwStyle = BLIT_COPY) = NULL;								//把别的rle位图绘制到此位图
	virtual BOOL DrawRle(int dx, int dy, int w, int h, IRleBitmap* bmp, BYTE* pPal, int sx, int sy, DWORD dwStyle = BLIT_COPY) = NULL;	//增强的rle绘制
	virtual BOOL Draw(ICanvas* pCanvas, int dx, int dy, DWORD dwStyle = BLIT_NORMAL) = NULL;											//将IBitmap画向Canvas
	virtual BOOL Draw(ICanvas* pCanvas, int dx, int dy, int w, int h, int sx, int sy, DWORD dwStyle = BLIT_NORMAL) = NULL;				//将IBitmap画向Canvas
	virtual BOOL DrawTextEx(ITTFont*,int x, int y, const char* str, WPixel color, WPixel outlinecolor, DWORD style=FONT_ALPHA) = NULL;	//输出TTF文字
};
//------------------------------------------------------------------------
struct IAlphaBitmap : public IBitmap
{
	virtual BOOL LoadTGA(const void *tgadata) = NULL;							//加载TGA图片
	virtual BOOL SaveTGA(const char *filename) const = NULL;					//存为32位TGA
	virtual BYTE* ChannelLine(int y) const = NULL;								//得到通道指针
	virtual void ClearChannel(int alpha) = NULL;								//清除 Alpha Channel
	virtual BOOL Combine(IAlphaBitmap* bmp) = NULL;								//合并二个AlphaBitmap
	virtual BOOL Load2Bmp(const void* bmpdata, const void* alphadata) = NULL;	//从二张Bitmap中生成AlphaBitmap，后一张是通道图，只能为256色
	virtual BOOL Load2BmpEx(IBitmap* pBmp, int nBmpX, int nBmpY,
		IBitmap* pAlpha, int nAlphaX, int nAlphaY,
		int nWidth, int nHeight) = NULL;										//从两个IBitmap中生成AlphaBitmap，后一张是通道图，只能为256色	
};
//------------------------------------------------------------------------
struct IRleBitmap : public IBitmap
{
	virtual int GetUseColor() = 0;						// 得到调色板使用的颜色数
	virtual BYTE* GetRleData() = 0;						// 得到rle数据
	virtual void SetRleData(BYTE* pData, int nSize) = 0;// 设置rle数据
	virtual BYTE* GetPalette() = 0;						// 得到调色板
	virtual void SetPalette(BYTE* pPal, int nSize) = 0;	// 设置新的调色板
	virtual BOOL HaveAlphaChannel() = 0;				// 是否具有alpha通道?
	virtual BOOL IsHiColor() = 0;						// 是否是16位RLE
};
//------------------------------------------------------------------------
struct ICanvas : public IBitmap
{
	//与IBitmap的交互
	virtual IBitmap* SelectBitmap(IBitmap* pBitmap) = NULL;																//选择一个IBitmap
	virtual IBitmap* GetBitmap(void) const = NULL;

	//与Canvas的交互
	virtual BOOL Draw(int x, int y, ICanvas* pCanvas, DWORD dwStyle = BLIT_NORMAL) = NULL;								//把别的位图绘制到此位图
	virtual BOOL Draw(int x, int y, IBitmap* bmp, DWORD dwStyle = BLIT_NORMAL) = NULL;									//把别的位图绘制到此位图
	virtual BOOL Draw(int dx, int dy, int w, int h, IBitmap* bmp, int sx, int sy, DWORD dwStyle = BLIT_NORMAL) = NULL;	//增强的Draw
	virtual BOOL Draw(int dx, int dy, int w, int h, ICanvas* bmp, int sx, int sy, DWORD dwStyle = BLIT_NORMAL) = NULL;	//增强的Draw
	virtual BOOL Update2DC(HDC hDC,int dx,int dy,int x,int y,int w,int h) = NULL;										//BLIT 一个矩形部分到 DC
	virtual void Draw(int x, int y, const char* s, WPixel clr) = NULL;													//打印ASCII字符

	//脏矩形与覆盖矩形管理
	virtual void AddDirtyRect(RECT& area) = NULL;																		//加入脏矩形管理
	virtual void AddDirtyRect(int x, int y, int w, int h) = NULL;
	virtual void AddInOutRect(RECT& area) = NULL;
	virtual void AddInOutWholeScreen() = NULL;
	virtual void AddInOutOverlay(RECT& area) = NULL;
	virtual void AddOverlay(RECT& area) = NULL;
	virtual void ResetDirtyRect(void) = NULL;																			//把所有脏矩形清0
	virtual void AddWholeScreen(void) = NULL;
	virtual void Merge(void) = NULL;
	virtual void* GetDirtyList() = NULL;
	virtual void AddDirtyList(void* pList) = NULL;
	virtual BOOL IsInDirty(RECT& rc) = NULL;
	//出脏矩形管理
	virtual void AddOutDirtyRect(RECT& area) = NULL;
	virtual void AddOutDirtyRect(int x, int y, int w, int h) = NULL;
	virtual void AddOutOverlay(RECT& area) = NULL;
	virtual void ResetOutDirtyRect(void) = NULL;																		//把所有出脏矩形清 0
	virtual void AddOutWholeScreen(void) = NULL;
	virtual void MergeOut(void) = NULL;
	virtual void* GetOutDirtyList() = NULL;
	virtual void AddOutDirtyList(void* pList) = NULL;
	virtual BOOL IsInEqualOut() = NULL;																					//入脏矩形是否与出脏矩形相等
	virtual void SetInEqualOut(BOOL) = NULL;	
	virtual BOOL IsOutDirty(RECT& rc) = NULL;	

	virtual void DrawDirtyList(ICanvas* pICanvas,WPixel color) = NULL;
	virtual void DrawOutDirtyList(ICanvas *pICanvas, WPixel color) = NULL;

	// 带裁剪框的文字输出
	virtual BOOL DrawClipText(ITTFont* pfont, int x, int y, const char* str, WPixel color, RECT rcClip, DWORD style=FONT_ALPHA) = NULL;	//输出TTF文字
	// 获取当前的脏矩形数目
	virtual int	GetDirtyCount() = NULL;
};
//------------------------------------------------------------------------
struct ITTFont
{
	virtual BOOL Create(HFONT font, int alpha = 16, int cachesize = 128) = NULL;
	virtual void* GetChar(unsigned int c) = NULL;												//获取一个字的数据
	virtual int GetHeight() const = NULL;														//获取字体高度
	virtual void Lock() = NULL;																	//Lock Cache Buffer (不可删减)
	virtual void UnLock() = NULL;																//Unlock Cache Buffer
	virtual DWORD GetLength(const char* str) = NULL;											//得到字符串的长度
	virtual DWORD GetCharLength(const WORD c) = NULL;											//得到一个字的长度
	virtual void Release() = NULL;
};
//------------------------------------------------------------------------
class ICanvasHelper
{
public:
	typedef BOOL (*ProcCreateIBitmap)(DWORD dwVersion, IBitmap** ppIBitmap, ITrace* pTrace );
	typedef BOOL (*ProcCreateICanvas)(DWORD dwVersion, ICanvas** ppICanvas, ITrace* pTrace );
	typedef BOOL (*ProcCreateITTFont)(DWORD dwVersion, ITTFont** ppITTFont, ITrace* pTrace );
	typedef BOOL (*ProcCreateIAlphaBitmap)(DWORD dwVersion, IAlphaBitmap** ppIBitmap, ITrace* pTrace );
	typedef BOOL (*ProcCreateIRleBitmap)(DWORD dwVersion, IRleBitmap** ppIBitmap, ITrace* pTrace );
public:
	ICanvasHelper() : m_hdll(0){}
	~ICanvasHelper()
	{
		if(m_hdll)
			FreeLibrary(m_hdll);
	}

	void LoadDLL()
	{
#ifdef _DEBUG
		m_hdll = LoadLibrary("FKCanvas_D.dll");
#else
		m_hdll = LoadLibrary("FKCanvas.dll");
#endif

		if(m_hdll == NULL)
			throw "无法加载 FKCanvas.dll";
	}

	BOOL CreateIBitmap(IBitmap** ppIBitmap, ITrace* pTrace = NULL)
	{
		try
		{
			if(m_hdll == NULL)
			{
				LoadDLL();
			}

			ProcCreateIBitmap proc;
			proc = (ProcCreateIBitmap)GetProcAddress(m_hdll, "CreateIBitmap");

			if(proc == NULL)
				throw "无法获取函数地址 CreateIBitmap!";

			return proc(1, ppIBitmap, pTrace);
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			return FALSE;
		}

		catch(...)
		{
			return FALSE;
		}
	}

	BOOL CreateIAlphaBitmap(IAlphaBitmap** ppIBitmap, ITrace* pTrace = NULL)
	{
		try
		{
			if(m_hdll == NULL)
			{
				LoadDLL();
			}

			ProcCreateIAlphaBitmap proc;
			proc = (ProcCreateIAlphaBitmap)GetProcAddress(m_hdll, "CreateIAlphaBitmap");

			if(proc == NULL)
				throw "无法获取函数地址 CreateIBitmap!";

			return proc(1, ppIBitmap, pTrace);
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			return FALSE;
		}

		catch(...)
		{
			return FALSE;
		}
	}

	BOOL CreateIRleBitmap(IRleBitmap** ppIBitmap, ITrace* pTrace = NULL)
	{
		try
		{
			if(m_hdll == NULL)
			{
				LoadDLL();
			}

			ProcCreateIRleBitmap proc;
			proc = (ProcCreateIRleBitmap)GetProcAddress(m_hdll, "CreateIRleBitmap");

			if(proc == NULL)
				throw "无法获取函数地址 CreateIRleBitmap!";

			return proc(1, ppIBitmap, pTrace);
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			return FALSE;
		}

		catch(...)
		{
			return FALSE;
		}
	}
	BOOL CreateICanvas(ICanvas** ppICanvas, ITrace* pTrace = NULL)
	{
		try
		{
			if(m_hdll == NULL)
			{
				LoadDLL();
			}

			ProcCreateICanvas proc;
			proc = (ProcCreateICanvas)GetProcAddress(m_hdll, "CreateICanvas");

			if(proc == NULL)
				throw "无法获取函数地址 CreateICanvas!";

			return proc(1, ppICanvas, pTrace);
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			return FALSE;
		}

		catch(...)
		{
			return FALSE;
		}
	}

	BOOL CreateITTFont(ITTFont** ppITTFont, ITrace* pTrace = NULL)
	{
		try
		{
			if(m_hdll == NULL)
			{
				LoadDLL();
			}

			ProcCreateITTFont proc;
			proc = (ProcCreateITTFont)GetProcAddress(m_hdll, "CreateITTFont");

			if(proc == NULL)
				throw "无法获取函数地址 CreateITTFont!";

			return proc(1, ppITTFont, pTrace);
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			return FALSE;
		}

		catch(...)
		{
			return FALSE;
		}
	}

private:
	HINSTANCE m_hdll;
};
//------------------------------------------------------------------------