/**
*	created:		2012-7-1   19:56
*	filename: 		GlobalFunc
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/GlobalFunc.h"
#include "../../FKGame/DataArchive/IDataArchive.h"
#include "../../FKGame/MapInfo/Tile.h"
#include "../Include/BasicCommon.h"
//------------------------------------------------------------------------
int Fun_PlaySound(POINT& ptTile, int nSoundID, BOOL bLoop)
{
	if(nSoundID <= 0 || g_pSound == NULL)
		return -1;

	POINT ptTileCenter;
	g_pMapView->TileToScreen(ptTile,ptTileCenter);
	ptTileCenter.x -= 400;
	ptTileCenter.y -= 300;

	int nRet = g_pSound->PlaySound(nSoundID, bLoop, ptTileCenter.x, ptTileCenter.y);

	if(!bLoop)
		nRet = -1;

	return nRet;
}
//------------------------------------------------------------------------
void Fun_StopSound(int& nChunnelID)
{
	if(nChunnelID < 0 || g_pSound == NULL)
		return;

	g_pSound->StopSound(nChunnelID);
	nChunnelID = -1;
}
//------------------------------------------------------------------------
void Fun_CalcStepTile(int nDir, int nStep, POINT& ptStep)
{
	int nDblStep = nStep << 1;
	switch (nDir) 
	{
	case 0: ptStep.x = -nStep;	ptStep.y =  nStep;	break;	// 垂直上
	case 1: ptStep.x =  0;		ptStep.y =  nStep;	break;	// 右上
	case 2: ptStep.x =  nStep;	ptStep.y =  nStep;	break;	// 水平右
	case 3: ptStep.x =  nStep;	ptStep.y =  0;		break;	// 右下
	case 4: ptStep.x =  nStep;	ptStep.y = -nStep;	break;	// 垂直下
	case 5: ptStep.x =  0;		ptStep.y = -nStep;	break;	// 左下
	case 6: ptStep.x = -nStep;	ptStep.y = -nStep;	break;	// 水平左
	case 7: ptStep.x = -nStep;	ptStep.y =  0;		break;	// 左上
	}
}
//------------------------------------------------------------------------
void Fun_CalcStepPixel(int nDir, int nStep, POINT& ptStep)
{
	int nDblStep = nStep << 1;
	switch (nDir) 
	{
	case 0: ptStep.x =  0;			ptStep.y = -nStep;	break;	// 垂直上
	case 1: ptStep.x =  nDblStep;	ptStep.y = -nStep;	break;	// 右上
	case 2: ptStep.x =  nDblStep;	ptStep.y =  0;		break;	// 水平右
	case 3: ptStep.x =  nDblStep;	ptStep.y =  nStep;	break;	// 右下
	case 4: ptStep.x =  0;			ptStep.y =  nStep;	break;	// 垂直下
	case 5: ptStep.x = -nDblStep;	ptStep.y =  nStep;	break;	// 左下
	case 6: ptStep.x = -nDblStep;	ptStep.y =  0;		break;	// 水平左
	case 7: ptStep.x = -nDblStep;	ptStep.y = -nStep;	break;	// 左上
	}
}
//------------------------------------------------------------------------
POINT Fun_step[32] =
{
	{0,-15},{3,-15},{6,-14},{8,-12},{11,-11},{12,-8},{14,-6},{15,-3},
	{15,0},{15,3},{14,6},{12,8},{11,11},{8,12},{6,14},{3,15},
	{0,15},{-3,15},{-6,14},{-8,12},{-11,11},{-12,8},{-14,6},{-15,3},
	{-15,0},{-15,-3},{-14,-6},{-12,-8},{-11,-11},{-8,-12},{-6,-14},{-3,-15},
};
//------------------------------------------------------------------------
// 计算某个方向上的速度(受45度倾斜的影响)
POINT Fun_CalcDirSpeedDefaultSpeed32_45(int Dir)
{
	return Fun_step[Dir];
}
//------------------------------------------------------------------------
// 按方向增长率矫正方向
void Fun_CalcNewDir(float &fOldDir, float fNewDir, float fIncDir)
{
	if(fIncDir < 0)
	{
		fOldDir = fNewDir;
	}
	else
	{
		float nInc = fOldDir - fNewDir;

		if((nInc < 16 && nInc > 0) || nInc < -16 )
			fOldDir -= fIncDir;
		else if(nInc > 16 || (nInc > -16 && nInc < 0))
			fOldDir += fIncDir;
		else if(nInc == 16 || nInc == -16)
		{
			if(rand() % 2 == 0)
				fOldDir -= fIncDir;
			else
				fOldDir += fIncDir;
		}

		if(fOldDir< 0)
			fOldDir = 31 + fOldDir;
		else if(fOldDir > 31)
			fOldDir = fOldDir - 31;

		nInc = fOldDir - fNewDir;
		if((nInc < fIncDir && nInc > -fIncDir) || ((nInc > (32 - fIncDir) || (nInc < -(32 - fIncDir)))))
			fOldDir = fNewDir;
	}
}
//------------------------------------------------------------------------