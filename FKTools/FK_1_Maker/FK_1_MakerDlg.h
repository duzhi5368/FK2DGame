
// FK_1_MakerDlg.h : 头文件
//

#pragma once


// CFK_1_MakerDlg 对话框
class CFK_1_MakerDlg : public CDialog
{
// 构造
public:
	CFK_1_MakerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FK_1_Maker_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
private:
	CString	m_CStrFileName;													// CSV文件路径
	void ChangeToSCPName(LPTSTR szSCPName, LPCTSTR szCSVName);				// 转换文件名
	BOOL MySaveFile(PTSTR szDefSaveFileName, LPTSTR szRealSaveFileName);	// 打开保存文件的窗口
	BOOL m_bPlus2Comma;														// 是否将 + 转换为 ,
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCheck1();
};
