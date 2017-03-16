/**
*	created:		2012-6-24   20:04
*	filename: 		Sha1
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//------------------------------------------------------------------------
typedef unsigned __int64 UINT64;
#define NULL 0
//------------------------------------------------------------------------
// The standard SHA1 needs the input string to fit into a block
// This function align the input string to meet the requirement
void AlignSHA1(const char *pIn,UINT64 *pBlks, UINT64 &nBlksLen)
{
	if (pIn == NULL)
	{
		return;
	}
	UINT64 nInLen = strlen(pIn);
	UINT64 nBlk = ((nInLen+8)>>6)+1;
	if (pBlks == NULL)
	{
		nBlksLen = nBlk*16;
		return;
	}
	for (int j=0; j<nBlksLen; j++)
	{
		pBlks[j] = 0;
	}
	for(UINT64 i=0; i<nInLen; i++)
	{		
		pBlks[i>>2] |= pIn[i]<<(24-(i&3)*8);		
	}
	pBlks[i>>2] |= 0x80<<(24-(i&3)*8);
	pBlks[nBlksLen-1]=(int)(nInLen*8);
	return ;
}
//------------------------------------------------------------------------
// The int32 add function which doesn't generate overflow
// exception. This is required by the algorithm

UINT64 add(UINT64 x, UINT64 y)
{
	UINT64 lsw = (x&0xffff)+(y&0xffff);
	UINT64 msw = (x>>16) + (y>>16) +(lsw>>16);
	return (msw<<16) | (lsw&0xffff);
}
//------------------------------------------------------------------------
UINT64 UnsigedRightMove(UINT64 num, UINT64 bit)
{	
	num &=0xffffffff;
	num >>= bit ;	
	return num;	
}
//------------------------------------------------------------------------
// The int32 _asm rol :)
UINT64 rol(UINT64 num,UINT64 cnt)
{		
	return(num<<cnt)|UnsigedRightMove(num, (32-cnt));
}
//------------------------------------------------------------------------
// Perform the appropriate triplet combination function for the current round
UINT64 ft(UINT64 t, UINT64 b, UINT64 c, UINT64 d)
{
	if(t<20)return(b&c)|((~b)&d);
	if(t<40)return b^c^d;
	if(t<60)return(b&c)|(b&d)|(c&d);
	return b^c^d;
}
//------------------------------------------------------------------------
// Determine the appropriate additive constant for the current iteration
UINT64 kt(UINT64 t) 
{
	return(t<20)?1518500249:(t<40)?1859775393:
		(t<60)?-1894007588:-899497514;
}
//------------------------------------------------------------------------
char* Uint64ToHexStr(UINT64 num, char *buf, int nLen)
{
	if (buf == NULL || nLen<8)
	{
		return buf;
	}
	char szTemp[9]={0};
	ultoa((unsigned long)num, szTemp, 16);
	static char *s_pZero[]=
	{
		"0",
		"00",
		"000"
		"0000",
		"00000",
		"000000",
		"0000000",
		"00000000",
		"000000000",
	};
	int nRealLen = strlen(szTemp);
	int nSpareBit = 7-nRealLen;
	if (nSpareBit >=0 && nSpareBit <=7)
	{
		memcpy(buf, s_pZero[nSpareBit], nSpareBit+1);
		memcpy(buf+ nSpareBit+1, szTemp, nRealLen);
	}
	else
	{
		memcpy(buf, szTemp, 8);
	}

	return buf;
}
//------------------------------------------------------------------------
char* SHA1(const unsigned char *pIn, char *pOut, int nOutLen)
{		
	if (pIn == NULL)
	{
		return pOut;
	}

	UINT64 nBlksLen = 0;
	AlignSHA1((const char*)pIn, NULL, nBlksLen);
	UINT64 *pBlks = new UINT64[(int)nBlksLen];

	if (pBlks == NULL)
	{
		return pOut;
	}
	AlignSHA1((const char*)pIn, pBlks, nBlksLen);	
	UINT64 w[80] ={0};
	UINT64 a=1732584193;
	UINT64 b=-271733879;
	UINT64 c=-1732584194;
	UINT64 d=271733878;
	UINT64 e=-1009589776;

	for (UINT64 i=0; i<nBlksLen; i+=16)
	{
		UINT64 olda=a;
		UINT64 oldb=b;
		UINT64 oldc=c;
		UINT64 oldd=d;
		UINT64 olde=e;
		for (UINT64 j=0; j<80; j++)
		{
			if (j<16)
			{
				w[j]=pBlks[i+j];
			}
			else
			{
				w[j]=rol(w[j-3]^w[j-8]^w[j-14]^w[j-16],1);
			}
			UINT64 t=add(add(rol(a,5),ft(j,b,c,d)),add(add(e,w[j]),kt(j)));
			e=d;
			d=c;
			c=rol(b,30);
			b=a;
			a=t;
		}
		a=add(a,olda);
		b=add(b,oldb);
		c=add(c,oldc);
		d=add(d,oldd);
		e=add(e,olde);
	}	
	Uint64ToHexStr(a, pOut, nOutLen);
	Uint64ToHexStr(b, pOut+8, nOutLen-8);
	Uint64ToHexStr(c, pOut+16, nOutLen-16);
	Uint64ToHexStr(d, pOut+24, nOutLen-24);
	Uint64ToHexStr(e, pOut+32, nOutLen-32);
	if (pBlks != NULL)
	{
		delete[] pBlks;		
		pBlks = NULL;
	}
	return pOut;
}
//------------------------------------------------------------------------