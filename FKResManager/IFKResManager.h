/**
*	created:		2012-6-18   18:55
*	filename: 		IFKResManager
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
// 资源管理器版本
#define VERSION_QUERY_RESOURCE_MANAGER		1
// 单行资源脚本最大长度
#define	MAX_SCRIPT_LINE_LEN					2048
//------------------------------------------------------------------------
// 资源对象接口类
struct IResScript;
struct IFileObject;
struct IResObject
{
public:
	// 设置对象的名字和类型模板
	virtual void SetObjectInfo(LPCSTR szObjectName = NULL, LPCSTR szTypeName = NULL) = 0;
	// 获取对象名
	virtual LPCSTR GetObjectName() = 0;
	// 获取对象类型模板名
	virtual LPCSTR GetTypeName() = 0;
	// 获取对象ID(系统自动产生的Root对象的ID为0,其他对象均大于0)
	virtual UINT GetID() = 0;
	// 清空(具体表现在仅仅删除本对象的属性，不进行其他操作)
	virtual void Empty() = 0;
	// 释放此对象(及其所有子对象)，一般不要调用此方法，目前此方法主要应用在文件上传服务器中
	virtual BOOL Release() = 0;
	// 获取对象的属性数目
	virtual int  GetPropertyCount() = 0;
	// 按索引号获取对象的属性名
	virtual LPCSTR GetPropertyName(int nIndex) = 0;
	// 按索引号获取对象的属性值
	virtual LPCSTR GetPropertyValue(int nIndex) = 0;

	// 获取资源脚本中指定ID的对象
	virtual IResObject* GetResObject(UINT uID) = 0;

	// 获取父对象ID
	virtual UINT GetParentID() = 0;
	// 获取父对象(包括Root对象)
	virtual IResObject* GetParentObject() = 0;

	// 创建新的子对象
	virtual IResObject* CreateSubObject(int nIndex = -1) = 0;
	// 获取子对象数目
	virtual int GetSubObjectCount() = 0;
	// 通过对象名获取子对象(大小写不敏感，默认得到最后一个子对象)
	// 成功,返回对象指针,否则,返回NULL
	virtual IResObject* GetSubObjectByName(LPCSTR szObjectName = NULL) = 0;
	// 通过索引号获取子对象
	// 成功,返回对象指针,否则,返回NULL
	virtual IResObject* GetSubObjectByIndex(int nIndex) = 0;
	// 将某个对象作为子对象(但加入的对象及其子对象不能通过哈希表访问，
	// 需在SaveScript()后重新加载才能访问)，主要用于两个脚本的合并
	virtual BOOL AddSubObject(IResObject* pResObject) = 0;
	// 删除子对象(默认删除尾对象)
	virtual BOOL EraseSubObjectByName(LPCSTR szObjectName = NULL) = 0;
	// 删除指定索引的对象
	virtual BOOL EraseSubObjectByIndex(int nIndex) = 0;

	// 添加属性
	// 成功返回TRUE,若属性名存在,则失败,返回FALSE
	virtual BOOL AddProperty(LPCSTR szPropertyName, LPCSTR szPropertyValue = NULL) = 0;
	// 删除指定的属性
	// 成功返回TRUE,如不存在此属性,则失败,返回FALSE
	virtual BOOL EraseProperty(LPCSTR szPropertyName) = 0;
	// 修改属性名称
	// 成功返回TRUE,如不存在此属性,则失败,返回FALSE
	virtual BOOL SetPropertyName(LPCSTR szOldPropertyName, LPCSTR szNewPropertyName) = 0;

	// 获取属性值
	virtual LPCSTR GetPropertyString(LPCSTR szPropertyName) = 0;
	virtual BOOL GetPropertyLong  (LPCSTR szPropertyName, long& nReturn, long nDefault = 0L) = 0;
	virtual BOOL GetPropertyFloat (LPCSTR szPropertyName, double& dlReturn,	double dlDefault = 0.0) = 0;
	
	// 设置属性值
	// 如果指定的属性名不存在则自动创建一个属性
	virtual BOOL SetPropertyString(LPCSTR szPropertyName, LPCSTR szSetString) = 0;
	virtual BOOL SetPropertyLong  (LPCSTR szPropertyName, long nSet) = 0;
	virtual BOOL SetPropertyFloat (LPCSTR szPropertyName, double dlSet) = 0;

	// 获取数组或数组长度(函数返回数组长度或获取的数组的实际长度)
	// 注1、如果lpData=NULL,返回数组长度
	// 注2、如果nCount指定某个值,则返回数组前nCount个值,默认(nCount=0)返回所有值,
	//		如果nCount超越数组的实际长度,则返回实际长度)
	virtual int GetArrayLong (LPCSTR szArrayName, long*   lpData = NULL, int nCount = 0) = 0;
	virtual int GetArrayFloat(LPCSTR szArrayName, double* lpData = NULL, int nCount = 0) = 0;
	virtual int GetArrayByte (LPCSTR szArrayName, BYTE*   lpData = NULL, int nCount = 0) = 0;
	
	// 设置数组(当数组名存在时则修改,否则新建数组)
	// 输入: 数组名,数组缓存和数组长度
	virtual BOOL SetArrayLong (LPCSTR szArrayName, long*   lpData, int nCount) = 0;
	virtual BOOL SetArrayFloat(LPCSTR szArrayName, double* lpData, int nCount) = 0;
	virtual BOOL SetArrayByte (LPCSTR szArrayName, BYTE*   lpData, int nCount) = 0;

	// 复制本对象，不包括子对象列表，默认采用系统分配的ID号，不记入哈希表
	virtual IResObject* Duplicate(BOOL bUseNewID = TRUE) = 0;
	// 复制本对象及其所有子对象，默认采用系统分配的ID号，不记入哈希表
	// virtual IResObject* DuplicateAll(BOOL bUseNewID = TRUE) = 0;
	virtual IResScript* GetResScript() = 0;

#ifdef _DEBUG
	virtual BOOL Dump() = 0;
#endif
};
//------------------------------------------------------------------------
// 资源脚本接口
// 注意：		每个.res资源文件对应一个资源脚本接口
//				当IResScript不再使用时,一定要Release()
struct IResScript
{
	// 加载类型列表
	virtual BOOL LoadTypeList(LPCSTR szTypeListFileName) = 0;

	// 保存资源脚本
	virtual BOOL SaveScript(LPCSTR szResScriptFileName = NULL, BYTE* pKey = NULL) = 0;

	// 编译成二进制文件
	virtual BOOL Compile(LPCSTR szFileName = NULL) = 0;

	// 访问指定ID的资源对象(通过哈希表访问.res文件中所包含的所有对象)
	virtual IResObject* GetResObjectRoot() = 0; // ID = 0
	virtual IResObject* GetResObject(UINT uID) = 0;

	// 获得本脚本中所有对象总数
	virtual int GetObjectCount() = 0;

	// 获得本脚本的文件名
	virtual LPCSTR GetScriptFileName() = 0;
	// 获得MaxID
	virtual UINT GetMaxID() = 0;

	// 目前仅用于文件服务器的下载等操作
	// 解析并获得.res文件中指定ID的对象的字符串缓冲(常用于服务器端)
	virtual BOOL ParseObjectServer(UINT uID, char* pData) = 0;
	// 将一个对象的脚本字符串解析成一个内存对象(常用于客户端)
	virtual IResObject* ParseObjectClient(char* pData, int nSize) = 0;

	virtual BOOL AddToHashMap(UINT uID, IResObject* pResObject) = 0;

	// 释放资源脚本(包括所有对象和列表的释放)
	virtual BOOL Release() = 0;

#ifdef _DEBUG
	virtual BOOL Dump() = 0;
#endif
};
//------------------------------------------------------------------------
// 二进制资源脚本接口
// 注意：		每个二进制资源文件对应一个二进制资源脚本接口
//				当IBinScript不再使用时,一定要Release()
struct IBinScript
{
	// 访问指定ID的二进制资源对象
	// 失败 返回NULL
	virtual HANDLE GetObject(UINT uID) = 0;

	// 获得本脚本中所有对象总数
	virtual int GetObjectCount() = 0;

	// 释放
	virtual BOOL Release() = 0;
  
    // 获取对象的二进制数据缓存
	// 成功 返回缓存指针；失败 返回NULL
	virtual BYTE* GetBuffer(HANDLE hBinObject) = 0;

	// 获得整个对象的缓存大小
	// 失败 返回-1
	virtual int GetSize(HANDLE hBinObject) = 0;

	// 获得不固定长度属性的数目
	// 失败 返回-1
	virtual int GetUnFixPropertyCount(HANDLE hBinObject) = 0;
	
	// 获得不固定长度属性的偏移量
	// 失败 返回-1
	// 注: nIndexEx指不固定属性的索引号(0,1,...,n),它忽略了有固定
	// 长度的那些属性，第一个不固定属性的索引为0，第二个1...类推
	virtual int GetUnFixPropertyOffset(HANDLE hBinObject, int nIndexEx) = 0;

	// 获得不固定长度属性的实际长度
	// 失败返回-1
	// 注: nIndexEx指不固定属性的索引号(0,1,...,n),它忽略了有固定
	// 长度的那些属性，第一个不固定属性的索引为0，第二个1...类推
	virtual int GetUnFixPropertyLength(HANDLE hBinObject, int nIndexEx) = 0;

	// 获取子对象数目
	// 失败返回-1
	virtual int GetSubObjectCount(HANDLE hBinObject) = 0;

	// 按索引获得子对象
	// 成功 返回子对象句柄；失败 返回-1
	virtual HANDLE GetSubObject(HANDLE hBinObject, int nIndex) = 0;

	// 删除指定句柄的对象(不管其下的子对象)
	virtual BOOL Delete(UINT uID) = 0;
};
//------------------------------------------------------------------------
// 资源加载器
// 要加载的资源类型
#define RES_BMP				0x00000001L
#define RES_TGA				0x00000002L
#define RES_JPG				0x00000004L
#define RES_RLE				0x00000008L
#define RES_SOUND			0x00000010L
#define RES_MUSIC			0x00000020L
#define RES_DATA			0x00000040L

struct IResLoader
{
	virtual HANDLE	LoadRes(LPCSTR szFileName, UINT uType) = 0;
	virtual HBITMAP LoadBMP(LPCSTR szResName) = 0;
	virtual HBITMAP LoadTGA(LPCSTR szResName) = 0;
	virtual HBITMAP LoadJPG(LPCSTR szResName) = 0;
};
//------------------------------------------------------------------------
// 资源管理器接口
struct IResMgr
{
	// 新建空的资源脚本(返回最上层对象列表指针,nStartID表明本资源脚本ID的起始值)
	virtual IResScript* CreateResScript(int nStartID = 0) = 0;

	// 创建并加载资源文件
	virtual IResScript* LoadResScript(LPCSTR szResFileName, BOOL bIgnoreID = FALSE, BYTE* pKey = NULL) = 0;
	virtual IResScript* LoadResScript(IFileObject* pFileObject, BOOL bIgnoreID = FALSE, BYTE* pKey = NULL) = 0;

	// 加载二进制脚本
	virtual IBinScript* LoadBinScript(LPCSTR szBinFileName) = 0;
	virtual IBinScript* LoadBinScript(IFileObject* pFileObject) = 0;

	// 通过文件列表合并资源文件
	virtual BOOL MergeResourceScriptByFileList(LPCSTR szListFileName, LPCSTR szMergeFileName) = 0;

	// 释放资源管理器(只释放自己,不释放创建的其他如IResScript*指针等)
	virtual BOOL Release() = 0;
};

struct ITrace;
//------------------------------------------------------------------------
/*
	资源管理器帮助类

	调用方式：
	1：在你的项目内创建 CIResMgrHelper 全局对象。
	2：调用 Create() 获取 IResMgr 对象。
	3：调用 IResMgr 的 GetResScript() 获取某资源文件的 IResScript。
	4：IResScript 可以 Create()或LoadScript()并获得顶层列表IResObjectList,也可以SaveScript()。
	5：IResObjectList可以AddResObject()、EraseResObject()等获得对应的IResObject。
	6：对 IResObject 进行对象属性的增删改，或者增删下一级的 IResObject 列表
	7：IResScript->Release()释放所有的对象和列表
	8：CIResMgrHelper 全局对象的析构，会自动调用Close释放全部管理。

	注：其中5，6步可循环迭代交替使用
*/
class CIResMgrHelper
{
public:
	typedef BOOL (*ProcCreateResMgr)(DWORD dwExpectVersion, ITrace *pTrace, IResMgr **ppResMgr);

