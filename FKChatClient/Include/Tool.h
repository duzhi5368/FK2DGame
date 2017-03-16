/**
*	created:		2012-6-28   2:46
*	filename: 		Tool
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <string>
//------------------------------------------------------------------------
#include "../../FKGame/Container/IContainerClient.h"
#include "../../FKGame/SceneObj/ICreature.h"
#include "../../FKGame/SceneObj/IGoods.h"
#include "../../FKCanvas/IFKCanvas.h"
//------------------------------------------------------------------------
using std::string;
//------------------------------------------------------------------------
class CTool  
{
public:
	CTool();
	virtual ~CTool();
public:
	static CTool* Instance();
public:
	LPCSTR GetNameByUID(UID uid);
	void Speak(char *lpszFormat,...);
	ICreature* GetCreatureThingByUID(UID uid);
	IThing* GetThing(UID uid);
	string GetGoodNameByGoodID(DWORD dwID);
	void IntMoneyToStr(int nMoney,char *lpsz);
	BOOL RemoveAddGoods( IContainerGoods *pGoods,IContainer *pContanier,int& nLocation);
	BOOL ReleaseContainer(IContainer *pContainer,BOOL bReleaseGoods);
	BOOL PlaySound(int nSoundId, BOOL bLoop = FALSE);
	void Random(BYTE by[],int nLen,int nMin,int nMax);
	int CouputeDistance(UID uid);
	string& BuilderToolTip(UID uid);
	string& BuilderToolTip(IGoods *pGoods);
	IBitmap* GetGoodsBitmap(DWORD dwGoodsId);
	long GetGoodsLevelByGoodsId(DWORD dwGoodsId);
	long GetGoodsLevelByUID(UID UId);
	WPixel GetGoodsNameColor(IGoods *pGoods);
	int GetEffectCount(IGoods *pGoods);
	int GetEquipmentEndUse(UID uid);
private:
	static CTool *			s_pInstance;
	string					m_strToolTip;
};
//------------------------------------------------------------------------





















