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
;		Description:		16位位图 文字输出
;		Original Author:	云风
;		Authors:
;		Create Time:		2000/9/23
;		Modify Time:		2000/12/21
;
;:*W*:._.:*I*:_.:*N*:._.:*D*:._.:*S*:._.:*O*:._.:*U*:._.:*L*:._.:

[bits 32]

%include "asm/c32.mac"
%include "asm/wpp.inc"

[section .text class=code use32]

function _draw_ascii
proc __draw_ascii@8
	saveesi
	saveedi
	saveebx
%$data	arg
%$char	arg
%$w	locvar
%$h	locvar
%$pitch	locvar
	endlocvar

	movzx eax,byte var(%$char)		; 读入 ascii 字符
	mov esi,ascii
	sub eax,33
	mov edx,var(%$data)
	shl eax,2
	mov ebx,[edx+WBmpStruct.ky]
	lea eax,[eax*3]
	xor ecx,ecx
	add esi,eax			; esi 指向字体数据
	cmp ebx,0
	jge .y_gt_top			; 没有被上边界裁掉?
	sub ecx,ebx			; 上边界-y = 裁掉的行数
	cmp ecx,12
	jl .skip1
	jmp .end
.skip1:
	add ebx,ecx
.y_gt_top:
	cmp ebx,[edx+WBmpStruct.h]
	jl .skip2
	jmp .end
.skip2:
	mov edi,ebx
	add esi,ecx
	imul edi,[edx+WBmpStruct.pitch]
	sub ecx,12
	add edi,[edx+WBmpStruct.ptr]	; edi 位图行准备写汉字的行指针
	sub ebx,ecx
	neg ecx
	cmp ebx,[edx+WBmpStruct.h]
	jle .bnoclip;
	add ecx,[edx+WBmpStruct.h]
	sub ecx,ebx
.bnoclip:				; ecx 准备画多少行
	mov ebx,[edx+WBmpStruct.kx]
	mov var(%$h),ecx
	xor ecx,ecx

	cmp ebx,0
	jge .x_gt_left			; 没有被左边界裁掉?
	sub ecx,ebx			; 左边界-x = 裁掉的列数
	cmp ecx,7
	jge .end;
	add ebx,ecx
.x_gt_left:
	cmp ebx,[edx+WBmpStruct.w]
	jge .end
	mov eax,ecx
	lea edi,[edi+2*ebx]
	sub ecx,7
	sub ebx,ecx
	neg ecx
	cmp ebx,[edx+WBmpStruct.w]
	jle .rnoclip
	add ecx,[edx+WBmpStruct.w]
	sub ecx,ebx
.rnoclip:				; ecx 准备画多少列
	add eax,ecx
	sub eax,7
	neg eax
	mov var(%$char),eax

	mov eax,[edx+WBmpStruct.data]
	mov edx,[edx+WBmpStruct.pitch]
	sub edx,ecx
	sub edx,ecx
	dec ecx
;	mov var(%$pitch),edx
	mov var(%$w),ecx
.nextline:
	mov ecx,var(%$char)
	mov bl,[esi]
	shr ebx,cl
	mov ecx,var(%$w)
.loop:
	bt ebx,ecx
	jnc .skip
	mov [edi],ax
.skip:
	add edi,2
	dec ecx
	jns .loop
	inc esi
;	add edi,var(%$pitch)
	add edi,edx
	dec dword var(%$h)
	jnz .nextline
.end:

endproc

