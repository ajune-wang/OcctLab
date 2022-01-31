#pragma once

#include "afxdialogex.h"
#include "..\resource.h"

// CInputDblDlg �Ի���

class CInputDblDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInputDblDlg)

public:
	CInputDblDlg(double value,const char *pszPrompt,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInputDblDlg();

	void			SetSpinIncrement(double incVal);

	double		GetValue() const { return m_dValue; }

// �Ի�������
	enum { IDD = IDD_DIALOG_INPUT_DOUBLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

protected:
	double m_dValue;
	CString m_szPrompt;
	double m_dSpinIncrement;//spinÿ�����ӻ���ٵ�����

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	
public:
	afx_msg void OnDeltaposSpinDblValue(NMHDR *pNMHDR, LRESULT *pResult);
};
