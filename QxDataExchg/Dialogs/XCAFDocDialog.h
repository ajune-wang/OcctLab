#pragma once
#include "afxcmn.h"

#include <TDocStd_Document.hxx>
#include "resource.h"

// XCAFDocDialog �Ի���

class XCAFDocDialog : public CDialog
{
	DECLARE_DYNAMIC(XCAFDocDialog)

public:
	XCAFDocDialog(const Handle(TDocStd_Document) &aDoc,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~XCAFDocDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_XCAF_DOC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

protected:
	void						ShowXDocStruct();

	void						ShowLabel(TDF_Label aLabel,HTREEITEM hParent);

protected:
	CTreeCtrl xCAFDocTree_;
	Handle(TDocStd_Document)	xCAFDoc_;	

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
