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
;		Description:		16位Alpha位图 Blit
;		Original Author:	云风
;		Authors:
;		Create Time:		2001/3/10
;		Modify Time:
;
;:*W*:._.:*I*:_.:*N*:._.:*D*:._.:*S*:._.:*O*:._.:*U*:._.:*L*:._.:

[bits 32]

%include "asm/c32.mac"
%include "asm/wpp.inc"
%include "asm/bmpclip.mac"

[section .text class=code use32]

; 把 16 位 Alpha 位图 bmp BLIT 到 this 位图上
;WSERR _stdcall _alpha_blit16(WBmpStruct *,const WAlphaBmpStruct *);
function _alpha_blit16
proc __alpha_blit16@8
	saveesi
	saveedi
	saveebx
	saveebp
%$this	arg
%$bmp	arg
%$w	locvar
%$h	locvar
%$w4	locvar
	endlocvar

	mov ebx,var(%$bmp)				; ebx = bmp 指针(AlphaBitmap)
	mov ecx,var(%$this)				; 保存 this 指针(Bitmap)
	mov ebp,[ebx+WBmpStruct.ptr]	; ebp为源位图的数据指针
	bmpclip	16						; 剪裁处理(16 bit)
									; ebx=bmp(源)指针,ecx=this(目标)指针
									; esi=源地址, edi=目标地址
.blit:
	pcmpeqd mm6,mm6					; mm6=0xFFFFFFFFFFFFFFFF
	sub ebp,esi						; ebp=源位图的数据指针(ebp)和实际绘图时的起始指针(esi)的偏移,ebp<=0
	mov edx,[ecx+WBmpStruct.pitch]	; edx=目标图 pitch
	neg ebp							; ebp>=0
	mov eax,var(%$w)				; eax=w
	shr ebp,1						; ebp=源位图的数据指针偏移的1/2,用于定位通道的实际指针
	sub eax,4						; eax=w-4
	xor ecx,ecx						; ecx=0,在后面用于宽度计数
	mov var(%$w4),eax				; w4=w-4
	and eax,3						; eax=(w-4)/4的余数
	add ebp,[ebx+WAlphaBmpStruct.channel]	; 源位图通道的实际绘图指针
	shl eax,4								; eax为行尾剩余象素数目所对应的位数
	mov ebx,[ebx+WBmpStruct.pitch]	; ebx=源图 pitch
	movd mm5,eax					; mm5=eax
	sar ebx,1						; ebx=源图 pitch/2，用于后面的通道指针递增
	psllq mm6,mm5					; mm6用于行尾正好不是4个象素时进行裁剪
	cld								; 设置指针为递增模式
	align 4
.loop:	
	mov eax,0xffffffff				; eax=0xffffffff
	cmp ecx,var(%$w)				; ecx<w?
	jl .loop_pixel					; ecx<w
.nextline:							; 处理下一行
	xor ecx,ecx						; ecx=0 (宽度计数)
	add ebp,ebx						; 通道指针改变到下一行
	lea esi,[esi+ebx*2]				; 源位图数据指针改变到下一行
	add edi,edx						; 目标位图数据指针改变到下一行
	dec dword var(%$h)				; h高度减1
	jnz .loop
	emms
	jmp .end
.skip4pixel:
	add ecx,4				; 宽度计数器增4
	jmp .loop

.loop_pixel:
	sub eax,[ebp+ecx]		; eax=255-alpha 值(记录4个) eax=0xb4b3b2b1 (b1...b4为a1...a4进行255-alpha的结果)
	jz .4pixel				; 说明4个象素点的alpha均为255，只画源位图，不需alpha混合
	cmp eax,0xffffffff		; 4个alpha值是否都等于0?
	jz .skip4pixel			; 是的，则跳过这4个象素
	pxor mm4,mm4			; mm4=0
	movd mm7,eax			; mm7=4个(255-alpha)=0x00000000 b4b3b2b1
	movq mm0,[edi+ecx*2]	; 目标像素
	punpcklbw mm7,mm4		; mm7 = 0x00b4 00b3 00b2 00b1;byte->word
	movq mm1,mm0			; mm1=mm0=目标像素
	movq mm4,mm0			; mm4=mm0=目标像素
	movq mm2,[mask_b]		; mm2=0x001f 001f 001f 001f (蓝色掩码)
	psrlw mm0,11			; mm0 = red (按word右移11位)
	pand mm2,mm4			; mm2 = blue
	pand mm1,[mask_g]		; mm1 = green(不过还没有移位，即 gggggg00000)
	pmullw mm0,mm7			; mm0=red*(255-alpha)
	psrlw mm1,5				; mm1=green
	pmullw mm2,mm7			; mm2=blue*(255-alpha)
	psllw mm0,3				; mm0=red<<3(有进位，实际是先<<11,然后>>8(呵呵,都要除以256嘛))
	pmullw mm1,mm7			; mm1=green*(255-alpha)
	psrlw mm2,8				; mm2=blue>>8(呵呵,都要除以256嘛)
	psrlw mm1,3				; mm1=green>>3(实际是先<<5,然后>>8(呵呵,都要除以256嘛))
	pand mm0,[mask_r]		; red分量?
	pand mm1,[mask_g]		; green分量?
	por mm0,mm2				; 
	por mm0,mm1				; 
	cmp ecx,var(%$w4)		; ecx<=w4?
	paddw mm0,[esi+ecx*2]	; mm0=源象素和目标象素的alpha混合(源象素在位图加载时即进行了alpha混合,这里就不需要混合了)
	jle .not_tail			; ecx<=w4,还没有到行末
