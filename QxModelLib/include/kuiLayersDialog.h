#pragma once
#include "afxwin.h"


// kuiLayersDialog �Ի���
class kcModel;

class kuiLayersDialog : public CDialogEx
{
	DECLARE_DYNAMIC(kuiLayersDialog)

public:
	kuiLayersDialog(kcModel *pModel,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~kuiLayersDialog();

	kcLayer*  GetSelLayer() const;

// �Ի�������
	enum { IDD = IDD_LAYERS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

protected:
	kcModel		*pModel_; //ģ��
	kcLayer		*pSelLayer_;

	CComboBox m_comboLayers;
public:
	virtual BOOL OnInitDialog();
};
