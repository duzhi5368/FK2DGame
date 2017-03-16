/**
*	created:		2012-6-18   21:12
*	filename: 		ResScript
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../Include/ResMgrCommon.h"
#include "../Include/ResDecoder.h"
#include "../Include/ResObject.h"
#include "../Include/BitTool.h"
#include "../Include/ResScript.h"
#include "../Include/StdFileObject.h"
#include "../Include/Encrypt.h"
//------------------------------------------------------------------------
CResScript::CResScript()
{
	m_pResObjectRoot = NULL;
	m_pFileObject = NULL;
	m_bReleaseFileObject = false;

	Empty();
}
//------------------------------------------------------------------------
CResScript::~CResScript()
{
}
//------------------------------------------------------------------------
// 新建空的资源脚本
BOOL CResScript::Create(int nStartID/* = 0*/)
{
	Empty();

	m_pResObjectRoot = new CResObject(this);
	Assert(m_pResObjectRoot);
	m_pResObjectRoot->SetObjectInfo("Root", NULL);
	m_pResObjectRoot->SetID(0);
	m_HashMap[0] = m_pResObjectRoot;
	m_nCount ++;
	return m_pResObjectRoot->SetPropertyLong("MaxID", nStartID);
}
//------------------------------------------------------------------------
// 加载资源脚本
BOOL CResScript::LoadScript(IFileObject* pFileObject, BOOL bIgnoreID, 
							bool bReleaseFileObject, BYTE* pKey)
{
	Assert(pFileObject);

	m_pFileObject = pFileObject;
	m_bReleaseFileObject = bReleaseFileObject;

	// 构造对象指针
	m_pResObjectRoot = new CResObject(this);
	Assert(m_pResObjectRoot);
	m_pResObjectRoot->SetObjectInfo("Root", NULL);
	m_pResObjectRoot->SetID(0);
	m_pResObjectRoot->SetPropertyLong("MaxID", 0);
	m_HashMap[0] = m_pResObjectRoot;
	m_nCount ++;

	CResDecoder ResDecoder(this, m_pResObjectRoot);
	// 解析脚本文件
	if (!ResDecoder.ParseFile(pFileObject, bIgnoreID, pKey))
	{
		SAFE_RELEASE(m_pResObjectRoot);
		return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------
// 加载类型列表
BOOL CResScript::LoadTypeList(LPCSTR szTypeListFileName)
{
	Assert(szTypeListFileName != NULL && *szTypeListFileName != 0);

	// 加载类型定义
	hash_map<string, DATATYPE>	mapType;
	mapType["bool"]		= dt_bool;
	mapType["char"]		= dt_char;
	mapType["short"]	= dt_short;
	mapType["int"]		= dt_int;
	mapType["float"]	= dt_float;
	mapType["byte"]		= dt_byte;
	mapType["word"]		= dt_word;
	mapType["dword"]	= dt_dword;
	mapType["string"]	= dt_string;

	FILE* pFile = NULL;
	pFile = fopen(szTypeListFileName, "r");
	if (pFile == NULL)
		return FALSE;

	string strLine(MAX_PATH, 0);
	char buf[MAX_PATH];
	TYPEINFO* pTypeInfo = NULL;

	while (fgets(buf, MAX_PATH, pFile))
	{
		strLine = buf;
		if (_find_string_no_case(strLine, "struct")) // 类型
		{
			// 去掉"struct"
			_trim(strLine);
			_erase_left(strLine, 6);

			string strTypeName = strLine;
			string strTypeID = strLine;
			_left_c(strTypeName, '=');
			_right_c(strTypeID, '=');

			pTypeInfo = new TYPEINFO;
			Assert(pTypeInfo);
			pTypeInfo->name = _trim(strTypeName);
			pTypeInfo->cbID = atoi(_trim(strTypeID));
			Assert(pTypeInfo->cbID != 0); // 0为系统默认的无效ID

			// 加入映射表
			m_TypeList[_make_lower(strTypeName)] = pTypeInfo;
		}
		else if (_find_string_no_case(strLine, "dim")) // 属性
		{
			PROPINFO pi;
			string str = strLine;

			// 去掉"dim"
			_trim(strLine);
			_erase_left(strLine, 3);
			_trim(strLine);

			int pos;
			if ((pos = _find_char(strLine, ' ')) || 
				(pos = _find_char(strLine, '\t'))) // 存在类型说明
			{
				string strType = strLine;
				_left(strType, pos - 1);
				_make_lower(strType);
				hash_map<string, DATATYPE>::iterator it = mapType.find(strType.c_str());
				if (it != mapType.end())
				{
					pi.type = (*it).second;
				}
				else
				{
					FKResTraceColorLn(MY_TRACE_COLOR, "错误： CResScript::LoadTypeList() 出现异常，无法识别的数据类型:%s !", strType.c_str());
					return FALSE;
				}

				// 去掉属性类型说明符(int dword,word,long,char,...)
				_erase_left(strLine, pos - 1);
				_trim(strLine);

				str = strLine;
				if (pos = _find_char(strLine, ':')) // 指定了域长
				{
					// 获得属性名
					_left(str, pos - 1);
					_trim(str);
					pi.propname = str;

					// 获得域长
					_erase_left(strLine, pos);
					_trim(strLine);
					pi.length = atoi(strLine.c_str());
					switch (pi.type)
					{
					case dt_undefine:
						FKResTraceColorLn(MY_TRACE_COLOR, 
							"错误： CResScript::LoadTypeList() 出现异常。无效的类型名称 dt_undefine ！");
						return FALSE;

					case dt_bool:
					case dt_char:
					case dt_byte:
						{
							if (pi.length > 8)
							{
								FKResTraceColorLn(MY_TRACE_COLOR,
									"错误： CResScript::LoadTypeList() 出现错误，字节过长！");
								return FALSE;
							}
						}
						break;
					case dt_short:
					case dt_word:
						{
							if (pi.length > 16)
							{
								FKResTraceColorLn(MY_TRACE_COLOR,
									"错误： CResScript::LoadTypeList() 出现错误，字节过长！");
								return FALSE;
							}
						}
						break;
					case dt_int:
					case dt_dword:
					case dt_float:
						{
							if (pi.length > 32)
							{
								FKResTraceColorLn(MY_TRACE_COLOR,
									"错误： CResScript::LoadTypeList() 出现错误，字节过长！");
								return FALSE;
							}
						}
						break;
					case dt_string:
						break;
					}

					pi.isArray = false;
				}
				else // 没指定域长
				{
					if (pos = _find_char(strLine, '[')) // 指定数组
					{
						// 获得属性名
						_left(str, pos - 1);
						_trim(str);
						pi.propname = str;

						// 获得数组长度
						_erase_right(strLine, 1);
						_erase_left(strLine, pos);
						_trim(strLine);

						pi.length = atoi(strLine.c_str());
						pi.isArray = true;
					}
					else // 没指定数组
					{
						pi.propname = str;
						pi.isArray = false;
						switch (pi.type)
						{
						case dt_undefine:
							FKResTraceColorLn(MY_TRACE_COLOR,
								"错误： CResScript::LoadTypeList() 出现异常。无效的类型名称 dt_undefine ！");
							return FALSE;

						case dt_bool:
						case dt_char:
						case dt_byte:
							pi.length = 8;
							break;
						case dt_short:
						case dt_word:
							pi.length = 16;
							break;
						case dt_int:
						case dt_dword:
						case dt_float:
							pi.length = 32;
							break;
						case dt_string:
							pi.length = 0;
							pi.isArray = true;
							break;
						}
					}
				}
				// 记录
				if (pTypeInfo)
				{
					pTypeInfo->prop.push_back(pi);
					int size=pTypeInfo->prop.size();
				}
			}
			else
				TraceLn("无效的属性描述: %s", str.c_str());
		}
	}

	fclose(pFile);
	return TRUE;
}
//------------------------------------------------------------------------
// 保存资源脚本(以后可能要传入类似IFileObject的接口)
BOOL CResScript::SaveScript(LPCSTR szResScriptFileName, BYTE* pKey)
{
	Assert(m_pResObjectRoot);

	FILE* fp = NULL;
	if (szResScriptFileName == NULL) // 默认
	{
		VerifyR(!m_pFileObject, FALSE, 
			"CResScript::SaveScript() 出现异常：文件名为空！");
		fp = fopen(m_pFileObject->GetFilePath(), "w");
	}
	else
	{
		if (m_pFileObject == NULL)
		{
			m_pFileObject = _create_fileobject(szResScriptFileName);
			Assert(m_pFileObject);
		}
		fp = fopen(szResScriptFileName, "w");
	}

	Assert(fp);

	// 写标头
	char header[]="FreeKnight资源脚本\n";  /*RESSCRIPT\n*/  /*"///////////////////////////////\n// *******  资源脚本  *******\n//\n//  每行最大长度2048字节\n///////////////////////////////\n\n";*/
	fwrite(header, lstrlen(header), 1, fp);

	// 写祖宗对象的属性
	int nCount = m_pResObjectRoot->GetPropertyCount();
	char s[256];
	for (int i = 0; i < nCount; i++)
	{
		wsprintf(s, "%s = %s\n", m_pResObjectRoot->GetPropertyName(i), m_pResObjectRoot->GetPropertyValue(i));
		fwrite(s, lstrlen(s), 1, fp); // ... = ...
	}

	// 写其他对象
	WriteResObject(m_pResObjectRoot, fp);

	// 关闭文件
	fclose(fp);

	if (pKey)
	{
		CStdFileObject file;
		file.SetFilePath(szResScriptFileName);
		// 打开文件
		if (!file.Open("rb"))
		{
			FKResTraceColorLn(MY_TRACE_COLOR, 
				"错误： CResScript::SaveScript() 出现异常。无法打开文件 %s", file.GetFilePath());
			return FALSE;
		}

		// 获得文件大小
		int uFileLen = file.GetLength();

		// 分配内存
		BYTE* pBuffer = new BYTE[uFileLen + 3];
		Assert(pBuffer);

		// 解析
		file.SeekToBegin();
		file.Read(pBuffer, uFileLen);
		file.Close();

		CEncrypt en;
		en.Encrypt(pBuffer, uFileLen + 3, pKey);

		// 打开文件
		if (!file.Open("wb"))
		{
			FKResTraceColorLn(MY_TRACE_COLOR, 
				"错误： CResScript::SaveScript() 出现异常。无法打开文件 %s！", file.GetFilePath());
			return FALSE;
		}
		file.Write(pBuffer, uFileLen);
		file.Close();
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 编译
BOOL CResScript::Compile(LPCSTR szFileName/* = NULL*/)
{
	if (!m_pResObjectRoot)
		return	FALSE;

	VerifyR(m_TypeList.empty(), FALSE,
		"CResScript::Compile() 出现异常。无法读取 TypeList 表文件！");

	FILE* fp = NULL;
	if (szFileName == NULL) // 默认
	{
		VerifyR(!m_pFileObject, FALSE, 
			"CResScript::Compile() 出现异常。文件名为空！");

		string strFileName = m_pFileObject->GetFilePath();
		int pos = strFileName.find_last_of('.') + 1;
		if (pos)
			_left(strFileName, pos - 1);

		strFileName += ".rcf";
		fp = fopen(strFileName.c_str(), "wb");
	}
	else
	{
		fp = fopen(szFileName, "wb");
	}
	Assert(fp);

	int nFilePos = 0;
	vector<ID_OFFSET>	idOffsetTable; // 用于记录ID-OFFSET映射

	// 写文件头
	BINOBJECTHEADER bohd;
	fwrite(&bohd, 1, sizeof(BINOBJECTHEADER), fp);
	nFilePos += sizeof(BINOBJECTHEADER);

	// 写对象
	WriteRootObject(m_pResObjectRoot, idOffsetTable, nFilePos, fp);
	WriteBinObject(m_pResObjectRoot, idOffsetTable, nFilePos, fp);

	// 写映射表
	fwrite(&idOffsetTable[0], 1, idOffsetTable.size()*sizeof(ID_OFFSET), fp);

	// 重写文件头
	fseek(fp, 0, SEEK_SET);
	bohd.dwMapOffset = nFilePos;
	bohd.dwObjectCount = idOffsetTable.size();
	fwrite(&bohd, 1, sizeof(BINOBJECTHEADER), fp);

	// 关闭文件
	fclose(fp);
	return TRUE;
}
//------------------------------------------------------------------------
IResObject* CResScript::GetResObjectRoot()
{
	return GetResObject(0);
}
//------------------------------------------------------------------------
// 访问指定ID的资源对象(内部通过哈希表快速访问)
IResObject* CResScript::GetResObject(UINT uID)
{
	if (uID == 0)
		return static_cast<IResObject*>(m_pResObjectRoot);

	CResObject* pResObject = NULL;
	hash_map<UINT, CResObject*>::iterator it = m_HashMap.find(uID);
	if (it != m_HashMap.end())
	{
		pResObject = (*it).second;
		Assert(pResObject != NULL);
		return pResObject;
	}
	return NULL;
}
//------------------------------------------------------------------------
// 获得本脚本中所有对象总数
int CResScript::GetObjectCount()
{
	return m_nCount;
}
//------------------------------------------------------------------------
// 获得本脚本的文件名
LPCSTR CResScript::GetScriptFileName()
{
	if (m_pFileObject == NULL)
		return NULL;
	return m_pFileObject->GetFilePath();
}
//------------------------------------------------------------------------
// 释放资源脚本
BOOL CResScript::Release()
{
#ifdef _DEBUG
	char szbuf[256];
	lstrcpy(szbuf, "释放资源脚本 ['无文件'] 完成。");
	if (m_pFileObject)
		wsprintf(szbuf, "释放资源脚本 ['%s'] 完成\r\n", GetScriptFileName());
#endif

	Empty();

	if (!m_TypeList.empty())
	{
		hash_map<string, TYPEINFO*>::iterator it;
		for (it = m_TypeList.begin(); it != m_TypeList.end(); it ++)
			((*it).second)->Release();
		m_TypeList.clear();
	}

#ifdef _DEBUG
	OutputDebugString(szbuf);
#endif

	delete this;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResScript::AddToHashMap(UINT uID, IResObject* pResObject)
{
	Assert(pResObject != NULL);
	CResObject* pCResObject = static_cast<CResObject*>(pResObject);
	if (uID == 0)
		uID = NextMaxID();
#ifdef _DEBUG
	if (GetResObject(uID))
	{
		FKResTraceColorLn(MY_TRACE_COLOR, "ID冲突。ID = %d。", uID);
	}
#endif
	m_HashMap[uID] = pCResObject;
	pCResObject->SetID(uID);
	if (pCResObject->m_pResScript == NULL)
		pCResObject->m_pResScript = this;
	m_nCount ++;
	return pResObject != NULL;
}
//------------------------------------------------------------------------
BOOL CResScript::EraseFromHashMap(UINT uID)
{
	CResObject* pResObject = NULL;
	hash_map<UINT, CResObject*>::iterator it = m_HashMap.find(uID);
	if (it != m_HashMap.end())
	{
		pResObject = (*it).second;
		Assert(pResObject != NULL);
		m_HashMap.erase(uID);
		m_nCount --;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
// 写对象的脚本(递归函数)
BOOL CResScript::WriteResObject(IResObject* pResObject, FILE* fp)
{
	Assert(pResObject != NULL && fp != NULL);

	// 先填充Tab字符
	static char str[2148];
	static int nTabCount = -1;
	nTabCount ++;
	memset(str, '\t', nTabCount);

	int nObjNum = pResObject->GetSubObjectCount();

	for (int num = 0; num < nObjNum; num ++)
	{
		IResObject* pSubObject = pResObject->GetSubObjectByIndex(num);
		if (!pSubObject)
		{
			FKResTraceColorLn(MY_TRACE_COLOR, 
				"错误： CResScript::WriteResObject() 出现异常。对象 %s 的编号为 NO.%d 的子对象不存在！", 
				pResObject->GetObjectName(), num + 1, pResObject->GetID());
			return FALSE;
		}

		// 写对象描述
		if (lstrlen(pSubObject->GetTypeName()) > 0)
			wsprintf(str + nTabCount, "Object %s:%s = %d\n", pSubObject->GetObjectName(), pSubObject->GetTypeName(), pSubObject->GetID());
		else
			wsprintf(str + nTabCount, "Object %s = %d\n", pSubObject->GetObjectName(), pSubObject->GetID());
		fprintf(fp,"\n");
		fwrite(str, lstrlen(str), 1, fp); // Object ...

		wsprintf(str + nTabCount, "{\n");
		fwrite(str, lstrlen(str), 1, fp); // 写"{"

		// 写属性
		int nCount = pSubObject->GetPropertyCount();
		for (int i = 0; i < nCount; i++)
		{
			sprintf(str + nTabCount, "%c%s = %s\n", '\t', pSubObject->GetPropertyName(i), pSubObject->GetPropertyValue(i));
			fwrite(str, lstrlen(str), 1, fp); // ... = ...
		}


		if (pSubObject->GetSubObjectCount() > 0) // 写子对象
		{
			WriteResObject(pSubObject, fp);
		}

		wsprintf(str + nTabCount, "}\n");
		fwrite(str, lstrlen(str), 1, fp); // 写"}"
	}
	nTabCount --;

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResScript::WriteBinObject(IResObject* pResObject, vector<ID_OFFSET>& idOffsetTable, int& nFilePos, FILE* fp)
{
	Assert(pResObject != NULL && fp != NULL);
	//dt_undefine,dt_bool,dt_char,dt_short,dt_int,dt_float,dt_byte,dt_word,dt_dword,dt_string
	static int TypeBytes[] = {0,1,1,2,4,4,1,2,4,0};

	int nObjNum = pResObject->GetSubObjectCount();

	for (int num = 0; num < nObjNum; num ++)
	{
		IResObject* pSubObject = pResObject->GetSubObjectByIndex(num);

		BINOBJECTINFO oi;
		TYPEINFO* pTypeInfo;
		PROPINFO pi;
		CBitTool bt;

		// 获得本对象的类型
		string strType = pSubObject->GetTypeName();
		hash_map<string, TYPEINFO*>::iterator it = m_TypeList.find(_make_lower(strType));
		if (it != m_TypeList.end())
		{
			pTypeInfo = (*it).second;
		}
		else // 本对象的类型模板不存在或没有定义，略过!
		{
			FKResTraceColorLn(MY_TRACE_COLOR, "Ignore object[%d]: %s[%s]", pSubObject->GetID(), pSubObject->GetObjectName(), pSubObject->GetTypeName());
			if (pSubObject->GetSubObjectCount() > 0) // 写子对象
				WriteBinObject(pSubObject, idOffsetTable, nFilePos, fp);
			continue;
		}

		// 设置对象的类型作为标识
		oi.cbIdentify = pTypeInfo->cbID;

		// 结构长度
		// oi.wSize = sizeof(BINOBJECTINFO);

		// 不定长数据偏移
		oi.wOffset = pTypeInfo->GetFixPropertyLength() + pTypeInfo->GetFixArrayLength();

		// 获得子对象数目和子对象列表的ID
		vector<DWORD> vSubObjectIDList;
		if (pSubObject->GetSubObjectCount() > 0)
		{
			oi.wSubObjectCount = pSubObject->GetSubObjectCount();
			//oi.wSize += oi.wSubObjectCount*sizeof(DWORD);
			for (int i = 0; i < oi.wSubObjectCount; i++)
				vSubObjectIDList.push_back(pSubObject->GetSubObjectByIndex(i)->GetID());
		}
		else
			oi.wSubObjectCount = 0;


		// 获得不定长的属性数目及各不定长属性的长度
		vector<PROPINFO> vProp = pTypeInfo->prop;
		vector<WORD>	vUnFixList;
		for (int j=0;j<vProp.size();j++)
		{
			pi = vProp[j];
			if (pi.IsUnFixProperty())
			{
				oi.cbUnFixCount ++;
				WORD nLen = 0;
				if (pi.type == dt_string) // 包括一个'\0'
				{
					LPCSTR scr = pSubObject->GetPropertyString(pi.propname.c_str());
					if (scr != NULL)
						nLen =1 + (WORD)(lstrlen(scr));
					else // 属性不存在，则置长度为0
						nLen = (WORD)0;
				}
				else
				{
					nLen =(WORD)pSubObject->GetArrayLong(pi.propname.c_str(), NULL);
					nLen *= TypeBytes[pi.type];
				}
				vUnFixList.push_back(nLen);
			}
		}
		//oi.wSize += oi.cbUnFixCount*sizeof(WORD);

		for(int k = 0; k < vProp.size(); k++) // 每个属性
		{
			PROPINFO pi = vProp[k];
			if (pi.IsFixProperty()) // 是定长属性
			{
				switch(pi.type)
				{
				case dt_undefine:
					FKResTraceColorLn(MY_TRACE_COLOR,"CResScript::WriteBinObject() catch error: undefine type name[\"dt_undefine\"]!");
					return FALSE;
				case dt_bool:
				case dt_char:
				case dt_short:
				case dt_int:
				case dt_byte:
				case dt_word:
				case dt_dword:
					{
						long lg;
						if (!pSubObject->GetPropertyLong(pi.propname.c_str(), lg))
							lg = 0;
						bt.Push(pi.type, &lg, pi.length);
					}
					break;
				case dt_float:
					{
						double fl;
						if (!pSubObject->GetPropertyFloat(pi.propname.c_str(), fl))
							fl = 0.0f;
						float f = (float)fl;
						bt.PushFloat(&f);
					}
					break;
				}
			}
			else if (pi.IsFixArray()) // 定长数组
			{
				switch(pi.type)
				{
				case dt_undefine:
					FKResTraceColorLn(MY_TRACE_COLOR,"CResScript::WriteBinObject() catch error: undefine type name[\"dt_undefine\"]!");
					return FALSE;
				case dt_bool:
				case dt_char:
				case dt_byte:
					{
						int array_len = pSubObject->GetArrayByte(pi.propname.c_str(), NULL);
						BYTE* data = NULL;
						if (array_len == 0) // 不存在该属性,补上
						{
							array_len = pi.GetLength();
							data = new BYTE[array_len];
							Assert(data);
							memset(data, 0, array_len);
						}
						else
						{
							data = new BYTE[array_len];
							Assert(data);
							memset(data, 0, array_len);
							pSubObject->GetArrayByte(pi.propname.c_str(), data);
						}
						bt.PushArray(pi.type, (void*)data, array_len);
						delete[] data;
					}
					break;
				case dt_short:
				case dt_word:
				case dt_int:
				case dt_dword:
					{
						int array_len = pSubObject->GetArrayLong(pi.propname.c_str(), NULL);
						long* data = NULL;
						if (array_len == 0) // 不存在该属性,补上
						{
							array_len = pi.GetLength();
							data = new long[array_len];
							Assert(data);
							memset(data, 0, array_len*sizeof(long));
						}
						else
						{
							data = new long[array_len];
							Assert(data);
							pSubObject->GetArrayLong(pi.propname.c_str(), data);
						}
						bt.PushArray(pi.type, (void*)data, array_len);
						delete[] data;
					}
					break;
				case dt_float:
					{
						int array_len = pSubObject->GetArrayFloat(pi.propname.c_str(), NULL);
						double* data = NULL;
						if (array_len == 0) // 不存在该属性,补上
						{
							array_len = pi.GetLength();
							data = new double[array_len];
							Assert(data);
							memset(data, 0, sizeof(data));
						}
						else
						{
							data = new double[array_len];
							Assert(data);
							pSubObject->GetArrayFloat(pi.propname.c_str(), data);
						}
						bt.PushArray(pi.type, (void*)data, array_len);
						delete[] data;
					}
					break;
				}
			}
			else if (pi.IsUnFixProperty()) // 不定长属性
			{
				if (pi.type == dt_string)
				{
					LPCSTR src = pSubObject->GetPropertyString(pi.propname.c_str());
					if (src != NULL)
						bt.PushString(src);
				}
				else
				{
					int nSize = 0;
					if (pi.type == dt_float) // 浮点型
					{
						nSize =pSubObject->GetArrayFloat(pi.propname.c_str(), NULL);
						if (nSize != 0)
						{
							double* pUnFixData = new double[nSize];
							Assert(pUnFixData);
							pSubObject->GetArrayFloat(pi.propname.c_str(), pUnFixData);
							bt.PushArray(pi.type, (void*)pUnFixData, nSize);
							delete[] pUnFixData;
						}
					}
					else
					{
						nSize =pSubObject->GetArrayLong(pi.propname.c_str(), NULL);
						if (nSize != 0)
						{
							long* pUnFixData = new long[nSize];
							Assert(pUnFixData);
							pSubObject->GetArrayLong(pi.propname.c_str(), pUnFixData);
							bt.PushArray(pi.type, (void*)pUnFixData, nSize);
							delete[] pUnFixData;
						}
					}
				}
			}
			else
				FKResTraceColorLn(MY_TRACE_COLOR, 
				"错误： CResScript::WriteBinObject() 出现异常。无法识别的属性！");
		}

		oi.wSize = bt.GetDataLength();

		// 写结构
		fwrite(&oi, 1, sizeof(BINOBJECTINFO), fp);
		// 写子对象列表
		if (!vSubObjectIDList.empty())
			fwrite(&vSubObjectIDList[0], 1, vSubObjectIDList.size()*sizeof(DWORD),fp);
		// 写不定长数据长度列表
		if (!vUnFixList.empty())
			fwrite(&vUnFixList[0], 1, vUnFixList.size()*sizeof(WORD),fp);
		// 写用户数据
		if (bt.GetDataLength())
		{
			// _data_xor((BYTE*)bt.GetDataBuffer(), bt.GetDataLength());
			fwrite(bt.GetDataBuffer(), 1, bt.GetDataLength(), fp);
		}

		ID_OFFSET idmap;
		idmap.id = pSubObject->GetID();
		idmap.offset = nFilePos;
		idOffsetTable.push_back(idmap);
		nFilePos += sizeof(BINOBJECTINFO) + 
			oi.wSubObjectCount*sizeof(DWORD) + oi.cbUnFixCount*sizeof(WORD) + oi.wSize;

		if (pSubObject->GetSubObjectCount() > 0) // 写子对象
			WriteBinObject(pSubObject, idOffsetTable, nFilePos, fp);
	}

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResScript::WriteRootObject(IResObject* pResObject, vector<ID_OFFSET>& idOffsetTable, int& nFilePos, FILE* fp)
{
	BINOBJECTINFO oi;
	CBitTool bt;
	oi.cbIdentify = 0;
	oi.cbUnFixCount = 0;

	// 获得子对象数目和子对象列表的ID
	vector<DWORD> vSubObjectIDList;
	if (pResObject->GetSubObjectCount() > 0)
	{
		oi.wSubObjectCount = pResObject->GetSubObjectCount();
		for (int i = 0; i < oi.wSubObjectCount; i++)
			vSubObjectIDList.push_back(pResObject->GetSubObjectByIndex(i)->GetID());
	}
	else
		oi.wSubObjectCount = 0;

	long lg;
	if (!pResObject->GetPropertyLong("MaxID", lg))
		lg = 0;
	bt.Push(dt_int, &lg, 32);

	oi.wSize = bt.GetDataLength();
	oi.wOffset = oi.wSize;

	// 写结构
	fwrite(&oi, 1, sizeof(BINOBJECTINFO), fp);
	// 写子对象列表
	if (!vSubObjectIDList.empty())
		fwrite(&vSubObjectIDList[0], 1, vSubObjectIDList.size()*sizeof(DWORD),fp);
	// 写用户数据
	if (bt.GetDataLength())
	{
		//_data_xor((BYTE*)bt.GetDataBuffer(), bt.GetDataLength());
		fwrite(bt.GetDataBuffer(), 1, bt.GetDataLength(), fp);
	}

	ID_OFFSET idmap;
	idmap.id = 0;
	idmap.offset = nFilePos;
	idOffsetTable.push_back(idmap);
	nFilePos += sizeof(BINOBJECTINFO) + 
		oi.wSubObjectCount*sizeof(DWORD) + oi.cbUnFixCount*sizeof(WORD) + oi.wSize;
	return TRUE;
}
//------------------------------------------------------------------------
// 获得当前最大ID号
UINT CResScript::GetMaxID()
{
	long lCurMaxID;
	m_pResObjectRoot->GetPropertyLong("MaxID", lCurMaxID);
	return lCurMaxID;
}
//------------------------------------------------------------------------
// 搜索res文件,返回指定ID的对象的脚本
BOOL CResScript::ParseObjectServer(UINT uID, char* pData)
{
	CResDecoder ResDecoder(this, m_pResObjectRoot);

	if (!ResDecoder.ParseObjectServer(uID, pData))
		return FALSE;

	return TRUE;
}
//------------------------------------------------------------------------
// 由某段字符串构建对象
IResObject* CResScript::ParseObjectClient(char* pData, int nSize)
{
	CResDecoder ResDecoder(this, m_pResObjectRoot);

	IResObject* pResObject = ResDecoder.ParseObjectClient(pData, nSize);
	Assert(pResObject);
	return pResObject;
}
//------------------------------------------------------------------------
// 设置当前最大ID号
UINT CResScript::NextMaxID()
{
	UINT uID = GetMaxID();
	uID ++;
	Assert(m_pResObjectRoot != NULL);
	m_pResObjectRoot->SetPropertyLong("MaxID", uID);
	return uID;
}
//------------------------------------------------------------------------
void CResScript::Empty()
{

	SAFE_RELEASE(m_pResObjectRoot);

	m_nCount = 0;

	if (m_bReleaseFileObject)
	{
		SAFE_RELEASE(m_pFileObject);
	}

	if (!m_HashMap.empty())
		m_HashMap.clear();
}
//------------------------------------------------------------------------
#ifdef _DEBUG
BOOL CResScript::Dump()
{
	Assert(m_pResObjectRoot);
	DebugMsgLn("\r\n------------- 打印整个脚本信息开始 -------------");
	DebugMsgLn("【脚本信息】脚本文件 = \"%s\" 对象总数 = %d", this->GetScriptFileName(), this->GetObjectCount());
	m_pResObjectRoot->Dump();
	DebugMsgLn("------------- 打印整个脚本信息结束 -------------\r\n");
	return TRUE;
}
#endif
//------------------------------------------------------------------------