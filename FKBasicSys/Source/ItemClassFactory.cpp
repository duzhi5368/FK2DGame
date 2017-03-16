/**
*	created:		2012-7-2   4:34
*	filename: 		ItemClassFactory
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
/*
	功能：	定义和创建地图上几种常用的类型的地上物类
			如静态物、多占位块静态物、固定动画、运动动画
 */
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../../FKGame/Timer/ITimerAxis.h"
#include "../Include/ItemClassFactory.h"
#include "../../FKGame/MapView/MapItemView.h"
#include "../Include/StaticObj.h"
#include "../Include/AniObj.h"
#include "../Include/Magic.h"
#include "../Include/MonsterView.h"
#include "../Include/ActorView.h"
//------------------------------------------------------------------------
CMagicFlowText* g_pFlowNum[12][enNUM_TYPE_MAX_NUM][2]	= {NULL};	// 数字和符号
CMagicFlowText* g_pCrazy[enNUM_TYPE_MAX_NUM]			= {NULL};	// 暴击
CMagicFlowText* g_pHedge								= {NULL};	// 躲闪
CMagicFlowText* g_pResist								= {NULL};	// 抵抗
//------------------------------------------------------------------------
CMagic*			g_pSkillSelMagic = NULL;	// 范围技能的选择光效
//------------------------------------------------------------------------
CAniLoader*		 g_pAniLoader = NULL;
CAniGroupLoader* g_pAniGroupLoader = NULL;
//------------------------------------------------------------------------
int g_CommonNumWidthHalf[3] = {0,0,0};
int g_CommonSignWidthHalf[3] = {0,0,0};
int g_CrazyNumWidthHalf[3] = {0,0,0};
int g_CrazySignWidthHalf[3] = {0,0,0};
int g_CharWidthHalf[3] = {0,0,0};
//------------------------------------------------------------------------
CItemClassFactory::ProcCreator CItemClassFactory::m_bufCreateFunctions[miMaxClassID] =
{
	NULL,							// 0
	&CItemClassFactory::CreateStaticObjTrans,			// 1
	&CItemClassFactory::CreateStaticObjMultiOccTrans,	// 2
	&CItemClassFactory::CreateAniObj,					// 3
	&CItemClassFactory::CreateAniObjMultiOcc,			// 4
	&CItemClassFactory::CreateStaticObj,				// 5
	NULL,							// 6
	&CItemClassFactory::CreateStaticObjSingleOcc,		// 7
	&CItemClassFactory::CreateStaticObjSingleOccTrans,	// 8
	&CItemClassFactory::CreateStaticObjMultiOcc,		// 9
	&CItemClassFactory::CreateMagic,					// 10
	&CItemClassFactory::CreateMonsterView,				// 11
	&CItemClassFactory::CreateActorView,				// 12
	&CItemClassFactory::CreateAniObjDiscrete,			// 13
	&CItemClassFactory::CreateAniObjMultiOccDiscrete,							// 14
	NULL,							// 15
	&CItemClassFactory::CreateAniObjMultiOccDiscrete,	// 16
	&CItemClassFactory::CreateStaticObjPicture,			// 17
	NULL,							// 18	
	&CItemClassFactory::CreateStaticObjArticle,			// 19
	NULL,							// 20
	NULL,							// 21
	NULL,							// 22
	NULL,							// 23
	NULL,							// 24
	NULL,							// 25
	NULL,							// 26
	&CItemClassFactory::CreateStaticObjSpecificBlock,	// 27
	NULL,							// 28
	&CItemClassFactory::CreateStaticObjSpecificBlockDown,// 29
	NULL,							// 30
	NULL,							// 31
	&CItemClassFactory::CreateAniObjTransport,			// 32
	&CItemClassFactory::CreateStaticObjBooth,			// 33
	NULL,							// 34
	NULL,							// 35
};
//------------------------------------------------------------------------
CMapItem* CItemClassFactory::CreateItem(DWORD dwClassID, IArchive* pArchive)
{
	if (dwClassID >= miMaxClassID)	// 当前未支持的命令码
	{
		PrintEventLn("ERROR: CItemClassFactory::CreateItem(archive) ,Unknow classID=%d",dwClassID);
		return NULL;
	}
	/*
#ifdef _USE
	switch (dwClassID) 
	{
	case miStaticObj_NoOcc_NoTrans:
	case miStaticObj_NoOcc_Trans:
	case miStaticObj_SingleOcc_NoTrans:
	case miStaticObj_SingleOcc_Trans:
	case miStaticObj_MultiOcc_NoTrans:
	case miStaticObj_MultiOcc_Trans:
	case miSpecificBlock:
	case miSpecificBlockDown:
		{
			long lOccOption, lTransOption, lSpecificBlock;
			IResObject* pStaticObj = g_ResLoader.m_pScpBmp->GetResObject(*(DWORD*)pArchive->GetBuffer());
			if (!pStaticObj) return NULL;
			pStaticObj->GetPropertyLong("SpecificBlock", lSpecificBlock, 0);
			pStaticObj->GetPropertyLong("OccupantOption", lOccOption, 0);
			pStaticObj->GetPropertyLong("TransOption", lTransOption, 0);
			if (lSpecificBlock != 0)
				dwClassID = (lSpecificBlock==1)?miSpecificBlock:miSpecificBlockDown;
			else if (lTransOption == 0)
			{
				if (lOccOption == 0)
					dwClassID = miStaticObj_NoOcc_NoTrans;
				else if (lOccOption == 1)
					dwClassID = miStaticObj_SingleOcc_NoTrans;
				else if (lOccOption == 2)
					dwClassID = miStaticObj_MultiOcc_NoTrans;
			}
			else
			{
				if (lOccOption == 0)
					dwClassID = miStaticObj_NoOcc_Trans;
				else if (lOccOption == 1)
					dwClassID = miStaticObj_SingleOcc_Trans;
				else if (lOccOption == 2)
					dwClassID = miStaticObj_MultiOcc_Trans;
			}
		}
		break;
	case miAniObj_NoOcc_RGB:
	case miAniObj_SingleOcc_RGB:
	case miAniObj_MultiOcc_RGB:
	case miAniObj_NoOcc_RGBA:
	case miAniObj_SingleOcc_RGBA:
	case miAniObj_MultiOcc_RGBA:
		{
			long lOccOption, lDelay1;
			IResObject* pAniObj = g_ResLoader.m_pScpAni->GetResObject(*(DWORD*)pArchive->GetBuffer());
			if (!pAniObj) return NULL;
			pAniObj->GetPropertyLong("OccupantOption", lOccOption, 0);
			pAniObj->GetPropertyLong("Delay1", lDelay1, 0);
			if (lOccOption == 0)
			{
				if (lDelay1 == 0)
					dwClassID = miAniObj_NoOcc_RGB;
				else
					dwClassID = miAniObj_NoOcc_RGBA;
			}
			else if (lOccOption == 1)
			{
				if (lDelay1 == 0)
					dwClassID = miAniObj_NoOcc_RGB;
				else
					dwClassID = miAniObj_NoOcc_RGBA;
			}
			else if (lOccOption == 2)
			{
				if (lDelay1 == 0)
					dwClassID = miAniObj_MultiOcc_RGB;
				else
					dwClassID = miAniObj_MultiOcc_RGBA;
			}
		}
		break;
	}
#endif
	*/
	if (m_bufCreateFunctions[dwClassID] == NULL)
	{
		PrintEventLn("ERROR: CItemClassFactory::CreateItem(archive), m_bufCreateFunctions[dwClassID=%d] == NULL", dwClassID);
		return NULL;
	}

	DWORD dwParam = *(DWORD*)pArchive->GetBuffer();
	CMapItemView* pItem = (this->*m_bufCreateFunctions[dwClassID])(dwParam);
	if (pItem == NULL)
	{
		PrintEventLn("ERROR: CItemClassFactory::CreateItem(archive), pItem(CID=%d,RID=%d) == NULL", dwClassID, dwParam);
		return NULL;
	}
	if (pItem->OnSerialize(FALSE, pArchive))
		return (CMapItem*)pItem;
	else
	{
		PrintEventLn("ERROR: CItemClassFactory::CreateItem(archive), pItem->OnSerialize(CID=%d,RID=%d) == NULL", dwClassID, dwParam);
		pItem->Release();
		return NULL;
	}
	return NULL;
}
//------------------------------------------------------------------------
CMapItem* CItemClassFactory::CreateItem(DWORD dwClassID, const char* buf, int nLen)
{
	if (dwClassID >= miMaxClassID)	// 当前未支持的dwClassID
	{
		PrintEventLn("ERROR: CItemClassFactory::CreateItem(buf) ,Unknow classID=%d", dwClassID);
		return NULL;
	}
	if (m_bufCreateFunctions[dwClassID] == NULL)
	{
		PrintEventLn("ERROR: CItemClassFactory::CreateItem(buf), m_bufCreateFunctions[dwClassID=%d] == NULL", dwClassID);		
		return NULL;
	}

	DWORD dwParam = *(DWORD*)buf;
	CMapItemView* pItem = (this->*m_bufCreateFunctions[dwClassID])(dwParam);
	if (pItem == NULL)
	{
		PrintEventLn("ERROR: CItemClassFactory::CreateItem(buf), pItem(CID=%d,RID=%d) == NULL", dwClassID, dwParam);		
		return NULL;
	}
	if (pItem->OnCreateItem(buf, nLen))
		return (CMapItem*)pItem;
	else
	{
		PrintEventLn("ERROR: CItemClassFactory::CreateItem(buf), pItem->OnCreateItem(CID=%d,RID=%d) == NULL", dwClassID, dwParam);
		pItem->Release();
		return NULL;
	}
	return NULL;
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateStaticObj(DWORD dwParam)
{
	return  (CMapItemView*) new CStaticObj();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateStaticObjTrans(DWORD dwParam)
{
	return  (CMapItemView*) new CStaticObjTrans();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateStaticObjSingleOcc(DWORD dwParam)
{
	return  (CMapItemView*) new CStaticObjSingleOcc();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateStaticObjSingleOccTrans(DWORD dwParam)
{
	return  (CMapItemView*) new CStaticObjSingleOccTrans();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateStaticObjMultiOcc(DWORD dwParam)
{
	return  (CMapItemView*) new CStaticObjMultiOcc();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateStaticObjMultiOccTrans(DWORD dwParam)
{
	return  (CMapItemView*) new CStaticObjMultiOccTrans();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateAniObj(DWORD dwParam)
{
	return  (CMapItemView*) new CAniObj();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateAniObjDiscrete(DWORD dwParam)
{
	return  (CMapItemView*) new CAniObjDiscrete();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateAniObjMultiOcc(DWORD dwParam)
{
	return  (CMapItemView*) new CAniObjMultiOcc();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateAniObjMultiOccDiscrete(DWORD dwParam)
{
	return  (CMapItemView*) new CAniObjMultiOccDiscrete();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateAniObjTransport(DWORD dwParam)
{
	return  (CMapItemView*) new CAniObjTransport();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateMagic(DWORD dwParam)
{
	return  (CMapItemView*) new CMagic();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateMonsterView(DWORD dwParam)
{
	return  (CMapItemView*) new CMonsterView();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateActorView(DWORD dwParam)
{
	return  (CMapItemView*) new CActorView();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateStaticObjSpecificBlock(DWORD dwParam)
{
	return (CMapItemView*) new CStaticObjSpecificBlock();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateStaticObjSpecificBlockDown(DWORD dwParam)
{
	return (CMapItemView*) new CStaticObjSpecificBlockDown();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateStaticObjArticle(DWORD dwParam)
{
	return (CMapItemView*) new CStaticObjArticle();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateStaticObjPicture(DWORD dwParam)
{
	return (CMapItemView*) new CStaticObjPicture();
}
//------------------------------------------------------------------------
CMapItemView* CItemClassFactory::CreateStaticObjBooth(DWORD dwParam)
{
	return (CMapItemView*) new CStaticObjBooth();
}
//------------------------------------------------------------------------
int CItemClassFactory::GetAllScript(IResScript** ppAllScp)
{
#ifndef _USE_BINSCRIPT
	if (ppAllScp)
	{
		ppAllScp[0] = g_ResLoader.m_pScpBmp;
		ppAllScp[1] = g_ResLoader.m_pScpAni;
		ppAllScp[2] = g_ResLoader.m_pScpMagic;
		ppAllScp[3] = g_ResLoader.m_pScpCreature;
		ppAllScp[4] = g_ResLoader.m_pScpBodyPart;
	}
#endif
	return 5;
}
//------------------------------------------------------------------------
int CItemClassFactory::ReLoadScript(int nScpTypeID)
{
	return g_ResLoader.ReLoadScript(nScpTypeID);
}
//------------------------------------------------------------------------
void CItemClassFactory::ClearItemBuffer()
{
	g_ResLoader.m_ObjectPool.Recycle();
}
//------------------------------------------------------------------------
void CItemClassFactory::Release()
{
	g_ResLoader.Close(); // 释放各种资源先
	
	for(int i = 0; i < enNUM_TYPE_MAX_NUM; i++)
	{
		for(int j = 0; j < 12; j++)
		{
			SAFE_RELEASE(g_pFlowNum[j][i][0]);
			SAFE_RELEASE(g_pFlowNum[j][i][1]);
		}
		SAFE_RELEASE(g_pCrazy[i]);
	}
	SAFE_RELEASE(g_pResist);
	SAFE_RELEASE(g_pHedge);
	
	SAFE_RELEASE(g_pSkillSelMagic);
	
	SAFE_DELETE(g_pAniLoader);
	SAFE_DELETE(g_pAniGroupLoader);

#ifdef _DEBUG
	OutputDebugString("IItemClassFactory::Release()");
#endif
	delete this;
}
//------------------------------------------------------------------------
//角色预处理
BOOL CItemClassFactory::ChangeModule(int nActIndex, int nIndex, DWORD dwResID, BYTE nPal, BYTE nAlpha)
{
	if (nActIndex < 0 || nActIndex >= MAX_ACTOR_NUM)
		return FALSE;
	return m_ActPreView[nActIndex].ChangeModule(nIndex, dwResID, nPal, nAlpha);
}
//------------------------------------------------------------------------
void CItemClassFactory::OnDraw(int nActIndex, ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if (nActIndex < 0 || nActIndex >= MAX_ACTOR_NUM)
		return;
	m_ActPreView[nActIndex].OnDraw(pCanvas, ptTileCenter, dwFlag);
}
//------------------------------------------------------------------------
void CItemClassFactory::ReleasePreView(int nActIndex)
{
	if (nActIndex < 0 || nActIndex >= MAX_ACTOR_NUM)
		return;
	m_ActPreView[nActIndex].ReleasePreView();
}
//------------------------------------------------------------------------
void CItemClassFactory::SetState(int nActIndex, int nState)
{
	if (nActIndex < 0 || nActIndex >= MAX_ACTOR_NUM)
		return;
	m_ActPreView[nActIndex].SetState(nState);
}
//------------------------------------------------------------------------
void CItemClassFactory::CreatePreView(int nActIndex, int nType)
{
	if (nActIndex < 0 || nActIndex >= MAX_ACTOR_NUM)
		return;
	if (nType >=0 && nType < MAX_CREATURE_TYPES)
	{
		m_ActPreView[nActIndex].CreatePreView(nType);
	}
}
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) BOOL CreateItemClassFactory(DWORD dwExpectVersion, IItemClassFactory** ppItemClassFactory, IClientGlobal* pGlobal)
{
	if (pGlobal)
		gp = pGlobal;
	else
		return FALSE;
	TraceLn("Start create BasicSys");
	
	if (dwExpectVersion != VERSION_QUERY_ITEMCLASSFACTORY)
	{
		PrintEventLn("ERROR: Create BasicSys, version error!");
		return FALSE;
	}
	
	if (!g_ResLoader.InitFileSystem(pGlobal))
	{
		PrintEventLn("ERROR: Create BasicSys, init file system error!");
		return FALSE;
	}
	
	CItemClassFactory* pItemClassFactory = new CItemClassFactory();
	if( pItemClassFactory == NULL )
	{
		PrintEventLn("错误: 创建 ItemClassFactory 失败！");
		return FALSE;
	}
	*ppItemClassFactory = static_cast<IItemClassFactory*>(pItemClassFactory);
	srand(GetTickCount());

	/* 
	// 漂血，致命抵抗闪避 等图片
	DWORD dwResID = 10000;
	DWORD dwTemp;
	int i, j, k;
	for(i = 0; i < 12; i++)
	{
		for(j = 0; j < enNUM_TYPE_MAX_NUM; j++)
		{
			g_pFlowNum[i][j][0] = new CMagicFlowText;
			ASSERT(g_pFlowNum[i][j][0]);
			dwTemp = dwResID + i * (enNUM_TYPE_MAX_NUM * 2 + 1) + j * 2;
			g_pFlowNum[i][j][0]->OnCreateItem((char*)&dwTemp, sizeof(DWORD));

			g_pFlowNum[i][j][1] = new CMagicFlowText;
			ASSERT(g_pFlowNum[i][j][1]);
			dwTemp = dwResID + i * (enNUM_TYPE_MAX_NUM * 2 + 1) + j * 2 + 1;
			g_pFlowNum[i][j][1]->OnCreateItem((char*)&dwTemp, sizeof(DWORD));

			for(k = 0; k < 3; k++) // 获取每祯图片大小
			{
				if(i < 10)
				{
					if(g_pFlowNum[i][j][0]->GetBitmapWidthHalf(k) > g_CommonNumWidthHalf[k])
						g_CommonNumWidthHalf[k] = g_pFlowNum[i][j][0]->GetBitmapWidthHalf(k);
				}
				else
				{
					if(g_pFlowNum[i][j][0]->GetBitmapWidthHalf(k) > g_CommonSignWidthHalf[k])
						g_CommonSignWidthHalf[k] = g_pFlowNum[i][j][0]->GetBitmapWidthHalf(k);
				}
				if(i < 10)
				{
					if(g_pFlowNum[i][j][1]->GetBitmapWidthHalf(k) > g_CrazyNumWidthHalf[k])
						g_CrazyNumWidthHalf[k] = g_pFlowNum[i][j][1]->GetBitmapWidthHalf(k);
				}
				else
				{
					if(g_pFlowNum[i][j][1]->GetBitmapWidthHalf(k) > g_CrazySignWidthHalf[k])
						g_CrazySignWidthHalf[k] = g_pFlowNum[i][j][1]->GetBitmapWidthHalf(k);
				}
			}
		}
	}

	for(i = 0; i < enNUM_TYPE_MAX_NUM; i++)
	{
		g_pCrazy[i] = new CMagicFlowText;
		ASSERT(g_pCrazy[i]);

		dwResID = 10132 + i;		// 致命
		g_pCrazy[i]->OnCreateItem((char*)&dwResID, sizeof(DWORD));

		for(k = 0; k < 3; k++)
		{
			if(g_pCrazy[i]->GetBitmapWidthHalf(k) > g_CharWidthHalf[k])
				g_CharWidthHalf[k] = g_pCrazy[i]->GetBitmapWidthHalf(k);
		}
	}

	g_pResist = new CMagicFlowText;
	ASSERT(g_pResist);
	g_pHedge = new CMagicFlowText;
	ASSERT(g_pHedge);

	dwResID = 10142;				// 抵抗
	g_pResist->OnCreateItem((char*)&dwResID, sizeof(DWORD));
	dwResID = 10140;				// 躲闪
	g_pHedge->OnCreateItem((char*)&dwResID, sizeof(DWORD));

	for(k = 0; k < 3; k++)
	{
		if(g_pResist->GetBitmapWidthHalf(k) > g_CharWidthHalf[k])
			g_CharWidthHalf[k] = g_pResist->GetBitmapWidthHalf(k);

		if(g_pHedge->GetBitmapWidthHalf(k) > g_CharWidthHalf[k])
			g_CharWidthHalf[k] = g_pHedge->GetBitmapWidthHalf(k);
	}
	*/

	g_pAniLoader = new CAniLoader;
	if(!g_pAniLoader->Create())
	{
		return FALSE;
	}
	
	g_pAniGroupLoader = new CAniGroupLoader;
	if(!g_pAniGroupLoader->Create())
	{
		return FALSE;
	}
	
	PrintEventLn("Finish BasicSys.Create");
	return *ppItemClassFactory != NULL;
}
//------------------------------------------------------------------------