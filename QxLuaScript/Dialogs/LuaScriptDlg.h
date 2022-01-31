#pragma once
#include "afxwin.h"

#include "..\resource.h"

// CLuaScriptDlg �Ի���

class CLuaScriptDlg : public CDialog
{
	DECLARE_DYNAMIC(CLuaScriptDlg)

public:
	CLuaScriptDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLuaScriptDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_LUA_SCRIPT };

public:
	//���һ��
	void				OutputLine(const char *pszLine);
	//�������
	void				Output(const char *pszText);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCheckScript();
	afx_msg void OnBnClickedButtonRunScript();
	afx_msg void OnBnClickedButtonClearScript();
	afx_msg void OnBnClickedButtonClearOutput();

protected:
	CString m_szScript;
	CString m_szOutput;
public:
	CEdit m_outputEdit;
};
