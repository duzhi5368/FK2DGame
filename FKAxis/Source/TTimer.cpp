/**
*	created:		2012-6-24   3:04
*	filename: 		TTimer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include <windows.h>
#include "../Include/IStrategy.h"
#include "../Include/TTimer.h"
#include "../Include/TimeAxisAux.h"
//------------------------------------------------------------------------
#define NTPORT		".\\NTPort.dll"
//------------------------------------------------------------------------
extern "C" 
{
	// Returns a value indicating the last operation is successful or not.
	typedef int (APIENTRY *fGetLastState)(char *s);
	// Enable your application to read or write specific ports.
	typedef void (APIENTRY *fEnablePorts)(WORD PortStart, WORD PortStop);
	// Disable your application to read or write specific ports.
	typedef void (APIENTRY *fDisablePorts)(WORD PortStart, WORD PortStop);
	// Returns a value from specific ports.
	typedef WORD (APIENTRY *fInport)(WORD PortNum);
	// Write a value to specific ports.
	typedef void (APIENTRY *fOutport)(WORD PortNum, WORD Data);
};
//------------------------------------------------------------------------
fInport pInport	= 0;
fOutport pOutport = 0;
fGetLastState pGetLastState = 0;
//------------------------------------------------------------------------
bool _stdcall TTimerGetPortVal(WORD wPortAddr, PDWORD pdwPortVal, BYTE bSize)
{
	if(pInport && pOutport && pGetLastState)
	{
		WORD wValue = pInport(wPortAddr);
		int iRet = pGetLastState(0);
		if(iRet != 0)return false;

		*pdwPortVal = wValue;

		return true;
	}
	return false;
}
//------------------------------------------------------------------------
bool _stdcall TTimerSetPortVal(WORD wPortAddr, DWORD dwPortVal, BYTE bSize)
{
	if(pInport && pOutport && pGetLastState)
	{
		pOutport(wPortAddr,dwPortVal);

		int iRet = pGetLastState(0);
		if(iRet != 0)return false;

		return true;
	}
	return false;
}
//------------------------------------------------------------------------
BOOL TTimer::TimerInit()
{
	if(CalcCpuFreq())
	{
		m_ui64Begin = GetRdtscCycleCount();
		return TRUE;
	}

	return FALSE;
}
//------------------------------------------------------------------------
BOOL TTimer::CalcCpuFreq()
{
	HINSTANCE hInstance = LoadLibrary(NTPORT);
	if(hInstance)
	{
		pGetLastState	= (fGetLastState)GetProcAddress(hInstance,"GetLastState");
		fEnablePorts pEnablePorts	= (fEnablePorts)GetProcAddress(hInstance,"EnablePorts");
		fDisablePorts pDisablePorts	= (fDisablePorts)GetProcAddress(hInstance,"DisablePorts");
		pInport	= (fInport)GetProcAddress(hInstance,"Inport");
		pOutport	= (fOutport)GetProcAddress(hInstance,"Outport");

		if(pGetLastState && pEnablePorts && pDisablePorts && pInport && pOutport)
		{
			//	pEnablePorts(0x40,0x43);
			char s[256];
			int iRet = pGetLastState(s);
			if(iRet == 0)
			{
				g_pfGetPortVal = TTimerGetPortVal;
				g_pfSetPortVal = TTimerSetPortVal;
				m_ui64Freq = GetCpuFreqEx();
			}
			//	pDisablePorts(0x40,0x43);
		}
		FreeLibrary(hInstance);
	}

	return m_ui64Freq > 0;
}
//------------------------------------------------------------------------