[section .data class=data use32]
ascii	dd	0x8080800,0x80808,0x8
	dd	0x242424,0x0,0x0
	dd	0x3f0a0a00,0x287e1414,0x28
	dd	0x28281e08,0xa0a0c18,0x83c
	dd	0x4c4a3100,0x29190e38,0x46
	dd	0x48483000,0x46444a32,0x39
	dd	0x80808,0x0,0x0
	dd	0x10080c06,0x10101010,0x60c08
	dd	0x4081820,0x4040404,0x301808
	dd	0x18360800,0x14,0x0
	dd	0x8000000,0x8087f08,0x8
	dd	0x0,0x18000000,0x100818
	dd	0x0,0x3e00,0x0
	dd	0x0,0x18000000,0x18
	dd	0x4020201,0x10100804,0x402020
	dd	0x42243c00,0x24424242,0x3c
	dd	0x8380800,0x8080808,0x3e
	dd	0x2023c00,0x20100806,0x3e
	dd	0x4043c00,0x4040418,0x38
	dd	0x140c0400,0x47e4424,0x4
	dd	0x20203c00,0x4040438,0x38
	dd	0x20100e00,0x2222322c,0x1c
	dd	0x4023e00,0x20101008,0x20
	dd	0x22221c00,0x2222241c,0x1c
	dd	0x22221c00,0x4021e22,0x38
	dd	0xc000000,0xc00000c,0xc
	dd	0x18000000,0x18000018,0x100818
	dd	0x2000000,0xc10100c,0x2
	dd	0x0,0x3e003e,0x0
	dd	0x20000000,0x18040418,0x20
	dd	0x2223c00,0x80804,0x8
	dd	0x4e221c00,0x205b5652,0x1c
	dd	0x14140800,0x223e2214,0x41
	dd	0x22223c00,0x2222223c,0x3c
	dd	0x40201e00,0x20404040,0x1e
	dd	0x42447800,0x44424242,0x78
	dd	0x20203e00,0x20203c20,0x3e
	dd	0x20203e00,0x20203c20,0x20
	dd	0x40201e00,0x22424e40,0x1e
	dd	0x22222200,0x2222223e,0x22
	dd	0x8083e00,0x8080808,0x3e
	dd	0x4043c00,0x4040404,0x78
	dd	0x28242200,0x24283030,0x22
	dd	0x20202000,0x20202020,0x3e
	dd	0x6a666600,0x42525a5a,0x42
	dd	0x32322200,0x26262a2a,0x22
	dd	0x42243c00,0x24424242,0x3c
	dd	0x22223c00,0x20203c22,0x20
	dd	0x42243c00,0x24424242,0x3041c
	dd	0x22223c00,0x24283824,0x22
	dd	0x20201e00,0x2020418,0x3c
	dd	0x8087f00,0x8080808,0x8
	dd	0x22222200,0x22222222,0x1c
	dd	0x22224100,0x1c141422,0x8
	dd	0x49414100,0x36363549,0x22
	dd	0x14224100,0x22140808,0x41
	dd	0x14224100,0x8080814,0x8
	dd	0x4027e00,0x40201008,0x7e
	dd	0x808080f,0x8080808,0xf0808
	dd	0x10202040,0x4040810,0x10202
	dd	0x404043c,0x4040404,0x3c0404
	dd	0x14180808,0x222414,0x0
	dd	0x0,0x0,0x7f00
	dd	0x810,0x0,0x0
	dd	0x1c000000,0x22221e02,0x1f
	dd	0x2c202020,0x22222232,0x3c
	dd	0x1e000000,0x20202020,0x1e
	dd	0x1e020202,0x26222222,0x1a
	dd	0x1c000000,0x20203e22,0x1e
	dd	0x7e10100e,0x10101010,0x10
	dd	0x1e000000,0x22222222,0x1c021e
	dd	0x2e202020,0x22222232,0x22
	dd	0x38000008,0x8080808,0x8
	dd	0x3c000004,0x4040404,0x380404
	dd	0x22202020,0x24283824,0x22
	dd	0x8080838,0x8080808,0x8
	dd	0x5b000000,0x4949496d,0x49
	dd	0x2e000000,0x22222232,0x22
	dd	0x3c000000,0x42424242,0x3c
	dd	0x2c000000,0x22222232,0x20203c
	dd	0x1e000000,0x22222222,0x2021e
	dd	0x2e000000,0x20202032,0x20
	dd	0x1e000000,0x2063820,0x3c
	dd	0x7e101000,0x10101010,0xe
	dd	0x22000000,0x26222222,0x3a
	dd	0x41000000,0x14142222,0x8
	dd	0x41000000,0x36555549,0x22
	dd	0x42000000,0x24181824,0x42
	dd	0x42000000,0x18182424,0x601010
	dd	0x7e000000,0x20100804,0x7e
	dd	0x808080e,0x8083008,0x60808
	dd	0x8080808,0x8080808,0x80808
	dd	0x8080830,0x8080608,0x380808
	dd	0x0,0x4e3900,0x0
	dd	0x22223e00,0x22222222,0x3e

