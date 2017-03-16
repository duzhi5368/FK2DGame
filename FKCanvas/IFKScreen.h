/**
*	created:		2012-6-19   12:56
*	filename: 		IFKScreen
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
struct ICanvas;
struct ITrace;
//------------------------------------------------------------------------
struct IScreen
{
	virtual BOOL Create(HWND hwnd, int x, int y, int w, int h) = NULL;						//创建
	virtual ICanvas* GetCanvas() = NULL;
	virtual BOOL MessageHandle(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = NULL;	//消息处理
	virtual void KeepFrames(int frames) = NULL;												//保持稳定帧
	virtual void SetKeepFrames(BOOL b) = NULL;
	virtual void ShowFPS(BOOL b) = NULL;													//显示帧数

	virtual BOOL OnUpdate() = NULL;															//更新窗口
	virtual void Release() = NULL;															//释放结口
	virtual HWND GetHWnd() = NULL;
};
//------------------------------------------------------------------------
class CIScreenHelper
{
    typedef BOOL (*ProcCreateIScreen) (DWORD dwVersion, IScreen** ppScreen, ITrace* pTrace );
public:
    BOOL  Create(IScreen** ppScreen, ITrace* pTrace = NULL)
    {
        try
        {
            if (m_hDll==NULL)
		#ifdef _DEBUG
			m_hDll = LoadLibrary("FKCanvas_D.dll");
		#else
			m_hDll = LoadLibrary("FKCanvas.dll");
		#endif
			if(m_hDll == NULL)
                throw "读取FKCanvas.dll失败！";

            ProcCreateIScreen CreateObject;

            CreateObject = (ProcCreateIScreen)::GetProcAddress(m_hDll,"CreateIScreen");

            if (CreateObject==NULL)
                throw "无法创建 screen 屏幕对象！";

            if (!CreateObject(0x1, ppScreen, pTrace) || *ppScreen==NULL)
                throw "创建 screen 屏幕对象失败！";

        }
        catch(LPSTR err_msg)
        {
            OutputDebugString(err_msg);
            OutputDebugString("\r\n");
            return false;
        }
        catch(...)
        {
            OutputDebugString("屏幕创建出现异常！\r\n");
            return false;
        }
        return true;
    }
    void  Close()
    {
        if (m_hDll)
        {
           ::FreeLibrary(m_hDll);
           m_hDll = NULL;
        }
    }
public:
	CIScreenHelper(){m_hDll = NULL;}
	~CIScreenHelper()
	{
		Close();
	}
private:
    HINSTANCE  m_hDll;
};
//------------------------------------------------------------------------