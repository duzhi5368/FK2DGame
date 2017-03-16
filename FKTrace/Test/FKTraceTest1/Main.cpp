/**
*	created:		2012-6-15   0:32
*	filename: 		Main
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../IFKTrace.h"
#include <iostream>
#include <string>
using namespace std;
//------------------------------------------------------------------------
CFKTrace g_Trace;
//------------------------------------------------------------------------
bool TestLogicLoop()
{
	string str;
	str.clear();
	cin >> str;
	g_Trace.ThreadSafeShowTraceLnWithTime( str.c_str()  );
	g_Trace.ShowTraceLn( str.c_str(), RGB(255, 0, 0), 12 );
	return true;
}
//------------------------------------------------------------------------
int main()
{
	g_Trace.Create( GetModuleHandle(NULL),
		OS_WINDOW |
		OS_FILE |
		OS_OUTPUTDEBUGSTRING,
		/*GetConsoleWindow(), */
		NULL,
		NULL,
		"Log\\trace.txt", 
		FALSE);

	MSG msg;
	while( 1 )
	{
		while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if(WM_QUIT == msg.message)
			{
				break;
			}

			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		TestLogicLoop();
		Sleep(0);
	}

	getchar();
	g_Trace.Close();
	return 0;
}