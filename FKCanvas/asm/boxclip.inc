	mov eax,[ecx]WBmpStruct.ky
	mov edi,[ecx]WBmpStruct.ptr	; 目标位图的数据指针
	mov edx,[ebx]WBoxStruct.h	; 读出矩形的高度
	mov h,edx
	sub eax,[ebx]WBoxStruct.ky	; eax = BLIT 从源位图的行位置
	jns _under_firstline

	add h,eax		; 修正需要BLIT的行数
	jle _endclip
	mov eax,h
	jmp _adjust_height
_endclip:
	xor eax,eax
	ret -1
	
_under_firstline:
	cmp eax,[ecx]WBmpStruct.h	; 比较目标位图的高度
	jge _endclip
	mov edx,eax
	add eax,h
	imul edx,[ecx]WBmpStruct.pitch	; 修正目标位图指针
	add edi,edx

_adjust_height:
	
	sub eax,[ecx]WBmpStruct.h	; Box 的下端和 目标图下端差(负的)
	jl _dealwith_width
	sub h,eax		; 修正需要 blit 的行数

_dealwith_width:
	mov edx,[ebx]WBoxStruct.w	; 读出 box 的宽度
	mov eax,[ecx]WBmpStruct.kx
	mov w,edx
	sub eax,[ebx]WBmpStruct.kx	; eax = BLIT 到目标位图的列位置
	jns _right_firstcol

	add w,eax		; 修正需要 BLIT 的列数
	jle _endclip
	mov eax,w
	jmp _adjust_width

_right_firstcol:
	cmp eax,[ecx]WBmpStruct.w	; 比较目标位图的宽度
	jge _endclip			; 超出位图则跳转
	lea edi,[edi+eax*2]		; 修正 this 位图指针
	add eax,w

_adjust_width:
	sub eax,[ecx]WBmpStruct.w	; bmp 图的右端和目标位图右端差(负的)
	jl _blit
	sub w,eax

