#pragma once

#include "resource.h"

// kuiProgressDlg �Ի���

class kcProgressThread;

class kuiProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(kuiProgressDlg)

public:
	kuiProgressDlg(kcProgressThread *pProgThread,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~kuiProgressDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_PROGRESS };

protected:
	kcProgressThread		*pProgThread_;
	UINT_PTR				nIDTimer_;
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClickedButtonTerminate();
protected:
	CString szProgText_;
};
