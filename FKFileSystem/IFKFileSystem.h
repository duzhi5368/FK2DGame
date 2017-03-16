/**
*	created:		2012-6-15   22:04
*	filename: 		IFKFileSystem
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
struct ITrace;
//------------------------------------------------------------------------
// 文件系统版本号
#define VERSION_QUERY_FILE_SYSTEM		1
// 文件系统类型(File System Style)宏定义
#define FSS_DIRECTORY			0	// 文件系统以目录态形式操作
#define FSS_PACKAGE				1	// 文件系统以包态形式操作
// 创建文件的类型(CreateFile Style)宏定义
#define CFS_NEW					0	// 新建一个文件，不允许更新
#define CFS_UPDATE				1	// 更新一个文件，文件必须存在
// 字符串长度定义
#define MAX_BLOCK_NAME_LEN		32  // 最大块名长度
#define MAX_RELATIVE_PATH_LEN	64  // 最大相对路径长度
// 文件系统压包和解包回调函数,dwFileCounts为每次回调时处理了的文件数目
typedef void (CALLBACK* PACKPROC)(DWORD dwFileCounts);

//------------------------------------------------------------------------
// 文件对象接口。功能类似于MFC的CFile类和CStdioFile
struct IFileObject
{
	/* 有关文件打开模式的信息: 
	"r"		读    ,文件不存在则失败
	"w"		写    ,文件不存在则创建,存在则清空内容
	"a"		添加  ,文件不存在则创建
	"r+"	读写  ,文件不存在则失败
	"w+"	读写  ,文件不存在则创建,存在则清空内容
	"a+"	读添加,文件不存在则创建
	"t"		文本方式
	"b"		二进制方式
	*/

	// 文件打开关闭(默认以只读、二进制形式打开)
	virtual BOOL Open(LPCSTR lpszMode = "rb") = 0;
	virtual BOOL Close() = 0;
	
	// 流的读写
	// 对于包文件中的文件对象拒绝所有越界的读写
	virtual UINT Read(void* lpBuf, UINT nCount) = 0;
	virtual BOOL Write(const void* lpBuf, UINT nCount) = 0;

	// 文本文件操作(与CStdioFile类的成员函数类似)
	// 对于包文件中的文件对象拒绝所有越界的读写
	// 当到了文件尾后,函数返回值为NULL
	virtual LPSTR ReadString(LPSTR lpsz, UINT nMax) = 0;
	virtual BOOL WriteString(LPCSTR lpsz) = 0;

	// 文件指针位置定位
	// 包文件的操作与单个文件类似
	// 参考: SEEK_SET 文件头; SEEK_CUR 文件当前位置; SEEK_END 文件尾
	virtual BOOL Seek(LONG lOff, UINT nFrom = SEEK_SET) = 0;
	virtual BOOL SeekToBegin() = 0;
	virtual BOOL SeekToEnd() = 0;
	virtual int	GetPosition() = 0;

	// 文件缓存清空
	// 包文件不支持
	virtual BOOL Flush() = 0;

	// 获取文件长度(文件没打开时也能获取长度)
	// 如果是包文件,则是指压缩后的长度
	virtual DWORD GetLength() = 0;
	virtual BOOL SetLength(DWORD dwNewLen) = 0;

	// 获得文件名,形如"filetitle.ext"
	virtual LPCSTR GetFileName() = 0;
	// 获得文件标题名,形如"filetitle"
	virtual BOOL GetFileTitle(char* buf) = 0;
	// 获得文件的路径名,形如"d:\dxGame\filetitle.ext"
	virtual LPCSTR GetFilePath() = 0;

	// 文件是否存在
	// 注: 包文件不支持,包态时可通过IFileSystem->GetFile(...)来判断是否存在
	virtual BOOL IsExist() = 0;
	// 释放(记住:你有义务这样做)
	virtual BOOL Release() = 0;
};

//------------------------------------------------------------------------
// 文件系统接口，对文件对象进行操作
struct IFileSystem
{
	// 创建文件
	// 如果文件存在,创建失败,返回NULL,并Trace出来;
	// 返回值: 成功返回文件对象;失败返回NULL
	// 注: 1、用户需自己释放获得的文件对象IFileObject->Release()
	//	   2、文件名采用相对路径
	//	   3、参数pData和nSize仅仅用于在包态中创建文件;在目录态时忽略
	//	   4、目录态时,该操作新建一空文件(没有打开,用户自己设置打开模式来打开!!)
	virtual IFileObject* CreateFile(LPCSTR szFileName, char* pData = NULL, int nSize = 0, UINT nStyle = CFS_NEW) = 0;

