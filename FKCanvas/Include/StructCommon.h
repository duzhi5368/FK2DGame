/**
*	created:		2012-6-19   13:05
*	filename: 		StructCommon
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../IFKCanvas.h"
//------------------------------------------------------------------------
#pragma pack (push)
#pragma pack (1)
//------------------------------------------------------------------------
struct TGAFILEHEADER{
	BYTE bIDLength;						//附加信息长度
	BYTE bPalType;						//调色板信息(不支持)
	BYTE bImageType;					//图象类型(只支持 3,10)
	WORD wPalIndex;						//调色板第一个索引值
	WORD wPalLength;					//调色板索引数
	BYTE bPalBits;						//一个调色板单位位数(15,16,24,32)
	WORD wLeft;							//图象左端坐标(基本不用)
	WORD wBottom;						//图象底端坐标(基本不用)
	WORD wWidth;						//图象宽度
	WORD wHeight;						//图象高度
	BYTE bBits;							//一个象素位数
	BYTE bAlphaBits:4;					//Alpha位数
	BYTE bReserved:1;					//保留,必须为0
	BYTE bTopDown:1;					//为1表示从上到下(只支持从下到上)
	BYTE bInterleaving:2;				//隔行(一般为0)
};
//------------------------------------------------------------------------
typedef struct __RLEHEADER				// 20 bytes
{
	WORD wFlags;						// RLE压缩数据标识(GF) (Graphic Flag? Gao Feng?)
	DWORD dwTotalSize;					// 本结构及其调色板和数据的整个大小
	DWORD dwWidth;						// RLE图片的宽
	DWORD dwHeight;						// RLE图片的高
	BYTE nUseColor;						// 本图片使用的调色板的颜色数(规定最大255种颜色,idx=255的点看作透明点)
	BYTE bHaveAlphaChannel;				// 是否有alpha通道数据
	DWORD dwDataOffset;					// 图片数据的偏移量
	__RLEHEADER(){memset(this, 0, sizeof(__RLEHEADER));wFlags='FG';}
	BOOL HavePalette(){return (nUseColor!=0) && (dwDataOffset!=sizeof(__RLEHEADER));}
	BOOL IsHiColor() {return (nUseColor==16) && (dwDataOffset==sizeof(__RLEHEADER));}
	BOOL IsValid(){
		return (wFlags=='FG' && dwTotalSize>0 && dwWidth>0 && 
			dwHeight>0 && dwDataOffset>=sizeof(__RLEHEADER));}
} RLEHEADER;
//------------------------------------------------------------------------
struct WBmpStruct
{
	int kx;					//起始坐标
	int ky;					//起始坐标
	int w;					//宽度
	int h;					//高度
	int pitch;				//行宽字节
	void* ptr;				// 第一扫描行指针
	DWORD userdata;			//用户数据
	BOOL bChild;			//是否为子位图

	WBmpStruct(){Empty();}
	WBmpStruct(const WBmpStruct&);
	~WBmpStruct();
	WBmpStruct& operator=(const WBmpStruct& bmp);
	void Empty() {bChild = FALSE; kx = ky = 0, w = h = 0, ptr = 0; userdata = 0; pitch = 0;} //清空结构
};
//------------------------------------------------------------------------
struct WAlphaBmpStruct : public WBmpStruct
{
	BYTE* channel;			//Alpha通道
	void* cmem;				//Alpha 通道内存块
	WAlphaBmpStruct(){Empty();}
	~WAlphaBmpStruct(){if(!bChild) delete []cmem; cmem = 0;}
	void Empty(){ bChild = FALSE;w = h = 0; ptr = 0, cmem = 0;}
};
//------------------------------------------------------------------------
struct WRleBmpStruct
{
	RLEHEADER rleheader;
	void* ptr;
	BYTE* pal;
	DWORD userdata;
	WRleBmpStruct(){memset(this, 0, sizeof(WRleBmpStruct));}
	void Clear()
	{
		if (ptr) {delete[] ptr;ptr=0;}
		if (pal && rleheader.HavePalette()) {delete[] pal;pal=0;}
	}
};
//------------------------------------------------------------------------
struct WFontData
{
	int w;				//宽度
	int h;				//高度
	int width;			//字宽　
	int kx;				//X 偏移
	int ky;				//Y 偏移
	int size;			//长度
	BYTE* data;			//实际数据
};
//------------------------------------------------------------------------
struct WCharStruct
{
	int kx;
	int ky;
	int w;				// 宽度
	int h;				// 高度
	BYTE *data;			// 第一扫描行指针
	WPixel color;		// 颜色
	WPixel outline;		// 轮廓的颜色
	short int width;	// 字宽(含字间距)

	WCharStruct(const WCharStruct& c){ w=c.w, h = c.h, data = c.data, color = c.color; outline = c.outline;}
	WCharStruct(){kx = ky = w = h = 0;
	data = NULL;
	color = outline = 0;
	width = 0;}
};
//------------------------------------------------------------------------
typedef DWORD (_stdcall *BLIT_FUNC)(DWORD,DWORD);
typedef unsigned long WSERR;
#define WS_OK 0
//------------------------------------------------------------------------
struct WBoxStruct
{
	int kx;
	int ky;
	int w;
	int h;
	int userdata;
	WPixel color;
};
//------------------------------------------------------------------------
struct WLineStruct
{
	int kx;
	int ky;
	int x;									// X偏移
	int y;									// Y偏移
	bool positive;
	WPixel color;							// 颜色
	int userdata;							// 用户数据
};
//------------------------------------------------------------------------
#pragma pack (pop)
//------------------------------------------------------------------------