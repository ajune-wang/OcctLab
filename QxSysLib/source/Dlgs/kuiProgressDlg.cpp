// Dialogs\kuiProgressDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "kcProgressThread.h"
#include "kuiProgressDlg.h"


// kuiProgressDlg �Ի���

IMPLEMENT_DYNAMIC(kuiProgressDlg, CDialog)

kuiProgressDlg::kuiProgressDlg(kcProgressThread *pProgThread,CWnd* pParent /*=NULL*/)
: CDialog(kuiProgressDlg::IDD, pParent),pProgThread_(pProgThread)
, szProgText_(_T(""))
{
	nIDTimer_ = 0;
}

kuiProgressDlg::~kuiProgressDlg()
{
}

void kuiProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_PROGTEXT, szProgText_);
}


BEGIN_MESSAGE_MAP(kuiProgressDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_BUTTON_TERMINATE, &kuiProgressDlg::OnClickedButtonTerminate)
END_MESSAGE_MAP()


// kuiProgressDlg ��Ϣ�������


BOOL kuiProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	nIDTimer_ = SetTimer(1,20,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void kuiProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(!pProgThread_->bRunning_){
		KillTimer(nIDTimer_);

		CDialog::OnOK();
	}else{
		std::string szText = pProgThread_->GetProgText();
		szProgText_ = szText.c_str();
		UpdateData(FALSE);
	}

	CDialog::OnTimer(nIDEvent);
}


void kuiProgressDlg::OnClickedButtonTerminate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	pProgThread_->Stop();

	if(nIDTimer_){
		KillTimer(nIDTimer_);
	}

	CDialog::OnOK();
}
