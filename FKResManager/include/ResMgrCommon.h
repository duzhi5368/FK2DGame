/**
*	created:		2012-6-18   19:42
*	filename: 		ResMgrCommon
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#ifdef _DEBUG
#define __USE_MALLOC
#endif
//------------------------------------------------------------------------
#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <vector>
#include <hash_map>
#include <queue>
#include <Mmsystem.h>
//------------------------------------------------------------------------
#include "../../FKTrace/IFKTrace.h"
#include "../../FKFileSystem/IFKFileSystem.h"
#include "../IFKResManager.h"
//------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------
// 常用宏定义
#ifndef _DEBUG
	#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#else
	#define SAFE_RELEASE(p)      { if(p) { try {(p)->Release();}catch(...){}; (p)=NULL; } }
#endif

#define SAFE_DELETE(p)		 { if(p) { delete (p); (p)=NULL; } }

#if !defined(ASSERT)
	#if defined(_DEBUG)
		#define ASSERT(f)			{if (!(f))__asm int 3}
	#else
		#define ASSERT(f)			((void*)0)
	#endif
#endif
//------------------------------------------------------------------------
#define FKResTraceColor		g_DefaultTrace.ThreadSafeDebugPrintColor
#define	FKResTraceColorLn	g_DefaultTrace.ThreadSafeDebugPrintColorLn
#define	DebugMsgLn			g_DefaultTrace.ThreadSafeDebugPrintLn
//------------------------------------------------------------------------
#ifndef	MY_TRACE_COLOR
	#define MY_TRACE_COLOR		RGB(146,0,123)
#endif
//------------------------------------------------------------------------
#ifdef TraceLn
	#undef TraceLn
#endif
#define TraceLn	DebugPrintLn
void DebugPrintLn(LPCSTR lpszFormat, ...);
//------------------------------------------------------------------------
#ifdef _DEBUG
	#define VerifyR(Expression, ReturnValue, Description ) \
		if (Expression) \
	{\
		FKResTraceColor(MY_TRACE_COLOR, "错误 : %s[%d]\r\n\t%s\r\n",__FILE__,__LINE__,Description);\
		return ReturnValue;\
	}
	#define Assert(Expression) \
		if (!(Expression))\
	{\
		FKResTraceColor(MY_TRACE_COLOR, "错误 : %s[%d]\r\n\t%s\r\n",__FILE__,__LINE__,#Expression);\
		return 0;\
	}
#else
	#define VerifyR(Expression, ReturnValue, Description ) \
		if (Expression) \
	{\
		return ReturnValue;\
	}
	#define Assert(Expression) \
		if (!(Expression))\
	{\
		return 0;\
	}
#endif
//------------------------------------------------------------------------
class CResMgr;
class CResScript;
class CResObject;
class CResDecoder;
class CBitTool;
class CBinObject;
class CBinScript;
//------------------------------------------------------------------------
extern void*		_data_xor(BYTE* buf, int nLen);
extern IFileObject* _create_fileobject(LPCSTR szFileName);
// 一些有关字符串操作的全局函数
extern int  		_find_string(string& str, string& str1, int nStart = 0);
extern int  		_find_string_no_case(string str, string str1, int nStart = 0);
extern int  		_find_char(string& str, char c, int nStart = 0);
extern int  		_find_char_no_case(string str, char c, int nStart = 0);
extern const char* 	_make_lower(string& str);
extern const char* 	_trim_left(string& str);
extern const char* 	_trim_right(string& str);
extern const char* 	_trim(string& str);
extern const char* 	_erase_left(string& str, int nSize);
extern const char* 	_erase_right(string& str, int nSize);
extern const char* 	_mid(string& str, int nStart, int nEnd);
extern const char* 	_mid_len(string& str, int nStart, int nLen);
extern const char* 	_left(string& str, int nSize);
extern const char* 	_right(string& str, int nSize);
extern const char* 	_left_c(string& str, char c);			// 变换为某个字符左边的串
extern const char* 	_right_c(string& str, char c);			// 变换为某个字符右边的串
extern const char* 	_file_path(string& str);
extern const char* 	_file_name(string& str);
extern const char* 	_file_title(string& str);
extern const char* 	_file_ext(string& str);
//------------------------------------------------------------------------
// 脚本语句行的类型定义
enum LINETYPE
{
	LT_UNDEFINE=0,	// 表示此行是未知的信息行
	LT_EMPTY,		// 表示此行是空行
	LT_COMMENT,		// 表示此行是注释行
	LT_MACRO,		// 表示宏定义
	LT_LBRACKET,	// 表示此行是左大括号，即"{"
	LT_RBRACKET,	// 表示此行是右大括号，即"}"

	LT_OBJECT,		// 表示此行是对象声名行，如Object objname
	LT_PROPERTY,	// 表示此行是属性设置
};
//------------------------------------------------------------------------
// 资源对象信息
typedef struct _RESOBJECTINFO
{
	string	strName;
	string	strType;
	UINT	uID;
	UINT	uParentID;
	_RESOBJECTINFO()		{strName.clear();strType.clear();uID=uParentID=0;}
	~_RESOBJECTINFO()		{strName.clear();strType.clear();}
} RESOBJECTINFO;
//------------------------------------------------------------------------
// 类型模板中所支持的数据类型
enum DATATYPE
{
	dt_undefine=0,
	dt_bool,
	dt_char,
	dt_short,
	dt_int,
	dt_float,
	dt_byte,
	dt_word,
	dt_dword,
	dt_string
};
//------------------------------------------------------------------------
// ID和Offset的映射
typedef struct _ID_OFFSET
{
	UINT id;
	DWORD offset;
} ID_OFFSET;
//------------------------------------------------------------------------
// 类型模板中的属性信息
typedef struct _PROPINFO
{
	DATATYPE type;						// 类型
	string	 propname;					// 属性名
	int		 length;					// 属性的域长，是数组则为数组长度
	bool	 isArray;					// 是否是数组

	_PROPINFO()						{length=0;type=dt_undefine;isArray=false;propname.clear();}
	void Release()					{delete this;}
	bool IsFixProperty()			{return ((length>0)&&(isArray==false));}		// 是固定长度的变量属性
	bool IsFixArray()				{return ((length>0)&&(isArray==true));}			// 是固定长度的数组属性
	bool IsUnFixProperty()			{return ((length==0)&&(isArray==true));}		// 是不定长度的数据
	int  GetLength()				{return length;}
} PROPINFO;	
//------------------------------------------------------------------------
// 类型模板信息
typedef struct _TYPEINFO
{
	string				name;			// 类型名
	BYTE				cbID;			// 类型ID
	vector<PROPINFO>	prop;			// 属性数组

	_TYPEINFO(){cbID=0;name.clear();prop.clear();}
	void Release()
	{
		if (!prop.empty()) prop.clear();
		delete this;
	}
	BYTE GetTypeID() {return cbID;}
	int GetFixPropertyLength()
	{
		int bitlen=0,bytelen=0;
		for(int i=0;i<prop.size();i++)
			if(prop[i].IsFixProperty())
				bitlen+=prop[i].GetLength();

		bytelen=bitlen>>3;
		if (bitlen&7)
			bytelen+=1;
		return bytelen;
	}
	int GetFixArrayLength()
	{
		static int bytes[] = {0,1,1,2,4,4,1,2,4,0};
		int arraylen=0;
		for(int i=0;i<prop.size();i++)
			if(prop[i].IsFixArray())
				arraylen+=prop[i].GetLength()*bytes[prop[i].type];
		return arraylen;
	}
	int GetUnFixPropertyCount()
	{
		int unfix_prop_count=0;
		for(int i=0;i<prop.size();i++)
			if(prop[i].IsUnFixProperty())
				unfix_prop_count++;
		return unfix_prop_count;
	}
} TYPEINFO;
//------------------------------------------------------------------------
// 二进制对象信息
typedef struct _BINOBJECTINFO	
{
	BYTE	cbIdentify;		// 标识
	BYTE	cbVersion;		// 版本号
	WORD	wSize;			// 数据缓冲的大小
	WORD	wOffset;		// 不定长数据的偏移
	WORD	wSubObjectCount;// 子对象数目( < 65535)
	BYTE	cbUnFixCount;	// 不定长属性的数目

	_BINOBJECTINFO()				{memset(this,0,sizeof(_BINOBJECTINFO));cbIdentify=255;cbVersion=1;}
	int GetSize()					{return (int)wSize;}
	int GetUnFixPropertyOffset()	{return (int)wOffset;}
	int GetSubObjectCount()			{return (int)wSubObjectCount;}
	int GetUnFixPropertyCount()		{return (int)cbUnFixCount;}
	BOOL IsValid()					{return ((cbIdentify!=255)&&(cbVersion==1));}
} BINOBJECTINFO;
//------------------------------------------------------------------------
// 二进制对象文件头
typedef struct _BINOBJECTHEADER
{
	DWORD	dwIdentify;		// 标识
	DWORD	dwObjectCount;	// 对象总数
	DWORD	dwMapOffset;	// 映射表偏移
	_BINOBJECTHEADER()				{memset(this,0,sizeof(_BINOBJECTHEADER));dwIdentify= 'DHOB';}//'IBKF';}
	BOOL IsValid()					{return (dwIdentify=='DHOB');}//'IBKF');}
} BINOBJECTHEADER;
//------------------------------------------------------------------------