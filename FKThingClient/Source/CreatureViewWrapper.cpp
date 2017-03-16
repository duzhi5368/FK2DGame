/**
*	created:		2012-7-2   23:58
*	filename: 		CreatureViewWrapper
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		生物显示包装类
*/
//------------------------------------------------------------------------
#include "../Include/CreatureViewWrapper.h"
#include "../Include/CThingClientFacade.h"
//------------------------------------------------------------------------
CCreatureViewWrapper::CCreatureViewWrapper(CMapItemView * pItemView)
{
	m_pItemView = pItemView; 
}
//------------------------------------------------------------------------
CMapItemView * CCreatureViewWrapper::operator ->()
{
	return m_pItemView;	
}
//------------------------------------------------------------------------
BOOL CCreatureViewWrapper::IsValid(void)
{
	return m_pItemView != NULL; 
}
//------------------------------------------------------------------------
CMapItemView * CCreatureViewWrapper::GetItemView(void)
{
	return m_pItemView; 
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::SetData(DWORD dwData)
{
	(IsValid() == TRUE) ? m_pItemView->SetItemData(dwData) : 0; 
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::AddFlag(DWORD dwFlag)
{
	(IsValid() == TRUE) ? m_pItemView->AddFlag(dwFlag) : 0; 
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::RemoveFlag(DWORD dwFlag)
{
	(IsValid() == TRUE) ? m_pItemView->RemoveFlag(dwFlag) : 0;
}
//------------------------------------------------------------------------
BOOL CCreatureViewWrapper::AddToMap(POINTS ptTile)
{
	POINT ptPos = { ptTile.x, ptTile.y };

	return g_pThingClientFacade->GetMapView()->AddItem(ptPos, m_pItemView);
}
//------------------------------------------------------------------------
DWORD CCreatureViewWrapper::GetMoveStyle(void)
{
	DWORD dwStyle = 0;
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdGetProperty, CrtPropMoveStyle, (DWORD)&dwStyle) : 0;
	return dwStyle;
}
//------------------------------------------------------------------------
void  CCreatureViewWrapper::SetMoveStyle(DWORD dwStyle)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropMoveStyle, (DWORD)&dwStyle) : 0;
}
//------------------------------------------------------------------------
POINT CCreatureViewWrapper::GetTile(void)
{
	POINT ptTile = { 0, 0 };
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdGetProperty, CrtPropTile, (DWORD)&ptTile) : 0;
	return ptTile;
}
//------------------------------------------------------------------------
int   CCreatureViewWrapper::GetDirection(void)
{
	int nDir = CrtDirNorth;
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdGetProperty, CrtDirNorth, (DWORD)&nDir) : 0;
	return nDir;
}
//------------------------------------------------------------------------
void  CCreatureViewWrapper::SetDirection(int nDir)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropDirection, (DWORD)&nDir) : 0;
}
//------------------------------------------------------------------------
void  CCreatureViewWrapper::SetTile(POINTS ptTile)
{
	POINT ptPos = { ptTile.x, ptTile.y };
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropTile, (DWORD)&ptPos) : 0;
}
//------------------------------------------------------------------------
void  CCreatureViewWrapper::GetName(LPSTR szBuf)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdGetProperty, CrtPropUserName, (DWORD)szBuf) : 0;
}
//------------------------------------------------------------------------
void  CCreatureViewWrapper::SetName(LPCSTR szName)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropUserName, (DWORD)szName) : 0;
}
//------------------------------------------------------------------------
BOOL  CCreatureViewWrapper::GetPropShowName(void)
{
	BOOL bShow = FALSE;
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdGetProperty, CrtPropShowName, (DWORD)&bShow) : 0;
	return bShow;
}
//------------------------------------------------------------------------
void  CCreatureViewWrapper::SetPropShowName(BOOL bShow)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropShowName, (DWORD)&bShow) : 0;
}
//------------------------------------------------------------------------
void  CCreatureViewWrapper::SetNameColor(int r, int g, int b)
{
	WPixel c(r, g, b);
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropNameColor, (DWORD)&c) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::SetNameColor(DWORD dwColor)
{
	WPixel c = (WPixel)dwColor;
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropNameColor, (DWORD)&c) : 0;
}
//------------------------------------------------------------------------
int	 CCreatureViewWrapper::GetState(void)
{
	long nRt = 0;
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdGetProperty, CrtPropState, (DWORD)&nRt) : 0;
	return nRt;
}
//------------------------------------------------------------------------
void  CCreatureViewWrapper::SetState( int nState)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropState, (DWORD)&nState) : 0;
}
//------------------------------------------------------------------------
POINT CCreatureViewWrapper::GetPosition(void)
{
	POINT ptLoc;
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdGetProperty, CrtPropAbsolutePosition, (DWORD)&ptLoc) : 0;
	return ptLoc;
}
//------------------------------------------------------------------------
void  CCreatureViewWrapper::SetPosition(POINT ptPos)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropAbsolutePosition, (DWORD)&ptPos) : 0;
}
//------------------------------------------------------------------------
DWORD CCreatureViewWrapper::GetPropWeaponStyle(void)
{
	DWORD dwStyle;
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdGetProperty, CrtPropWeaponStyle, (DWORD)&dwStyle) : 0;
	return dwStyle;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::SetPropWeaponStyle(DWORD dwStyle)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropWeaponStyle, (DWORD)&dwStyle) : 0;
}
//------------------------------------------------------------------------
BOOL  CCreatureViewWrapper::IsMoving(void)
{
	BOOL bMoving = FALSE;
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdGetProperty, CrtPropIsMoving, (DWORD)&bMoving) : 0;
	return bMoving;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::Move(POINTS * pPath, int nPathLen)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdMove, (DWORD)pPath, nPathLen) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::Stand(POINT ptTile)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdStand, (DWORD)&ptTile, 0) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::ChangeLocForSkill(POINT ptTile)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdChangeLocForSkill, (DWORD)&ptTile, 0) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::Sit(void)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetting, TRUE, 0) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::BeHit(SBehitParam * pParam)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdBeHit, (DWORD)pParam, 0) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::Die(DWORD dwParam1, DWORD dwParam2)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdDie, dwParam1, dwParam2) : 0; 
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::Fallow(void)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdFallow, 0, 0) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::Born(void)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdBorn, 0, 0) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::Upgrade(void)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdUpgrade, 0, 0) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::ChangeModule(DWORD dwModule, DWORD dwResID)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdChangeModule, dwModule, dwResID) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::ChangeModulePal(DWORD dwModule, DWORD dwPalType)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdModulePalType, dwModule, dwPalType) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::SpecialModule(DWORD dwModule, DWORD dwResID)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSpecialModule, dwModule, dwResID) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::StopAttack(void)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdStopAttack, 0, 0) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::Attack(DWORD pParam)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdAttack, (DWORD)pParam, 0) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::StopPrepAttack(void)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdStopPreAttack, 0, 0) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::PrepAttack(DWORD pParam)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdPreAttack, (DWORD)pParam, 0) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::ChangeToState(int nState, BOOL bFlag)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdAppearance, bFlag, nState) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::DestroyMagic(DWORD dwParam)
{
	//	m_pItemView->OnCommand(CrtCmdDestroyMagic, dwParam, sizeof(DWORD) );
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::SetMoveSpeedLevel(int * pSpeed)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropMoveSpeedLevel, (DWORD)pSpeed) : 0;
}
//------------------------------------------------------------------------
int CCreatureViewWrapper::GetMoveSpeedLevel(void)
{
	int nSpeedLevel = 0;
	if (IsValid())
	{
		m_pItemView->OnCommand(CrtCmdGetProperty, CrtPropMoveSpeedLevel, (DWORD)&nSpeedLevel);
	}
	return nSpeedLevel;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::SetAttackSpeed(int * pSpeed)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropAttackSpeedLevel, (DWORD)pSpeed) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::SetBloodInfo( int nNowBlood, int nTotalBlood)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdBloodInfo, (DWORD)&nNowBlood, (DWORD)&nTotalBlood) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::SetChatInfo(LPCSTR szMsg, WPixel * pColor)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdChatInfo , (DWORD)szMsg, (DWORD)pColor) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::SetTribeTitle(IBitmap * pBitmap)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropGroupInfoImage, (DWORD)&pBitmap) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::SetTitle(LPCSTR szMsg, DWORD dwEffectID)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdClearTitle, 0, 0) : 0;
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdAddTitle, (DWORD)szMsg, dwEffectID) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::SetWeaponAttackSoundID(int nSoundID)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropWeaponSoundID, (int)&nSoundID) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::SetWeaponBeHitSoundID(int nSoundID)
{
	(IsValid() == TRUE) ? m_pItemView->OnCommand(CrtCmdSetProperty, CrtPropWeaponBeHitSoundID, (int)&nSoundID) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::SetFlagColorOnMinmap(DWORD dwColor)
{
	(IsValid() == TRUE) ? m_pItemView->SetFlagColorOnMinmap(dwColor) : 0;
}
//------------------------------------------------------------------------
void CCreatureViewWrapper::SetFlagColorOnMinmap(int r, int g, int b)
{
	WPixel wpFlagColor(r, g, b);
	(IsValid() == TRUE) ? m_pItemView->SetFlagColorOnMinmap(wpFlagColor) : 0;
}
//------------------------------------------------------------------------