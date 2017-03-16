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
;		Description:		字符串转 ID
;		Original Author:	云风
;		Authors:
;		Create Time:		2001/9/13
;		Modify Time:		2001/12/6
;
;:*W*:._.:*I*:_.:*N*:._.:*D*:._.:*S*:._.:*O*:._.:*U*:._.:*L*:._.:

[bits 32]

%include "asm/c32.mac"

[section .text class=code use32]

function _string2id
proc __string2id@4
	saveesi
	saveedi
	saveebx
	saveebp
%$str	arg
%$len	locvar
	endlocvar

	cld
	mov esi,var(%$str)
	mov ebp,0xF4FA8928
	mov ecx,0x37A8470E		;x0=0x37A8470E
	mov edi,0x7758B42B		;y0=0x7758B42B
.loop:
	mov ebx,0x267B0B11		;w=0x267B0B11
	rol ebp,1
	xor ebx,ebp
	lodsd
	test eax,0xff
	jnz .notend
	jmp .byte0
.notend:
	test eax,0xff00
	jz .byte1
	test eax,0xff0000
	jz .byte2
	test eax,0xff000000
	jz .byte3

	xor ecx,eax
	mov edx,ebx
	xor edi,eax

	add edx,edi
	or edx,0x2040801		;a=0x2040801
	and edx,0xBFEF7FDF		;c=0xBFEF7FDF

	mov eax,ecx
	mul edx
	adc eax,edx
	mov edx,ebx
	adc eax,0

	add edx,ecx
	or edx,0x804021			;b=0x804021
	and edx,0x7DFEFBFF		;d=0x7DFEFBFF

	mov ecx,eax
	mov eax,edi
	mul edx

	add edx,edx
	adc eax,edx
	jnc .skip
	add eax,2
.skip:
	mov edi,eax
	jmp .loop
.byte1:
	and eax,0xff
	jmp .byte3
.byte2:
	and eax,0xffff
	jmp .byte3
.byte3:
	xor ecx,eax
	mov edx,ebx
	xor edi,eax

	add edx,edi
	or edx,0x2040801		;a=0x2040801
	and edx,0xBFEF7FDF		;c=0xBFEF7FDF

	mov eax,ecx
	mul edx
	adc eax,edx
	mov edx,ebx
	adc eax,0

	add edx,ecx
	or edx,0x804021			;b=0x804021
	and edx,0x7DFEFBFF		;d=0x7DFEFBFF

	mov ecx,eax
	mov eax,edi
	mul edx

	add edx,edx
	adc eax,edx
	jnc .skip1
	add eax,2
.skip1:
	mov edi,eax
.tail1:
	mov ebx,0x267B0B11		;w=0x267B0B11
	rol ebp,1
	xor ebx,ebp
.byte0:
	xor ecx,0x9BE74448
	mov edx,ebx
	xor edi,0x9BE74448

	add edx,edi
	or edx,0x2040801		;a=0x2040801
	and edx,0xBFEF7FDF		;c=0xBFEF7FDF

	mov eax,ecx
	mul edx
	adc eax,edx
	mov edx,ebx
	adc eax,0

	add edx,ecx
	or edx,0x804021			;b=0x804021
	and edx,0x7DFEFBFF		;d=0x7DFEFBFF

	mov ecx,eax
	mov eax,edi
	mul edx

	add edx,edx
	adc eax,edx
	jnc .skip2
	add eax,2
.skip2:
	mov edi,eax
.tail2:
	mov ebx,0x267B0B11		;w=0x267B0B11
	rol ebp,1
	xor ebx,ebp
	xor ecx,0x66F42C48
	mov edx,ebx
	xor edi,0x66F42C48

	add edx,edi
	or edx,0x2040801		;a=0x2040801
	and edx,0xBFEF7FDF		;c=0xBFEF7FDF

	mov eax,ecx
	mul edx
	adc eax,edx
	mov edx,ebx
	adc eax,0

	add edx,ecx
	or edx,0x804021			;b=0x804021
	and edx,0x7DFEFBFF		;d=0x7DFEFBFF

	mov ecx,eax
	mov eax,edi
	mul edx

	add edx,edx
	adc eax,edx
	jnc .skip3
	add eax,2
.skip3:
	mov edx,ecx
	xor eax,ecx
endproc

