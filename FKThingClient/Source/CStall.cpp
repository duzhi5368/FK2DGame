/**
*	created:		2012-7-3   23:22
*	filename: 		CStall
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		摊位
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/CStall.h"
#include "../Include/CThingClientFacade.h"
#include "../Include/CreatureViewWrapper.h"
//------------------------------------------------------------------------
IBitmap * CStall::m_pSloganBmpRes = NULL;
//------------------------------------------------------------------------
// 招牌图片名字
#define STALLSLOGAN_RES_FILENAME "StallSlogan.bmp"
//------------------------------------------------------------------------
CStall::CStall(void)
{
	// 普通属性初始化
	m_uid = INVALID_UID;
	m_pItemView = NULL;
	memset(&m_Property, 0, sizeof(m_Property));

	// 逻辑对像类型初始化
	m_ThingClass.SetThingClass(THINGCLASS_STALL);
}
//------------------------------------------------------------------------
CStall::~CStall(void)
{

}
//------------------------------------------------------------------------
void CStall::Release(void)
{
	// 发一事件出去,即将删除
	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_RELEASE);
	OnMessage(wMsgID, NULL, 0);

	if(g_pThingClientFacade->GetThingTable() == NULL)
	{
		delete this;
		return;
	}

	if(g_pThingClientFacade->GetMapView() == NULL)
	{
		delete this;
		return;
	}
		
	// 从ThingTable中清除
	if(!g_pThingClientFacade->GetThingTable()->Remove(m_uid))
	{
		TraceLn("从ThingTable中清除失败！摊位！");
	}

	// 清除MapItem
	if(m_pItemView != NULL)
	{
		POINTS _ptLoc = GetLoc();
		POINT  ptLoc = { _ptLoc.x, _ptLoc.y };
		if(!g_pThingClientFacade->GetMapView()->RemoveItem(ptLoc, m_pItemView))
		{
			TraceLn("清除MapItem失败！摊位！[%d, %d]", ptLoc.x, ptLoc.y);
		}

		if(m_pSloganBmp != NULL)
		{
			CImageLoader::GetInstance()->FreeImage(m_pSloganBmp);
			m_pSloganBmp = NULL;
		}

		// ????因为basicsys未用FCL，无法释放
		SAFE_RELEASE(m_pItemView);
	}

	delete this;
}
//------------------------------------------------------------------------
BOOL CStall::Create(void)
{	
	IItemClassFactory * pItemClassFactory = g_pThingClientFacade->GetItemFactory();
	if(pItemClassFactory == NULL)
	{
		return FALSE;
	}
	
	// 创建地图部件
	m_pItemView = (CMapItemView *)pItemClassFactory->CreateItem(miStaticObj_Booth, (LPCSTR)&m_Property.dwResID, sizeof(DWORD));
	if(m_pItemView == NULL)
	{
		TraceLn("Error! CStall::Create() : Create MapItemView Failed!");
		return FALSE;
	}
	m_pItemView->SetItemData((DWORD)(this));

	// 招牌
	if(m_pSloganBmpRes == NULL)
	{
		m_pSloganBmpRes = static_cast<IBitmap *>(CImageLoader::GetInstance()->LoadImage(STALLSLOGAN_RES_FILENAME));
		if(m_pSloganBmpRes == NULL)
		{
			return FALSE;
		}
	}
	SetSloganBmp();	
	
	// 参数设定
	m_pItemView->OnCommand(AtcCmdSetProperty, AtcPropBkImage, (DWORD)&m_pSloganBmp);
	m_pItemView->OnCommand(AtcCmdSetProperty, AtcPropName, (DWORD)&m_Property.szName[0]);
	WPixel clr = WPixel(0, 255, 0);
	m_pItemView->OnCommand(AtcCmdSetProperty, AtcPropNameColor, (DWORD)&clr);
	m_pItemView->AddFlag(CMapItemView::flagName);	
	m_pItemView->AddFlag(CMapItemView::flagCanbeSelected);	

	// 添加入地图
	POINT ptTile = { m_Property.ptLoc.x, m_Property.ptLoc.y };
	g_pThingClientFacade->GetMapView()->AddItem(ptTile, m_pItemView);	

	return TRUE;
}
//------------------------------------------------------------------------
void CStall::SetUID(UID uid)
{
	m_uid = uid;
}
//------------------------------------------------------------------------
UID	CStall::GetUID(void)
{
	return m_uid;
}
//------------------------------------------------------------------------
DWORD CStall::GetMapID(void)
{
	return GETUIDSERVERID(m_uid);
}
//------------------------------------------------------------------------
void CStall::SetLoc(POINTS ptLoc)
{
	m_Property.ptLoc = ptLoc;
}
//------------------------------------------------------------------------
POINTS CStall::GetLoc(void)
{
	POINTS ptLoc = { 0, 0 };
	if(m_pItemView != NULL)
	{
		POINT _ptLoc = m_pItemView->GetTile();
		ptLoc.x = _ptLoc.x;
		ptLoc.y = _ptLoc.y;
	}

	return ptLoc;
}
//------------------------------------------------------------------------
IThingClass * CStall::GetThingClass(void)
{
	return static_cast<IThingClass *>(&m_ThingClass);
}
//------------------------------------------------------------------------
//// 私有对象现场管理
//// 说明：
//// 私有对象现场是指用户自己的角色或装备等的详细信息，其它人是看不到的
//// 以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端
//// 如：用户登录时，服务器取得人物对象现场
//// 客户端根据dwClassID创建对象，调用OnGetPrivateContext()设置对象现场
BOOL CStall::OnSetPrivateContext(LPCSTR buf, int nLen)
{
	if(buf == NULL || nLen != sizeof(SSTALL_PROPERTY_LIST))
	{
		TraceLn("CStall::OnSetPrivateContext Failed! [buf == NULL]");
		return FALSE;
	}

	memcpy(&m_Property, buf, sizeof(SSTALL_PROPERTY_LIST));

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CStall::OnGetPrivateContext(LPSTR buf, int &nLen)
{
	// 客户端不可能这样调的
	return FALSE;
}
//------------------------------------------------------------------------
//// 对象公开现场管理
//// 说明：
//// 公开现场为可以让所有GameWorld中的其它对象查看的对象信息，如角色名、HP、装备形状等
//// 服务器取得对象现场，以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端，
//// 客户端根据dwThingClassID创建对象，调用OnSetContext()设置对象现场
BOOL CStall::OnSetPublicContext(LPCSTR buf, int nLen)
{
	return OnSetPrivateContext(buf, nLen);
}
//------------------------------------------------------------------------
BOOL CStall::OnGetPublicContext(LPSTR buf, int &nLen)
{	
	// 客户端不可能这样调的
	return FALSE;
}
//------------------------------------------------------------------------
//// 管理thing的数据库现场
//// 说明：
//// OnGetDBContext() 用于将对象现场保护到数据库
//// OnSetDBContext() 用于通过数据库返回的数据创建对象
BOOL CStall::OnSetDBContext(LPCSTR buf, int nLen)
{	
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CStall::OnGetDBContext(LPSTR buf, int &nLen)
{
	return FALSE;
}
//------------------------------------------------------------------------
int CStall::OnCommand(DWORD dwCommand, LPCSTR buf, int nLen)
{
	return 0;
}
//------------------------------------------------------------------------
//// 消息通知, 驱使逻辑对像进行运转
void CStall::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return;
	}

	/*
	// 先判定否决是否通过
	if(!pEventServer->FireVote((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_STALL, (DWORD)this))
	{
		return;
	}

	// 然后行为事件
	pEventServer->FireAction((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_STALL, (DWORD)this);
	*/
}
//------------------------------------------------------------------------
//// 描  述：触发响应
void CStall::OnResponse(DWORD dwMsg, LPCSTR buf, int nLen)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return;
	}

	/*
	// 触发响应
	pEventServer->FireResponse((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_STALL, (DWORD)this);
	*/
}
//------------------------------------------------------------------------
//// 描  述: 订阅本对象的否决事件
//// 参  数: [输入] DWORD dwMsg - 事件ID
//// 参  数: [输入] IEventVoteListener * pListener - 事件否决者
//// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
//// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CStall::SubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_STALL, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
//// 描  述：取消订阅本对象的否决事件,不会delete pListener指针
//// 参  数：[输入] DWORD dwMsg  - 事件ID
//// 参  数：[输入] IEventVoteListener * pListener  - 事件否决者	
//// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CStall::UnSubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_STALL, (DWORD)this);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
//// 描  述: 订阅本对象的行为事件
//// 参  数: [输入] DWORD dwMsg - 事件ID
//// 参  数: [输入] IEventActionListener * pListener - 事件行为执行者
//// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
//// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CStall::SubscibeMessage(DWORD dwMsg, IEventActionListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_STALL, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
//// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
//// 参  数：[输入] DWORD dwMsg  - 事件ID
//// 参  数：[输入] IEventActionListener * pListener  - 事件行为执行者
//// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CStall::UnSubscibeMessage(DWORD dwMsg, IEventActionListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_STALL, (DWORD)this);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
//// 描  述: 订阅本对象的响应事件
//// 参  数: [输入] DWORD dwMsg - 事件ID
//// 参  数: [输入] IEventResponseListener * pListener - 事件响应者
//// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
//// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE	
BOOL CStall::SubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_STALL, (DWORD)this, szDescription);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
//// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
//// 参  数：[输入] DWORD dwMsg  - 事件ID
//// 参  数：[输入] IEventResponseListener * pListener  - 事件响应者
//// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CStall::UnSubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	/*
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_STALL, (DWORD)this);
	*/
	return TRUE;
}
//------------------------------------------------------------------------
void CStall::Accept(IMapItemVisitor &visitor)
{
	visitor.VisitStall();
}
//------------------------------------------------------------------------
CMapItemView * CStall::GetMapItem(void) const
{
	return m_pItemView;
}
//------------------------------------------------------------------------
void CStall::SetMapItem(CMapItemView * pMapItemView)
{
	m_pItemView = pMapItemView;
}
//------------------------------------------------------------------------
void CStall::SetSloganBmp(void)
{
	DWORD w = g_pGlobal->m_pDefFontWithOutline->GetLength(m_Property.szName) + 8;
	DWORD h = m_pSloganBmpRes->GetHeight();

	if(!g_pGlobal->m_CanvasHelper.CreateIBitmap(&m_pSloganBmp))
	{
		m_pSloganBmp = NULL;
		return;
	}
	m_pSloganBmp->Create(w, h);
	
	m_pSloganBmp->Draw(0, 0, 4, h, m_pSloganBmpRes, 0, 0);
	int len = 4;
	while(len < w)
	{
		m_pSloganBmp->Draw(len, 0, 13, h, m_pSloganBmpRes, 4, 0);
		len += 13;
	}
	m_pSloganBmp->Draw(w - 4, 0, 4, h, m_pSloganBmpRes, m_pSloganBmpRes->GetWidth()-4, 0);
}
//------------------------------------------------------------------------
UID CStall::GetMaster(void)
{
	return m_Property.uidMaster;
}
//------------------------------------------------------------------------