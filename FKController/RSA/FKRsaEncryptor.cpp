/**
*	created:		2012-2-29   6:00
*	filename: 		FKRsaEncryptor
*	author:			FreeKnight
*	Copyright (C): 	TianCong (2012) FreeKnightServerEngine
*	purpose:		
*/
//------------------------------------------------------------------------
#include "FKRsaEncryptor.h"
//------------------------------------------------------------------------
// 默认种子
#define VALUE_1 0x0515
#define VALUE_2	0x0728
#define VALUE_3 0x0928
#define VALUE_4 0x0927
//------------------------------------------------------------------------
CBigInt g_BigIntSeed;
static const int g_nStrLEN = 62; // 26 + 26 + 10
static const char g_arrCharElem[g_nStrLEN] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
	'u', 'v', 'w', 'x', 'y', 'z'
};
//------------------------------------------------------------------------
// 设置明文种子
bool FKRsaEncryptor::Init( char* p_szSeed )
{
	g_BigIntSeed.m_nLength=4;  
	g_BigIntSeed.m_ulValue[3]=VALUE_1;  
	g_BigIntSeed.m_ulValue[2]=VALUE_2;  
	g_BigIntSeed.m_ulValue[1]=VALUE_3;  
	g_BigIntSeed.m_ulValue[0]=VALUE_4; 

	if( p_szSeed == NULL )
	{
		return false;
	}

	int nTemp = 0;
	for( int i=0;i< BI_MAXLEN; ++i )
	{
		// 大写
		if( p_szSeed[i]>='a' && p_szSeed[i]<='z' )
		{
			nTemp = p_szSeed[i];
			nTemp -= 97;
			g_BigIntSeed.m_ulValue[i]	= nTemp;
			nTemp = 0;
		}
		// 小写
		else if( p_szSeed[i]>='A' && p_szSeed[i]<='Z' )
		{
			nTemp = p_szSeed[i];
			nTemp -= 65;
			g_BigIntSeed.m_ulValue[i]	= p_szSeed[i];
			nTemp = 0;
		}
		// 数字
		else if( p_szSeed[i] >= '0' && p_szSeed[i] <= '9' )
		{
			nTemp = p_szSeed[i];
			nTemp -= 48;
			g_BigIntSeed.m_ulValue[i]	= p_szSeed[i];
			nTemp = 0;
		}
		else
		{
			return false;
		}
	}
	return true;
}
//------------------------------------------------------------------------
// 生成密文
CBigInt FKRsaEncryptor::GetEncryptResult()
{
	CBigInt e;
	CBigInt n;
	CBigInt c;

	e.m_nLength=1;  
	e.m_ulValue[0]=0x34703;  

	n.m_nLength=4;  
	n.m_ulValue[3]=0x78CDED41;   
	n.m_ulValue[2]=0xF71A0BF9;  
	n.m_ulValue[1]=0xA1F36E0A;  
	n.m_ulValue[0]=0x60F3EE89;  

	c.Mov(g_BigIntSeed.ModExp(e,n));  

	return c;
}
//------------------------------------------------------------------------
// 判断密文是否合法
bool FKRsaEncryptor::IsValidEncrypt( CBigInt& p_EncryptCode )
{
	CBigInt d;
	CBigInt n;
	CBigInt m;

	d.m_nLength=4;  
	d.m_ulValue[3]=0x4C9327F3;  
	d.m_ulValue[2]=0x70B028FD;  
	d.m_ulValue[1]=0x08D56387;  
	d.m_ulValue[0]=0xFCC6161B;  

	n.m_nLength=4;  
	n.m_ulValue[3]=0x78CDED41; 
	n.m_ulValue[2]=0xF71A0BF9;  
	n.m_ulValue[1]=0xA1F36E0A;  
	n.m_ulValue[0]=0x60F3EE89;  

	m.Mov(p_EncryptCode.ModExp(d,n));  

	for( int i = 0; i < 4; ++i )
	{
		if( m.m_ulValue[i] != g_BigIntSeed.m_ulValue[i] )
		{
			return false;
		}
	}

	return true;
}
//------------------------------------------------------------------------
// 生成随机字符串
void FKRsaEncryptor::RandStr( char* p_szOutStr, const int p_nLen )
{
	p_szOutStr[p_nLen] = '\0';
	int iRand = 0;
	for (int i = 0; i < p_nLen; ++i)
	{
		iRand = rand() % g_nStrLEN;				// iRand = 0 - 61
		p_szOutStr[i] = g_arrCharElem[iRand];
	}
}
//------------------------------------------------------------------------