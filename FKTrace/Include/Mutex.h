/**
*	created:		2012-6-14   23:55
*	filename: 		Mutex
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CMutex
{
public:
	CMutex()
	{
		m_hObject = NULL;
	}
	~CMutex(){::CloseHandle(m_hObject);}
	BOOL Create()
	{
		m_hObject = ::CreateMutex(NULL, FALSE, NULL);
		return m_hObject != NULL;
	}
	BOOL Lock(DWORD dwTimeout = INFINITE)
	{
		if (m_hObject == NULL)
			return FALSE;

		if (::WaitForSingleObject(m_hObject, dwTimeout) == WAIT_OBJECT_0)
			return TRUE;
		else
			return FALSE;
	}
	BOOL Unlock()
	{
		if (m_hObject == NULL)
			return FALSE;
		return ::ReleaseMutex(m_hObject);
	}
public:
	HANDLE m_hObject;
};
//------------------------------------------------------------------------