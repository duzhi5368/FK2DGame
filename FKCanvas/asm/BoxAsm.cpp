/**
*	created:		2012-6-20   19:52
*	filename: 		BoxAsm
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "Asm.h"
//------------------------------------------------------------------------
__int64 x0020 = 0x0020002000200020;
//------------------------------------------------------------------------
// 把 矩形 BLIT 到 目标 位图上
WSERR _stdcall box16(WBmpStruct* bmp, const WBoxStruct* box)
{
	int w,h;	
	__asm
	{
		mov ecx,bmp		; 保存 this 指针
			mov ebx,box		; edx = box 指针
	#include "boxclip.inc"
			; edi=目标地址
			_blit:
		movd mm0,[ebx]WBoxStruct.color
			mov edx,[ecx]WBmpStruct.pitch
			
			punpcklwd mm0,mm0
			mov eax,w
			punpckldq mm0,mm0
			
			mov edx,[ecx]WBmpStruct.pitch	; 目标图 pitch
			add eax,eax
			mov ecx,w
			sub edx,eax
			movd eax,mm0
			mov ebx,h
			cld
			align 4
			_loop:	
		sub ecx,4
			jle _nextline 
			movq [edi],mm0
			add edi,8
			jmp _loop
			_nextline:
		add ecx,4
			rep stosw
			add edi,edx
			mov ecx,w
			dec ebx
			jnz _loop
			emms
	}
	return WS_OK;
}
//------------------------------------------------------------------------
// 把 矩形 BLIT 到 目标 位图上
WSERR _stdcall trans_box16(WBmpStruct* bmp, const WBoxStruct* box)
{
	int w,h;	
	__asm
	{
	mov ecx,bmp		; 保存 this 指针
	mov ebx,box		; edx = box 指针
	#include "boxclip.inc"
					; edi=目标地址
_blit:
	mov eax,[ebx]WBoxStruct.userdata	; alpha 值 (0-255)
	movd mm4,[ebx]WBoxStruct.color
	test eax,eax
	jz _endclip
	movd mm6,eax
	punpcklwd mm4,mm4
	punpcklwd mm6,mm6
	punpckldq mm4,mm4
	punpckldq mm6,mm6

	movq mm1,mm4
	movq mm2,mm4

	psrlw mm6,3

	pand mm4,mask_r		; red 分量
	pand mm1,mask_g		; green 分量
	psrlw mm4,5
	pand mm2,mask_b		; blue 分量
	pmullw mm4,mm6
	psrlw mm1,5

	pand mm4,mask_r

	pmullw mm1,mm6 
	pand mm1,mask_g
	pmullw mm2,mm6
	por mm4,mm1
	psrlw mm2,5
	movq mm7,x0020
	por mm4,mm2
	psubusw mm7,mm6			; 32-alpha
	pcmpeqw mm6,mm6

	mov edx,[ecx]WBmpStruct.pitch
	mov eax,w
	mov edx,[ecx]WBmpStruct.pitch	; 目标图 pitch
	add eax,eax
	mov ecx,w
	sub edx,eax
	mov eax,3
	mov ebx,h
	and eax,ecx
	jz _add4
	lea edx,[edx+eax*2]
	jmp _label
_add4:
	add edx,8
_label:
	sub eax,4
	pxor mm5,mm5
	neg eax
	shl eax,4
	movd mm5,eax
	movd eax,mm4
	psrlq mm6,mm5
	cld
	align 4
_loop:	
	movq mm0,[edi]
	movq mm1,mm0
	movq mm2,mm0

	pand mm0,mask_r		; red 分量
	pand mm1,mask_g		; green 分量
	psrlw mm0,5
	pand mm2,mask_b		; blue 分量
	pmullw mm0,mm7
	psrlw mm1,5

	pand mm0,mask_r

	pmullw mm1,mm7 
	pand mm1,mask_g
	pmullw mm2,mm7
	por mm0,mm1
	psrlw mm2,5
	por mm0,mm2

	sub ecx,4
	paddusw mm0,mm4
	jl _tail
	jz _nextline
	movq [edi],mm0
	add edi,8
	jmp _loop
_tail:
	movq mm1,[edi]
	pand mm0,mm6
	psubusw mm1,mm6
	por mm0,mm1
_nextline:
	movq [edi],mm0
	add edi,edx
	mov ecx,w
	dec ebx
	jnz _loop
	emms		
	}
	return WS_OK;
}
//------------------------------------------------------------------------