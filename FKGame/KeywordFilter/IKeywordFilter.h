/**
*	created:		2012-6-29   14:49
*	filename: 		IKeywordFilter
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		关键字过滤
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// 关键字的级别，作为过滤时传入的要求等级
enum Keyword_Option
{
	enKL_Replace = 0,		// 替换
	enKL_Deny = 1,			// 不允许
};
//------------------------------------------------------------------------
// 关键字过滤接口
struct IKeywordFilter
{
	// 销毁关键字过滤
	virtual void Release(void) = 0;

	/*
	 *	功能: 过滤,可提供替换字符,
			  如果需要得到替换关键字后的字符串,必须传入lpszOut以及dwOutSize
			  并且保证lpszOut已分配空间,否则可能导致内存访问非法。
		传入：lpszIn，要过滤的字符串
			  lLevelLow表示替换该级别和该级别以上的关键字，默认为0，表示过滤所有关键字
		返回：如果返回0表示允许通过，否则返回找到的最高级敏感关键字级别
			  lpszOut为已替换了关键字的字符串,若没有找到关键字,则为原字符串
			  dwOutSize为替换后字符串lpszOut的长度，最好为strlen(lpszIn)
			  如果长度不够则只输出长度范围内的字符
	 */
	virtual LONG Filter(LPCSTR lpszIn, LPSTR lpszOut = NULL,
		DWORD dwOutSize = 0, LONG lLevelLow = 0) = 0;

};
//------------------------------------------------------------------------
// 创建通用关键字过滤(输出函数)
extern "C" __declspec(dllexport) BOOL CreateKeywordFilter(IKeywordFilter **ppKeywordFilter);
typedef BOOL (__cdecl *ProcCreateKeywordFilter)(IKeywordFilter **ppKeywordFilter);
//------------------------------------------------------------------------
// 关键字过滤创建辅助类
class CKeywordFilterHelper
{
public:
	CKeywordFilterHelper()
	{
		m_hDll = NULL;
		m_pKeywordFilter = NULL;
	}
	
	~CKeywordFilterHelper()
	{
	}

	// 创建关键字过滤
	BOOL Create(void)
	{
		if (m_hDll != NULL)
		{
			return FALSE;
		}
		
		try
		{
#ifdef _DEBUG
			m_hDll = ::LoadLibrary("FKKeywordFilter_D.dll");
			if (NULL == m_hDll)
			{
				throw "无法加载 FKKeywordFilter_D.dll!";
			}
#else
			m_hDll = ::LoadLibrary("FKKeywordFilter.dll");
			if (NULL == m_hDll)
			{
				throw "无法加载 FKKeywordFilter.dll!";
			}
#endif
			
			ProcCreateKeywordFilter pProc = NULL;
			pProc = (ProcCreateKeywordFilter)::GetProcAddress(m_hDll, "CreateKeywordFilter");
			if (NULL == pProc)
			{
				throw "Cann't get process CreateKeywordFilter!";
			}
			
			if (FALSE == pProc(&m_pKeywordFilter))
			{
				throw "Create KeywordFilter error!";
			}
			
			return TRUE;
		}
		catch (LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			OutputDebugString("\r\n");
		}
		catch (...)
		{
			OutputDebugString("KeywordFilter unknow error!");
			OutputDebugString("\r\n");
		}
		
		return FALSE;
	}

	// 释放宠物模块
	void Close(void)
	{
		if (m_pKeywordFilter != NULL)
		{
			m_pKeywordFilter->Release();
			m_pKeywordFilter = NULL;
		}
		
		if (m_hDll != NULL)
		{
			::FreeLibrary(m_hDll);
			m_hDll = NULL;
		}
	}

public:	
	IKeywordFilter *m_pKeywordFilter;

private:
	HINSTANCE m_hDll;

};
//------------------------------------------------------------------------