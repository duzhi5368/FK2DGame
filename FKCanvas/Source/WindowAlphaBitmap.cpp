/**
*	created:		2012-6-20   19:33
*	filename: 		WindowAlphaBitmap
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "../Include/WindowAlphaBitmap.h"
//------------------------------------------------------------------------
WAlphaBitmap::WAlphaBitmap()
{

}
//------------------------------------------------------------------------
WAlphaBitmap::~WAlphaBitmap()
{
	Destroy();
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::Create(int w, int h)
{
	WAlphaBmpStruct* pdata = (WAlphaBmpStruct*) GetBmpStruct();
	if(pdata)
	{
		if(pdata->cmem)
		{
			delete []pdata->cmem;
			delete []pdata->ptr;
			pdata->cmem = 0;
			pdata->ptr = 0;
		}
	}
	else
		pdata = new WAlphaBmpStruct;

	pdata->w = w;
	w = (w*2+7)&0xFFFFFFF8; // 8字节对齐

	if((pdata->ptr = new BYTE[w*h+7]) == 0)
	{
		delete pdata;
		FKTraceLn("Cannot create alpha bitmap!");
	}

	if((pdata->cmem = new BYTE[w*h/2 + 3]) == 0)
	{
		delete pdata;
		FKTraceLn("Create alpha channel failed!");
		return FALSE;
	}

	pdata->pitch = w;
	pdata->kx = pdata->ky = 0;
	pdata->h = h;
	pdata->channel = (BYTE*)pdata->cmem;
	SetData(pdata);

	return TRUE;
}
//------------------------------------------------------------------------
void WAlphaBitmap::Destroy()
{
	WAlphaBmpStruct* pdata = (WAlphaBmpStruct*)m_pBmpStruct;

	delete pdata;
	SetData(0);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::Create(IBitmap* parent, int x, int y, int nWidth, int nHeight)
{
	//ASSERT(dynamic_cast<WAlphaBitmap*>(parent));

	WAlphaBmpStruct* data = (WAlphaBmpStruct*)GetBmpStruct();
	WAlphaBmpStruct* parent_data = (WAlphaBmpStruct*)parent->GetBmpStruct();
	
	if(data)
		delete data;

	data = new WAlphaBmpStruct;
	int xx = 0, yy = 0;

	if(x >= parent_data->w || y >= parent_data->h || parent_data->ptr == 0)
	{
		data->Empty();
		SetData(data);
		FKTraceLn("Warning: Create an empty child alpha bitmap");
		return TRUE;
	}

	if(x < 0)
	{
		xx = x;
		nWidth += x;
		x = 0;
	}

	if(y < 0)
	{
		yy = y;
		nHeight += y;
		y = 0;
	}

	if(nWidth <= 0 || nHeight <= 0)
	{
		data->Empty();
		SetData(data);
		return TRUE;
	}

	int offset = y*parent_data->pitch/2+x;
	data->w = MIN(nWidth, parent_data->w - x);
	data->h = MIN(nHeight, parent_data->h - y);
	data->channel = parent_data->channel + offset;
	data->ptr = (BYTE*)((DWORD)(parent_data->ptr) + offset*2);
	data->cmem = 0;
	data->pitch = parent_data->pitch;
	data->kx = xx;
	data->ky = yy;

	SetData(data);
	return TRUE;	
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::Create(int w, int h, void* ptr)
{
	WAlphaBmpStruct* data = (WAlphaBmpStruct*)GetBmpStruct();

	if(data)
	{
		if(data->ptr)
		{
			delete []data->ptr;
			data->ptr = 0;
			delete []data->cmem;
			data->cmem = 0;
		}
	}
	else
		data = new WAlphaBmpStruct;
	data->w = w;
	w = (w*2+7) & 0xFFFFFFF8; // 8字节对齐
	data->ptr = ptr;
	data->channel = (BYTE*)((DWORD)ptr + w*h);
	data->pitch = w;
	data->kx = data->ky = 0;
	data->h = h;
	data->cmem = 0;

	SetData(data);

	return TRUE;
}
//------------------------------------------------------------------------
void WAlphaBitmap::Draw(int x, int y,const char* s, WPixel clr)
{
	WBitmap::Draw(x, y, s, clr);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::Draw(int dx, int dy, int w, int h, IBitmap* bmp, int sx, int sy, DWORD dwStyle /* = BLIT_NORMAL */)
{
	return WBitmap::Draw(dx, dy, w, h, bmp, sx, sy, dwStyle);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::Draw(int x, int y, IBitmap* bmp, DWORD dwStyle)
{
	return WBitmap::Draw(x, y, bmp, dwStyle);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::DrawAlpha(int dx, int dy, IAlphaBitmap* bmp, DWORD dwStyle )//把别的位图绘制到此位图
{
	return WBitmap::DrawAlpha(dx, dy, bmp, dwStyle);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::DrawAlpha(int dx, int dy, int w, int h, IAlphaBitmap* bmp, int sx, int sy, DWORD dwStyle) //增强的AlphaDraw
{
	return WBitmap::DrawAlpha(dx, dy, w, h, bmp, sx, sy, dwStyle);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::DrawRle(int dx, int dy, IRleBitmap* bmp, DWORD dwStyle)
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::DrawRle(int dx, int dy, int w, int h, IRleBitmap* bmp, int sx, int sy, DWORD dwStyle)
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::DrawRle(int dx, int dy, IRleBitmap* bmp, BYTE* pPal, DWORD dwStyle)
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::DrawRle(int dx, int dy, int w, int h, IRleBitmap* bmp, BYTE* pPal, int sx, int sy, DWORD dwStyle)
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::DrawBox(RECT& rc, WPixel clr, DWORD dwStyle /* = BLIT_NORMAL */, int alpha /* = 255 */)
{
	return WBitmap::DrawBox(rc, clr, dwStyle, alpha);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::DrawLine(POINT& ptFrom, POINT& ptTo, WPixel clr, DWORD dwStyle /* = BLIT_NORMAL */, int alpha /* = 255 */)
{
	return WBitmap::DrawLine(ptFrom, ptTo, clr, dwStyle, alpha);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::DrawText(ITTFont* pFont, int x, int y, const char* str, WPixel color, DWORD style/* =FONT_ALPHA */)
{
	return WBitmap::DrawText(pFont, x, y, str, color, style);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::DrawTextEx(ITTFont* pFont, int x, int y, const char* str, WPixel color, WPixel outlinecolor, DWORD style/* =FONT_ALPHA */)
{
	return WBitmap::DrawTextEx(pFont, x, y, str, color, outlinecolor, style);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::IsTransparent(POINT& ptSelect)
{	
	if(!IsValid())
	{
		FKTraceLn("AlphaBitmap is invalid!");
		return TRUE; // 图片为空，可看作透明，呵呵
	}

	WAlphaBmpStruct* data = (WAlphaBmpStruct*)m_pBmpStruct;
	
	if (ptSelect.x < 0 || ptSelect.y < 0 ||
		ptSelect.x > data->w || ptSelect.y > data->h) // 非法点
		return TRUE;

	BYTE* pLine = data->channel + ptSelect.y*data->pitch/2;
	BYTE* pColor = pLine + ptSelect.x;
	if (*pColor == 0)
		return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------
int WAlphaBitmap::GetHeight() const
{
	return WBitmap::GetHeight();
}
//------------------------------------------------------------------------
DWORD WAlphaBitmap::GetUserData() const
{
	return WBitmap::GetUserData();
}
//------------------------------------------------------------------------
int WAlphaBitmap::GetWidth()const 
{
	return WBitmap::GetWidth();
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::IsValid() const
{
	WAlphaBmpStruct* pdata = (WAlphaBmpStruct*)m_pBmpStruct;
	return (pdata->ptr != 0 && pdata->channel != 0);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::Load(void *bmpdata)
{
	return LoadTGA(bmpdata);
}
//------------------------------------------------------------------------
WPixel* WAlphaBitmap::operator[](int y)const
{
	return WBitmap::operator[](y);
}
//------------------------------------------------------------------------
void WAlphaBitmap::Release()
{
	delete this;
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::Save(const char *filename) const
{
	return WBitmap::Save(filename);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::Scroll(int x, int y)
{
	FKTraceLn("Alphabitmap scroll is not implemented!");
	return FALSE;
}
//------------------------------------------------------------------------
void WAlphaBitmap::SetUserData(DWORD userdata)
{
	WBitmap::SetUserData(userdata);
}
//------------------------------------------------------------------------
IBitmap* WAlphaBitmap::SubBitmap(int x, int y, int w, int h)
{
	WAlphaBitmap* pBmp = new WAlphaBitmap;
	pBmp->Create(static_cast<WBitmap*>(this), x, y, w, h);

	return (WBitmap*)pBmp;
}
//------------------------------------------------------------------------
IBitmap* WAlphaBitmap::SubTile(POINT ptLeftTop)
{
	FKTraceLn("Alphabitmap SubTile is not implemented!");
	return NULL;
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::Clear(WPixel clr, RECT* pRc /* = NULL */)
{
	return WBitmap::Clear(clr, pRc);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::Blit2DC(HDC hDC,int dx,int dy,int x,int y,int w,int h)
{
	return WBitmap::Blit2DC(hDC, dx, dy, x, y, w, h);
}
//------------------------------------------------------------------------
BYTE* WAlphaBitmap::ChannelLine(int y) const
{
	ASSERT(IsValid());

	WAlphaBmpStruct* data = (WAlphaBmpStruct*)m_pBmpStruct;
	return data->channel+y*data->pitch/2;
}
//------------------------------------------------------------------------
void WAlphaBitmap::ClearChannel(int alpha)
{
	ASSERT(IsValid());

	WAlphaBmpStruct* data = (WAlphaBmpStruct*)m_pBmpStruct;
	memset(data->channel, alpha, data->h*data->pitch/2);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::Combine(IAlphaBitmap* bmp)
{

	WAlphaBmpStruct* dest_data = (WAlphaBmpStruct*)m_pBmpStruct;
	WAlphaBmpStruct* src_data = (WAlphaBmpStruct*)bmp->GetBmpStruct();
	if(src_data == 0 || dest_data == 0)
	{
		FKTraceLn("WAlphaBitmap::Combine error: invalid bitmap");
		return FALSE;
	}

	int from_line = dest_data->ky - src_data->ky;
	int to_line = from_line+src_data->h;

	int src_line;
	if(from_line < 0)
	{
		src_line = -from_line;
		from_line = 0;
	}
	else
		src_line = 0;

	to_line = MIN(to_line, dest_data->h);
	if(from_line >= to_line)
		return TRUE;

	int from_offset = dest_data->kx - src_data->kx;
	int to_offset = from_offset + src_data->w;
	int src_offset = 0;
	if(from_offset < 0)
	{
		src_offset = -from_offset;
		from_offset = 0;
	}

	int i, j;
	BYTE *line_src, *line_des;
	DrawAlpha(0, 0, bmp);

	for(i = from_line; i < to_line; i ++, src_line ++)
	{
		line_des = ChannelLine(i) + from_offset;
		line_src = bmp->ChannelLine(src_line) + src_offset;
		for(j = from_offset; j < to_offset; j ++)
		{
			*line_des = 255-((255-*line_des)*(255-*line_src)+255)/256;
			++line_des;
			++line_src;
		}
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::Load2Bmp(const void* bmpdata, const void* alphadata)
{
	int width, height;
	
	
	BITMAPFILEHEADER* bfbmp = (BITMAPFILEHEADER*)bmpdata;
	BITMAPFILEHEADER* bfalpha = (BITMAPFILEHEADER*)alphadata;

	if(bfbmp->bfType != 19778 || bfalpha->bfType != 19778)	//"MB"
	{
		FKTraceLn("WAlphaBitmap::Load2Bmp fail, the input data is not a bitmap!");
		return FALSE;
	}

	BITMAPINFOHEADER* bibmp = (BITMAPINFOHEADER*)((DWORD)bmpdata + sizeof(
		BITMAPFILEHEADER));
	BITMAPINFOHEADER* bialpha = (BITMAPINFOHEADER*)((DWORD)alphadata + sizeof(
		BITMAPFILEHEADER));
	if(bibmp->biWidth != bialpha->biWidth)
	{
		FKTraceLn("Load2bmp the bitmapwidth %d != alpha width %d", bibmp->biWidth, 
			bibmp->biHeight);
		return FALSE;

	}

	if(bibmp->biHeight != bialpha->biHeight)
	{
		FKTraceLn("Load2bmp the bitmap height %d != alpha height %d", bibmp->biHeight,
			bialpha->biHeight);
		return FALSE;
	}

	if(bialpha->biBitCount != 8)
	{
		FKTraceLn("Load2bmp, the alpha bitmap only receive 8 bit bitmap, but the %d\
			is present", bialpha->biBitCount);
		return FALSE;
	}

	if(!Create(bibmp->biWidth, bibmp->biHeight))
	{
		return FALSE;
	}

	WAlphaBmpStruct* data = (WAlphaBmpStruct*)m_pBmpStruct;
	width = MIN(data->w, (int)bibmp->biWidth);
	height = MIN(data->h, (int)bibmp->biHeight);

	//Load the alpha channel
	{
		static BYTE palAlpha[256];

		int p = (bialpha->biWidth + 3) &0xFFFFFFFC;

		BYTE* bits = (BYTE*)((DWORD)bfalpha + bfalpha->bfOffBits + 
			p*(bialpha->biHeight - height));

		int i, j, usecolor = bialpha->biClrUsed;
		if(usecolor == 0)
			usecolor = 255;
		
		DWORD* color = (DWORD*)((DWORD)bialpha + bialpha->biSize);
		//FKTraceLn("Begin trace alpha:");
		for(i = 0; i <= usecolor; i ++)
		{
			palAlpha[i] = (BYTE)color[i] ;
			
		}

		BYTE* alphabits = ChannelLine(height - 1);

		for(i = 0; i < height; i ++, bits += p, alphabits = ChannelLine(height - i - 1))
		{
			for(j = 0; j < width; j ++)
				alphabits[j] = palAlpha[bits[j]];
		}

	}

	//Load bitmap data
	//格式转换
	WPixel* bmppix;
	BYTE* alpapix;
	switch(bibmp->biBitCount)
	{
	case 8:	//调色板
		{
			static WPixel pal[256];
			int p = (bibmp->biWidth + 3) & 0xFFFFFFFC;
			

			BYTE* bits = (BYTE*)((DWORD)bfbmp + bfbmp->bfOffBits + p*(bibmp->biHeight - height));

			int i, j, usecolor = bibmp->biClrUsed;
			
			if(usecolor == 0)
				usecolor = 256;

			DWORD* color = (DWORD*)((DWORD)bibmp + bibmp->biSize);

			for(i = 0; i < usecolor; i ++)
				pal[i] = color[i];

			bmppix = (*this)[height - 1];
			alpapix = ChannelLine(height - 1);

			for(i = 0; i < height; i ++, bits += p, bmppix = (*this)[height - 1 - i],
				alpapix = ChannelLine(height - 1 - i))
				for(j = 0; j < width; j ++)
					if(alpapix[j])
					{
					
						bmppix[j] = pal[bits[j]];
						if(alpapix[j] < 255)
						{
							bmppix[j].red -= bmppix[j].red*(256 - alpapix[j])/256;
							bmppix[j].blue -= bmppix[j].blue*(255 - alpapix[j])/256;
							bmppix[j].green -= bmppix[j].green*(255 - alpapix[j])/256;
							bmppix[j].red = MAX(0, MIN(bmppix[j].red, alpapix[j]));
							bmppix[j].green = MAX(0, MIN(bmppix[j].green, alpapix[j]));
							bmppix[j].blue = MAX(0, MIN(bmppix[j].blue, alpapix[j]));

						}
					}
					else
						bmppix[j] = 0;
		}
		break;

	case 16:	//16位
		{
			int p = ((bibmp->biWidth*2 + 3) & 0xFFFFFFFC)/2;
			short* bits = (short*)((DWORD)bfbmp + bfbmp->bfOffBits + p*(bibmp->biHeight - height));
			
			int i, j;
			bmppix = (*this)[height - 1];
			alpapix = ChannelLine(height - 1);
			for(i = 0; i < height; i ++, bits += p, bmppix = (*this)[height - 1 - i],
				alpapix = ChannelLine(height - 1 - i))
				for(j = 0; j < width; j ++)
					if(alpapix[j])
					{
						bmppix[j] = bits[j];
						if(alpapix[j] < 255)
						{
							bmppix[j].red -= bmppix[j].red*(256 - alpapix[j])/256;
							bmppix[j].blue -= bmppix[j].blue*(255 - alpapix[j])/256;
							bmppix[j].green -= bmppix[j].green*(255 - alpapix[j])/256;
							bmppix[j].red = MAX(0, MIN(bmppix[j].red, alpapix[j]));
							bmppix[j].green = MAX(0, MIN(bmppix[j].green, alpapix[j]));
							bmppix[j].blue = MAX(0, MIN(bmppix[j].blue, alpapix[j]));

						}
					}
					else
						bmppix[j] = 0;
		}
		break;
		
	case 24:	//24位
		{
			
			int p = (bibmp->biWidth*3+3)&0xFFFFFFFC;
			
			DWORD bits = (DWORD)bfbmp + bfbmp->bfOffBits + p*(bibmp->biHeight - height);

			int i, j;

			bmppix = (*this)[height - 1];
			alpapix = ChannelLine(height - 1);
			int r, g, b;
			
			
			for(i = 0; i < height; i ++, bits += p, bmppix = (*this)[height - 1 - i],
				alpapix = ChannelLine(height - 1 - i))
				for(j = 0; j < width; j ++)
					if(alpapix[j])
					{
						
						//bmppix[j] = *(DWORD*)(bits + j*3);
						
						b = (BYTE)*(DWORD*)(bits + j*3);
						g = (BYTE)(*(DWORD*)(bits + j*3) >> 8);
						r = (BYTE)(*(DWORD*)(bits + j*3) >> 16);		
						
						if(alpapix[j] < 255)
						{
							r -= r*(256 - alpapix[j])/256;
							g -= g*(256 - alpapix[j])/256;
							b -= b*(256 - alpapix[j])/256;
							r = MAX(0, MIN(r, alpapix[j]));
							g = MAX(0, MIN(g, alpapix[j]));
							b = MAX(0, MIN(b, alpapix[j]));
						}
						
						bmppix[j] = WPixel(r, g, b);
						
					}
					else
						bmppix[j] = 0;
			
		}
		break;

	default:
		{
			FKTraceLn("Load Bitmap %d is invalid bitcount!", bibmp->biBitCount);
			return FALSE;
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::Load2BmpEx(IBitmap* pBmp, int nBmpX, int nBmpY,
	IBitmap* pAlpha, int nAlphaX, int nAlphaY,
	int nWidth, int nHeight)
{	
	if (!pBmp->IsValid() || !pAlpha->IsValid()) return FALSE;

	// create sub bitmap
	IBitmap* pSubBmp;
	if (nBmpX >= 0 && nBmpY >= 0)
		pSubBmp = pBmp->SubBitmap(nBmpX, nBmpY, nWidth, nHeight);
	else
	{
		POINT pt;pt.x=nBmpX;pt.y=nBmpY;
		pSubBmp = pBmp->SubTile(pt);
	}
	IBitmap* pSubAlpha = pAlpha->SubBitmap(nAlphaX, nAlphaY, nWidth, nHeight);
	if (!pSubBmp || !pSubAlpha) 
	{
		FKTraceLn("WAlphaBitmap::Load2BmpEx: SubBitmap error");
		return FALSE;
	}

	// create IAlphaBitmap
	if(!Create(nWidth, nHeight))
	{
		return FALSE;
	}

	WAlphaBmpStruct* data = (WAlphaBmpStruct*)m_pBmpStruct;
	int width = MIN(data->w, nWidth);
	int height = MIN(data->h, nHeight);

	WPixel* bmppix, *bmppixSrc, *alphapixSrc;
	BYTE* alphapix;
	bmppix = (*this)[0];
	bmppixSrc = (*pSubBmp)[0];
	alphapixSrc = (*pSubAlpha)[0];
	alphapix = ChannelLine(0);

	int i, j;
	for(i = 0; i < height; i ++, 
		bmppixSrc =(*pSubBmp)[i], 
		bmppix = (*this)[i],
		alphapixSrc =(*pSubAlpha)[i], 
		alphapix = ChannelLine(i)
		)
		for(j = 0; j < width; j ++)
			if(alphapix[j] = (alphapixSrc[j].green<<2))
			{
				bmppix[j] = bmppixSrc[j];
				if(alphapix[j] < 255)
				{
					bmppix[j].red -= bmppix[j].red*(256 - alphapix[j])>>8;
					bmppix[j].blue -= bmppix[j].blue*(255 - alphapix[j])>>8;
					bmppix[j].green -= bmppix[j].green*(255 - alphapix[j])>>8;
					bmppix[j].red = MAX(0, MIN(bmppix[j].red, alphapix[j]));
					bmppix[j].green = MAX(0, MIN(bmppix[j].green, alphapix[j]));
					bmppix[j].blue = MAX(0, MIN(bmppix[j].blue, alphapix[j]));

				}
			}
			else
				bmppix[j] = 0;

	if (nBmpX < 0 || nBmpY < 0)
		pSubBmp->Release();
	return TRUE;
}
//------------------------------------------------------------------------
extern void __fastcall Memset16Bit (WPixel* dst, WPixel fill, int num );
//------------------------------------------------------------------------
BOOL WAlphaBitmap::LoadEx(void* customdata)
{
	RLEHEADER* rlehead = (RLEHEADER*)customdata;

	if (rlehead->wFlags != 'FG')
	{
		FKTraceLn("WAlphaBitmap::LoadEx invalid rle head");
		return FALSE;
	}
	
	WPixel* bmppix;
	BYTE* alpha_line;
	int i, j, width, height, usecolor = rlehead->nUseColor;
	
	if(m_pBmpStruct)
		Destroy();

	Create(width = rlehead->dwWidth, height = rlehead->dwHeight);
	BYTE* bits = (BYTE*)((DWORD)rlehead + rlehead->dwDataOffset);

	//格式转换
	WPixel* color = (WPixel*)((DWORD)customdata + sizeof(RLEHEADER));

	// 读入数据
	int nPos = 0, rep_count;
	for(i = 0; i < height; i ++)
	{
		bmppix = (*this)[i];
		alpha_line = ChannelLine(i);
		for(j = 0; j < width;)
		{
			if (bits[nPos] & 0x80) // 连续的透明点
			{
				rep_count = (bits[nPos++]&0x7f) + 1;
				Memset16Bit(&bmppix[j], 0, rep_count);
				memset(&alpha_line[j], 0, rep_count);
				j += rep_count;
			}
			else // 连续的不规则点
			{
				BYTE alpha;
				rep_count = bits[nPos++] + 1;
				if (rep_count&0x1) // odd
				{
					bmppix[j] = color[bits[nPos++]];
					alpha_line[j++] = ((bits[nPos++]+1)<<5)-1;
					rep_count --;
				}

				while (rep_count)
				{
					bmppix[j] = color[bits[nPos++]];
					alpha = bits[nPos++];
					alpha_line[j++] = ((((alpha&0xf0)>>4)+1)<<5)-1;
					bmppix[j] = color[bits[nPos++]];
					alpha_line[j++] = (((alpha&0xf)+1)<<5)-1;
					rep_count -= 2;
				}
			}
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------
// 得到图片所占内存的大小
DWORD WAlphaBitmap::GetSize()
{
	WAlphaBmpStruct* pData = (WAlphaBmpStruct*)m_pBmpStruct;
	if (pData)
	{
		int size = pData->pitch*pData->h;
		size += size>>1;
		return (size+10);
	}
	return 0;
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::DrawPixel(int x, int y, WPixel clr, DWORD dwStyle, int alpha)
{
	return WBitmap::DrawPixel(x, y, clr, dwStyle, alpha);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::DrawText(int x, int y, const char* str, WPixel color, DWORD style/*=FONT_ALPHA*/)
{
	return WBitmap::DrawText(x, y, str, color, style);
}
//------------------------------------------------------------------------
// 将IBitmap画向Canvas
BOOL WAlphaBitmap::Draw(ICanvas* pCanvas, int dx, int dy, DWORD dwStyle)
{
	return pCanvas->DrawAlpha(dx, dy, static_cast<IAlphaBitmap*>(this), dwStyle);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::Draw(ICanvas* pCanvas, int dx, int dy, int w, int h, int sx, int sy, DWORD dwStyle)
{
	return pCanvas->DrawAlpha(dx, dy, w, h, static_cast<IAlphaBitmap*>(this), sx, sy, dwStyle);
}
//------------------------------------------------------------------------
BOOL WAlphaBitmap::LoadTGA(const void *tgadata)
{
	DWORD dataptr;
	WPixel *line;
	BYTE* alpha_line;
	int i, j, width, height,ii;
	TGAFILEHEADER* head = (TGAFILEHEADER*)tgadata;
	if((head->bImageType != 2 && head->bImageType != 10)
		|| head->bBits != 32 || head->bInterleaving != 0/* || head->bTopDown != 0*/)
	{
		if(head->bBits == 24)	//没有通道
		{
			if(m_pBmpStruct)
				Destroy();

			Create(width = (int)head->wWidth, height=(int)head->wHeight);
			dataptr = (DWORD)tgadata + sizeof(TGAFILEHEADER)+head->bIDLength;
			if(head->bImageType == 2)	//不压缩
			{
				int rr = 0, gg = 0, bb = 0;
				bb = *(BYTE*)dataptr;
				gg = *(BYTE*)(dataptr + 1);
				rr = *(BYTE*)(dataptr + 2);

				for(ii = height - 1; ii >= 0; ii --)
				{
					if (head->bTopDown) 
						i = height - 1 - ii;
					else 
						i = ii;
					line = (*this)[i];
					alpha_line = ChannelLine(i);
					for(j = 0; j < width; j ++)
					{
						int red, green, blue;
						blue = *(BYTE*)dataptr ++;
						green = *(BYTE*)dataptr ++;
						red = *(BYTE*)dataptr ++;
						if(red == rr && green == gg && blue == bb)
						{
							line[j].color = 0;
							alpha_line[j] = 0;
						}
						else
						{
							line[j] = WPixel(red, green, blue);
							alpha_line[j] = 255;
						}
					}		

				}
			}
			else	//RLE压缩
			{
				int run = 0, raw = 0;
				WPixel runpixel;
				int runalpha;
				int red, green, blue;
				int rr, gg, bb;
				bb = *(BYTE*)(dataptr + 1);
				gg = *(BYTE*)(dataptr + 2);
				rr = *(BYTE*)(dataptr + 3);
				for(ii = height - 1; ii >= 0; ii --)
				{
					if (head->bTopDown)
						i = height - 1 - ii;
					else 
						i = ii;
					line = (*this)[i];
					alpha_line = ChannelLine(i);
					for(j = 0; j < width;)
					{
						if(run > 0)
						{
							line[j] = runpixel;
							alpha_line[j] = runalpha;
							--run;
							++j;
						}
						else if(raw > 0)
						{
							blue = *(BYTE*)dataptr ++;
							green = *(BYTE*)dataptr ++;
							red = *(BYTE*)dataptr ++;
							if(red == rr && green == gg && blue == bb)//mask
							{
								line[j].color = 0;
								alpha_line[j] = 0;
							}
							else
							{
								line[j] = WPixel(red, green, blue);
								alpha_line[j] = 255;

							}
							--raw;
							++j;
						}		
						else		//读数据
						{
							BYTE packhead = *(BYTE*)dataptr ++;
							if(packhead & 0x80)
							{
								run = (packhead & 0x7F) + 1;
									
								blue = *(BYTE*)dataptr ++;
								green = *(BYTE*)dataptr ++;
								red = *(BYTE*)dataptr ++;
								if(red == rr && green == gg && blue == bb)
									runpixel.color = 0, runalpha = 0;
								else
								{
									runpixel = WPixel(red, green, blue);
									runalpha = 255;
								}
							}
							else
							{
								raw = packhead + 1;
							}

						}
					}

				}

			}
			return TRUE;
		}
		
		//FKTraceLn("WAlphaBitmap::LoadTGA load a none alpha channel bitmap!");
		return FALSE;
	}
	
	if(m_pBmpStruct)
		Destroy();

#define __Alpha_Level 255
	enum {fResume=1,fPreMulti=2};
	DWORD dwFlagResume_And_PreMulti = fResume | fPreMulti;
	//dwFlagResume_And_PreMulti = 0;

	Create(width = (int)head->wWidth, height = (int)head->wHeight);
	dataptr = (DWORD)tgadata+sizeof(TGAFILEHEADER)+head->bIDLength;
	if(head->bImageType == 2)	//不压缩
	{
		int rr = 0, gg = 0, bb = 0;
		if((*(BYTE*)(dataptr + 3)) == 0)
		{
			bb = *(BYTE*)dataptr;
			gg = *(BYTE*)(dataptr+1);
			rr = *(BYTE*)(dataptr+2);
		}
		for(ii = height-1; ii >= 0; ii --)
		{
			if (head->bTopDown)
				i = height - 1 - ii;
			else 
				i = ii;
			line = (*this)[i];
			alpha_line = ChannelLine(i);
			for(j = 0; j < width; j ++)
			{
				int red, green, blue, alpha;
				blue = *(BYTE*)dataptr ++;
				green = *(BYTE*)dataptr ++;
				red = *(BYTE*)dataptr ++;
				alpha = *(BYTE*)dataptr ++;
				if(alpha == 0)
					line[j] = 0;
				else
				{
					if(alpha < 255)
					{
						// 还原
						if (dwFlagResume_And_PreMulti & fResume)
						{							
							red -= rr*(__Alpha_Level-alpha)/__Alpha_Level;
							if (red < 0) red = 0;
							red = red * __Alpha_Level / alpha;
							if (red > 255) red = 255;
							
							green -= gg*(__Alpha_Level-alpha)/__Alpha_Level;
							if (green < 0) green = 0;
							green = green * __Alpha_Level / alpha;
							if (green > 255) green = 255;
							
							blue -= bb*(__Alpha_Level-alpha)/__Alpha_Level;
							if (blue < 0) blue = 0;
							blue = blue * __Alpha_Level / alpha;
							if (blue > 255) blue = 255;
						}

						// 预乘、补偿
						if (dwFlagResume_And_PreMulti & fPreMulti)
						{
							red = red * alpha + 128;red = (red + red / 256) / 256;
							green = green * alpha + 128;green = (green + green / 256) / 256;
							blue = blue * alpha + 128;blue = (blue + blue / 256) / 256;
						}

						// 预乘处理(原理：先去掉跟背景色(rr,gg,bb)合并前的颜色，再进行预乘)
						/*red -= rr*(256-alpha)/256; // 这里计算后的red实际上 = 未与背景合成前的red*alpha/256
						green -= gg*(256-alpha)/256;
						blue -= bb*(256-alpha)/256;
						red = MAX(0, MIN(alpha, red));
						green = MAX(0, MIN(alpha, green));
						blue = MAX(0, MIN(alpha, blue));
						red = red * 255 / alpha;
						green = green * 255 / alpha;
						blue = blue * 255 / alpha;*/
					}
					line[j] = WPixel(red, green, blue);
				}
				alpha_line[j] = alpha;
			}
		}
		
	}
	else	//RLE压缩
	{
		int run = 0, raw = 0;
		WPixel runpixel;
		int runalpha;
		int red, green, blue, alpha;
		int rr = 0, gg = 0, bb = 0;
		if(*(BYTE*)(dataptr + 4) == 0)
		{
			bb = *(BYTE*)(dataptr + 1);
			gg = *(BYTE*)(dataptr + 2);
			rr = *(BYTE*)(dataptr + 3);
		}

		for(ii = height - 1; ii >= 0; ii --)
		{
			if (head->bTopDown)
				i = height - 1 - ii;
			else 
				i = ii;
			line = (*this)[i];
			alpha_line = ChannelLine(i);
			for(j = 0; j < width;)
			{
				if(run > 0)
				{
					line[j] = runpixel;
					alpha_line[j] = runalpha;
					-- run;
					++ j;
				}
				else if(raw > 0)
				{
					blue = *(BYTE*)dataptr ++;
					green = *(BYTE*)dataptr ++;
					red = *(BYTE*)dataptr ++;
					alpha = *(BYTE*)dataptr ++;
					if(alpha == 0)
						line[j] = 0;
					else
					{
						if(alpha < 255)
						{
							// 还原
							if (dwFlagResume_And_PreMulti & fResume)
							{
								red -= rr*(__Alpha_Level-alpha)/__Alpha_Level;
								if (red < 0) red = 0;
								red = red * __Alpha_Level / alpha;
								if (red > 255) red = 255;
								
								green -= gg*(__Alpha_Level-alpha)/__Alpha_Level;
								if (green < 0) green = 0;
								green = green * __Alpha_Level / alpha;
								if (green > 255) green = 255;
								
								blue -= bb*(__Alpha_Level-alpha)/__Alpha_Level;
								if (blue < 0) blue = 0;
								blue = blue * __Alpha_Level / alpha;
								if (blue > 255) blue = 255;
							}
							
							// 预乘、补偿
							if (dwFlagResume_And_PreMulti & fPreMulti)
							{
								red = red * alpha + 128;red = (red + red / 256) / 256;
								green = green * alpha + 128;green = (green + green / 256) / 256;
								blue = blue * alpha + 128;blue = (blue + blue / 256) / 256;
							}
						}
						line[j] = WPixel(red, green, blue);
					}
					alpha_line[j] = alpha;
					--raw;
					++j;
				}
				else
				{
					BYTE packhead = *(BYTE*)dataptr ++;
					if(packhead & 0x80)
					{
						run = (packhead &0x7f) + 1; 

						blue = *(BYTE*)dataptr ++;
						green = *(BYTE*)dataptr ++;
						red = *(BYTE*)dataptr ++;
						runalpha = *(BYTE*)dataptr ++;
						if(runalpha == 0)
							runpixel = 0;
						else
						{
							if(runalpha < 255)
							{
								// 还原
								if (dwFlagResume_And_PreMulti & fResume)
								{
									red -= rr*(__Alpha_Level-runalpha)/__Alpha_Level;
									if (red < 0) red = 0;
									red = red * __Alpha_Level / runalpha;
									if (red > 255) red = 255;

									green -= gg*(__Alpha_Level-runalpha)/__Alpha_Level;
									if (green < 0) green = 0;
									green = green * __Alpha_Level / runalpha;
									if (green > 255) green = 255;

									blue -= bb*(__Alpha_Level-runalpha)/__Alpha_Level;
									if (blue < 0) blue = 0;
									blue = blue * __Alpha_Level / runalpha;
									if (blue > 255) blue = 255;
								}
								// 预乘、补偿
								if (dwFlagResume_And_PreMulti & fPreMulti)
								{
									red = red * runalpha + 128;red = (red + red / 256) / 256;
									green = green * runalpha + 128;green = (green + green / 256) / 256;
									blue = blue * runalpha + 128;blue = (blue + blue / 256) / 256;
								}
							}
							runpixel = WPixel(red, green, blue);
								
						}
					}
					else
						raw = packhead + 1;
				}
			}
		}
		
	}

	return TRUE;	
}
//------------------------------------------------------------------------
//可存为无压缩32位TGA
BOOL WAlphaBitmap::SaveTGA(const char *filename) const
{
	FILE* f;
	f = fopen(filename, "wb");
	if(f == 0)
	{
		FKTraceLn("Open %s fail!", filename);
		return FALSE;
	}

	int i, j;
	TGAFILEHEADER head;
	memset(&head, 0, sizeof(TGAFILEHEADER));
	head.bImageType = 2;	//不压缩
	head.bBits = 32;
	head.wWidth = GetWidth();
	head.wHeight = GetHeight();
	head.bAlphaBits = 8;

	fwrite(&head, 1, sizeof(head), f);
	DWORD* line = (DWORD*)_alloca(GetWidth()*4);
	WPixel* data_line;
	BYTE* channel_line;

	for(i = GetHeight() - 1; i >= 0; i --)
	{
		data_line = operator[](i);
		channel_line = ChannelLine(i);
		for(j = 0; j < GetWidth(); j ++, ++data_line, ++channel_line)
		{
			int r, g, b;
			r = data_line->red;
			if(r > 0)
				r = (r << 3 | 0x07) << 16;
			
			g = data_line->green;
			if(g > 0)
				g = (g << 2 | 0x3) << 8;

			b = data_line->blue;
			if(b > 0)
				g = (b << 3 | 0x7);
			line[j] = (DWORD)r|g|g|(*channel_line << 24);
		}
		fwrite(line, GetWidth()*4, 1, f);
	}
	fclose(f);

	return TRUE;
	
}
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) BOOL CreateIAlphaBitmap(DWORD dwVersion, IAlphaBitmap** ppIBitmap, ITrace* pTrace)
{
#ifdef _DEBUG
	if(pTrace)
		g_DefaultTrace.Create(pTrace);
#endif

	WAlphaBitmap* pBitmap = new WAlphaBitmap;
	
	if(pBitmap == NULL)
		return FALSE;

	*ppIBitmap = static_cast<IAlphaBitmap*>(pBitmap);

	return TRUE;
}
//------------------------------------------------------------------------