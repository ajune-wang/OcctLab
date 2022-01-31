// OcctTryDoc.cpp :  COcctLabDoc ���ʵ��
//

#include "stdafx.h"
#include <OpenGl_GraphicDriver.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include "QxCommandItem.h"
#include "QxPluginMgr.h"
#include "kiCommandMgr.h"
#include "kcModel.h"
#include "kcSnapMngr.h"
#include "kiCommand.h"
#include "kiUndoMgr.h"
#include "OcctLab.h"
#include "kcIGESReader.h"
#include "kcDXFReader.h"
#include "kcBRepReader.h"
#include "kcBRepExport.h"
#include "kcSTEPReader.h"
#include "kcSTEPExport.h"
#include "kcIGESExport.h"
#include "QxStgDocWriter.h"
#include "QxStgDocReader.h"
#include "kuiInterface.h"
#include "kiSelSet.h"
#include "kcDocContext.h"
#include "QxLuaScript.h"
#include "kcmDelete.h"
#include "mainfrm.h"
#include "OcctLabDoc.h"

// COcctLabDoc

IMPLEMENT_DYNCREATE(COcctLabDoc, CDocument)

BEGIN_MESSAGE_MAP(COcctLabDoc, CDocument)
	ON_COMMAND_RANGE(KI_CMD_ID_BEGIN,KI_CMD_ID_END,OnMenuItemDispatch)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(ID_MENU_IMPORT_IGES, OnMenuImportIges)
	ON_COMMAND(ID_MENU_IMPORT_DXF, OnMenuImportDxf)
	ON_COMMAND(IDM_IMPORT_BREP_FILE, OnImportBrepFile)
	ON_COMMAND(ID_IMPORT_STEP_FILE, &COcctLabDoc::OnImportStepFile)
	ON_COMMAND(ID_MENU_EXPORT_STEP, &COcctLabDoc::OnMenuExportStep)
	ON_COMMAND(ID_MENU_EXPORT_BREP, &COcctLabDoc::OnMenuExportBrep)
	ON_COMMAND(ID_MENU_EXPORT_SELSTEP, &COcctLabDoc::OnMenuExportSelstep)
	ON_COMMAND(ID_MENU_EXPORT_SELBREP, &COcctLabDoc::OnMenuExportSelbrep)
	ON_COMMAND(ID_MENU_EXPORT_IGES, &COcctLabDoc::OnMenuExportIges)
	ON_COMMAND(IDM_EXPORT_SEL_IGES, &COcctLabDoc::OnExportSelIges)
END_MESSAGE_MAP()


// COcctLabDoc ����/����

COcctLabDoc::COcctLabDoc()
{
	// TODO: �ڴ����һ���Թ������
	m_pModel = NULL;
	m_pCommandMgr = NULL;
	m_pSnapMgr = NULL;
	m_bNewDocument = TRUE;
	m_bSerializeOK = FALSE;

	OcctInit();
}

COcctLabDoc::~COcctLabDoc()
{
	if(m_pModel){
		m_pModel->Destroy();
		delete m_pModel;
	}

	if(m_pCommandMgr){
		m_pCommandMgr->Destroy();
		delete m_pCommandMgr;
	}

	if(m_pSnapMgr){
		delete m_pSnapMgr;
	}

	if(!m_AISContext.IsNull()){
		m_AISContext->RemoveAll(Standard_True);

		Handle(Graphic3d_GraphicDriver) hGLDrver = ((COcctLabApp *)AfxGetApp())->GetGraphicDriver();
		if(!hGLDrver.IsNull()){
			int num = hGLDrver->InquireViewLimit();
		}

		//m_AISContext.Nullify();

	}

	if(!m_Viewer.IsNull()){
		m_Viewer.Nullify();
		//
		//Handle(V3d_Viewer) aViewer;
		//glb_GetContext().SetViewer(aViewer);
	}
}

