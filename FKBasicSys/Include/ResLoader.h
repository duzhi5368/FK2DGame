/**
*	created:		2012-7-1   19:47
*	filename: 		ResLoader
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		管理所有脚本的装载与读取
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#ifdef _DEBUG
#define __USE_MALLOC
#endif
//------------------------------------------------------------------------
#include <map>
using std::map;
//------------------------------------------------------------------------
#include "../../FKGame/Common/CreatureViewDef.h"
#include "ObjPool.h"
//------------------------------------------------------------------------
#define				MAX_CREATURE_TYPE		200		// 最大生物种族数
#define				MAX_DELAY_TYPE			9		// 最大帧延时等级
#define				MAX_SPEED_GRADE_TYPE	9		// 最大速度等级（走路，跑步，骑马，攻击等的等级）
#define				MAX_STEP_TYPE			3		// 最大移动步长种类（走路，跑步，骑马……)

// 本模块加载的所有脚本类型
enum enScriptTypeID
{
	stBmp = 0,
	stAni,
	stMagic,
	stNpc,
	stMonster,
	stActor,
	stSkill
};

// 帧排序二进制文件结构
struct STypeContext						//单个类型上下文
{
	BYTE			m_nX;				//行，实际状态（动作）数
	BYTE			m_nY;				//列，方向数，一般是8
	BYTE			m_nZ;				//中轴，23个状态（动作）中单个状态最大帧数。
	BYTE			m_nIndexWide;		//保存的排序数组宽度，也就是实际部件数（特效除外）
	BYTE			m_nIndexCount;		//保存了几个排序索引
};

// 各种资源的ID偏移
enum IDOffset
{
	ioStatic	= 0,
	ioAni		= 200000,
	ioMagic		= 300000,
	ioCreature	= 400000,
};

struct SMonsterViewCreateParam;
struct SMagicCreateParam;
struct SAniObjCreateParam;
struct SStaticObjCreateParam;
struct SAniContext;
class CResLoader  
{	
public:
	CObjectPool<>		m_ObjectPool;
//下面的都是fsp文件系统。里面是图包资源
	IFileSystem*		m_pfsBmp;
	IFileSystem*		m_pfsAni;
	IFileSystem*		m_pfsMagic;
	IFileSystem*		m_pfsCreature;

#ifdef _USE_BINSCRIPT
	IBinScript*			m_pScpBmp;
	IBinScript*			m_pScpBodyPart;
	IBinScript*			m_pScpCreature;
	IBinScript*			m_pScpAni;
#else
	IResScript*			m_pScpBmp;
	IResScript*			m_pScpBodyPart;
	IResScript*			m_pScpCreature;
	IResScript*			m_pScpAni;
#endif
	//怪物多站位脚本
	IResScript*			m_pScpNpcOcc;
	IResScript*			m_pScpMagic;
	IResScript*			m_pScpSkill;
	IResScript*			m_pScpColorList;

private:
	//帧排序文件结构以及数据保存
	BYTE					*m_pFrameSort[MAX_CREATURE_TYPE];	//保存排序列表
	BYTE					m_nTypes;							//生物类型数，不算0，从1开始，因为是记录有几个类型，不是第几个类型
	STypeContext			*m_tcContext[MAX_CREATURE_TYPE];	//保存文件头信息
	BYTE					*m_pTypeIndex[MAX_CREATURE_TYPE];	//保存排序索引
	//保存帧延时等级
	int*					m_FrameDelay[MAX_CREATURE_TYPE];	//保存帧延时等级数据，9个等级，这里只是保存指针
	int*					m_WalkGrade[MAX_CREATURE_TYPE];		//保存走路速度等级数据，9个等级，这里只是保存指针
	int*					m_RunGrade[MAX_CREATURE_TYPE];		//保存跑步速度等级数据，9个等级，这里只是保存指针
	int*					m_RideGrade[MAX_CREATURE_TYPE];		//保存骑马速度等级数据，9个等级，这里只是保存指针
	int*					m_AttackGrade[MAX_CREATURE_TYPE];	//保存攻击速度等级数据，9个等级，这里只是保存指针
	float*					m_fStep[MAX_CREATURE_TYPE];			//保存Step
	BYTE*					m_DelayIndex[MAX_CREATURE_TYPE];	//保存帧延时索引
	BYTE*					m_StateIndex[MAX_CREATURE_TYPE];	//保存状态索引
	BYTE*					m_AttackIndex[MAX_CREATURE_TYPE];	//保存状态攻击祯索引
	BYTE*					m_AttackSoundIndex[MAX_CREATURE_TYPE];	//保存状态攻击音效播放祯索引
	int						m_nHalfHeigh[MAX_CREATURE_TYPE];	//保存生物半高
	int						m_nWholeHeight[MAX_CREATURE_TYPE];	//保存生物全高
	BYTE					m_nFallowStateID[MAX_CREATURE_TYPE];//保存休闲动作ID
	int						m_nBeHitLoopBegin[MAX_CREATURE_TYPE];//休闲动作循环开始帧
	int						m_nBeHitLoopEnd[MAX_CREATURE_TYPE];//休闲动作循环开始帧

	BYTE*					m_bytePartFlag[MAX_CREATURE_TYPE];	//保存“部件标志”数据 只有生物编辑器用到
	POINT					m_ptAnchor[MAX_CREATURE_TYPE];		//保存锚点

public:
	CResLoader();
	virtual ~CResLoader();
	void Close();
	BOOL InitFileSystem(IClientGlobal* pGlobal);
	BOOL InitDelayAndStep(IClientGlobal* pGlobal);
	//专用读取帧排序脚本
	BOOL LoadFrameSort(IClientGlobal* pGlobal);
	
	// 判断状态是否存在
	BOOL GetStateEnable(int nType, int nState);

	//根据帧延时等级获取对应的走路祯延时
	int GetWalkDelayByGrade(int nType, int nGrade);
	//根据帧延时等级获取对应的跑步祯延时
	int GetRunDelayByGrade(int nType, int nGrade);
	//根据帧延时等级获取对应的骑马祯延时
	int GetRideDelayByGrade(int nType, int nGrade);
	//根据帧延时等级获取对应的攻击祯延时
	int GetAttackDelayByGrade(int nType, int nGrade);

	//获取状态攻击祯
	int GetAttackFrame(int nType, int nState);

	//获取攻击音效播放祯
	int GetAttackSoundFrame(int nType, int nState);

	//根据帧延时等级获取对应等级的数据
	int GetDelayByGrade(int nType, int nGrade);
	//读取帧排序
	int GetFrameSort(BYTE *pBuf, SAniContext AniContext);
	//获取帧延时
	int GetFrameDelay(int nType, int nState);
	float GetStep(int nType, int nState);
	//获取生物半高，全高
	int GetHalfHeigh(int nType)
	{
		return m_nHalfHeigh[nType];
	}
	int GetWholeHeigh(int nType)
	{
		return m_nWholeHeight[nType];
	}
	int GetFallowStateID(int nType)
	{
		return m_nFallowStateID[nType];
	}

	int GetBeHitLoopBegin(int nType)
	{
		return m_nBeHitLoopBegin[nType];
	}

	int GetBeHitLoopEnd(int nType)
	{
		return m_nBeHitLoopEnd[nType];
	}

	//获取怪物、NPC的站位方式
	BOOL GetNpcOccupantInfo(DWORD dwResID, unsigned char* buf, int nLen);

	//获取锚点
	void GetAnchor (int nType, POINT& point);
	//加载部件状态标志
	BYTE* GetStateFlag(int nModuleType, DWORD dwID);
	//获取部件调色板
	int* GetHSLpal(DWORD dwID, int nPalType);
	//递归查找ResObj
	IResObject* SearchObj(IResObject* pParent, DWORD dwID);		//根据ID查找,该功能很少用到
	IResObject* SearchObj(IResObject* pParent, LPCSTR pName);	//根据名字查找
	//获取怪物和NPC各个部件对应的ID,上层提供内存
	BOOL GetMonsterbuf(DWORD dwID, DWORD *pModuleID, BYTE *pAlpha, BYTE *pPal);
	//根据排序索引获取部件索引
	int GetPartIndexBySortIndex(int nType, int nSort);

	int ReLoadScript(int nScpTypeID);

	void AddSize(DWORD dwSize);
	IPoolNodeObject* RequestPic(DWORD dwResID);
	IPoolNodeObject* RequestAni(DWORD dwResID);
	IPoolNodeObject* RequestMagic(DWORD dwResID);
	IPoolNodeObject* RequestCreature(DWORD dwResID);
	BOOL ReleaseRes(DWORD dwResID);

public: // 下面是一些与脚本交互的函数
	BOOL GetResID(int nIndex, DWORD& dwResID);

	// 获取物件创建参数
	BOOL GetMonsterCreateParam(DWORD dwMstID, SMonsterViewCreateParam& mvCreateParam);
	BOOL GetMagicCreateParam(DWORD dwResID, SMagicCreateParam& mCreateParam);
	BOOL GetAniObjCreateParam(DWORD dwResID, SAniObjCreateParam& aoCreateParam);
	BOOL GetStaticObjCreateParam(DWORD dwResID, SStaticObjCreateParam& soCreateParam);
	
	BOOL GetAniObjOccupantInfo(DWORD dwResID, unsigned char* buf, int nLen);
	BOOL GetStaticObjOccupantInfo(DWORD dwResID, unsigned char* buf, int nLen);
	int  GetStaticObjOccupantOption(DWORD dwResID);
	BOOL GetActorUserPal(DWORD dwResID, int& nCount, BYTE** ppUserPal);
	BOOL GetMonsterUserPal(DWORD dwResID, int& nCount, BYTE** ppUserPal);

	// 各种类型魔法脚本的读取过程
	int ReadAttachMagic(void* obj, void* buf);
	int ReadBaseMagic(void* obj, void* buf);
	int ReadLifeMagic(void* obj, void* buf);
	int ReadThreeStateMagic(void* obj, void* buf);
	int ReadRainMagic(void* obj, void* buf);
	int ReadRingMagic(void* obj, void* buf);
	int ReadSingleDirMagic(void* obj, void* buf);
	int ReadEightDirMagic(void* obj, void* buf);
	int ReadGeneralMagic(void* obj, void* buf);
	int ReadMouseMagic(void* obj, void* buf);
};
//------------------------------------------------------------------------
extern CResLoader g_ResLoader;
//------------------------------------------------------------------------