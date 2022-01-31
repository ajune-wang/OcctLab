#pragma once
#include "afxcmn.h"

#include "resource.h"

//���Կ�Ĵ����࣬���ཫ��Ӧ������ӵ��öԻ�����
class kcPropHandler{
public:
	virtual ~kcPropHandler(){}

	//��Ҫ�ӿڣ�����������Ϣ
	virtual int SetProperty(CTreeCtrl &propTree) = 0;
};

// kcPropertyDlg �Ի���

class kcPropertyDlg : public CDialog
{
	DECLARE_DYNAMIC(kcPropertyDlg)

public:
	kcPropertyDlg(kcPropHandler *pPropHandler,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~kcPropertyDlg();

// �Ի�������
	enum { IDD = IDD_PROPERTY_DIALOG };

	void  ClearAllProperty();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

protected:
	kcPropHandler		*m_pPropHandler;//���Դ������ָ�룬�ⲿ���룬���ﲻ���ͷ�
	CTreeCtrl m_propTree;//������
};
