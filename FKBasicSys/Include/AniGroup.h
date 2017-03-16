/**
*	created:		2012-7-2   1:14
*	filename: 		AniGroup
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Ani.h"
#include <map>
#include "../../FKGame/Common/CreatureViewDef.h"
#include "AniGroupLoader.h"
//------------------------------------------------------------------------
//动画包组文件结构图
/*--------------------------------------------------------------------------
文件头			包含文件版本等
信息			包含文件中有多少动画等
所有动画的索引	包含文件中所有的动画的索引
所有帧的索引	包含文件中所有的帧（指所有动画的帧数加在一起）索引
数据			数据区
--------------------------------------------------------------------------*/
enum PalType
{
	ptFrozen=0, // 冰冻
	ptToxic,	// 中毒
	ptBerserk,	// 狂暴
	ptType1,	// 类型1
	ptType2,
	ptType3,
	ptType4,
	ptType5,

	ptMaxPalTypeCount,
};
//------------------------------------------------------------------------
class CMultiParticleMask;
class CParticle;
class CParticleMask;
//------------------------------------------------------------------------
class CAniGroup : public IPoolNodeObject // 40 bytes
{
public:
	typedef std::map<int,CMultiParticleMask*> MaskSet;
	typedef MaskSet::iterator mask_iterator;
	
public:
	enLoadState					m_LoadState[CrtStateMax];
	DWORD						m_dwResID;

	IBitmap**					m_ppaState;			// 指向每个状态的图象数据
	IBitmap**					m_ppaIBitmap;		// 帧图形数据
	BYTE*						m_pPal;				// 公用调色板
	stGroupInfoHeader			m_GroupInfoHdr;		// 组信息头
	stAniIndex*					m_paAniIndex;		// 动画索引
	stFrameIndex*				m_paFrameIndex;		// 动画帧索引
	IFileObject*				m_pFile;			// 文件对象
	DWORD						m_nDataOffset;		// 数据偏移
	BYTE*						m_pUserPal[ptMaxPalTypeCount]; // 这里不new，直接指向二进制脚本里面的地址
	int							m_nMaxUserPalCount;
	MaskSet						m_setMask;
	DWORD						m_dwSize;
public:
	CAniGroup();
	virtual ~CAniGroup();
	void Draw(ICanvas *pCanvas, int nX, int nY, int nAni, int nFrame, int nFlag, BYTE nAlpha = 0, BYTE* pPal=NULL, CParticle * pParticle=NULL);
	bool Open(IFileSystem* pFileSystem, LPCSTR szName, DWORD dwResID);

	// 动画组属性
	int GetAniCount(){return m_GroupInfoHdr.total_ani;}
	int GetTotalFrames(){return m_GroupInfoHdr.total_frame;}

	// 动画属性
	int	GetTotalFrames(int nAni);

	// 帧属性
	void GetOffset(int nAni, int nFrame, int& nOffsetX, int& nOffsetY);
	int GetWidth(int nAni, int nFrame);
	int GetHeight(int nAni, int nFrame);
	IBitmap* GetBitmap(int nAni, int nFrame);

	// 调色板相关
	BYTE* GetPal()							{return m_pPal;}
	int GetUseColor()						{return m_GroupInfoHdr.use_color;}
	BYTE* GetUserPal(int nIndex)			{return m_pUserPal[nIndex];}
	BYTE** GetUserPal()						{return m_pUserPal;}
	void SetMaxUserPalCount(int nCount)		{m_nMaxUserPalCount=nCount;}
	int  GetMaxUserPalCount()				{return m_nMaxUserPalCount;}

	virtual DWORD GetSize(){return m_dwSize;}
	virtual void Release(){delete this;}
	
	BOOL GetStateData(int nState, BYTE* pBuffer, DWORD dwBufferSize);
	DWORD GetStateSize(int nState);
	IBitmap* GetStateBitmap(int nState);
	CMultiParticleMask * GetParticleMask(CParticle * pParticle,int cFrame,BYTE * pPal);
};
//------------------------------------------------------------------------
// 动画上下文
struct SAniContext
{
	//BYTE nDelay;		// 帧延时，即速度
	int nType;			// 生物类型，华夏2特有
	int nFrames;		// 帧数,不超过255帧
	int nCurFrame;		// 当前是第几帧
	int nState;			// 当前状态
	int nDir;			// 当前方向

	SAniContext()			{Clear();}
	void Clear()			{memset(this,0,sizeof(SAniContext));}
	void NextFrame()		{if(++nCurFrame>=nFrames)nCurFrame=0;}
	bool IsPlayOver()		{return nCurFrame > nFrames;}
	int  State()			{return (int)nState;}
	int  Dir()				{return (int)nDir;}
	int  CurFrame()			{return (int)nCurFrame;}
	int  Frames()			{return (int)nFrames;}
	bool IsState(int state) {return (int)nState==state;}
	bool IsDir(int dir)		{return (int)nDir==dir;}
	bool Equal(SAniContext *pAniContext)
	{
		if(nCurFrame != pAniContext->nCurFrame
		|| nDir != pAniContext->nDir
		|| nState != pAniContext->nState
		|| nType != pAniContext->nType)
			return FALSE;
		return TRUE;
	}
	void SetData(SAniContext *pAniContext)
	{
		nType		= pAniContext->nType;
		nFrames		= pAniContext->nFrames;
		nCurFrame	= pAniContext->nCurFrame;
		nState		= pAniContext->nState;
		nDir		= pAniContext->nDir;
	}
};
//------------------------------------------------------------------------
#define cur_ani ((m_AniContext.nState<<3)+m_AniContext.nDir)
//------------------------------------------------------------------------	