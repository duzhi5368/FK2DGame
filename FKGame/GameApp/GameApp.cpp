/**
*	created:		2012-6-24   1:05
*	filename: 		GameApp
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		游戏程序主循环，实现消息派发、时间轴管理、绘图循环等
*/
//------------------------------------------------------------------------
#include "PreInclude.h"
#include "CommonFunc.h"
#include "../PerformanceCheck/Performance.h"
#include "../Common/Macro.h"
#include "../Adapter/Adapter.h"
#include "../MapView/MapItemView.h"
#include "../Common/CreatureViewDef.h"
//------------------------------------------------------------------------
BOOL							g_bMouseThreadRun = TRUE;
CCriticalSectionLock			g_DrawMouseLock;
CTimerAxisTunner				g_Tunner;
//------------------------------------------------------------------------
void CTimerAxisTunner::Start(void)
{
	m_dwStart = ::GetTickCount();
	g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->SetTimer(0, static_cast<ITimerSink*>(this), 3000);
}
//------------------------------------------------------------------------
void CTimerAxisTunner::OnTimer(DWORD dwEventID)
{
	DWORD dwNow = ::GetTickCount();
	PrintEventLn("dwNow - m_dwStart = %d",dwNow - m_dwStart);
	if(dwNow > m_dwStart && dwNow  - m_dwStart >= 3300)
	{
		ITimeAxisAux* pAxisAux = g_ClientGlobal.m_TimerAxisHelper.GetTimeAxisAux();
		if(pAxisAux)
		{
			pAxisAux->RecalcCpuFreq();
		}
	}
	g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));
}
//------------------------------------------------------------------------
CCursorDraw::CCursorDraw(void)
{
	m_pCursorBackSave = NULL;
	m_pScreenSaveCanvas = NULL;
	m_bReady = FALSE;
	memset(&m_rcCursorBackSave, 0, sizeof(RECT));
	memset(&m_ptOldCursor, 0, sizeof(POINT));
	m_bUpdateScreenFlag = TRUE;
}
//------------------------------------------------------------------------
CCursorDraw::~CCursorDraw(void)
{
	Release();
}
//------------------------------------------------------------------------
void CCursorDraw::ReleaseCursorBuffer(void)
{
	SAFE_RELEASE(m_pCursorBackSave);
}
//------------------------------------------------------------------------
void CCursorDraw::CreateCursorBuffer(ITrace *pTrace)
{
	g_ClientGlobal.m_CanvasHelper.CreateIBitmap(&m_pCursorBackSave);
	m_pCursorBackSave->Create(100,100);
}
//------------------------------------------------------------------------
void CCursorDraw::Create(ITrace *pTrace)
{
	CreateCursorBuffer(pTrace);
}
//------------------------------------------------------------------------
void CCursorDraw::Release(void)
{
	ReleaseCursorBuffer();
	this->CCursorDraw::CCursorDraw();
}
//------------------------------------------------------------------------
void CCursorDraw::SaveScreen(ICanvas *pScreen)
{
	if(!g_bMouseThreadRun)
		return;

	m_pScreenSaveCanvas = pScreen;
	
}
//------------------------------------------------------------------------
void CCursorDraw::DrawCursorThread()
{
	WINDOWPLACEMENT wp;
	GetWindowPlacement(g_ClientGlobal.m_hMainWnd,&wp);
	if (wp.showCmd == SW_SHOWMINIMIZED)
		return;

	if(!m_bUpdateScreenFlag || !m_bReady || !g_bMouseThreadRun || !g_bActive)
		return;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_ClientGlobal.m_hMainWnd,&pt);
	pt.x = MAX(0, pt.x);
	pt.y = MAX(0, pt.y);
	pt.x = MIN(VIEW_WIDTH, pt.x);
	pt.y = MIN(VIEW_HEIGHT, pt.y);
	if(m_ptOldCursor.x == pt.x && m_ptOldCursor.y == pt.y)
		return;

	m_ptOldCursor = pt;

	XCursor *pCursor = XDesktop::GetInstance()->GetCurrentCursor();
	ASSERT(pCursor);

	RECT rcTemp;
	RECT rcUpdate;

	pCursor->MoveTo(pt);
	pCursor->GetWindowRect(&rcTemp);
	
	int iWidth = rcTemp.right - rcTemp.left;
	int iHeight = rcTemp.bottom - rcTemp.top;

	IBitmap* pAttachBmp = pCursor->GetAttach();
	if (pAttachBmp)
	{
		rcTemp.left -= pAttachBmp->GetWidth()/2 + 1;
		rcTemp.top -= pAttachBmp->GetHeight()/2 + 1;
		rcTemp.right = rcTemp.left + pAttachBmp->GetWidth() + 2;
		rcTemp.bottom = rcTemp.top + pAttachBmp->GetHeight() + 2;
		OffsetRect(&rcTemp,-pCursor->GetHots().x,-pCursor->GetHots().y);
	}
	rcTemp.left = MAX(0,rcTemp.left);
	rcTemp.top = MAX(0, rcTemp.top);
	rcTemp.right = MIN(VIEW_WIDTH, rcTemp.right);
	rcTemp.bottom = MIN(VIEW_HEIGHT, rcTemp.bottom);

	m_pScreenSaveCanvas->GetBitmap()->Draw(m_rcCursorBackSave.left,m_rcCursorBackSave.top,
		m_rcCursorBackSave.right - m_rcCursorBackSave.left,m_rcCursorBackSave.bottom - m_rcCursorBackSave.top,
		m_pCursorBackSave,
		0, 0);
	
	rcUpdate.left = MIN(m_rcCursorBackSave.left, rcTemp.left);
	rcUpdate.top = MIN(m_rcCursorBackSave.top, rcTemp.top);
	rcUpdate.right = MAX(m_rcCursorBackSave.right, rcTemp.right);
	rcUpdate.bottom = MAX(m_rcCursorBackSave.bottom, rcTemp.bottom);	

	m_rcCursorBackSave = rcTemp;
	m_pCursorBackSave->Draw(0,0,
			rcTemp.right - rcTemp.left,rcTemp.bottom - rcTemp.top,
			m_pScreenSaveCanvas->GetBitmap()
			,rcTemp.left,rcTemp.top);

	pCursor->OnDraw(m_pScreenSaveCanvas);

	HDC hdc = GetDC(g_ClientGlobal.m_hMainWnd);
	m_pScreenSaveCanvas->Update2DC(hdc,
		rcUpdate.left,rcUpdate.top,
		rcUpdate.left,rcUpdate.top,
		rcUpdate.right - rcUpdate.left, rcUpdate.bottom - rcUpdate.top);
	
	ReleaseDC(g_ClientGlobal.m_hMainWnd,hdc);
	
	return;
}
//------------------------------------------------------------------------
void CCursorDraw::DrawCursorLoop(ICanvas *pCanvas)
{
	XCursor *pCursor = XDesktop::GetInstance()->GetCurrentCursor();
	if(pCursor)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_ClientGlobal.m_hMainWnd,&pt);
		pt.x = MAX(0, pt.x);
		pt.y = MAX(0, pt.y);
		pt.x = MIN(VIEW_WIDTH, pt.x);
		pt.y = MIN(VIEW_HEIGHT, pt.y);
		pCursor->MoveTo(pt);

		if(g_bMouseThreadRun)
		{
			pCursor->GetWindowRect(&m_rcCursorBackSave);

			int iWidth = m_rcCursorBackSave.right - m_rcCursorBackSave.left;
			int iHeight = m_rcCursorBackSave.bottom - m_rcCursorBackSave.top;

			IBitmap* pAttachBmp = pCursor->GetAttach();
			if (pAttachBmp)
			{
				m_rcCursorBackSave.left -= pAttachBmp->GetWidth()/2 + 1;
				m_rcCursorBackSave.top -= pAttachBmp->GetHeight()/2 + 1;
				m_rcCursorBackSave.right = m_rcCursorBackSave.left + pAttachBmp->GetWidth() + 2;
				m_rcCursorBackSave.bottom = m_rcCursorBackSave.top + pAttachBmp->GetHeight() + 2;
				OffsetRect(&m_rcCursorBackSave,-pCursor->GetHots().x,-pCursor->GetHots().y);
				
			}
			m_rcCursorBackSave.left = MAX(0,m_rcCursorBackSave.left);
			m_rcCursorBackSave.top = MAX(0, m_rcCursorBackSave.top);
			m_rcCursorBackSave.right = MIN(VIEW_WIDTH, m_rcCursorBackSave.right);
			m_rcCursorBackSave.bottom = MIN(VIEW_HEIGHT, m_rcCursorBackSave.bottom);			
			
			m_pCursorBackSave->Draw(0,0,
					m_rcCursorBackSave.right - m_rcCursorBackSave.left,m_rcCursorBackSave.bottom - m_rcCursorBackSave.top,
					pCanvas
					,m_rcCursorBackSave.left,m_rcCursorBackSave.top);
		}

		pCursor->OnDraw(pCanvas);
	}
}
//------------------------------------------------------------------------
void CCursorDraw::UpdateToScreen(ICanvas *pCanvas)
{
	if(!m_bUpdateScreenFlag)
	{
		return;
	}

	HDC hdc = GetDC(g_ClientGlobal.m_hMainWnd);
	pCanvas->Update2DC(hdc,0,0,0,0,VIEW_WIDTH,VIEW_HEIGHT);
	ReleaseDC(g_ClientGlobal.m_hMainWnd,hdc);
}
//------------------------------------------------------------------------
void CCursorDraw::SetUpdateScreen(BOOL bFlag)
{
	m_bUpdateScreenFlag = bFlag;
}
//------------------------------------------------------------------------
// 鼠标处理实现线程部分
static DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	CGameApp *pApp = (CGameApp *)lpParameter;
    while(g_bMouseThreadRun)
    {
        g_DrawMouseLock.Lock();

		try
		{
			pApp->m_cd.DrawCursorThread();
		}
		catch (...)
		{
		}

        g_DrawMouseLock.Unlock();

		Sleep(5);
    }
	return 0;
}
//------------------------------------------------------------------------
CGameApp::CGameApp(void)
{
	m_bStand = FALSE;
	g_dwDxVer = 0;
		
	m_nMoveDelta = MOVE_DELTA;
	m_pScreen = NULL;
	m_pScreenBack = NULL;
	m_pOccupantBitmap = NULL;

	memset(&m_rs, 0, sizeof(m_rs));
	m_rs.m_dwTimes = 1;
	m_rs.m_dwTick = TICK_NEED;


	m_hInstance = NULL;

	memset( m_szVersion,0,sizeof( m_szVersion ));
	memset( m_szServerInfo,0,sizeof(m_szServerInfo));
	memset( m_szCardInfo,0,sizeof(m_szCardInfo));

	m_bQuit = FALSE;
	m_dwPing = 0;
}
//------------------------------------------------------------------------
CGameApp::~CGameApp(void)
{
	Close();
}
//------------------------------------------------------------------------
void CGameApp::ReleaseScreenBuffer(void)
{
	if(m_pScreen)
	{
		if(m_pScreen->GetBitmap())
			m_pScreen->GetBitmap()->Release();
		m_pScreen->Release();
		m_pScreen = NULL;
	}
	if(m_pScreenBack)
	{
		if(m_pScreenBack->GetBitmap())
			m_pScreenBack->GetBitmap()->Release();
		m_pScreenBack->Release();
		m_pScreenBack = NULL;
	}
}
//------------------------------------------------------------------------
void CGameApp::CreateScreenBuffer(ITrace *pTrace)
{
	g_ClientGlobal.m_CanvasHelper.CreateICanvas(&m_pScreen,pTrace);
	IBitmap* pBitmap = NULL;
	g_ClientGlobal.m_CanvasHelper.CreateIBitmap(&pBitmap);
	pBitmap->Create(VIEW_WIDTH,VIEW_HEIGHT);
	m_pScreen->SelectBitmap(pBitmap);

	g_ClientGlobal.m_CanvasHelper.CreateICanvas(&m_pScreenBack,pTrace);
	pBitmap = NULL;
	g_ClientGlobal.m_CanvasHelper.CreateIBitmap(&pBitmap);
	pBitmap->Create(VIEW_WIDTH,VIEW_HEIGHT);
	m_pScreenBack->SelectBitmap(pBitmap);
}
//------------------------------------------------------------------------
BOOL CGameApp::InitGame(HINSTANCE hInstance, HWND hWnd, ICanvas *pScreen, ITrace *pTrace, void *pParam1, void *pParam2)
{
	PrintEventLn("进行游戏初始化。");
	
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);

	m_hInstance = hInstance;
	try
	{
		if(pScreen == NULL)
		{
			// 先创建一个pScreen用于测试，pScreen应该来自于窗口系统
			CreateScreenBuffer(pTrace);
			pScreen = m_pScreen;
		}

		m_cd.Create(pTrace);
		PrintEventLn("创建 Trace 对象。");

		// 创建全局对象
		char szWorkDir[MAX_PATH] = "";
		GetCurrentDirectory(MAX_PATH,szWorkDir);		// 以程序启动时的目录为当前目录
		
		ITTFont * pFontDef = XFontCreate::GetInstance()->CreateFont( "宋体", 12,FALSE,FALSE,FALSE,FALSE,1024);
		ITTFont * pFontOutLineDef = XFontCreate::GetInstance()->CreateFont( "宋体", 12,FALSE,FALSE,FALSE,TRUE,1024);

		if(!g_ClientGlobal.Create(hInstance, szWorkDir, hWnd, pScreen, pFontDef, pFontOutLineDef, pTrace))
		{
			throw "g_ClientGlobal.Create() failed";
		}	

		TraceSysInfo();		

		g_ClientGlobal.m_pGameApp = static_cast<IGameApp * >(this);
		g_CommonFunc.Create();
		
		char szFlagFile[MAX_PATH] = "";
		wsprintf(szFlagFile, OCCUPANT_BITMAP_FILE, g_ClientGlobal.m_szWorkDir);
		m_pOccupantBitmap = g_CommonFunc.LoadBitmap(szFlagFile);

		// 创建输入接口		
		if(!g_CommonFunc.m_ControllerHelper.Create(&g_ClientGlobal))
		{
			throw "Input Event Create Failed!";		
		}
	
		// 加载客户端应用模块
		PrintEventLn("开始创建 GameModule 模块外接器。");
		if(g_GameModuleHelper.Create(static_cast<IClientGlobal *>(&g_ClientGlobal)) == FALSE)
		{
			MessageBox(0, "Create game module Failed!", "", 0);
			return FALSE;
		}
		PrintEventLn("创建 GameModule 模块外接器完成。");
	
		// 创建游戏世界
		PrintEventLn("开始创建 ClientWorld 游戏世界。");
		if(!g_ClientGlobal.m_ClientWorld.Init())
		{
			MessageBox(0, "Create game world Failed!", "", 0);			
			return FALSE;
		}
		PrintEventLn("创建 ClientWorld 游戏世界完成。");

		// 读入版本信息		
		ReadVersion();
	}
	catch(LPCSTR szMsg)
	{
		PrintEventLn("Error : CGameApp::InitGame() : %s",szMsg);
		MsgBox(szMsg);
		Close();
		return FALSE;
	}

	PrintEventLn("--------------- 初始完毕 ---------------");
	return TRUE;
}
//------------------------------------------------------------------------
void CGameApp::Close(void)
{
	m_bQuit = TRUE;
	g_CommonFunc.Close();
	m_cd.Release();
	ReleaseScreenBuffer();
	if(m_pOccupantBitmap)
	{
		m_pOccupantBitmap->Release();
		m_pOccupantBitmap = NULL;
	}
}
//------------------------------------------------------------------------
void CGameApp::SetUpdateScreen(BOOL bFlag)
{
	m_cd.SetUpdateScreen(bFlag);
}
//------------------------------------------------------------------------
// 消息过滤，接管鼠标、键盘等控制消息
// 返回：返回TRUE表示此消息已经被接管，主窗口不必再处理此消息
BOOL CGameApp::PreTranslateMessage(MSG * pMsg)
{
	if(g_ClientGlobal.m_pWndMgr && pMsg->message != WM_PAINT && pMsg->message != WM_DISPLAYCHANGE)
	{
		if(g_ClientGlobal.m_pWndMgr->OnMessage(pMsg->message, pMsg->wParam, pMsg->lParam) == 0)
		{
			return 0;
		}
	}

	switch(pMsg->message)
	{
	case WM_LBUTTONUP:
		{
			POINT ptTile;
			if(!GetCursorPointTile(ptTile))
			{
				return FALSE;
			}

			if(0 == g_CommonFunc.m_ControllerHelper.OnLButtonUp(pMsg->wParam, pMsg->lParam))
			{
				return TRUE;
			}

			return TRUE;
		}
		break;
	case WM_LBUTTONDOWN:
		{
			POINT ptTile;
			if(GetCursorPointTile(ptTile) == FALSE)
			{
				return FALSE;
			}

			if(0 == g_CommonFunc.m_ControllerHelper.OnLButtonDown(pMsg->wParam, pMsg->lParam))
			{
				return TRUE;
			}
		}
		break;
	case WM_RBUTTONUP:
		{
			POINT ptTile;
			if(!GetCursorPointTile(ptTile))
			{
				return FALSE;
			}

			if(0 == g_CommonFunc.m_ControllerHelper.OnRButtonUp(pMsg->wParam, pMsg->lParam))
			{
				return TRUE;
			}

			return TRUE;
		}
		break;
	case WM_RBUTTONDOWN:
		{
			POINT ptTile;
			if(!GetCursorPointTile(ptTile))
			{
				return FALSE;
			}

			if(0 == g_CommonFunc.m_ControllerHelper.OnRButtonDown(pMsg->wParam, pMsg->lParam))
			{
				return TRUE;
			}

			return TRUE;
		}
		break;		
	case WM_SYSKEYUP:
		{
			if(pMsg->wParam == VK_RETURN)
			{
				OnCommand(IDM_FULLSCREEN, 0, 0);
			}
		}
		break;
	case WM_KEYUP:
		if(0 == g_CommonFunc.m_ControllerHelper.OnKeyUp(pMsg->wParam, pMsg->lParam))
		{
			return TRUE;
		}
		break;
	case WM_KEYDOWN:
		if(0 == g_CommonFunc.m_ControllerHelper.OnKeyDown(pMsg->wParam,pMsg->lParam))
		{
			return TRUE;
		}

		{
			struct KEY_MAP
			{
				int key;
				DWORD Command;
			};
			
			static KEY_MAP KeyMap[] = 
			{
				// todo...
				{VK_HOME,	IDM_CENTER},
				{VK_F3,	IDM_FULLSCREEN}
			};
		
			int nCount = sizeof(KeyMap) / sizeof(KEY_MAP);
			for(int key = 0; key < nCount; key++)
			{
				if(pMsg->wParam == KeyMap[key].key)
				{
					if(KeyMap[key].key == VK_F3)
					{
						OnCommand(KeyMap[key].Command, 0, 0);
					}
					else
					{
						OnCommand(KeyMap[key].Command, 0, 0);
					}
				}
			}
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			POINT ptTile;
			if(!GetCursorPointTile(ptTile))
			{
				return FALSE;
			}

			if(0 == g_CommonFunc.m_ControllerHelper.OnMouseMove(pMsg->wParam, pMsg->lParam))
			{
				return TRUE;
			}
			return 0;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
LRESULT CGameApp::OnCommand(DWORD dwCmd, DWORD dwParam1, DWORD dwParam2)
{
	if(0 == g_CommonFunc.m_ControllerHelper.OnCommand(dwCmd, dwParam1, dwParam2))
	{
		if(IController::enPlayerLogin == dwCmd)
		{
			NewbieHelp();
		}
		return 0;
	}

	switch(dwCmd)
	{
	case IDM_CENTER: // 玩家居中显示切换
		break;
	case IDM_FLAG:
		{
			if(g_ClientGlobal.m_pMapView)
			{
				DWORD dwFlag = g_ClientGlobal.m_pMapView->GetDrawFlag(IMapView::maskDrawGrid);
				if(dwFlag)
				{
				  g_ClientGlobal.m_pMapView->SetDrawFlag(0, IMapView::maskDrawGrid);
				}
				else
				{
				  g_ClientGlobal.m_pMapView->SetDrawFlag(IMapView::maskDrawGrid, 0);
				}

				dwFlag = g_ClientGlobal.m_pMapView->GetDrawFlag(IMapView::maskDrawOccupant);
				if(dwFlag)
				{
				  g_ClientGlobal.m_pMapView->SetDrawFlag(0, IMapView::maskDrawOccupant);
				}
				else
				{
				  g_ClientGlobal.m_pMapView->SetDrawFlag(IMapView::maskDrawOccupant, 0, m_pOccupantBitmap);
				}
			}
		}
		break;
	case IDM_PING:
		{
			m_dwPing = dwParam1;
		}
		break;
	case IDM_GET_SERVER_INFO:
		{
			if(dwParam1)
			{
				strcpy((TCHAR*)dwParam1,m_szServerInfo);
			}
		}
		break;
	case IDM_FULLSCREEN:
		{
			if(dwParam1 == 1)
			{
				if(dwParam2)
				{
					*(long*)dwParam2 = CAdapter::GetInstance()->IsFullscreen();
				}
				break;
			}
			else
			{
				if(dwParam1 == 2)
				{
					if(CAdapter::GetInstance()->IsFullscreen() == dwParam2)
					{
						break;
					}
				}

				CAdapter::GetInstance()->ToggleMode(g_ClientGlobal.m_hMainWnd, 1024, 768, 16, 75);

				BOOL bFullScreen = CAdapter::GetInstance()->IsFullscreen();

				// 显示显示设置面板
				if(g_ClientGlobal.m_pWndMgr != NULL)
				{
					g_ClientGlobal.m_pWndMgr->OnEvent(WND_GRAPHICSSETTING, 
										 WE_AW_GRAPHICSSETTING_FULLSCREENMODE,
										 (DWORD)bFullScreen,
										 0);
				}
			}
		}
		break;
	case gcPlayerMovie:
		{
			g_bActive = !dwParam1;
			break;
		}
	default:
		break;
	}
	return -1;
}
//------------------------------------------------------------------------
float CGameApp::GetFPS(void)
{
	static DWORD dwOrigin = g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->GetTimeTick();
	static DWORD dwNow = dwOrigin;
	static DWORD dwFrames = 0;

	dwNow = g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->GetTimeTick();
	dwFrames++;

	static float fps = 0.0f;
	if((dwNow - dwOrigin) >= 1000)
	{
		fps = dwFrames * 1000.0f / (dwNow - dwOrigin);
		dwOrigin = dwNow;
		dwFrames = 0;
	}

	return fps;
}
//------------------------------------------------------------------------
BOOL CGameApp::Loop(void)
{
	static DWORD dwOld = g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->GetTimeTick();
	static DWORD dwOrigin = dwOld;																	// 渲染前帧时间
	static DWORD dwError = 0;																		// 误差时间
	static DWORD dwTime = dwOld;
	dwTime = g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->GetTimeTick();

	//当无焦点降帧

	{
		static BOOL bActive = g_bActive;
		static DWORD dwTickOrigin = m_rs.m_dwTick;
		if(bActive != g_bActive)
		{
			bActive = g_bActive;

			if(bActive)
			{
				TICK_NEED = dwTickOrigin;
				m_rs.m_dwTick = TICK_NEED;
			}
			else
			{
				dwTickOrigin = m_rs.m_dwTick;
				TICK_NEED = TICK_INACTIVE;
				m_rs.m_dwTick = TICK_NEED;
			}
		}	
	}

	static DWORD dwTickThis = m_rs.m_dwTick;


	if(dwError >= m_rs.m_dwTick)
	{
		// 将FPS写入
		// SimpleTraceLn( "FPS: %d", dwError );
		// FPS过慢无法接受时的处理。扣除累积误差（比如长时间载入场景）
		while(dwError >= m_rs.m_dwTick * 2)
		{
			dwError -= m_rs.m_dwTick;
			m_rs.m_fFps = GetFPS();
			m_rs.m_dwRenderTimes++;
		}

		dwError -= m_rs.m_dwTick;
		
		static DWORD dwTimeTotal = 0;
		static DWORD dwNum = 0;
		
		DWORD dw1 = g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->GetTimeTick();
		m_rs.m_dwError = dwError;
		m_rs.m_dwRenderTime = dwTickThis;
		m_rs.m_dwRenderTimes++;
		m_rs.m_dwWholeFps = 1000 * m_rs.m_dwRenderTimes / (dwTime - dwOld);
		m_rs.m_fFps = GetFPS();
		m_rs.m_dwTime = (dwTime - dwOld) / 1000;

		for(DWORD i = 0;i < m_rs.m_dwTimes;i++)
		{	
			STARTPROFILE("Timer::OnCheck")
			OnCheck();
			STOPPROFILE

			// 滚动屏幕保持玩家居中显示
			STARTPROFILE("ScrolltoCenter")
			ScrolltoCenter();
			STOPPROFILE

			WINDOWPLACEMENT wp;
			GetWindowPlacement(g_ClientGlobal.m_hMainWnd,&wp);
			if (wp.showCmd != SW_SHOWMINIMIZED)
			{		
				STARTPROFILE("Update")
				Update();
				STOPPROFILE	

				g_DrawMouseLock.Lock();

				m_cd.SaveScreen(m_pScreen);
				m_cd.DrawCursorLoop(m_pScreen);

				STARTPROFILE("UpdateToScreen")
				m_cd.UpdateToScreen(m_pScreen);
				STOPPROFILE

				g_ClientGlobal.m_pScreen = m_pScreen;
				m_pScreen = m_pScreenBack;
				m_pScreenBack = g_ClientGlobal.m_pScreen;

				XDesktop* pDesktop = XDesktop::GetInstance();
				pDesktop->SetCanvas(m_pScreen);

				g_DrawMouseLock.Unlock();
			}
		}

		DWORD dw2 = g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->GetTimeTick();
		
		dwTimeTotal += (dw2 - dw1);
		dwNum++;
		
		if(dwNum >= TICK_TIMES)
		{
			dwTickThis = dwTimeTotal / dwNum;

			if(dwTickThis < TICK_BOTTOM)
			{
				if(g_bActive)
				{
					//对相当好配置的机器(fps超过底限，则强令进行修正)
					TICK_NEED = TICK_BOTTOM;
					m_rs.m_dwTick = TICK_NEED;
				}
			}
			else
			{
				//减少绘图间隔
				if(dwTickThis + TICK_VAR + TICK_VAR / 2<= m_rs.m_dwTick)
				{
					TICK_NEED -= TICK_VAR;
					if(TICK_NEED < TICK_BOTTOM)
						TICK_NEED = TICK_BOTTOM;
					m_rs.m_dwTick = TICK_NEED;
				}
				//增加绘图间隔
				else if(dwTickThis > m_rs.m_dwTick - TICK_VAR / 2)
				{
					TICK_NEED += TICK_VAR;
					m_rs.m_dwTick = TICK_NEED;
				}
			}

			dwNum = 0;
			dwTimeTotal = 0;
		}
	}

	dwError += (dwTime - dwOrigin);
	{
		dwOrigin = dwTime;
	}
	return TRUE;
}
//------------------------------------------------------------------------
void CGameApp::CursorLoop(void)
{
	static BOOL bShow = TRUE;
	static HCURSOR hC = LoadCursor(0,IDC_ARROW);
	RECT  rc;
	POINT cursor;
	GetCursorPos( &cursor );
	HWND hPt = WindowFromPoint(cursor);

	ScreenToClient( g_ClientGlobal.m_hMainWnd , &cursor );
	GetClientRect( g_ClientGlobal.m_hMainWnd , &rc );
	if( PtInRect( &rc , cursor ) && (hPt == g_ClientGlobal.m_hMainWnd))
	{
		if(bShow)
		{
			SetCursor(0);
			bShow = TRUE;
		}
	}
}
//------------------------------------------------------------------------
// 计算一个函数的执行时间
void CGameApp::CalcTime(CalcFunc fn, BOOL bDWORD, DWORD* pdwDword, float* pfFloat, DWORD dwTimes, DWORD dwCallTimes)
{
#ifdef _DEBUG
	static DWORD dwTimeTotal = 0;
	static DWORD dwNum = 0;
	
	DWORD dw1 = g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->GetTimeTick();
	
	for(DWORD i = 0;i < dwCallTimes;i++)
		(this->*fn)();
	
	DWORD dw2 = g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->GetTimeTick();
	
	dwTimeTotal += (dw2 - dw1);
	dwNum++;
	
	if(dwNum == dwTimes)
	{
		if(bDWORD)
			*pdwDword = dwTimeTotal / dwTimes;
		else
			*pfFloat = (float)dwTimeTotal / dwTimes;

		dwNum = 0;
		dwTimeTotal = 0;
	}
#else
	(this->*fn)();
#endif
}
//------------------------------------------------------------------------
void CGameApp::OnCheck(void)
{
	if(g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis)
	{
		g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->OnCheck();
	}
}
//------------------------------------------------------------------------
char * CGameApp::RollMsgPing(void)
{
	static char szPing[256];
	if(m_dwPing == 0)
	{
		return 0;
	}

	sprintf(szPing,"Ping: %d",m_dwPing);
	return szPing;
}
//------------------------------------------------------------------------
char* CGameApp::RollMsgVer(void)
{
	return m_szVersion;
}
//------------------------------------------------------------------------
char* CGameApp::RollMsgServerInfo(void)
{
	return m_szServerInfo;
}
//------------------------------------------------------------------------
char* CGameApp::RollMsgTime(void)
{
	static char szTime[256];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szTime,"%04d.%02d.%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	return szTime;
}
//------------------------------------------------------------------------
char* CGameApp::RollMsgCard(void)
{
	return m_szCardInfo;
}
//------------------------------------------------------------------------
void CGameApp::RollMsg(void)
{
typedef char* (CGameApp::*RollMsgFunc)();

	static DWORD dwOld = g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->GetTimeTick();
	DWORD dwNow = g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->GetTimeTick();
	struct RollMsgStruct
	{
		RollMsgFunc	pFunc;
		DWORD	dwMillisec;
	};
	static RollMsgStruct pFunc[] = 
	{
		{&CGameApp::RollMsgVer			,1000},
		{&CGameApp::RollMsgServerInfo	,1000},
		{&CGameApp::RollMsgTime			,1000},
		{&CGameApp::RollMsgPing			,1000},
		{&CGameApp::RollMsgCard			,1000}
	};
	static	int iState = 0;

	char *pMsg = (this->*pFunc[iState].pFunc)();
	if(!pMsg || strlen(pMsg) == 0)
		pMsg = RollMsgServerInfo();
	if(!pMsg || strlen(pMsg) == 0)
		pMsg = RollMsgVer();

	if(pMsg)
	{
		if(iState == 4 && m_szCardInfo[0])
		{
			m_pScreen->GetBitmap()->DrawText(g_ClientGlobal.m_pDefFontWithOutline,0,0,pMsg,m_colorCardInfo,FONT_NOALPHA);
		}
		else
		{
			m_pScreen->GetBitmap()->DrawText(g_ClientGlobal.m_pDefFontWithOutline,0,0,pMsg,WPixel(54,186,34),FONT_NOALPHA);
		}
	}
	if(dwNow - dwOld > pFunc[iState].dwMillisec)
	{
		iState = (iState + 1) % (sizeof(pFunc) / sizeof(pFunc[0]));
		dwOld = dwNow;
	}
}
//------------------------------------------------------------------------
void CGameApp::Update(BOOL bAddWhole)
{
	if(bAddWhole)
	{
		m_pScreen->AddWholeScreen();
	}

	// 地图管理绘制器
	if(g_ClientGlobal.m_pMapView != NULL)
	{
		g_ClientGlobal.m_pMapView->OnDraw(m_pScreen, g_ClientGlobal.m_pDefFont);		
	}

	// 绘制界面
	if(g_ClientGlobal.m_pWndMgr)
	{
		g_ClientGlobal.m_pWndMgr->OnMessage(WM_PAINT, 0, 0);
	}

	// 绘制提示信息
	IChatClient * pChatClient = g_ChatClientHelper.GetChatClient();
	if(pChatClient != NULL)
	{
		pChatClient->OnSysMsgDraw(m_pScreen);
	}
	{
		RollMsg();
	}

	// 绘制绘图信息
	if(g_ClientGlobal.m_pMapView != NULL && g_ClientGlobal.m_pMapView->GetDrawFlag(IMapView::maskDrawOccupant) == IMapView::maskDrawOccupant)
	{	
		RECT rcBox = { 5, 550, 215, 765 };
		m_pScreen->DrawBox(rcBox, WPixel(0, 0, 255), BLIT_TRANS, 55);

		char str[256];
		sprintf(str,"   [ FKEngine RenderInfo ]");
		m_pScreen->Draw(5, 560, str, WPixel(0, 255, 0));
		sprintf(str,"------------------------------");
		m_pScreen->Draw(5, 575, str, WPixel(0, 255, 0));
		sprintf(str,"dwTick = %d",m_rs.m_dwTick);
		m_pScreen->Draw(8, 600, str, WPixel(0, 255, 0));
		sprintf(str,"dwTimes = %d", m_rs.m_dwTimes);
		m_pScreen->Draw(8, 615, str, WPixel(0, 255, 0));
		sprintf(str,"dwError = %d", m_rs.m_dwError);
		m_pScreen->Draw(8, 630, str, WPixel(0, 255, 0));
		sprintf(str,"dwRenderTime = %d", m_rs.m_dwRenderTime);
		m_pScreen->Draw(8, 645, str, WPixel(0, 255, 0));
		sprintf(str,"dwRenderTimes = %d", m_rs.m_dwRenderTimes);
		m_pScreen->Draw(8, 660, str, WPixel(0, 255, 0));
		sprintf(str,"fFps = %4.4f", m_rs.m_fFps);
		m_pScreen->Draw(8, 675, str, WPixel(0, 255, 0));
		sprintf(str,"dwWholeFps = %d", m_rs.m_dwWholeFps);
		m_pScreen->Draw(8, 690, str, WPixel(0, 255, 0));
		sprintf(str,"dwTime = %d", m_rs.m_dwTime);
		m_pScreen->Draw(8, 705, str, WPixel(0, 255, 0));
		sprintf(str,"PlayerState = %s", m_rs.m_strState);
		m_pScreen->Draw(8, 720, str, WPixel(0, 255, 0));
		sprintf(str,"dwOnCheckTime = %d", m_rs.m_dwOnCheckTime);
		m_pScreen->Draw(8, 735, str, WPixel(0, 255, 0));
		sprintf(str,"fUpdateToScreenTime = %4.4f", m_rs.m_fUpdateToScreenTime);
		m_pScreen->Draw(8, 750, str, WPixel(0, 255, 0));
	}

	m_cd.m_bReady = TRUE;
}
//------------------------------------------------------------------------
BOOL CGameApp::ScrolltoCenter(void)
{
	if(g_ClientGlobal.m_pPlayer == NULL || g_ClientGlobal.m_pMapView == NULL)
	{
		return FALSE;
	}

	POINT ptTile = {0,0};
	g_ClientGlobal.m_pPlayer->GetMapItem()->OnCommand(CrtCmdGetProperty,CrtPropAbsolutePosition,(DWORD)&ptTile);
	return g_ClientGlobal.m_pMapView->ScrollToCenter(ptTile);
}
//------------------------------------------------------------------------
void CGameApp::OnMouseMove(DWORD wParam, DWORD lParam)
{

}
//------------------------------------------------------------------------
void CGameApp::ReadVersion(void)
{
	IResScript * pResScript = g_ClientGlobal.m_ResMgrHelper.m_pResMgr->LoadResScript( CLIENT_VERSION_FILE );
	if( pResScript == NULL )
		return;

	IResObject * pRoot = pResScript->GetResObjectRoot();
	if( pRoot )
	{
		IResObject * pVerObj = pRoot->GetSubObjectByName( "ClientInfo" );
		if( pVerObj )
			sprintf( m_szVersion , "FKGame Ver:%s" , pVerObj->GetPropertyString( "version" ));
	}

	pResScript->Release();
	pResScript = NULL;
}
//------------------------------------------------------------------------
// 这个函数当玩家上线时候被调用，用来做点事情吧...
void CGameApp::NewbieHelp(void)
{
	g_Tunner.Start();
	return;
}
//------------------------------------------------------------------------