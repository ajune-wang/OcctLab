#pragma once


// CLineStyleAndWidthDlg �Ի���

class CLineStyleAndWidthDlg : public CDialog
{
	DECLARE_DYNAMIC(CLineStyleAndWidthDlg)

public:
	CLineStyleAndWidthDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLineStyleAndWidthDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_LINE_DISPATTR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	
public:
	int m_nLineStyle;
	double m_dLineWidth;
};
