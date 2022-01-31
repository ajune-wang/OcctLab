#pragma once

#include "..\resource.h"

// CTextEditDialog �Ի���

class CTextEditDialog : public CDialog
{
	DECLARE_DYNAMIC(CTextEditDialog)

public:
	CTextEditDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTextEditDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_TEXTEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	LOGFONT		m_logFont;
	CString		m_szText;
	double		m_dTextHeight;
	int			m_nCharSpace;
	BOOL		m_bGroup;
	BOOL		m_bMakeFace;
	afx_msg void OnBnClickedButtonSelfont();
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();

protected:
	BOOL		m_bSelFont;
	
};
