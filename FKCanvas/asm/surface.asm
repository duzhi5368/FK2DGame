;
;                ..;;;;,                     ;;;,    
;            ..;;;"` ;;:           ,;;;;;: ,;;;:.,;..          _/
;      ,;..,;;"`    :;;'            :;;"`,;;;;;;"":;;         _/ 
;      ;;;"  `:;;. ;;'           ..;;:  .;;,.;;:',;;"    _/_/_/_/_/
;     .;;`   ,;;" .;"          ,;;;;;;" :;`',;",;;"         _/
;    ,;;,:.,;;;  ,;:          :" ,;:` , `:.;;;;;'`         _/   
;    ;;"'':;;:. .;; .          ,;;;,;:;;,;;;, ,;             _/
;   :;;..;;;;;; :;' :.        :;;;"` `:;;;;;,;,.;.          _/
; .;;":;;`  '"";;:  ';;       '""   .;;`.;";:;;;;` ;,  _/_/_/_/_/
;;;;" `'       "::. ,;;:          .;"`  ::. '   .,;;;     _/ 
;""             ';;;;;;;"        ""     ';;;;;;;;;;`     _/
;
;                       Windsoul++
;
;                 基于 DirectX 的游戏程序库               
;       A Game Programming Library Base On DirectX    
; 
;               by 云风 (Cloud Wu)  1999-2000                          
;
;       http://member.netease.com/~cloudwu 
;		mailto:cloudwu@263.net
; 
;		请阅读 readme.txt 中的版权信息
;		See readme.txt for copyright information.
;
;		Description:		16位位图 Surface 操作
;		Original Author:	云风
;		Authors:
;		Create Time:		2000/12/27
;		Modify Time:		2002/4/15
;
;:*W*:._.:*I*:._.:*N*:._.:*D*:._.:*S*:._.:*O*:._.:*U*:._.:*L*:._.:

[bits 32]

%include "c32.mac"
%include "wpp.inc"

block_width		equ	64
block_height	equ	32

[section .text class=code use32]

;void __stdcall _blit_surface_565(WBmpStruct *,void *ptr)
function __blit_surface_565
proc __blit_surface_565@8
	saveesi
	saveedi
%$bmp	arg
%$ptr	arg
	endlocvar

	mov edx,var(%$bmp)
	mov edi,var(%$ptr)
	mov esi,[edx+WBmpStruct.ptr]
	mov edx,[edx+WBmpStruct.pitch]
	mov eax,block_height
	sub edx,block_width*2
	mov ecx,block_width*2/8
.loop:
	movq mm0,[esi]
	add esi,8
	movq [edi],mm0
	add edi,8
	dec ecx
	jnz .loop
	mov ecx,block_width*2/8
	add esi,edx
	add edi,[pitch]
	dec eax
	jnz .loop
	emms
endproc

;void __stdcall _blit_surface_555(WBmpStruct *,void *ptr)
function __blit_surface_555
proc __blit_surface_555@8
	saveesi
	saveedi
%$bmp	arg
%$ptr	arg
	endlocvar

	mov edx,var(%$bmp)
	movq mm2,[mask_rg]
	movq mm3,[mask_b]
	mov edi,var(%$ptr)
	mov esi,[edx+WBmpStruct.ptr]
	mov edx,[edx+WBmpStruct.pitch]
	mov eax,block_height
	sub edx,block_width*2
	mov ecx,block_width*2/8
.loop:
	movq mm0,[esi]
	add esi,8
	movq mm1,mm0
	psrlw mm0,1
	pand mm1,mm3
	pand mm0,mm2
	por mm0,mm1
	movq [edi],mm0
	add edi,8
	dec ecx
	jnz .loop
	mov ecx,block_width*2/8
	add esi,edx
	add edi,[pitch]
	dec eax
	jnz .loop
	emms
endproc

;void __stdcall _blit_surface_24(WBmpStruct *,void *ptr)
function __blit_surface_24
proc __blit_surface_24@8
	saveesi
	saveedi
	saveebx
%$bmp	arg
%$ptr	arg
	endlocvar

	mov edx,var(%$bmp)
	mov edi,var(%$ptr)
	mov esi,[edx+WBmpStruct.ptr]
	mov edx,[edx+WBmpStruct.pitch]
	mov ebx,block_height
	sub edx,block_width*2
	mov ecx,block_width*2/8
	pxor mm7,mm7
	movq mm6,[mask_g]
.loop:
	movq mm0,[esi]
	add esi,8
	movq mm1,mm0
	movq mm2,mm0

	pand mm0,[mask_r]
	pand mm1,mm6
	pand mm2,[mask_b]

	movq mm3,mm0
	movq mm4,mm1
	movq mm5,mm2

	punpcklwd mm0,mm7
	punpckhwd mm3,mm7

	punpcklwd mm1,mm7
	punpckhwd mm4,mm7

	punpcklwd mm2,mm7
	punpckhwd mm5,mm7

	pslld mm0,8
	pslld mm3,8

	pslld mm1,5
	pslld mm4,5

	pslld mm2,3
	pslld mm5,3

	por mm0,mm1
	por mm3,mm4

	por mm0,mm2
	por mm3,mm5

	movd eax,mm0
	psrlq mm0,32
	mov [edi],ax
	shr eax,16
	mov [edi+2],al

	movd eax,mm0
	mov [edi+3],ax
	shr eax,16
	mov [edi+5],al

	movd eax,mm3
	psrlq mm3,32
	mov [edi+6],ax
	shr eax,16
	mov [edi+8],al

	movd eax,mm3
	mov [edi+9],ax
	shr eax,16
	mov [edi+11],al

	add edi,12
	dec ecx
	jz .next
	jmp .loop
.next:
	mov ecx,block_width*2/8
	add esi,edx
	add edi,[pitch]
	dec ebx
	jz .end
	jmp .loop
.end:
	emms
endproc


;void __stdcall _blit_surface_32(WBmpStruct *,void *ptr)
function __blit_surface_32
proc __blit_surface_32@8
	saveesi
	saveedi
%$bmp	arg
%$ptr	arg
	endlocvar

	mov edx,var(%$bmp)
	mov edi,var(%$ptr)
	mov esi,[edx+WBmpStruct.ptr]
	mov edx,[edx+WBmpStruct.pitch]
	mov eax,block_height
	sub edx,block_width*2
	mov ecx,block_width*2/8
	pxor mm7,mm7
	movq mm6,[mask_g]
.loop:
	movq mm0,[esi]
	add esi,8
	movq mm1,mm0
	movq mm2,mm0

	pand mm0,[mask_r]
	pand mm1,mm6
	pand mm2,[mask_b]

	movq mm3,mm0
	movq mm4,mm1
	movq mm5,mm2

	punpcklwd mm0,mm7
	punpckhwd mm3,mm7

	punpcklwd mm1,mm7
	punpckhwd mm4,mm7

	punpcklwd mm2,mm7
	punpckhwd mm5,mm7

	pslld mm0,8
	pslld mm3,8

	pslld mm1,5
	pslld mm4,5

	pslld mm2,3
	pslld mm5,3

	por mm0,mm1
	por mm3,mm4

	por mm0,mm2
	por mm3,mm5

	movq [edi],mm0
	movq [edi+8],mm3
	add edi,16
	dec ecx
	jnz .loop
	mov ecx,block_width*2/8
	add esi,edx
	add edi,[pitch]
	dec eax
	jnz .loop;
	emms
endproc

;void _stdcall _set_blit_pitch(int pitch);
function __set_blit_pitch
proc __set_blit_pitch@4
	saveesi
	saveedi
%$pitch	arg
	endlocvar
	mov eax,var(%$pitch)
	mov [pitch],eax
endproc

[section .data class=data use32]
mask_rg	dd	0xffe0ffe0,0xffe0ffe0
mask_r	dd	0xf800f800,0xf800f800
mask_g	dd	0x07e007e0,0x07e007e0
mask_b	dd	0x001f001f,0x001f001f

[section .bss class=bss use32]
pitch	resd	1
