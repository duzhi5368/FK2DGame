/**
*	created:		2012-6-26   0:09
*	filename: 		IItemClassFactory
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		地图对象创建的类工厂接口
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CMapItem;
struct IArchive;
struct IResScript;
class CMapItemView;
struct ICanvas;
//------------------------------------------------------------------------
// 客户端类工厂
struct IItemClassFactory
{
	// 返回对象基本接口指针
	virtual CMapItem* CreateItem(DWORD dwClassID, IArchive* pArchive) = NULL;
	virtual CMapItem* CreateItem(DWORD dwClassID, const char* buf, int nLen) = NULL;
	virtual void Release() = NULL;

	// 得到basicsys加载的所有脚本,ppAllScp=NULL时，传回所有脚步的数目
	virtual int GetAllScript(IResScript** ppAllScp) = NULL;
	// 重新加载脚本(适合于地图编辑器，不停的更新脚本来看效果而不需要退出编辑器重进)
	virtual int ReLoadScript(int nScpTypeID) = NULL;
	// 清除缓冲，用于地图转换时
	virtual void ClearItemBuffer() = NULL;

	//以下是登陆界面显示人物的处理，为了保存接口的完整整洁，主要以插件的形式来实现
	//更换图包
	virtual BOOL ChangeModule (int nActIndex, int nIndex, DWORD dwResID, BYTE nPal = 0, BYTE nAlpha = 0) = NULL;
	virtual void OnDraw(int nActIndex, ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag) = NULL;
	virtual void ReleasePreView(int nActIndex) = NULL;					//释放PreView的图包资源
	virtual void SetState(int nActIndex, int nState) = NULL;			//设置角色状态
	virtual void CreatePreView (int nActIndex, int nType) = NULL;		//设置创建的角色类型

	virtual BOOL InitFactory () = NULL;			//初始化
};
//------------------------------------------------------------------------
// 服务器端类工厂
struct IItemClassFactoryServer
{
	virtual CMapItem* CreateItem(DWORD dwClassID, DWORD dwResID) = NULL;
	virtual void Release() = NULL;
};
//------------------------------------------------------------------------
