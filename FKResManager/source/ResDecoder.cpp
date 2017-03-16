/**
*	created:		2012-6-18   20:27
*	filename: 		ResDecoder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../Include/ResMgrCommon.h"
#include "../Include/ResObject.h"
#include "../Include/ResScript.h"
#include "../Include/ResDecoder.h"
#include <sys/stat.h>
#include "../Include/Encrypt.h"
//------------------------------------------------------------------------
// 构造函数
CResDecoder::CResDecoder(CResScript* pResScript, CResObject* pResObjectRoot)
{
	m_pResScript = pResScript;
	m_pScriptBuffer = NULL;
	m_Pos = NULL;
}
//------------------------------------------------------------------------
// 析构函数
CResDecoder::~CResDecoder()
{
	// 释放宏所占资源
	if (!m_Macro.empty())
		m_Macro.clear();
}
//------------------------------------------------------------------------
// 解析资源脚本文件
BOOL CResDecoder::ParseFile(IFileObject* pFileObject, BOOL bIgnoreID, BYTE* pKey)
{
	// 确认是否获取了祖宗对象指针
	Assert(pFileObject && m_pResScript);

	// 打开文件
	if (!pFileObject->Open())
	{
		FKResTraceColorLn( MY_TRACE_COLOR, 
			"ERROR: CResDecoder::ParseFile() catch error: can't open file[\"%s\"]", pFileObject->GetFilePath());
		return FALSE;
	}
	
	// 获得文件大小
	int uFileLen = pFileObject->GetLength();
	
	// 分配内存
	m_pScriptBuffer = new char[uFileLen + 3];
	Assert(m_pScriptBuffer);

	// 解析
	pFileObject->SeekToBegin();
	pFileObject->Read(m_pScriptBuffer, uFileLen);
	pFileObject->Close();

	if (pKey)
	{
		CEncrypt en;
		en.Decrypt((BYTE*)m_pScriptBuffer, uFileLen + 3, pKey);
	}
	
	// 自动产生一个回车换行符，有利于一行字符串的读取
	m_pScriptBuffer[uFileLen] = '\r';
	m_pScriptBuffer[uFileLen+1] = '\n';
	m_pScriptBuffer[uFileLen+2] = '\0';
	m_Pos = m_pScriptBuffer;
	if (!ParseToBuffer(bIgnoreID))
	{
		delete [] m_pScriptBuffer;
		m_pScriptBuffer = NULL;
		return FALSE;
	}
	
	// 暂时不可释放，被占用
	// delete [] m_pScriptBuffer;
	m_pScriptBuffer = NULL;

	return TRUE;
}
//------------------------------------------------------------------------
// 从文件缓冲中获取一行文本
inline char* CResDecoder::GetStringLine(string& str)
{
	int nLen = 0;
	if (*m_Pos == 0) return NULL;
	while (*m_Pos != 13)
	{
		m_Pos ++;
		nLen ++;
	}
	*m_Pos = 10;
	str.clear();
	str.resize(nLen + 1);
	memcpy(&str[0], m_Pos - nLen, nLen + 1);
	m_Pos += 2;
	return (m_Pos - nLen - 2);
}
//------------------------------------------------------------------------
// 解析缓存数据,若成功返回处理后的脚本长度，否则返回0
BOOL CResDecoder::ParseToBuffer(BOOL bIgnoreID)
{
	int nLineNum = 0;				// 行数
	string strLine;					// 行缓存
	int nDepth = 0, oldDepth = 0;	// 对象层次深度计数
	string strName, strType, strID; // 对象的名字、类型、ID缓冲
	UINT uID = 0;
	CResObject* obj = static_cast<CResObject*>(m_pResScript->GetResObjectRoot());
	Assert(obj);
	UINT uParentID = obj->GetID();

    // 一行一行的读
	while(GetStringLine(strLine))
    {
		nLineNum++;

		switch(GetLineType(strLine))
		{
		case LT_UNDEFINE:
			FKResTraceColorLn(MY_TRACE_COLOR, "第 %d 行出现不可解析的语句或语法错误：\n\n%s", nLineNum, strLine.c_str());
			return FALSE;
		case LT_EMPTY:
		case LT_COMMENT:
			break;
		case LT_MACRO:
		{
			// 附加去掉每行后面的注释
			int nLocate = strLine.find_last_of("//");
			if (nLocate != -1)
				_left(strLine, nLocate - 1);

			_trim(strLine);
			_erase_left(strLine, 7); // 7 = #define
			_trim(strLine);
			int pos = _find_char(strLine, ' ');
			if (pos == 0)			// 不存在
				pos = _find_char(strLine, '\t');
			if (pos == 0) 
			{
				TraceLn("第 %d 行出现不可解析的宏定义：\n\n%s", nLineNum, strLine.c_str());
				return FALSE;
			}
			string strMacroName = strLine;
			string strValue = strLine;
			_left(strMacroName, pos - 1);
			_trim(strMacroName);
			_erase_left(strValue, pos);
			_trim(strValue);
			RecordMacro(strMacroName, strValue);
		}
		break;

		// 记录对象名、类型名和ID
		case LT_OBJECT:
			{
				HandleMacro(strLine);
				_trim(strLine);
				_erase_left(strLine, 6);

				int pos = _find_char(strLine, ':');
				int pos1 = _find_char(strLine, '=');
				strName.clear();

				if (pos) // 有类型
				{
					strName = strLine;
					_left(strName, pos - 1);
					_trim(strName);
					if (pos1) // 有ID
					{
						strType = strLine;
						_mid(strType, pos, pos1 - 2);
						_trim(strType);

						if (bIgnoreID)
							uID = 0;
						else
						{
							strID = strLine;
							_erase_left(strID, pos1);
							_trim(strID);
							uID = atoi(strID.c_str());
						}
					}
					else // 无ID
					{
						uID = 0;
						strType = strLine;
						_erase_left(strType, pos);
						_trim(strType);
					}
				}
				else // 无类型
				{
					strType.clear();
					if (pos1) // 有ID
					{
						strName = strLine;
						_left(strName, pos1 - 1);
						_trim(strName);

						if (bIgnoreID)
							uID = 0;
						else
						{
							strID = strLine;
							_erase_left(strID, pos1);
							_trim(strID);
							uID = atoi(strID.c_str());
						}
					}
					else // 无ID
					{
						strName = strLine;
						_trim(strName);
						uID = 0;
					}
				}
			}
			break;

		// new一个对象
		case LT_LBRACKET:
			{
				nDepth ++;

				if (nDepth > oldDepth)			// 第一个子节点
					uParentID = obj->GetID();

				// 新建对象
				obj = new CResObject(m_pResScript);
				Assert(obj);
				obj->SetObjectInfo(strName.c_str(), strType.c_str());
				obj->SetParentID(uParentID);
				m_pResScript->AddToHashMap(uID, static_cast<IResObject*>(obj));
				
				IResObject* pParentObject = m_pResScript->GetResObject(uParentID);
				Assert(pParentObject);
				pParentObject->AddSubObject(static_cast<IResObject*>(obj));

				oldDepth = nDepth;
			}
			break;

		// 跳出
		case LT_RBRACKET:
			{
				nDepth --;
				obj = static_cast<CResObject*>(obj->GetParentObject());
				uParentID = obj->GetID();
				oldDepth = nDepth + 1;
			}
			break;

		// 给当前对象添加属性
		case LT_PROPERTY:
			{
				HandleMacro(strLine);
				int pos = _find_char(strLine, '=');
				string strPropName = strLine;
				string strValue = strLine;
				_left(strPropName, pos - 1);
				_erase_left(strValue, pos);
				_trim(strPropName);
				_trim(strValue);
				Assert( obj->SetPropertyString(strPropName.c_str(), &strValue[0]) );
			}
			break;
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 分析每一行的语句类型,不修改其内容
LINETYPE CResDecoder::GetLineType(string& strLine)
{
    char CurChar;
    bool bIsValidChar = false;
	int  k = 0;
    while (CurChar = strLine[k++])
    {
		switch(CurChar)
		{
		case '/':		// 是注释行("//")
			if (!bIsValidChar && strLine[k] == '/')
				return LT_COMMENT;
			else
				bIsValidChar = true;
			break;

		case ' ':		// 是空格和TAB键，忽略之
		case '\t':
			break;

		case '\r':		// 是回车符
		case '\n':		// 是换行符
			if (!bIsValidChar)
				return LT_EMPTY;
			break;

		case '#':		// 是宏定义(#define)
			if (!bIsValidChar)
				return LT_MACRO;
			break;

		case '{':		// 是{
			if (!bIsValidChar)
				return LT_LBRACKET;
			break;

		case '}':		// 是}
			if (!bIsValidChar)
				return LT_RBRACKET;
			break;

		case '=':		// 是}
			if (bIsValidChar)
				return LT_PROPERTY;
			break;

		case 'O':		// 可能会是Object
		case 'o':
			if (!bIsValidChar && _find_string_no_case(strLine, "object"))
				return LT_OBJECT;
			bIsValidChar = true;
			break;

		default: // 中文字或不可识别的语句
			bIsValidChar = true;
			break;
		}
	}

	return LT_UNDEFINE;
}
//------------------------------------------------------------------------
// 录制宏
void CResDecoder::RecordMacro(string strMacroName, string strValue)
{
	MACRO macro;
	macro.Name = strMacroName;
	macro.Value = strValue;
	m_Macro.push_back(macro);
}
//------------------------------------------------------------------------
// 处理宏(宏替换)
BOOL CResDecoder::HandleMacro(string& strLine)
{
	// 宏替换
	if (m_Macro.empty())
		return TRUE;

	vector<MACRO>::iterator it;
	for (it = m_Macro.begin(); it != m_Macro.end(); it++)
	{
		MACRO  mac = *it;
		int pos = 1;
		while(pos)
		{
			pos = _find_string(strLine, mac.Name);
			if(!pos)
				break;

			strLine.replace(strLine.begin() + pos - 1, 
				strLine.begin() + pos -1 + mac.Name.size(),
							mac.Value.c_str());
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 搜索res文件,返回指定ID的对象的脚本
BOOL CResDecoder::ParseObjectServer(UINT uID, char* pData)
{
	char buf[MAX_SCRIPT_LINE_LEN];
	BOOL bFlags = FALSE;
	BOOL bHaveRightBracket = FALSE;
	int nLineNum = 0;
	string strLine, strID;
	char strTmp[64];
	memset( strTmp, 0, sizeof(strTmp) );
	_itoa( nLineNum, strTmp, 10 );
	strID = string(strTmp);

	Assert(pData);
	
	IFileObject* pFileObject = _create_fileobject(m_pResScript->GetScriptFileName());
	Assert(pFileObject);
	pFileObject->Open("r");

	while (pFileObject->ReadString(buf, MAX_SCRIPT_LINE_LEN))
	{
		strLine = buf;
		nLineNum++;
		switch(GetLineType(strLine))
		{
		case LT_UNDEFINE:// 未定义的语句
			TraceLn("第 %d 行出现不可解析的语句或语法错误：\n\n%s", nLineNum, strLine.c_str());
			*pData = 0;
			return FALSE;
		case LT_EMPTY:
		case LT_COMMENT:
			continue;
			break;
		case LT_MACRO:
		{
			_trim(strLine);
			_erase_left(strLine, 7); // 7 = #define
			_trim(strLine);
			int pos = _find_char(strLine, ' ');
			if (pos == 0) // not exist!
				pos = _find_char(strLine, '\t');
			if (pos == 0) 
			{
				TraceLn("第 %d 行出现不可解析的宏定义：\n\n%s", nLineNum, strLine.c_str());
				*pData = 0;
				return FALSE;
			}
			string strMacroName = strLine;
			string strValue = strLine;
			_left(strMacroName, pos - 1);
			_trim(strMacroName);
			_erase_left(strValue, pos);
			_trim(strValue);
			RecordMacro(strMacroName, strValue);
		}
		break;

		case LT_OBJECT:// object ...
			HandleMacro(strLine);
			if (bFlags == FALSE)
			{
				if (_find_string(strLine, strID)) // 真的找到啦?呵呵
				{
					// 如果要找10，而这里是100呢?
					int pos = _find_char(strLine, '=');
					if (pos)
					{
						string strID1 = strLine;
						_erase_left(strID1, pos);
						_trim(strID1);
						UINT uID1 = atoi(strID1.c_str());
						if (uID1 != uID)
							continue;
					}

					bFlags = TRUE;
					if (*pData != '\0')
						*pData = '\0';
				}
			}
			else
			{
				if (!bHaveRightBracket)
				{
					_trim_left(strLine);
					lstrcat(pData, "}\n");
				}

				SAFE_RELEASE(pFileObject);
				return TRUE;
			}
			break;

		case LT_LBRACKET: // "{"
			break;
		case LT_RBRACKET: // "}"
			if (bFlags)
			{
				bHaveRightBracket = TRUE;
				_trim_left(strLine);
				lstrcat(pData, strLine.c_str());

				SAFE_RELEASE(pFileObject);
				return TRUE;
			}
			break;
		case LT_PROPERTY:
			HandleMacro(strLine);
			break;
		}

		if (bFlags)
		{
			_trim_left(strLine);
			lstrcat(pData, strLine.c_str());
		}
	}

	// 没有!
	pData[0] = '\0';
	SAFE_RELEASE(pFileObject);
	return FALSE;
}
//------------------------------------------------------------------------
// 由某段字符串构建对象
IResObject* CResDecoder::ParseObjectClient(char* pData, int nSize)
{
	string strLine, strName, strType, strID;
	CResObject* obj = NULL;
	UINT uID = 0;
	string buf = pData;
	int nPos = _find_char(buf, '\n');
	while(nPos)
	{
		strLine.resize(nPos);
		int nNum = buf.copy(&strLine[0], nPos);

		switch (GetLineType(strLine))
		{
		case LT_OBJECT:
			{
				_trim(strLine);
				_erase_left(strLine, 6); // erase "Object"

				int pos = _find_char(strLine, ':');
				int pos1 = _find_char(strLine, '=');

				if (pos) // 有类型
				{
					strName = strLine;
					_left(strName, pos - 1);
					_trim(strName);
					if (pos1) // 有ID
					{
						strType = strLine;
						_mid(strType, pos, pos1 - 2);
						_trim(strType);

						strID = strLine;
						_erase_left(strID, pos1);
						_trim(strID);

					}
					else // 无ID
					{
						uID = 0;
						strType = strLine;
						_erase_left(strType, pos);
						_trim(strType);
					}
				}
				else // 无类型
				{
					strType.clear();
					if (pos1) // 有ID
					{
						strName = strLine;
						_left(strName, pos1 - 1);
						_trim(strName);

						strID = strLine;
						_erase_left(strID, pos1);
						_trim(strID);
						uID = atoi(strID.c_str());
					}
					else // 无ID
					{
						strName = strLine;
						_trim(strName);
						uID = 0;
					}
				}
			}
			break;

		case LT_LBRACKET:
			{
				// 新建对象
				obj = new CResObject();
				Assert(obj != NULL);
				obj->SetObjectInfo(strName.c_str(), strType.c_str());
				obj->SetID(uID);
			}
			break;
		case LT_RBRACKET:
			return static_cast<IResObject*>(obj);
		case LT_PROPERTY:
			{
				int pos = _find_char(strLine, '=');
				string strPropName = strLine;
				string strValue = strLine;
				_left(strPropName, pos - 1);
				_erase_left(strValue, pos);
				_trim(strPropName);
				_trim(strValue);
				Assert( obj->SetPropertyString(strPropName.c_str(), &strValue[0]) );
			}
			break;
		}

	_erase_left(buf, nPos);
	nPos = _find_char(buf, '\n');
	}
	
	return static_cast<IResObject*>(obj);
}
//------------------------------------------------------------------------