	// 获得某个文件对象(但不打开对应文件!)
	// 返回值: 成功返回文件对象;失败返回NULL
	// 文件名采用相对路径
	virtual IFileObject* GetFile(LPCSTR szFileName) = 0;

	// 创建目录(目录态支持多级创建,包态不支持)
	// 成功返回TRUE;失败返回FALSE
	// 目录名采用相对路径
	virtual BOOL CreateDirectory(LPCSTR szDirectoryName) = 0;

	// 删除指定的文件
	// 如果待删的文件不存在,则跳过此操作(权当执行了删除),返回TRUE,并Trace出来;
	// 返回值: 成功返回TRUE;失败返回FALSE
	// 文件名采用相对路径;包文件中的删除操作仅仅是标识为无效而已
	virtual BOOL DeleteFile(LPCSTR szFileName) = 0;

	// 删除目录(包括多级和目录里的文件)
	// 如果待删的目录不存在,则跳过此操作(权当执行了删除),返回TRUE,并Trace出来;
	// 返回值: 成功返回TRUE;失败返回FALSE
	// 目录名采用相对路径;包文件中的删除操作仅仅是标识为无效而已
	virtual BOOL DeleteDirectory(LPCSTR szDirectoryName) = 0;

	// 文件(目录)重命名
	// 如果待改名的文件不存在,则更名失败,返回FALSE,并Trace出来;
	// 返回值: 成功返回TRUE;失败返回FALSE
	// 注: 1、仅仅修改名字,不允许改变路径; 2、采用相对路径
	virtual BOOL Rename(LPCSTR szOldFileName,LPCSTR szNewFileName) = 0;
	
	// 1、将目录态文件系统复制成包态文件系统；或反过来(nFlags == 0)
	// 2、将当前文件子系统复制到另一个文件子系统，(nFlags != 0)
	// 如在包文件系统中，经常增删时出现很多空洞，复制到新文件子系统可以解决这个问题
	// 注: 这里的参数采用绝对路径!!!
	virtual BOOL CopyToFileSystem(LPCSTR szNewFileSystemName, int nFlags = 0, PACKPROC lpfnPackProC = NULL) = 0;

	// 释放
	// 注：仅仅释放自己,不关心由她创建的IFileObject
	virtual BOOL Release() = 0;
	
	virtual IFileObject* GetFirstFile() = 0;
	virtual IFileObject* GetNextFile() = 0;
	virtual DWORD GetLength() = 0;
	virtual IFileObject* GetIndexFile(LPCSTR szFileName, DWORD dwCompSize) = 0;
};


//------------------------------------------------------------------------
// 文件系统辅助类
class CIFileSystemHelper
{
public:
	typedef BOOL (*ProcCreateFileSystem)(DWORD dwExpectVersion,
			LPCSTR szFileSystemName, DWORD dwStyle, ITrace* pTrace,
			IFileSystem** ppFileSystem,	void* Reserved );

	CIFileSystemHelper()
	{
		m_hdll = NULL;
	}
	~CIFileSystemHelper()
	{
		if(m_hdll)
		{
			::FreeLibrary(m_hdll);
			m_hdll = NULL;
		}
	}
	BOOL Create(IFileSystem** ppFileSystem, LPCSTR szFileSystemName, DWORD dwStyle, ITrace* pTrace,	void* Reserved = NULL)
	{
		try
		{
			if(m_hdll == NULL)
			{
				#ifdef _DEBUG
					m_hdll = ::LoadLibrary("FKFileSystem_D.dll");
					if(m_hdll == NULL)
						throw "读取 FileSystemD.dll 失败！";
				#else
					m_hdll = ::LoadLibrary("FKFileSystem.dll");
					if(m_hdll == NULL)
						throw "读取 FileSystem.dll 失败！";
				#endif
			}
			ProcCreateFileSystem proc;
			proc = (ProcCreateFileSystem)GetProcAddress(m_hdll, "CreateFileSystem");
			if(proc == NULL)
				throw "无法获取函数 CreateFileSystem 地址！')";

			if(!proc(VERSION_QUERY_FILE_SYSTEM, szFileSystemName, dwStyle, pTrace, ppFileSystem, Reserved))
				throw "CreateFileSystem() 出错！";
			return ppFileSystem != NULL;
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);OutputDebugString("\r\n");
			return FALSE;
		}
		catch(...)
		{
			OutputDebugString("CreateFileSystem() 出现异常！\r\n");
			return FALSE;
		}
	}
private:
	HINSTANCE		m_hdll;
};
//------------------------------------------------------------------------