/**
*	created:		2012-6-28   4:16
*	filename: 		BulletinContainer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现公告信息容器，存放系统公告
*/
//------------------------------------------------------------------------
#include "../Include/BulletinContainer.h"
//------------------------------------------------------------------------
CBulletinContainer::CBulletinContainer(void)
: m_pFont(NULL)
, m_nStartPos(0)
, m_nTotalLen(0)
, m_bMouseOver(FALSE)
{
	// 创建绘制公告的字体
	m_pFont = XFontCreate::GetInstance()->CreateFont("宋体", 15, FALSE, FALSE, FALSE, TRUE);
}
//------------------------------------------------------------------------
CBulletinContainer::~CBulletinContainer(void)
{
	// 销毁字体
	if (m_pFont != NULL)
	{
		XFontCreate::GetInstance()->ReleaseFont(m_pFont);
	}

	XDesktop * pDesktop = GetDesktop();
	if (pDesktop != NULL && pDesktop->GetTimeAxis() != NULL)
	{
		pDesktop->GetTimeAxis()->KillTimer(0, static_cast<ITimerSink*>(this));
	}	
}

//-------------------------------------------------------------------
// 描  述：向左滚动(以最右边为参照点) 
//-------------------------------------------------------------------
void CBulletinContainer::OnDraw(ICanvas* pCanvas, RECT& rcClip )
{	
	if (pCanvas == NULL || m_items.size() == 0)
	{
		return;
	}	

	RECT rc = m_rcWnd;
	int nPos = m_nStartPos;
	int nLeft = 0 - GetWidth();

	for (ItemListItr itr = m_items.begin(); itr != m_items.end(); itr++)
	{		
		// 在有效范围内绘制
		if ((nPos + (*itr).GetLength()) > nLeft)
		{
			if (nPos > nLeft)
			{
				rc.left = m_rcWnd.right + nPos;
			}
			else
			{
				rc.left = m_rcWnd.left;
			}
		
			rc.right = rc.left + (*itr).GetLength();			
			if (rc.right > m_rcWnd.right)
			{
				rc.right = m_rcWnd.right;
			}
			
			// 绘制公告
			(*itr).OnDraw(pCanvas, m_rcWnd.right + nPos, rc.top, rc);
		}

		// 获取下一个公告的显示位置
		nPos += (*itr).GetLength();
		if (nPos > 0)
		{
			break;
		}
	}
}
//------------------------------------------------------------------------
DWORD CBulletinContainer::OnMouseMove(UINT nButton, UINT nShift, POINT pt)
{
	if (PtInRect(&m_rcWnd, pt) && IsVisible())
	{
		if (GetDesktop() != NULL)
		{
			GetDesktop()->SetCaptureWindow(this);
		}		

		m_bMouseOver = TRUE;
	}
	else
	{
		if (GetDesktop() != NULL)
		{
			GetDesktop()->SetCaptureWindow(NULL);
		}				

		m_bMouseOver = FALSE;
	}	

	MouseOver(pt);

	return 0;
}
//------------------------------------------------------------------------
DWORD CBulletinContainer::OnMouseDown(UINT nButton, UINT nShift, POINT pt)
{
	if (IsVisible()) 
	{
		ItemListItr itr = GetItemByPoint(pt);
		if (itr != m_items.end())
		{
			(*itr).Click();
		}
	}	
	
	return 0;
}
//------------------------------------------------------------------------
void CBulletinContainer::OnTimer(DWORD dwEventID)
{
	if (m_bMouseOver)
	{
		return;
	}

	m_nStartPos -= 1;

	// 检查是否已经完成了一次滚动
	if (abs(m_nStartPos) > (m_nTotalLen + GetWidth()))
	{
		for (ItemListItr itr = m_items.begin(); itr != m_items.end();)
		{
			ItemListItr preItr = itr++;
			(*preItr).Scrolled();			

			// 如果已经完成，则删除该公告			
			if ((*preItr).IsFinished())
			{				
				RemoveItem(preItr);
			}			
		}

		// 进行下一轮滚动
		m_nStartPos = 0;

		if (m_items.size() == 0)
		{
			XDesktop * pDesktop = GetDesktop();
			if (pDesktop != NULL && pDesktop->GetTimeAxis() != NULL)
			{
				pDesktop->GetTimeAxis()->KillTimer(0, static_cast<ITimerSink*>(this));
			}
		}
	}	
}

