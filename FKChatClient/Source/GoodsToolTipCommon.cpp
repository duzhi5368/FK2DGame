/**
*	created:		2012-6-28   8:39
*	filename: 		GoodsToolTipCommon
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/GoodsToolTipCommon.h"
#include "../Include/Tool.h"
#include "../Include/ChatClient.h"
//------------------------------------------------------------------------
CGoodsToolTipCommon::CGoodsToolTipCommon()
{

}
//------------------------------------------------------------------------
CGoodsToolTipCommon::~CGoodsToolTipCommon()
{

}
//------------------------------------------------------------------------
//用一个新对象重置TOOLTIP生成器
BOOL CGoodsToolTipCommon::SetObject(DWORD dwGoodsID, UID uid)
{
	strToolTip.clear();	
	m_GoodsProp = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(dwGoodsID);	
	m_pGoods = (IGoods *)CTool::Instance()->GetThing(uid);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CGoodsToolTipCommon::SetObject(DWORD dwGoodsID, IGoods *pGoods)
{
	strToolTip.clear();	
	m_GoodsProp = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(dwGoodsID);	
	m_pGoods = pGoods;
	return TRUE;
}
//------------------------------------------------------------------------
void CGoodsToolTipCommon::AddBind(void)            //绑定
{
	if (m_pGoods == NULL)
	{
		return;
	}

	if (m_GoodsProp.lHoldBind)
	{
		if (m_pGoods->GetBindFlag(GOODS_BINDFLAG_SETHOLD))
		{
			strToolTip = strToolTip + "[:255,153,000,c]已绑定" + "\r\n";			
		}
		else
		{
			strToolTip = strToolTip + "[:255,153,000,c]拾取后绑定" + "\r\n";
		}
		return;
	}

	if (m_GoodsProp.lUseBind)
	{
		if (m_pGoods->GetBindFlag(GOODS_BINDFLAG_SETUSE))
		{
			strToolTip = strToolTip + "[:255,153,000,c]已绑定" + "\r\n";	
		}
		else
		{
			strToolTip = strToolTip + "[:255,153,000,c]使用后绑定" + "\r\n";
		}
		return;
	}	
}
//------------------------------------------------------------------------