/**
*	created:		2012-7-4   20:32
*	filename: 		CCsvToScp
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		CSVToSCP类用于把*.csv格式的文件转化为*.scp格式的文件
*/
//------------------------------------------------------------------------
#include "StdAfx.h"
#include "CCSVToSCP.h"
//------------------------------------------------------------------------
TCHAR wchBuf[MAX_STRING_LEN];
TCHAR wchObjectName[MAX_STRING_LEN];
//------------------------------------------------------------------------
CCSVToSCP::CCSVToSCP()
{
}
//------------------------------------------------------------------------
CCSVToSCP::~CCSVToSCP()
{
}
//------------------------------------------------------------------------
BOOL CCSVToSCP::MakeCSVToSCP(LPCTSTR szCSVFileName, LPCTSTR szSCPFileName, LPTSTR szErrMsg, BOOL bPlus2Comma)
{
	ifstream inCsvFile(szCSVFileName, ios::in);
	if ( !inCsvFile )
	{
		sprintf(szErrMsg, "转换失败！\n打开CSV文件失败！提示，CSV文件不可为中文！");
		return FALSE;
	}

	ofstream outSCPFile(szSCPFileName, ios::out);
	if ( !outSCPFile )
	{
		sprintf(szErrMsg, "转换失败！\n创建FK1文件失败！");
		inCsvFile.close();
		return FALSE;
	}

	inCsvFile.getline(wchBuf, sizeof(wchBuf));
	if ( strlen(wchBuf) == 0 )
	{
		sprintf(szErrMsg, "转换失败！\nCSV文件第一行为空！");
		outSCPFile.close();
		inCsvFile.close();
		return FALSE;
	}

	CString cstrFirstRow[256];
	int nQueue = SaveFirstRow(wchBuf, strlen(wchBuf), cstrFirstRow);
	int nLen = PrintConstStr(outSCPFile);				// 输出头标示

	inCsvFile.getline(wchBuf, sizeof(wchBuf));

	int nResObjectLevel[MAX_OBJECT_LEVEL]= {0};
	int nCurResObject = 0;
	int nMaxID = 1;
	int nCurID = 1;
	vector<int> nVectorID;

	BOOL bFirst = TRUE;
	int nOldLevel = 0;
	
	while(1)
	{
		memset(wchBuf, 0, sizeof(wchBuf));
		inCsvFile.get(wchBuf, 4, ',');
		int nNewLevel = 0;
		nNewLevel = atoi(wchBuf);
		if ( 0 == nNewLevel)
		{
			if(( inCsvFile.eof() != 0 ) || ( inCsvFile.bad() != 0 ) || ( inCsvFile.fail() != 0 ))
			{
				break;
			}
			
			inCsvFile.getline(wchBuf, sizeof(wchBuf));
			continue;
		}

		nNewLevel = (nNewLevel > 0) ? nNewLevel : 1;

		if ( TRUE == bFirst)
		{
			nCurResObject++;
			bFirst = FALSE;
		}
		else
			if (nNewLevel > nOldLevel)
			{
				nCurResObject++;
				nResObjectLevel[nCurResObject] = nResObjectLevel[nCurResObject - 1] + (nNewLevel - nOldLevel);
				outSCPFile << "\n";
			}
			else
				if (nNewLevel < nOldLevel)
				{
					for (int i = nOldLevel; i >= nNewLevel && nCurResObject >= 1; i--)
					{
						PrintTab(outSCPFile, nResObjectLevel[nCurResObject]);
						outSCPFile << "}\n";
						nCurResObject--;
					}

					outSCPFile << "\n";
					nCurResObject++;
				}
				else
				{
					PrintTab(outSCPFile, nResObjectLevel[nCurResObject]);
					outSCPFile << "}\n\n";
				}
		nOldLevel = nNewLevel;

		PrintTab(outSCPFile, nResObjectLevel[nCurResObject]);
		outSCPFile << "Object ";
		inCsvFile.getline(wchBuf, sizeof(wchBuf), ',');
		inCsvFile.get(wchBuf, sizeof(wchBuf), ',');
		outSCPFile << wchBuf;
		strcpy(wchObjectName, wchBuf);
		outSCPFile << " = ";
		inCsvFile.get();
		inCsvFile.get(wchBuf, sizeof(wchBuf), ',');
		if ( strlen(wchBuf) > 0)
		{
			nCurID = (atoi(wchBuf) > 0) ? atoi(wchBuf) : nCurID;
			//判断ID是否已用过
			if ( !IsIDUsed(nVectorID, nCurID) )
			{
				sprintf(szErrMsg, "转换失败！\n有重复的ID：%d", nCurID);
				outSCPFile.close();
				inCsvFile.close();

				ofstream outFileEmpty(szSCPFileName, ios::out);
				outFileEmpty.close();
				return FALSE;
			}

			vector<int>::iterator it = nVectorID.end();
			nVectorID.insert(it, nCurID);
		}
		nMaxID = (nCurID > nMaxID) ? nCurID : nMaxID;
		outSCPFile << nCurID++;		
		outSCPFile << "\n";

		PrintTab(outSCPFile, nResObjectLevel[nCurResObject]);
		outSCPFile << "{\n";

		for (int i = 3; i < nQueue; i++)
		{	
			inCsvFile.get();	//读取','
			
			if ( i == (nQueue - 1) )
			{
				inCsvFile.get(wchBuf, sizeof(wchBuf), '\n');
			}
			else
			{
				inCsvFile.get(wchBuf, sizeof(wchBuf), ',');
				if ( strrchr(wchBuf, '\n') )
				{
					sprintf(szErrMsg, "转换失败！\n下面Object所在的行缺少一些列:\nObject ＝ %s ", wchObjectName);
					outSCPFile.close();
					inCsvFile.close();

					ofstream outFileEmpty(szSCPFileName, ios::out);
					outFileEmpty.close();
					return FALSE;
				}				
			}

			if ( strlen(wchBuf) > 0)
			{
				//将+转为,
				if (TRUE == bPlus2Comma)
				{
					PlusToComma(wchBuf, strlen(wchBuf));
				}

				PrintTab(outSCPFile, nResObjectLevel[nCurResObject]);
				outSCPFile << "\t";
				outSCPFile << cstrFirstRow[i];
				outSCPFile << " = ";
				outSCPFile << wchBuf;
				outSCPFile << "\n";
			}						
		}		
	}

	nVectorID.clear();

	while (nCurResObject >= 1)
	{
		PrintTab(outSCPFile, nResObjectLevel[nCurResObject]);
		outSCPFile << "}\n";
		nCurResObject--;
	}

	outSCPFile.seekp(nLen + 5, ios::beg);
	outSCPFile << nMaxID;

	outSCPFile.close();
	inCsvFile.close();

	return TRUE;
}
//------------------------------------------------------------------------
int CCSVToSCP::SaveFirstRow(TCHAR *pwchBuf, int nBufLen, CString *pcstrFirstRow)
{
	CString cstrTemp;
	int nQueue = 0;
	for (int i = 0; i < nBufLen; i++)
	{
		if (pwchBuf[i] != ',')
		{
			if (pwchBuf[i] != ' ')
			{
				cstrTemp += pwchBuf[i];
			}			
		}
		else
		{
			pcstrFirstRow[nQueue++] = cstrTemp;
			cstrTemp.Empty();
		}
	}
	pcstrFirstRow[nQueue++] = cstrTemp;
	return nQueue;
}
//------------------------------------------------------------------------
BOOL CCSVToSCP::IsIDUsed(vector<int> &nVectorID, int nCurID)
{
	for (vector<int>::iterator it = nVectorID.begin(); it != nVectorID.end(); it++)
	{
		if (nCurID == *it)
		{
			return FALSE;
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------
int CCSVToSCP::PrintConstStr(ofstream &outSCPFile)
{
	int ntemp = 0;
	TCHAR tchConstStr[] = {"//--------------- FK1资源脚本 From CSV ---------------\n//--------------- 每行最大长度2048字节 ---------------\n// 最大ID为 = "};
	outSCPFile << tchConstStr;
	outSCPFile << "        \n";

	return sizeof(tchConstStr);
}
//------------------------------------------------------------------------
void CCSVToSCP::PrintTab(ofstream &outSCPFile, int nNum)
{
	for ( int i = 1; i <= nNum; i++)
	{
		outSCPFile << '\t';
	}
}
//------------------------------------------------------------------------
void CCSVToSCP::PlusToComma(TCHAR *pwchBuf, int nBufLen)
{
	for (int i = 0; i < nBufLen; i++)
	{
		if ( '+' == pwchBuf[i] )
		{
			wchBuf[i] = ',';
		}
	}
}
//------------------------------------------------------------------------