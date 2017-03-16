/**
*	created:		2012-7-4   0:00
*	filename: 		CreatureActionCSC
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		响应服务器发下来CSC类型消息
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/Event/IEvent.h"
#include "../../FKGame/SceneObj/ICreature.h"
//------------------------------------------------------------------------
class IEvent;
class IEventActionListener;
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_MOVE);
class CCreatureAction_Creature_Move_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/************ CCreatureAction_Creature_Move_CSC *********/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_CHANGELOCATION);
class CCreatureAction_Creature_ChangeLocation_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/****  CCreatureAction_Creature_ChangeLocation_CSC ******/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_CHANGELOCATION_FOR_SKILL);
class CCreatureAction_Creature_ChangeLocationForSkill_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/****  CCreatureAction_Creature_ChangeLocation_CSC ******/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_CHANGEMOVESTYLE);
class CCreatureAction_Creature_ChangeMoveStyle_CSC : public IEventActionListener
{
public:
	virtual void OnAction(IEvent * pEvent);
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_STOP_ATTACK);
class CCreatureAction_Creature_StopAttack_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/****  CCreatureAction_Creature_StopAttack_CSC **********/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_PRE_ATTACK);
class CCreatureAction_Creature_PreAttack_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/*****CCreatureAction_Creature_PreAttack_CSC ************/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_ATTACK);
class CCreatureAction_Creature_Attack_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/****CCreatureAction_Creature_Attack_CSC ****************/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_STOP_PRE_ATTACK);
class CCreatureAction_Creature_StopPreAttack_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/*****CCreatureAction_Creature_StopPreAttack_CSC*********/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_DAMAGE_REHEARSAL);
class CCreatureAction_Creature_Damage_Rehearsal_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/****** CCreatureAction_Creature_Damage_Rehearsal_CSC ***/
};
//------------------------------------------------------------------------
// MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_CURE_REHEARSAL);
class CCreatureAction_Creature_Cure_Rehearsal_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/****** CCreatureAction_Creature_Cure_Rehearsal_CSC ***/
};
//------------------------------------------------------------------------
// MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_DIE
class CCreatureAction_Creature_Die_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/*******CCreatureAction_Creature_Die_CSC ****************/
};
//------------------------------------------------------------------------
// MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_UPGRADE
class CCreatureAction_Creature_Upgrade_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/******* CCreatureAction_Creature_Upgrade_CSC ***********/
};
//------------------------------------------------------------------------
// MAINACTION_CIRCULTYPE_CSC，MAINACTION_MSGID_BRUSHPROPDATA
class CCreatureAction_Creature_BrushProData_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/******* CCreatureAction_Creature_BrushProData_CSC ******/
private:
	void		 BrushMessage_Exp(ICreature * pCreature, int nOldVal, int nNewVal);
};
//------------------------------------------------------------------------
// MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSG_OPENBOX
class CCreatureAction_Creature_OpenBox_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/******  CCreatureAction_Creature_OpenBox_CSC ***********/
};
//------------------------------------------------------------------------
// MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_RELIVE
class CCreatureAction_Creature_Relive_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/*******  CCreatureAction_Creature_Relive_CSC ***********/
};
//------------------------------------------------------------------------
// MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_STAND
class CCreatureAction_Creature_Stand_CSC : public IEventActionListener
{
public:
	//////////////////////////////////////////////////////////
	/*************** IEventActionListener *******************/
	virtual void OnAction(IEvent * pEvent);
	
	//////////////////////////////////////////////////////////
	/**********CCreatureAction_Creature_Stand_CSC ***********/
};
//------------------------------------------------------------------------