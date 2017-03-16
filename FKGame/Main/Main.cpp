/**
*	created:		2012-6-20   23:02
*	filename: 		Main
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "PreInclude.h"
#include "../SystemInfo/SystemInfo.h"
#include "../CrashHelper/CrashReport.h"
#include "../PrintScreen/PrintScreen.h"
#include "../Network/IAppNetworkLink.h"
#include "../Common/WinMsgDef.h"
#include "../ProcessProtect/ProcessLimitGuard.h"
#include "../Common/CreaturePropertyDef.h"
//------------------------------------------------------------------------
LRESULT CALLBACK		WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK		About(HWND,   UINT, WPARAM, LPARAM);
//------------------------------------------------------------------------
HINSTANCE				g_hInstance = NULL;
BOOL					g_bActive = FALSE;
DWORD					g_dwDxVer = 0;
DWORD					TICK_NEED = 30;			// 标准建议两帧间隔时间
DWORD					TICK_VAR = 3;
//------------------------------------------------------------------------
#define WM_GM_ENTER		(WM_USER+1011)
//------------------------------------------------------------------------
void TraceSysInfo(void)
{
	siCpu cpuType;
	siInt osType;

	char strTemp[256];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(strTemp,"%d/%d/%d %d:%d:%d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	PrintEventLn("--------------- 当前时间 ---------------");
	PrintEventLn(strTemp);
	PrintEventLn("--------------- 系统信息 ---------------");
	PrintEventLn("");

	OSVERSIONINFO osInfo;
	osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osInfo);

	strcpy(strTemp,"操作系统 : ");
	strcat(strTemp,siGetOsName());
	strcat(strTemp," ");
	strcat(strTemp,osInfo.szCSDVersion);
	PrintEventLn(strTemp);
	PrintEventLn("系统版本 : %d.%d.%d",osInfo.dwMajorVersion,osInfo.dwMinorVersion,osInfo.dwBuildNumber);

	osType = siGetOsType();
	strcpy(strTemp,"系统类型 : ");

	switch(osType)
	{
		case SI_OS_NT:
			strcat(strTemp,"SI_OS_NT");
			break;

		case SI_OS_2K:
			strcat(strTemp,"SI_OS_2K");
			break;

		case SI_OS_XP:
			strcat(strTemp,"SI_OS_XP");
			break;

		case SI_OS_DOTNET:
			strcat(strTemp,"SI_OS_DOTNET");
			break;
			
		case SI_OS_95:
			strcat(strTemp,"SI_OS_95");
			break;

		case SI_OS_98:
			strcat(strTemp,"SI_OS_98");
			break;

		case SI_OS_ME:
			strcat(strTemp,"SI_OS_ME");
			break;

		case SI_OS_LINUX:
			strcat(strTemp,"SI_OS_LINUX");
			break;

		case SI_OS_FREEBSD:
			strcat(strTemp,"SI_OS_FREEBSD");
			break;

		default:
			strcat(strTemp,"SI_OS_UNKNOWN");
			break;
	}

	PrintEventLn(strTemp);

	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo); 
	PrintEventLn("");
	PrintEventLn("CPU ID: %s", "PROCESSOR_ARCHITECTURE_INTEL");
	PrintEventLn("CPU数量 : %u",siSysInfo.dwNumberOfProcessors); 

	UINT64 ui64Freq = 0;
	ITimeAxisAux* pAxisAux = g_ClientGlobal.m_TimerAxisHelper.GetTimeAxisAux();
	if(pAxisAux)
	{
		ui64Freq = pAxisAux->GetCpuFreq();
	}
	PrintEventLn("CPU速度 : ~%u mhz", ui64Freq / 1000000);
	//CPU
	cpuType = siGetCpuType();
	PrintEventLn("CPU标识 : %s",cpuType.idString);
	PrintEventLn("");

	//CPU支持的特性
	PrintEventLn("MMX: %s", cpuType.hasMMX ? "yes" : "no");
	PrintEventLn("SSE: %s", cpuType.hasSSE ? "yes" : "no");
	PrintEventLn("SSE2: %s", cpuType.hasSSE2 ? "yes" : "no");
	PrintEventLn("3dNow!: %s", cpuType.has3dNow ? "yes" : "no");
	PrintEventLn("");

	MEMORYSTATUS stat;
	GlobalMemoryStatus (&stat);
	
	PrintEventLn("内存使用百分比 : %ld",stat.dwMemoryLoad);
	PrintEventLn("%*ld %sB 物理内存.",12, stat.dwTotalPhys/1024, "K");
	PrintEventLn("%*ld %sB 可用物理内存.",12, stat.dwAvailPhys/1024, "K");
	PrintEventLn("");

	PrintEventLn("%*ld %sB 页面文件.",12, stat.dwTotalPageFile/1024, "K");
	PrintEventLn("%*ld %sB 可用页面文件.",12, stat.dwAvailPageFile/1024, "K");
	PrintEventLn("");

	PrintEventLn("%*ld %sB 虚拟内存.",12, stat.dwTotalVirtual/1024, "K");
	PrintEventLn("%*ld %sB 可用虚拟内存.",12, stat.dwAvailVirtual/1024, "K");
	PrintEventLn("");
	PrintEventLn("显示信息");

	DEVMODE dm;
	if(EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dm))
	{
		PrintEventLn("显示模式 : %dX%dX%d %d",dm.dmPelsWidth,dm.dmPelsHeight,dm.dmBitsPerPel,dm.dmDisplayFrequency);
	}

	PrintEventLn("");

	struct 
	{
		char szDescription[64];
		char szVer[64];
		DWORD dwVer;
	}DxVer[] = 
	{
		{"4.05.00.0155",		"5.0"		,5},
		{"4.05.01.1721",		"5.0"		,5},
		{"4.05.01.1998",		"5.0"		,5},
		{"4.06.02.0436",		"6.0"		,6},
		{"4.07.00.0700",		"7.0"		,7},
		{"4.07.00.0716",		"7.0a"		,7},
		{"4.08.00.0400",		"8.0"		,8},
		{"4.08.01.0810",		"8.1"		,8},
		{"4.08.01.0881",		"8.1"		,8},
		{"4.09.00.0900",		"9.0"		,9},
		{"4.09.00.0901",		"9.0a"		,9},
		{"4.09.00.0902",		"9.0b"		,9},
		{"4.09.00.0904",		"9.0c"		,9}
	};

	char szVer[256];
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\DirectX",0,KEY_READ,&hKey) == ERROR_SUCCESS)
	{
		DWORD dwSize = 256;
		if(RegQueryValueEx(hKey,"Version",0,0,(LPBYTE)szVer,&dwSize) == ERROR_SUCCESS)
		{
			char szDesc[256];
			strcpy(szDesc,"9.0c +");

			for(int i = 0;i < sizeof(DxVer) / sizeof(DxVer[0]);i++)
				if(string(DxVer[i].szDescription) == string(szVer))
				{
					strcpy(szDesc,DxVer[i].szVer);
					g_dwDxVer = DxVer[i].dwVer;
					break;
				}
			PrintEventLn("DirectX Version = %s",szDesc);
			PrintEventLn("");
		}
		RegCloseKey(hKey);
	}

	PrintEventLn("--------------- 信息完毕 ---------------");
}
//------------------------------------------------------------------------
// 逻辑主函数入口
//------------------------------------------------------------------------
int WinMainEx(HINSTANCE hInstance,
              HINSTANCE hPrevInstance,
			  LPSTR     lpCmdLine,
			  int       nCmdShow)
{
	g_hInstance = hInstance;
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	// 初始化 richedit
	// LoadLibrary("RICHED32.DLL");

	// 设置标题
	TCHAR szTitle[128];
	memset( szTitle, 0, sizeof(szTitle) );
	memcpy( szTitle, WIN_TITLE_NAME, sizeof(string(WIN_TITLE_NAME)) );
	
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hIconSm		= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= WIN_CLASS_NAME;
	wcex.hIconSm		= NULL;

	// 注册窗口类
	if( !RegisterClassEx(&wcex) )
	{
		return -1;
	}

	int nWidth = GAME_WINDOW_WIDTH + 2 * ( GetSystemMetrics(SM_CXDLGFRAME) + GetSystemMetrics(SM_CXBORDER));
	int nHeight = GAME_WINDOW_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) + GetSystemMetrics(SM_CXBORDER);

	HWND hWnd = CreateWindow(WIN_CLASS_NAME, szTitle, WS_CLIPCHILDREN|WS_POPUP|WS_CAPTION|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU,
		0,0,0,0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return FALSE;

	DWORD	dwScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	DWORD	dwScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	RECT rcWindow = {0,0,GAME_WINDOW_WIDTH,GAME_WINDOW_HEIGHT};
	AdjustWindowRectEx(&rcWindow,GetWindowLong(hWnd,GWL_STYLE),GetMenu(hWnd) != 0,GetWindowLong(hWnd,GWL_EXSTYLE));
	rcWindow.right = rcWindow.right - rcWindow.left;
	rcWindow.bottom = rcWindow.bottom - rcWindow.top;
	rcWindow.left=(int)((int)dwScreenWidth-(int)rcWindow.right)/2;
	rcWindow.top=(int)((int)dwScreenHeight-(int)rcWindow.bottom)/2;
	rcWindow.right+=rcWindow.left;
	rcWindow.bottom+=rcWindow.top;

	MoveWindow(hWnd,rcWindow.left,rcWindow.top,rcWindow.right-rcWindow.left,rcWindow.bottom-rcWindow.top,TRUE);
	
	// 去掉文件夹的只读属性
	char szWorkDir[MAX_PATH] = "";
	GetCurrentDirectory(MAX_PATH, szWorkDir);
	::SetFileAttributes(szWorkDir, FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_NORMAL);

	// 创建文件夹
	char szLogDirectoryName[MAX_PATH];
	wsprintf(szLogDirectoryName, LOG_PATH, szWorkDir);
	::CreateDirectory(szLogDirectoryName, NULL);

#ifdef _DEBUG
	g_DefaultTrace.Create(hInstance, OS_WINDOW | OS_FILE | OS_OUTPUTDEBUGSTRING, hWnd, NULL, TRACE_LOG_FILE, FALSE);
#else
	g_DefaultTrace.Create(hInstance,
							  OS_FILE,
							  hWnd, 
							  NULL, 
							  TRACE_LOG_FILE,
							  FALSE);

	UINT nDebug = GetPrivateProfileInt("Option","Debug",0,".\\config.ini");
	if(nDebug)
	{
		g_DefaultTrace.Create(hInstance,
								  OS_WINDOW | OS_FILE | OS_OUTPUTDEBUGSTRING,
								  hWnd,
								  NULL,
								  TRACE_LOG_FILE,
								  FALSE);
	}
#endif

	// 游戏逻辑模块初始化
	if(g_GameApp.InitGame(hInstance, hWnd, NULL, g_DefaultTrace.GetTraceObject()) == FALSE)
	{
		MessageBox(hWnd,"初始化游戏系统失败","出错",MB_OK);
		return -3;
	}

	// 读取区名和游戏世界名		
	char szConfigFile[MAX_PATH] = { 0 };
	sprintf(szConfigFile, "%s/%s", szWorkDir, GAME_CONFIG_FILE );
	
	char szTempAreaInfor[256];
	ZeroMemory(szTempAreaInfor, sizeof(szTempAreaInfor));
	GetPrivateProfileString("GameInfo", "AreaInfo", "未知区", szTempAreaInfor, sizeof(szTempAreaInfor), szConfigFile);
	char szTempWorldInfor[256];
	ZeroMemory(szTempWorldInfor, sizeof(szTempWorldInfor));
	GetPrivateProfileString("GameInfo", "WorldInfo", "未知服", szTempWorldInfor, sizeof(szTempWorldInfor), szConfigFile);

#ifdef FK_TEST
	// 读取初始化地图
	int nStartMapID = 0;
	POINT tagStartWorldPos;
	nStartMapID = GetPrivateProfileInt("MapInfo", "StartMapID", 1, szConfigFile );
	tagStartWorldPos.x = GetPrivateProfileInt("MapInfo", "StartPosX", 0, szConfigFile );
	tagStartWorldPos.y = GetPrivateProfileInt("MapInfo", "StartPosY", 0, szConfigFile );
	if( g_CommonFunc.CreateNewWorld(nStartMapID, tagStartWorldPos) )
	{
		TraceLn( "加载地图 MapID = %d, X = %d, Y = %d 成功！", nStartMapID, tagStartWorldPos.x, tagStartWorldPos.y );
	}
	else
	{
		TraceLn( "加载地图 MapID = %d, X = %d, Y = %d 失败！", nStartMapID, tagStartWorldPos.x, tagStartWorldPos.y );
	}


#pragma pack(1)
	struct stCreateMainPlayer
	{
		UID			m_ID;
		WORD		m_wClassID;
		WORD		m_wReserved;
		SACTOR_PROPERTY_PRIVATE tag;
	};
#pragma pack()
	stCreateMainPlayer NetMsg;
	memset( &NetMsg, 0, sizeof(stCreateMainPlayer) );
	NetMsg.m_ID = 1;
	NetMsg.m_wClassID = 1;
	NetMsg.m_wReserved = 0;
	strcpy_s( NetMsg.tag.szName, MAX_NAME_LEN - 15, "自由骑士笃志" );
	NetMsg.tag.ptLoc.x = tagStartWorldPos.x;
	NetMsg.tag.ptLoc.y = tagStartWorldPos.y;
	NetMsg.tag.nSex = 1;
	NetMsg.tag.nLevel = 28;
	NetMsg.tag.nOccupation = 1;
	NetMsg.tag.nMoveSpeedLevel = 7;
	g_ClientGlobal.m_ClientWorld.m_PrivateContextDataReceiver.OnReceived( (const char*)(&NetMsg), sizeof(stCreateMainPlayer) );

#endif
	// 老板键
	UINT nBossKey = GetPrivateProfileInt("UserOption","BossKey",0, szConfigFile );
	if(nBossKey)
	{
		BOOL bHotkey = RegisterHotKey(hWnd,0xbbbb,0,VK_F6);
		if( bHotkey == FALSE )
		{
			PrintEventLn("注册老板键失败!");
		}
	}

	MSG msg;
	// 主循环
	while(1) 
	{
		g_CommonFunc.m_ControllerHelper->LockReceiver(TRUE);

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

		if(msg.message == WM_QUIT)
		{
			g_CommonFunc.m_ControllerHelper->LockReceiver(FALSE);
			break;
		}

		// 主逻辑循环
		if(g_GameApp.Loop() == FALSE)
		{
			g_CommonFunc.m_ControllerHelper->LockReceiver(FALSE);
			break;
		}
			
		g_CommonFunc.m_ControllerHelper->LockReceiver(FALSE);
		Sleep(1);
	}

    g_DrawMouseLock.Lock();
	g_bMouseThreadRun = FALSE;
    g_DrawMouseLock.Unlock();

	g_GameApp.Close();
	g_DefaultTrace.Close();
	g_ClientGlobal.Close();

	return msg.wParam;
}
//------------------------------------------------------------------------
// 主入口函数
//------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// 挂接CrashDump
	if(g_CrashDumpHelper.IsValid())
	{
		if( ! g_CrashDumpHelper->AttachCrashDumpSink(&g_CrashReport) )
		{
			TraceLn("挂接自定义DumpCrashReporter失败！");
		}
	}
	else
	{
		TraceLn("挂接 CrashDump 失败！");
	}

	// 不能启动超过二个相同的进程
	if(!CProcessLimitGuard<1>::Instance()->IsOwned())
	{
		MessageBox(0, "对不起，本游戏不支持双开以及多开。", "FKGame提示", 0);
		return 0;
	}

	// 二阶入口
	int iRet = WinMainEx(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	// 卸载CrashDump
	if(g_CrashDumpHelper.IsValid())
	{
		if( ! g_CrashDumpHelper->DetachCrashDumpSink(&g_CrashReport) )
		{
			TraceLn("卸载自定义DumpCrashReporter失败！");
		}
	}
	else
	{
		TraceLn("挂接 CrashDump 失败！");
	}
	
	return iRet;
}
//------------------------------------------------------------------------
// 主消息回调
//------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	MSG msg;
	memset(&msg, 0, sizeof(msg));
	msg.hwnd = hWnd;
	msg.wParam = wParam;
	msg.lParam = lParam;
	msg.message = message;

	if(g_GameApp.PreTranslateMessage(&msg) == TRUE)
	{		
	}

	static BOOL bHide = FALSE;
	static BOOL bHotkey = FALSE;
	switch (message) 
	{
	case WM_POWERBROADCAST:
		{
			PrintEventLn("WM_POWERBROADCAST : wParam = %d",wParam);
			ITimeAxisAux* pAxisAux = g_ClientGlobal.m_TimerAxisHelper.GetTimeAxisAux();
			if(pAxisAux)
			{
				pAxisAux->RecalcCpuFreq();
				PrintEventLn("变频 : CpuFreq =  %08I64u",pAxisAux->GetCpuFreq());
			}
			return BROADCAST_QUERY_DENY;
		}
		break;
	case WM_KEYUP:
		{
			if(wParam == VK_SNAPSHOT)//截屏
			{
				time_t	long_time;
				struct tm* local_time;
				time(&long_time);
				local_time = localtime(&long_time);
			
				char szPath[MAX_PATH];
				sprintf(szPath , "%s\\Screenshots\\screen_%02d%02d%02d%02d%02d.jpg", g_ClientGlobal.m_szWorkDir,
																					 local_time->tm_mon + 1,
																					 local_time->tm_mday,
																					 local_time->tm_hour,
																					 local_time->tm_min,
																					 local_time->tm_sec);
			
				SaveJpg(g_ClientGlobal.m_pScreen, szPath, 90);
				
				char szMsg[256] = {0};
				sprintf(szMsg, "屏幕已保存至%s", szPath);
				g_ClientGlobal.GetChatClient()->DisplayFloatMessage(2, szMsg);;
				break;
			}
			else if(wParam == VK_F2)
			{
				if( g_ClientGlobal.m_pMapView != NULL)
				{
					g_ClientGlobal.m_pMapView->SetTinyMapVisible(TRUE);
				}

				break;				
			}
			else if(wParam == VK_F1)
			{
				g_ProfileManager.Output();
				break;
			}
			
			return DefWindowProc(hWnd,message,wParam,lParam);
		}
		break;	
	case WM_KEYDOWN:
		{
			return DefWindowProc(hWnd,message,wParam,lParam);
		}
		break;
	case WM_SETCURSOR:
		{
			RECT  rc;
			POINT cursor;
			GetCursorPos(&cursor);
			HWND hPt = WindowFromPoint(cursor);
			ScreenToClient(g_ClientGlobal.m_hMainWnd, &cursor);
			GetClientRect(g_ClientGlobal.m_hMainWnd, &rc );
			if(PtInRect(&rc, cursor) && (hPt == g_ClientGlobal.m_hMainWnd) && g_ClientGlobal.m_hMainWnd == (HWND)wParam)
			{
				SetCursor(FALSE);
				return TRUE;
			}
			else
			{
				ShowCursor(TRUE);
				return DefWindowProc(hWnd,message,wParam,lParam);;
			}
		}
		return TRUE;
	case WM_SYSCOMMAND:
		{
			if(wParam == SC_KEYMENU)
			{
				return 0;
			}
			else if(wParam == SC_CLOSE)
			{
				if(g_ClientGlobal.m_pAppNetworkLink != NULL && g_ClientGlobal.m_pAppNetworkLink->GetType() == NETWORKLINKTYPE_RUNNING)
				{
					g_ClientGlobal.m_pWndMgr->OnEvent(WND_SYSTEMMENU, WE_AW_SHOWEXITWND, 0, 0);
				}
				else
				{
					if(MessageBox(hWnd,"确认要结束游戏吗?","FKGame提示", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
					{
						return DefWindowProc(hWnd,message,wParam,lParam);
					}
				}

				return 0;
			}			
		}
		return DefWindowProc(hWnd,message,wParam,lParam);
	case WM_COPYDATA:
		{
			PCOPYDATASTRUCT pData = (PCOPYDATASTRUCT)lParam;
			if(pData && pData->dwData == 0x10000)
			{
			}
		}
		return TRUE;
	case WM_SIZE:
		if( (SIZE_MAXHIDE == wParam) || (SIZE_MINIMIZED == wParam) )  
		{
			g_bActive = FALSE;
		}
		else
		{
			g_bActive = TRUE;
		}
		if(SIZE_MAXIMIZED == wParam)
		{
			g_GameApp.OnCommand(IDM_FULLSCREEN, 0, 0);
			return 0;
		}
		return 0;
	case WM_ACTIVATEAPP:
		{
			g_bActive = wParam;
		}
		return 0;
	case WM_ACTIVATE:
		{
			if((LOWORD(wParam) == WA_ACTIVE) || (LOWORD(wParam) == WA_CLICKACTIVE))
				g_bActive = TRUE;
			else
				g_bActive = FALSE;
		}
		return 0;
	case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			g_GameApp.OnCommand(wmId, 0, 0);

			switch (wmId)
			{
			// todo..
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_HOTKEY:
		{
			ShowWindow(hWnd,(bHide = !bHide) ? SW_HIDE : SW_SHOW);
		}
		break;
	case WM_CLOSE:
		if(bHotkey)
		{
			bHotkey = (UnregisterHotKey(hWnd,100) == FALSE);
		}
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
			break;
	case WM_CREATE:
		{
			// TODO:
		}
		break;
	default:
		{
			return DefWindowProc(hWnd,message,wParam,lParam);
		}
	}
   return 0;
}
//------------------------------------------------------------------------
// 关于窗口
//------------------------------------------------------------------------
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
//------------------------------------------------------------------------