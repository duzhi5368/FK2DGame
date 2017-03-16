/**
*	created:		2012-6-20   19:20
*	filename: 		WindowRleBitmap
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include <stdio.h>
#include "../Include/WindowRleBitmap.h"
//------------------------------------------------------------------------
WRleBitmap::WRleBitmap()
{

}
//------------------------------------------------------------------------
WRleBitmap::~WRleBitmap()
{
	Destroy();
}
//------------------------------------------------------------------------
BOOL WRleBitmap::Create(int nWidth, int nHeight)
{
	FKTraceLn("WRleBitmap::Create is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
void WRleBitmap::Destroy()
{
	WRleBmpStruct* pdata = (WRleBmpStruct*)m_pBmpStruct;
	if (pdata)
	{
		pdata->Clear();
		SAFE_DELETE(pdata);
		SetData(NULL);
	}
}
//------------------------------------------------------------------------
BOOL WRleBitmap::Create(IBitmap* parent, int x, int y, int nWidth, int nHeight)
{
	FKTraceLn("WRleBitmap::Create is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::Create(int nWidth, int nHeight, void* ptr)
{
	return Load(ptr);
}
//------------------------------------------------------------------------
void WRleBitmap::Draw(int x, int y,const char* s, WPixel clr)
{
	FKTraceLn("WRleBitmap::Draw is not implemented!");
}
//------------------------------------------------------------------------
BOOL WRleBitmap::Draw(int dx, int dy, int w, int h, IBitmap* bmp, int sx, int sy, DWORD dwStyle /* = BLIT_NORMAL */)
{
	FKTraceLn("WRleBitmap::Draw is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::Draw(int x, int y, IBitmap* bmp, DWORD dwStyle)
{
	FKTraceLn("WRleBitmap::Draw is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::DrawAlpha(int dx, int dy, IAlphaBitmap* bmp, DWORD dwStyle )//把别的位图绘制到此位图
{
	FKTraceLn("WRleBitmap::DrawAlpha is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::DrawAlpha(int dx, int dy, int w, int h, IAlphaBitmap* bmp, int sx, int sy, DWORD dwStyle) //增强的AlphaDraw
{
	FKTraceLn("WRleBitmap::DrawAlpha is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::DrawRle(int dx, int dy, IRleBitmap* bmp, DWORD dwStyle)
{
	FKTraceLn("WRleBitmap::DrawRle is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::DrawRle(int dx, int dy, int w, int h, IRleBitmap* bmp, int sx, int sy, DWORD dwStyle)
{
	FKTraceLn("WRleBitmap::DrawRle is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::DrawRle(int dx, int dy, IRleBitmap* bmp, BYTE* pPal, DWORD dwStyle)
{
	FKTraceLn("WRleBitmap::DrawRle is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::DrawRle(int dx, int dy, int w, int h, IRleBitmap* bmp, BYTE* pPal, int sx, int sy, DWORD dwStyle)
{
	FKTraceLn("WRleBitmap::DrawRle is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::DrawBox(RECT& rc, WPixel clr, DWORD dwStyle /* = BLIT_NORMAL */, int alpha /* = 255 */)
{
	FKTraceLn("WRleBitmap::DrawBox is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::DrawLine(POINT& ptFrom, POINT& ptTo, WPixel clr, DWORD dwStyle /* = BLIT_NORMAL */, int alpha /* = 255 */)
{
	FKTraceLn("WRleBitmap::DrawLine is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::DrawText(ITTFont* pFont, int x, int y, const char* str, WPixel color, DWORD style/* =FONT_ALPHA */)
{
	FKTraceLn("WRleBitmap::DrawText is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::DrawTextEx(ITTFont* pFont, int x, int y, const char* str, WPixel color, WPixel outlinecolor, DWORD style/* =FONT_ALPHA */)
{
	FKTraceLn("WRleBitmap::DrawText is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::IsTransparent(POINT& ptSelect)
{	
	if(!IsValid())
	{
		FKTraceLn("RleBitmap is invalid!");
		return TRUE; // 图片为空，可看作透明，呵呵
	}
	
	WRleBmpStruct* bmp = (WRleBmpStruct*)m_pBmpStruct;
	if (ptSelect.x < 0 || ptSelect.y < 0 ||
		ptSelect.x > (int)bmp->rleheader.dwWidth || 
		ptSelect.y > (int)bmp->rleheader.dwHeight) // 非法点
		return TRUE;
	
	BYTE* bits = (BYTE*)bmp->ptr;
	int rep_count,j,nPos=0;
	if (HaveAlphaChannel()) // 有alpha通道
	{
		// 先跳到指定的行
		for (j=0; j<ptSelect.y; j++)
			nPos += *(unsigned short*)&bits[nPos];

		nPos += 2;
		for(j = 0; j < ptSelect.x;)
		{
			if (bits[nPos] < 0x40) // 透明区
			{
				j += bits[nPos++] + 1;
				if (j >= ptSelect.x)
					return TRUE;
			}
			else if (bits[nPos] < 0x80) // 影子区
			{
				rep_count = bits[nPos++] -64 + 1;
				nPos++; // 跳过alpha级别值
				j += rep_count;
				if (j >= ptSelect.x)
					return FALSE;
			}
			else if (bits[nPos] < 0xc0) // 8级alpha区
			{
				rep_count = bits[nPos++] -128 + 1;
				nPos += rep_count;
				j += rep_count;
				if (j >= ptSelect.x)
					return FALSE;
			}
			else // 不规则序列
			{
				rep_count = bits[nPos++] - 192 + 1;
				nPos += (rep_count<<1);
				j += rep_count;
				if (j >= ptSelect.x)
					return FALSE;
			}
		}
		// ptSelect.x == 0 ?
		if (bits[nPos] < 0x40)
			return TRUE;
	}
	else // 无alpha通道数据
	{
		// 先跳到指定的行
		for (j=0; j<ptSelect.y; j++)
			nPos += *(unsigned short*)&bits[nPos];

		nPos +=2;
		for(j = 0; j < ptSelect.x;)
		{
			if (bits[nPos] < 0x80) // 透明色
			{
				rep_count = bits[nPos++] + 1;
				j += rep_count;
				if (j >= ptSelect.x)
					return TRUE;
			}
			else if (bits[nPos] < 0xc0) // 其他重复的颜色
			{
				rep_count = bits[nPos++] - 128 + 1;
				nPos ++; // 跳过颜色值
				j += rep_count;
				if (j >= ptSelect.x)
					return FALSE;
			}
			else // 不规则序列
			{
				rep_count = bits[nPos++] - 192 + 1;
				nPos += rep_count;
				j += rep_count;
				if (j >= ptSelect.x)
					return FALSE;
			}
		}
		// ptSelect.x == 0 ?
		if (bits[nPos] < 0x80)
			return TRUE;
	}

	return FALSE;
}
//------------------------------------------------------------------------
int WRleBitmap::GetHeight() const
{
	WRleBmpStruct* pdata = (WRleBmpStruct*)m_pBmpStruct;
	if (pdata)
		return pdata->rleheader.dwHeight;
	return 0;
}
//------------------------------------------------------------------------
DWORD WRleBitmap::GetUserData() const
{
	WRleBmpStruct* pdata = (WRleBmpStruct*)m_pBmpStruct;
	return pdata->userdata;
}
//------------------------------------------------------------------------
int WRleBitmap::GetWidth()const 
{
	WRleBmpStruct* pdata = (WRleBmpStruct*)m_pBmpStruct;
	if (pdata)
		return pdata->rleheader.dwWidth;
	return 0;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::IsValid() const
{
	WRleBmpStruct* pdata = (WRleBmpStruct*)m_pBmpStruct;
	return (pdata && pdata->ptr);
}
//------------------------------------------------------------------------
BOOL WRleBitmap::Load(void *bmpdata)
{
	if(bmpdata == NULL)
	{
		FKTraceLn("Invalid rle bitmap data!");
		return FALSE;
	}
	
	RLEHEADER* rlehead = (RLEHEADER*)bmpdata;
	if (rlehead->wFlags != 'FG')
	{
		FKTraceLn("WRleBitmap::Load invalid rle head");
		return FALSE;
	}
	
	//检查以前是否有数据
	WRleBmpStruct* pdata = (WRleBmpStruct*)GetBmpStruct();
	if(pdata)	
		pdata->Clear();
	else	
		pdata = new WRleBmpStruct;
	
	memcpy(&pdata->rleheader, rlehead, sizeof(RLEHEADER));
	int nDataLen, nPalLen;
	nPalLen = pdata->rleheader.nUseColor<<1;
	nDataLen = pdata->rleheader.dwTotalSize - pdata->rleheader.dwDataOffset;
	
	if (rlehead->HavePalette())
	{
		pdata->pal = new BYTE[nPalLen];
		memcpy(pdata->pal, (BYTE*)rlehead + sizeof(RLEHEADER), nPalLen);
	}
	if (nDataLen)
	{
		pdata->ptr = new BYTE[nDataLen+7];
		memcpy(pdata->ptr, (BYTE*)rlehead + rlehead->dwDataOffset, nDataLen);
	}
	
	SetData(pdata);
	
	return TRUE;
}
//------------------------------------------------------------------------
WPixel* WRleBitmap::operator[](int y)const
{
	FKTraceLn("WRleBitmap::operator[] is not implemented!");
	return NULL;
}
//------------------------------------------------------------------------
void WRleBitmap::Release()
{
	delete this;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::Save(const char *filename) const
{
	if(!IsValid())
	{
		FKTraceLn("WRleBitmap::Save: bitmap is invalid!");
		return FALSE;
	}

	WRleBmpStruct* pdata = (WRleBmpStruct*)GetBmpStruct();
	int nDataLen, nPalLen;
	nPalLen = pdata->rleheader.nUseColor<<1;
	nDataLen = pdata->rleheader.dwTotalSize - pdata->rleheader.dwDataOffset;

	FILE* f;
	if((f = fopen(filename, "wb")) == NULL)
	{
		FKTraceLn("WRleBitmap::Save: %s file cannot open!");
		return FALSE;
	}
	fwrite(&pdata->rleheader, 1, sizeof(RLEHEADER), f);
	if (pdata->rleheader.HavePalette())
		fwrite(pdata->pal, 1, nPalLen, f);
	if (nDataLen)
		fwrite(pdata->ptr, 1, nDataLen, f);
	fclose(f);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::Scroll(int x, int y)
{
	FKTraceLn("WRleBitmap::Scroll is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
void WRleBitmap::SetUserData(DWORD userdata)
{
	if(!IsValid())
	{
		FKTraceLn("SetUserData on invalid bitmap!");
		return;
	}
	
	((WRleBmpStruct*)m_pBmpStruct)->userdata = userdata;
}
//------------------------------------------------------------------------
IBitmap* WRleBitmap::SubBitmap(int x, int y, int w, int h)
{
	FKTraceLn("WRleBitmap::SubBitmap is not implemented!");
	return NULL;
}
//------------------------------------------------------------------------
IBitmap* WRleBitmap::SubTile(POINT ptLeftTop)
{
	FKTraceLn("WRleBitmap::SubTile is not implemented!");
	return NULL;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::Clear(WPixel clr, RECT* pRc /* = NULL */)
{
	FKTraceLn("WRleBitmap::Clear is not implemented!");
	return NULL;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::Blit2DC(HDC hDC,int dx,int dy,int x,int y,int w,int h)
{
	FKTraceLn("WRleBitmap::Blit2DC is not implemented!");
	return NULL;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::LoadEx(void* customdata)
{
	FKTraceLn("WRleBitmap::LoadEx is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
// 得到图片所占内存的大小
DWORD WRleBitmap::GetSize()
{
	WRleBmpStruct* pData = (WRleBmpStruct*)m_pBmpStruct;
	if (pData)
		return pData->rleheader.dwTotalSize;
	return 0;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::DrawPixel(int x, int y, WPixel clr, DWORD dwStyle, int alpha)
{
	FKTraceLn("WRleBitmap::DrawPixel is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
BOOL WRleBitmap::DrawText(int x, int y, const char* str, WPixel color, DWORD style/*=FONT_ALPHA*/)
{
	FKTraceLn("WRleBitmap::DrawText is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
// 将IBitmap画向Canvas
BOOL WRleBitmap::Draw(ICanvas* pCanvas, int dx, int dy, DWORD dwStyle)
{
	return pCanvas->DrawRle(dx, dy, static_cast<IRleBitmap*>(this), dwStyle);
}
//------------------------------------------------------------------------
BOOL WRleBitmap::Draw(ICanvas* pCanvas, int dx, int dy, int w, int h, int sx, int sy, DWORD dwStyle)
{
	return pCanvas->DrawRle(dx, dy, w, h, static_cast<IRleBitmap*>(this), sx, sy, dwStyle);
}
//------------------------------------------------------------------------
// IRleBitmap
// 得到调色板使用的颜色数
int WRleBitmap::GetUseColor() 
{
	if (!IsValid() || IsHiColor()) return 0;
	return ((WRleBmpStruct*)m_pBmpStruct)->rleheader.nUseColor;
}
//------------------------------------------------------------------------
// 得到rle数据
BYTE* WRleBitmap::GetRleData()
{
	if (!IsValid()) return NULL;
	WRleBmpStruct* pdata = (WRleBmpStruct*)m_pBmpStruct;
	return (BYTE*)pdata->ptr;
}
//------------------------------------------------------------------------
// 设置rle数据
void WRleBitmap::SetRleData(BYTE* pData, int nSize)
{
	if (!IsValid()) return;
	WRleBmpStruct* pdata = (WRleBmpStruct*)m_pBmpStruct;
	pdata->ptr = pData;
}
//------------------------------------------------------------------------
// 得到调色板
BYTE* WRleBitmap::GetPalette()
{
	if (!IsValid()) return NULL;
	WRleBmpStruct* pdata = (WRleBmpStruct*)m_pBmpStruct;
	return pdata->pal;
}
//------------------------------------------------------------------------
// 设置新的调色板
void WRleBitmap::SetPalette(BYTE* pPal, int nSize)
{
	if (!IsValid() || IsHiColor()) return;
	WRleBmpStruct* pdata = (WRleBmpStruct*)m_pBmpStruct;
	if (pdata->rleheader.HavePalette())
	{
		if ((pdata->rleheader.nUseColor<<1) != nSize)
		{
			FKTraceLn("WRleBitmap::SetPalette(), size error!");
			return;
		}
		memcpy(pdata->pal, pPal, nSize);	
	}
	else
	{
		pdata->pal = pPal;
	}
}
//------------------------------------------------------------------------
// 是否具有alpha通道?
BOOL WRleBitmap::HaveAlphaChannel()
{
	return (m_pBmpStruct && ((WRleBmpStruct*)m_pBmpStruct)->rleheader.bHaveAlphaChannel);
}
//------------------------------------------------------------------------
// 是否是16位RLE
BOOL WRleBitmap::IsHiColor()
{
	return (m_pBmpStruct && ((WRleBmpStruct*)m_pBmpStruct)->rleheader.IsHiColor());
}
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) BOOL CreateIRleBitmap(DWORD dwVersion, IRleBitmap** ppIBitmap, ITrace* pTrace)
{
#ifdef _DEBUG
	if(pTrace)
		g_DefaultTrace.Create(pTrace);
#endif

	WRleBitmap* pBitmap = new WRleBitmap;
	
	if(pBitmap == NULL)
		return FALSE;

	*ppIBitmap = static_cast<IRleBitmap*>(pBitmap);

	return TRUE;
}
//------------------------------------------------------------------------
/*
void mmx_blit_32(char* pSrc, char* pOvr)
{
	__asm
	{ 
		pxor mm2,mm2 //把MM2清0
		mov edx,dword ptr [pSrc] //取32bit像素地址到edx
		movd mm0,dword ptr [edx] //把Source像素取到mm0低32bit
		mov edx,dword ptr [pOvr]
		movd mm1,dword ptr [edx] //把32 bit Overlay像素取到mm1低32bit
		punpcklbw mm0,mm2 //Source:8 bit到16 bit以容纳结果,32bit expand to 64 bit
		punpcklbw mm1,mm2 //Overlay:8 bit到16 bit以容纳结果.32bit expand to 64 bit
		movq mm3,mm1 // 因为要用Overlay的Alpha值
		punpckhwd mm3,mm3 // 高字移动到双字
		punpckhdq mm3,mm3 // 双字移动到四字,现在有八个像素的Alpha了!
		movq mm4,mm0 //Source->mm4
		movq mm5,mm1 //Overlay->mm5
		psubusw mm4,mm1 //Source-Overlay,饱和减,小于0为0
		psubusw mm5,mm0 //Overlay-Source,饱和减,小于0为0
		pmullw mm4,mm3 //Alpha * (Source-Overlay)
		pmullw mm5,mm3 //Alpha * (Overlay-Source)
		psrlw mm4,8 //除以256,now mm4 get the result,(Source-Overlay)<0 部分
		psrlw mm5,8 //除以256,now mm5 get the result,(Overlay-Source)>0 部分
		paddusw mm0,mm5 //饱和加到原图象:D=Alpha*(O-S)+S,(Source-Overlay)<0 部分
		psubusw mm0,mm4 //饱和加到原图象D=S-Alpha*(S-O),(Overlay-Source)>0 部分
		packuswb mm0,mm0 //紧缩到低32bit
		mov edx,dword ptr [pSrc]
		movd dword ptr [edx],mm0 //保存结果
		emms ///必要的!Empty MMX Status
	}
}
*/
//------------------------------------------------------------------------