/**
*	created:		2012-2-29   4:53
*	filename: 		FKRsaEncryptor
*	author:			FreeKnight
*	Copyright (C): 	TianCong (2012) FreeKnightServerEngine
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "FKBigInt.h"
#include <string>
//------------------------------------------------------------------------
class FKRsaEncryptor
{
public:
	// 设置明文种子
	static bool Init( char* p_szSeed );
	// 生成密文
	static CBigInt GetEncryptResult();
	// 判断密文是否合法
	static bool	IsValidEncrypt( CBigInt& p_EncryptCode );
	// 生成随机字符串
	static void RandStr( char* p_szOutStr, const int p_nLen = BI_MAXLEN );
};