;ascii	dd	0x3c180000,0x18183c3c,0x18180018,0
;	dd	0x66666600,0x24,0,0
;	dd	0x6c000000,0x6c6cfe6c,0x6c6cfe6c,0
;	dd	0xc67c1818,0x67cc0c2,0x7cc68606,0x1818
;	dd	0,0x180cc6c2,0x86c66030,0
;	dd	0x6c380000,0xdc76386c,0x76cccccc,0
;	dd	0x30303000,0x60,0,0
;	dd	0x180c0000,0x30303030,0xc183030,0
;	dd	0x18300000,0xc0c0c0c,0x30180c0c,0
;	dd	0,0xff3c6600,0x663c,0
;	dd	0,0x7e181800,0x1818,0
;	dd	0,0,0x18181800,0x30
;	dd	0,0xfe000000,0,0
;	dd	0,0,0x18180000,0
;	dd	0,0x180c0602,0x80c06030,0
;	dd	0x6c380000,0xd6d6c6c6,0x386cc6c6,0
;	dd	0x38180000,0x18181878,0x7e181818,0
;	dd	0xc67c0000,0x30180c06,0xfec6c060,0
;	dd	0xc67c0000,0x63c0606,0x7cc60606,0
;	dd	0x1c0c0000,0xfecc6c3c,0x1e0c0c0c,0
;	dd	0xc0fe0000,0x6fcc0c0,0x7cc60606,0
;	dd	0x60380000,0xc6fcc0c0,0x7cc6c6c6,0
;	dd	0xc6fe0000,0x180c0606,0x30303030,0
;	dd	0xc67c0000,0xc67cc6c6,0x7cc6c6c6,0
;	dd	0xc67c0000,0x67ec6c6,0x780c0606,0
;	dd	0,0x1818,0x181800,0
;	dd	0,0x1818,0x30181800,0
;	dd	0x6000000,0x6030180c,0x60c1830,0
;	dd	0,0x7e00,0x7e,0
;	dd	0x60000000,0x60c1830,0x6030180c,0
;	dd	0xc67c0000,0x18180cc6,0x18180018,0
;	dd	0x7c000000,0xdedec6c6,0x7cc0dcde,0
;	dd	0x38100000,0xfec6c66c,0xc6c6c6c6,0
;	dd	0x66fc0000,0x667c6666,0xfc666666,0
;	dd	0x663c0000,0xc0c0c0c2,0x3c66c2c0,0
;	dd	0x6cf80000,0x66666666,0xf86c6666,0
;	dd	0x66fe0000,0x68786862,0xfe666260,0
;	dd	0x66fe0000,0x68786862,0xf0606060,0
;	dd	0x663c0000,0xdec0c0c2,0x3a66c6c6,0
;	dd	0xc6c60000,0xc6fec6c6,0xc6c6c6c6,0
;	dd	0x183c0000,0x18181818,0x3c181818,0
;	dd	0xc1e0000,0xc0c0c0c,0x78cccccc,0
;	dd	0x66e60000,0x78786c66,0xe666666c,0
;	dd	0x60f00000,0x60606060,0xfe666260,0
;	dd	0xeec60000,0xc6d6fefe,0xc6c6c6c6,0
;	dd	0xe6c60000,0xcedefef6,0xc6c6c6c6,0
;	dd	0xc67c0000,0xc6c6c6c6,0x7cc6c6c6,0
;	dd	0x66fc0000,0x607c6666,0xf0606060,0
;	dd	0xc67c0000,0xc6c6c6c6,0x7cded6c6,0xe0c
;	dd	0x66fc0000,0x6c7c6666,0xe6666666,0
;	dd	0xc67c0000,0xc3860c6,0x7cc6c606,0
;	dd	0x7e7e0000,0x1818185a,0x3c181818,0
;	dd	0xc6c60000,0xc6c6c6c6,0x7cc6c6c6,0
;	dd	0xc6c60000,0xc6c6c6c6,0x10386cc6,0
;	dd	0xc6c60000,0xd6d6c6c6,0x6ceefed6,0
;	dd	0xc6c60000,0x38387c6c,0xc6c66c7c,0
;	dd	0x66660000,0x183c6666,0x3c181818,0
;	dd	0xc6fe0000,0x30180c86,0xfec6c260,0
;	dd	0x303c0000,0x30303030,0x3c303030,0
;	dd	0x80000000,0x3870e0c0,0x2060e1c,0
;	dd	0xc3c0000,0xc0c0c0c,0x3c0c0c0c,0
;	dd	0xc66c3810,0,0,0
;	dd	0,0,0,0xff00
;	dd	0x183030,0,0,0
;	dd	0,0x7c0c7800,0x76cccccc,0
;	dd	0x60e00000,0x666c7860,0x7c666666,0
;	dd	0,0xc0c67c00,0x7cc6c0c0,0
;	dd	0xc1c0000,0xcc6c3c0c,0x76cccccc,0
;	dd	0,0xfec67c00,0x7cc6c0c0,0
;	dd	0x6c380000,0x60f06064,0xf0606060,0
;	dd	0,0xcccc7600,0x7ccccccc,0x78cc0c
;	dd	0x60e00000,0x66766c60,0xe6666666,0
;	dd	0x18180000,0x18183800,0x3c181818,0
;	dd	0x6060000,0x6060e00,0x6060606,0x3c6666
;	dd	0x60e00000,0x786c6660,0xe6666c78,0
;	dd	0x18380000,0x18181818,0x3c181818,0
;	dd	0,0xd6feec00,0xc6d6d6d6,0
;	dd	0,0x6666dc00,0x66666666,0
;	dd	0,0xc6c67c00,0x7cc6c6c6,0
;	dd	0,0x6666dc00,0x7c666666,0xf06060
;	dd	0,0xcccc7600,0x7ccccccc,0x1e0c0c
;	dd	0,0x6676dc00,0xf0606060,0
;	dd	0,0x60c67c00,0x7cc60c38,0
;	dd	0x30100000,0x3030fc30,0x1c363030,0
;	dd	0,0xcccccc00,0x76cccccc,0
;	dd	0,0x66666600,0x183c6666,0
;	dd	0,0xd6c6c600,0x6cfed6d6,0
;	dd	0,0x386cc600,0xc66c3838,0
;	dd	0,0xc6c6c600,0x7ec6c6c6,0xf80c06
;	dd	0,0x18ccfe00,0xfec66030,0
;	dd	0x180e0000,0x18701818,0xe181818,0
;	dd	0x18180000,0x18001818,0x18181818,0
;	dd	0x18700000,0x180e1818,0x70181818,0
;	dd	0xdc760000,0,0,0
