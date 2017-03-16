/**
*	created:		2012-7-2   23:42
*	filename: 		CThingClass
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/CThingClass.h"
//------------------------------------------------------------------------
CThingClass::CThingClass(void)
{
	m_dwClassVal = 0;
}
//------------------------------------------------------------------------
CThingClass::~CThingClass(void)
{

}
//------------------------------------------------------------------------
DWORD CThingClass::GetClassVal(void)
{
	return m_dwClassVal;
}
//------------------------------------------------------------------------
DWORD CThingClass::GetThingClass(void)
{
	return m_dwClassVal & 0xff;	
}
//------------------------------------------------------------------------
void CThingClass::SetThingClass(DWORD dwThingClass)
{
	if(dwThingClass < 0 || dwThingClass >= 0xff)
	{
		return;
	}

	m_dwClassVal &= 0xffffff00;
	m_dwClassVal |= dwThingClass;
}
//------------------------------------------------------------------------
void CThingClass::SetCreature(void)
{
	m_dwClassVal |= THINGCLASS_FLAG_CREATURE;
}
//------------------------------------------------------------------------
void CThingClass::SetEquipment(void)
{
	m_dwClassVal |= THINGCLASS_FLAG_EQUIPMENT;
}
//------------------------------------------------------------------------
void CThingClass::SetMedicament(void)
{
	m_dwClassVal |= THINGCLASS_FLAG_MEDICAMENT;
}
//------------------------------------------------------------------------
void CThingClass::SetWarrant(void)
{
	m_dwClassVal |= THINGCLASS_FLAG_WARRANT;
}
//------------------------------------------------------------------------
void CThingClass::SetValuables(void)
{
	m_dwClassVal |= THINGCLASS_FLAG_VALUABLES;
}
//------------------------------------------------------------------------
BOOL CThingClass::IsCreature(void)
{
	return ((m_dwClassVal & THINGCLASS_FLAG_CREATURE) == THINGCLASS_FLAG_CREATURE) ? TRUE : FALSE;
}
//------------------------------------------------------------------------
BOOL CThingClass::IsEquipment(void)
{
	return ((m_dwClassVal & THINGCLASS_FLAG_EQUIPMENT) == THINGCLASS_FLAG_EQUIPMENT) ? TRUE : FALSE;
}
//------------------------------------------------------------------------
BOOL CThingClass::IsMedicament(void)
{
	return ((m_dwClassVal & THINGCLASS_FLAG_MEDICAMENT) == THINGCLASS_FLAG_MEDICAMENT) ? TRUE : FALSE;
}
//------------------------------------------------------------------------
BOOL CThingClass::IsWarrant(void)
{
	return ((m_dwClassVal & THINGCLASS_FLAG_WARRANT) == THINGCLASS_FLAG_WARRANT) ? TRUE : FALSE;
}
//------------------------------------------------------------------------
BOOL CThingClass::IsValuables(void)
{
	return ((m_dwClassVal & THINGCLASS_FLAG_VALUABLES) == THINGCLASS_FLAG_VALUABLES) ? TRUE : FALSE;
}
//------------------------------------------------------------------------