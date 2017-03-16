/**
*	created:		2012-6-24   20:48
*	filename: 		MACSender
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "MACSender.h"
//------------------------------------------------------------------------
typedef unsigned char UCHAR;
#define  PROC_NETBIOS UCHAR (WINAPI*)(NCB*)
//------------------------------------------------------------------------
BOOL NETTOOLS::GetMACAddress(char * MACAddress)
{

	if(NULL == MACAddress)
	{		
		return FALSE;
	}

	HINSTANCE hInstance = NULL;
	try
	{
		hInstance = LoadLibrary("NetApi32.dll");
		if(hInstance == NULL) 
			throw 1;//TEXT("can not load Netapi32.dll");
		UCHAR (WINAPI *fnNetbios)(NCB*);

		fnNetbios=(PROC_NETBIOS)GetProcAddress(hInstance,"Netbios");
		if(fnNetbios == NULL)
			throw "can not GetProcAddress(NetBios)";
		typedef struct _ASTAT_
		{
			ADAPTER_STATUS adapt;
			NAME_BUFFER    NameBuff [30];
		}ASTAT, * PASTAT;
		ASTAT Adapter;

		NCB Ncb;
		UCHAR uRetCode;
		LANA_ENUM   lenum;

		memset( &Ncb, 0, sizeof(Ncb) );
		Ncb.ncb_command = NCBENUM;
		Ncb.ncb_buffer = (UCHAR *)&lenum;
		Ncb.ncb_length = sizeof(lenum);
		uRetCode = fnNetbios( &Ncb );

		if(uRetCode != NRC_GOODRET)
			throw "NCBENUM error";
		if(lenum.length == 0)
		{
			throw "lenum.length == 0";
			memset(MACAddress, 0, 17);  //把网卡号清空
			return FALSE;
		}

		// 问题 : 从后向前? 在我的win2000下竟然取出4个网卡地址,序号分别为6,5,4,7,前三个MAC一样,而只有最后一个是对的,奇怪
		//        所以现在从后向前取
		int nLoop = 0;
		for(int i = lenum.length - 1; i >= 0; i--)
		{
			memset( &Ncb, 0, sizeof(Ncb) );
			Ncb.ncb_command = NCBRESET;
			Ncb.ncb_lana_num = lenum.lana[i];

			uRetCode = fnNetbios( &Ncb );			
			if(uRetCode != NRC_GOODRET)
				throw "NCBRESET error";

			memset( &Ncb, 0, sizeof (Ncb) );
			Ncb.ncb_command = NCBASTAT;
			Ncb.ncb_lana_num = lenum.lana[i];

			strcpy((char*)Ncb.ncb_callname,  "*               " );
			Ncb.ncb_buffer = (BYTE*) &Adapter;
			Ncb.ncb_length = sizeof(Adapter);

			uRetCode = fnNetbios( &Ncb );

			if(uRetCode != NRC_GOODRET)
				throw "NCBASTAT error";
			sprintf(MACAddress, "%02X-%02X-%02X-%02X-%02X-%02X", 
				Adapter.adapt.adapter_address[0],
				Adapter.adapt.adapter_address[1],
				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5]);
			break;
		}
		if (hInstance) 
			FreeLibrary(hInstance);

		return TRUE;
	}
	catch (...)
	{
		memset(MACAddress, 0, 17);  //把网卡号清空
		if(hInstance) FreeLibrary(hInstance);
		return FALSE;
	}

	memset(MACAddress, 0, 17);  //把网卡号清空
	return FALSE;
}
//------------------------------------------------------------------------
CMACSender::CMACSender()
{
	m_nMacPosition = 0;
}
//------------------------------------------------------------------------
CMACSender::~CMACSender()
{

}
//------------------------------------------------------------------------
void CMACSender::SetMacPosition(DWORD dwPos)
{
	m_nMacPosition = dwPos;
}
//------------------------------------------------------------------------
int CMACSender::GetMacPosition(void)
{
	return abs(m_nMacPosition) % 490;
}
//------------------------------------------------------------------------
LPCSTR CMACSender::GetMacPackage()
{	
	char szSerialNO[20];		
	NETTOOLS::GetMACAddress(szSerialNO);

	srand(GetTickCount());
	for (int i = 0; i < 512; ++i)
	{
		//在这里把内存打乱
		m_Mac[i] = rand();
	}

	int nPos = GetMacPosition();
	for ( int i = 0; i < 20; ++i)
	{
		m_Mac[nPos + i] = szSerialNO[i] ^ m_Mac[511];
	}

	return m_Mac;
}
//------------------------------------------------------------------------