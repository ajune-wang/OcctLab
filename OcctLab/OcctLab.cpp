// OcctTry.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "QxPluginMgr.h"
#include "kiCommandMgr.h"
#include "kuiSyscmdPlugin.h"
#include "kuiTrsfPlugin.h"
#include "kcHandleMgr.h"
#include "ScriptLib.h"
#include "MainFrm.h"
#include "OcctLabDoc.h"
#include "OcctLabView.h"
#include <OpenGl_GraphicDriver.hxx>
#include "OcctLab.h"


// COcctLabApp

BEGIN_MESSAGE_MAP(COcctLabApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// COcctLabApp ����

COcctLabApp::COcctLabApp()
{
#ifdef _DEBUG
	// Disable tracking of memory for the scope of the InitInstance()
	AfxEnableMemoryTracking(FALSE);
#endif  // _DEBUG

	try
	{
		Handle(Aspect_DisplayConnection) aDisplayConnection;
		m_graphicDriver = new OpenGl_GraphicDriver (aDisplayConnection);
	}
	catch(Standard_Failure)
	{
		AfxMessageBox("Fatal error during graphic initialization",MB_ICONSTOP);
		ExitProcess(1);
	}

	m_pPluginMgr = NULL;
}


// Ψһ��һ�� COcctLabApp ����

COcctLabApp theApp;

// COcctLabApp ��ʼ��

BOOL COcctLabApp::InitInstance()
{
#ifdef _DEBUG
	// Disable tracking of memory for the scope of the InitInstance()
	AfxEnableMemoryTracking(FALSE);
#endif  // _DEBUG

	//�����ʼ��ʧ��,���Ƴ�.
	if(m_graphicDriver.IsNull())
		return FALSE;

	TRACE("\n ------ CP_ACP is %d,OEM_ACP is %d.------\n",GetACP(),GetOEMCP());

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CWinApp::InitInstance();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)

	if(!LoadPlugin()){
		return FALSE;
	}

	//�ű���ʼ��
	///NOTE:����ط���ʼ��δ�غ��ʣ�Ŀǰ�ȷ������
	//
	LuaScriptInit();

	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(COcctLabDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(COcctLabView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// ������׼������DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������ں�׺ʱ�ŵ��� DragAcceptFiles��
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand  ֮����

	return TRUE;
}

// ���ز��,��ȡ�����������Ϣ
//
BOOL	COcctLabApp::LoadPlugin()
{
	m_pPluginMgr = new QxPluginMgr;
	if(!m_pPluginMgr)
		return FALSE;

	QxCmdPlugin *pPlugin = NULL;
		
	pPlugin = new kuiTrsfPlugin;
	m_pPluginMgr->LoadPlugin(pPlugin);

	pPlugin = new kuiSyscmdPlugin;
	m_pPluginMgr->LoadPlugin(pPlugin);

	SearchAndLoadDynPlugin();

	return TRUE;
}

//��̬���ز��
BOOL COcctLabApp::SearchAndLoadDynPlugin()
{
	//��ȡ���Ŀ¼����ִ��Ŀ¼��plugin��Ŀ¼��
	TCHAR szBuff[_MAX_PATH];
	::GetModuleFileName(m_hInstance, szBuff, _MAX_PATH);
	size_t n = strnlen_s(szBuff,_MAX_PATH);
	while(n > 0 && szBuff[n-1] != '\\'){
		n --;
	}
	szBuff[n] = '\0';
	CString szPath = szBuff;
	szPath += "Plugin"; //

	//����dll�ļ�
	CFileFind fileFind;
	CString szFindFlt = szPath + "\\*.dll";
	BOOL bFound = fileFind.FindFile(szFindFlt);
	while(bFound){
		bFound = fileFind.FindNextFile();
		//
		CString szDllPath = fileFind.GetFilePath();
		//
		if(m_pPluginMgr->LoadDynPlugin(szDllPath)){
			TRACE("\n###---load plugin %s.------\n",szDllPath);
		}
	}

	return TRUE;
}

int COcctLabApp::ExitInstance()
{
	if(m_pPluginMgr){
		m_pPluginMgr->Destroy();
		delete m_pPluginMgr;
	}

	return CWinApp::ExitInstance();
}


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void COcctLabApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// COcctLabApp ��Ϣ�������


