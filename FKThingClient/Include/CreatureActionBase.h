/**
*	created:		2012-7-3   23:49
*	filename: 		CreatureActionBase
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		生物基本行为,响应各种行为事件的发生
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/Event/IEvent.h"
//------------------------------------------------------------------------
class IEvent;
class IEventActionListener;
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_BORN)
class CCreatureAction_Creature_Born_CC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);

	//////////////////////////////////////////////////////////
	/******* CCreatureAction_Creature_Born_CC ***************/
private:	
	void		 OnProcState(ICreature * pCreature, char * pContext, int nLen);
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_MOVE)
class CCreatureAction_Actor_Move_CC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);

	//////////////////////////////////////////////////////////
	/*************** CCreatureAction_Move *******************/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_PRE_ATTACK)
class CCreatureAction_Actor_PrepAttack_CC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);

	//////////////////////////////////////////////////////////
	/*************** CCreatureAction_PrepAttack_CC **********/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_ATTACK)
class CCreatureAction_Actor_Attack_CC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);

	//////////////////////////////////////////////////////////
	/*************** CCreatureAction_Attack_CC **************/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_STOP_ATTACK);
class CCreatureAction_Actor_StopAttack_CC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);

	//////////////////////////////////////////////////////////
	/****  CCreatureAction_StopAttack_CC ********************/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_STOP_PRE_ATTACK);
class CCreatureAction_Actor_StopPreAttack_CC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);

	//////////////////////////////////////////////////////////
	/****  CCreatureAction_StopPreAttack_CC *****************/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_CHANGEMOVESTYLE)
class CCreatureAction_Actor_ChangeMoveStyle_CC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);

	//////////////////////////////////////////////////////////
	/********* CCreatureAction_ChangeMoveStyle_CC ***********/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_DIE)
class CCreatureAction_Actor_Die_CC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);

	//////////////////////////////////////////////////////////
	/********* CCreatureAction_Actor_Die_CC *****************/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_DIE)
class CCreatureAction_Monster_Die_CC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);

	//////////////////////////////////////////////////////////
	/********* CCreatureAction_Monster_Die_CC *****************/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_SYSTEMMESSAGE);
class CCreatureAction_Actor_SystemMessage_CC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);

	//////////////////////////////////////////////////////////
	/******* CCreatureAction_Actor_SystemMessage_CC *********/
};
//------------------------------------------------------------------------