// ���캯����ʼ�����á�
//
BOOL	COcctLabDoc::OcctInit()
{
	Handle(Graphic3d_GraphicDriver) theGraphicDriver = ((COcctLabApp *)AfxGetApp())->GetGraphicDriver();
	if(theGraphicDriver.IsNull())
		return FALSE;

	//����viewer����ÿ��Docһ��
	m_Viewer = new V3d_Viewer(theGraphicDriver);
	m_Viewer->SetDefaultLights();
	m_Viewer->SetLightOn();

	//����Context����ÿ��docһ��
	m_AISContext = new AIS_InteractiveContext(m_Viewer);

	//��Ⱦģʽ.
	m_AISContext->SetDisplayMode(AIS_Shaded,Standard_False);
	m_AISContext->SetIsoNumber(3);
	m_AISContext->IsoOnPlane(Standard_True);
	//m_AISContext->SelectionColor(Quantity_NOC_GREEN);
	//��ʼ��������ʾģʽ
	Handle(Prs3d_Drawer) aDrawer = m_AISContext->DefaultDrawer();
	aDrawer->SetFaceBoundaryDraw(true);

	//�ʷ־��ȷ�ʽ
	TRACE("\n DeviationCoefficient is %f.",m_AISContext->DeviationCoefficient());
	TRACE("\n MaximalDeviationCoefficient is %f.",aDrawer->MaximalChordialDeviation());
	TRACE("\n DeviationAlgle is %f.",m_AISContext->DeviationAngle());

	Aspect_TypeOfDeflection defType = aDrawer->TypeOfDeflection();
	if(defType == Aspect_TOD_RELATIVE){
		TRACE("\n type of deflection is Aspect_TOD_RELATIVE.");
	}else{
		TRACE("\n type of deflection is Aspect_TOD_ABSOLUTE.");
	}
	// �趨Ϊ����ֵģʽ����ʾЧ�����á�
	aDrawer->SetTypeOfDeflection(Aspect_TOD_ABSOLUTE);//ȡ����ֵ
	aDrawer->SetMaximalChordialDeviation(0.01);
	//m_AISContext->SetDeviationCoefficient(0.1);

	// ����ģ�Ͷ���ÿ��Docһ��
	m_pModel = new kcModel(m_AISContext);
	//��¼��Ҫ�Ķ����Ա�����ȶ�����Է���ķ���
	m_pModel->Initialize();

	m_pSnapMgr = new kcSnapMngr(m_pModel);

	pDocContext_ = new kcDocContext(this,m_AISContext,m_pModel);
	pDocContext_->SetSnapManager(m_pSnapMgr);

	//�ű���ʼ��
	LuaScriptInit(pDocContext_);

	return TRUE;
}

// ��ʼ�����������
// ���������Ҫʹ�õ�һЩ���ڶ�����Ҫ���������ʹ�ã�
// CDocument�Ĺ��캯������û�д������ڶ�����˲��ܵ��øú���
BOOL	COcctLabDoc::InitCommandMgr()
{
	if(m_pCommandMgr)
		return TRUE;

	QxPluginMgr *pPluginMgr = ((COcctLabApp *)AfxGetApp())->GetPluginMgr();
	if(!pPluginMgr)
		return FALSE;

	QxCommandItem *pCmdItem = NULL;
	UINT iCount = pPluginMgr->GetItemCount();
	if(iCount == 0)
		return FALSE;

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	kiInputEdit *pInputEdit = pFrame->GetInputEdit();
	pDocContext_->SetInputEdit(pInputEdit);

	//�����������������û��Docһ������
	m_pCommandMgr = new kiCommandMgr;
	m_pCommandMgr->Initialize(pDocContext_,pPluginMgr);
		
	pDocContext_->SetCmdManager(m_pCommandMgr);
	
	//ע��ϵͳ����ж�Ӧ�����в˵��
	RegisterSysCommand();

	return TRUE;
}

kiSelSet* COcctLabDoc::GetGlobalSelSet() const
{
	if(m_pModel){
		return m_pModel->GetGlobalSelSet();
	}
	return NULL;
}

BOOL COcctLabDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)
	m_bNewDocument = TRUE;
	m_pModel->Clear();
	m_pSnapMgr->Clear();
	//
	CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
	if(pMainFrm){
		kuiModelTreeInterface *pIF = pMainFrm->GetModelTreeInterface();
		if(pIF){
			pIF->SetModel(m_pModel);
			pIF->Initialize();
		}
	}
	
	//m_pModel->CreateDefaultLayer();

	return TRUE;
}

BOOL COcctLabDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	m_bNewDocument = FALSE;
	m_pModel->Clear();
	m_pSnapMgr->Clear();
	//
	CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
	if(pMainFrm)
	{
		kuiModelTreeInterface *pIF = pMainFrm->GetModelTreeInterface();
		if(pIF){
			pIF->SetModel(m_pModel);
			pIF->Initialize();
		}
	}

	m_bSerializeOK = FALSE;
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	if(!m_bSerializeOK){
		return FALSE;
	}
	
	return TRUE;
}

BOOL COcctLabDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: �ڴ����ר�ô����/����û���
	m_bSerializeOK = FALSE;
	if(!CDocument::OnSaveDocument(lpszPathName))
		return FALSE;
	return m_bSerializeOK;
}

void COcctLabDoc::DeleteContents()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDocument::DeleteContents();
}

void COcctLabDoc::OnCloseDocument()
{
	// TODO: �ڴ����ר�ô����/����û���
	// �ĵ��رպ�
	pDocContext_->Clear();

	CDocument::OnCloseDocument();
}

void COcctLabDoc::SetTitle(LPCTSTR lpszTitle)
{
	// TODO: �ڴ����ר�ô����/����û���

	CDocument::SetTitle(lpszTitle);
}

// COcctLabDoc ���л�

