/**
*	created:		2012-6-24   3:02
*	filename: 		TimeAxisOutput
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/TimeAxis.h"
#include "../Include/TimeAxisAux.h"
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) ITimeAxis* CreateAxis(DWORD dwVer)
{
	CTimeAxisAux::Instance()->TimerInit();

	CTimeAxis * pAxis = new CTimeAxis;
	pAxis->InitTimerList();

	return pAxis;
}
//------------------------------------------------------------------------