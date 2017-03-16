/**
*	created:		2012-7-1   0:03
*	filename: 		ScriptAdapter_cse
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		csv加密格式脚本加载器
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/Script/IScriptUpdate.h"
#include "ScriptUpdate.h"
#include <vector>
#include <string>
using std::vector;
using std::string;
//------------------------------------------------------------------------
class CScriptAdapter_cse;
//------------------------------------------------------------------------
class CScriptAdaptercse_row : public IScriptIterator
{
public:
	virtual int Size() const;	
	virtual _VARIANT& GetData(int nIndex);	
	virtual _VARIANT& operator [](int nIndex);	
	virtual _VARIANT& GetData(LPCSTR szName);	
	virtual _VARIANT& operator [](LPCSTR szName);	
	virtual void Release();

	CScriptAdaptercse_row();
	BOOL InitData(CScriptAdapter_cse* pScript, char* line, char cSeparator, BOOL bAllowSeries);
	virtual ~CScriptAdaptercse_row();
	
private:
	CScriptAdapter_cse* m_pScript;
	_VARIANT*           m_pData;
	char*               m_pLine;
	int				    m_nSize;
};
//------------------------------------------------------------------------
class CScriptAdapter_cse : public IScriptIterator
{
public:
	virtual int Size() const;	
	virtual _VARIANT& GetData(int nIndex);	
	virtual _VARIANT& operator[](int nIndex);	
	virtual _VARIANT& GetData(LPCSTR szName);	
	virtual _VARIANT& operator[](LPCSTR szName);	
	virtual void Release();

	CScriptAdapter_cse();
	virtual BOOL LoadScript(LPCSTR szFile);	
	virtual IScriptIterator* GetScriptIterator();	
	virtual ~CScriptAdapter_cse();

public:
	int GetRowIndex(LPCSTR lpRowTitle);
	int GetColIndex(LPCSTR lpColTitle);

protected:
	std::vector<_VARIANT>	 m_table; // 行集
	std::vector<std::string> m_rowTitle; // 行标题集
	CScriptAdaptercse_row    m_colTitle; // 列标题集,也就是第一行
};
//------------------------------------------------------------------------