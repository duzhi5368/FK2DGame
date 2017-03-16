/**
*	created:		2012-6-28   10:27
*	filename: 		ToopTipBuilder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/ToolTipBuilder.h"
#include "../Include/ChatClient.h"
//------------------------------------------------------------------------
CToolTipBuilder *CToolTipBuilder::pBuilder = NULL;
//------------------------------------------------------------------------
CToolTipBuilder::CToolTipBuilder()
{
	m_pGenerator = &m_DefaultGenerator; 	
}
//------------------------------------------------------------------------
CToolTipBuilder::~CToolTipBuilder()
{

}
//------------------------------------------------------------------------
BOOL CToolTipBuilder::Init()
{
	SAFE_DELETE(pBuilder);
	pBuilder = new CToolTipBuilder;
	return TRUE;
}
//------------------------------------------------------------------------
void CToolTipBuilder::Release()
{
	SAFE_DELETE(pBuilder) ;
}
//------------------------------------------------------------------------
CToolTipBuilder * CToolTipBuilder::Instance()
{
	if( pBuilder == NULL)
	{
		Init();
	}
	return pBuilder;
}
//------------------------------------------------------------------------
//用一个新对象重置TOOLTIP生成器
BOOL CToolTipBuilder::SetObject(DWORD dwGoodsID, UID uid)
{
	if (g_pGlobal->m_pDataCenter == NULL)
	{
		return FALSE;			
	}

	GOODS_BASEPROPERTY prop = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(dwGoodsID);	
	switch(prop.lClass) {
	case GOODS_CLASS_EQUIPMENT:   //1 为装备 
		m_pGenerator = &m_EquipGenerator;
		m_EquipGenerator.SetObject(dwGoodsID, uid);
		break;

	case GOODS_CLASS_MEDICAMENT:   //
	case GOODS_CLASS_WARRANT:
		m_pGenerator = &m_GoodsGenerator;
		m_GoodsGenerator.SetObject(dwGoodsID, uid);
		break;
	default:
		m_pGenerator = &m_DefaultGenerator; 
		break;
	}

	return TRUE;
}
//------------------------------------------------------------------------
//用一个新对象重置TOOLTIP生成器
BOOL CToolTipBuilder::SetObject(DWORD dwGoodsID, IGoods *pGoods)
{
	if( pGoods == NULL)
	{
		return FALSE;
	}
	GOODS_BASEPROPERTY prop = g_pGlobal->m_pDataCenter->GetGoodsBaseProperty(dwGoodsID);	
	switch(prop.lClass) {
	case GOODS_CLASS_EQUIPMENT:   //1 为装备 
		m_pGenerator = &m_EquipGenerator;
		m_EquipGenerator.SetObject(dwGoodsID, pGoods);
		break;

	case GOODS_CLASS_MEDICAMENT:   //
	case GOODS_CLASS_WARRANT:
		&m_GoodsGenerator;
		m_pGenerator = &m_GoodsGenerator;
		m_GoodsGenerator.SetObject(dwGoodsID, pGoods);
		break;
	default:
		m_pGenerator = &m_DefaultGenerator; 
		break;
	}
	return TRUE;	
}
//------------------------------------------------------------------------
//用一个新对象重置TOOLTIP生成器
BOOL CToolTipBuilder::SetObject(int nSkillID)
{
	m_pGenerator = &m_SkillGenerator;
	m_SkillGenerator.SetObject(nSkillID);
	return TRUE;
}
//------------------------------------------------------------------------
void CToolTipBuilder::AddItem(int nItemID)
{
	m_pGenerator->AddItem(nItemID);
}
//------------------------------------------------------------------------
LPCSTR CToolTipBuilder::GetToolTip(void)
{
	return m_pGenerator->GetToolTip();
}
//------------------------------------------------------------------------