.clip:						; 到了行尾，没有4个点blit，要进行裁剪
	pand mm4,mm6			; 将目标象素中有效的点用0x0000替换
	psubusw mm0,mm6			; 将mm0中无效的点用0x0000替换
	por mm0,mm4				; 源和目标象素混合
	movq [edi+ecx*2],mm0	; 写入混合后的象素
	jmp .nextline			; 处理下一行
.4pixel:					; 移动4个不需alpha混合计算的4个象素点
	cmp ecx,var(%$w4)		; ecx<=w4?
	movq mm0,[esi+ecx*2]	; mm0=源象素
	jle .neednotclip		; ecx<=w4,不需裁剪
	movq mm4,[edi+ecx*2]	; 需要裁剪,mm4=目标象素
	jmp .clip				; 处理裁剪

.neednotclip:				; 不需要裁剪的数据处理
	movq [edi+ecx*2],mm0	; 将混合数据写入目标位图
	add ecx,4				; 宽度递增4
	jmp .loop

.not_tail:					; 还没有到行尾
	movq [edi+ecx*2],mm0	; 将混合数据写入目标位图
	add ecx,4				; 宽度递增4
	jmp .loop
.end:	
	xor eax,eax
;	mov eax,var(%$this)		; 返回 this 指针
endproc

function _alpha_trans_blit16
proc __alpha_trans_blit16@8
	saveesi
	saveedi
	saveebx
	saveebp
%$this	arg
%$bmp	arg
%$w	locvar
%$h	locvar
%$w4	locvar
	endlocvar

	mov ebx,var(%$bmp)		; edx = bmp 指针
	movd mm7,[ebx+WBmpStruct.data]
	mov ecx,var(%$this)		; this 指针
	mov ebp,[ebx+WBmpStruct.ptr]	; ebp 为源图指针
	bmpclip	16			; 剪裁处理(16 bit)
					; ebx=bmp(目标)指针,ecx=this(源)指针
					; esi=源地址, edi=目标地址
.blit:
	punpcklwd mm7,mm7
	pcmpeqd mm6,mm6
	punpckldq mm7,mm7		; mm7=alpha
	sub ebp,esi
	mov edx,[ecx+WBmpStruct.pitch]	; 目标图 pitch
	neg ebp
	mov eax,var(%$w)
	shr ebp,1
	sub eax,4
	xor ecx,ecx
	mov var(%$w4),eax
	and eax,3
	add ebp,[ebx+WAlphaBmpStruct.channel]
	shl eax,4
	mov ebx,[ebx+WBmpStruct.pitch]	; 源图 pitch
	movd mm5,eax
	sar ebx,1
	psllq mm6,mm5
	cld
	movq [mask],mm6
	align 4
.loop:	
	cmp ecx,var(%$w)
	jl .loop_pixel
.nextline:
	xor ecx,ecx
	add ebp,ebx
	lea esi,[esi+ebx*2]
	add edi,edx
	dec dword var(%$h)
	jnz .loop
	emms
	jmp .end
.skip4pixel:
	add ecx,4
	jmp .loop
.loop_pixel:
	mov eax,[ebp+ecx]		; alpha 值
	test eax,eax
	jz .skip4pixel

	movd mm6,eax
	pcmpeqb mm4,mm4
	movq mm3,[edi+ecx*2]
	punpcklbw mm6,mm4
	movq mm0,[esi+ecx*2]		; 目标像素
	pcmpeqw mm6,mm4
	movq mm4,mm3
	pand mm6,mm7			; mm6 = alpha
	movq mm5,mm3
	psrlw mm6,3

	psrlw mm3,5
	pand mm4,[mask_g]
	pand mm5,[mask_b]
	movq mm1,mm0
	movq mm2,mm0
	psrlw mm0,5
	pand mm1,[mask_g]
	pand mm2,[mask_b]
	psubw mm0,mm3
	psubw mm1,mm4
	pmullw mm0,mm6
	movq mm3,[edi+ecx*2]
	psubw mm2,mm5
	pmullw mm1,mm6
	paddw mm0,mm3
	pmullw mm2,mm6
	psrlw mm1,5
	psrlw mm2,5
	paddw mm1,mm4
	paddw mm2,mm5
	pand mm0,[mask_r]
	pand mm1,[mask_g]
	pand mm2,[mask_b]
	por mm0,mm1
	por mm0,mm2

	cmp ecx,var(%$w4)
	jle .not_tail
	pand mm3,[mask]
	psubusw mm0,[mask]
	por mm0,mm3
	movq [edi+ecx*2],mm0
	jmp .nextline
.not_tail:
	movq [edi+ecx*2],mm0
	add ecx,4
	jmp .loop
.end:	
	xor eax,eax
endproc

[section .data class=data use32]
mask_r	dd	0xf800f800,0xf800f800
mask_g	dd	0x07e007e0,0x07e007e0
mask_b	dd	0x001f001f,0x001f001f

[section .bss class=bss use32]
mask	resq	1