/**
*	created:		2012-6-20   20:00
*	filename: 		DirtyList
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/DirtyList.h"
#include "../Include/CanvasCommon.h"
//------------------------------------------------------------------------
CDirtyList::CDirtyList()
{
	ClearDirty();
	SetRectEmpty(&m_rcBorder);
}
//------------------------------------------------------------------------
CDirtyList::~CDirtyList()
{

}
//------------------------------------------------------------------------
void CDirtyList::_ClearDirty()
{
	m_nCount = 0;

	m_nCurp = m_nLastp = SEND;
	m_bWhole = FALSE;
}
//------------------------------------------------------------------------
void CDirtyList::ClearDirty()
{
	for(int i = 0; i < MAX_DIRTY_SIZE; i ++)
	{
		m_nPosList[i] = SFREE;
	}
	_ClearDirty();
}
//------------------------------------------------------------------------
void CDirtyList::_AddDirtyRect(int left, int top, int right, int bottom)
{
	if(m_bWhole)
		return;

	if(m_nCount >= MAX_DIRTY_SIZE)
	{
		FKTraceLn("CDirtyRect::_AddDirtyRect, the buffer is full!");
		AddWholeScreen();
		return;
	}
	
	if(left >= right || top >= bottom)
		return;


	if(left < m_rcBorder.left)
		left = m_rcBorder.left;

	if(top < m_rcBorder.top)
		top = m_rcBorder.top;

	if(right > m_rcBorder.right)
		right = m_rcBorder.right;

	if(bottom > m_rcBorder.bottom)
		bottom = m_rcBorder.bottom;


	if(left >= right || top >= bottom)
		return;


	int ipos, *prep;
	
	//如果list不为空
	for(ipos = m_nLastp, prep = &m_nLastp; ipos != SEND; prep = &m_nPosList[ipos], ipos = *prep)
	{
		RECT& rc = m_rcList[ipos];
		
		//有相交部分
		if(rc.left < right && rc.right > left && rc.top < bottom && rc.bottom > top)
		{
			
			//此矩形已增加
			if(left >= rc.left && right <= rc.right && top >= rc.top && bottom <= rc.bottom)
				return;
	
			//所加矩形包括原来的矩形
			if(left <= rc.left && right >= rc.right && top <= rc.top && bottom >= rc.bottom)
			{
				*prep = m_nPosList[ipos];
				m_nPosList[ipos] = SFREE;
				ipos = *prep;
				m_nCount --;

				_AddDirtyRect(left, top, right, bottom);
				return;
		
			}

			if(left < rc.left)
			{
				_AddDirtyRect(left, MAX(rc.top, top), rc.left, MIN(rc.bottom, bottom));
			}
			if(rc.right < right)
			{
				_AddDirtyRect(rc.right, MAX(rc.top, top), right, MIN(rc.bottom, bottom));
			}

			if(top < rc.top)
				_AddDirtyRect(left, top, right, rc.top);
			if(rc.bottom < bottom)
				_AddDirtyRect(left, rc.bottom, right, bottom);

			return;	
		}
	}

	for(ipos = 0; ipos < MAX_DIRTY_SIZE && m_nPosList[ipos] != SFREE; ipos ++);

	if(ipos == MAX_DIRTY_SIZE)
	{
		FKTraceLn("_AddDirtyRect, the buffer is full!");
		return;
	}
	
	m_rcList[ipos].left		= left;
	m_rcList[ipos].right	= right;
	m_rcList[ipos].top		= top;
	m_rcList[ipos].bottom	= bottom;

	m_nCount ++;

	if(m_nLastp == SEND)
	{
		m_nLastp = ipos;
		m_nPosList[ipos] = SEND;
		return;
	}

	int jpos;

	for(prep = &m_nLastp, jpos = m_nLastp; jpos != SEND && m_rcList[jpos].top < top; prep = &m_nPosList[jpos], jpos = *prep);
	for(; jpos != SEND && m_rcList[jpos].top <= top && m_rcList[jpos].left < left; prep = &m_nPosList[jpos], jpos = *prep);
	*prep = ipos;
	m_nPosList[ipos] = jpos;
	
}
//------------------------------------------------------------------------
void CDirtyList::AddDirtyRectDirect(int left, int top, int right, int bottom)
{
	if(m_bWhole)
		return;

	if(m_nCount >= MAX_DIRTY_SIZE)
	{
		FKTraceLn("CDirtyRect::AddDirtyRectDirect, the buffer is full!");
		AddWholeScreen();
		return;
	}

	if(left >= right || top >= bottom)
		return;

	if(left < m_rcBorder.left)
		left = m_rcBorder.left;
	if(top < m_rcBorder.top)
		top = m_rcBorder.top;
	if(right > m_rcBorder.right)
		right = m_rcBorder.right;
	if(bottom > m_rcBorder.bottom)
		bottom = m_rcBorder.bottom;

	if(left >= right || top >= bottom)
		return;

	int ipos, *prep;
	for(ipos = 0; ipos < MAX_DIRTY_SIZE && m_nPosList[ipos] != SFREE; ipos ++);

	if(ipos == MAX_DIRTY_SIZE)
	{
		FKTraceLn("_AddDirtyRect, the buffer is full!");
		return;
	}
	
	m_rcList[ipos].left		= left;
	m_rcList[ipos].right	= right;
	m_rcList[ipos].top		= top;
	m_rcList[ipos].bottom	= bottom;

	m_nCount ++;

	if(m_nLastp == SEND)
	{
		m_nLastp = ipos;
		m_nPosList[ipos] = SEND;
		return;
	}

	int jpos;

	for(prep = &m_nLastp, jpos = m_nLastp; jpos != SEND && m_rcList[jpos].top < top; prep = &m_nPosList[jpos], jpos = *prep);
	for(; jpos != SEND && m_rcList[jpos].top <= top && m_rcList[jpos].left < left; prep = &m_nPosList[jpos], jpos = *prep);
	*prep = ipos;
	m_nPosList[ipos] = jpos;

}
//------------------------------------------------------------------------
RECT* CDirtyList::SeekFirstDirty()
{
	if(m_nLastp == SEND)
		return NULL;

	m_nCurp = m_nLastp;
	m_pPrev = &m_nLastp;

	return &m_rcList[m_nCurp];
}
//------------------------------------------------------------------------
RECT* CDirtyList::SeekNextDirty()
{
	ASSERT(m_nCurp >= 0);

	m_pPrev = &m_nPosList[m_nCurp];
	m_nCurp = m_nPosList[m_nCurp];

	if(m_nCurp == SEND)
		return NULL;

	return &m_rcList[m_nCurp];
}
//------------------------------------------------------------------------
void CDirtyList::AddDirtyRect(RECT& rc)
{
	_AddDirtyRect(rc.left, rc.top, rc.right, rc.bottom);
	
}
//------------------------------------------------------------------------
//合并，整理矩形
void CDirtyList::PackDirty()
{

	if(m_bWhole || m_nCount < 2)
		return;
	
	int ipos, jpos, *pre_ipos;

	BOOL bEnd = FALSE;
	while(!bEnd)
	{
		bEnd = TRUE;

		for(ipos = m_nLastp, pre_ipos = &m_nLastp; ipos != SEND && m_nPosList[ipos] != SEND; pre_ipos = &m_nPosList[ipos], ipos = *pre_ipos)
		{
			for(jpos = m_nPosList[ipos]; jpos != SEND; jpos = m_nPosList[jpos])
			{
				RECT& A = m_rcList[ipos];
				RECT& B = m_rcList[jpos];

				if((A.right == B.left) && (A.bottom > B.top ) )
				{
					bEnd = FALSE;

					if(A.top < B.top)
					{
						if(A.bottom > B.bottom)
						{
							AddDirtyRectDirect(A.left, B.bottom, A.right, A.bottom);
							A.bottom = B.top;
							B.left = A.left;
						}
						else if(A.bottom < B.bottom)
						{
							AddDirtyRectDirect(B.left, A.bottom, B.right, B.bottom);
							B.bottom = A.bottom;
							A.bottom = B.top;
							B.left = A.left;
						}
						else
						{
							A.bottom = B.top;
							B.left = A.left;
						}
						
					}
					else	//二个top相等
					{
						if(A.bottom > B.bottom)
						{
							B.left = A.left;

							//Delete A
							*pre_ipos = m_nPosList[ipos];
							m_nPosList[ipos] = SFREE;
							ipos = *pre_ipos;

							m_nCount --;
							//jpos = m_nPosList[ipos];

							AddDirtyRectDirect(A.left, B.bottom, A.right, A.bottom);
							
						}
						else if(A.bottom < B.bottom)
						{
							int bottom = B.bottom;
							int left = B.left;

							B.bottom = A.bottom;
							B.left = A.left;

							//del A
							*pre_ipos = m_nPosList[ipos];
							m_nPosList[ipos] = SFREE;
							ipos = *pre_ipos;

							m_nCount --;
							//jpos = m_nPosList[ipos];

							//Add new
							AddDirtyRectDirect(left, A.bottom, B.right, bottom);

						}
						else
						{
							B.left = A.left;

							*pre_ipos = m_nPosList[ipos];
							m_nPosList[ipos] = SFREE;
							ipos = *pre_ipos;

							m_nCount --;
							//jpos = m_nPosList[ipos];
						}
					}
				}
			}
		}
	}
}
//------------------------------------------------------------------------
void CDirtyList::PackDirty(int x, int y, int w, int h)
{
	if(m_nCount < 2)
		return;

	int r = x + w;
	int b = y + h;

	for(int curp = m_nLastp, *prep = &m_nLastp; curp != SEND;)
	{
		RECT& rc = m_rcList[curp];

		if(rc.left >= x && rc.top >= y && rc.right <= r && rc.bottom <= b)
		{
			*prep = m_nPosList[curp];
			m_nPosList[curp] = SFREE;
			curp = *prep;
			m_nCount --;
			continue;
		}
		prep = &m_nPosList[curp];
		curp = *prep;

	}

	_AddDirtyRect(x, y, r, b);
}
//------------------------------------------------------------------------
void CDirtyList::RemoveDirty(RECT& rc)
{
	if(m_nCount == 0)
		return;

	RECT A;

	for(int curp = m_nLastp, *prep = &m_nLastp; curp != SEND; )
	{
		A = m_rcList[curp];
		if(rc.left < A.right && rc.right > A.left && rc.top < A.bottom && rc.bottom > A.top)
		{
			//Delete the rect
			*prep = m_nPosList[curp];
			m_nPosList[curp] = SFREE;
			m_nCount --;

			//Top
			if(A.top < rc.top)
				AddDirtyRectDirect(A.left, A.top, A.right, rc.top);
			//Left
			if(A.left < rc.left)
				AddDirtyRectDirect(A.left, MAX(rc.top, A.top), rc.left, MIN(A.bottom,rc.bottom));
			//Right
			if(A.right > rc.right)
				AddDirtyRectDirect(rc.right, MAX(rc.top, A.top), A.right, MIN(A.bottom,rc.bottom));
			//Bottom
			if(A.bottom > rc.bottom)
				AddDirtyRectDirect(A.left, rc.bottom, A.right, A.bottom);

			curp = *prep;
			
			continue;
		}
		prep = &m_nPosList[curp];
		curp = *prep;
	}
}
//------------------------------------------------------------------------
void CDirtyList::AddDirtyList(CDirtyList* pList)
{
	RECT* pRc = pList->SeekFirstDirty();

	while(pRc)
	{
		AddDirtyRect(*pRc);
		pRc = pList->SeekNextDirty();
	}
}
//------------------------------------------------------------------------
BOOL CDirtyList::IsDirty(RECT& rc)
{
	if(m_bWhole)
		return TRUE;

	RECT rcTemp = rc, rcTemp2;

	if(!IntersectRect(&rcTemp, &m_rcBorder, &rcTemp))
		return FALSE;

	RECT* pRc = SeekFirstDirty();

	while(pRc)
	{
		if(IntersectRect(&rcTemp2, &rcTemp, pRc))
			return TRUE;

		if(pRc->top > rcTemp.bottom)
			break;

		pRc = SeekNextDirty();
	}
	return FALSE;
}
//------------------------------------------------------------------------
void CDirtyList::Scroll(int x, int y)
{
	RECT* pRc = SeekFirstDirty();

	while(pRc)
	{
		pRc->left -= ABS(x);
		pRc->right += ABS(x);
		pRc->top -= ABS(y);
		pRc->bottom += ABS(y);
		IntersectRect(pRc, pRc, &m_rcBorder);

		pRc = SeekNextDirty();
	}
}
//------------------------------------------------------------------------
void CDirtyList::AddWholeScreen()
{
	if(m_bWhole)
		return;
	if(m_nCount > 0)
		_ClearDirty();
	m_bWhole = TRUE;
};
//------------------------------------------------------------------------