// �����Ҫ������Model�����ݣ���Ӧ�����Ա���ϵͳ��һЩ����
//
void COcctLabDoc::Serialize(CArchive& ar)
{
	bool bRc = false;

	if (ar.IsStoring()){
		QxStgDocWriter aDocWiter(pDocContext_);
		bRc = aDocWiter.Save(ar);
	}
	else
	{
		QxStgDocReader aDocReader(pDocContext_);
		bRc = aDocReader.Load(ar);
	}
	m_bSerializeOK = bRc ? TRUE : FALSE;
}

// COcctLabDoc ���

#ifdef _DEBUG
void COcctLabDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COcctLabDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// ע��ϵͳ����
// ����һЩ���Ͳ˵�������Ƚ����������.
//
BOOL  COcctLabDoc::RegisterSysCommand()
{
	ASSERT(m_pCommandMgr);
	kiCommand *pCommand = NULL;

	if(m_pCommandMgr){
		pCommand = new kcmDelete;
		pCommand->SetID(ID_EDIT_DELETE);
		m_pCommandMgr->RegisterCommand(pCommand);
	}

	return TRUE;
}

// COcctLabDoc ����
void	COcctLabDoc::OnMenuItemDispatch(UINT nID)
{
	if(m_pCommandMgr){
		m_pCommandMgr->ExecCommand(nID);
	}
}

// ִ��undo����
void COcctLabDoc::OnEditUndo()
{
	if(!m_pModel) return;

	kiUndoMgr *pUndoMgr = m_pModel->GetUndoMgr();
	if(!pUndoMgr)
		return;

	if(pUndoMgr->Undo())
	{
		UpdateAllViews(NULL);
	}
}

void COcctLabDoc::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	if(!m_pModel) return;

	kiUndoMgr *pUndoMgr = m_pModel->GetUndoMgr();
	if(!pUndoMgr)
		return;

	if(!pUndoMgr || !pUndoMgr->CanUndo())
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

// ִ����������
void COcctLabDoc::OnEditRedo()
{
	if(!m_pModel) return;

	kiUndoMgr *pUndoMgr = m_pModel->GetUndoMgr();
	if(!pUndoMgr)
		return;

	if(pUndoMgr->Redo())
	{
		UpdateAllViews(NULL);
	}
}

void COcctLabDoc::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	if(!m_pModel) return;

	kiUndoMgr *pUndoMgr = m_pModel->GetUndoMgr();
	if(!pUndoMgr)
		return;

	if(!pUndoMgr || !pUndoMgr->CanRedo())
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

// ɾ�����������
void COcctLabDoc::OnEditDelete()
{
	if(m_pCommandMgr)
		m_pCommandMgr->ExecCommand(ID_EDIT_DELETE);
}

void COcctLabDoc::OnUpdateEditDelete(CCmdUI *pCmdUI)
{
	
}

void COcctLabDoc::OnMenuImportIges()
{
	kcIGESReader igesReader(m_pModel);
	if(igesReader.DoImport())
	{
		UpdateAllViews(NULL);
	}
}

void COcctLabDoc::OnMenuExportIges()
{
	kcIGESExport aExport(m_pModel);
	aExport.DoExport();
}

void COcctLabDoc::OnExportSelIges()
{
	kcIGESExport aExport(m_pModel);
	aExport.EnableSelectExport(true);
	aExport.SetAISContext(m_AISContext);

	aExport.DoExport();
}

void COcctLabDoc::OnMenuImportDxf()
{
	//CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
	//COcctLabDoc *pDoc = (COcctLabDoc *)pMainFrm->GetActiveDocument();
	kcDXFReader dxfReader(m_pModel);
	if(dxfReader.DoImport())
	{
		UpdateAllViews(NULL);
	}
}


void COcctLabDoc::OnImportBrepFile()
{
	kcBRepReader brepReader(m_pModel);
	if(brepReader.DoImport())
	{
		UpdateAllViews(NULL);
	}
}


void COcctLabDoc::OnImportStepFile()
{
	kcXCAFSTEPReader stepReader(m_pModel);
	//kcSTEPReader stepReader(m_pModel);
	if(stepReader.DoImport())
	{
		UpdateAllViews(NULL);
	}
}


void COcctLabDoc::OnMenuExportStep()
{
	kcSTEPExport stepExp(m_pModel);
	stepExp.DoExport();
}


void COcctLabDoc::OnMenuExportBrep()
{
	kcBRepExport brepExp(m_pModel);
	brepExp.DoExport();
}


void COcctLabDoc::OnMenuExportSelstep()
{
	kcSTEPExport stepExp(m_pModel);
	stepExp.EnableSelectExport(true);
	stepExp.SetAISContext(m_AISContext);

	int nRc = AfxMessageBox("��Ϊ��ϵ���?",MB_YESNO);
	if(nRc == IDYES){
		stepExp.ExportAsCompound(true);
	}

	stepExp.DoExport();
}


void COcctLabDoc::OnMenuExportSelbrep()
{
	kcBRepExport brepExp(m_pModel);
	brepExp.EnableSelectExport(true);
	brepExp.SetAISContext(m_AISContext);
	brepExp.DoExport();
}





