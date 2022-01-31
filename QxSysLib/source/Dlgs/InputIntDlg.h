#pragma once

//#include "afxdialogex.h"
#include "..\resource.h"

// CInputIntDlg �Ի���

class CInputIntDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputIntDlg)

public:
	CInputIntDlg(int value,const char *pszPrompt,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInputIntDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_INPUT_INT };

	void		SetSpinIncrement(int incVal);

	int		GetValue() const { return m_nValue; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

protected:
	int m_nValue;
	CString m_szPrompt;
	int m_nSpinIncrement;//spinÿ�����ӻ���ٵ���

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	
public:
	afx_msg void OnDeltaposSpinIntValue(NMHDR *pNMHDR, LRESULT *pResult);
};
