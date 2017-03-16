/**
*	created:		2012-6-18   20:01
*	filename: 		BinObj
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/ResMgrCommon.h"
#include "../Include/BinObj.h"
//------------------------------------------------------------------------
CBinObject::CBinObject()
{
	m_ObjectBuffer = NULL;
	if (!m_SubObjectList.empty())
		m_SubObjectList.clear();
	if (!m_UnFixSizeList.empty())
		m_UnFixSizeList.clear();
}
//------------------------------------------------------------------------
CBinObject::~CBinObject()
{
}
//------------------------------------------------------------------------
int CBinObject::GetUnFixPropertyOffset(int nIndexEx)
{
	VerifyR((nIndexEx < 0 && nIndexEx >= m_UnFixSizeList.size()), -1, "nIndexEx overflow!");

	if (nIndexEx == 0)
		return  m_wOffset;

	int nLen = m_wOffset;
	for (int i=0; i<nIndexEx; i++)
		nLen += GetUnFixPropertyLength(i);

	return nLen;
}
//------------------------------------------------------------------------
int CBinObject::GetUnFixPropertyLength(int nIndexEx)
{
	VerifyR((nIndexEx < 0 && nIndexEx >= m_UnFixSizeList.size()), -1, "nIndexEx overflow!");

	if (m_UnFixSizeList.empty())
		return 0;
	return m_UnFixSizeList[nIndexEx];
}
//------------------------------------------------------------------------
UINT CBinObject::GetSubObject(int nIndex)
{
	Assert(nIndex >= 0 && nIndex < m_SubObjectList.size());

	return m_SubObjectList[nIndex];;
}
//------------------------------------------------------------------------
// 只释放自己，因为他看不到别的对象!
BOOL CBinObject::Release()
{
	if (m_ObjectBuffer)
	{
		delete[] m_ObjectBuffer;
		m_ObjectBuffer = NULL;
	}
	if (!m_SubObjectList.empty())
		m_SubObjectList.clear();
	if (!m_UnFixSizeList.empty())
		m_UnFixSizeList.clear();

	delete this;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CBinObject::SetIDList(UINT* pIDList, int nSize)
{
	Assert(pIDList != NULL && nSize != 0)

		for(int i=0; i<nSize/sizeof(UINT); i++)
			m_SubObjectList.push_back(pIDList[i]);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CBinObject::SetUnFixSizeList(WORD* pSizeList, int nSize)
{
	Assert(pSizeList != NULL && nSize != 0);

	for(int i=0; i<nSize/sizeof(WORD); i++)
		m_UnFixSizeList.push_back(pSizeList[i]);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CBinObject::SetObjectBuffer(BYTE* pData, int nSize)
{
	Assert(pData != NULL && nSize != 0);

	m_ObjectBuffer = new BYTE[nSize];
	Assert(m_ObjectBuffer);
	memcpy(m_ObjectBuffer, pData, nSize);
	return TRUE;
}
//------------------------------------------------------------------------