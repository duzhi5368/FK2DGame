/**
*	created:		2012-6-25   1:39
*	filename: 		WndMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/GameGlobal/IClientGlobal.h"
#include "../Include/WndMgr.h"
#include "../../FKGame/WndSys/WndDef.h"
//------------------------------------------------------------------------
const static char WndResFileName[64] =  "Data\\scp\\Window.scp";
//------------------------------------------------------------------------
IClientGlobal * g_pGlobal = NULL;
CFKTrace g_DefaultTrace;
//------------------------------------------------------------------------
CWndMgr::CWndMgr()
{
	m_pResScript = NULL;
	m_pFileSystem = NULL;
	m_pFileSystemEx = NULL;
	ZeroMemory(m_WndTellShow, sizeof(m_WndTellShow));
	__DeclarePrimaryWnd();
}
//------------------------------------------------------------------------
void CWndMgr::CloseAllWnd()
{	

}
//------------------------------------------------------------------------
CWndMgr::~CWndMgr()
{
	CloseAllWnd();
	SAFE_RELEASE(m_pResScript);
	SAFE_RELEASE(m_pFileSystem);
	SAFE_RELEASE(m_pFileSystemEx);
}
//------------------------------------------------------------------------
void CWndMgr::Release()
{
	delete this;
}
//------------------------------------------------------------------------
BOOL CWndMgr::Init(IClientGlobal *pGlobal)
{	
	// 判断是否读取压缩文件
	char szConfigFile[MAX_PATH] = {0};
	sprintf(szConfigFile, "%s%s",g_pGlobal->m_szWorkDir, GAME_CONFIG_FILE);
	UINT bNoPack = GetPrivateProfileInt("GameInfo", "UsePack", 0, szConfigFile);

	if (bNoPack == 0)
	{
		pGlobal->m_FileSystemHelper.Create(&m_pFileSystem, DEBUG_FILESYSTEM_DIR, 0, g_pGlobal->m_TraceHelper.GetTraceObject());		
	}
	else
	{
		pGlobal->m_FileSystemHelper.Create(&m_pFileSystem, RELEASE_FILESYSTEM_FILE, 1, g_pGlobal->m_TraceHelper.GetTraceObject());
	}

	if (m_pFileSystem == NULL)
	{
		FKTraceLn("[WndMgr]: Load FileSystem fail.");
	}
	else
	{
		FKTraceLn("创建 FileSystem 成功。");
	}

	Assert(m_pFileSystem);

	m_ResMgrHelper.Create(g_pGlobal->m_TraceHelper.GetTraceObject());
	if (m_ResMgrHelper.m_pResMgr == NULL)
	{
		FKTraceLn("[WndMgr]: Load ResMgr fail.");
	}
	else
	{
		FKTraceLn("创建 ResSystem 成功。");
	}
	Assert(m_ResMgrHelper.m_pResMgr);

#ifdef _DEBUG
	m_pResScript = m_ResMgrHelper.m_pResMgr->LoadResScript(DEBUG_RES_LIST_FILE);
#else
	BYTE pKey[] = "abcd";
	m_pResScript = m_ResMgrHelper.m_pResMgr->LoadResScript(RELEASE_RES_LIST_FILE, FALSE, pKey);
#endif

	if (m_pResScript == NULL)
	{
		FKTraceLn("[WndMgr]: Load m_pResScript fail.");
	}
	ASSERT(m_pResScript);

	IResObject *pObject = m_pResScript->GetResObjectRoot();

	//设置图片存放路径
	CImageLoader::GetInstance()->SetFileSystem(m_pFileSystem);

	pGlobal->m_FileSystemHelper.Create(&m_pFileSystemEx, g_pGlobal->m_szWorkDir, 0, g_pGlobal->m_TraceHelper.GetTraceObject());

	//创建窗口系统
	XWindow *pWindows = m_WndSysHelper.CreateWndSys(pGlobal->m_hMainWnd,
		pGlobal->m_pScreen, pGlobal->m_TimerAxisHelper.m_pTimeAxis,
		m_pFileSystem, pObject, pGlobal->m_pDefFont);

	if (pWindows == NULL)
	{
		FKTraceLn("[WndMgr]: Create WndSys fail.");
		return FALSE;
	}
	else
	{
		FKTraceLn("创建 WndSystem 成功。");
	}
	
	if (m_WndSysHelper.GetGameDesktop() == NULL)
	{
		FKTraceLn("[WndMgr]: Load Desktop fail.");
		return FALSE;
	}
	else
	{
		FKTraceLn("创建 Desktop 成功。");
	}
 
	//设置自绘鼠标
	m_WndSysHelper.GetGameDesktop()->SetCursorOwnDraw(TRUE);

	return TRUE;
}
//------------------------------------------------------------------------
void CWndMgr::__LoadWndToolTip(IResScript *pRescript)
{
	if (pRescript == NULL)
	{
		return ;
	}
	IResObject *pResObject = pRescript->GetResObjectRoot();
	if (pResObject == NULL)
	{
		return;
	}
	pResObject = pResObject->GetSubObjectByIndex(0);
	if (pResObject == NULL)
	{
		return;
	}

	for (int nSubObjIndex = 0; nSubObjIndex < pResObject->GetSubObjectCount(); ++nSubObjIndex)
	{
		IResObject *pResSubObj = pResObject->GetSubObjectByIndex(nSubObjIndex);
		for (int nPropertyCount = 0; nPropertyCount < pResSubObj->GetPropertyCount(); ++nPropertyCount)
		{
			LPCSTR szToolTipID = pResSubObj->GetPropertyName(nPropertyCount);
			int nToolTipID = atol(szToolTipID);
			LPCSTR szToolTip = pResSubObj->GetPropertyString(szToolTipID);
			CToolTipLoader::GetInstance().AddToolTip(nToolTipID, szToolTip);
		}
	}
}
//------------------------------------------------------------------------
//取得鼠标上附带的数据
ICursorData* CWndMgr::GetCursorAttachData()
{
	XWindow *pWndDesktop = m_WndSysHelper.GetGameDesktop();

	return (ICursorData*)(pWndDesktop->GetCursor()->GetUserData());
}
//------------------------------------------------------------------------
//描  述：检查取得鼠标上的附加的类型
int CWndMgr::GetCursorAttachType() 
{
	//ICursorData *pData = GetCursorAttachData();
	//return pData ? pData->GetDataType() : enCursorDataTypeMax; 
	return 0;
}
//------------------------------------------------------------------------
void CWndMgr::ClearCursorAttach()
{
	//ICursorData *pData = GetCursorAttachData();
	//SetCursorAttach(NULL, NULL);
	//SAFE_RELEASE(pData);
}
//------------------------------------------------------------------------
void CWndMgr::SetCursorAttach(IBitmap *pBitmap, ICursorData *pData)
{	
	XWindow *pWndDesktop = m_WndSysHelper.GetGameDesktop();
	XCursor *pCursor = pWndDesktop->GetCursor();
	if (pCursor == NULL)
	{
		return;
	}
	
	IBitmap *pOldBitmap = pCursor->GetAttach();
	pCursor->SetAttach(pBitmap);
	if (pOldBitmap != NULL)
	{
		CImageLoader::GetInstance()->FreeImage(pOldBitmap);
		pOldBitmap = NULL;
	}


	ICursorData *pOldData = (ICursorData*)(pCursor->GetUserData());
	pCursor->SetUserData((DWORD)pData);
}
//------------------------------------------------------------------------

#define CreateAWindow( T )									\
{															\
    T * pWnd = new T;										\
	if (pWnd->Create(&m_WndSysHelper,						\
					  m_WndSysHelper.GetGameDesktop(),		\
					  m_pResScript->GetResObject(uWndID),	\
					  uWndID								\
					  )										\
					  )										\
				return (void*)pWnd;							\
	return NULL;											\
}															\


//------------------------------------------------------------------------
void* CWndMgr::CreateWnd(UINT uWndID)
{
	XWindow *pWndDesktop = m_WndSysHelper.GetGameDesktop();

	if (pWndDesktop == NULL)
	{
		return NULL;
	}

	//如果窗口已经创建,返回窗口
	XWindow *pWnd = pWndDesktop->FindWindowByID(uWndID);
	if (pWnd != NULL)
	{
		return pWnd;
	}

	switch(uWndID) 
	{
		default: break;
	}

	return NULL;
}
//------------------------------------------------------------------------
BOOL CWndMgr::CloseWnd(UINT uWndID)
{
	XWindow *pWndDesktop = m_WndSysHelper.GetGameDesktop();
	Assert(pWndDesktop);
	XWindow *pWnd = pWndDesktop->FindWindowByID(uWndID);
	
	if (pWnd != NULL)
	{
		pWnd->Release();
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CWndMgr::ShowWndAux(UINT uWndID, int nCmdShow)
{
	XWindow *pWndDesktop = m_WndSysHelper.GetGameDesktop();
	Assert(pWndDesktop);
	XWindow *pWnd = pWndDesktop->FindWindowByID(uWndID);
	if (pWnd == NULL)
	{
		return FALSE;
	}

	if (nCmdShow == SW_SHOW)
	{	
		if (pWnd->IsVisible())
		{//窗口已经显示
			return TRUE;
		}
		if (uWndID < MAX_WINDOW_NUM && m_WndTellShow[uWndID]) 
		{
			// OnEvent(uWndID, WE_CW_ALLWND_SHOW, 1, 0);
		}
		pWnd->SetVisible(TRUE);
		pWnd->SetActive();
		pWnd->SetFocus();
	}
	else if (nCmdShow == SW_HIDE)
	{	
		if (!pWnd->IsVisible())
		{//窗口已经隐藏
			return TRUE;
		}
		if (uWndID < MAX_WINDOW_NUM && m_WndTellShow[uWndID]) 
		{
			// OnEvent(uWndID, WE_CW_ALLWND_SHOW, 0, 0);
		}
		pWnd->SetVisible(FALSE);
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CWndMgr::IsWindowOpened(UINT uWndID)
{
	XWindow *pWndDesktop = m_WndSysHelper.GetGameDesktop();
	Assert(pWndDesktop);
	XWindow *pWnd = pWndDesktop->FindWindowByID(uWndID);

	if (pWnd == NULL)
	{
		return FALSE;
	}

	return pWnd->IsVisible();
}
//------------------------------------------------------------------------
DWORD CWndMgr::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	XWindow *pWndDesktop = m_WndSysHelper.GetGameDesktop();
	
	DWORD dwReturn = m_WndSysHelper.OnMessage(uMsg, wParam, lParam);
	
	switch(uMsg) {
		case WM_CHANGE_GAMECUROSR:
			pWndDesktop->SetCursor(wParam);
			break;
	}
			
	return dwReturn;
}
//------------------------------------------------------------------------
BOOL CWndMgr::OnEvent(UINT uWndID, UINT uEventID, WPARAM wParam, LPARAM lParam)
{
	XWindow *pWndDesktop = m_WndSysHelper.GetGameDesktop();
	if (pWndDesktop == NULL) 
	{
		return FALSE;
	}

	XWindow* pWnd = pWndDesktop->FindWindowByID(uWndID);
	if (pWnd == NULL)
	{
		return FALSE;
	}
	if (pWnd->GetWndTraits() & XWT_GENERAL)
	{
		IWndEnableCall4 *pCall = (IWndEnableCall4*)pWnd;
		return pCall->OnEvent(uEventID, wParam, lParam);
	}
	IWndEnableCall * pCall = static_cast< IWndEnableCall* >(pWnd);//*/
	return pCall->OnEvent(uEventID, wParam, lParam);	
}
//------------------------------------------------------------------------
BOOL CWndMgr::SubscibeMessage(UINT uWndID, UINT uEventID, WMMESSAGE_CONNECTION *pConnect)
{
	ASSERT(uWndID >= 0 && uWndID < MAX_WINDOW_NUM);
	return m_MessageSender[uWndID].Subscibe(uEventID, pConnect);
}
//------------------------------------------------------------------------
BOOL CWndMgr::UnSubscibeMessage(UINT uWndID, UINT uEventID, WMMESSAGE_CONNECTION *pConnect)
{
	ASSERT(uWndID >= 0 && uWndID <  MAX_WINDOW_NUM);
	return m_MessageSender[uWndID].UnSubscibe(uEventID, pConnect);
}
//------------------------------------------------------------------------
void CWndMgr::PostWindowMessage(UINT uWndID, UINT uEventID, WPARAM wParam, LPARAM lParam)
{
	ASSERT(uWndID >= 0 && uWndID < MAX_WINDOW_NUM);

	BOOL bTrue = TRUE;
	m_MessageSender[uWndID].Emit(uEventID, wParam, lParam, bTrue);
}
//------------------------------------------------------------------------
void CWndMgr::RegTellShowWnd(UINT uWndID)
{
	if (uWndID>=0 && uWndID <= MAX_WINDOW_NUM)
	{
		m_WndTellShow[uWndID] = 1;
	}
}
//------------------------------------------------------------------------
void CWndMgr::UnRegTellShowWnd(UINT uWndID)
{
	if (uWndID>=0 && uWndID <= MAX_WINDOW_NUM)
	{
		m_WndTellShow[uWndID] = 0;
	}
}
//------------------------------------------------------------------------
BOOL CWndMgr::CloseAllFunctionWnd()
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CWndMgr::ShowWnd(UINT uWndID, int nCmdShow)
{
	return ShowWndAux(uWndID, nCmdShow);
}
//------------------------------------------------------------------------
// 定制输入框
// 1 = 确定，2 = 取消，3 = 重试， 4 = 退出
void CWndMgr::CustomInputBox(LPCSTR pMsg, 
							 IWndCallback * pCallBack, 
							 DWORD dwMaxLen, 
							 BOOL  bDigitOnly,
							 BOOL  bNullFlag,
							 BOOL  bPasswordFlag,
							 int    nCourse)
{
	XWindow * pWndDesktop = m_WndSysHelper.GetGameDesktop();
	if(pWndDesktop == NULL)
		return;	
	
	XWindow * pWnd = pWndDesktop->FindWindowByID(WND_INPUTWND);
	if(pWnd == NULL)
		return;
	
	struct INPUTBOXCONTEXT
	{
		IWndCallback * pCallBack;
		char           szMsgDesc[1024];
		BOOL           bDigitOnly;
		DWORD          dwMaxLen;
		BOOL           bNullFlag;
		BOOL           bPasswordFlag;
		int            nCourse;            //表示窗口弹出的原因
	};
	
	INPUTBOXCONTEXT stInputBoxContext;
	stInputBoxContext.bDigitOnly = bDigitOnly;
	stInputBoxContext.dwMaxLen = dwMaxLen;
	stInputBoxContext.pCallBack = pCallBack;
	lstrcpyn(stInputBoxContext.szMsgDesc, pMsg, sizeof(stInputBoxContext.szMsgDesc));
	stInputBoxContext.bNullFlag = bNullFlag;
	stInputBoxContext.bPasswordFlag = bPasswordFlag;
	stInputBoxContext.nCourse = nCourse;

	
	IWndEnableCall4 * pInputWnd = (IWndEnableCall4 *)pWnd;	
	pInputWnd->OnEvent(WE_CW_INPUT_SET, (DWORD)&stInputBoxContext, sizeof(INPUTBOXCONTEXT));
}
//------------------------------------------------------------------------
// 定制消息框
// 1 = 确定，2 = 取消，3 = 重试， 4 = 退出
void CWndMgr::CustomMsgBox(LPCSTR pMsg, DWORD BtnNO1, 
						   DWORD BtnNO2, 
						   DWORD BtnNO3,
						   IWndCallback * pCallBack,  
						   RECT * rcWnd,
						   WPARAM wParam,
						   LPARAM lParam,
						   int    nCourse)
{
	/*
	XWindow * pWndDesktop = m_WndSysHelper.GetGameDesktop();
	if(pWndDesktop == NULL)
		return;	
	
	XWindow * pWnd = pWndDesktop->FindWindowByID(WND_CUSTOMMSGBOX);
	if(pWnd == NULL)
		return;	
	
	RECT rcTempPos;
	if(rcWnd != NULL)
		memcpy(&rcTempPos, rcWnd, sizeof(RECT));
	else
		pWnd->GetWindowRect(&rcTempPos);

	struct MSGBOXCONEXT
	{
		char  szMsgDesc[1024];
		DWORD dwBtnNO[3];
		IWndCallback * pCallBack;
		RECT  rcWnd;
		int nCourse;
	};
	
	MSGBOXCONEXT stContext;
	lstrcpyn(stContext.szMsgDesc, pMsg, sizeof(stContext.szMsgDesc));
	stContext.dwBtnNO[0] = BtnNO1;
	stContext.dwBtnNO[1] = BtnNO2;
	stContext.dwBtnNO[2] = BtnNO3;
	stContext.pCallBack = pCallBack;
	memcpy(&stContext.rcWnd, &rcTempPos, sizeof(RECT));
	stContext.nCourse = nCourse;
	
	IWndEnableCall4 * pMsgBoxWnd = (IWndEnableCall4 *)pWnd;
	pMsgBoxWnd->OnEvent(WE_CW_MSGBOX_SETCONTEXT, (DWORD)&stContext, sizeof(MSGBOXCONEXT));	
	pMsgBoxWnd->BringToTopLevel();

	g_pGlobal->GetSoundEngine()->PlaySound(565);
	*/
}
//------------------------------------------------------------------------
void CWndMgr::__DeclarePrimaryWnd(void)
{	

}
//------------------------------------------------------------------------
BOOL CWndMgr::HideOnePrimacyWnd(void)
{
	//从可关闭列表中找一个开着的窗口来关闭, 找到的话,返回TRUE
	for (std::list<int>::iterator itor = m_lsPrimaryWnd.begin();
	itor != m_lsPrimaryWnd.end(); ++itor)
	{
		XWindow *pWnd = (XWindow *)CreateWnd(*itor);
		if (pWnd && pWnd->IsVisible())
		{
			ShowWnd(*itor, SW_HIDE);
			return TRUE;
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) BOOL CreateWndMgrDLL(IClientGlobal *pGlobal, IWndMgr **ppWndMgr, IWndCallback * pCallBack)
{
	if (NULL == pGlobal || *ppWndMgr != NULL)
	{
		return FALSE;
	}

	g_pGlobal = pGlobal;

	FKTraceLn("开始创建 WndMgr 窗口管理器。");

	CWndMgr *pWndMgr = new CWndMgr;
	if (NULL == pWndMgr)
	{
		FKTraceLn("[WndMgr]: New WndMgr fail.");
		return FALSE;
	}

	if (FALSE == pWndMgr->Init(g_pGlobal))
	{
		SAFE_RELEASE(pWndMgr);
		FKTraceLn("[WndMgr]: Init WndMgr fail.");
		return FALSE;
	}

	*ppWndMgr = static_cast<IWndMgr*>(pWndMgr);

	FKTraceLn("创建 WndMgr 完成。");

	return TRUE;
}
//------------------------------------------------------------------------