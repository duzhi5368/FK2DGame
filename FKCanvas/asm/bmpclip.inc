		mov eax,[ecx]WBmpStruct.ky		; 目标位图的y向开始位置
		mov edi,[ecx]WBmpStruct.ptr		; 目标位图的数据指针
		mov edx,[ebx]WBmpStruct.h		; 读出源位图的高度
		mov esi,[ebx]WBmpStruct.ptr		; 源位图的数据指针
		mov h,edx
		sub eax,[ebx]WBmpStruct.ky		; eax = BLIT 从源位图的行位置
		jns _under_firstline
		
		add h,eax						; 修正需要BLIT的行数
		jle _endclip
		imul eax,[ebx]WBmpStruct.pitch	; 算出 eax 行的指针偏移 (负的)
		sub esi,eax
		mov eax,h
		jmp _adjust_height
_endclip:
		;xor eax,eax
		;mov eax, 1
		;neg eax
		;ret
		jmp _end
		
_under_firstline:
		cmp eax,[ecx]WBmpStruct.h	; 比较 this 位图的高度
		jge _endclip
		mov edx,eax
		add eax,h
		imul edx,[ecx]WBmpStruct.pitch	; 修正 this 位图指针
		add edi,edx
		
_adjust_height:
		sub eax,[ecx]WBmpStruct.h	; bmp 图的下端和 this 图下端差(负的)
		jl _dealwith_width
		sub h,eax		; 修正需要 blit 的行数
		
_dealwith_width:
		mov edx,[ebx]WBmpStruct.w	; 读出 bmp 的宽度
		mov eax,[ecx]WBmpStruct.kx
		mov w,edx
		sub eax,[ebx]WBmpStruct.kx	; eax = BLIT 到 this 位图的列位置
		jns _right_firstcol
		
		add w,eax		; 修正需要 BLIT 的列数
		jle _endclip
		sub esi,eax
		sub esi,eax			; 修正源数据指针
		mov eax,w
		jmp _adjust_width
		
_right_firstcol:
		cmp eax,[ecx]WBmpStruct.w	; 比较 this 位图的宽度
		jge _endclip			; 超出位图则跳转
		lea edi,[edi+eax*2]		; 修正 this 位图指针
		add eax,w
		
_adjust_width:
		sub eax,[ecx]WBmpStruct.w	; bmp 图的右端和 this 位图右端差(负的)
		jl _blit
		sub w,eax