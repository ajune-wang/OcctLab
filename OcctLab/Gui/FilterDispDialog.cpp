// Gui\FilterDispDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "..\OcctLab.h"
#include "FilterDispDialog.h"


// CFilterDispDialog �Ի���

IMPLEMENT_DYNAMIC(CFilterDispDialog, CDialog)

CFilterDispDialog::CFilterDispDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterDispDialog::IDD, pParent)
	, m_bDispPoint(TRUE)
	, m_bDispCurve(TRUE)
	, m_bDispWire(TRUE)
	, m_bDispSurface(TRUE)
	, m_bDispShell(FALSE)
	, m_bDispSolid(TRUE)
	, m_bDispCompSolid(TRUE)
	, m_bDispCompound(TRUE)
{

}

CFilterDispDialog::~CFilterDispDialog()
{
}

void CFilterDispDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_POINT, m_bDispPoint);
	DDX_Check(pDX, IDC_CHECK_CURVE, m_bDispCurve);
	DDX_Check(pDX, IDC_CHECK_WIRE, m_bDispWire);
	DDX_Check(pDX, IDC_CHECK_SURFACE, m_bDispSurface);
	DDX_Check(pDX, IDC_CHECK_SHELL, m_bDispShell);
	DDX_Check(pDX, IDC_CHECK_SOLID, m_bDispSolid);
	DDX_Check(pDX, IDC_CHECK_COMPSOLID, m_bDispCompSolid);
	DDX_Check(pDX, IDC_CHECK_COMPOUND, m_bDispCompound);
}


BEGIN_MESSAGE_MAP(CFilterDispDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CFilterDispDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CFilterDispDialog ��Ϣ�������

BOOL CFilterDispDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CFilterDispDialog::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialog::OnOK();
}

void CFilterDispDialog::OnBnClickedOk()
{
	
	OnOK();
}
