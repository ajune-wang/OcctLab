#pragma once

#include "..\resource.h"

// CFilterDispDialog �Ի���

class CFilterDispDialog : public CDialog
{
	DECLARE_DYNAMIC(CFilterDispDialog)

public:
	CFilterDispDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFilterDispDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_FILTER_DISPLAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	BOOL m_bDispPoint;
	BOOL m_bDispCurve;
	BOOL m_bDispWire;
	BOOL m_bDispSurface;
	BOOL m_bDispShell;
	BOOL m_bDispSolid;
	BOOL m_bDispCompSolid;
	BOOL m_bDispCompound;
	afx_msg void OnBnClickedOk();
};
