// GUI\LuaScriptDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "QxLuaScript.h"
#include "QxLuaState.h"
#include "LuaScriptDlg.h"


// ��ǰ�Ľű��Ի���ָ��
// ��¼Ψһ�ĶԻ���
static CLuaScriptDlg *s_pScriptDlg = NULL;

bool ShowLuaScriptDialog(bool bShow)
{
	if(s_pScriptDlg == NULL){
		//��һ��ʹ�ýű������г�ʼ��
		
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		s_pScriptDlg = new CLuaScriptDlg;
		s_pScriptDlg->Create(IDD_DIALOG_LUA_SCRIPT);
	}
	//��ʾ����
	if(bShow){
		s_pScriptDlg->ShowWindow(SW_SHOW);
	}else{
		s_pScriptDlg->ShowWindow(SW_HIDE);
	}

	return true;
}

// CLuaScriptDlg �Ի���

IMPLEMENT_DYNAMIC(CLuaScriptDlg, CDialog)

CLuaScriptDlg::CLuaScriptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLuaScriptDlg::IDD, pParent)
	, m_szScript(_T(""))
	, m_szOutput(_T(""))
{
	s_pScriptDlg = this;
}

CLuaScriptDlg::~CLuaScriptDlg()
{
	s_pScriptDlg = NULL;
}

void CLuaScriptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LUA_SCRIPT, m_szScript);
	DDX_Text(pDX, IDC_EDIT_OUTPUT, m_szOutput);
	DDX_Control(pDX, IDC_EDIT_OUTPUT, m_outputEdit);
}

//���һ��
void  CLuaScriptDlg::OutputLine(const char *pszLine)
{
	if(pszLine){
		if(!m_szOutput.IsEmpty()){
			if(m_szOutput.GetLength() >= 2){
				CString szp = m_szOutput.Right(2);
				if(szp != "\r\n")
					m_szOutput += "\r\n";
			}
		}
		m_szOutput += pszLine;
		m_szOutput += "\r\n";
		UpdateData(FALSE);

		int nLines = m_outputEdit.GetLineCount();
		m_outputEdit.LineScroll(nLines);
	}
}

//�������
void  CLuaScriptDlg::Output(const char *pszText)
{
	if(pszText){
		m_szOutput += pszText;
		UpdateData(FALSE);

		int nLines = m_outputEdit.GetLineCount();
		m_outputEdit.LineScroll(nLines);
	}
}


BEGIN_MESSAGE_MAP(CLuaScriptDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_SCRIPT, &CLuaScriptDlg::OnBnClickedButtonCheckScript)
	ON_BN_CLICKED(IDC_BUTTON_RUN_SCRIPT, &CLuaScriptDlg::OnBnClickedButtonRunScript)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_SCRIPT, &CLuaScriptDlg::OnBnClickedButtonClearScript)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_OUTPUT, &CLuaScriptDlg::OnBnClickedButtonClearOutput)
END_MESSAGE_MAP()


// CLuaScriptDlg ��Ϣ�������


void CLuaScriptDlg::OnBnClickedButtonCheckScript()
{
	UpdateData();
	if(m_szScript.IsEmpty())
		return;

	QxLuaState luaState;
	std::string szErr;

	if(!luaState.Compile(m_szScript)){
		szErr = luaState.GetErrorMsg();
		OutputLine(szErr.c_str());
	}else{
		OutputLine("���ͨ��.");
	}
}


void CLuaScriptDlg::OnBnClickedButtonRunScript()
{
	UpdateData();
	if(m_szScript.IsEmpty())
		return;

	QxLuaState luaState;
	std::string szErr;

	if(!luaState.Execute(m_szScript)){
		szErr = luaState.GetErrorMsg();
		OutputLine(szErr.c_str());
	}else{
		OutputLine("ִ�гɹ�.");
	}
}


void CLuaScriptDlg::OnBnClickedButtonClearScript()
{
	m_szScript.Empty();
	UpdateData(FALSE);
}


void CLuaScriptDlg::OnBnClickedButtonClearOutput()
{
	m_szOutput.Empty();
	UpdateData(FALSE);
}
