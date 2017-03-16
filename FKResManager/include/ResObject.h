/**
*	created:		2012-6-18   20:21
*	filename: 		ResObject
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CResObject : public IResObject
{
public:

private:
	RESOBJECTINFO	m_ResObjectInfo;	// 对象信息
	vector<string>	m_vProp;			// 对象属性
	vector<UINT>	m_vID;				// 子对象ID列表
public:
	CResScript*		m_pResScript;		// 用于通过ID访问其他对象

public:
	CResObject();
	CResObject(CResScript* pResScript);
	virtual ~CResObject();

	// IResObject
	// 对象信息相关
	void	SetObjectInfo(LPCSTR szObjectName = NULL, LPCSTR szTypeName = NULL);
	LPCSTR	GetObjectName()	{return m_ResObjectInfo.strName.c_str();}
	LPCSTR	GetTypeName(){return m_ResObjectInfo.strType.c_str();}
	UINT	GetID()			{return m_ResObjectInfo.uID;}
	void Empty();
	BOOL Release();
	// 全局对象相关
	IResObject* GetResObject(UINT uID);
	// 父对象相关
	UINT	GetParentID()	{return m_ResObjectInfo.uParentID;}
	IResObject* GetParentObject();
	// 子对象相关
	IResObject* CreateSubObject(int nIndex = -1);
	int GetSubObjectCount();
	IResObject* GetSubObjectByName(LPCSTR szObjectName = NULL);
	IResObject* GetSubObjectByIndex(int nIndex);
	BOOL AddSubObject(IResObject* pResObject);
	BOOL EraseSubObjectByName(LPCSTR szObjectName = NULL);
	BOOL EraseSubObjectByIndex(int nIndex);
	// 对象属性相关
	int  GetPropertyCount();
	LPCSTR GetPropertyName(int nIndex);
	LPCSTR GetPropertyValue(int nIndex);
	BOOL AddProperty(LPCSTR szPropertyName, LPCSTR szPropertyValue = NULL);
	BOOL EraseProperty(LPCSTR szPropertyName);
	BOOL SetPropertyName(LPCSTR szOldPropertyName, LPCSTR szNewPropertyName);
	LPCSTR GetPropertyString(LPCSTR szPropertyName);
	BOOL GetPropertyLong  (LPCSTR szPropertyName, long& nReturn, long nDefault = 0L);
	BOOL GetPropertyFloat (LPCSTR szPropertyName, double& dlReturn,	double dlDefault = 0.0);
	int GetArrayLong (LPCSTR szArrayName, long*   lpData, int nCount = 0);
	int GetArrayFloat(LPCSTR szArrayName, double* lpData, int nCount = 0);
	int GetArrayByte (LPCSTR szArrayName, BYTE*   lpData, int nCount = 0);
	// 设置...
	BOOL SetPropertyString(LPCSTR szPropertyName, LPCSTR szSetString);
	BOOL SetPropertyLong  (LPCSTR szPropertyName, long nSet);
	BOOL SetPropertyFloat (LPCSTR szPropertyName, double dlSet);
	BOOL SetArrayLong (LPCSTR szArrayName, long*   lpData, int nCount);
	BOOL SetArrayFloat(LPCSTR szArrayName, double* lpData, int nCount);
	BOOL SetArrayByte (LPCSTR szArrayName, BYTE*   lpData, int nCount);
	IResObject* Duplicate(BOOL bUseNewID = TRUE);
	IResScript* GetResScript();

	BOOL SetProperty(LPCSTR szPropertyName, LPCSTR szString = NULL);
	void SetParentID(UINT uID)	{ m_ResObjectInfo.uParentID=uID;}
	void SetID(UINT uID)	{ m_ResObjectInfo.uID = uID;}
	int	  FindProperty(LPCSTR szPropertyName);
	BOOL ModifyString(int nIndex, LPCSTR szString);
	BOOL ExistType(){return (!m_ResObjectInfo.strType.empty());}
	BOOL RemoveID(UINT uID);

#ifdef _DEBUG
	BOOL Dump();
#endif
};
//------------------------------------------------------------------------