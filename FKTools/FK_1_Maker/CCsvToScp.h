/**
*	created:		2012-7-4   20:31
*	filename: 		CCsvToScp
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <vector>
#include <fstream>
#include <list>
using namespace std;
//------------------------------------------------------------------------
#define MAX_STRING_LEN		2048
#define MAX_STRING_Name		256
#define MAX_OBJECT_LEVEL	128
//------------------------------------------------------------------------
class CCSVToSCP
{
public:
	CCSVToSCP();

	~CCSVToSCP();
	//将*.csv格式的文件转化为*.scp格式的文件
	BOOL	MakeCSVToSCP(LPCTSTR szCSVFileName, LPCTSTR szSCPFileName, LPTSTR szErrMsg, BOOL bPlus2Comma);

protected:
	int		SaveFirstRow(TCHAR *pwchBuf, int nBufLen, CString *pcstrFirstRow);
	BOOL	IsIDUsed(vector<int> &nVectorID, int nCurID);
	int		PrintConstStr(ofstream &outSCPFile);
	void	PrintTab(ofstream &outSCPFile, int nNum);
	void	PlusToComma(TCHAR *pwchBuf, int nBufLen);

private:
};
//------------------------------------------------------------------------