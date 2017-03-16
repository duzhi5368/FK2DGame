/**
*	created:		2012-6-20   20:07
*	filename: 		WindowCanvas
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/WindowCanvas.h"
#include "../Include/CanvasCommon.h"
//------------------------------------------------------------------------
WCanvas::WCanvas()
{
	m_pBmp = NULL;
	m_bInEqualOut = TRUE;
	m_bmp.SetData(&m_bmpStruct);
}
//------------------------------------------------------------------------
WCanvas::~WCanvas()
{
	m_bmp.SetData(NULL);
}
//------------------------------------------------------------------------
IBitmap* WCanvas::GetBitmap() const
{
	return m_pBmp;
}
//------------------------------------------------------------------------
IBitmap* WCanvas::SelectBitmap(IBitmap* pBitmap)
{
	IBitmap* pOld = m_pBmp;
	m_pBmp = pBitmap;

	SetBorder();
	return pOld;
}
//------------------------------------------------------------------------
void WCanvas::AddDirtyRect(RECT& area)
{
	m_rcDirty.AddDirtyRect(area);
}
//------------------------------------------------------------------------
void WCanvas::AddOutDirtyRect(RECT& area)
{
	m_rcOutDirty.AddDirtyRect(area);
}
//------------------------------------------------------------------------
void WCanvas::AddInOutRect(RECT& area)
{
	m_rcDirty.AddDirtyRect(area);
	if(!m_bInEqualOut)
		m_rcOutDirty.AddDirtyRect(area);
}
//------------------------------------------------------------------------
void WCanvas::AddOutDirtyRect(int x, int y, int w, int h)
{
		RECT rc;
		rc.left = x;
		rc.top = y;
		rc.right = x + w;
		rc.bottom = y + h;
		AddOutDirtyRect(rc);
}
//------------------------------------------------------------------------
void WCanvas::AddOverlay(RECT& area)
{
	m_rcDirty.RemoveDirty(area);
}
//------------------------------------------------------------------------
void WCanvas::AddOutOverlay(RECT& area)
{
	m_rcOutDirty.RemoveDirty(area);
}
//------------------------------------------------------------------------
void WCanvas::AddInOutOverlay(RECT& area)
{
	m_rcDirty.RemoveDirty(area);
	if(!m_bInEqualOut)
		m_rcOutDirty.RemoveDirty(area);
}
//------------------------------------------------------------------------
void WCanvas::SetBorder()
{
	if(!m_pBmp || !m_pBmp->IsValid())
	{
		FKTraceLn("WCavas::SetBorder, IBitmap is invalid!");
		return;
	}
	RECT rc = {0, 0, m_pBmp->GetWidth(), m_pBmp->GetHeight()};
	m_rcDirty.SetBorder(rc);
	
	m_rcOutDirty.SetBorder(rc);
}
//------------------------------------------------------------------------
//增加整个Canvas
void WCanvas::AddWholeScreen()
{
	m_rcDirty.AddWholeScreen();
}
//------------------------------------------------------------------------
void WCanvas::AddOutWholeScreen()
{
	m_rcOutDirty.AddWholeScreen();
}
//------------------------------------------------------------------------
void WCanvas::AddInOutWholeScreen()
{
	m_rcDirty.AddWholeScreen();
	if(!m_bInEqualOut)
		m_rcOutDirty.AddWholeScreen();
}
//------------------------------------------------------------------------
void WCanvas::ResetDirtyRect()
{
	m_rcDirty.ClearDirty();
}
//------------------------------------------------------------------------
void WCanvas::ResetOutDirtyRect()
{
	m_rcOutDirty.ClearDirty();
}
//------------------------------------------------------------------------
BOOL WCanvas::Draw(int x, int y, ICanvas* pCanvas, DWORD dwStyle)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::Draw the canvas is invalid!");
		return FALSE;
	}

	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->Draw(x, y, pCanvas->GetBitmap(), dwStyle);

	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;	
	
	BOOL ret;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{		
		SubBmpStruct(*pRc);
		ret = m_bmp.Draw(x - pRc->left, y - pRc->top, pCanvas->GetBitmap(), dwStyle);

		if(!ret)
			return FALSE;
	}while(pRc = m_rcDirty.SeekNextDirty());

	return ret;
}
//------------------------------------------------------------------------
BOOL WCanvas::Draw(int dx, int dy, int w, int h, ICanvas* pCanvas, int sx, int sy, DWORD dwStyle )  //增强的Draw
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::Draw the canvas is invalid!");
		return FALSE;
	}

	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->Draw(dx, dy, w, h, pCanvas->GetBitmap(), sx, sy, dwStyle);

	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;	
	
	BOOL ret;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		SubBmpStruct(*pRc);
		ret = m_bmp.Draw(dx - pRc->left, dy - pRc->top, w, h, pCanvas->GetBitmap(), sx, sy, dwStyle);

		if(!ret)
			return FALSE;
	}
	while(pRc = m_rcDirty.SeekNextDirty());

	return ret;
}
//------------------------------------------------------------------------
//是否有效
BOOL WCanvas::IsValid() const
{
	return (m_pBmp && m_pBmp->IsValid());
}
//------------------------------------------------------------------------
BOOL WCanvas::Clear(WPixel clr, RECT* pRc1)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::Clear the canvas is invalid!");
		return FALSE;
	}

	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->Clear(clr, pRc1);

	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;	
		
	BOOL ret;
	RECT temp;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		if(pRc1)
		{
			IntersectRect(&temp, pRc, pRc1);
			if(!IsRectEmpty(&temp))
			{
				SubBmpStruct(temp);
				ret = m_bmp.Clear(clr);
			}
		}
		else
		{
			SubBmpStruct(*pRc);
			ret = m_bmp.Clear(clr);
		}

		if(!ret)
			return FALSE;
	}while(pRc = m_rcDirty.SeekNextDirty());

	return ret;
}
//------------------------------------------------------------------------
BOOL WCanvas::Create(int nWidth, int nHeight)
{
	if(!m_pBmp->Create(nWidth, nHeight))
		return FALSE;

	SetBorder();

	return TRUE;
}
//------------------------------------------------------------------------
BOOL WCanvas::Create(int nWidth, int nHeight, void* ptr)
{
	if(!m_pBmp->Create(nWidth, nHeight, ptr))
		return FALSE;

	SetBorder();

	return TRUE;
}
//------------------------------------------------------------------------
BOOL WCanvas::Create(IBitmap* parent, int x, int y, int nWidth, int nHeight)
{
	if(!m_pBmp->Create(parent, x, y, nWidth, nHeight))
		return FALSE;

	SetBorder();

	return TRUE;
}
//------------------------------------------------------------------------
BOOL WCanvas::Update2DC(HDC hdc, int dx, int dy, int x, int y, int w, int h) 
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::Blit the canvas is invalid!");
		return FALSE;
	}
	
	CDirtyList* pList;
	if(m_bInEqualOut)
	{
		pList = &m_rcDirty;
	}
	else
	{
		pList = &m_rcOutDirty;
	}

	if(pList->IsWholeDirty())
		return Blit2DC(hdc, dx, dy, x, y, w, h);

	if(pList->GetDirtyCount() == 0)
		return TRUE;
	
	BOOL ret;
	RECT rcDest, rcReal;
	SetRect(&rcDest, x, y, x+w, y+h);
	RECT* pRc1 = pList->SeekFirstDirty();
	do
	{
		IntersectRect(&rcReal, &rcDest, pRc1);
		
		if(!IsRectEmpty(&rcReal))
		{
			ret = m_pBmp->Blit2DC(hdc, dx - (x - rcReal.left), dy - (y - rcReal.top), rcReal.left, rcReal.top,
				rcReal.right - rcReal.left, rcReal.bottom - rcReal.top);

			if(!ret)
				return FALSE;
		}

	}while(pRc1 = pList->SeekNextDirty());
	
	return ret;
}
//------------------------------------------------------------------------
//不处理裁剪
BOOL WCanvas::Blit2DC(HDC hdc, int dx, int dy, int x, int y, int w, int h) 
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::Blit the canvas is invalid!");
		return FALSE;
	}
	
	return m_pBmp->Blit2DC(hdc, dx, dy, x, y, w, h);
}
//------------------------------------------------------------------------
BOOL WCanvas::Draw(int x, int y, IBitmap* pBmpSrc, DWORD dwStyle)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::Draw the canvas is invalid!");
		return FALSE;
	}

	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->Draw(x, y, pBmpSrc, dwStyle);
	
	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;	
		
	BOOL ret;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		SubBmpStruct(*pRc);
		ret = m_bmp.Draw(x - pRc->left, y - pRc->top, pBmpSrc, dwStyle);
		if(!ret)
			return FALSE;
	}while(pRc = m_rcDirty.SeekNextDirty());

	return ret;
}
//------------------------------------------------------------------------
BOOL WCanvas::DrawAlpha(int x, int y, IAlphaBitmap* pBmpSrc, DWORD dwStyle)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::Draw the canvas is invalid!");
		return FALSE;
	}

	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->DrawAlpha(x, y, pBmpSrc, dwStyle);
	
	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;

	BOOL ret;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		SubBmpStruct(*pRc);
		ret = m_bmp.DrawAlpha(x - pRc->left, y - pRc->top, pBmpSrc, dwStyle);
		if(!ret)
			return FALSE;
	}while(pRc = m_rcDirty.SeekNextDirty());

	return ret;
}
//------------------------------------------------------------------------
BOOL WCanvas::Draw(int dx, int dy, int w, int h, IBitmap* pBmpSrc, int sx, int sy, DWORD dwStyle)  //增强的Draw
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::Draw the canvas is invalid!");
		return FALSE;
	}

	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->Draw(dx, dy,w, h, pBmpSrc, sx, sy, dwStyle);
	
	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;

	BOOL ret;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		SubBmpStruct(*pRc);
		ret = m_bmp.Draw(dx - pRc->left, dy - pRc->top, w, h, pBmpSrc, sx, sy, dwStyle);
		if(!ret)
			return FALSE;
	}while(pRc = m_rcDirty.SeekNextDirty());

	return ret;
}
//------------------------------------------------------------------------
BOOL WCanvas::DrawAlpha(int dx, int dy, int w, int h, IAlphaBitmap* pBmpSrc, int sx, int sy, DWORD dwStyle)  //增强的Draw
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::Draw the canvas is invalid!");
		return FALSE;
	}

	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->DrawAlpha(dx, dy,w, h, pBmpSrc, sx, sy, dwStyle);
	
	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;	
		
	BOOL ret;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		SubBmpStruct(*pRc);
		ret = m_bmp.DrawAlpha(dx - pRc->left, dy - pRc->top, w, h, pBmpSrc, sx, sy, dwStyle);
		if(!ret)
			return FALSE;
	}while(pRc = m_rcDirty.SeekNextDirty());

	return ret;
}
//------------------------------------------------------------------------
// 把别的rle位图绘制到此位图
BOOL WCanvas::DrawRle(int dx, int dy, IRleBitmap* bmp, DWORD dwStyle)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::DrawRle the canvas is invalid!");
		return FALSE;
	}
	
	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->DrawRle(dx, dy, bmp, dwStyle);
	
	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;	
		
	BOOL ret;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		SubBmpStruct(*pRc);
		ret = m_bmp.DrawRle(dx - pRc->left, dy - pRc->top, bmp, dwStyle);
		if(!ret)
			return FALSE;
	}while(pRc = m_rcDirty.SeekNextDirty());
	
	return ret;
}
//------------------------------------------------------------------------
// 增强的rle绘制
BOOL WCanvas::DrawRle(int dx, int dy, int w, int h, IRleBitmap* bmp, int sx, int sy, DWORD dwStyle)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::DrawRle the canvas is invalid!");
		return FALSE;
	}
	
	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->DrawRle(dx, dy, w, h, bmp, sx, sy, dwStyle);
	
	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;	

	BOOL ret;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		SubBmpStruct(*pRc);
		ret = m_bmp.DrawRle(dx - pRc->left, dy - pRc->top, w, h, bmp, sx, sy, dwStyle);
		
		if(!ret)
			return FALSE;
	}while(pRc = m_rcDirty.SeekNextDirty());
	
	return ret;
}
//------------------------------------------------------------------------
// 把别的rle位图绘制到此位图
BOOL WCanvas::DrawRle(int dx, int dy, IRleBitmap* bmp, BYTE* pPal, DWORD dwStyle)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::DrawRle the canvas is invalid!");
		return FALSE;
	}
	
	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->DrawRle(dx, dy, bmp, pPal, dwStyle);
	
	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;	
		
	BOOL ret;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		SubBmpStruct(*pRc);
		ret = m_bmp.DrawRle(dx - pRc->left, dy - pRc->top, bmp, pPal, dwStyle);
		if(!ret)
			return FALSE;
	}while(pRc = m_rcDirty.SeekNextDirty());
	
	return ret;
}
//------------------------------------------------------------------------
// 增强的rle绘制
BOOL WCanvas::DrawRle(int dx, int dy, int w, int h, IRleBitmap* bmp, BYTE* pPal, int sx, int sy, DWORD dwStyle)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::DrawRle the canvas is invalid!");
		return FALSE;
	}
	
	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->DrawRle(dx, dy, w, h, bmp, pPal, sx, sy, dwStyle);

	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;	

	BOOL ret;	
	RECT* pRc = m_rcDirty.SeekFirstDirty();	
	do
	{		
		SubBmpStruct(*pRc);
		ret = m_bmp.DrawRle(dx - pRc->left, dy - pRc->top, w, h, bmp, pPal, sx, sy, dwStyle);
		
		if(!ret)
			return FALSE;
	}while(pRc = m_rcDirty.SeekNextDirty());
	
	return ret;
}
//------------------------------------------------------------------------
// 将IBitmap画向Canvas
BOOL WCanvas::Draw(ICanvas* pCanvas, int dx, int dy, DWORD dwStyle)
{
	return pCanvas->Draw(dx, dy, static_cast<ICanvas*>(this), dwStyle);
}
//------------------------------------------------------------------------
BOOL WCanvas::Draw(ICanvas* pCanvas, int dx, int dy, int w, int h, int sx, int sy, DWORD dwStyle)
{
	return pCanvas->Draw(dx, dy, w, h, static_cast<ICanvas*>(this), sx, sy, dwStyle);
}
//------------------------------------------------------------------------
void WCanvas::Draw(int x, int y, const char* s, WPixel clr)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::Draw the canvas is invalid!");
		return ;
	}
	m_pBmp->Draw(x, y, s, clr);
	return;
}
//------------------------------------------------------------------------
int WCanvas::GetWidth() const
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::GetWidth, the canvas is invalid!");
		return 0;
	}
	return m_pBmp->GetWidth();
}
//------------------------------------------------------------------------
int WCanvas::GetHeight() const
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::GetHeight, the canvas is invalid!");
		return 0;
	}
	return m_pBmp->GetHeight();
}
//------------------------------------------------------------------------
BOOL WCanvas::Load(void* bmpdata)
{
	if(!m_pBmp)
	{
		FKTraceLn("WCanvas::Load, m_pBmp is NULL!");
		return FALSE;
	}

	return m_pBmp->Load(bmpdata);
}
//------------------------------------------------------------------------
void* WCanvas::GetBmpStruct() const
{
	if(!m_pBmp)
	{
		FKTraceLn("WCanvas::GetBmpStruct, m_pBmp is NULL!");
		return NULL;
	}

	return m_pBmp->GetBmpStruct();
}
//------------------------------------------------------------------------
void WCanvas::SetData(void* pBmp)
{
	if(!m_pBmp)
	{
		FKTraceLn("WCanvas::SetData, m_pBmp is NULL!");
		return ;
	}

	m_pBmp->SetData(pBmp);
}
//------------------------------------------------------------------------
void WCanvas::SetUserData(DWORD userdata)
{
	if(!m_pBmp)
	{
		FKTraceLn("WCanvas::SetUserData, m_pBmp is NULL!");
		return ;
	}
	m_pBmp->SetUserData(userdata);
}
//------------------------------------------------------------------------
WPixel* WCanvas::operator [](int y) const
{
	if(!m_pBmp)
	{
		FKTraceLn("WCanvas::[], m_pBmp is NULL!");
		return NULL;
	}
	return (*m_pBmp)[y];
}
//------------------------------------------------------------------------
void WCanvas::Release()
{
	delete this;
}
//------------------------------------------------------------------------
BOOL WCanvas::Save(const char* filename) const
{
	if(!m_pBmp)
	{
		FKTraceLn("WCanvas::Save, m_pBmp is NULL!");
		return FALSE;
	}
	return m_pBmp->Save(filename);
}
//------------------------------------------------------------------------
BOOL WCanvas::DrawBox(RECT& rc, WPixel clr, DWORD dwStyle /*= BLIT_NORMAL*/, int alpha /*= 255*/)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::DrawBox the canvas is invalid!");
		return FALSE;
	}

	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->DrawBox(rc, clr, dwStyle, alpha);

	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;	

	BOOL ret;
	RECT rctemp;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		SubBmpStruct(*pRc);
		rctemp = rc;
		OffsetRect(&rctemp, -pRc->left, -pRc->top);
		ret = m_bmp.DrawBox(rctemp, clr, dwStyle, alpha);
	}while(pRc = m_rcDirty.SeekNextDirty());

	return ret;
}
//------------------------------------------------------------------------
BOOL WCanvas::DrawLine(POINT& ptFrom, POINT& ptTo, WPixel clr, DWORD dwStyle /*= BLIT_NORMAL*/, int alpha/* = 255*/)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::Line the canvas is invalid!");
		return FALSE;
	}

	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->DrawLine(ptFrom, ptTo, clr, dwStyle, alpha);

	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;	

	BOOL ret;
	POINT pt1, pt2;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		pt1 = ptFrom;
		pt2 = ptTo;

		
		SubBmpStruct(*pRc);
		
		pt1.x -= pRc->left;
		pt1.y -= pRc->top;

		pt2.x -= pRc->left;
		pt2.y -= pRc->top;

		ret = m_bmp.DrawLine(pt1, pt2, clr, dwStyle, alpha);

	}while(pRc = m_rcDirty.SeekNextDirty());

	return ret;
}
//------------------------------------------------------------------------
BOOL WCanvas::DrawText(ITTFont* pfont, int x, int y, const char* str, WPixel color, DWORD style/* =FONT_ALPHA */)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::DrawText the canvas is invalid!");
		return FALSE;
	}

	
	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->DrawText(pfont, x, y, str, color, style);

	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;
	
	BOOL ret;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		SubBmpStruct(*pRc);
		ret = m_bmp.DrawText(pfont, x - pRc->left, y - pRc->top, str, color, style);
		if(!ret)
		{
			FKTraceLn("Canvas::DrawText failed!");
			return FALSE;
		}
	}while(pRc = m_rcDirty.SeekNextDirty());

	return ret;
}
//------------------------------------------------------------------------
BOOL WCanvas::DrawTextEx(ITTFont* pfont, int x, int y, const char* str, WPixel color, WPixel outlinecolor, DWORD style/* =FONT_ALPHA */)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::DrawText the canvas is invalid!");
		return FALSE;
	}

	
	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->DrawTextEx(pfont, x, y, str, color, outlinecolor, style);

	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;
	
	BOOL ret;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		SubBmpStruct(*pRc);
		ret = m_bmp.DrawTextEx(pfont, x - pRc->left, y - pRc->top, str, color, outlinecolor, style);
		if(!ret)
		{
			FKTraceLn("Canvas::DrawText failed!");
			return FALSE;
		}
	}while(pRc = m_rcDirty.SeekNextDirty());

	return ret;
}
//------------------------------------------------------------------------
// 带裁剪框的文字输出
BOOL WCanvas::DrawClipText(ITTFont* pfont, int x, int y, const char* str, WPixel color, RECT rcClip, DWORD style)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::DrawText the canvas is invalid!");
		return FALSE;
	}
	
	if(m_rcDirty.IsWholeDirty())
	{
		SubBmpStruct(rcClip);
		return m_bmp.DrawText(pfont, x - rcClip.left, y - rcClip.top, str, color, style);
	}
	
	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;
	
	BOOL ret;
	RECT rcTemp;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		IntersectRect(&rcTemp, pRc, &rcClip);
		if (IsRectEmpty(&rcTemp))
			continue;
		SubBmpStruct(rcTemp);
		ret = m_bmp.DrawText(pfont, x - rcTemp.left, y - rcTemp.top, str, color, style);
		if(!ret)
		{
			FKTraceLn("Canvas::DrawText failed!");
			return FALSE;
		}
	}while(pRc = m_rcDirty.SeekNextDirty());
	
	return ret;
}
//------------------------------------------------------------------------
IBitmap* WCanvas::SubBitmap(int x, int y, int nWidth, int nHeight)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::SubBitmap, canvas is invalid!");
		return NULL;
	}
	return m_pBmp->SubBitmap( x,  y,  nWidth,  nHeight);
}
//------------------------------------------------------------------------
IBitmap* WCanvas::SubTile(POINT ptLeftTop)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::SubTile, canvas is invalid!");
		return NULL;
	}
	return m_pBmp->SubTile(ptLeftTop);
}
//------------------------------------------------------------------------
DWORD WCanvas::GetUserData() const
{
	if(!m_pBmp)
	{
		FKTraceLn("WCanvas::GetUserData, m_pBmp is NULL!");
		return 0;
	}

	return m_pBmp->GetUserData();
}
//------------------------------------------------------------------------
void WCanvas::Merge(void)
{
	m_rcDirty.PackDirty();
}
//------------------------------------------------------------------------
void WCanvas::MergeOut(void)
{
	m_rcOutDirty.PackDirty();
}
//------------------------------------------------------------------------
void* WCanvas::GetDirtyList()
{
	return (void*)(&m_rcDirty);
}
//------------------------------------------------------------------------
void* WCanvas::GetOutDirtyList()
{
	return (void*)(&m_rcOutDirty);
}
//------------------------------------------------------------------------
void WCanvas::AddDirtyList(void* pVoid)
{
	CDirtyList* pList = (CDirtyList*)pVoid;

	m_rcDirty.AddDirtyList(pList);
}
//------------------------------------------------------------------------
void WCanvas::AddOutDirtyList(void* pVoid)
{
	CDirtyList* pList = (CDirtyList*)pVoid;

	m_rcOutDirty.AddDirtyList(pList);
}
//------------------------------------------------------------------------
void WCanvas::SubBmpStruct(RECT& rc)
{
	WBmpStruct* pParentData = (WBmpStruct*)m_pBmp->GetBmpStruct();
	ASSERT(pParentData);

	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	int x = rc.left ;
	int y = rc.top;
	
	if(x < 0)
	{
		width += x;
		x = 0;
	}
	if(rc.top < 0)
	{
		height += y;
		y = 0;
	}
	
	m_bmpStruct.kx = 0;
	m_bmpStruct.ky = 0;
	m_bmpStruct.pitch = pParentData->pitch;
	m_bmpStruct.w = MIN(width, pParentData->w - x);
	m_bmpStruct.h = MIN(height, pParentData->h - y);
	m_bmpStruct.ptr = (void*)((DWORD)pParentData->ptr + pParentData->pitch*y + x*2);
	m_bmpStruct.userdata = pParentData->userdata;
	m_bmpStruct.bChild = TRUE;
}
//------------------------------------------------------------------------
BOOL WCanvas::IsTransparent(POINT& ptSelect)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::IsTransparent() the canvas is invalid!");
		return TRUE;
	}
	return m_pBmp->IsTransparent(ptSelect);
}
//------------------------------------------------------------------------
// 得到图片所占内存的大小
DWORD WCanvas::GetSize()
{
	if(!m_pBmp)
	{
		FKTraceLn("WCanvas::GetSize, m_pBmp is NULL!");
		return 0;
	}
	
	return m_pBmp->GetSize();
}
//------------------------------------------------------------------------
BOOL WCanvas::DrawPixel(int x, int y, WPixel clr, DWORD dwStyle, int alpha)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::PutPixel the canvas is invalid!");
		return FALSE;
	}
	
	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->DrawPixel(x, y, clr, dwStyle, alpha);
	
	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;
	
	BOOL ret;
	RECT* pRc = m_rcDirty.SeekFirstDirty();
	do
	{
		SubBmpStruct(*pRc);
		ret = m_bmp.DrawPixel(x - pRc->left, y - pRc->top, clr, dwStyle, alpha);
		
		if(!ret)
			return FALSE;
	}while(pRc = m_rcDirty.SeekNextDirty());
	
	return ret;
}
//------------------------------------------------------------------------
BOOL WCanvas::DrawText(int x, int y, const char* str, WPixel color, DWORD style/*=FONT_ALPHA*/)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::DrawText the canvas is invalid!");
		return FALSE;
	}

	if(m_rcDirty.IsWholeDirty())
		return m_pBmp->DrawText(/*m_pDefFont, */x, y, str, color, style);
	
	if(m_rcDirty.GetDirtyCount() == 0)
		return TRUE;

	BOOL ret;
	RECT* pRc = m_rcDirty.SeekFirstDirty();	
	do
	{
		SubBmpStruct(*pRc);
		ret = m_bmp.DrawText(x - pRc->left, y - pRc->top, str, color, style);
		if(!ret)
		{
			FKTraceLn("Canvas::DrawText failed!");
			return FALSE;
		}
	}while(pRc = m_rcDirty.SeekNextDirty());
	
	return ret;
}
//------------------------------------------------------------------------
BOOL WCanvas::Scroll(int x, int y)
{
	if(!IsValid())
	{
		FKTraceLn("WCanvas::scroll the canvas is invalid!");
		return FALSE;
	}

	return m_pBmp->Scroll(x, y);
}
//------------------------------------------------------------------------
//显示脏矩形列表
void WCanvas::DrawDirtyList(ICanvas* pICanvas,WPixel color)
{	
	RECT* prc = NULL;
	int n = m_rcDirty.GetDirtyCount();
	if(n==0)return;
	
	char strInfo[128];
	sprintf(strInfo,"Dirty rect count is %d\r\n",n);
	OutputDebugString(strInfo);

	prc = m_rcDirty.SeekFirstDirty();
	IBitmap *pIBitmap = pICanvas->GetBitmap();
	for(int i=1;i<=n;i++)
	{	
		POINT ptFrom,ptTo;
		ptFrom.x = prc->left; ptFrom.y = prc->top;
		ptTo.x = prc->left; ptTo.y = prc->bottom;
		pIBitmap->DrawLine(ptFrom,ptTo,color,BLIT_COPY,233);
		
		ptFrom.x = prc->left; ptFrom.y = prc->top;
		ptTo.x = prc->right; ptTo.y = prc->top;
		pIBitmap->DrawLine(ptFrom,ptTo,color,BLIT_COPY,233);
		
		ptFrom.x = prc->right; ptFrom.y = prc->top;
		ptTo.x = prc->right; ptTo.y = prc->bottom;
		pIBitmap->DrawLine(ptFrom,ptTo,color,BLIT_COPY,233);

		ptFrom.x = prc->left; ptFrom.y = prc->bottom;
		ptTo.x = prc->right; ptTo.y = prc->bottom;
		pIBitmap->DrawLine(ptFrom,ptTo,color,BLIT_COPY,233);
		
		if(i==n)
			break;
		prc = m_rcDirty.SeekNextDirty();				
	}
}
//------------------------------------------------------------------------
void WCanvas::DrawOutDirtyList(ICanvas *pICanvas, WPixel color)
{
	RECT* prc = NULL;
	int n = m_rcOutDirty.GetDirtyCount();
	if(n==0)return;
	prc = m_rcOutDirty.SeekFirstDirty();
	IBitmap *pIBitmap = pICanvas->GetBitmap();
	for(int i=1;i<=n;i++)
	{	
		POINT ptFrom,ptTo;
		ptFrom.x = prc->left; ptFrom.y = prc->top;
		ptTo.x = prc->left; ptTo.y = prc->bottom;
		pIBitmap->DrawLine(ptFrom,ptTo,color,BLIT_COPY,233);
		
		ptFrom.x = prc->left; ptFrom.y = prc->top;
		ptTo.x = prc->right; ptTo.y = prc->top;
		pIBitmap->DrawLine(ptFrom,ptTo,color,BLIT_COPY,233);
		
		ptFrom.x = prc->right; ptFrom.y = prc->top;
		ptTo.x = prc->right; ptTo.y = prc->bottom;
		pIBitmap->DrawLine(ptFrom,ptTo,color,BLIT_COPY,233);

		ptFrom.x = prc->left; ptFrom.y = prc->bottom;
		ptTo.x = prc->right; ptTo.y = prc->bottom;
		pIBitmap->DrawLine(ptFrom,ptTo,color,BLIT_COPY,233);
		
		if(i==n)
			break;
		prc = m_rcOutDirty.SeekNextDirty();				
	}
}
//------------------------------------------------------------------------
/*
void WCanvas::CreateDefFont()
{
   // Create Font
   HDC hdc = GetDC(NULL);
   HFONT hFont = CreateFont(
	   -MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72), 
	   0, 0, 0,
	   FW_NORMAL, 0, 0, 0, 
	   DEFAULT_CHARSET, OUT_DEVICE_PRECIS,
	   CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
	   DEFAULT_PITCH | FF_SWISS, "宋体");

   ReleaseDC(NULL, hdc);
   
   m_pDefFont = new WFont;
   
   if (!m_pDefFont->Create(hFont, 0))
   {
	   FKTraceLn("WCanvas::CreateDefFont, fail!");
	   DeleteObject(hFont);
	   return;
   }
   DeleteObject(hFont);
}*/
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) BOOL CreateICanvas(DWORD dwVersion, ICanvas** ppICanvas, ITrace* pTrace)
{
#ifdef _DEBUG
	if(pTrace)
		g_DefaultTrace.Create(pTrace);
#endif

	WCanvas* pCanvas = new WCanvas;
	
	if(pCanvas == NULL)
		return FALSE;

	*ppICanvas = static_cast<ICanvas*>(pCanvas);

	return TRUE;
}
//------------------------------------------------------------------------