	CIResMgrHelper()
	{
		m_hdll = NULL;
		m_pResMgr = NULL;
	}
	~CIResMgrHelper()
	{
		Close();	
	}
	IResMgr* Create(ITrace *pTrace)
	{
		Close();
		try
		{
			if(m_hdll == NULL)
			{
				#ifdef _DEBUG
					m_hdll = ::LoadLibrary("FKResManager_D.dll");
				#else
					m_hdll = ::LoadLibrary("FKResManager.dll");
				#endif
				if(m_hdll == NULL)
					throw "Can not load ResourceManager.dll";
			}
			ProcCreateResMgr proc;
			proc = (ProcCreateResMgr)GetProcAddress(m_hdll, "CreateResMgr");
			if(proc == NULL)
				throw "调用 CreateResMgr 失败！无法找到该函数地址！)";

			if(!proc(VERSION_QUERY_RESOURCE_MANAGER,pTrace,&m_pResMgr))
				throw "CreateResMgr() 出现异常！";
			return m_pResMgr;
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);OutputDebugString("\r\n");
			return FALSE;
		}
		catch(...)
		{
			OutputDebugString("CIResMgrHelper::Create() 出现异常！\r\n");
			return FALSE;
		}
	}
	void Close()
	{
		if (m_pResMgr)
		{
			m_pResMgr->Release();
			m_pResMgr = NULL;
		}
		if(m_hdll)
		{
			FreeLibrary(m_hdll);
			m_hdll = NULL;
		}
	}
public:
	IResMgr* m_pResMgr;
private:
	HINSTANCE		m_hdll;
};
//------------------------------------------------------------------------

