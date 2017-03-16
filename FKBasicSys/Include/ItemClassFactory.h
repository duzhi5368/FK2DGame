/**
*	created:		2012-7-2   4:32
*	filename: 		ItemClassFactory
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/IItemClassFactory.h"
#include "../CBasicSysHelper.h"
#include "../../FKGame/MapView/MapItemView.h"
#include "../Include/ResLoader.h"
#include "../../FKGame/Common/MapItemDef.h"
#include "ActorPreView.h"
//------------------------------------------------------------------------
class CItemClassFactory : public IItemClassFactory
{
public:
	typedef CMapItemView* (CItemClassFactory::*ProcCreator)(DWORD dwParam);
	static ProcCreator m_bufCreateFunctions[miMaxClassID];

public:
	CActPreView						m_ActPreView[MAX_ACTOR_NUM];		//角色预处理

public:
	virtual int GetAllScript(IResScript** ppAllScp);
	virtual int ReLoadScript(int nScpTypeID);
	virtual void ClearItemBuffer();

	CItemClassFactory(){};
	virtual ~CItemClassFactory(){};

	virtual CMapItem* CreateItem(DWORD dwClassID,IArchive* pArchive);
	virtual CMapItem* CreateItem(DWORD dwClassID,const char* buf, int nLen);
	virtual void Release();
	
	//角色预处理接口
	//更换图包
	virtual BOOL ChangeModule (int nActIndex, int nIndex, DWORD dwResID, BYTE nPal = 0, BYTE nAlpha = 0);
	virtual void OnDraw(int nActIndex, ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag);
	virtual void ReleasePreView(int nActIndex);	//释放PreView的图包资源，清除所有的调色板和Alpha数据
	virtual void SetState(int nActIndex, int nState);
	virtual void CreatePreView(int nActIndex, int nType);
	
	virtual BOOL InitFactory () {return TRUE;};			//初始化
public:
	CMapItemView* CreateStaticObj(DWORD dwParam);
	CMapItemView* CreateStaticObjTrans(DWORD dwParam);
	CMapItemView* CreateStaticObjSingleOcc(DWORD dwParam);
	CMapItemView* CreateStaticObjSingleOccTrans(DWORD dwParam);
	CMapItemView* CreateStaticObjMultiOcc(DWORD dwParam);
	CMapItemView* CreateStaticObjMultiOccTrans(DWORD dwParam);
	CMapItemView* CreateAniObj(DWORD dwParam);
	CMapItemView* CreateAniObjDiscrete(DWORD dwParam);
	CMapItemView* CreateAniObjMultiOcc(DWORD dwParam);
	CMapItemView* CreateAniObjMultiOccDiscrete(DWORD dwParam);
	CMapItemView* CreateAniObjTransport(DWORD dwParam);
	CMapItemView* CreateMagic(DWORD dwParam);
	CMapItemView* CreateMonsterView(DWORD dwParam);
	CMapItemView* CreateActorView(DWORD dwParam);

	CMapItemView* CreateStaticObjSpecificBlock(DWORD dwParam);
	CMapItemView* CreateStaticObjSpecificBlockDown(DWORD dwParam);
	CMapItemView* CreateStaticObjArticle(DWORD dwParam);
	CMapItemView* CreateStaticObjBooth(DWORD dwParam);
	CMapItemView* CreateStaticObjPicture(DWORD dwParam);

};
//------------------------------------------------------------------------