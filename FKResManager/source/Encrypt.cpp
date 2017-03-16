/**
*	created:		2012-6-18   20:24
*	filename: 		Encrypt
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../Include/ResMgrCommon.h"
#include "../Include/Encrypt.h"
//------------------------------------------------------------------------
CEncrypt::CEncrypt()
{

}
//------------------------------------------------------------------------
CEncrypt::~CEncrypt()
{

}
//------------------------------------------------------------------------
BOOL CEncrypt::Encrypt(BYTE* pBuffer, int nSize, BYTE* pKey)
{
	return MakeSimpleMap(pBuffer, nSize);
}
//------------------------------------------------------------------------
BOOL CEncrypt::Decrypt(BYTE* pBuffer, int nSize, BYTE* pKey)
{
	return MakeSimpleMap(pBuffer, nSize);
}
//------------------------------------------------------------------------
BOOL CEncrypt::Encrypt(BYTE* pInput, BYTE* pOutput, int nSize, BYTE* pKey)
{
	memcpy(pOutput, pInput, nSize);
	return MakeSimpleMap(pOutput, nSize);
}
//------------------------------------------------------------------------
BOOL CEncrypt::Decrypt(BYTE* pInput, BYTE* pOutput, int nSize, BYTE* pKey)
{
	memcpy(pOutput, pInput, nSize);
	return MakeSimpleMap(pOutput, nSize);
}
//------------------------------------------------------------------------
// 作简单异或运算
// 速度测试结果:一个130K的buffer经过10000次MakeSimpleMap操作，耗时1000ms
BOOL CEncrypt::MakeSimpleMap(BYTE* buf, int nLen)
{
	__asm{
		// eax:(nLen/8)
		// ebx:尾部经过8字节对齐后剩余的小于8字节的字节数所占的位数
		// ecx:64-ebx
		// mm7:key
		mov esi, dword ptr buf;
		mov eax, nLen;
		pcmpeqd mm7, mm7;
		movd mm6, eax;
		mov ebx, eax;
		psllq mm6, 32;
		movd mm5, ebx;
		and ebx, 7;
		paddd mm6, mm5;
		shr eax, 3;
		pxor mm7, mm6;
		align 4;
_loop:	
		cmp eax, 0;
		jz _tail;
		movq mm0, [esi];
		pxor mm0, mm7;
		movq [esi], mm0;
		add esi,8;
		dec eax;
		jmp _loop;
_tail:
		cmp ebx, 0;
		jz _done;

		// 处理尾部残余的字节
		pcmpeqd mm1, mm1;
		pcmpeqd mm2, mm2;
		shl ebx, 3;
		mov ecx, 64;
		movq mm0, [esi];
		sub ecx, ebx;
		movd mm3, ebx;
		movd mm4, ecx;
		psllq mm1, mm3;
		psrlq mm2,mm4;
		pand mm1, mm0;
		por mm1, mm2;
		pxor mm0, mm7;
		pand mm0, mm1;
		movq [esi], mm0;
_done:
		emms;
	}
	return TRUE;
}
//------------------------------------------------------------------------