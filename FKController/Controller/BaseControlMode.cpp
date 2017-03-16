/**
*	created:		2012-6-25   0:54
*	filename: 		BaseControlMode
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include <time.h>
#include "BaseControlMode.h"
#include "../FKControllerInclude.h"
#include "../../FKGame/MapView/IMapView.h"
#include "../PrintScreen/PrintScreen.h"
//------------------------------------------------------------------------
#ifndef INVALID_SKILLID
#define INVALID_SKILLID -1
#endif
//------------------------------------------------------------------------
DWORD __gPT1(void)   //取线程ID的函数
{
	return 0;
}
//------------------------------------------------------------------------
void __gPT3(void)   //改返回值
{
	
}
//------------------------------------------------------------------------
DWORD __idt;    //游戏线程的ID
//------------------------------------------------------------------------
CBaseControlMode::CBaseControlMode()
{
	//连接各种行为引擎
	m_bAutoAttack = FALSE;
}
//------------------------------------------------------------------------
void CBaseControlMode::ConnectSingal(void)
{
}
//------------------------------------------------------------------------
void CBaseControlMode::DisConnectSingal(void)
{
	m_SkillConnect.UnConnect();
	m_GoodsConnect.UnConnect();
	m_CommandQueueConnect.UnConnect();
	m_SkillSelfConnect.UnConnect();
	m_CommandTraceConnect.UnConnect();
	m_CommandSelectTeamate.UnConnect();
	m_CommandAttackTeamate.UnConnect();
	m_CommandPet.UnConnect();
	m_CommandSetLockFire.UnConnect();
	m_CommandCancelTarget.UnConnect();
	m_CommandLongMove.UnConnect();
	m_CommandLoadMapBlock.UnConnect();
}
//------------------------------------------------------------------------
CBaseControlMode::~CBaseControlMode()
{
}
//------------------------------------------------------------------------
LRESULT CBaseControlMode::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{	
	// 取得鼠标当前所在的TILE
	POINT cursor;
	cursor.x = LOWORD(lParam);
	cursor.y = HIWORD(lParam);
	POINT ptTile;
	g_ClientGlobal->m_pMapView->ScreenToTile(cursor,ptTile);
	
	if(!g_ClientGlobal->m_pMapView->IsValidTile(ptTile)) 
		return -1;
	
	return 0;
}
//------------------------------------------------------------------------
LRESULT CBaseControlMode::OnRButtonUp(WPARAM wParam, LPARAM lParam)
{
	return 0;
}
//------------------------------------------------------------------------
BOOL CBaseControlMode::__isTreasureBox(CMapItemView *pItem)
{
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CBaseControlMode::ProcessLUASelectActorCmd(CMapItemView* pItem, POINT ptTileClicked)
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CBaseControlMode::ProcessCursorAttackCmd(CMapItemView* pItem, POINT ptTileClicked)
{
	return FALSE;
}
//------------------------------------------------------------------------
LRESULT CBaseControlMode::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	return 0;
}
//------------------------------------------------------------------------
LRESULT CBaseControlMode::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	return 0;
}
//------------------------------------------------------------------------
LRESULT CBaseControlMode::OnMouseMove(WPARAM wParam, LPARAM lParam)
{	
	return 0;
}
//------------------------------------------------------------------------
void CBaseControlMode::__TranslateKeyCode(int *pKeyCode)
{
	//这个方法以后根据策划需要修改	
	BOOL bPressShift = FALSE;
	if (GetAsyncKeyState(VK_SHIFT) < 0)
	{
		bPressShift = TRUE;
	}

	if (bPressShift && *pKeyCode == '5')
	{
		*pKeyCode = '0';
		return;
	}


	if (bPressShift
		&& (*pKeyCode >= '1' && *pKeyCode <= '4'))
	{
		*pKeyCode = *pKeyCode + 5;
	}
}
//------------------------------------------------------------------------
LRESULT CBaseControlMode::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	return 0;
}
//------------------------------------------------------------------------
void CBaseControlMode::__SaveScreen(void)
{
	time_t	long_time;
	struct tm* local_time;
	time(&long_time);
	local_time = localtime(&long_time);
	
	char szPath[ MAX_PATH ];
	sprintf( szPath , "%s\\Screenshots\\screen_%02d%02d%02d%02d%02d.jpg" ,
		g_ClientGlobal->m_szWorkDir,
		local_time->tm_mon+1,
		local_time->tm_mday ,
		local_time->tm_hour, 
		local_time->tm_min, 
		local_time->tm_sec
		);
	
	SaveJpg( g_ClientGlobal->m_pScreen , szPath , 90);
}
//------------------------------------------------------------------------
void CBaseControlMode::__PickUpTreasure_Round()
{

}
//------------------------------------------------------------------------
BOOL CBaseControlMode::__TryPickup(int Tile_X, int Tile_Y)
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CBaseControlMode::__TryPickupBox(CMapItemView* pItem)
{
	return TRUE;
}
//------------------------------------------------------------------------
void CBaseControlMode::UseSkill(int SkillID)
{

}
//------------------------------------------------------------------------
void CBaseControlMode::UseSkill_Self(int SkillID)
{

}
//------------------------------------------------------------------------
BOOL CBaseControlMode::__SkillInColding(int nSkillID)
{
	return TRUE;
}
//------------------------------------------------------------------------
LRESULT CBaseControlMode::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	return -1;
}
//------------------------------------------------------------------------
BOOL CBaseControlMode::clearAttackCmd()
{
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CBaseControlMode::attack_Immediate(int SkillID)
{
	return FALSE;
}
//------------------------------------------------------------------------
void CBaseControlMode::OnSkillClickedConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue )
{
	bContinue = FALSE;
	if (wParam == FALSE) //快捷指令消息未经窗口模块处理
	{
		UseSkill(lParam);
	}
}
//------------------------------------------------------------------------
void CBaseControlMode::OnGoodsClickedConnect(WPARAM wParam, LPARAM lParam,BOOL &bContinue)
{

}
//------------------------------------------------------------------------
void CBaseControlMode::OnClearCommandQueueConnect(WPARAM wParam, LPARAM lParam,BOOL &bContinue)
{
}
//------------------------------------------------------------------------
void CBaseControlMode::OnTraceCommandConnect(WPARAM wParam, LPARAM lParam,BOOL &bContinue)
{
}
//------------------------------------------------------------------------
void CBaseControlMode::OnSelfSkillConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue)
{
	UseSkill_Self(wParam);
}
//------------------------------------------------------------------------
void CBaseControlMode::OnSelectNewTargetCommandConnect(WPARAM wParam, LPARAM lParam,BOOL &bContinue)
{
}
//------------------------------------------------------------------------
void CBaseControlMode::OnAttackTeamateCommandConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue)
{

}
//------------------------------------------------------------------------
void CBaseControlMode::OnExecutePetCommandConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue)
{

}
//------------------------------------------------------------------------
void CBaseControlMode::OnSetLockFireState(WPARAM wParam, LPARAM lParam, BOOL &bContinue)
{
}
//------------------------------------------------------------------------
void CBaseControlMode::OnCancelTargetCommandConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue)
{
}
//------------------------------------------------------------------------
void CBaseControlMode::OnLongmoveCommandConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue)
{
}
//------------------------------------------------------------------------
void CBaseControlMode::OnLoadMapBlockCommandConnect( WPARAM wParam, LPARAM lParam,BOOL &bContinue)
{
} 
//------------------------------------------------------------------------