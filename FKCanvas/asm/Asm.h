/**
*	created:		2012-6-20   18:29
*	filename: 		Asm
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../IFKCanvas.h"
#include "../Include/StructCommon.h"
//------------------------------------------------------------------------
extern __int64 mask;
extern __int64 xffdf;
extern __int64 x0020;
extern __int64 mask_r;
extern __int64 mask_g;
extern __int64 mask_b;
extern __int64 mask_alpha;
extern __int64 alpha;
//------------------------------------------------------------------------
// 16bits位图绘制
WSERR _stdcall blit16(WBmpStruct* dest, const WBmpStruct* src);
WSERR _stdcall clear16(WBmpStruct* bmp, WPixel color);
WSERR _stdcall mask_blit16(WBmpStruct* dest,const WBmpStruct* src);
WSERR _stdcall additive_blit16(WBmpStruct* dest,const WBmpStruct* src);
WSERR _stdcall subtract_blit16(WBmpStruct* dest,const WBmpStruct* src);
WSERR _stdcall trans_blit16(WBmpStruct* dest,const WBmpStruct* src);
WSERR _stdcall mask_trans_blit16(WBmpStruct* dest,const WBmpStruct* src);
WSERR _stdcall move16(WBmpStruct* bmp, const int x, const int y);
WSERR _stdcall back_move16(WBmpStruct* bmp, const int x, const int y);

// 16位Alpha位图 Blit
WSERR _stdcall alpha_blit16(WBmpStruct* dest, const WAlphaBmpStruct* src);
WSERR _stdcall alpha_trans_blit16(WBmpStruct* dest, const WAlphaBmpStruct* src);

// 矩形绘制
WSERR _stdcall box16(WBmpStruct* bmp, const WBoxStruct* box);
WSERR _stdcall trans_box16(WBmpStruct* bmp, const WBoxStruct* box);

// 16位位图 文字输出
WSERR _stdcall draw_ascii(WBmpStruct* bmp, char c);

// 字体绘制
WSERR _stdcall font16_alpha(WBmpStruct* bmp, const WCharStruct* c);
WSERR _stdcall font16_noalpha(WBmpStruct* bmp, const WCharStruct* c);
WSERR _stdcall font16_noalpha_with_outline(WBmpStruct* bmp, const WCharStruct* c);

void __fastcall Memset16Bit (WPixel* dst, WPixel fill, int num);

void alpha_1(void);
void alpha_2(void);
void alpha_3(void);
void alpha_4(void);
void alpha_5(void);
void alpha_6(void);
void alpha_7(void);
void alpha_8(void);

void blit_alpha(WPixel* dest, WPixel* src, int alpha);
void blit_rle_copy(int dx, int dy, int w, int h, int pitch, void* ptr, WPixel* color, BYTE* bits);
void blit_rle_mask(int dx, int dy, int w, int h, int pitch, void* ptr, WPixel* color, BYTE* bits);
void blit_rle_copy_trans(int dx, int dy, int w, int h, int pitch, void* ptr, WPixel* color, BYTE* bits, DWORD userdata);
void blit_rle_mask_trans(int dx, int dy, int w, int h, int pitch, void* ptr, WPixel* color, BYTE* bits, DWORD userdata);
void blit_rle_clip_copy(int dx, int dy, int w, int h, RECT& clip, int pitch, void* ptr, WPixel* color, BYTE* bits);
void blit_rle_clip_mask(int dx, int dy, int w, int h, RECT& clip, int pitch, void* ptr, WPixel* color, BYTE* bits);
void blit_rle_clip_copy_trans(int dx, int dy, int w, int h, RECT& clip, int pitch, void* ptr, WPixel* color, BYTE* bits, DWORD userdata);
void blit_rle_clip_mask_trans(int dx, int dy, int w, int h, RECT& clip, int pitch, void* ptr, WPixel* color, BYTE* bits, DWORD userdata);

WSERR __stdcall blit_rle_alpha(int dx, int dy, int w, int h, int pitch, void* ptr, WPixel* color, BYTE* bits);
WSERR __stdcall blit_rle_alpha_trans(int dx, int dy, int w, int h, int pitch, void* ptr, WPixel* color, BYTE* bits, DWORD userdata);
WSERR __stdcall blit_rle_clip_alpha(int dx, int dy, int w, int h, RECT* clip, int pitch, void* ptr, WPixel* color, BYTE* bits);
WSERR __stdcall blit_rle_clip_alpha_trans(int dx, int dy, int w, int h, RECT* clip, int pitch, void* ptr, WPixel* color, BYTE* bits, DWORD userdata);

WSERR __stdcall blit_rle_alpha16(int dx, int dy, int w, int h, int pitch, void* ptr, BYTE* bits);
WSERR __stdcall blit_rle_clip_alpha16(int dx, int dy, int w, int h, RECT* clip, int pitch, void* ptr, BYTE* bits);

WSERR _stdcall alpha_blit16_modify(WBmpStruct* dest, const WAlphaBmpStruct* src);

#ifdef _DEBUG
	void ablend_565(unsigned char *lpAlpha,unsigned int iAlpPitch,
					unsigned char *lpSrc,unsigned int iSrcX, unsigned int iSrcY,
					unsigned int iSrcPitch, unsigned char *lpDst,
					unsigned int iDstX, unsigned int iDstY,
					unsigned int iDstW, unsigned int iDstH,
					unsigned int iDstPitch);
	void alpha_blit16_bycplusplus(unsigned char *lpAlpha,unsigned int iAlpPitch,
								  unsigned char *lpSrc,unsigned int iSrcX, unsigned int iSrcY,
								  unsigned int iSrcPitch, unsigned char *lpDst,
								  unsigned int iDstX, unsigned int iDstY,
								  unsigned int iDstW, unsigned int iDstH,
								  unsigned int iDstPitch);
	void __AlphaBlend565(const BYTE *pAlphas, DWORD iAlpPitch, const WORD *pSrc, DWORD iSrcPitch,
						 WORD *pDest, DWORD uDestW, DWORD uDestH, DWORD uDestPitch);
#endif
