/**
*	created:		2012-7-1   5:02
*	filename: 		MiniObjPic
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/MiniObjPic.h"
#include "../../FKGame/WndSys/WndSys.h"
#include "../../FKGame/GameGlobal/IClientGlobal.h"
//------------------------------------------------------------------------
CMiniObjPic g_MiniObjPic;
//------------------------------------------------------------------------
CMiniObjPic::CMiniObjPic()
{
	memset(m_pPIC, 0, sizeof(IBitmap *) * PIC_MAX);
	memset(m_Color, 0, sizeof(WPixel) * PIC_MAX);
	memset(m_offsetX, 0, sizeof(int) * PIC_MAX);
	memset(m_offsetY, 0, sizeof(int) * PIC_MAX);	
}
//------------------------------------------------------------------------
CMiniObjPic::~CMiniObjPic()
{

}
//------------------------------------------------------------------------
BOOL CMiniObjPic::Init()
{
	m_pPIC[PIC_NPC] = CImageLoader::GetInstance()->LoadImage("MapView\\npc.tga");
	m_pPIC[PIC_ORE] = CImageLoader::GetInstance()->LoadImage("MapView\\ore.tga");;
	m_pPIC[PIC_MEDICIAL] = CImageLoader::GetInstance()->LoadImage("MapView\\medicial.tga");;
	m_pPIC[PIC_PET] = CImageLoader::GetInstance()->LoadImage("MapView\\pet.tga");;
	m_pPIC[PIC_DOOR] = CImageLoader::GetInstance()->LoadImage("MapView\\door.tga");;
	m_pPIC[PIC_TEAMMATE] = CImageLoader::GetInstance()->LoadImage("MapView\\teamate.tga");;
	m_pPIC[PIC_OTHERACTOR] = CImageLoader::GetInstance()->LoadImage("MapView\\other.tga");;
	m_pPIC[PIC_SELF] = CImageLoader::GetInstance()->LoadImage("MapView\\self.tga");;
	m_pPIC[PIC_MONSTER] = CImageLoader::GetInstance()->LoadImage("MapView\\monster.tga");;
	
	m_Color[PIC_NPC] = WPixel(253, 167, 1);
	m_Color[PIC_ORE] = WPixel(2, 142, 147);
	m_Color[PIC_MEDICIAL] = WPixel(2, 142, 147);
	m_Color[PIC_PET] = WPixel(126, 0, 252);
	m_Color[PIC_DOOR] = WPixel(0, 255, 255);
	m_Color[PIC_TEAMMATE] = WPixel(69, 99, 246);
	m_Color[PIC_OTHERACTOR] = WPixel(0, 255, 255);
	m_Color[PIC_SELF] = WPixel(0, 220, 0);
	m_Color[PIC_MONSTER] = WPixel(255, 0, 0);

	for (int i = 0; i < PIC_MAX; ++i)
	{
		if (m_pPIC[i])
		{
			m_offsetX[i] = -(m_pPIC[i]->GetWidth() / 2);
			m_offsetY[i] = -(m_pPIC[i]->GetHeight() / 2);
		}
		else
		{
			m_bOK = FALSE;
			return FALSE;
		}
	}

	m_bOK = TRUE;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMiniObjPic::IsOK()
{
	return m_bOK;
}
//------------------------------------------------------------------------
IBitmap * CMiniObjPic::GetPIC(DWORD dwID)
{
	if (dwID < PIC_MIN || dwID >= PIC_MAX)
	{
		return NULL;
	}
	return m_pPIC[dwID];
}
//------------------------------------------------------------------------
WPixel CMiniObjPic::GetColor(DWORD dwID)
{
	if (dwID < PIC_MIN || dwID >= PIC_MAX)
	{
		return 0;
	}
	return m_Color[dwID];
}
//------------------------------------------------------------------------
BOOL CMiniObjPic::DrawPIC(ICanvas* pCanvas, int xPos, int yPos, DWORD dwPICID)
{
	IBitmap *pBitmap = GetPIC(dwPICID);
	if (pBitmap == NULL)
	{
		return FALSE;
	}

	pCanvas->DrawAlpha(xPos + m_offsetX[dwPICID], yPos + m_offsetY[dwPICID], 
				pBitmap->GetWidth(), pBitmap->GetHeight(), (IAlphaBitmap *)pBitmap, 0, 0);
	return TRUE;
}
//------------------------------------------------------------------------