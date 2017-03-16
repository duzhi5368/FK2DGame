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
;		Description:		汇编头文件
;		Original Author:	云风
;		Authors:
;		Create Time:		2000/9/23
;		Modify Time:		2001/8/24
;
;:*W*:._.:*I*:._.:*N*:._.:*D*:._.:*S*:._.:*O*:._.:*U*:._.:*L*:._.:

%ifndef _WPP_STRUCT_
%define _WPP_STRUCT_

%macro struct 1
	resb %1_size
%endmacro 

%define __SECT__

	struc WBmpStruct
.kx:	resd	1
.ky:	resd	1
.w:	resd	1
.h:	resd	1
.pitch:	resd	1
.ptr:	resd	1
;.mem:	resd	1
.data:	resd	1
.bChild resd	1
	endstruc

	struc WAlphaBmpStruct
.bmp:	struct	WBmpStruct
.channel:	resd	1
.cmem:	resd	1
	endstruc

	struc WCharStruct
.kx:	resd	1
.ky:	resd	1
.w:	resd	1
.h:	resd	1
.data:	resd	1
.color:	resw	1
.width:	resw	1
	endstruc

	struc WBoxStruct
.kx:	resd	1
.ky:	resd	1
.w:	resd	1
.h:	resd	1
.data:	resd	1
.color:	resw	1
	endstruc

%endif

