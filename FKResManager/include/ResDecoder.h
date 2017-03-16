/**
*	created:		2012-6-18   20:18
*	filename: 		ResDecoder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// 宏结构
typedef struct _MACRO
{
public:
	string Name;
	string Value;
public:
	_MACRO()	{Name.clear();Value.clear();}
	_MACRO(string name, string value)	{Name = name; Value = value;}
	~_MACRO()	{Name.clear();Value.clear();}
} MACRO;
//------------------------------------------------------------------------
// 资源解码类
class CResDecoder
{
private:
	CResScript*			m_pResScript;		// 资源脚本指针
	char*				m_pScriptBuffer;	// 资源脚本缓存
	char*				m_Pos;				// 资源脚本文件指针
	vector<MACRO>		m_Macro;			// 宏数组

public:
	CResDecoder(CResScript* pResScript, CResObject* pResObjectRoot);
	virtual ~CResDecoder();

	BOOL ParseFile(IFileObject* pFileObject, BOOL bIgnoreID = FALSE, BYTE* pKey = NULL);
	BOOL ParseToBuffer(BOOL bIgnoreID = FALSE);
	LINETYPE GetLineType(string& strLine);
	inline char* GetStringLine(string& str);

	BOOL ParseObjectServer(UINT uID, char* pData);
	IResObject* ParseObjectClient(char* pData, int nSize);

	void RecordMacro(string strMacroName, string strValue);
	BOOL HandleMacro(string& strLine);
};
//------------------------------------------------------------------------