//-------------------------------------------------------------------
// 描  述：添加一个公告(格式: 滚动次数|url|公告文本  如: 5|www.aaa.com|测试！)
//-------------------------------------------------------------------
void CBulletinContainer::AddItem(LPCSTR lpszBulletin)
{
	int nCount = 1;
	string strUrl;
	string strBulletin = lpszBulletin;

	// 获取滚动次数
	int nPos = strBulletin.find('|');
	if (nPos != string::npos)
	{
		nCount = atoi(strBulletin.substr(0, nPos).c_str());
		strBulletin = strBulletin.substr(nPos + 1, strBulletin.size() - nPos);
	}

	// 获取URL
	nPos = strBulletin.find('|');
	if (nPos != string::npos)
	{
		strUrl = strBulletin.substr(0, nPos);
		strBulletin = strBulletin.substr(nPos + 1, strBulletin.size() - nPos);
	}
	
	CBulletinItem item;
	item.SetFont(m_pFont);
	item.SetText(strBulletin);
	item.SetUrl(strUrl);
	item.SetScrollCount(nCount);
	
	m_items.push_back(item);
	m_nTotalLen += item.GetLength();

	if (m_items.size() == 1)
	{
		XDesktop * pDesktop = GetDesktop();
		if (pDesktop != NULL && pDesktop->GetTimeAxis() != NULL)
		{
			pDesktop->GetTimeAxis()->SetTimer(0, static_cast<ITimerSink*>(this), 50, "CBulletinContainer::AddItem");
		}
	}
}

//-------------------------------------------------------------------
// 描  述：删除一个公告 
//-------------------------------------------------------------------
void CBulletinContainer::RemoveItem(ItemListItr itr)
{
	if (itr != m_items.end())
	{
		m_nTotalLen -= (*itr).GetLength();
		m_items.erase(itr);
	}	
}

//-------------------------------------------------------------------
// 描  述：获取坐标指向的公告 
//-------------------------------------------------------------------
CBulletinContainer::ItemListItr CBulletinContainer::GetItemByPoint(POINT pt)
{
	int nPos = pt.x - m_rcWnd.right;
	int nTmpPos = m_nStartPos;

	ItemListItr itr = m_items.begin();
	for (; itr != m_items.end(); itr++)
	{
		if ((nTmpPos <= nPos) && (nTmpPos + (*itr).GetLength() >= nPos))
		{
			break;
		}

		nTmpPos += (*itr).GetLength();
	}

	return itr;
}

//-------------------------------------------------------------------
// 描  述：鼠标移到到公告的上面 
//-------------------------------------------------------------------
void CBulletinContainer::MouseOver(POINT pt)
{
	int nPos = pt.x - m_rcWnd.right;
	int nTmpPos = m_nStartPos;
	
	ItemListItr curItr = m_items.end();
	ItemListItr itr = m_items.begin();
	for (; itr != m_items.end(); itr++)
	{
		if ((nTmpPos <= nPos) && (nTmpPos + (*itr).GetLength() >= nPos))
		{
			curItr = itr;
		}
		else
		{
			(*itr).MouseOver(FALSE);
		}
		
		nTmpPos += (*itr).GetLength();
	}

	if (curItr != m_items.end())
	{
		if (PtInRect(&m_rcWnd, pt))
		{
			(*curItr).MouseOver(TRUE);
			SetCursor(CT_HAND);
			return;
		}
		else
		{
			(*curItr).MouseOver(FALSE);
		}
	}

	SetCursor(CT_ARROW);
}
//------------------------------------------------------------------------