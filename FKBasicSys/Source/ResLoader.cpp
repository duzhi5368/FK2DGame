/**
*	created:		2012-7-2   4:02
*	filename: 		ResLoader
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/ResLoader.h"
#include "../Include/Ani.h"
#include "../Include/AniGroup.h"
#include "../Include/AniObj.h"
#include "../Include/StaticObj.h"
#include "../Include/Magic.h"
#include "../Include/ActorView.h"
#include "../Include/MonsterView.h"
#include "../Include/SkillCreate.h"
#include "../Include/ActorTitle.h"
#include "../Include/ParticleSetting.h"
//------------------------------------------------------------------------
CResLoader g_ResLoader;
//------------------------------------------------------------------------
CResLoader::CResLoader()
{
	memset (this, 0, sizeof(this));
}
//------------------------------------------------------------------------
CResLoader::~CResLoader()
{
}
//------------------------------------------------------------------------
void CResLoader::Close()
{
	SAFE_RELEASE(m_pfsBmp);
	SAFE_RELEASE(m_pfsAni);
	SAFE_RELEASE(m_pfsMagic);
	SAFE_RELEASE(m_pfsCreature);

	SAFE_RELEASE(m_pScpBmp);
	SAFE_RELEASE(m_pScpAni);
	SAFE_RELEASE(m_pScpMagic);
	SAFE_RELEASE(m_pScpCreature);
	SAFE_RELEASE(m_pScpBodyPart);

	SAFE_RELEASE(m_pScpSkill);
	SAFE_RELEASE(m_pScpColorList);
	m_ObjectPool.Close();
	for (int i = 0; i < MAX_CREATURE_TYPE; i++)
	{
		SAFE_DELETE(m_tcContext[i]);
		SAFE_DELETE_ARRAY(m_pFrameSort[i]);
		SAFE_DELETE_ARRAY(m_pTypeIndex[i]);
	}
#ifndef _USE_BINSCRIPT		//这个是为了满足Debug版本的需求。两个版本的脚本读取不一样
	for (int j = 0; j < MAX_CREATURE_TYPE; j++)
	{
		SAFE_DELETE_ARRAY(m_FrameDelay[j]);
		SAFE_DELETE_ARRAY(m_WalkGrade[j]);
		SAFE_DELETE_ARRAY(m_RunGrade[j]);
		SAFE_DELETE_ARRAY(m_RideGrade[j]);
		SAFE_DELETE_ARRAY(m_AttackGrade[j]);
		SAFE_DELETE_ARRAY(m_fStep[j]);
		SAFE_DELETE_ARRAY(m_DelayIndex[j]);
		SAFE_DELETE_ARRAY(m_bytePartFlag[j]);
		SAFE_DELETE_ARRAY(m_StateIndex[j]);
		SAFE_DELETE_ARRAY(m_AttackIndex[j]);
		SAFE_DELETE_ARRAY(m_AttackSoundIndex[j]);
	}
#endif
}
//------------------------------------------------------------------------
BOOL CResLoader::InitFileSystem(IClientGlobal* pGlobal)
{
	char buf[MAX_PATH];
	ITrace* pTrace = NULL;// g_TraceHelper.GetTraceObject();
	strcat (pGlobal->m_szWorkDir, "\\Data");//函数结束后还原
	
	// 加载文件系统
	#ifdef _USE_FSP
		DWORD dwFileSysStyle = FSS_PACKAGE;
	#else
		DWORD dwFileSysStyle = FSS_DIRECTORY;
	#endif
	
	#ifdef _USE_FSP
		wsprintf(buf, "%s\\res\\bmp.fsp", pGlobal->m_szWorkDir);
	#else
		wsprintf(buf, "%s\\res\\bmp", pGlobal->m_szWorkDir);
	#endif
		if (!g_fsHelper.Create(&m_pfsBmp, buf, dwFileSysStyle, pTrace))
		{
			PrintEventLn("ERROR: Init Filesystem, file system create failed[%s]", buf);
			return FALSE;
		}

	#ifdef _USE_FSP
		wsprintf(buf, "%s\\res\\ani.fsp", pGlobal->m_szWorkDir);
	#else
		wsprintf(buf, "%s\\res\\ani", pGlobal->m_szWorkDir);
	#endif
		if (!g_fsHelper.Create(&m_pfsAni, buf, dwFileSysStyle, pTrace))
		{
			PrintEventLn("ERROR: Init Filesystem, file system create failed[%s]", buf);
			return FALSE;
		}

	#ifdef _USE_FSP
		wsprintf(buf, "%s\\res\\magic.fsp", pGlobal->m_szWorkDir);
	#else
		wsprintf(buf, "%s\\res\\magic", pGlobal->m_szWorkDir);
	#endif
		if (!g_fsHelper.Create(&m_pfsMagic, buf, dwFileSysStyle, pTrace))
		{
			PrintEventLn("ERROR: Init Filesystem, file system create failed[%s]", buf);
			return FALSE;
		}

	#ifdef _USE_FSP
		wsprintf(buf, "%s\\res\\BodyPart.fsp", pGlobal->m_szWorkDir);
	#else
		wsprintf(buf, "%s\\res\\BodyPart", pGlobal->m_szWorkDir);
	#endif
		if (!g_fsHelper.Create(&m_pfsCreature, buf, dwFileSysStyle, pTrace))
		{
			PrintEventLn("ERROR: Init Filesystem, file system create failed[%s]", buf);
			return FALSE;
		}

	// 加载脚本文件
	#ifdef _DEBUG
		DWORD dwStart=GetTickCount();
		BYTE pKey[] = "abcd";
		wsprintf(buf, "%s\\scp\\bmp.rcf", pGlobal->m_szWorkDir);
		m_pScpBmp = g_pResMgr->LoadBinScript(buf);
		if (!m_pScpBmp)
		{
			PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
			return FALSE;
		}

		wsprintf(buf, "%s\\scp\\ani.rcf", pGlobal->m_szWorkDir);
		m_pScpAni = g_pResMgr->LoadBinScript(buf);
		if (!m_pScpAni)
		{
			PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
			return FALSE;
		}
		
		wsprintf(buf, "%s\\scp\\magic.scp", pGlobal->m_szWorkDir);
		m_pScpMagic = g_pResMgr->LoadResScript(buf);
		if (!m_pScpMagic)
		{
			PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
			return FALSE;
		}
		
		wsprintf(buf, "%s\\scp\\Creature.rcf", pGlobal->m_szWorkDir);
		m_pScpCreature = g_pResMgr->LoadBinScript(buf);
		if (!m_pScpCreature)
		{
			PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
			return FALSE;
		}
		
		wsprintf(buf, "%s\\scp\\BodyPart.rcf", pGlobal->m_szWorkDir);
		m_pScpBodyPart = g_pResMgr->LoadBinScript(buf);
		if (!m_pScpBodyPart)
		{
			PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
			return FALSE;
		}
		
		wsprintf(buf, "%s\\scp\\skill_view.scp", pGlobal->m_szWorkDir);
		m_pScpSkill = g_pResMgr->LoadResScript(buf);
		if (!m_pScpSkill)
		{
			PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
			return FALSE;
		}
		
		//怪物、NPC多站位脚本
		wsprintf(buf, "%s\\scp\\NpcOcc.scp", pGlobal->m_szWorkDir);
		m_pScpNpcOcc = g_pResMgr->LoadResScript(buf);
		if (!m_pScpNpcOcc)
		{
			PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
			return FALSE;
		}
		TraceLn("加载资源耗时: %d ms", GetTickCount()-dwStart);
	
	#else // Release
			BYTE pKey[] = "abcd";
		#ifdef _USE_BINSCRIPT
			wsprintf(buf, "%s\\scp\\bmp.rcf", pGlobal->m_szWorkDir);
			m_pScpBmp = g_pResMgr->LoadBinScript(buf);	
		#else
			wsprintf(buf, "%s\\scp\\bmp.res", pGlobal->m_szWorkDir);
			m_pScpBmp = g_pResMgr->LoadResScript(buf, FALSE, pKey);
		#endif
			if (!m_pScpBmp)
			{
				PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
				return FALSE;
			}
		#ifdef _USE_BINSCRIPT
			wsprintf(buf, "%s\\scp\\ani.rcf", pGlobal->m_szWorkDir);
			m_pScpAni = g_pResMgr->LoadBinScript(buf);	
		#else
			wsprintf(buf, "%s\\scp\\ani.res", pGlobal->m_szWorkDir);
			m_pScpAni = g_pResMgr->LoadResScript(buf, FALSE, pKey);
		#endif
			if (!m_pScpAni)
			{
				PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
				return FALSE;
			}
			wsprintf(buf, "%s\\scp\\magic.res", pGlobal->m_szWorkDir);
			m_pScpMagic = g_pResMgr->LoadResScript(buf, FALSE, pKey);
			if (!m_pScpMagic)
			{
				PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
				return FALSE;
			}
		#ifdef _USE_BINSCRIPT
			wsprintf(buf, "%s\\scp\\Creature.rcf", pGlobal->m_szWorkDir);
			m_pScpCreature = g_pResMgr->LoadBinScript(buf);
		#else
			wsprintf(buf, "%s\\scp\\Creature.res", pGlobal->m_szWorkDir);
			m_pScpCreature = g_pResMgr->LoadResScript(buf, FALSE, pKey);
		#endif
			if (!m_pScpCreature)
			{
				PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
				return FALSE;
			}
	
		#ifdef _USE_BINSCRIPT
			wsprintf(buf, "%s\\scp\\BodyPart.rcf", pGlobal->m_szWorkDir);
			m_pScpBodyPart = g_pResMgr->LoadBinScript(buf);
		#else
			wsprintf(buf, "%s\\scp\\BodyPart.res", pGlobal->m_szWorkDir);
			m_pScpBodyPart = g_pResMgr->LoadResScript(buf, FALSE, pKey);
		#endif
			if (!m_pScpBodyPart)
			{
				PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
				return FALSE;
			}
			wsprintf(buf, "%s\\scp\\skill_view.res", pGlobal->m_szWorkDir);
			m_pScpSkill = g_pResMgr->LoadResScript(buf, FALSE, pKey);
			if (!m_pScpSkill)
			{
				PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
				return FALSE;
			}
	
			wsprintf(buf, "%s\\scp\\ColorList.res", pGlobal->m_szWorkDir);
			m_pScpColorList = g_pResMgr->LoadResScript(buf, FALSE, pKey);
			if (!m_pScpColorList)
			{
				PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
				return FALSE;
			}
			//怪物多站位脚本
			wsprintf(buf, "%s\\scp\\NpcOcc.res", pGlobal->m_szWorkDir);
			m_pScpNpcOcc = g_pResMgr->LoadResScript(buf, FALSE, pKey);
			if (!m_pScpNpcOcc)
			{
				PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
				return FALSE;
			}
			// Init Color List
			IResObject* pRoot = m_pScpColorList->GetResObjectRoot();
			int nCount = pRoot->GetSubObjectCount();
			for (int i=0; i<nCount; i++)
			{
				IResObject* pObj = pRoot->GetSubObjectByIndex(i);
				if (pObj)
				{
					int rgb[3];
					pObj->GetArrayLong("color", (long*)rgb, 3);
					CActorTitle::m_ColorList.push_back(WPixel(rgb[0], rgb[1], rgb[2]));
				}
			}
		
			PrintEventLn(".");
			wsprintf(buf,"%s\\scp\\particle.res",pGlobal->m_szWorkDir);
			IResScript * pResParticle = g_pResMgr->LoadResScript(buf,FALSE,pKey);
			if(!pResParticle)
			{
				PrintEventLn("ERROR: Init filesystem, script load failed[%s]",buf);
				return false;
			}
			if(!CParticleSetting::GetInstance()->Initialize(pResParticle))
			{
				PrintEventLn("ERROR:初始化效果数据失败");
				return false;
			}


	#endif // _DEBUG
	if(!LoadFrameSort(pGlobal))	//读取帧排序脚本，因为是二进制脚本，所以Release和Debug都是一样的
	{
		PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", "FameSort.Dat");
		return FALSE;
	}
	
	//还原m_szWorkDir
	int nLen = strlen (pGlobal->m_szWorkDir);
	while (nLen > 0)
	{
		if (pGlobal->m_szWorkDir[nLen] == '\\')
		{
			pGlobal->m_szWorkDir[nLen] = '\0';
			break;
		}
		pGlobal->m_szWorkDir[nLen] = '\0';
		nLen--;
	}
	return TRUE;
}
//------------------------------------------------------------------------
//专门读取帧排序脚本
BOOL CResLoader::LoadFrameSort(IClientGlobal* pGlobal)
{
	FILE *fp = NULL;

	char buf[256];
	memset (buf, 0, 256);
	wsprintf(buf, "%s\\scp\\FrameSort.Dat", pGlobal->m_szWorkDir);
	fp = fopen (buf, "rb");
	if (!fp)
	{
		PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
		return FALSE;
	}
	//读取文件头结构体，文件数据区，排序List
	fread(&m_nTypes, 1, sizeof(BYTE), fp);
	if (m_nTypes == 0)
	{
		fclose (fp);
		PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
		return FALSE;
	}
	for (BYTE i = 0; i < m_nTypes; i++)
	{
		m_tcContext[i] = new STypeContext;
		int nPos = 1 + i * sizeof(STypeContext);
		int nByte = sizeof(STypeContext);//测试代码
		if (fseek (fp, nPos, SEEK_SET) != 0)
		{
			fclose(fp);
			return FALSE;
		}
		fread(m_tcContext[i], 1, sizeof(STypeContext), fp);
		int x = m_tcContext[i]->m_nX;		
		int y = m_tcContext[i]->m_nY;
		int z = m_tcContext[i]->m_nZ;
		int count = m_tcContext[i]->m_nIndexCount;
		int wide = m_tcContext[i]->m_nIndexWide;
		m_pFrameSort[i] = new BYTE[(m_tcContext[i]->m_nIndexCount * m_tcContext[i]->m_nIndexWide)];
		nPos = 1 + m_nTypes * sizeof(STypeContext);
		int nOffset = 0;
		for (BYTE j = 0; j < i; j++)
		{
			nOffset += (m_tcContext[j]->m_nX * m_tcContext[j]->m_nY * m_tcContext[j]->m_nZ + m_tcContext[j]->m_nIndexWide * m_tcContext[j]->m_nIndexCount);
		}
		nPos += nOffset;
		if (fseek(fp, nPos, SEEK_SET) != 0)
		{
			fclose(fp);
			return FALSE;
		}
		fread(m_pFrameSort[i], 1, m_tcContext[i]->m_nIndexCount * m_tcContext[i]->m_nIndexWide, fp);
		nPos += m_tcContext[i]->m_nIndexCount * m_tcContext[i]->m_nIndexWide;
		if (fseek(fp, nPos, SEEK_SET) != 0)
		{
			fclose(fp);
			return FALSE;
		}
		m_pTypeIndex[i] = new BYTE[m_tcContext[i]->m_nX*m_tcContext[i]->m_nY*m_tcContext[i]->m_nZ];
		fread(m_pTypeIndex[i], 1, m_tcContext[i]->m_nX*m_tcContext[i]->m_nY*m_tcContext[i]->m_nZ, fp);
	}
	fclose(fp);
	//读取并保存帧延时等级数据
#ifdef _USE_BINSCRIPT
	HANDLE hParent = m_pScpCreature->GetObject(0);
	if (hParent == NULL)
	{
		PrintEventLn("ERROR: Init Filesystem, script load failed[%s]", buf);
		return FALSE;
	}
	int nCount = m_pScpCreature->GetSubObjectCount(hParent);
	HANDLE hChild = m_pScpCreature->GetSubObject(hParent, 0);
	if (hChild == NULL)
		return FALSE;
#pragma pack(push)
#pragma pack(1)
	struct enCreatureModule
	{
		int		Anchor[2];
		int		nHalfHeightOffset;
		int		nHeightOffset;
		float	Step[MAX_STEP_TYPE];
		int		FrameDelay[MAX_DELAY_TYPE];
		int		WalkGrade[MAX_SPEED_GRADE_TYPE];
		int		RunGrade[MAX_SPEED_GRADE_TYPE];
		int		RideGrade[MAX_SPEED_GRADE_TYPE];
		int		AttackGrade[MAX_SPEED_GRADE_TYPE];
		BYTE	FrameDelayIndex[CrtStateMax];
		BYTE	StateIndex[CrtStateMax];
		BYTE	AttackIndex[CrtStateMax];
		BYTE	AttackSoundIndex[CrtStateMax];
		BYTE	FallowStateID;
		int		BehitStateLoop[2];
	};
	enCreatureModule *pCrtObj = NULL;
	nCount = m_pScpCreature->GetSubObjectCount(hChild);
#pragma pack(pop)
	for ( int i = 0; i < nCount; i++)
	{
		HANDLE hType = m_pScpCreature->GetSubObject(hChild, i);
		if (hType == NULL)
			break;
		BYTE *pBuf = m_pScpCreature->GetBuffer(hType);
		int nSize = m_pScpCreature->GetSize(hType);
		int nLen = sizeof(enCreatureModule);
		pCrtObj = (enCreatureModule *)pBuf;

		m_ptAnchor[i].x		= pCrtObj->Anchor[0];
		m_ptAnchor[i].y		= pCrtObj->Anchor[1];
		m_nHalfHeigh[i]		= pCrtObj->nHalfHeightOffset;
		m_nWholeHeight[i]	= pCrtObj->nHeightOffset;
		m_fStep[i]			= pCrtObj->Step;
		m_FrameDelay[i]		= pCrtObj->FrameDelay;
		m_WalkGrade[i]		= pCrtObj->WalkGrade;
		m_RunGrade[i]		= pCrtObj->RunGrade;
		m_RideGrade[i]		= pCrtObj->RideGrade;
		m_AttackGrade[i]	= pCrtObj->AttackGrade;
		m_DelayIndex[i]		= pCrtObj->FrameDelayIndex;
		m_StateIndex[i]		= pCrtObj->StateIndex;
		m_AttackIndex[i]	= pCrtObj->AttackIndex;
		m_AttackSoundIndex[i]	= pCrtObj->AttackSoundIndex;
		m_nFallowStateID[i]	= pCrtObj->FallowStateID;
		m_nBeHitLoopBegin[i] = pCrtObj->BehitStateLoop[0];
		m_nBeHitLoopEnd[i] = pCrtObj->BehitStateLoop[1];
	}
#else
	IResObject* pRoot = m_pScpCreature->GetResObjectRoot();
	IResObject* pResObj = pRoot->GetSubObjectByIndex(0);
	IResObject* pResSubObj = NULL;
	for (int i = 0; i < MAX_CREATURE_TYPE; i++)
	{
		pResSubObj = pResObj->GetSubObjectByIndex(i);
		if (pResSubObj == NULL)
			break;

		m_FrameDelay[i] = new int[MAX_DELAY_TYPE];
		pResSubObj->GetArrayLong("FrameDelay", (long*)&m_FrameDelay[i]);

		m_WalkGrade[i] = new int[MAX_SPEED_GRADE_TYPE];
		pResSubObj->GetArrayLong("WalkGrade", (long*)&m_WalkGrade[i]);

		m_RunGrade[i] = new int[MAX_SPEED_GRADE_TYPE];
		pResSubObj->GetArrayLong("RunGrade", (long*)&m_RunGrade[i]);

		m_RideGrade[i] = new int[MAX_SPEED_GRADE_TYPE];
		pResSubObj->GetArrayLong("RideGrade", (long*)&m_RideGrade[i]);

		m_AttackGrade[i] = new int[MAX_SPEED_GRADE_TYPE];
		pResSubObj->GetArrayLong("AttackGrade", (long*)&m_AttackGrade[i]);

		m_fStep[i] = new float[MAX_STEP_TYPE];
		//下面这个处理非常难看，主要原因是GetArrayFloat这个函数返回的是double指针，
		//而，sizeof(float) = 4，sizeof(double) = 8，强制转换怕有问题，只有暂时这样写了
		double *p = new double[MAX_STEP_TYPE];
		if(pResSubObj->GetArrayFloat("Step", p) != MAX_STEP_TYPE)
		{
			SAFE_DELETE_ARRAY(p);
			PrintEventLn("ERROR: Init Filesystem, load FrameDelay failed[%s]", buf);
			return FALSE;
		}
		m_fStep[i][0] = p[0];
		m_fStep[i][1] = p[1];
		m_fStep[i][2] = p[2];
		SAFE_DELETE_ARRAY(p);
		m_DelayIndex[i] = new BYTE[CrtStateMax];
		if (pResSubObj->GetArrayByte("FrameDelayIndex", m_DelayIndex[i]) == 0)
		{
			PrintEventLn("ERROR: Init Filesystem, load FrameDelayIndex failed[%s]", buf);
			return FALSE;
		}
		long nBuf[2];//只有x，y
		memset (nBuf, 0, sizeof(int) * 2);
		pResSubObj->GetArrayLong("Anchor", nBuf);
		m_ptAnchor[i].x = nBuf[0];
		m_ptAnchor[i].y = nBuf[1];
		m_bytePartFlag[i] = new BYTE[CrtPartMax];
		if (pResSubObj->GetArrayByte("部件标志", m_bytePartFlag[i]) == 0)
		{
			PrintEventLn("ERROR: Init Filesystem, load 部件标志 failed[%s]", buf);
			return FALSE;
		}
		m_StateIndex[i] = new  BYTE[CrtStateMax];
		if (pResSubObj->GetArrayByte("StateIndex", m_StateIndex[i]) == 0)
		{
			PrintEventLn("ERROR: Init Filesystem, load StateIndex failed[%s]", buf);
			return FALSE;
		}
		m_AttackIndex[i] = new  BYTE[CrtStateMax];
		if (pResSubObj->GetArrayByte("AttackIndex", m_AttackIndex[i]) == 0)
		{
			PrintEventLn("ERROR: Init Filesystem, load AttackIndex failed[%s]", buf);
			return FALSE;
		}
		m_AttackSoundIndex[i] = new  BYTE[CrtStateMax];
		if (pResSubObj->GetArrayByte("AttackSoundIndex", m_AttackSoundIndex[i]) == 0)
		{
			PrintEventLn("ERROR: Init Filesystem, load AttackIndex failed[%s]", buf);
			return FALSE;
		}
	}
#endif
	return TRUE;
}
//------------------------------------------------------------------------
//获取锚点
void CResLoader::GetAnchor(int nType, POINT& point)
{
	point.x = m_ptAnchor[nType].x;
	point.y = m_ptAnchor[nType].y;
}
//------------------------------------------------------------------------
//获取怪物多站位信息
BOOL CResLoader::GetNpcOccupantInfo(DWORD dwResID, unsigned char* buf, int nLen)
{
	IResObject* pStaticObj = m_pScpNpcOcc->GetResObject(dwResID);
	if (!pStaticObj)
		return FALSE;
	
	if (pStaticObj->GetPropertyString("OccupantFlags")) // 有占位信息
	{
		// for occupant
		int nCount = pStaticObj->GetArrayLong("OccupantFlags");
		if (nCount)
		{
			if (nCount > nLen) nCount = nLen;
			if (nCount != pStaticObj->GetArrayByte("OccupantFlags", (BYTE*)buf, nCount))
			{
				return FALSE;
			}
			//解释脚本，如果脚本不解释，要在地图编辑器里面加入摆放多站位怪物功能。
			//这个代码是否保留看情况而定，不解释脚本可以直接注销
			for (int i = 4; i < nCount; i++)
			{
				switch (buf[i])
				{
				case CTile::masktype_block:				//行走格挡
					{
						
					}
					break;
				case CTile::masktype_fly:					//飞行格挡
					{
						
					}
					break;
				case CTile::masktype_flood:				//蔓延格挡
					{
						
					}
					break;
				case CTile::masktype_block_and_fly:		//行走飞行格挡
					{
						
					}
					break;
				case CTile::masktype_block_and_flood:		//行走蔓延格挡
					{
						
					}
					break;
				case CTile::masktype_fly_and_flood:		//飞行蔓延格挡
					{
						
					}
					break;
				case CTile::masktype_block_fly_flood:		//行走飞行蔓延格挡
					{
						
					}
					break;
				default:							//不格挡
					{
						
					}
				}
			}
			return nCount;
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------
//获取怪物和NPC各个部件对应的ID,创建怪物的时候调用
//参数:类型,怪物ID,返回缓冲
BOOL CResLoader::GetMonsterbuf(DWORD dwID, DWORD *pModule, BYTE *pAlpha, BYTE *pPal)
{
#ifdef _USE_BINSCRIPT
	HANDLE hMstObj = m_pScpCreature->GetObject(dwID);
	if (!hMstObj)
		return FALSE;
	int nCount = m_pScpCreature->GetSubObjectCount(hMstObj);
	for (int i = 0; i < nCount; i++)
	{
		//获取头、身体、鞋子、护手等
		HANDLE hSubObj = m_pScpCreature->GetSubObject(hMstObj, i);
		BYTE *nIndex = m_pScpCreature->GetBuffer(hSubObj);
		//获取下层buf，如神武头，神武衣服，神武鞋子等
		//储存顺序：ResID，Pal，Alpha。
		HANDLE hPart = m_pScpCreature->GetSubObject(hSubObj, 0);
		if(!hPart)
			continue;
		BYTE *nBuf = m_pScpCreature->GetBuffer(hPart);
		pModule[*nIndex] = *(DWORD*)nBuf;
		int nSize = m_pScpCreature->GetSize(hPart);
		if (nSize == sizeof(DWORD))
			continue;
		else if (nSize == (sizeof(DWORD) + sizeof(BYTE)))
		{
			nBuf += sizeof(DWORD);
			if (*nBuf > MAX_PAL_TYPES)
			{
				pAlpha[*nIndex] = *nBuf;
			}
			else
			{
				pPal[*nIndex] = *nBuf;
			}
		}
		else						//nSize = 6
		{
			nBuf += sizeof(DWORD);
			pAlpha[*nIndex] = *nBuf;
			nBuf += sizeof(BYTE);
			pPal[*nIndex] = *nBuf;
		}
	}
#else
	IResObject *pResObj = m_pScpCreature->GetResObject(dwID);
	if (!pResObj)
		return FALSE;
	int nCount = pResObj->GetSubObjectCount();
	for (int j = 0; j < nCount; j++)
	{
		IResObject *pSubObj = pResObj->GetSubObjectByIndex(j);
		if (!pSubObj)
			return FALSE;
		long lIndex = 0;
		if (!pSubObj->GetPropertyLong("PartIndex", lIndex))
		{
			PrintEventLn("ERROR: Init Filesystem, load PartIndex failed[]");
			return FALSE;
		}
		IResObject *pPartObj = pSubObj->GetSubObjectByIndex(0);
		if (!pPartObj)
			return FALSE;
		long ltemp = 0;
		if (!pPartObj->GetPropertyLong("ResID", ltemp))
		{
			PrintEventLn("ERROR: Init Filesystem, load ResID failed[]");
			return FALSE;
		}
		pModule[lIndex] = ltemp;
		if (!pPartObj->GetPropertyLong("Alpha", ltemp))
		{
		}
		pAlpha[lIndex] = (BYTE)ltemp;
		pPartObj->GetPropertyLong("Pal", ltemp);
		pPal[lIndex] = (BYTE)ltemp;
	}
	
#endif
	return TRUE;
}
//------------------------------------------------------------------------
//获取帧排序
//返回值：排序个数（该类型的生物个数），错误返回-1
//参数：返回缓冲，类型，状态，方向，帧
int CResLoader::GetFrameSort(BYTE *pBuf, SAniContext AniContext)
{
	//下面这样处理虽然效率低了些，但程序可读性强多了
	int nTypes = AniContext.nType;
	int nState = AniContext.nState;
	int nDir = AniContext.nDir;
	int nFrame = AniContext.nCurFrame;
	if (pBuf == NULL || nTypes > m_nTypes)
	{
		return -1;
	}
//	nTypes--;			//因为数组从0开始，这个类型开始设定是从1开始（以后改为从0开始要删除这句）
	//下面的计算公式可参考FrameSoreStruct.h的说明
	int nPos = nState*m_tcContext[nTypes]->m_nY*m_tcContext[nTypes]->m_nZ + nDir*m_tcContext[nTypes]->m_nZ + nFrame;
	BYTE nIndex = m_pTypeIndex[nTypes][nPos];
	//根据索引号查找实际列表
	int nOffset = (int)(nIndex * m_tcContext[nTypes]->m_nIndexWide);
	memcpy (pBuf, &m_pFrameSort[nTypes][nOffset], m_tcContext[nTypes]->m_nIndexWide);
	return m_tcContext[nTypes]->m_nIndexWide;
}
//------------------------------------------------------------------------
//根据帧延时等级获取对应的走路祯延时
//返回值：实际数据，-1为错
//参数：类型，等级
int CResLoader::GetWalkDelayByGrade(int nType, int nGrade)
{
	if (nGrade >= MAX_DELAY_TYPE || nGrade < 0)
	{
		return -1;
	}
	return m_WalkGrade[nType][nGrade];
}
//------------------------------------------------------------------------
//根据帧延时等级获取对应的跑步祯延时
//返回值：实际数据，-1为错
//参数：类型，等级
int CResLoader::GetRunDelayByGrade(int nType, int nGrade)
{
	if (nGrade >= MAX_DELAY_TYPE || nGrade < 0)
	{
		return -1;
	}
	return m_RunGrade[nType][nGrade];
}
//------------------------------------------------------------------------
//根据帧延时等级获取对应的骑马祯延时
//返回值：实际数据，-1为错
//参数：类型，等级
int CResLoader::GetRideDelayByGrade(int nType, int nGrade)
{
	if (nGrade >= MAX_DELAY_TYPE || nGrade < 0)
	{
		return -1;
	}
	return m_RideGrade[nType][nGrade];
}
//------------------------------------------------------------------------
//根据帧延时等级获取对应的攻击祯延时
//返回值：实际数据，-1为错
//参数：类型，等级
int CResLoader::GetAttackDelayByGrade(int nType, int nGrade)
{
	if (nGrade >= MAX_DELAY_TYPE || nGrade < 0)
	{
		return -1;
	}
	return m_AttackGrade[nType][nGrade];
}
//------------------------------------------------------------------------
//根据帧延时等级获取帧延时数据
//返回值：实际数据，-1为错
//参数：类型，等级
int CResLoader::GetDelayByGrade(int nType, int nGrade)
{
	if (nGrade >= MAX_DELAY_TYPE || nGrade < 0)
	{
		return -1;
	}
	return m_FrameDelay[nType][nGrade];
}
//------------------------------------------------------------------------
float CResLoader::GetStep(int nType, int nState)
{
#ifdef _USE_BINSCRIPT
	switch(nState)
	{
	case CrtStateWalk:
		{
			return m_fStep[nType][0];			
		}
	case CrtStateRun:
		{
			return m_fStep[nType][1];			
		}
	case CrtStateRideRun:
		{
			return m_fStep[nType][2];			
		}
	default:
		break;
	}

#else
	switch(nState)
	{
	case CrtStateWalk:
		{
			return m_fStep[nType][0];			
		}
	case CrtStateRun:
		{
			return m_fStep[nType][1];	
		}
	case CrtStateRideRun:
		{
			return m_fStep[nType][2];
		}
	default:
		break;		
	}
#endif
	return 0.0f;

}
//------------------------------------------------------------------------
BOOL CResLoader::GetStateEnable(int nType, int nState)
{
	BYTE nStateEnable = *(m_StateIndex[nType] + nState);

	return (nStateEnable == 1);
}
//------------------------------------------------------------------------
int CResLoader::GetAttackFrame(int nType, int nState)
{
	return *(m_AttackIndex[nType] + nState);
}
//------------------------------------------------------------------------
int CResLoader::GetAttackSoundFrame(int nType, int nState)
{
	return *(m_AttackSoundIndex[nType] + nState);
}
//------------------------------------------------------------------------
int CResLoader::GetFrameDelay(int nType, int nState)
{
	BYTE nGrade = *(m_DelayIndex[nType] + nState);

	return GetDelayByGrade(nType, (int)nGrade);
}
//------------------------------------------------------------------------
//加载部件状态标志
//返回值：布尔类型
//参数：部件ID，指向脚本状态标志的指针
BYTE* CResLoader::GetStateFlag(int nModuleType, DWORD dwID)
{
#ifdef _USE_BINSCRIPT
	HANDLE hCreatureObj = m_pScpBodyPart->GetObject(dwID);
	if (hCreatureObj == NULL) return NULL;
	
	BYTE* pBuf = m_pScpBodyPart->GetBuffer(hCreatureObj);
	return pBuf;
#else
	//下面是读取类型脚本，根据脚本制定原则，前面两项是不可能出错的，所以不做判断。

	IResObject *pObj = m_pScpBodyPart->GetResObject(dwID);
	if (pObj == NULL)
		return NULL;
	BYTE *p = new BYTE[CrtPartMax];		//申请的内存会在CCreature的Release()里面释放
	if (pObj->GetArrayByte("StateFlag", p) == 0)
	{
		PrintEventLn("ERROR: Init Filesystem, load StateFlag failed[]");
	}
	return p;
#endif
	return NULL;//没有使用二进制脚本，返回失败，有空再加上debug版本的读取
}
//------------------------------------------------------------------------
//获取调色板的HSL数值
//返回值：指向二进制脚本的指针
//参数：资源ID，第几个调色板
int* CResLoader::GetHSLpal(DWORD dwID, int nPalType)
{
#ifdef _USE_BINSCRIPT
	HANDLE hCreatureObj = m_pScpBodyPart->GetObject(dwID);
	if (hCreatureObj == NULL) return NULL;
	
	BYTE* pBuf = m_pScpBodyPart->GetBuffer(hCreatureObj);
	int nSize = m_pScpBodyPart->GetSize(hCreatureObj);
	int nLen = (nPalType-1)*sizeof(int)*3 + 23;//冰冻、中毒、狂暴+第几个 + StateFlag*1
	pBuf += nLen;
	return (int *)pBuf;
#else/*
	IResObject *pObj = m_pScpBodyPart->GetResObject(dwID);
	if (pObj == NULL)
		return NULL;
	char buf[16] = {0};
	sprintf(buf, "type%d", nPalType);
	long *p = new long[3];			//亮度、色度、饱和度
	if (pObj->GetArrayLong(buf, p) == 0)
	{
		PrintEventLn("ERROR: Init Filesystem, load BodyPart Pal failed[%s]", buf);
	}
	return p;						//这个堆内存会在函数外部释放，详细参看该函数调用*/
