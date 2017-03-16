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
;               by 云风 (Cloud Wu)  1999-2001
;
;       http://member.netease.com/~cloudwu 
;		mailto:cloudwu@263.net
; 
;		请阅读 readme.txt 中的版权信息
;		See readme.txt for copyright information.
;
;		Description:		16位矩形
;		Original Author:	云风
;		Authors:
;		Create Time:		2001/8/24
;		Modify Time:
;
;:*W*:._.:*I*:_.:*N*:._.:*D*:._.:*S*:._.:*O*:._.:*U*:._.:*L*:._.:

[bits 32]

%include "asm/c32.mac"
%include "asm/wpp.inc"
%include "asm/boxclip.mac"

[section .text class=code use32]

; 把 矩形 BLIT 到 目标 位图上
;WSERR _stdcall _box16(WBmpStruct *,const WBoxStruct *);
function _box16
proc __box16@8
	saveedi
	saveebx
%$this	arg
%$box	arg
%$w	locvar
%$h	locvar
	endlocvar

	mov ecx,var(%$this)		; 保存 this 指针
	mov ebx,var(%$box)		; edx = box 指针
	boxclip	16			; 剪裁处理(16 bit)
					; edi=目标地址
.blit:
	movd mm0,[ebx+WBoxStruct.color]
	mov edx,[ecx+WBmpStruct.pitch]

	punpcklwd mm0,mm0
	mov eax,var(%$w)
	punpckldq mm0,mm0

	mov edx,[ecx+WBmpStruct.pitch]	; 目标图 pitch
	add eax,eax
	mov ecx,var(%$w)
	sub edx,eax
	movd eax,mm0
	mov ebx,var(%$h)
	cld
	align 4
.loop:	
	sub ecx,4
	jle .nextline 
	movq [edi],mm0
	add edi,8
	jmp .loop
.nextline:
	add ecx,4
	rep stosw
	add edi,edx
	mov ecx,var(%$w)
	dec ebx
	jnz .loop
	emms
	xor eax,eax
endproc

; 把 矩形半透明 BLIT 到 目标 位图上
;WSERR _stdcall _trans_box16(WBmpStruct *,const WBoxStruct *);
function _trans_box16
proc __trans_box16@8
	saveesi
	saveedi
	saveebx
%$this	arg
%$box	arg
%$w	locvar
%$h	locvar
	endlocvar

	mov ecx,var(%$this)		; 保存 this 指针
	mov ebx,var(%$box)		; edx = box 指针
	boxclip	16			; 剪裁处理(16 bit)
					; edi=目标地址
.blit:
	mov eax,[ebx+WBoxStruct.data]	; alpha 值 (0-255)
	movd mm4,[ebx+WBoxStruct.color]
	test eax,eax
	jz .endclip
	movd mm6,eax
	punpcklwd mm4,mm4
	punpcklwd mm6,mm6
	punpckldq mm4,mm4
	punpckldq mm6,mm6

	movq mm1,mm4
	movq mm2,mm4

	psrlw mm6,3

	pand mm4,[mask_r]		; red 分量
	pand mm1,[mask_g]		; green 分量
	psrlw mm4,5
	pand mm2,[mask_b]		; blue 分量
	pmullw mm4,mm6
	psrlw mm1,5

	pand mm4,[mask_r]

	pmullw mm1,mm6 
	pand mm1,[mask_g]
	pmullw mm2,mm6
	por mm4,mm1
	psrlw mm2,5
	movq mm7,[x0020]
	por mm4,mm2
	psubusw mm7,mm6			; 32-alpha
	pcmpeqw mm6,mm6

	mov edx,[ecx+WBmpStruct.pitch]
	mov eax,var(%$w)
	mov edx,[ecx+WBmpStruct.pitch]	; 目标图 pitch
	add eax,eax
	mov ecx,var(%$w)
	sub edx,eax
	mov eax,3
	mov ebx,var(%$h)
	and eax,ecx
	jz .add4
	lea edx,[edx+eax*2]
	jmp .label
.add4:
	add edx,8
.label:
	sub eax,4
	pxor mm5,mm5
	neg eax
	shl eax,4
	movd mm5,eax
	movd eax,mm4
	psrlq mm6,mm5
	cld
	align 4
.loop:	
	movq mm0,[edi]
	movq mm1,mm0
	movq mm2,mm0

	pand mm0,[mask_r]		; red 分量
	pand mm1,[mask_g]		; green 分量
	psrlw mm0,5
	pand mm2,[mask_b]		; blue 分量
	pmullw mm0,mm7
	psrlw mm1,5

	pand mm0,[mask_r]

	pmullw mm1,mm7 
	pand mm1,[mask_g]
	pmullw mm2,mm7
	por mm0,mm1
	psrlw mm2,5
	por mm0,mm2

	sub ecx,4
	paddusw mm0,mm4
	jl .tail
	jz .nextline
	movq [edi],mm0
	add edi,8
	jmp .loop
.tail:
	movq mm1,[edi]
	pand mm0,mm6
	psubusw mm1,mm6
	por mm0,mm1
.nextline:
	movq [edi],mm0
	add edi,edx
	mov ecx,var(%$w)
	dec ebx
	jnz .loop
	emms
	xor eax,eax
endproc

[section .data class=data use32]
x0020	dd	0x00200020,0x00200020
mask	dd	0xf81ff81f,0xf81ff81f
mask_r	dd	0xf800f800,0xf800f800
mask_g	dd	0x07e007e0,0x07e007e0
mask_b	dd	0x001f001f,0x001f001f
