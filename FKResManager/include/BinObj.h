/**
*	created:		2012-6-18   20:00
*	filename: 		BinObj
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CBinObject
{
private:
	WORD				m_wSize;		// 数据缓冲的大小
	WORD				m_wOffset;		// 不定长数据的偏移
	BYTE*				m_ObjectBuffer; // 对象缓冲
	vector<DWORD>		m_SubObjectList;// 子对象列表
	vector<WORD>		m_UnFixSizeList;// 不定长属性长度列表

public:
	CBinObject();
	~CBinObject();

	BYTE* GetBuffer()				{return m_ObjectBuffer;}
	DWORD GetSize()					{return m_wSize;}
	DWORD GetSubObjectCount()		{return m_SubObjectList.size();}
	DWORD GetUnFixPropertyCount()	{return m_UnFixSizeList.size();}

	int   GetUnFixPropertyOffset(int nIndexEx);
	int   GetUnFixPropertyLength(int nIndexEx);
	UINT  GetSubObject(int nIndex);
	BOOL  Release();

	BOOL SetSize(int nSize)	{return (m_wSize=nSize);}
	BOOL SetUnFixPropertyOffset(int nOffset) {return (m_wOffset=nOffset);}
	BOOL SetIDList(UINT* pIDList, int nSize);
	BOOL SetUnFixSizeList(WORD* pSizeList, int nSize);
	BOOL SetObjectBuffer(BYTE* pData, int nSize);
};
//------------------------------------------------------------------------