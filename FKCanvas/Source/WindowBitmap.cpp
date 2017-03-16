/**
*	created:		2012-6-20   18:54
*	filename: 		WindowBitmap
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/WindowBitmap.h"
#include "../Include/WindowChar.h"
#include "../asm/Asm.h"
#include <stdio.h>
//------------------------------------------------------------------------
WBitmap::WBitmap()
{
	m_pBmpStruct = 0;

}
//------------------------------------------------------------------------
WBitmap::~WBitmap()
{
	Destroy();
}
//------------------------------------------------------------------------
void WBitmap::Destroy()
{
	WBmpStruct* pdata = (WBmpStruct*)GetBmpStruct();
	SAFE_DELETE(pdata);	
}
//------------------------------------------------------------------------
//释放
void WBitmap::Release()
{
	delete this;
}
//------------------------------------------------------------------------
//创建地图
BOOL WBitmap::Create(int nWidth, int nHeight)
{
	if(nWidth <= 0 || nHeight <= 0)
	{
		FKTrace("Invalid bitmap width or height!\n");
		return FALSE;
	}
	
	//检查以前是否有数据	
	WBmpStruct* pdata = (WBmpStruct*)GetBmpStruct();
	if(pdata)
	{
		SAFE_DELETE(pdata->ptr);
	}
	else
		pdata = new WBmpStruct;
	
	pdata->w = nWidth;
	pdata->h = nHeight;

	//QWORD对齐
	nWidth = (nWidth*2+7)&0xFFFFFFF8; // 8字节对齐
	pdata->pitch = nWidth;

	if((pdata->ptr = new BYTE[nWidth*nHeight+7]) == 0) // 多出几个字节是为了防止mmx指令访问内存越界，千万不要拿掉
	{
		SAFE_DELETE(pdata);
		FKTrace("Create bitmap, no enought memory!\n");
		return FALSE;
	}
	SetData(pdata);
	
	return TRUE;
}
//------------------------------------------------------------------------
BOOL WBitmap::Create(int nWidth, int nHeight, void* ptr)
{
	if(nWidth <= 0 || nHeight <= 0 || ptr == NULL)
	{
		FKTrace("Invalid bitmap width or height or data!\n");
		return FALSE;
	}
	
	//检查以前是否有数据
	WBmpStruct* pdata = (WBmpStruct*)GetBmpStruct();
	if(pdata)
	{
		SAFE_DELETE(pdata->ptr);
	}
	else
		pdata = new WBmpStruct;
	
	pdata->w = nWidth;
	pdata->h = nHeight;

	//QWORD对齐
	nWidth = (nWidth*2+7)&0xFFFFFFF8; // 8字节对齐
	pdata->pitch = nWidth;

	pdata->ptr = ptr;
	SetData(pdata);

	return TRUE;
}
//------------------------------------------------------------------------
//从父对象创建一个位图，与父对象共享内存
BOOL WBitmap::Create(IBitmap* parent, int x, int y, int nWidth, int nHeight)
{
	if(!parent->IsValid())
	{
		FKTrace("Invalid parent!\n");
		return FALSE;
	}
		
	WBmpStruct* pdata = (WBmpStruct*)GetBmpStruct();
	WBmpStruct* pParentData = (WBmpStruct*)parent->GetBmpStruct();

	if(pdata)
	{
		delete []pdata->ptr;
		pdata->ptr = 0;
	}
	else
		pdata = new WBmpStruct;

	if(x >= pParentData->w || y >= pParentData->h)
	{
		pdata->Empty();
		SetData(pdata);
		return TRUE;
	}

	int xx = 0, yy = 0;

	if(x < 0) nWidth += x, x = 0;
	if(y < 0) nHeight += y, y = 0;

	if(nWidth <= 0 || nHeight <= 0)
	{
		pdata->Empty();
		SetData(pdata);
		return TRUE;
	}
			
	pdata->w = MIN(nWidth, pParentData->w - x);
	pdata->h = MIN(nHeight, pParentData->h - y);
	
	pdata->pitch = pParentData->pitch;
	pdata->ptr = (void*)((DWORD)(pParentData->ptr) + y*pParentData->pitch + x*2);
	ASSERT(pdata->ptr);
	pdata->userdata = pParentData->userdata;
	pdata->bChild = TRUE;
	pdata->kx = xx;
	pdata->ky = yy;

	SetData(pdata);

	return TRUE;
}
//------------------------------------------------------------------------
//创建一个子位图，与本位图共享内存空间
IBitmap* WBitmap::SubBitmap(int x, int y, int nWidth, int nHeight)
{
	WBitmap* pBitmap = new WBitmap;
	if(pBitmap->Create(static_cast<IBitmap*>(this), x, y, nWidth, nHeight))
	{
		return static_cast<IBitmap*>(pBitmap);
	}
	else
	{
		delete pBitmap;
		return NULL;
	}
}
//------------------------------------------------------------------------
//从本位图上创建一个独立的Tile子位图(即菱形格位图, ptLeftTop为菱形格的左顶点)
IBitmap* WBitmap::SubTile(POINT ptLeftTop)
{
	int w, h;
	w = GetWidth(); 
	h = GetHeight();
	if (w%64 != 0 || h%32 != 0)
	{
		FKTraceLn("WBitmap::SubTile bitmap's size is not legal");
		return NULL;
	}

	WBitmap* pSubTile = new WBitmap;
	WPixel clsMask = WPixel(255, 0, 255);
	int nMax = 30;
	WPixel trans[32];
	for (int k=0;k<32;k++) trans[k]=clsMask;
	int nStartX,nStartY;
	if (pSubTile && pSubTile->Create(64, 32))
	{
		for (int i=0; i<32; i++)
		{
			if (nMax) memcpy((*pSubTile)[i], (void*)trans, nMax<<1);
			nStartX = ptLeftTop.x + nMax;
			nStartY = ptLeftTop.y + i;
			if (nStartX<0 && nStartY<0)
			{
				nStartX+=w;
				nStartY+=h;
				memcpy(&(*pSubTile)[i][nMax], &(*this)[nStartY][nStartX], (64-(nMax<<1)));
				memcpy(&(*pSubTile)[i][32], &(*this)[nStartY][0], (64-(nMax<<1)));
			}
			else if (nStartX<0 && nStartY>=0)
			{
				nStartX+=w;
				memcpy(&(*pSubTile)[i][nMax], &(*this)[nStartY][nStartX], (64-(nMax<<1)));
				memcpy(&(*pSubTile)[i][32], &(*this)[nStartY][0], (64-(nMax<<1)));
			}
			else if (nStartX>=0 && nStartY<0)
			{
				nStartY+=h;
				memcpy(&(*pSubTile)[i][nMax], &(*this)[nStartY][nStartX], (64-(nMax<<1))<<1);
			}
			else
			{
				memcpy(&(*pSubTile)[i][nMax], &(*this)[nStartY][nStartX], (64-(nMax<<1))<<1);
			}
			if (nMax) memcpy(&(*pSubTile)[i][64-nMax], (void*)trans, nMax<<1);
			if (i>=15) nMax += 2;
			else nMax -= 2;
		}
	}
	return pSubTile;
}
//------------------------------------------------------------------------
//返回宽度
int WBitmap::GetWidth() const
{
	if(!m_pBmpStruct)
	{
		FKTraceLn("GetWidth on invalid bitmap!");
		return 0;
	}
	
	return ((WBmpStruct*)m_pBmpStruct)->w;
}
//------------------------------------------------------------------------
//返回高度
int WBitmap::GetHeight() const
{
	if(!m_pBmpStruct)
	{
		FKTraceLn("GetHeight on invalid bitmap!");
		return 0;
	}

	return ((WBmpStruct*)m_pBmpStruct)->h;
}
//------------------------------------------------------------------------
//检验位图
BOOL WBitmap::IsValid() const
{
	return (m_pBmpStruct != NULL && ((WBmpStruct*)m_pBmpStruct)->ptr != NULL);
}
//------------------------------------------------------------------------
//返回扫描行
WPixel* WBitmap::operator [](int y) const
{
	//检查传入参数
	if(y < 0 || y >= GetHeight())
	{
		return NULL;
	}

	if(m_pBmpStruct != NULL && ((WBmpStruct*)m_pBmpStruct)->ptr != NULL)
	{
		return (WPixel*)((DWORD)((WBmpStruct*)m_pBmpStruct)->ptr + y*((WBmpStruct*)m_pBmpStruct)->pitch);
	}
	else
	{
		FKTraceLn("Invalid Bitmap on operator[]!");
		return NULL;
	}
}
//------------------------------------------------------------------------
//用户数据
void WBitmap::SetUserData(DWORD userdata)
{
	if(!IsValid())
	{
		FKTraceLn("SetUserData on invalid bitmap!");
		return;
	}

	((WBmpStruct*)m_pBmpStruct)->userdata = userdata;
}
//------------------------------------------------------------------------
DWORD WBitmap::GetUserData() const
{
	if(!m_pBmpStruct)
	{
		FKTraceLn("GetUser Data on invalid bitmap!");
		return 0;
	}
	return ((WBmpStruct*)m_pBmpStruct)->userdata;
}
//------------------------------------------------------------------------
//输入输出
BOOL WBitmap::Load(void* bmpdata)
{
	int width, height;
	WPixel* bmppix;
	BITMAPFILEHEADER* bf = (BITMAPFILEHEADER*)bmpdata;

	if(bf->bfType != 19778) //'MB'
	{
		FKTraceLn("Load bitmap: invalid data!");
		return FALSE;
	}

	BITMAPINFOHEADER* bi = (BITMAPINFOHEADER*)((DWORD)bmpdata + sizeof(BITMAPFILEHEADER));

	
	if(!Create(bi->biWidth, bi->biHeight))
	{
		return FALSE;
	}

	WBmpStruct* data = (WBmpStruct*)GetBmpStruct();

	width = MIN(data->w, (int)bi->biWidth);
	height = MIN(data->h, (int)bi->biHeight);

	//格式转换
	switch(bi->biBitCount)
	{
	case 1: // 两色图片
		{
			WPixel pal[2];
			int p = ((bi->biWidth + 7) & 0xFFFFFFF8) >> 3; // 8字节对齐,每个字节的8个位标识8个象素的索引
			// p 这里为宽度用字节来表达的字节数
			BYTE* bits = (BYTE*)((DWORD)bf + bf->bfOffBits + p*(bi->biHeight - height));
			
			int i, j, usecolor = bi->biClrUsed;
			if (usecolor == 0)
				usecolor = 2;
			
			DWORD* color = (DWORD*)((DWORD)bi + bi->biSize);
			
			for (i=0; i<usecolor; i++)
				pal[i] = color[i];
			
			for (i=0; i<height; i++,bits+=p)
			{
				bmppix = (*this)[height - 1 - i];
				for (j=0; j<width; j+=8)
				{
					for (int k=0; k<8; k++) // 每个字节表达8个象素的索引，最高位指向第一个象素，最低位指向第八个象素
					{
						bmppix[j+k] = pal[(bits[j>>3]>>(7-k)) & 0x1];
					}
				}
			}
		}
		break;
	case 8:	//调色板
		{
			static WPixel pal[256];
			int p = (bi->biWidth + 3) & 0xFFFFFFFC;
			
			BYTE* bits = (BYTE*)((DWORD)bf + bf->bfOffBits + p*(bi->biHeight - height));

			int i, j, usecolor = bi->biClrUsed;
			if (usecolor == 0)
				usecolor = 256;

			DWORD* color = (DWORD*)((DWORD)bi + bi->biSize);

			for (i=0; i<usecolor; i++)
				pal[i] = color[i];

			for (i=0; i<height; i++,bits+=p)
			{
				bmppix = (*this)[height - 1 - i];
				for (j=0; j<width; j++)
					bmppix[j] = pal[bits[j]];
			}
		}
		break;

	case 16:	//16位(555和565)
		{
			if (bi->biSize == sizeof(BITMAPINFOHEADER) + 16) // 565
			{
				int p = ((bi->biWidth*2 + 3) & 0xFFFFFFFC)/2;
				short* bits = (short*)((DWORD)bf + bf->bfOffBits + p*(bi->biHeight - height));
				
				for (int i=0; i<height; i++,bits+=p)
				{
					bmppix = (*this)[height - 1 - i];
					memcpy(bmppix, bits, width*2);
				}
			}
			else // 555
			{
				int p = ((bi->biWidth*2 + 3) & 0xFFFFFFFC)/2;
				WORD* bits = (WORD*)((DWORD)bf + bf->bfOffBits + p*(bi->biHeight - height));
				
				for (int i=0; i<height; i++,bits+=p)
				{
					bmppix = (*this)[height - 1 - i];
					for (int j=0; j<width; j++)
					{
						// 1555->565
						bits[j]=bits[j]&0x7fff;
						bmppix[j].color=((bits[j]<<1)&0xffe0)|(bits[j]&0x1f);
					}
				}
			}
		}
		break;
		
	case 24:	//24位
		{
			int p = (bi->biWidth*3+3)&0xFFFFFFFC;
			DWORD bits = (DWORD)bf + bf->bfOffBits + p*(bi->biHeight - height);

			for (int i=0; i<height; i++,bits+=p)
			{
				bmppix = (*this)[height - 1 - i];
				for (int j=0; j<width; j++)
				{
					DWORD dw = *(DWORD*)(bits + j*3);
					bmppix[j] = *(DWORD*)(bits + j*3);
				}
			}
		}
		break;

	case 32: // 32位
		{
			int p = bi->biWidth;
			DWORD* bits = (DWORD*)((DWORD)bf + bf->bfOffBits + p*(bi->biHeight - height));

			for (int i=0; i<height; i++,bits+=p)
			{
				bmppix = (*this)[height - 1 - i];
				for (int j=0; j<width; j++)
					bmppix[j] = bits[j];
			}
		}
		break;

	default:
		{
			FKTraceLn("Load Bitmap %d is invalid bitcount!", bi->biBitCount);
			return FALSE;
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
BOOL WBitmap::LoadEx(void* customdata)
{
	int width, height;
	WPixel* bmppix;
	RLEHEADER* rlehead = (RLEHEADER*)customdata;
	if (rlehead->wFlags != 'FG')
	{
		FKTraceLn("WBitmap::LoadEx invalid rle head");
		return FALSE;
	}
	
	if(!Create(rlehead->dwWidth, rlehead->dwHeight))
	{
		return FALSE;
	}
	
	WBmpStruct* data = (WBmpStruct*)GetBmpStruct();
	
	width = MIN(data->w, (int)rlehead->dwWidth);
	height = MIN(data->h, (int)rlehead->dwHeight);
	
	//格式转换
	BYTE* bits = (BYTE*)((DWORD)rlehead + rlehead->dwDataOffset);
	
	int i, j, usecolor = rlehead->nUseColor;
	
	WPixel* color = (WPixel*)((DWORD)customdata + sizeof(RLEHEADER));

	int nPos = 0,rep_count;
	WPixel rep_color;
	WPixel color_mask = WPixel(255,0,255);
	for(i = 0; i < height; i ++)
	{
		bmppix = (*this)[i];
		for(j = 0; j < width;)
		{
			switch(bits[nPos]) 
			{
			case 0xFF: // 透明色
				rep_count = bits[nPos+1];
				nPos += 2;
				Memset16Bit(&bmppix[j], color_mask, rep_count);
				j += rep_count;
				break;
			case 0xFE: // 其他重复的颜色
				rep_count = bits[nPos+1];
				rep_color = color[bits[nPos+2]];
				nPos += 3;
				Memset16Bit(&bmppix[j], rep_color, rep_count);
				j += rep_count;
				break;
			default: // 连续的不规则字符
				rep_count = bits[nPos];
				nPos += 1;
				while (rep_count--)
					bmppix[j++] = color[bits[nPos++]];
				break;
			}
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------

BOOL WBitmap::Save(const char* filename) const
{
	if (SaveEx(filename, 24))
		return TRUE;
	if(!IsValid())
	{
		FKTraceLn("Save bitmap: bitmap is invalid!");
		return FALSE;
	}
	
	FILE* f;

	BITMAPFILEHEADER bithead;
	BITMAPINFOHEADER bitinfo;
	int headsize, linesize, i, j, s;
	DWORD color;

	linesize = (GetWidth()*3 + 3) &0xFFFFFFFC;
	s = linesize - GetWidth()*3;

	if((f = fopen(filename, "wb")) == NULL)
	{
		FKTraceLn("SaveBitmap: %s file cannot open!");
		return FALSE;
	}

	headsize = sizeof(bithead) + sizeof(bitinfo);
	bithead.bfType = 'MB';
	bithead.bfSize = headsize + linesize*GetHeight();
	bithead.bfReserved1 = 0;
	bithead.bfReserved2 = 0;
	bithead.bfOffBits = headsize;
	fwrite(&bithead, 1, sizeof(bithead), f);

	bitinfo.biSize			= sizeof(bitinfo);
	bitinfo.biWidth			= GetWidth();
	bitinfo.biHeight		= GetHeight();
	bitinfo.biPlanes		= 1;
	bitinfo.biBitCount		= 24;
	bitinfo.biCompression	= 0;
	bitinfo.biSizeImage		= 0;
	bitinfo.biXPelsPerMeter = 72;
	bitinfo.biYPelsPerMeter = 72;
	bitinfo.biClrUsed		= 0;
	bitinfo.biClrImportant	= 0;
	fwrite(&bitinfo, 1, sizeof(bitinfo), f);

	for(i = GetHeight() - 1; i >= 0; i --)
	{
		for(j = 0; j < GetWidth(); j ++)
		{
			color = (DWORD)(*this)[i][j];
			fwrite(&color, 1, 3, f);
		}
		fwrite(&color, 1, s, f);
	}

	fclose(f);
	return TRUE;

}
//------------------------------------------------------------------------
BOOL WBitmap::SaveEx(const char* filename, int bpp) const
{
	if (!IsValid())
	{
		FKTraceLn("Save bitmap: bitmap is invalid!");
		return FALSE;
	}

	if (bpp < 15 || bpp > 32)
	{
		FKTraceLn("Save bitmap: %dbit is unsupport!", bpp);
		return FALSE;
	}
	
	int w = GetWidth();
	int h = GetHeight();

	FILE* file;
	BITMAPFILEHEADER* pbfh;
	BITMAPINFOHEADER* pbih;
	int filesize, headsize, linesize, datasize;
	int i, j;

	switch (bpp) 
	{
	case 15:
		headsize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		linesize = ((w<<1) + 3) & 0xFFFFFFFC;
		break;
	case 16:
		headsize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 16;
		linesize = ((w<<1) + 3) & 0xFFFFFFFC;
		break;
	case 24:
		headsize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		linesize = ((w<<2) - w + 3) & 0xFFFFFFFC;
		break;
	case 32:
		headsize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 16;
		linesize = w<<2;
		break;
	}
	datasize = linesize * h;
	filesize = headsize + datasize;
	filesize = (filesize + 3) & 0xFFFFFFFC;

	if ((file = fopen(filename, "wb")) == NULL)
	{
		FKTraceLn("SaveBitmap: %s file cannot open!", filename);
		return FALSE;
	}

	BYTE* filebuf = new BYTE[filesize];
	memset(filebuf + filesize - 4, 0, 4);
	
	pbfh = (BITMAPFILEHEADER*)filebuf;
	pbfh->bfType		= 'MB';
	pbfh->bfSize		= filesize;
	pbfh->bfReserved1	= 0;
	pbfh->bfReserved2	= 0;
	pbfh->bfOffBits		= headsize;

	pbih = (BITMAPINFOHEADER*)(filebuf + sizeof(BITMAPFILEHEADER));
	pbih->biSize			= sizeof(BITMAPINFOHEADER);
	pbih->biWidth			= w;
	pbih->biHeight			= h;
	pbih->biPlanes			= 1;
	pbih->biBitCount		= bpp == 15 ? 16 : bpp;
	pbih->biCompression		= bpp == 16 ? 3 : 0;
	pbih->biSizeImage		= 0;
	pbih->biXPelsPerMeter	= 72;
	pbih->biYPelsPerMeter	= 72;
	pbih->biClrUsed			= 0;
	pbih->biClrImportant	= 0;

	switch (bpp) 
	{
	case 15: // 1555,最高位总是置为1
		{
			WORD* pLine = (WORD*)(filebuf + headsize);
			for (i=h-1; i>=0; i--)
			{
				for (j=0; j<w; j++)
				{
					// 565->888,888->1555
					//DWORD color = (*this)[i][j];
					//pLine[j] = (WORD)((color>>9&0xfc00)|(color>>6&0x3e0)|(color>>3&0x1f));
					
					// 565->1555
					WORD& color = (*this)[i][j].color;
					pLine[j] = ((color>>1)&0xffe0)|(color&0x1f);
				}
				pLine = (WORD*)((char*)pLine + linesize);
			}
		}
		break;
	case 16: // 在BITMAPINFOHEADER后面的16个字节中保存R,G,B,A的掩码
		{
			pbih->biSize += 16;
			DWORD* pMask = (DWORD*)(filebuf + headsize - 16);
			*pMask++ = 0xf800;
			*pMask++ = 0x7e0;
			*pMask++ = 0x1f;
			*pMask = 0;
			BYTE* pLine = filebuf + headsize;
			for (i=h-1; i>=0; i--)
			{
				memcpy(pLine, (*this)[i], linesize);
				pLine += linesize;
			}
		}
		break;
	case 24:
		{
			int trail = linesize - (w<<1) - w;
			BYTE* pLine = filebuf + headsize;
			for (i=h-1; i>=0; i--)
			{
				for (j=0; j<w; j++,pLine+=3)
					*(DWORD*)pLine = (*this)[i][j];
				pLine += trail;
			}
		}
		break;
	case 32: // photoshop保存的顺序为abgr
		{
			pbih->biSize += 16;
			DWORD* pMask = (DWORD*)(filebuf + headsize - 16);
			*pMask++ = 0xff; // blue mask
			*pMask++ = 0xff00; // green mask
			*pMask++ = 0xff0000; // red mask
			*pMask = 0; // alpha mask
			int trail = linesize - (w<<2);
			BYTE* pLine = filebuf + headsize;
			for (i=h-1; i>=0; i--)
			{
				for (j=0; j<w; j++,pLine+=4)
					*(DWORD*)pLine = (*this)[i][j];
				pLine += trail;
			}
		}
		break;
	}

	fwrite(filebuf, 1, filesize, file);
	fclose(file);
	delete[] filebuf;
	return TRUE;
}
//------------------------------------------------------------------------
//绘制到DC
BOOL WBitmap::Blit2DC(HDC hDC, int dx, int dy, int x, int y, int w, int h) 
{
	static int BMPINFO16[13] = {
		sizeof(BITMAPINFOHEADER),
		0,  //宽度
		0,  //高度
		0x100001, BI_BITFIELDS, 0, 0, 0, 0, 0,
		0xF800, 0x7E0, 0x1F
	};

	WBmpStruct* data = (WBmpStruct*)GetBmpStruct();
	if(data == 0)
		return TRUE;

	BMPINFO16[1] = data->pitch >> 1;
	BMPINFO16[2] = -h;
	if(SetDIBitsToDevice(hDC, dx, dy, w, h, x, 0, 0, h, (LPVOID)((unsigned)data->ptr + data->pitch*y),
		(BITMAPINFO*)&(BMPINFO16[0]), 0) == 0)
	{
		FKTraceLn("SetDIBBitsToDevice fail!");
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------
extern "C" {
	WSERR _stdcall _clear16(WBmpStruct* bmp,WPixel color);
	WSERR _stdcall _blit16(WBmpStruct *,const WBmpStruct *);
	WSERR _stdcall _mask_blit16(WBmpStruct *,const WBmpStruct *);
	WSERR _stdcall _additive_blit16(WBmpStruct *,const WBmpStruct *);
	WSERR _stdcall _subtract_blit16(WBmpStruct *,const WBmpStruct *);
	WSERR _stdcall _trans_blit16(WBmpStruct *,const WBmpStruct *);
	WSERR _stdcall _mask_trans_blit16(WBmpStruct *,const WBmpStruct *);
	WSERR _stdcall _back_blit16(WBmpStruct *,const WBmpStruct *);
}
//------------------------------------------------------------------------
static BLIT_FUNC _blit_func[16]={
	(BLIT_FUNC)_blit16,					// BLIT_COPY
	(BLIT_FUNC)_additive_blit16,			// BLIT_ADDITIVE
	(BLIT_FUNC)_subtract_blit16,			// BLIT_SUBTRACT
	(BLIT_FUNC)_trans_blit16,				// BLIT_TRANS
	(BLIT_FUNC)_mask_blit16,				// BLIT_MASK
	(BLIT_FUNC)_back_blit16,				// BLIT_BACK
	0,
	(BLIT_FUNC)_mask_trans_blit16,			// BLIT_MASK|BLIT_TRANS
	0,0,0,0,0,0,0,0
};
//------------------------------------------------------------------------
/*
static BLIT_FUNC blit_func[16]={
	(BLIT_FUNC)blit16,					// BLIT_COPY
	(BLIT_FUNC)additive_blit16,			// BLIT_ADDITIVE
	(BLIT_FUNC)subtract_blit16,			// BLIT_SUBTRACT
	(BLIT_FUNC)trans_blit16,			// BLIT_TRANS
	(BLIT_FUNC)mask_blit16,				// BLIT_MASK
	0,									// BLIT_BACK
	0,
	(BLIT_FUNC)mask_trans_blit16,			// BLIT_MASK|BLIT_TRANS
	0,0,0,0,0,0,0,0
};*/
extern "C" {
	WSERR _stdcall _alpha_blit16(WBmpStruct* ,const WAlphaBmpStruct *);
	WSERR _stdcall _alpha_trans_blit16(WBmpStruct* ,const WAlphaBmpStruct *);
}
//------------------------------------------------------------------------
static BLIT_FUNC _alpha_blit_func[8]={
	(BLIT_FUNC)_alpha_blit16,				// BLIT_COPY
	0,
	0,
	(BLIT_FUNC)_alpha_trans_blit16,				// BLIT_TRANS
	(BLIT_FUNC)_alpha_blit16,
	0,
	0,
	(BLIT_FUNC)_alpha_trans_blit16
};
//------------------------------------------------------------------------
/*
static BLIT_FUNC _alpha_blit_func[8]={
	(BLIT_FUNC)alpha_blit16,				// BLIT_COPY
	0,
	0,
	(BLIT_FUNC)alpha_trans_blit16,				// BLIT_TRANS
	(BLIT_FUNC)alpha_blit16,
	0,
	0,
	(BLIT_FUNC)alpha_trans_blit16
};*/
//------------------------------------------------------------------------
//清屏
BOOL WBitmap::Clear(WPixel color, RECT* pRc)
{
	if(!IsValid())
		return TRUE;
	SetPos(0, 0);
	
	if(!pRc)
		return _clear16((WBmpStruct*)GetBmpStruct(), color) == 0;
	else
	{
		WBmpStruct bmpStruct;

		int width = pRc->right - pRc->left;
		int height = pRc->bottom - pRc->top;
		int x = pRc->left;
		int y = pRc->top;
		
		if (x < 0)
		{
			width += x;
			x = 0;
		}

		if (pRc->top < 0)
		{
			height += y;
			y = 0;
		}
		
		bmpStruct.kx = 0;
		bmpStruct.ky = 0;
		bmpStruct.pitch = ((WBmpStruct*)m_pBmpStruct)->pitch;
		bmpStruct.w = MIN(width, ((WBmpStruct*)m_pBmpStruct)->w - x);
		bmpStruct.h = MIN(height, ((WBmpStruct*)m_pBmpStruct)->h - y);
		bmpStruct.ptr = (void*)((DWORD)((WBmpStruct*)m_pBmpStruct)->ptr + ((WBmpStruct*)m_pBmpStruct)->pitch*y + x*2);
		bmpStruct.userdata = ((WBmpStruct*)m_pBmpStruct)->userdata;
		bmpStruct.bChild = TRUE;

		return _clear16(&bmpStruct, color) == 0;
	}
}
//------------------------------------------------------------------------
//绘制
BOOL WBitmap::Draw(int x, int y, IBitmap* bmp, DWORD dwStyle)
{
	if(dwStyle >= 16)
		return FALSE;

	BLIT_FUNC blit = _blit_func[dwStyle];
	if(blit == 0)
	{
		FKTraceLn("Invalid blit style!");
		return FALSE;
	}

	if(!IsValid())
	{
		FKTraceLn("Bitmap Draw is invalid!");
		return FALSE;
	}

	int oldx = ((WBmpStruct*)m_pBmpStruct)->kx;
	int oldy = ((WBmpStruct*)m_pBmpStruct)->ky;
	SetPos(x, y);

	BOOL ret = ( blit((DWORD)GetBmpStruct(), (DWORD)bmp->GetBmpStruct()) == 0);

	((WBmpStruct*)m_pBmpStruct)->kx = oldx;
	((WBmpStruct*)m_pBmpStruct)->ky = oldy;

	return ret;
}
//------------------------------------------------------------------------
void WBitmap::SetPos(int x, int y)
{
	ASSERT(m_pBmpStruct != NULL);
	((WBmpStruct*)m_pBmpStruct)->kx = x;
	((WBmpStruct*)m_pBmpStruct)->ky = y; 
}
//------------------------------------------------------------------------
//打印ASCII字符，只能用于输出测试信息
extern "C" void _stdcall _draw_ascii(WBmpStruct *bmp,char c);
static WSERR _draw_str(int x, int y, WBmpStruct* bmp, const char* s)
{
	int i;
	int oldx = bmp->kx, oldy = bmp->ky;
	bmp->kx = x;
	bmp->ky = y;

	for(i = 0; s[i]; i ++, bmp->kx += 7)
	{
		if(s[i] < 33)
			continue;
		_draw_ascii(bmp, s[i]);
	}
	bmp->kx = oldx;
	bmp->ky = oldy;

	return 0;	
}
//------------------------------------------------------------------------
void WBitmap::Draw(int x, int y, const char* s, WPixel color)
{
	SetUserData(color.color);
	_draw_str(x, y, (WBmpStruct*)m_pBmpStruct, s);
}
//------------------------------------------------------------------------
extern "C" {
	WSERR _stdcall _box16(WBmpStruct* bmp,const WBoxStruct *);
	WSERR _stdcall _trans_box16(WBmpStruct *,const WBoxStruct *);
}
//------------------------------------------------------------------------
static BLIT_FUNC _box_func[4]={
	(BLIT_FUNC)_box16,						// BLIT_COPY
	0,0,
	(BLIT_FUNC)_trans_box16,				// BLIT_TRANS
};
//------------------------------------------------------------------------
/*
static BLIT_FUNC box_func[4]={
	(BLIT_FUNC)box16,						// BLIT_COPY
	0,0,
	(BLIT_FUNC)trans_box16,				// BLIT_TRANS
};*/
//------------------------------------------------------------------------
BOOL WBitmap::DrawBox(RECT& rc, WPixel clr, DWORD dwStyle /*= BLIT_NORMAL*/, int alpha/* = 255*/)
{
	if(!IsValid())
	{
		FKTraceLn("WBitmap DrawBox, but the bitmap is invalid!");
		return FALSE;
	}


	ASSERT(dwStyle < 4);
	WBoxStruct box;
	box.color = clr;

	RECT myrc = rc;
	if (myrc.left > myrc.right)
	{
		int nTmp = myrc.left;
		myrc.left = myrc.right;
		myrc.right = nTmp;
	}
	if (myrc.top > myrc.bottom)
	{
		int nTmp = myrc.top;
		myrc.top = myrc.bottom;
		myrc.bottom = nTmp;
	}
	int oldx = ((WBmpStruct*)m_pBmpStruct)->kx = myrc.left;
	int oldy = ((WBmpStruct*)m_pBmpStruct)->ky = myrc.top;
	SetPos(myrc.left, myrc.top);

	box.kx = 0;
	box.ky = 0;
	box.w = myrc.right - myrc.left;
	box.h = myrc.bottom - myrc.top;
	box.userdata = alpha;
	if (box.w == 0 || box.h == 0) return FALSE;

	BOOL ret =  (_box_func[dwStyle]((DWORD)m_pBmpStruct, (DWORD)&box) == 0);
	((WBmpStruct*)m_pBmpStruct)->kx = oldx;
	((WBmpStruct*)m_pBmpStruct)->ky = oldy;
	return ret;
}
//------------------------------------------------------------------------
//绘制直线的函数
extern WSERR _stdcall _draw_hline_solid(WBmpStruct *bmp,const WLineStruct *line);
extern WSERR _stdcall _draw_hline_trans(WBmpStruct *bmp,const WLineStruct *line);
extern WSERR _stdcall _draw_hline_xor(WBmpStruct *bmp,const WLineStruct *line);
extern WSERR _stdcall _draw_line_solid(WBmpStruct *bmp,const WLineStruct *line);
extern WSERR _stdcall _draw_line_trans(WBmpStruct *bmp,const WLineStruct *line);
extern WSERR _stdcall _draw_line_xor(WBmpStruct *bmp,const WLineStruct *line);
extern WSERR _stdcall _draw_vline_solid(WBmpStruct *bmp,const WLineStruct *line);
extern WSERR _stdcall _draw_vline_trans(WBmpStruct *bmp,const WLineStruct *line);
extern WSERR _stdcall _draw_vline_xor(WBmpStruct *bmp,const WLineStruct *line);
//------------------------------------------------------------------------
BOOL WBitmap::DrawLine(POINT& ptFrom, POINT& ptTo, WPixel clr, DWORD dwStyle/* = BLIT_NORMAL*/, int alpha/* = 255*/)
{
	if(!IsValid())
	{
		FKTraceLn("WBitmap line, but the bitmap is invalid!");
		return FALSE;
	}
	WBmpStruct* bmp = (WBmpStruct*)m_pBmpStruct;
	
	int oldx = bmp->kx;
	int oldy = bmp->ky;

	SetPos(ptFrom.x, ptFrom.y);
	
	//设置WLineStruct
	WLineStruct line;
	line.color = clr;	
	line.userdata = alpha;
	
	int width = ptTo.x - ptFrom.x ;
	int height = ptTo.y - ptFrom.y ;

	if(width >= 0)
	{
		line.kx = 0;
		line.x = width;
		line.positive = TRUE;
	}
	else
	{
		line.kx = -width;
		line.x = -width;
		line.positive = FALSE;
	}
	if(height >= 0)
	{
		line.ky = 0;
		line.y = height;
	}
	else
	{
		line.ky = -height;
		line.y = -height;
		line.positive = !line.positive;
	}
	switch(dwStyle)
	{
	case BLIT_TRANS:
		if(line.y == 0)
			return !_draw_hline_trans(bmp, &line);
		if(line.x == 0)
			return !_draw_vline_trans(bmp, &line);
		return !_draw_line_trans(bmp, &line);
		

	case BLIT_XOR:
		if(line.y == 0)
			return !_draw_hline_xor(bmp, &line);
		if(line.x == 0)
			return !_draw_vline_xor(bmp, &line);
		return !_draw_line_xor(bmp, &line);

	default:
		if(line.y == 0)
			return !_draw_hline_solid(bmp, &line);
		if(line.x == 0)
			return !_draw_vline_solid(bmp, &line);
		return _draw_line_solid(bmp, &line);
	}
	
	SetPos(oldx, oldy);
}
//------------------------------------------------------------------------
BOOL WBitmap::Draw(int dx, int dy, int w, int h, IBitmap* bmp, int sx, int sy, DWORD dwStyle)
{
	if(dwStyle >= 16)
		return FALSE;

	if (w <= 0 || h <= 0)
		return TRUE;

	BLIT_FUNC blit = _blit_func[dwStyle];
	if(blit == 0)
	{
		FKTraceLn("Invalid blit style!");
		return FALSE;
	}

	if(!IsValid())
	{
		FKTraceLn("Bitmap Draw is invalid!");
		return FALSE;
	}

	int oldx = ((WBmpStruct*)m_pBmpStruct)->kx;
	int oldy = ((WBmpStruct*)m_pBmpStruct)->ky;
	SetPos(dx, dy);

	WBmpStruct bmpStruct;
	memcpy(&bmpStruct, bmp->GetBmpStruct(), sizeof(WBmpStruct));
	bmpStruct.kx = 0;
	bmpStruct.ky = 0;
	bmpStruct.w = MIN(w, bmpStruct.w - sx);
	bmpStruct.h = MIN(h, bmpStruct.h - sy);
	bmpStruct.ptr = (void*)((DWORD)bmpStruct.ptr + sy*bmpStruct.pitch + sx*2);
	bmpStruct.bChild = TRUE;
	if (bmpStruct.w <= 0 || bmpStruct.h <= 0)
		return TRUE;

#ifdef _DEBUG
	WBmpStruct* ptmp1 = (WBmpStruct*)m_pBmpStruct;
	WBmpStruct* ptmp2 = (WBmpStruct*)(bmp->GetBmpStruct());
#endif

	BOOL ret = ( blit((DWORD)GetBmpStruct(), (DWORD)(&bmpStruct)) == 0);

	((WBmpStruct*)m_pBmpStruct)->kx = oldx;
	((WBmpStruct*)m_pBmpStruct)->ky = oldy;

	return ret;
}
//------------------------------------------------------------------------
BOOL WBitmap::DrawAlpha(int dx, int dy, int w, int h, IAlphaBitmap* bmp, int sx, int sy, DWORD dwStyle)
{/*
	// src
	WAlphaBmpStruct* tgainfo= (WAlphaBmpStruct*)bmp->GetBmpStruct();
	// dest
	WBmpStruct* bmpinfo = (WBmpStruct*)GetBmpStruct();
	// param
	DWORD iSrcPitch = tgainfo->pitch;
	DWORD iAlpPitch = iSrcPitch / 2;
	int nOffset = sy*iAlpPitch + sx;
	const BYTE* pAlpha = tgainfo->channel + nOffset;
	const WORD* pSrc = (const WORD*)((DWORD)(tgainfo->ptr) + nOffset*2);
	DWORD dwSrcW = MIN(w, tgainfo->w - sx);
	DWORD dwSrcH = MIN(h, tgainfo->h - sy);
	
	DWORD uDestPitch = bmpinfo->pitch;
	WORD* pDest = (WORD*)((DWORD)(bmpinfo->ptr) + dy*uDestPitch + dx*2);
	DWORD dwDstW = MIN(w, bmpinfo->w - dx);
	DWORD dwDstH = MIN(h, bmpinfo->h - dy);

	DWORD uDestW = MIN(dwSrcW, dwDstW);
	DWORD uDestH = MIN(dwSrcH, dwDstH);
	
	__AlphaBlend565(pAlpha, iAlpPitch, pSrc, iSrcPitch, 
		pDest, uDestW, uDestH, uDestPitch);

	return TRUE;*/
	
	if(dwStyle >= 16)
		return FALSE;

	if (w <= 0 || h <= 0)
		return TRUE;

	BLIT_FUNC blit = _alpha_blit_func[dwStyle];
	if(blit == 0)
	{
		FKTraceLn("Invalid blit style!");
		return FALSE;
	}

	if(!IsValid())
	{
		FKTraceLn("Bitmap Draw is invalid!");
		return FALSE;
	}

	int oldx = ((WBmpStruct*)m_pBmpStruct)->kx;
	int oldy = ((WBmpStruct*)m_pBmpStruct)->ky;
	SetPos(dx, dy);

	WAlphaBmpStruct bmpStruct;
	memcpy(&bmpStruct, bmp->GetBmpStruct(), sizeof(WAlphaBmpStruct));
	bmpStruct.kx = 0;
	bmpStruct.ky = 0;
	bmpStruct.w = MIN(w, bmpStruct.w - sx);
	bmpStruct.h = MIN(h, bmpStruct.h - sy);
	bmpStruct.ptr = (void*)((DWORD)bmpStruct.ptr + sy*bmpStruct.pitch + sx*2);
	bmpStruct.channel = bmpStruct.channel + sy*bmpStruct.pitch/2 + sx;
	bmpStruct.bChild = TRUE;
	if (bmpStruct.w <= 0 || bmpStruct.h <= 0)
		return TRUE;

#ifdef _DEBUG
	WBmpStruct* ptmp1 = (WBmpStruct*)m_pBmpStruct;
	WAlphaBmpStruct* ptmp2 = (WAlphaBmpStruct*)(bmp->GetBmpStruct());
#endif

	//BOOL ret = alpha_blit16((WBmpStruct*)m_pBmpStruct, &bmpStruct);
	BOOL ret = ( blit((DWORD)GetBmpStruct(), (DWORD)(&bmpStruct)) == 0);

	((WBmpStruct*)m_pBmpStruct)->kx = oldx;
	((WBmpStruct*)m_pBmpStruct)->ky = oldy;

	return ret;
}
//------------------------------------------------------------------------
BOOL WBitmap::DrawAlpha(int x, int y, IAlphaBitmap* bmp, DWORD dwStyle)
{/*
	// src
	WAlphaBmpStruct* tgainfo= (WAlphaBmpStruct*)bmp->GetBmpStruct();
	// dest
	WBmpStruct* bmpinfo = (WBmpStruct*)GetBmpStruct();
	// param
	DWORD iSrcPitch = tgainfo->pitch;
	DWORD iAlpPitch = iSrcPitch / 2;
	BYTE* pAlpha = tgainfo->channel;
	WORD* pSrc = (WORD*)((DWORD)(tgainfo->ptr));
	
	DWORD uDestPitch = bmpinfo->pitch;
	WORD* pDest = (WORD*)((DWORD)(bmpinfo->ptr) + y*uDestPitch + x*2);
	DWORD uDestW = MIN(tgainfo->w, bmpinfo->w - x);
	DWORD uDestH = MIN(tgainfo->h, bmpinfo->h - y);
	
	//__AlphaBlend565(pAlpha, iAlpPitch, pSrc, iSrcPitch, pDest, uDestW, uDestH, uDestPitch);
	//ablend_565(pAlpha, iAlpPitch, (BYTE*)pSrc, 0, 0, iSrcPitch, (BYTE*)pDest, 0, 0, uDestW, uDestH, uDestPitch);
	alpha_blit16_bycplusplus(pAlpha, iAlpPitch, (BYTE*)pSrc, 0, 0, iSrcPitch, (BYTE*)pDest, 400, 0, uDestW, uDestH, uDestPitch);
	//return TRUE;
*/
	
	if(dwStyle >= 16)
		return FALSE;

	BLIT_FUNC blit = _alpha_blit_func[dwStyle];
	if(blit == 0)
	{
		FKTraceLn("Invalid blit style!");
		return FALSE;
	}

	if(!IsValid())
	{
		FKTraceLn("Bitmap Draw is invalid!");
		return FALSE;
	}

	int oldx = ((WBmpStruct*)m_pBmpStruct)->kx;
	int oldy = ((WBmpStruct*)m_pBmpStruct)->ky;
	SetPos(x, y);

	WBmpStruct* ptmp1 = (WBmpStruct*)m_pBmpStruct;
	WAlphaBmpStruct* ptmp2 = (WAlphaBmpStruct*)(bmp->GetBmpStruct());
	ptmp2->kx = 0;
	ptmp2->ky = 0;

	BOOL ret = ( blit((DWORD)GetBmpStruct(), (DWORD)bmp->GetBmpStruct()) == 0);
	//BOOL ret = (alpha_blit16_modify(ptmp1, ptmp2) == 0);

	((WBmpStruct*)m_pBmpStruct)->kx = oldx;
	((WBmpStruct*)m_pBmpStruct)->ky = oldy;

	return ret;
}
//------------------------------------------------------------------------
void BlitAlpha(WPixel* dst, WPixel* src, int alpha)
{
	
	//dst->red = (alpha*src->red+(255-alpha)*dst->red)>>8;
	//dst->green = (alpha*src->green+(255-alpha)*dst->green)>>8;
	//dst->blue = (alpha*src->blue+(255-alpha)*dst->blue)>>8;


	dst->red = src->red + (alpha*dst->red>>8);
	dst->green = src->green + (alpha*dst->green>>8);
	dst->blue = src->blue + (alpha*dst->blue>>8);


	return;

	WORD sTemp = src->color;
	WORD dTemp = dst->color; // 目标点

	int red, green, blue;
	int sb,db,sg,dg,sr,dr;

	sb = sTemp & 0x1f;// 蓝色分量
	db = dTemp & 0x1f; 

	sg = (sTemp >> 5) & 0x3f; // 绿色分量
	dg = (dTemp >> 5) & 0x3f; 

	sr = (sTemp >> 11) & 0x1f;// 红色分量
	dr = (dTemp >> 11) & 0x1f;

	blue = (alpha * (sb - db) >> 8) + db;// 按照改进公式2运算三个分量
	green = (alpha * (sg - dg) >> 8) + dg; 
	red = (alpha * (sr - dr) >> 8) + dr; 
	dst->color = blue | (green << 5) | (red << 11); // 565格式生成结果并赋给目标点
}
//------------------------------------------------------------------------
void WBitmap::DrawRleFunc(int dx, int dy, int w, int h, BYTE* bits)
{
	WPixel* bmppix;int i, j, rep_count;
	int nPos = 0;
	WPixel rep_color;
	WPixel color_mask = WPixel(255,0,255);
	
	color_mask = 0;
	int alphasrc;
	for(i = 0; i < h; i ++)
	{
		bmppix = (*this)[i+dy] + dx;
		nPos +=2;
		for(j = 0; j < w;)
		{
			if (bits[nPos] < 0x80) // 透明区
			{
				j += bits[nPos++] + 1;
			}
			else // 不规则序列
			{
				rep_count = bits[nPos++] - 0x80 + 1;
				for (int k=0; k<rep_count; k++)
				{
					rep_color = *(WPixel*)&bits[nPos+2*k];
					alphasrc = bits[nPos+2*rep_count+k];
					BlitAlpha(&bmppix[j++],&rep_color,alphasrc);
				}
				nPos += (rep_count<<1)+rep_count;
			}
		}
	}//
}
//------------------------------------------------------------------------
// 把别的rle位图绘制到此位图
BOOL WBitmap::DrawRle(int dx, int dy, IRleBitmap* bmp, DWORD dwStyle)
{
	if(!IsValid() || !bmp->IsValid())
	{
		FKTraceLn("Bitmap DrawRle is invalid!");
		return FALSE;
	}
	
	WBmpStruct* dest = (WBmpStruct*)m_pBmpStruct;
	WRleBmpStruct* src = (WRleBmpStruct*)bmp->GetBmpStruct();
	
	int sx=0,sy=0,w = src->rleheader.dwWidth,h = src->rleheader.dwHeight;
	bool bClip = false;

	// 裁剪
	if (dx+w-1 < 0 || dx > dest->w || dy+h-1 < 0|| dy > dest->h) return TRUE;
	if (dx < 0)	w+=dx,sx-=dx,dx=0,bClip=true;
	if (dy < 0) h+=dy,sy-=dy,dy=0,bClip=true;
	if (dx+w > dest->w) w=dest->w-dx,bClip=true;
	if (dy+h > dest->h) h=dest->h-dy,bClip=true;
	if (w <= 0 || h <= 0)
		return TRUE;
	
	int pitch=dest->pitch;
	int oldx = dest->kx;
	int oldy = dest->ky;
	SetPos(dx, dy);

	//格式转换
	BYTE* bits = bmp->GetRleData();
	WPixel* color = (WPixel*)(bmp->GetPalette());
	
	if (bClip) // 有裁减blit
	{
		DrawRle(dx, dy, w, h, bmp, sx, sy, dwStyle);
	}
	else // 无裁减blit
	{
		int nPos = 0;
		WPixel rep_color;
		WPixel color_mask = WPixel(255,0,255);
		
		if (bmp->HaveAlphaChannel()) // 有alpha通道
		{
			if (bmp->IsHiColor())
			{
				blit_rle_alpha16(dx, dy, w, h, pitch, dest->ptr, bits);
				//DrawRleFunc(dx, dy, w, h, bits);
			}
			else
			{
				switch (dwStyle) 
				{
				case BLIT_COPY:case BLIT_MASK:
					blit_rle_alpha(dx, dy, w, h, pitch, dest->ptr, color, bits);
					break;
				case BLIT_TRANS|BLIT_COPY:case BLIT_TRANS|BLIT_MASK:
					blit_rle_alpha_trans(dx, dy, w, h, pitch, dest->ptr, color, bits, src->userdata);
					break;
				default:
					break;
				}
			}
		}
		else // 无alpha通道
		{
			switch(dwStyle)
			{
			case BLIT_COPY:
				blit_rle_copy(dx, dy, w, h, pitch, dest->ptr, color, bits);break;
			case BLIT_MASK:
				blit_rle_mask(dx, dy, w, h, pitch, dest->ptr, color, bits);break;
			case BLIT_COPY|BLIT_TRANS:
				blit_rle_copy_trans(dx, dy, w, h, pitch, dest->ptr, color, bits, src->userdata);break;
			case BLIT_MASK|BLIT_TRANS:
				blit_rle_mask_trans(dx, dy, w, h, pitch, dest->ptr, color, bits, src->userdata);break;
			default:
				break;
			}		
		}
	}
	
	dest->kx = oldx;
	dest->ky = oldy;
	
	return TRUE;
}
//------------------------------------------------------------------------
// 把别的rle位图绘制到此位图(外界指定调色板)
BOOL WBitmap::DrawRle(int dx, int dy, IRleBitmap* bmp, BYTE* pPal, DWORD dwStyle)
{
	if(!IsValid() || !bmp->IsValid())
	{
		FKTraceLn("Bitmap DrawRle is invalid!");
		return FALSE;
	}
	
	WBmpStruct* dest = (WBmpStruct*)m_pBmpStruct;
	WRleBmpStruct* src = (WRleBmpStruct*)bmp->GetBmpStruct();
	
	int sx=0,sy=0,w = src->rleheader.dwWidth,h = src->rleheader.dwHeight;
	bool bClip = false;

	// 裁剪
	if (dx+w-1 < 0 || dx > dest->w || dy+h-1 < 0|| dy > dest->h) return TRUE;
	if (dx < 0)	w+=dx,sx-=dx,dx=0,bClip=true;
	if (dy < 0) h+=dy,sy-=dy,dy=0,bClip=true;
	if (dx+w > dest->w) w=dest->w-dx,bClip=true;
	if (dy+h > dest->h) h=dest->h-dy,bClip=true;
	if (w <= 0 || h <= 0)
		return TRUE;
	
	int pitch=dest->pitch;
	int oldx = dest->kx;
	int oldy = dest->ky;
	SetPos(dx, dy);

	//格式转换
	BYTE* bits = bmp->GetRleData();
	WPixel* color = (WPixel*)pPal; // 取用外界调色板
	
	if (bClip) // 有裁减blit
	{
		if (bmp)
			DrawRle(dx, dy, w, h, bmp, pPal, sx, sy, dwStyle);
	}
	else // 无裁减blit
	{
		int nPos = 0;
		WPixel rep_color;
		WPixel color_mask = WPixel(255,0,255);
		
		if (bmp->HaveAlphaChannel()) // 有alpha通道
		{
			if (bmp->IsHiColor())
				//DrawRleFunc(dx, dy, w, h, bits);
				blit_rle_alpha16(dx, dy, w, h, pitch, dest->ptr, bits);
			else
			{
				//blit_rle_alpha(dx, dy, w, h, pitch, dest->ptr, color, bits);
				switch (dwStyle) 
				{
				case BLIT_COPY:case BLIT_MASK:
					blit_rle_alpha(dx, dy, w, h, pitch, dest->ptr, color, bits);
					break;
				case BLIT_TRANS|BLIT_COPY:case BLIT_TRANS|BLIT_MASK:
					blit_rle_alpha_trans(dx, dy, w, h, pitch, dest->ptr, color, bits, src->userdata);
					break;
				default:
					break;
				}
			}
		}
		else // 无alpha通道
		{
			switch(dwStyle)
			{
			case BLIT_COPY:
				blit_rle_copy(dx, dy, w, h, pitch, dest->ptr, color, bits);break;
			case BLIT_MASK:
				blit_rle_mask(dx, dy, w, h, pitch, dest->ptr, color, bits);break;
			case BLIT_COPY|BLIT_TRANS:
				blit_rle_copy_trans(dx, dy, w, h, pitch, dest->ptr, color, bits, src->userdata);break;
			case BLIT_MASK|BLIT_TRANS:
				blit_rle_mask_trans(dx, dy, w, h, pitch, dest->ptr, color, bits, src->userdata);break;
			default:
				break;
			}		
		}
	}
	
	dest->kx = oldx;
	dest->ky = oldy;
	
	return TRUE;
}
//------------------------------------------------------------------------
// 增强的rle绘制
BOOL WBitmap::DrawRle(int dx, int dy, int w, int h, IRleBitmap* bmp, int sx, int sy, DWORD dwStyle)
{
	if (w <= 0 || h <= 0)
		return TRUE;

	if(!IsValid() || !bmp->IsValid())
	{
		FKTraceLn("Bitmap DrawRle is invalid!");
		return FALSE;
	}

	WBmpStruct* dest = (WBmpStruct*)m_pBmpStruct;
	WRleBmpStruct* src = (WRleBmpStruct*)bmp->GetBmpStruct();

	// 裁剪
	if (sx > (int)src->rleheader.dwWidth || sy > (int)src->rleheader.dwHeight ||
		sx+w-1 < 0 || sy+h-1 < 0 ||
		dx+w-1 < 0 || dx > dest->w || dy+h-1 < 0|| dy > dest->h) return TRUE;
	if (sx<0) sx=0,w+=sx;
	if (sy<0) sy=0,h+=sy;
	if (dx < 0)	w+=dx,sx-=dx,dx=0;
	if (dy < 0) h+=dy,sy-=dy,dy=0;
	if (dx+w > dest->w) w=dest->w-dx;
	if (dy+h > dest->h) h=dest->h-dy;
	if (sx+w > (int)src->rleheader.dwWidth) w=(int)src->rleheader.dwWidth-sx;
	if (sy+h > (int)src->rleheader.dwHeight) h=(int)src->rleheader.dwHeight-sy;
	if (w <= 0 || h <= 0)
		return TRUE;
	
	int pitch=dest->pitch;
	int oldx = dest->kx;
	int oldy = dest->ky;
	SetPos(dx, dy);
	
	//格式转换
	int nSrcWidth = bmp->GetWidth();
	int nSrcHeight = bmp->GetHeight();
	BYTE* bits = bmp->GetRleData();
	WPixel* color = (WPixel*)bmp->GetPalette();
	w=MIN(w, nSrcWidth-sx);
	h=MIN(h, nSrcHeight-sy);
	RECT clip;
	SetRect(&clip, sx, sy, sx+w-1, sy+h-1);

	WPixel rep_color;
	WPixel color_mask = WPixel(255,0,255);
	
	if (bmp->HaveAlphaChannel()) // 有alpha通道
	{
		if (bmp->IsHiColor())
			//DrawRleFunc(dx, dy, w, h, bits);
			blit_rle_clip_alpha16(dx, dy, nSrcWidth, nSrcHeight, &clip, pitch, dest->ptr, bits);
		else
		{
			switch (dwStyle) 
			{
			case BLIT_COPY:case BLIT_MASK:
				blit_rle_clip_alpha(dx, dy, nSrcWidth, nSrcHeight, &clip, pitch, dest->ptr, color, bits);
				break;
			case BLIT_TRANS|BLIT_COPY:case BLIT_TRANS|BLIT_MASK:
				blit_rle_clip_alpha_trans(dx, dy, nSrcWidth, nSrcHeight, &clip, pitch, dest->ptr, color, bits, src->userdata);
				break;
			default:
				break;
			}
		}
	}
	else // 无alpha通道数据
	{
		switch(dwStyle)
		{
		case BLIT_COPY:
			blit_rle_clip_copy(dx,dy,nSrcWidth,nSrcHeight,clip,pitch,dest->ptr,color,bits);break;
		case BLIT_MASK:
			blit_rle_clip_mask(dx,dy,nSrcWidth,nSrcHeight,clip,pitch,dest->ptr,color,bits);break;
		case BLIT_COPY|BLIT_TRANS:
			blit_rle_clip_copy_trans(dx,dy,nSrcWidth,nSrcHeight,clip,pitch,dest->ptr,color,bits, src->userdata);break;
		case BLIT_MASK|BLIT_TRANS:
			blit_rle_clip_mask_trans(dx,dy,nSrcWidth,nSrcHeight,clip,pitch,dest->ptr,color,bits, src->userdata);break;
		}
	}
	dest->kx = oldx;
	dest->ky = oldy;
	
	return TRUE;
}
//------------------------------------------------------------------------
// 增强的rle绘制(外界指定调色板)
BOOL WBitmap::DrawRle(int dx, int dy, int w, int h, IRleBitmap* bmp, BYTE* pPal, int sx, int sy, DWORD dwStyle)
{
	if (w <= 0 || h <= 0)
		return TRUE;
	
	if(!IsValid() || !bmp->IsValid())
	{
		FKTraceLn("Bitmap DrawRle is invalid!");
		return FALSE;
	}
	
	WBmpStruct* dest = (WBmpStruct*)m_pBmpStruct;
	WRleBmpStruct* src = (WRleBmpStruct*)bmp->GetBmpStruct();
	
	// 裁剪
	if (sx > (int)src->rleheader.dwWidth || sy > (int)src->rleheader.dwHeight ||
		sx+w-1 < 0 || sy+h-1 < 0 ||
		dx+w-1 < 0 || dx > dest->w || dy+h-1 < 0|| dy > dest->h) return TRUE;
	if (sx<0) sx=0,w+=sx;
	if (sy<0) sy=0,h+=sy;
	if (dx < 0)	w+=dx,sx-=dx,dx=0;
	if (dy < 0) h+=dy,sy-=dy,dy=0;
	if (dx+w > dest->w) w=dest->w-dx;
	if (dy+h > dest->h) h=dest->h-dy;
	if (sx+w > (int)src->rleheader.dwWidth) w=(int)src->rleheader.dwWidth-sx;
	if (sy+h > (int)src->rleheader.dwHeight) h=(int)src->rleheader.dwHeight-sy;
	if (w <= 0 || h <= 0)
		return TRUE;
	
	int pitch=dest->pitch;
	int oldx = dest->kx;
	int oldy = dest->ky;
	SetPos(dx, dy);
	
	//格式转换
	int nSrcWidth = bmp->GetWidth();
	int nSrcHeight = bmp->GetHeight();
	BYTE* bits = bmp->GetRleData();
	WPixel* color = (WPixel*)pPal;
	w=MIN(w, nSrcWidth-sx);
	h=MIN(h, nSrcHeight-sy);
	RECT clip;
	SetRect(&clip, sx, sy, sx+w-1, sy+h-1);
	
	WPixel rep_color;
	WPixel color_mask = WPixel(255,0,255);
	
	if (bmp->HaveAlphaChannel()) // 有alpha通道
	{
		if (bmp->IsHiColor())
			//DrawRleFunc(dx, dy, w, h, bits);
			blit_rle_clip_alpha16(dx, dy, nSrcWidth, nSrcHeight, &clip, pitch, dest->ptr, bits);
		else
		{
			switch (dwStyle) 
			{
			case BLIT_COPY:case BLIT_MASK:
				blit_rle_clip_alpha(dx, dy, nSrcWidth, nSrcHeight, &clip, pitch, dest->ptr, color, bits);
				break;
			case BLIT_TRANS|BLIT_COPY:case BLIT_TRANS|BLIT_MASK:
				blit_rle_clip_alpha_trans(dx, dy, nSrcWidth, nSrcHeight, &clip, pitch, dest->ptr, color, bits, src->userdata);
				break;
			default:
				break;
			}
		}
	}
	else // 无alpha通道数据
	{
		switch(dwStyle)
		{
		case BLIT_COPY:
			blit_rle_clip_copy(dx,dy,nSrcWidth,nSrcHeight,clip,pitch,dest->ptr,color,bits);break;
		case BLIT_MASK:
			blit_rle_clip_mask(dx,dy,nSrcWidth,nSrcHeight,clip,pitch,dest->ptr,color,bits);break;
		case BLIT_COPY|BLIT_TRANS:
			blit_rle_clip_copy_trans(dx,dy,nSrcWidth,nSrcHeight,clip,pitch,dest->ptr,color,bits, src->userdata);break;
		case BLIT_MASK|BLIT_TRANS:
			blit_rle_clip_mask_trans(dx,dy,nSrcWidth,nSrcHeight,clip,pitch,dest->ptr,color,bits, src->userdata);break;
		}
	}
	dest->kx = oldx;
	dest->ky = oldy;
	
	return TRUE;
}
//------------------------------------------------------------------------
extern "C"
{
BOOL  _stdcall _move16(WBmpStruct*, const int, const int);
BOOL  _stdcall _back_move16(WBmpStruct*, const int, const int);
}
//------------------------------------------------------------------------
BOOL WBitmap::Scroll(int x, int y)
{
	if(!IsValid())
	{
		FKTraceLn("WBitmap::Scroll, invalid bitmap on scroll");
		return FALSE;
	}
	WBmpStruct* bmp = (WBmpStruct*)m_pBmpStruct;
	if(ABS(x) >= bmp->w || ABS(y) >= bmp->h)
	{
		FKTraceLn("WBitmap::Scroll, invalid x or y");
		FKTraceLn("The width %d, x = %d ", bmp->w, x);
		FKTraceLn("The height %d, y = %d", bmp->h, y);

		return FALSE;
	}

	if(y < 0 || (y == 0 && x < 0))
		return _move16(bmp, x, y);
	else
		return _back_move16(bmp, x, y);
}
//------------------------------------------------------------------------
BOOL WBitmap::DrawText(ITTFont* pfont, int x, int y, const char* str, WPixel color, DWORD style/* =FONT_ALPHA */)
{
	if (!pfont) return FALSE;

	WChar tmp;
	BOOL ret;
	WBmpStruct* pStruct = (WBmpStruct*)m_pBmpStruct;
	int oldx = pStruct->kx;
	int oldy = pStruct->ky;

	int i;
	for(i = 0; str[i]; i ++)
	{
		unsigned a, b;
		a = (unsigned char)str[i];
		if(a > 0x80)
		{
			++i;
			b = (unsigned char)str[i];
			if(b == 0)
				break;
			a = a*256 + b;
		}
		if(tmp.Create(pfont, a, color))
		{
			if(a > 32)
			{
				SetPos(x, y);
				ret = (tmp.DrawFunc(style)((DWORD)m_pBmpStruct, (DWORD)tmp.GetData()) == 0);
			}/*
#ifdef _DEBUG
			static int f1=0;
			if (tmp.GetWidth() == 0 && f1==0)
			{
				f1=1;
				__asm int 3;
			}
#endif*/
			x += tmp.GetWidth();
		}
	}
	
	SetPos(oldx, oldy);
	return ret;
}
//------------------------------------------------------------------------
BOOL WBitmap::DrawTextEx(ITTFont* pfont, int x, int y, const char* str, WPixel color, WPixel outlinecolor, DWORD style/* =FONT_ALPHA */)
{
	if (!pfont) return FALSE;

	WChar tmp;
	BOOL ret;
	WBmpStruct* pStruct = (WBmpStruct*)m_pBmpStruct;
	int oldx = pStruct->kx;
	int oldy = pStruct->ky;

	int i;
	for(i = 0; str[i]; i ++)
	{
		unsigned a, b;
		a = (unsigned char)str[i];
		if(a > 0x80)
		{
			++i;
			b = (unsigned char)str[i];
			if(b == 0)
				break;
			a = a*256 + b;
		}
		if(tmp.Create(pfont, a, color, outlinecolor))
		{
			if(a > 32)
			{
				SetPos(x, y);
				ret = (tmp.DrawFunc(style)((DWORD)m_pBmpStruct, (DWORD)tmp.GetData()) == 0);
			}/*
#ifdef _DEBUG
			static int f1=0;
			if (tmp.GetWidth() == 0 && f1==0)
			{
				f1=1;
				__asm int 3;
			}
#endif*/
			x += tmp.GetWidth();
		}
	}
	
	SetPos(oldx, oldy);
	return ret;
}
//------------------------------------------------------------------------
// 测试某个点是否透明，可用于今后在上层组件中对对象进行选中判断
BOOL WBitmap::IsTransparent(POINT& ptSelect)
{
	if(!IsValid())
	{
		FKTraceLn("Bitmap is invalid!");
		return TRUE; // 图片为空，可看作透明，呵呵
	}

	WBmpStruct* bmp = (WBmpStruct*)m_pBmpStruct;
	
	if (ptSelect.x < 0 || ptSelect.y < 0 ||
		ptSelect.x > bmp->w || ptSelect.y > bmp->h) // 非法点
		return TRUE;
	WPixel* pLine = (WPixel*)((DWORD)(bmp)->ptr + ptSelect.y*(bmp)->pitch);
	WPixel* pColor = (WPixel*) ((BYTE*)pLine + sizeof(WPixel)*(ptSelect.x));
	if (*pColor == WPixel(255,0,255))
		return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------
// 得到图片所占内存的大小
DWORD WBitmap::GetSize()
{
	WBmpStruct* pData = (WBmpStruct*)m_pBmpStruct;
	if (pData)
		return pData->pitch*pData->h;
	return 0;
}
//------------------------------------------------------------------------
inline WORD PixelAlphaBlending(DWORD x,DWORD y,int alpha)
{
	DWORD result;
    x = ((x&0xFFFF) | (x<<16)) & 0x7E0F81F;
    y = ((y&0xFFFF) | (y<<16)) & 0x7E0F81F;

    result = ((y - x) * alpha / 32 + x) & 0x7E0F81F;
    return (WORD)((result&0xFFFF) | (result>>16));
}
//------------------------------------------------------------------------
BOOL WBitmap::DrawPixel(int x, int y, WPixel clr, DWORD dwStyle, int alpha)
{
	if(!IsValid())
	{
		FKTraceLn("WBitmap::DrawPixel, but the bitmap is invalid!");
		return FALSE;
	}
	WBmpStruct* bmp = (WBmpStruct*)m_pBmpStruct;
	
	int oldx = bmp->kx;
	int oldy = bmp->ky;
	
	SetPos(x, y);

	if( x < 0 || x >= bmp->w || y < 0 || y >= bmp->h)
		return TRUE;
		
	switch(dwStyle)
	{
	case BLIT_TRANS:
		{
			(*this)[y][x].color = PixelAlphaBlending((*this)[y][x].color,clr.color,alpha>>3);
		}
		break;		

	default:
		(*this)[y][x] = clr;
		break;
	}
	/*
	if( drawMode & XorMode )
		pixel ^= *(short*)( line[y] + x*2 );
	else if( drawMode & AlphaMode )
		pixel = pixel;//(short)blender->blender[ alpha ]( pixel, *(short*)( line[y] + x*2 ) );
	*/
	SetPos(oldx, oldy);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL WBitmap::DrawText(int x, int y, const char* str, WPixel color, DWORD style/*=FONT_ALPHA*/)
{
	// 初始化文字显示例程
	/*if (!hanziInstalled)
	{
		if (!InitHanzi(WHanzi::SmallFont))
		{
			return FALSE;
		}
	}
	
	hz.TextOut(this, x, y, (char*)str, color);*/
#ifdef _DEBUG
	static bool f = false;
	if (!f)
	{
		f=!f;
		FKTraceLn("dot font not support now!");
	}
#endif
	return TRUE;
}
//------------------------------------------------------------------------
// 将IBitmap画向Canvas
BOOL WBitmap::Draw(ICanvas* pCanvas, int dx, int dy, DWORD dwStyle)
{
	return pCanvas->Draw(dx, dy, static_cast<IBitmap*>(this), dwStyle);
}
//------------------------------------------------------------------------
BOOL WBitmap::Draw(ICanvas* pCanvas, int dx, int dy, int w, int h, int sx, int sy, DWORD dwStyle)
{
	return pCanvas->Draw(dx, dy, w, h, static_cast<IBitmap*>(this), sx, sy, dwStyle);
}
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) BOOL CreateIBitmap(DWORD dwVersion, IBitmap** ppIBitmap, ITrace* pTrace)
{
#ifdef _DEBUG
	if(pTrace)
		g_DefaultTrace.Create(pTrace);
#endif

	WBitmap* pBitmap = new WBitmap;
	
	if(pBitmap == NULL)
		return FALSE;

	*ppIBitmap = static_cast<IBitmap*>(pBitmap);

	return TRUE;
}
//------------------------------------------------------------------------