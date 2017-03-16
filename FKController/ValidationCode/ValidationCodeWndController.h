/**
*	created:		2012-6-24   22:47
*	filename: 		ValidationCodeWndController
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "WndCallback.h"
#include "WndMgrSignal.h"
#include "CreatureSignal.h"
#include "../../FKGame/Event/IEvent.h"
//------------------------------------------------------------------------
const int IMAGE_WIDTH=100;
const int IMAGE_HEIGHT=30;
//------------------------------------------------------------------------
class CValidationCodeWndController:public CWndCallback, public ITimerSink, public IEventActionListener
{
public:
	BOOL Create();
	void OnRecv( DWORD dwMsg , char * buf , int nLen );
	~CValidationCodeWndController();
	virtual void OnTimer(DWORD dwEventID);
	virtual void OnAction(IEvent *pEvent);
private:
	void     ConnectUISignal(void);
	void     ConnectServerMsg(void);
	void     OnSubmit(WPARAM wParam, LPARAM lParam, BOOL & bContinue);
	void	 OnRefresh(WPARAM wParam, LPARAM lParam, BOOL & bContinue);
	void     OnUpdate	  (LPCSTR buf, int nLen, BOOL &bContinue);
	void	 UpdateUI(void);

private:
	CWMConnection<CValidationCodeWndController> m_SubmitConnect;
	CWMConnection<CValidationCodeWndController> m_RefreshConnect;
	IBitmap *m_pImage;
	ICanvasHelper m_CanvasHelper;
	//RGBQUAD			 m_bmiColors[256];
	int m_nCurrentSecond;

private:
	enum{
		DELAY_TICK
	};
};
//------------------------------------------------------------------------
extern CValidationCodeWndController g_ValidationCodeWndController;
//------------------------------------------------------------------------