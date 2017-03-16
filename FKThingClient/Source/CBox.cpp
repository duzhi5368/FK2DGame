/**
*	created:		2012-7-3   0:41
*	filename: 		CBox
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/CBox.h"
#include "../Include/ThingClientCommon.h"
#include "../Include/BoxPopedomMgr.h"
#include "../Include/CreatureViewWrapper.h"
#include "../Include/CThingClientFacade.h"
#include "../../FKGame/Network/IAppNetworkLink.h"
//------------------------------------------------------------------------
CBox::CBox() : m_uid(INVALID_UID), m_pMapItemView(NULL)
{
	m_ThingClass.SetThingClass(THINGCLASS_BOX);	
	ZeroMemory(&m_Property, sizeof(SBOX_PROPERTY_LIST));
}
//------------------------------------------------------------------------
CBox::~CBox()
{

}
//------------------------------------------------------------------------
BOOL CBox::Create()
{
	struct SBoxResID
	{
		DWORD	dwResIDBox;
		DWORD	dwResIDStar;
	};
	
	// 取得创建工厂	
	IItemClassFactory * pItemClassFactory = g_pThingClientFacade->GetItemFactory();
	if(pItemClassFactory == NULL)
	{
		return FALSE;
	}
	
	// 创建地图部件
	SBoxResID resid;
	resid.dwResIDBox  = m_Property.dwResID1;
	resid.dwResIDStar = m_Property.dwResID2;		
	m_pMapItemView = (CMapItemView*)pItemClassFactory->CreateItem(miStaticObj_Article, (LPCSTR)&resid, sizeof(SBoxResID));
	if(m_pMapItemView == NULL)
	{
		return FALSE;
	}

	m_pMapItemView->SetItemData((DWORD)(this));
	
	// 各种外观属性设置
	CCreatureViewWrapper viewwrapper(m_pMapItemView);
	viewwrapper.AddToMap(m_Property.ptLoc);
	viewwrapper.SetTile(m_Property.ptLoc);

	// 是否显示星星
	if(g_pGlobal == NULL || g_pGlobal->m_pPlayer == NULL)
	{
		// 盒子先创建，人物还没有创建
		g_BoxPopedomManager.CheckBox(m_uid);
	}
	else
	{
		BOOL bRight = g_BoxPopedomManager.CanOpen(m_uid, g_pGlobal->m_pPlayer->GetUID());
		SetRight(bRight);
	}
	
	// 掉落动画
	if(m_Property.dwState == BOX_CREATE)
	{
		m_pMapItemView->OnCommand(AtcCmdThrow, 0, 0);
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
void CBox::Release(void)
{
	// 开始销毁了...
	// 发一事件出去,即将删除
	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_RELEASE);
	OnMessage(wMsgID, NULL, 0);

	// 销毁里面的容器
	try
	{
		if (m_Property.dwState == BOX_OPENING
			|| m_Property.dwState == BOX_OPENED)
		{			
			g_pGlobal->m_pWndMgr->OnEvent(WND_TREASUREBOX, WE_TREASURE_CLOSE, 0, (LPARAM)&m_uid);		
			Close();
		}
		ClearBox();
	}
	catch (...) {
	}
	
	// 从ThingTable中清除
	IThingTable* pThingTable = g_pThingClientFacade->GetThingTable();
	if (pThingTable == NULL
		|| !pThingTable->Remove(m_uid))
	{
		TraceLn("从ThingTable中清除%s失败！", GetName());
	}

	// 清除MapItem
	IMapView* pMapView;
	if (m_pMapItemView != NULL
		&& (pMapView = g_pThingClientFacade->GetMapView()) != NULL)
	{
		POINTS ptLoc  = GetLoc();
		POINT  ptTile = { ptLoc.x, ptLoc.y };

		if (!pMapView->RemoveItem(ptTile, m_pMapItemView))
		{
			TraceLn("清除MapItem失败！%s[%d, %d]", GetName(), ptLoc.x, ptLoc.y);
		}

		SAFE_RELEASE(m_pMapItemView);
	}
	
	delete this;
}
//------------------------------------------------------------------------
void CBox::SetUID(UID uid)
{
	m_uid = uid;
}
//------------------------------------------------------------------------
UID	CBox::GetUID(void)
{
	return m_uid;
}
//------------------------------------------------------------------------
DWORD CBox::GetMapID(void)
{
	return GETUIDSERVERID(m_uid);
}
//------------------------------------------------------------------------
void CBox::SetLoc(POINTS ptLoc)
{
	m_Property.ptLoc = ptLoc;
}
//------------------------------------------------------------------------
POINTS CBox::GetLoc(void)
{
	return m_Property.ptLoc;
}
//------------------------------------------------------------------------
IThingClass* CBox::GetThingClass(void)
{
	return static_cast<IThingClass*>(&m_ThingClass);
}
//------------------------------------------------------------------------
// 私有对象现场管理
// 说明：
// 私有对象现场是指用户自己的角色或装备等的详细信息，其它人是看不到的
// 以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端
// 如：用户登录时，服务器取得人物对象现场
// 客户端根据dwClassID创建对象，调用OnGetPrivateContext()设置对象现场
BOOL CBox::OnSetPrivateContext(LPCSTR buf, int nLen)
{
	if (buf == NULL || nLen != sizeof(SBOX_PROPERTY_LIST))
	{
		TraceLn("CBox::OnSetPrivateContext Failed! [buf == NULL]");
		return FALSE;
	}
	
	memcpy(&m_Property, buf, nLen);	
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CBox::OnGetPrivateContext(LPSTR buf, int &nLen)
{
	// 客户端不可能这样调的
	return FALSE;
}
//------------------------------------------------------------------------
// 对象公开现场管理
// 说明：
// 公开现场为可以让所有GameWorld中的其它对象查看的对象信息，如角色名、HP、装备形状等
// 服务器取得对象现场，以dwThingClassID + uid + wDataLen + data[wDataLen] 的形式发给客户端，
// 客户端根据dwThingClassID创建对象，调用OnSetContext()设置对象现场
BOOL CBox::OnSetPublicContext(LPCSTR buf, int nLen)
{
	// 基本数据
	CDataArchive dataarchive((char*)buf, nLen);
	if (!dataarchive.Read(&m_Property, sizeof(SBOX_PROPERTY_LIST)))
	{
		return FALSE;
	}

	// 复位
	g_BoxPopedomManager.ClearBox(m_uid);

	// 得到该盒子的所有权限者
	// 个数
	int nSize = 0;
	if (!dataarchive.Read(&nSize, sizeof(int)))
	{
		return FALSE;
	}

	long ldbid;	
	for (int i = 0; i < nSize; ++i)
	{
		if (!dataarchive.Read(&ldbid, sizeof(long)))
		{
			return FALSE;
		}

		// 权限
		g_BoxPopedomManager.AddMaster(m_uid, ldbid);
	}

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CBox::OnGetPublicContext(LPSTR buf, int &nLen)
{
	// 客户端不可能这样调的
	return FALSE;
}
//------------------------------------------------------------------------
int CBox::OnCommand(DWORD dwCommand, LPCSTR buf, int nLen)
{
	return 0;
}
//------------------------------------------------------------------------
// 消息通知, 驱使逻辑对像进行运转
void CBox::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
{
	// 事件服务器
	IEventServer* pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return;
	}

	// 先判定否决是否通过
	if(pEventServer->FireVote((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_BOX, (DWORD)this) == FALSE)
	{
		return;
	}

	// 然后行为事件
	pEventServer->FireAction((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_BOX, (DWORD)this);
}
//------------------------------------------------------------------------
// 描  述：触发响应（FireResponse）
void CBox::OnResponse(DWORD dwMsg, LPCSTR buf, int nLen)
{
	// 事件服务器	
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return;
	}

	// 触发响应
	pEventServer->FireResponse((BYTE *)buf, nLen, dwMsg, SRC_TYPE_ID_BOX, (DWORD)this);
}
//------------------------------------------------------------------------
// 描  述: 订阅本对象的否决事件
// 参  数: [输入] DWORD dwMsg - 事件ID
// 参  数: [输入] IEventVoteListener * pListener - 事件否决者
// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CBox::SubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_BOX, (DWORD)this, szDescription);
}
//------------------------------------------------------------------------
// 描  述：取消订阅本对象的否决事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventVoteListener * pListener  - 事件否决者	
// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CBox::UnSubscibeMessage(DWORD dwMsg, IEventVoteListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_BOX, (DWORD)this);
}
//------------------------------------------------------------------------
// 描  述: 订阅本对象的行为事件
// 参  数: [输入] DWORD dwMsg - 事件ID
// 参  数: [输入] IEventActionListener * pListener - 事件行为执行者
// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CBox::SubscibeMessage(DWORD dwMsg, IEventActionListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_BOX, (DWORD)this, szDescription);
}
//------------------------------------------------------------------------
// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventActionListener * pListener  - 事件行为执行者
// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CBox::UnSubscibeMessage(DWORD dwMsg, IEventActionListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_BOX, (DWORD)this);	
}
//------------------------------------------------------------------------
// 描  述: 订阅本对象的响应事件
// 参  数: [输入] DWORD dwMsg - 事件ID
// 参  数: [输入] IEventResponseListener * pListener - 事件响应者
// 参  数: [输入] const char szDescription[32]  - 订阅者描述信息,可以就填订阅者的类名, 已便调试
// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
BOOL CBox::SubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener, const char szDescription[32])
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	return pEventServer->AddListener(pListener, dwMsg, SRC_TYPE_ID_BOX, (DWORD)this, szDescription);
}
//------------------------------------------------------------------------
// 描  述：取消订阅本对象的行为事件,不会delete pListener指针
// 参  数：[输入] DWORD dwMsg  - 事件ID
// 参  数：[输入] IEventResponseListener * pListener  - 事件响应者
// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE退订消息
BOOL CBox::UnSubscibeMessage(DWORD dwMsg, IEventResponseListener * pListener)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}
	
	return pEventServer->RemoveListener(pListener, dwMsg, SRC_TYPE_ID_BOX, (DWORD)this);	
}
//------------------------------------------------------------------------
void CBox::Accept(IMapItemVisitor &visitor)
{
	visitor.VisitBox();
}
//------------------------------------------------------------------------
CMapItemView* CBox::GetMapItem() const
{
	return m_pMapItemView;
}
//------------------------------------------------------------------------
void CBox::SetMapItem(CMapItemView * pMapItemView)
{
	m_pMapItemView = pMapItemView;
}
//------------------------------------------------------------------------
LPCSTR CBox::GetName() const
{
	return m_Property.szName;
}
//------------------------------------------------------------------------
BOOL CBox::Open()
{
	// 向服务器发送打开命令
	char buf[256];
	int nLen = sizeof(buf);
	CDataArchive archive(buf, nLen);
	CS_ACTION_HEAD actionhead;
	
	actionhead.m_wKeyRoot = CMD_ROOT_THING;
	actionhead.m_wKeyMain = ROOTTHING_CS_MAIN_ACTION;
	actionhead.m_wKeySub  = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_OPENBOX);
	
	archive.Write(&actionhead, sizeof(CS_ACTION_HEAD));
	archive.Write(&m_uid, sizeof(UID));

	DWORD dwState = BOX_OPENING;
	archive.Write(&dwState, sizeof(DWORD));	
	
	// 发送
	g_pGlobal->m_pAppNetworkLink->Send(buf, archive.GetBufferOffset());
	return TRUE;	
}
//------------------------------------------------------------------------
void CBox::Close()
{
	// 向服务器发送打开命令
	char buf[256];
	int nLen = sizeof(buf);
	CDataArchive archive(buf, nLen);
	CS_ACTION_HEAD actionhead;
	
	actionhead.m_wKeyRoot = CMD_ROOT_THING;
	actionhead.m_wKeyMain = ROOTTHING_CS_MAIN_ACTION;
	actionhead.m_wKeySub  = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_OPENBOX);
	
	archive.Write(&actionhead, sizeof(CS_ACTION_HEAD));
	archive.Write(&m_uid, sizeof(UID));

	DWORD dwState = BOX_CLOSED;
	archive.Write(&dwState, sizeof(DWORD));
	
	// 发送
	g_pGlobal->m_pAppNetworkLink->Send(buf, archive.GetBufferOffset());	
}
//------------------------------------------------------------------------
void CBox::Respond(LPCSTR pBuf, int nLen)
{
	// 正在打开状态+打开时间
	// 打开状态+容器的个数+容器1id+容器1大小+容器2id+容器2大小+...
	// 关闭状态	
	if (pBuf == NULL || nLen < sizeof(DWORD))
	{
		return;
	}

	// 状态(记录的服务器的状态)
	DWORD dwState = *(DWORD*)pBuf;
	pBuf += sizeof(DWORD);
	nLen -= sizeof(DWORD);
	
	switch (dwState)
	{
	case BOX_OPENING:
		{
			// 准备打开
			m_Property.dwState = BOX_OPENING;
			DWORD dwOpenTime = *(DWORD*)pBuf;
			PrepareOpen(dwOpenTime);
			return;
		}
		
	case BOX_OPENED:
		// 打开
		m_Property.dwState = BOX_OPENED;
		OpenBox(pBuf, nLen);
		return;
		
	case BOX_CLOSED:
		// 关闭
		m_Property.dwState = BOX_CLOSED;
		CloseBox();
		return;
		
	case BOX_COMMAND:
		Command(pBuf, nLen);
		return;
		
	default:
		return;
	}
}
//------------------------------------------------------------------------
DWORD CBox::GetType() const
{
	return m_Property.dwTypeId;
}
//------------------------------------------------------------------------
DWORD CBox::GetID() const
{
	return m_Property.dwBoxId;
}
//------------------------------------------------------------------------
void CBox::PrepareOpen(DWORD dwOpenTime)
{
	// 发送一个否决消息
	IEventServer* pEventServer = g_pThingClientFacade->GetEventServer();
	if (pEventServer == NULL)
	{
		return;
	}
	
	SBOX_CONTEXT context;
	context.uidBox = m_uid;
	context.dwState = BOX_OPENING;
	
	WORD wEventID = MAKE_MSGID(MAINACTION_CIRCULTYPE_SS, MAINACTION_MSGID_OPENBOX);	
	if (!pEventServer->FireVote((BYTE*)&context, sizeof(SBOX_CONTEXT), wEventID, SRC_TYPE_ID_ACTOR, (DWORD)g_pGlobal->m_pPlayer))
	{
		return;
	}
	
	// 调用界面显示
	g_pGlobal->m_pWndMgr->OnEvent(WND_TREASUREBOX, WE_TREASURE_PROGRESSBAR_SHOW, dwOpenTime, (LPARAM)&m_uid);
}
//------------------------------------------------------------------------
void CBox::OpenBox(LPCSTR pBuf, int nLen)
{
	/*
	if (pBuf == NULL || nLen < sizeof(int))
	{
		return;
	}
	
	// 发送一个否决消息
	IEventServer* pEventServer = g_pThingClientFacade->GetEventServer();
	if (pEventServer == NULL)
	{
		return;
	}
	
	SBOX_CONTEXT context;
	context.uidBox = m_uid;
	context.dwState = BOX_OPENED;
	
	WORD wEventID = MAKE_MSGID(MAINACTION_CIRCULTYPE_SS, MAINACTION_MSGID_OPENBOX);	
	if (!pEventServer->FireVote((BYTE*)&context, sizeof(SBOX_CONTEXT), wEventID, SRC_TYPE_ID_ACTOR, (DWORD)g_pGlobal->m_pPlayer))
	{
		return;
	}

	// 将之前的清除
	m_objects.clear();

	// 容器的个数+容器1id+容器1大小+容器2id+容器2大小+...
	int nContainer = *(int*)pBuf;
	if (nContainer <= 0)
	{
		return;
	}

	pBuf += sizeof(int);
	nLen -= sizeof(int);

	// 得到容器管理器
	IContainerMgr* pContainerMgr = g_pGlobal->GetContainerMgr();
	if (pContainerMgr == NULL)
	{
		return;
	}

	// 要打开的容器
	IContainer* pCurContainer = NULL;
	IContainer* pContainer = NULL;

	for (int i = 0; i < nContainer; ++i)
	{
		// 容器id
		DWORD dwContainerId = *(int*)pBuf;
		pBuf += sizeof(DWORD);
		nLen -= sizeof(DWORD);

		// 容器大小
		int nSize = *(int*)pBuf;
		pBuf += sizeof(int);
		nLen -= sizeof(int);
		
		// 创建容器
		pContainer = pContainerMgr->GetContainer(dwContainerId);
		if (pContainer == NULL)
		{
			SContainerCreateContext context;
			context.dwID = dwContainerId;
			context.nMaxSize = nSize;
			context.dwType = GOODS_TYPE_REALY;
			context.bCanAdd = FALSE;

			pContainer = pContainerMgr->CreateContainer((LPCSTR)&context, sizeof(SContainerCreateContext));
			if (pContainer == NULL)
			{
				continue;
			}
			PrintEventLn("创建容器 = %d", pContainer->GetID());
		}

		if (pCurContainer == NULL)
		{
			pCurContainer = pContainer;
		}

		// 容器加入盒子
		m_objects.push_back(dwContainerId);
	}

	// 打开
	if (pCurContainer != NULL)
	{
		pCurContainer->Open();

		// 调用界面显示
		g_pGlobal->m_pWndMgr->OnEvent(WND_TREASUREBOX, WE_TREASURE_OPEN, pCurContainer->GetID(), (LPARAM)&m_uid);
		
		// 发送一个响应消息
		pEventServer->FireResponse((BYTE*)&context, sizeof(SBOX_CONTEXT), wEventID, SRC_TYPE_ID_ACTOR, (DWORD)g_pGlobal->m_pPlayer);
	}

	*/
}
//------------------------------------------------------------------------
void CBox::CloseBox()
{
	g_pGlobal->m_pWndMgr->OnEvent(WND_TREASUREBOX, WE_TREASURE_CLOSE, 0, (LPARAM)&m_uid);

	vector<DWORD>::iterator it = m_objects.begin();
	if (it == m_objects.end())
	{
		return;
	}
/*
	IContainerMgr* pContainerMgr = g_pThingClientFacade->GetContainerMgr();
	if (pContainerMgr == NULL)
	{
		return;
	}

	IContainer* pContainer = pContainerMgr->GetContainer(*it);
	if (pContainer == NULL)
	{
		return;
	}

	if (   pContainer->GetGoodsNumber() == 0
		|| m_Property.dwTypeId == BOX_TYPE_MINE
		|| m_Property.dwTypeId == BOX_TYPE_MEDICINE)
	{
		m_objects.erase(it);
		pContainer->Release();
	}
	*/
}
//------------------------------------------------------------------------
void CBox::ClearBox()
{
	/*
	IContainerMgr* pContainerMgr = g_pThingClientFacade->GetContainerMgr();
	if (pContainerMgr == NULL)
	{
		return;
	}
	
	for (vector<DWORD>::iterator it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		// 容器 id
		DWORD dwContainerId = *it;
		
		// 得到容器
		IContainer* pContainer = pContainerMgr->GetContainer(dwContainerId);
		if (pContainer == NULL)
		{
			continue;
		}
		
		for (int i = 0; i < pContainer->GetSize(); ++i)
		{
			IContainerGoods* pContainerGoods = pContainer->GetGoods(i);
			if (pContainerGoods == NULL)
			{
				continue;
			}
			
			UID uidGoods = pContainerGoods->GetUID();
			IGoods* pGoods = (IGoods*)g_pThingClientFacade->GetThing(uidGoods);
			SAFE_RELEASE(pGoods);
		}
		
		pContainer->Release();
	}

	m_objects.clear();
	*/
}
//------------------------------------------------------------------------
void CBox::SetRight(BOOL bRight)
{
	CCreatureViewWrapper viewwrapper(m_pMapItemView);
	m_pMapItemView->OnCommand(AtcCmdSetProperty, AtcPropShowStar, (DWORD)&bRight);
	if (bRight)
	{
		viewwrapper.AddFlag(CMapItemView::flagCanbeSelected);		
	}
	else
	{
		viewwrapper.RemoveFlag(CMapItemView::flagCanbeSelected);
	}
}
//------------------------------------------------------------------------
BOOL CBox::IsCanOpen(void)
{
	return g_BoxPopedomManager.CanOpen(m_uid, g_pGlobal->m_pPlayer->GetUID());
}
//------------------------------------------------------------------------
void CBox::Command(LPCSTR pBuf, int nLen)
{
	if (nLen < sizeof(DWORD))
	{
		return;
	}
	
	DWORD dwCmd = *(DWORD*)pBuf;
	pBuf += sizeof(DWORD);
	nLen -= sizeof(DWORD);

	switch(dwCmd)
	{
	case BOX_COMMAND_CLEARRIGHT:
		// 权限时间到，服务器开放权限
		g_BoxPopedomManager.ClearBox(m_uid);
		SetRight(TRUE);
		break;

	case BOX_COMMAND_ADDRIGHT:
		g_BoxPopedomManager.AddMaster(m_uid, g_pGlobal->m_pPlayer->GetActorDBID());
		SetRight(TRUE);
		break;

	case BOX_COMMAND_DELRIGHT:
		g_BoxPopedomManager.DelMaster(m_uid, g_pGlobal->m_pPlayer->GetActorDBID());
		SetRight(FALSE);
		break;
	}
}
//------------------------------------------------------------------------