#endif
	return NULL;
}
//------------------------------------------------------------------------
//根据排序索引获取部件索引
//返回：部件索引，错误返回-1
//参数：类型，排序索引
int CResLoader::GetPartIndexBySortIndex(int nType, int nSort)
{
	for (int i = 0; i < CrtPartMax; i+=2)
	{
		int nCount = -1;
		if (m_bytePartFlag[nType][i] == 1)
			nCount++;
		if (nCount == nSort)
			return i;
	}
	return -1;
}
//------------------------------------------------------------------------
IResObject* CResLoader::SearchObj(IResObject* pObj, LPCSTR p)
{
	if (!pObj || !p)
		return NULL;
	int nCount = pObj->GetSubObjectCount();
	if (nCount > 0)
	{
		for (int i = 0; i < nCount; i++)
		{
			IResObject *pSub = pObj->GetSubObjectByIndex(i);
			LPCSTR pName = pSub->GetObjectName();
			if (strcmp (p, pName) == 0 && strlen(p) == strlen(pName))
			{
				return pSub;
			}
			else
			{
				IResObject * pResult = SearchObj(pSub, p);
				if (pResult != NULL)
				{
					return pResult;
				}
			}
		}
	}
	else
	{
		return NULL;
	}
	return NULL;
}
//------------------------------------------------------------------------
IResObject* CResLoader::SearchObj(IResObject *pObj, DWORD dwID)
{
	if (!pObj)
		return NULL;
	int nCount = pObj->GetSubObjectCount();
	if (nCount > 0)
	{
		for (int i = 0; i < nCount; i++)
		{
			IResObject *pSub = pObj->GetSubObjectByIndex(i);
			UINT dwDest = pSub->GetID();
			if (dwID == dwDest)
			{
				return pSub;
			}
			else
			{
				IResObject * pResult = SearchObj(pSub, dwID);
				if (pResult != NULL)
				{
					return pResult;
				}
			}
		}
	}
	else
	{
		return NULL;
	}
	return NULL;
}
//------------------------------------------------------------------------
int CResLoader::ReLoadScript(int nScpTypeID)
{
#ifndef _USE_BINSCRIPT
	char buf[MAX_PATH];
	
	// Load script file
	if (nScpTypeID == stBmp)
	{
		wsprintf(buf, "%s\\scp\\bmp.scp", gp->m_szWorkDir);
		if (m_pScpBmp) m_pScpBmp->Release();
		m_pScpBmp = g_pResMgr->LoadResScript(buf);
		if (!m_pScpBmp) return FALSE;
	}
	
	if (nScpTypeID == stAni)
	{
		wsprintf(buf, "%s\\scp\\ani.scp", gp->m_szWorkDir);
		if (m_pScpAni) m_pScpAni->Release();
		m_pScpAni = g_pResMgr->LoadResScript(buf);
		if (!m_pScpAni) return FALSE;
	}
	
	if (nScpTypeID == stMagic)
	{
		wsprintf(buf, "%s\\scp\\magic.scp", gp->m_szWorkDir);
		if (m_pScpMagic) m_pScpMagic->Release();
		m_pScpMagic = g_pResMgr->LoadResScript(buf);
		if (!m_pScpMagic) return FALSE;
	}
	
	if (nScpTypeID == stMonster)
	{
		wsprintf(buf, "%s\\scp\\Creature.scp", gp->m_szWorkDir);
		if (m_pScpCreature) m_pScpCreature->Release();
		m_pScpCreature = g_pResMgr->LoadResScript(buf);
		if (!m_pScpCreature) return FALSE;
	}
	
	if (nScpTypeID == stActor)
	{
		wsprintf(buf, "%s\\scp\\actor.scp", gp->m_szWorkDir);
		if (m_pScpBodyPart) m_pScpBodyPart->Release();
		m_pScpBodyPart = g_pResMgr->LoadResScript(buf);
		if (!m_pScpBodyPart) return FALSE;
	}
	
	if (nScpTypeID == stSkill)
	{
		wsprintf(buf, "%s\\scp\\skill_view.scp", gp->m_szWorkDir);
		if (m_pScpSkill) m_pScpSkill->Release();
		m_pScpSkill = g_pResMgr->LoadResScript(buf);
		if (!m_pScpSkill) return FALSE;
	}
#endif	
	return TRUE;
}
//------------------------------------------------------------------------
void CResLoader::AddSize(DWORD dwSize)
{
	m_ObjectPool.AddSize(dwSize);
}
//------------------------------------------------------------------------
IPoolNodeObject* CResLoader::RequestPic(DWORD dwResID)
{
	CPoolNode* pNode = NULL;
	if (m_ObjectPool.Find(ioStatic + dwResID, &pNode))
		// return pNode->GetPoolNodeObject();
		return pNode->m_pObj;

	CPic* pPoolNodeObj = new CPic;
	char szName[MAX_PATH];
	
	wsprintf(szName, "%d.sfp", dwResID);
	if (!pPoolNodeObj->Open(m_pfsBmp, szName, ioStatic + dwResID))
	{
		wsprintf(szName, "%d.sfp", 100000);
		pPoolNodeObj->Open(m_pfsBmp, szName, ioStatic + dwResID);
		TraceColorLn(MY_TRACE_COLOR, "静态图片资源 [%d] 不存在", dwResID);
	}

	CPoolNode node(static_cast<IPoolNodeObject*>(pPoolNodeObj), ioStatic + dwResID);
	if (m_ObjectPool.AddObjectRef(node))
		return node.GetPoolNodeObject();

	delete pPoolNodeObj;

	return NULL;
}
//------------------------------------------------------------------------
IPoolNodeObject* CResLoader::RequestAni(DWORD dwResID)
{
	CPoolNode* pNode;
	if (m_ObjectPool.Find(ioAni + dwResID, &pNode))
		// return pNode->GetPoolNodeObject();
		return pNode->m_pObj;
	
	CAni* pPoolNodeObj = new CAni;
	char szName[MAX_PATH];
	wsprintf(szName, "%d.mfp", dwResID);
	if (!pPoolNodeObj->Open(m_pfsAni, szName, ioAni + dwResID))
	{
		TraceColorLn(MY_TRACE_COLOR, "ani image(%d) not exist!", dwResID);
		delete pPoolNodeObj;
		return NULL;
	}
	
	CPoolNode node(static_cast<IPoolNodeObject*>(pPoolNodeObj), ioAni + dwResID);
	if (m_ObjectPool.AddObjectRef(node))
		return node.GetPoolNodeObject();

	delete pPoolNodeObj;
	return NULL;
}
//------------------------------------------------------------------------
IPoolNodeObject* CResLoader::RequestMagic(DWORD dwResID)
{
	CPoolNode* pNode;
	if (m_ObjectPool.Find(ioMagic + dwResID, &pNode))
		// return pNode->GetPoolNodeObject();
		return pNode->m_pObj;
	
	CAni* pPoolNodeObj = new CAni;
	char szName[MAX_PATH];
	wsprintf(szName, "%d.mfp", dwResID);
	if (!pPoolNodeObj->Open(m_pfsMagic, szName, ioAni + dwResID))
	{
		TraceColorLn(MY_TRACE_COLOR, "magic image(%d) not exist!", dwResID);		
		delete pPoolNodeObj;
		return NULL;
	}
	
	CPoolNode node(static_cast<IPoolNodeObject*>(pPoolNodeObj), ioMagic + dwResID);
	if (m_ObjectPool.AddObjectRef(node))
		return node.GetPoolNodeObject();

	delete pPoolNodeObj;
	return NULL;
}
//------------------------------------------------------------------------
IPoolNodeObject* CResLoader::RequestCreature(DWORD dwResID)
{
	CPoolNode* pNode;
	if (m_ObjectPool.Find(ioCreature + dwResID, &pNode))
		// return pNode->GetPoolNodeObject();
		return pNode->m_pObj;
	
	CAniGroup* pPoolNodeObj = new CAniGroup;
	char szName[MAX_PATH];
	wsprintf(szName, "%d.msp", dwResID);
	if (!pPoolNodeObj->Open(m_pfsCreature, szName, ioCreature + dwResID))
	{
		TraceColorLn(MY_TRACE_COLOR, "actor image(%d) not exist!", dwResID);	
		delete pPoolNodeObj;
		return NULL;
	}
	
	CPoolNode node(static_cast<IPoolNodeObject*>(pPoolNodeObj), ioCreature + dwResID);
	if (m_ObjectPool.AddObjectRef(node))
		return node.GetPoolNodeObject();

	delete pPoolNodeObj;
	return NULL;
}
//------------------------------------------------------------------------
BOOL CResLoader::ReleaseRes(DWORD dwResID)
{
	if (dwResID == 0) return TRUE;
	if (m_ObjectPool.RemoveObjectRef(dwResID))
		return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------
// 用于陷阱对象的资源ID的读取
BOOL CResLoader::GetResID(int nIndex, DWORD& dwResID)
{
/*//不知道是否还有陷阱，保留
	IResObject* pSkillObj = (IResObject*)CSkillCreate::getSkillScpObj(0, nIndex);
	if (!pSkillObj || !pSkillObj->GetPropertyLong("TrapID", (long&)dwResID))
		return FALSE;
*/
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResLoader::GetActorUserPal(DWORD dwResID, /*in out*/int& nCount, /*in out*/BYTE** ppPal)
{
#ifdef _USE_BINSCRIPT
	HANDLE hCreatureObj = m_pScpBodyPart->GetObject(dwResID);
	if (hCreatureObj == NULL) return FALSE;
	BYTE* pBuf = m_pScpBodyPart->GetBuffer(hCreatureObj);
	
	int nTrueCount = 0;
	for (int i=0; i<nCount; i++)
	{
		int nLen = m_pScpBodyPart->GetUnFixPropertyLength(hCreatureObj, i);
		if (nLen != 0)
		{
			nTrueCount++;
			int nOffset = m_pScpBodyPart->GetUnFixPropertyOffset(hCreatureObj, i);
			ppPal[i] = pBuf + nOffset;
		}
	}
	nCount = nTrueCount;
#endif
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResLoader::GetMonsterCreateParam(DWORD dwMstID, SMonsterViewCreateParam& mvCreateParam)
{
#ifndef _USE_BINSCRIPT
	// 得到相应Monster的资源对象
	IResObject* pMonsterObj = m_pScpCreature->GetResObject(dwMstID);
	if (!pMonsterObj) 
	{
		PrintEventLn("ERROR: Can't find MstID[%d]", dwMstID);
		return FALSE;	
	}
	//获取部件等级标记
	if (pMonsterObj->GetArrayByte("PartsFlag", mvCreateParam.nStateFlag, CrtPartMax) == 0)
	{
		PrintEventLn("ERROR: Init Filesystem, load PartsFlag failed[]");
	}
	//怪物类型
	if (!pMonsterObj->GetPropertyLong("nType", (long&)mvCreateParam.nType))
	{
		PrintEventLn("ERROR: Init Filesystem, load nType failed[]");
	}
	pMonsterObj = pMonsterObj->GetParentObject();
	// 体型大小
	if(!pMonsterObj->GetPropertyLong("nShape", (long&)mvCreateParam.nShape, 1))
	{
		PrintEventLn("ERROR: Init Filesystem, load nShape failed[]");
	}
	// Monster的移动步长等级
	if (!pMonsterObj->GetPropertyLong("nWalkGrade", (long&)mvCreateParam.nWalkGrade, 1))
	{
		PrintEventLn("ERROR: Init Filesystem, load nWalkGrade failed[]");
	}
	if (!pMonsterObj->GetPropertyLong("nRunGrade", (long&)mvCreateParam.nRunGrade, 1))
	{
		PrintEventLn("ERROR: Init Filesystem, load nRunGrade failed[]");
	}
	//攻击等级
	if (!pMonsterObj->GetPropertyLong("nAttackGrade", (long&)mvCreateParam.nAttackGrade, 1))
	{
		PrintEventLn("ERROR: Init Filesystem, load nAttackGrade failed[]");
	}

	// 位置偏移
	if (!pMonsterObj->GetPropertyLong("HalfHeightOffset", (long&)mvCreateParam.nHalfHeightOffset, -40))
	{
		PrintEventLn("ERROR: Init Filesystem, load HalfGeightOffset failed[]");
	}
	if (!pMonsterObj->GetPropertyLong("HeightOffset", (long&)mvCreateParam.nHeightOffset, -80))
	{
		PrintEventLn("ERROR: Init Filesystem, load HeightOffset failed[]");
	}

#else
//#pragma pack(push)
//#pragma pack(1)
//	struct MstObj
//	{
//		BYTE PartsFlag[CrtPartMax];
//		int  nType;
//	};
//#pragma pack(pop)
//	MstObj  *pMstObj = NULL;

	HANDLE hMonsterObj = m_pScpCreature->GetObject(dwMstID);
	if (hMonsterObj == NULL) return FALSE;
	int nSize = m_pScpCreature->GetSize(hMonsterObj);
	BYTE* pBuf = m_pScpCreature->GetBuffer(hMonsterObj);
// 	pMstObj = (MstObj *)pBuf;

	int nLen = sizeof(mvCreateParam.nStateFlag);
	memcpy (&mvCreateParam.nStateFlag, pBuf, nLen);
//	pBuf = pBuf + nSize - sizeof(int) * 7;
	pBuf = pBuf + nSize - sizeof(int) * 10;

	mvCreateParam.nType = *(int *)pBuf;
	pBuf += sizeof(int);

	// 生物默认普通攻击动作
	nLen = sizeof(mvCreateParam.nActStateCommonAttack);
	memcpy (&mvCreateParam.nActStateCommonAttack, pBuf, nLen);
	pBuf += nLen;

	// 生物默认蓄气动作
	mvCreateParam.nActStateJuqi = *(int *)pBuf;
	pBuf += sizeof(int);

	// 生物默认释放技能动作
	mvCreateParam.nActStateSkillAttack = *(int *)pBuf;
	pBuf += sizeof(int);

	// 生物普通攻击音效
	mvCreateParam.nSoundIDCommonAttack = *(int *)pBuf;
	pBuf += sizeof(int);

	// 生物休闲音效
	mvCreateParam.nSoundIDFallow = *(int *)pBuf;
	pBuf += sizeof(int);

	// 生物被击音效
	mvCreateParam.nSoundIDBeHit = *(int *)pBuf;
	pBuf += sizeof(int);

	// 生物死亡音效
	mvCreateParam.nSoundIDDie = *(int *)pBuf;
#endif
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResLoader::GetMonsterUserPal(DWORD dwResID, /*in out*/int& nCount, /*in out*/BYTE** ppPal)
{
#ifdef _USE_BINSCRIPT
	HANDLE hMonsterObj = m_pScpCreature->GetObject(dwResID);
	if (hMonsterObj == NULL) return FALSE;
	BYTE* pBuf = m_pScpCreature->GetBuffer(hMonsterObj);

	int nTrueCount = 0;
	for (int i=0; i<nCount; i++)
	{
		int nLen = m_pScpCreature->GetUnFixPropertyLength(hMonsterObj, i);
		if (nLen != 0)
		{
			nTrueCount++;
			int nOffset = m_pScpCreature->GetUnFixPropertyOffset(hMonsterObj, i);
			ppPal[i] = pBuf + nOffset;
		}
	}
	nCount = nTrueCount;
#endif
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResLoader::GetMagicCreateParam(DWORD dwResID, SMagicCreateParam& mCreateParam)
{
	IResObject* pMagicObj = m_pScpMagic->GetResObject(dwResID);
	if (!pMagicObj) return FALSE;
	
	POINT pt;long nVal;
	
	// 锚点
	pMagicObj->GetArrayLong("Anchor", (long*)&pt, 2);
	mCreateParam.ptAnchor.x = pt.x;
	mCreateParam.ptAnchor.y = pt.y;
	
	// 动画延时
	pMagicObj->GetPropertyLong("Delay", (long&)nVal, 30);
	mCreateParam.nDelay = nVal;

	pMagicObj->GetPropertyLong("LoopStartIndex", (long&)mCreateParam.nLoopStartIndex);
	pMagicObj->GetPropertyLong("LoopEndIndex", (long&)mCreateParam.nLoopEndIndex);
	
	pMagicObj->GetPropertyLong("Life", (long&)mCreateParam.nLife);
	pMagicObj->GetArrayLong("Offset", (long*)&mCreateParam.ptOffset, 2);
	
	return TRUE;
}
//------------------------------------------------------------------------
/*
BOOL CResLoader::GetTrapViewCreateParam(DWORD dwResID, TrapViewCreateParam& tCreateParam)
{
	IResObject* pMagicObj = m_pScpMagic->GetResObject(dwResID);
	if (!pMagicObj) return FALSE;
	
	POINT pt;long nVal;
	
	// 锚点
	pMagicObj->GetArrayLong("Anchor", (long*)&pt, 2);
	tCreateParam.ptAnchor.x = pt.x;
	tCreateParam.ptAnchor.y = pt.y;
	
	// 动画延时
	pMagicObj->GetPropertyLong("Delay", (long&)nVal, 30);
	tCreateParam.nDelay = nVal;

	pMagicObj->GetPropertyLong("LoopStartIndex", (long&)tCreateParam.nLoopStartIndex);
	pMagicObj->GetPropertyLong("LoopEndIndex", (long&)tCreateParam.nLoopEndIndex);

	return TRUE;
}
*/
//------------------------------------------------------------------------
BOOL CResLoader::GetAniObjCreateParam(DWORD dwResID, SAniObjCreateParam& aoCreateParam)
{
#ifndef _USE_BINSCRIPT
	IResObject* pAniObj = m_pScpAni->GetResObject(dwResID);
	if (!pAniObj) return FALSE;
	
	// 锚点
	pAniObj->GetArrayLong("Anchor", (long*)&aoCreateParam.ptAnchor, 2);
	
	// 动画延时
	pAniObj->GetPropertyLong("Delay", (long&)aoCreateParam.nDelay, 30);
	pAniObj->GetPropertyLong("Delay1", (long&)aoCreateParam.nDelay1, 0);

	// 排序偏移点
	pAniObj->GetArrayLong("SortTileOffset1", (long*)&aoCreateParam.ptSortOff1, 2);
	pAniObj->GetArrayLong("SortTileOffset2", (long*)&aoCreateParam.ptSortOff2, 2);
#else
	HANDLE hAniObj = m_pScpAni->GetObject(dwResID);
	if (hAniObj == NULL) return FALSE;
	int nSize = m_pScpAni->GetSize(hAniObj);
	BYTE* pBuf = m_pScpAni->GetBuffer(hAniObj);
	if (nSize < sizeof(SAniObjCreateParam))
		return FALSE;
	memcpy(&aoCreateParam, pBuf, sizeof(SAniObjCreateParam));
#endif	
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResLoader::GetAniObjOccupantInfo(DWORD dwResID, unsigned char* buf, int nLen)
{
#ifndef _USE_BINSCRIPT
	IResObject* pAniObj = m_pScpAni->GetResObject(dwResID);
	if (!pAniObj) return FALSE;
	
	if (pAniObj->GetPropertyString("OccupantFlags")) // 有占位信息
	{
		// for occupant
		int nCount = pAniObj->GetArrayLong("OccupantFlags");
		if (nCount)
		{
			if (nCount > nLen) nCount = nLen;
			if (nCount != pAniObj->GetArrayByte("OccupantFlags", (BYTE*)buf, nCount))
			{
				TraceColorLn(MY_TRACE_COLOR, "CResLoader::GetAniObjOccupantInfo, read script failed!");
				return FALSE;
			}
			return nCount;
		}
	}
#else
	HANDLE hAniObj = m_pScpAni->GetObject(dwResID);
	if (hAniObj == NULL) return FALSE;
	int nCount = m_pScpAni->GetUnFixPropertyLength(hAniObj, 0);
	if (nCount > nLen)
		nCount = nLen;
	if (nCount != 0)
	{
		int nOffset = m_pScpAni->GetUnFixPropertyOffset(hAniObj, 0);
		memcpy(buf, m_pScpAni->GetBuffer(hAniObj) + nOffset, nCount);
		return nCount;
	}
#endif
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CResLoader::GetStaticObjCreateParam(DWORD dwResID, SStaticObjCreateParam& soCreateParam)
{
#ifndef _USE_BINSCRIPT
	IResObject* pStaticObj = m_pScpBmp->GetResObject(dwResID);
	if (!pStaticObj) return FALSE;
	
	// 锚点
	pStaticObj->GetArrayLong("Anchor", (long*)&soCreateParam.ptAnchor, 2);
	pStaticObj->GetArrayLong("SortTileOffset1", (long*)&soCreateParam.ptSortOff1, 2);
	pStaticObj->GetArrayLong("SortTileOffset2", (long*)&soCreateParam.ptSortOff2, 2);
#else
	HANDLE hStaticObj = m_pScpBmp->GetObject(dwResID);
	if (hStaticObj == NULL) return FALSE;
	int nSize = m_pScpBmp->GetSize(hStaticObj);
	BYTE* pBuf = m_pScpBmp->GetBuffer(hStaticObj);
	if (nSize < sizeof(SStaticObjCreateParam))
		return FALSE;
	memcpy(&soCreateParam, pBuf, sizeof(SStaticObjCreateParam));
#endif
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResLoader::GetStaticObjOccupantInfo(DWORD dwResID, unsigned char* buf, int nLen)
{
#ifndef _USE_BINSCRIPT
	IResObject* pStaticObj = m_pScpBmp->GetResObject(dwResID);
	if (!pStaticObj) return FALSE;
	
	if (pStaticObj->GetPropertyString("OccupantFlags")) // 有占位信息
	{
		// for occupant
		int nCount = pStaticObj->GetArrayLong("OccupantFlags");
		if (nCount)
		{
			if (nCount > nLen) nCount = nLen;
			if (nCount != pStaticObj->GetArrayByte("OccupantFlags", (BYTE*)buf, nCount))
			{
				TraceColorLn(MY_TRACE_COLOR, "CResLoader::GetStaticObjOccupantInfo, read script failed!");
				return FALSE;
			}
			return nCount;
		}
	}
#else
	HANDLE hStaticObj = m_pScpBmp->GetObject(dwResID);
	if (hStaticObj == NULL) return FALSE;
	int nCount = m_pScpBmp->GetUnFixPropertyLength(hStaticObj, 0);
	if (nCount > nLen)
		nCount = nLen;
	if (nCount != 0)
	{
		int nOffset = m_pScpBmp->GetUnFixPropertyOffset(hStaticObj, 0);
		memcpy(buf, m_pScpBmp->GetBuffer(hStaticObj) + nOffset, nCount);
		return nCount;
	}
#endif
	return FALSE;
}
//------------------------------------------------------------------------
int CResLoader::GetStaticObjOccupantOption(DWORD dwResID)
{
#ifndef _USE_BINSCRIPT
	IResObject* pStaticObj = m_pScpBmp->GetResObject(dwResID);
	if (!pStaticObj) return 0;
	
	int nOccupantOption;
	if (pStaticObj->GetPropertyLong("OccupantOption", (long&)nOccupantOption)) // 有占位信息
		return nOccupantOption;
#endif
	return 0;
}
//------------------------------------------------------------------------