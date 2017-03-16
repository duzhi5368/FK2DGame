/**
*	created:		2012-6-24   22:52
*	filename: 		ValidationCodeWndController
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "ValidationCodeWndController.h"
#include "../FKControllerInclude.h"
//------------------------------------------------------------------------
CValidationCodeWndController g_ValidationCodeWndController;
//------------------------------------------------------------------------
BOOL CValidationCodeWndController::Create()
{
	ConnectUISignal();
	// g_ClientGlobal->m_CanvasHelper.CreateIBitmap(&m_pImage);
	m_nCurrentSecond=0;

	//这里用0去订那个事件
	//g_pGlobal->m_pEventServer->AddListener(static_cast<IEventActionListener*>(this), 
	//	MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_VALIDATION), 
	//	SRC_TYPE_ID_ACTOR, (DWORD)0, "CValidationCodeWndController::Create");
	
 
	return TRUE;
}
//------------------------------------------------------------------------
CValidationCodeWndController::~CValidationCodeWndController()
{
	if(m_pImage!=NULL)
	{
		m_pImage->Release();
		m_pImage=NULL;
	}	
}
//------------------------------------------------------------------------
void CValidationCodeWndController::ConnectUISignal()
{	
	//m_SubmitConnect.Connect(g_pGlobal->m_pWndMgr,
	//	WND_VALIDATIONCODE,
	//	WE_WC_VALIDATIONCODE_SUBMIT,
	//	this,
	//	&CValidationCodeWndController::OnSubmit);
	//m_RefreshConnect.Connect(g_pGlobal->m_pWndMgr,
	//	WND_VALIDATIONCODE,
	//	WE_WC_VALIDATIONCODE_REFRESH,
	//	this,
	//	&CValidationCodeWndController::OnRefresh);
}
//------------------------------------------------------------------------
void CValidationCodeWndController::OnSubmit(WPARAM wParam, LPARAM lParam, BOOL & bContinue)
{
	/*int *pSelectIndex=(int*)lParam;

	char buf[256];
	CDataArchive archive(buf, 256);
	CS_ACTION_HEAD actionhead(MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, THINGPARTID_ACTOR_VALIDATION));
	archive.Write(&actionhead, sizeof(CS_ACTION_HEAD));
	
	archive.Write(pSelectIndex, sizeof(int));
	g_pGlobal->m_pAppNetworkLink->Send(buf, archive.GetBufferOffset());					
			
	g_pGlobal->m_pWndMgr->ShowWnd(WND_VALIDATIONCODE,SW_HIDE);*/
}
//------------------------------------------------------------------------
void CValidationCodeWndController::OnRefresh(WPARAM wParam, LPARAM lParam, BOOL & bContinue)
{
	/*
	char buf[16] = { 0 }; 
		CDataArchive archive(buf, 16);
		
		WORD temp = CMD_THING;
		archive.Write(&temp, sizeof(WORD));
		temp = CS_VALIDATIONCODE_MESSAGE;
		archive.Write(&temp, sizeof(WORD));
		temp = 1;//请求刷新
		archive.Write(&temp, sizeof(WORD));
	
		g_pGlobal->m_pLink->Send(buf, archive.GetBufferOffset());*/
	
}
//------------------------------------------------------------------------
const BYTE Shift1bpp[] = { 7,     6,     5,     4,     3,     2,     1,     0     };
//------------------------------------------------------------------------
void CValidationCodeWndController::OnRecv( DWORD dwMsg , char * buf , int nLen ) 
{
	TraceLn("Enter ValidationCode");
/*
	CValidationCodeHeader *pValidationCodeHeader=(CValidationCodeHeader*)buf;
	if(pValidationCodeHeader==NULL)
	{
		return;
	}
	int nDelay=pValidationCodeHeader->nDelayTime;	
	//更新描述
	g_pGlobal->m_pWndMgr->OnEvent(WND_VALIDATIONCODE,2,(WPARAM)(pValidationCodeHeader->szDescription),0);
	//更新答案
	g_pGlobal->m_pWndMgr->OnEvent(WND_VALIDATIONCODE,4,(WPARAM)(pValidationCodeHeader->Answers),0);

	char *pBitmapData=buf+sizeof(CValidationCodeHeader);
	
	BITMAPFILEHEADER *pBMPFileHeader=(BITMAPFILEHEADER*)pBitmapData;
	BYTE *pOrigin=(BYTE*)pBMPFileHeader+pBMPFileHeader->bfOffBits;
	m_pImage->Create(pValidationCodeHeader->nBitmapWidth,
		pValidationCodeHeader->nBitmapHeigth);
	int nBPS=(pValidationCodeHeader->nBitmapWidth * 1 + 31) / 32 * 4;
	for(int y=0;y<pValidationCodeHeader->nBitmapHeigth;y++)
	{
		for(int x=0;x<pValidationCodeHeader->nBitmapWidth;x++)
		{
			BYTE * pPixel = pOrigin + y * nBPS;
			DWORD dwIndex=( pPixel[x/8] >> Shift1bpp[x%8] ) & 0x01;
			if(dwIndex==0)//黑白颠倒
			{
				dwIndex=0xffffff;
			}
			else
			{
				dwIndex=0;
			}
			m_pImage->DrawPixel(x,pValidationCodeHeader->nBitmapHeigth-y-1,WPixel(dwIndex));
		}
	}		

	switch(dwMsg) {
	case 0://普通发送
		{
			char temp[16]={0};
			sprintf(temp,"%d",nDelay);
			g_pGlobal->m_pWndMgr->OnEvent(WND_VALIDATIONCODE,0,(WPARAM)temp,(LPARAM)m_pImage);
			g_pGlobal->m_pWndMgr->ShowWnd(WND_VALIDATIONCODE,SW_SHOW);	
			
			//重新计时 
			m_nCurrentSecond=nDelay;
			g_pGlobal->m_TimerAxisHelper.m_pTimeAxis->SetTimer(DELAY_TICK,this,1000);
			break;
		}
	case 1://更新图片
		{
			g_pGlobal->m_pWndMgr->OnEvent(WND_VALIDATIONCODE,3,0,(LPARAM)m_pImage);
			break;
		}
	default:
		break;
	}
	*/
}
//------------------------------------------------------------------------
void  CValidationCodeWndController::OnTimer(DWORD dwEventID)
{
	/*
	switch(dwEventID) {
	case DELAY_TICK:
		{
		m_nCurrentSecond--;
		if(m_nCurrentSecond>0){
			char temp[10];
			sprintf(temp,"%d",m_nCurrentSecond);
			g_pGlobal->m_pWndMgr->OnEvent(WND_VALIDATIONCODE,1,(WPARAM)temp,0);//跟新时间
		}
		else{//超时
			g_pGlobal->m_pWndMgr->ShowWnd(WND_VALIDATIONCODE,SW_HIDE);	
			g_pGlobal->m_TimerAxisHelper.m_pTimeAxis->KillTimer(DELAY_TICK,this);
		}
		break;
		}
	default:
		break;
	}
	*/
}
//------------------------------------------------------------------------
void CValidationCodeWndController::OnAction(IEvent *pEvent)
{
	int nContextLen;
	BYTE *pBuff = pEvent->GetContext(nContextLen);
	if (pBuff)
	{
		OnRecv(0 , (char *)pBuff , nContextLen);
	}
}
//------------------------------------------------------------------------