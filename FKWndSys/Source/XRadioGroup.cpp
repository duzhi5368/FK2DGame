/**
*	created:		2012-6-24   0:08
*	filename: 		XRadioGroup
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
XRadioGroup::XRadioGroup()
{
	m_lstRadios.clear();
}
//------------------------------------------------------------------------
XRadioGroup::~XRadioGroup()
{
}
//------------------------------------------------------------------------
void XRadioGroup::Release()
{
	delete this;
}
//------------------------------------------------------------------------
XRadio* XRadioGroup::GetSelectedRadio()
{
	list<XRadio*>::iterator it;
	for (it = m_lstRadios.begin(); it != m_lstRadios.end(); it++)
		if ((*it)->GetState() == XRadio::RS_CHECK)
			return (*it);
	return NULL;
}
//------------------------------------------------------------------------
void XRadioGroup::SelectRadio(XRadio* pRadio)
{
	list<XRadio*>::iterator it;
	int nIndex = 0;
	int nIdxChecked = 0;
	for (it = m_lstRadios.begin(); it != m_lstRadios.end(); it++,nIndex++)
	{
		if (*it == pRadio)
			nIdxChecked = nIndex;

		if ((*it)->GetState() == XRadio::RS_CHECK)
		{
			if ((*it) != pRadio) // 将先前的那个 uncheck
			{
				(*it)->SetState(XRadio::RS_UNCHECK);
				(*it)->InvalidRect();
			}
		}
	}

	pRadio->SetState(XRadio::RS_CHECK);
	pRadio->InvalidRect();

	s_indexChecked((DWORD)&nIdxChecked);
}
//------------------------------------------------------------------------
int XRadioGroup::GetIndexOfRadio(XRadio* pRadio)
{
    int ret = 0;
    bool found = false;
    list<XRadio*>::const_iterator it;
    for (it=m_lstRadios.begin(); it!=m_lstRadios.end(); it++)
    {
        if ((*it) == pRadio)
        {
            found = true;
            break;
        }
        ++ret;
    }
    if(!found)
	{
		return -1;
	}

    return ret;
}
//------------------------------------------------------------------------
int XRadioGroup::AddRadio(XRadio* pRadio)
{
	m_lstRadios.push_back(pRadio);
	return m_lstRadios.size();
}
//------------------------------------------------------------------------
void XRadioGroup::RemoveRadio(XRadio* pRadio)
{
	list<XRadio*>::iterator it;
	for (it=m_lstRadios.begin(); it!=m_lstRadios.end(); it++)
	{
        if ((*it) == pRadio)
		{
			m_lstRadios.erase(it);
			break;
		}
	}

	if (m_lstRadios.empty())
		Release();
}
//------------------------------------------------------------------------