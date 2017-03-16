/**
*	created:		2012-6-23   23:17
*	filename: 		SigSlot
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
SlotHolder::~SlotHolder()
{
	disconnectAll();
}
//------------------------------------------------------------------------
void SlotHolder::connectTo(BasicSignal* sender)
{
	_senders.insert(sender);
}
//------------------------------------------------------------------------
void SlotHolder::disconnectFrom(BasicSignal* sender)
{
	_senders.erase(sender);
}
//------------------------------------------------------------------------
void SlotHolder::disconnectAll()
{
	const_iterator it = _senders.begin();
	const_iterator itEnd = _senders.end();
	for (; it!=itEnd; ++it)
		(*it)->disconnectSlot(this);
	_senders.erase(_senders.begin(), _senders.end());
}
//------------------------------------------------------------------------
Signal0::~Signal0()
{
	disconnectAll();
}
//------------------------------------------------------------------------
void Signal0::disconnectAll()
{
	ConnectionList::const_iterator it = _connectedSlots.begin();
	ConnectionList::const_iterator itEnd = _connectedSlots.end();
	for (; it!=itEnd; ++it)
	{
		(*it)->getSlotHolder()->disconnectFrom(this);
		(*it)->release();
	}
	_connectedSlots.erase(_connectedSlots.begin(), _connectedSlots.end());
}
//------------------------------------------------------------------------
void Signal0::disconnectSlot(SlotHolder* slot)
{
	ConnectionList::iterator it = _connectedSlots.begin();
	ConnectionList::iterator itEnd = _connectedSlots.end();
	int nSize = _connectedSlots.size();

	while(it != itEnd)
	{
		ConnectionList::iterator itNext = it;
		++itNext;

		if ((*it)->getSlotHolder() == slot)
		{
			(*it)->release();
			_connectedSlots.erase(it);
		}

		it = itNext;
	}
}
//------------------------------------------------------------------------
void Signal0::connect(BasicConnection0* conn)
{
	_connectedSlots.push_back(conn);
	conn->getSlotHolder()->connectTo(this);
}
//------------------------------------------------------------------------
void Signal0::emit()
{
	ConnectionList::const_iterator it = _connectedSlots.begin();
	ConnectionList::const_iterator itEnd = _connectedSlots.end();
	for (; it!=itEnd; ++it)
		(*it)->trigger();
}
//------------------------------------------------------------------------
void Signal0::operator()()
{
	emit();
}
//------------------------------------------------------------------------
Signal1::~Signal1()
{
	disconnectAll();
}
//------------------------------------------------------------------------
void Signal1::disconnectAll()
{
	ConnectionList::const_iterator it = _connectedSlots.begin();
	ConnectionList::const_iterator itEnd = _connectedSlots.end();
	for (; it!=itEnd; ++it)
	{
		(*it)->getSlotHolder()->disconnectFrom(this);
		(*it)->release();
	}

	_connectedSlots.erase(_connectedSlots.begin(), _connectedSlots.end());
}
//------------------------------------------------------------------------
void Signal1::disconnectSlot(SlotHolder* slot)
{
	ConnectionList::iterator it = _connectedSlots.begin();
	ConnectionList::iterator itEnd = _connectedSlots.end();
	while (it!=itEnd)
	{
		ConnectionList::iterator itNext = it;
		++itNext;

		if ((*it)->getSlotHolder() == slot)
		{
			(*it)->release();
			_connectedSlots.erase(it);
		}

		it = itNext;
	}
}
//------------------------------------------------------------------------
void Signal1::connect(BasicConnection1* conn)
{
	_connectedSlots.push_back(conn);
	conn->getSlotHolder()->connectTo(this);
}
//------------------------------------------------------------------------
void Signal1::emit(DWORD dwParam)
{
	ConnectionList::const_iterator it = _connectedSlots.begin();
	ConnectionList::const_iterator itEnd = _connectedSlots.end();
	for (; it!=itEnd; ++it)
		(*it)->trigger(dwParam);
}
//------------------------------------------------------------------------
void Signal1::operator()(DWORD dwParam)
{
	emit(dwParam);
}
//------------------------------------------------------------------------