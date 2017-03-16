/**
*	created:		2012-7-2   2:03
*	filename: 		FrameControl
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		帧控制类
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CFrameControl
{
	struct stFrameInfo
	{
		int nFrameIdx;		// 帧索引
		int nLoopCount;		// 帧循环次数
	};
	stFrameInfo*	m_pFrameInfoList;	// 帧信息列表
	int				m_nCounts;			// 需要进行帧控制的数目，即帧信息列表的长度
	int				m_nCurPos;			// 帧控制当前进行到了哪个位置?
	int				m_nLoopCounts;		// 帧循环计数
	
public:
	CFrameControl() : m_pFrameInfoList(NULL), m_nCounts(0), m_nCurPos(0), m_nLoopCounts(0) {}
	virtual ~CFrameControl() {Clear();}
	void InitFrameInfoList(int nCounts) // 初始化帧信息列表
	{
		if (nCounts > m_nCounts)
		{
			delete[] m_pFrameInfoList;
			m_pFrameInfoList = new stFrameInfo[nCounts];
		}
		Clear();
		memset(m_pFrameInfoList, 0xFF, sizeof(stFrameInfo)*nCounts);
		ASSERT(m_pFrameInfoList);
		m_nCurPos = nCounts;
	}
	void Clear()
	{
		m_nCounts = m_nCurPos = m_nLoopCounts = 0;
	}
	void Close()
	{
		SAFE_DELETE_ARRAY(m_pFrameInfoList);
		m_nCounts = m_nCurPos = m_nLoopCounts = 0;
	}
	bool IsValid()
	{
		return (m_pFrameInfoList!=NULL && m_nCounts!=0);
	}
	void AddFrameInfo(int nFrame, int nLoopCount) // 添加帧信息
	{
		if (m_nCurPos > 0) // 添加完毕
		{
			m_pFrameInfoList[m_nCounts].nFrameIdx  = nFrame;
			m_pFrameInfoList[m_nCounts].nLoopCount = nLoopCount;
			m_nCounts ++;
			m_nCurPos --;
		}
	}
	bool Eval(int& nCurFrame, int& nFrames) // 帧数回绕时，返回true
	{
		if (m_pFrameInfoList && m_pFrameInfoList[m_nCurPos].nFrameIdx == nCurFrame)
		{
			if (++m_nLoopCounts >= m_pFrameInfoList[m_nCurPos].nLoopCount)
			{
				m_nLoopCounts = 0;
				nCurFrame ++;
				if (++m_nCurPos >= m_nCounts)
				{
					m_nCurPos = 0;
					if (nCurFrame >= nFrames)
					{
						nCurFrame = 0;
						return true;
					}
				}
			}
		}
		else
		{
			if (++nCurFrame >= nFrames)
			{
				nCurFrame = 0;
				return true;
			}
		}
		return false;
	}
};
//------------------------------------------------------------------------