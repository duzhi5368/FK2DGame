/**
*	created:		2012-7-3   0:02
*	filename: 		BoxPopedomMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		box权限管理，集中管理
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <vector>
#include <map>
#include "../../FKGame/Event/IEvent.h"
using std::vector;
using std::map;
//------------------------------------------------------------------------
typedef __int64 UID;
//------------------------------------------------------------------------
class CBoxPopedomManager : public IEventActionListener
{
public:
	virtual void OnAction(IEvent* pEvent);

	// 增加一权限者,
	void AddMaster(UID uidBox, long lMasterDBID);
	void DelMaster(UID uidBox, long lMasterDBID);

	// 检测能否拾起物品
	BOOL CanOpen(UID uidBox, UID uidMaster);
	BOOL CanOpen(UID uidBox, long lMasterDBID);

	// 清除此box的归属
	BOOL ClearBox(UID uidBox);

	// 取得此box是否已有归属
	BOOL IsAttach(UID uidBox);

	// 取得有权限者
	void GetMaster(UID uidBox, long* pMasterDBID, int &nMasterCount);

	// 取得有权限的人数
	int GetCount(UID uidBox) const;

	// 盒子先于人物创建的解决
	void CheckBox(UID uidBox);

	CBoxPopedomManager(void);
	~CBoxPopedomManager(void);
private:
	BOOL Init();
	void Term();

private:
	typedef vector<UID>					UID_VEC;
	typedef vector<long>			    TVECTOR_BOXMASTER;
	typedef map<UID, TVECTOR_BOXMASTER>	TMAP_ALLBOX;
	TMAP_ALLBOX		m_mapAllBox;
	UID_VEC         m_uidVec;
	BOOL            m_bInit;	
};
//------------------------------------------------------------------------
extern CBoxPopedomManager g_BoxPopedomManager;
//------------------------------------------------------------------------