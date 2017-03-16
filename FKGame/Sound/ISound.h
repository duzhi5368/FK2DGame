/**
*	created:		2012-6-30   5:17
*	filename: 		ISound
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		音乐模块接口
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
struct IResObject;
struct IClientGlobal;
//------------------------------------------------------------------------
#define  INVALID_CHANNELID -1
//------------------------------------------------------------------------
struct ISound
{
	virtual bool Create(int Max_Channel) = 0;

	//////////////////////////////////////////////////////////////////
	//输入参数: 播放音乐文件的ID , 是否循环, 播放的位置, (这里假设人物是站在0,0 (屏幕中心)位置)
	//输出参数：频道ＩＤ（用于停止效）
	//////////////////////////////////////////////////////////////////
	virtual int PlaySound(int id, BOOL bLoop = FALSE, int offsetX = 0, int offsetY = 0) = 0;

	//停止音效
	virtual void StopSound(int nChunnelID) = 0;

	//播放声音
	virtual void PlayMusic(int id, BOOL bLoop = TRUE) = 0;

	//停止声音
	virtual void StopMusic() = 0;

	virtual void Release() = 0;

	virtual void SetSoundTable(IResObject *pResObj) = 0;

	//设置音乐音量 (0 -- 100)
	virtual void SetMusicVolume(int nDegree) = 0;

	//获取声音音量
	virtual int  GetMusicVolume(void) = 0;

	//设置音效音量 (0 -- 100)
	virtual void SetSoundVolume(int nDegree) = 0;
	
	//获取声效音量
	virtual int  GetSoundVolume(void) = 0;

	//设置音乐静音状态
	virtual void SetMusicMute(BOOL bState) = 0;

	//获取音乐静音状态
	virtual BOOL GetMusicMute(void) = 0;
	
	//设置音效静音状态
	virtual void SetSoundMute(BOOL bState) = 0;

	//获取音效静音状态
	virtual BOOL GetSoundMute(void) = 0;

	//设置音乐循环播放
	virtual void SetMusicLoop(BOOL bState) = 0;
	
	//获取音乐循环播放
	virtual BOOL GetMusicLoop(void) = 0; 

	//更新人物位置(用于触发环境音效)
	virtual void UpdateActorLoc(const POINT &pt) {};
	virtual void UpdateActorLoc(const POINTS &pts) {};

	//清除所有正在发音的频道
	virtual void ClearAllSound(){};

	//从脚本中获取所有地图的环境音效列表
	virtual BOOL LoadEnvironmentSoundScript(IResObject *pResScript){return TRUE;};
	
	//切换环境音效配置
	virtual void SwitchEnvironmentSoundConfig(int nKey) {};

	//播放环境音效
	virtual int PlayEnvirSound(int id, BOOL bLoop = FALSE, int offsetX = 0, int offsetY = 0) {return INVALID_CHANNELID;};

	//复位当前环境音效配置(例如将所有音效的下次播放时间清0等工作), 在切换地图时调
	virtual void ResetCurrentEnvironmentSoundConfig(void){};

	//测试所有声音文件, 看看播放时是否会非法
	virtual void TestAllSoundFile(void){};

	//播放视频
	virtual BOOL PlayVideo(const char * szFileName){return FALSE;};

	//停止视频
	virtual void StopVideo(void){};

	//停止某一ID的声音(如果有的话)
	virtual void StopSoundBySoundID(int nSoundID){};
};
//------------------------------------------------------------------------
class CSoundHelper
{
private:
	typedef bool (*CreateSoundProc)(IClientGlobal *pGlobal, ISound **sound, int Max_Channel);
	HMODULE m_hdll;

public:
	CSoundHelper()
	{
		m_pSound = NULL;
		m_hdll = NULL;
	}

	~CSoundHelper()
	{
		Close();
	}

	BOOL Create(int Max_Channel ,IClientGlobal *pGlobal)
	{
		
		Close();//先释放旧的
		
		try
		{
			
#ifdef _DEBUG
			m_hdll = ::LoadLibrary("FKSound_D.dll");
			if (m_hdll == NULL)
				throw "无法加载 FKSound_D.dll";
#else				
			m_hdll = ::LoadLibrary("FKSound.dll");
			if (m_hdll == NULL)
				throw "无法加载 FKSound.dll";
#endif
			

			CreateSoundProc CreateSound = NULL;
			CreateSound = (CreateSoundProc)GetProcAddress(m_hdll,"CreateSound");
			if (CreateSound == NULL)
				throw "Can not GetProcAddress('CreateSound')";
			
			if (!CreateSound(pGlobal, &m_pSound, Max_Channel))
			{
				
				throw "call fail in function : CreateSound()";
			}
				
			
			return m_pSound != NULL;
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			OutputDebugString("\r\n");
			
			return FALSE;
		}
		catch(...)
		{
			OutputDebugString("CreateSound::Create() : catch error\r\n");
			
			return FALSE;
		}

		return TRUE;
	}

	void Close()
	{
		if (m_pSound)
		{
			m_pSound->Release();
			m_pSound = NULL;
		}

		if (m_hdll)
		{
			::FreeLibrary(m_hdll);
			m_hdll = NULL;
		}
	}



public:
	ISound *m_pSound;
};
//------------------------------------------------------------------------