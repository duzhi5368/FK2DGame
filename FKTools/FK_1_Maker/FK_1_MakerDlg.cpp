
// FK_1_MakerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FK_1_Maker.h"
#include "FK_1_MakerDlg.h"
#include "CCsvToScp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFK_1_MakerDlg 对话框




CFK_1_MakerDlg::CFK_1_MakerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFK_1_MakerDlg::IDD, pParent)
{
	m_CStrFileName = _T("");
	m_bPlus2Comma = TRUE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFK_1_MakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_CStrFileName);
	DDX_Check(pDX, IDC_CHECK1, m_bPlus2Comma);
}

BEGIN_MESSAGE_MAP(CFK_1_MakerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CFK_1_MakerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFK_1_MakerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK1, &CFK_1_MakerDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CFK_1_MakerDlg 消息处理程序

BOOL CFK_1_MakerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFK_1_MakerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFK_1_MakerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//------------------------------------------------------------------------
// 选择文件按钮
void CFK_1_MakerDlg::OnBnClickedButton1()
{
	CFileDialog FileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Excel生成的CSV格式 (*.csv)|*.csv|", this);
	if ( IDOK != FileDialog.DoModal())
	{
		return;
	}
	m_CStrFileName = FileDialog.GetPathName();
	UpdateData(FALSE);

	return;
}
//------------------------------------------------------------------------
// 转换文件按钮
void CFK_1_MakerDlg::OnBnClickedButton2()
{
	if ( m_CStrFileName.GetLength() <= 0)
	{
		MessageBox("请输入CSV文件名!");
		return;
	}	
	TCHAR *szCSVFileName = m_CStrFileName.GetBuffer(m_CStrFileName.GetLength());
	m_CStrFileName.ReleaseBuffer();

	TCHAR szDefSaveFileName[MAX_STRING_Name] = {0};
	ChangeToSCPName(szDefSaveFileName, szCSVFileName);

	TCHAR szSaveFileName[MAX_STRING_Name] = {0};
	if ( !MySaveFile(szDefSaveFileName, szSaveFileName) )
	{
		return;
	}

	TCHAR szErrmsg[MAX_STRING_Name] = {0};
	CCSVToSCP MyCSVToSCP;
	if ( !MyCSVToSCP.MakeCSVToSCP(szCSVFileName, szSaveFileName, szErrmsg, m_bPlus2Comma) )
	{
		MessageBox(szErrmsg);
	}
	else
	{
		MessageBox("CSV -> FK1 文件转换成功...");
	}
}
//------------------------------------------------------------------------
// 转换文件名
void CFK_1_MakerDlg::ChangeToSCPName(LPTSTR szSCPName, LPCTSTR szCSVName)
{	
	strcpy(szSCPName, szCSVName);
	char *pPoint = strrchr(szSCPName, '.');
	strcpy( (pPoint+ 1), "FK1" );
}
//------------------------------------------------------------------------
//打开保存文件的窗口
BOOL CFK_1_MakerDlg::MySaveFile(PTSTR szDefSaveFileName, LPTSTR szRealSaveFileName)
{
	CFileDialog FileDialog(FALSE, NULL, szDefSaveFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"FK1资源文件 (*.FK1)|*.FK1|", this);
	if ( IDOK != FileDialog.DoModal())
	{
		return FALSE;
	}
	CString cstrFileName = FileDialog.GetPathName();
	UpdateData(FALSE);

	strcpy(szRealSaveFileName, cstrFileName.GetBuffer(cstrFileName.GetLength()) );
	cstrFileName.ReleaseBuffer();

	return TRUE;
}
//------------------------------------------------------------------------
void CFK_1_MakerDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bPlus2Comma = !m_bPlus2Comma;
}
//------------------------------------------------------------------------