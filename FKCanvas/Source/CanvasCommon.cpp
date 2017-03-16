/**
*	created:		2012-6-19   20:57
*	filename: 		CanvasCommon
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../IFKCanvas.h"
#include "../../FKTrace/IFKTrace.h"
#include "../Include/CanvasCommon.h"
#include "../Include/StructCommon.h"
//------------------------------------------------------------------------
#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif
//------------------------------------------------------------------------
WBmpStruct::~WBmpStruct()
{
	if(ptr)
	{
		if(!bChild)
			delete []ptr;
		ptr = 0;
	}
}
//------------------------------------------------------------------------
WBmpStruct& WBmpStruct::operator =(const WBmpStruct& data)
{
	w  = data.w;
	h  = data.h;
	pitch = data.pitch;
	ptr = data.ptr;
	userdata = data.userdata;
	bChild = data.bChild;
	return *this;
}
//------------------------------------------------------------------------
WBmpStruct::WBmpStruct(const WBmpStruct& data)
{
	w=data.w,h=data.h,
		pitch=data.pitch,ptr=data.ptr,

		userdata=data.userdata;

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
WSERR _stdcall _draw_line_solid(WBmpStruct *bmp,const WLineStruct *line)
{
	int x1=bmp->kx-line->kx,x2=x1+line->x,y1=bmp->ky-line->ky,y2=y1+line->y;
	int dx = x2-x1;
	int dy = y2-y1;
	int i1, i2;
	int dd;
	WPixel *pixel,color;
	if (line->positive) {
		if (dx >= dy) {
			i1 = 2 * dy;
			dd = i1 -  dx;
			i2 = dd -  dx;
			if (x2<0 || y1>=bmp->h) return WS_OK;
			if (x2>bmp->w) x2=bmp->w;
			color=line->color;
			pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y1)+x1;

			while (x1 < x2) {
				if (x1>=0 && y1>=0)
					*pixel=color;

				if (dd >= 0) {
					if (++y1>=bmp->h) return WS_OK;
					pixel=(WPixel*)((DWORD)pixel+bmp->pitch);
					dd += i2;
				}
				else         
					dd += i1; 

				x1 ++;
				++pixel;
			}
		}
		else {
			i1 = 2 * dx;
			dd = i1 -  dy;
			i2 = dd -  dy;
			if (y2<0 || x1>=bmp->w) return WS_OK;
			if (y2>bmp->h) y2=bmp->h;
			color=line->color;
			pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y1)+x1;

			while (y1 < y2) {
				if (x1>=0 && y1>=0)
					*pixel=color;

				if (dd >= 0) {
					if (++x1>=bmp->w) return WS_OK;
					++pixel;
					dd += i2;
				}
				else         
					dd += i1; 

				y1 ++;
				pixel=(WPixel*)((DWORD)pixel+bmp->pitch);
			}
		}
	}
	else {
		if (dx >= dy) {
			i1 = 2 * dy;
			dd = i1 -  dx;
			i2 = dd -  dx;
			if (x2<0 || y2<0) return WS_OK;
			if (x2>bmp->w) x2=bmp->w;
			color=line->color;
			pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y2)+x1;

			while (x1 < x2) {
				if (x1>=0 && y2<bmp->h)
					*pixel=color;

				if (dd >= 0) {
					if (--y2<0) return WS_OK;
					pixel=(WPixel*)((DWORD)pixel-bmp->pitch);
					dd += i2;
				}
				else         
					dd += i1; 

				x1 ++;
				++pixel;
			}
		}
		else {
			i1 = 2 * dx;
			dd = i1 -  dy;
			i2 = dd -  dy;
			if (y1>=bmp->h || x1>=bmp->w) return WS_OK;
			if (y1<0) y1=0;
			color=line->color;
			pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y2)+x1;

			while (y2 > y1) {
				if (x1>=0 && y2<bmp->h)
					*pixel=color;

				if (dd >= 0) {
					if (++x1>=bmp->w) return WS_OK;
					++pixel;
					dd += i2;
				}
				else         
					dd += i1; 

				y2 --;
				pixel=(WPixel*)((DWORD)pixel-bmp->pitch);
			}
		}
	}
	return WS_OK;
}
//------------------------------------------------------------------------
WSERR _stdcall _draw_hline_solid(WBmpStruct *bmp,const WLineStruct *line)
{
	int y=bmp->ky-line->ky;
	int x1,x2;
	WPixel *pixel,color;
	if (y<0 || y>=bmp->h) return WS_OK;
	color=line->color;
	x1=bmp->kx-line->kx,x2=x1+line->x;
	if (x1<0) x1=0;
	if (x2>=bmp->w) x2=bmp->w;
	pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y)+x1;
	for (;x1<x2;x1++,++pixel)
		*pixel=color;
	return WS_OK;
}
//------------------------------------------------------------------------
WSERR _stdcall _draw_vline_solid(WBmpStruct *bmp,const WLineStruct *line)
{
	int x=bmp->kx-line->kx;
	int y1,y2;
	WPixel *pixel,color;
	if (x<0 || x>=bmp->w) return WS_OK;
	color=line->color;
	y1=bmp->ky-line->ky,y2=y1+line->y;
	if (y1<0) y1=0;
	if (y2>=bmp->h) y2=bmp->h;
	pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y1)+x;
	for (;y1<y2;y1++,pixel=(WPixel*)((DWORD)pixel+bmp->pitch))
		*pixel=color;
	return WS_OK;
}
//------------------------------------------------------------------------
WSERR _stdcall _draw_line_trans(WBmpStruct *bmp,const WLineStruct *line)
{
	int x1=bmp->kx-line->kx,x2=x1+line->x,y1=bmp->ky-line->ky,y2=y1+line->y;
	int dx = x2-x1;
	int dy = y2-y1;
	int i1, i2;
	int dd;
	int alpha=line->userdata>>3;
	WPixel *pixel,color;
	if (line->positive) {
		if (dx >= dy) {
			i1 = 2 * dy;
			dd = i1 -  dx;
			i2 = dd -  dx;
			if (x2<0 || y1>=bmp->h) return WS_OK;
			if (x2>bmp->w) x2=bmp->w;
			color=line->color;
			pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y1)+x1;

			while (x1 < x2) {
				if (x1>=0 && y1>=0)
					pixel->color=PixelAlphaBlending(pixel->color,color.color,alpha);

				if (dd >= 0) {
					if (++y1>=bmp->h) return WS_OK;
					pixel=(WPixel*)((DWORD)pixel+bmp->pitch);
					dd += i2;
				}
				else         
					dd += i1; 

				x1 ++;
				++pixel;
			}
		}
		else {
			i1 = 2 * dx;
			dd = i1 -  dy;
			i2 = dd -  dy;
			if (y2<0 || x1>=bmp->w) return WS_OK;
			if (y2>bmp->h) y2=bmp->h;
			color=line->color;
			pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y1)+x1;

			while (y1 < y2) {
				if (x1>=0 && y1>=0)
					pixel->color=PixelAlphaBlending(pixel->color,color.color,alpha);

				if (dd >= 0) {
					if (++x1>=bmp->w) return WS_OK;
					++pixel;
					dd += i2;
				}
				else         
					dd += i1; 

				y1 ++;
				pixel=(WPixel*)((DWORD)pixel+bmp->pitch);
			}
		}
	}
	else {
		if (dx >= dy) {
			i1 = 2 * dy;
			dd = i1 -  dx;
			i2 = dd -  dx;
			if (x2<0 || y2<0) return WS_OK;
			if (x2>bmp->w) x2=bmp->w;
			color=line->color;
			pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y2)+x1;

			while (x1 < x2) {
				if (x1>=0 && y2<bmp->h)
					pixel->color=PixelAlphaBlending(pixel->color,color.color,alpha);

				if (dd >= 0) {
					if (--y2<0) return WS_OK;
					pixel=(WPixel*)((DWORD)pixel-bmp->pitch);
					dd += i2;
				}
				else         
					dd += i1; 

				x1 ++;
				++pixel;
			}
		}
		else {
			i1 = 2 * dx;
			dd = i1 -  dy;
			i2 = dd -  dy;
			if (y1>=bmp->h || x1>=bmp->w) return WS_OK;
			if (y1<0) y1=0;
			color=line->color;
			pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y2)+x1;

			while (y2 > y1) {
				if (x1>=0 && y2<bmp->h)
					pixel->color=PixelAlphaBlending(pixel->color,color.color,alpha);

				if (dd >= 0) {
					if (++x1>=bmp->w) return WS_OK;
					++pixel;
					dd += i2;
				}
				else         
					dd += i1; 

				y2 --;
				pixel=(WPixel*)((DWORD)pixel-bmp->pitch);
			}
		}
	}
	return WS_OK;
}
//------------------------------------------------------------------------
WSERR _stdcall _draw_hline_trans(WBmpStruct *bmp,const WLineStruct *line)
{
	int y=bmp->ky-line->ky;
	int x1,x2;
	int alpha=line->userdata>>3;
	WPixel *pixel,color;
	if (y<0 || y>=bmp->h) return WS_OK;
	color=line->color;
	x1=bmp->kx-line->kx,x2=x1+line->x;
	if (x1<0) x1=0;
	if (x2>=bmp->w) x2=bmp->w;
	pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y)+x1;
	for (;x1<x2;x1++,++pixel)
		pixel->color=PixelAlphaBlending(pixel->color,color.color,alpha);
	return WS_OK;
}
//------------------------------------------------------------------------
WSERR _stdcall _draw_vline_trans(WBmpStruct *bmp,const WLineStruct *line)
{
	int x=bmp->kx-line->kx;
	int y1,y2;
	int alpha=line->userdata>>3;
	WPixel *pixel,color;
	if (x<0 || x>=bmp->w) return WS_OK;
	color=line->color;
	y1=bmp->ky-line->ky,y2=y1+line->y;
	if (y1<0) y1=0;
	if (y2>=bmp->h) y2=bmp->h;
	pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y1)+x;
	for (;y1<y2;y1++,pixel=(WPixel*)((DWORD)pixel+bmp->pitch))
		pixel->color=PixelAlphaBlending(pixel->color,color.color,alpha);
	return WS_OK;
}
//------------------------------------------------------------------------
WSERR _stdcall _draw_line_xor(WBmpStruct *bmp,const WLineStruct *line)
{
	int x1=bmp->kx-line->kx,x2=x1+line->x,y1=bmp->ky-line->ky,y2=y1+line->y;
	int dx = x2-x1;
	int dy = y2-y1;
	int i1, i2;
	int dd;
	int alpha=line->userdata>>3;
	WPixel *pixel,color;
	if (line->positive) {
		if (dx >= dy) {
			i1 = 2 * dy;
			dd = i1 -  dx;
			i2 = dd -  dx;
			if (x2<0 || y1>=bmp->h) return WS_OK;
			if (x2>bmp->w) x2=bmp->w;
			color=line->color;
			pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y1)+x1;

			while (x1 < x2) {
				if (x1>=0 && y1>=0)
					pixel->color^=color.color;

				if (dd >= 0) {
					if (++y1>=bmp->h) return WS_OK;
					pixel=(WPixel*)((DWORD)pixel+bmp->pitch);
					dd += i2;
				}
				else         
					dd += i1; 

				x1 ++;
				++pixel;
			}
		}
		else {
			i1 = 2 * dx;
			dd = i1 -  dy;
			i2 = dd -  dy;
			if (y2<0 || x1>=bmp->w) return WS_OK;
			if (y2>bmp->h) y2=bmp->h;
			color=line->color;
			pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y1)+x1;

			while (y1 < y2) {
				if (x1>=0 && y1>=0)
					pixel->color^=color.color;

				if (dd >= 0) {
					if (++x1>=bmp->w) return WS_OK;
					++pixel;
					dd += i2;
				}
				else         
					dd += i1; 

				y1 ++;
				pixel=(WPixel*)((DWORD)pixel+bmp->pitch);
			}
		}
	}
	else {
		if (dx >= dy) {
			i1 = 2 * dy;
			dd = i1 -  dx;
			i2 = dd -  dx;
			if (x2<0 || y2<0) return WS_OK;
			if (x2>bmp->w) x2=bmp->w;
			color=line->color;
			pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y2)+x1;

			while (x1 < x2) {
				if (x1>=0 && y2<bmp->h)
					pixel->color^=color.color;

				if (dd >= 0) {
					if (--y2<0) return WS_OK;
					pixel=(WPixel*)((DWORD)pixel-bmp->pitch);
					dd += i2;
				}
				else         
					dd += i1; 

				x1 ++;
				++pixel;
			}
		}
		else {
			i1 = 2 * dx;
			dd = i1 -  dy;
			i2 = dd -  dy;
			if (y1>=bmp->h || x1>=bmp->w) return WS_OK;
			if (y1<0) y1=0;
			color=line->color;
			pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y2)+x1;

			while (y2 > y1) {
				if (x1>=0 && y2<bmp->h)
					pixel->color^=color.color;

				if (dd >= 0) {
					if (++x1>=bmp->w) return WS_OK;
					++pixel;
					dd += i2;
				}
				else         
					dd += i1; 

				y2 --;
				pixel=(WPixel*)((DWORD)pixel-bmp->pitch);
			}
		}
	}
	return WS_OK;
}
//------------------------------------------------------------------------
WSERR _stdcall _draw_hline_xor(WBmpStruct *bmp,const WLineStruct *line)
{
	int y=bmp->ky-line->ky;
	int x1,x2;
	int alpha=line->userdata>>3;
	WPixel *pixel,color;
	if (y<0 || y>=bmp->h) return WS_OK;
	color=line->color;
	x1=bmp->kx-line->kx,x2=x1+line->x;
	if (x1<0) x1=0;
	if (x2>=bmp->w) x2=bmp->w;
	pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y)+x1;
	for (;x1<x2;x1++,++pixel)
		pixel->color^=color.color;
	return WS_OK;
}
//------------------------------------------------------------------------
WSERR _stdcall _draw_vline_xor(WBmpStruct *bmp,const WLineStruct *line)
{
	int x=bmp->kx-line->kx;
	int y1,y2;
	int alpha=line->userdata>>3;
	WPixel *pixel,color;
	if (x<0 || x>=bmp->w) return WS_OK;
	color=line->color;
	y1=bmp->ky-line->ky,y2=y1+line->y;
	if (y1<0) y1=0;
	if (y2>=bmp->h) y2=bmp->h;
	pixel=(WPixel*)((DWORD)bmp->ptr+bmp->pitch*y1)+x;
	for (;y1<y2;y1++,pixel=(WPixel*)((DWORD)pixel+bmp->pitch))
		pixel->color^=color.color;
	return WS_OK;
}
//------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
//------------------------------------------------------------------------