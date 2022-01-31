// OcctTryView.cpp : COcctLabView ���ʵ��
//

#include "stdafx.h"

#include <WNT_Window.hxx>
#include <StdSelect_ShapeTypeFilter.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include "kiCommand.h"
#include "kiCommandMgr.h"
#include "kcBasePlane.h"
#include "XInputEdit.h"
#include "kiSelSet.h"
#include "kcEntity.h"
#include "kcModel.h"
#include "kuiInterface.h"
#include "kcSnapMngr.h"
#include "kvGrid.h"
#include "kcgTextLib.h"
#include "kcDocContext.h"
#include "QxWndTimerMgr.h"
#include "QxSysLib.h"
#include "OcctLab.h"
#include "mainFrm.h"
#include "OcctLabDoc.h"
#include "OcctLabView.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

// COcctLabView

IMPLEMENT_DYNCREATE(COcctLabView, CView)

BEGIN_MESSAGE_MAP(COcctLabView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_COMMAND(ID_MENU_VIEW_FRONT, OnMenuViewFront)
	ON_COMMAND(ID_MENU_VIEW_BACK, OnMenuViewBack)
	ON_COMMAND(ID_MENU_VIEW_LEFT, OnMenuViewLeft)
	ON_COMMAND(ID_MENU_VIEW_RIGHT, OnMenuViewRight)
	ON_COMMAND(ID_MENU_VIEW_TOP, OnMenuViewTop)
	ON_COMMAND(ID_MENU_VIEW_BOTTOM, OnMenuViewBottom)
	ON_COMMAND(ID_MENU_VIEW_RESTORE, OnMenuViewRestore)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_DESTROY()
	ON_COMMAND(ID_MENU_DUMP_VIEW, OnMenuDumpView)
	ON_COMMAND(IDM_MENU_SHAPE_STRUCT, OnMenuShapeStruct)
	ON_COMMAND(ID_MENU_FITALL, &COcctLabView::OnMenuFitall)
	ON_COMMAND(ID_MENU_DISP_SENEITIVE, &COcctLabView::OnMenuDispSeneitive)
END_MESSAGE_MAP()

// COcctLabView ����/����

COcctLabView::COcctLabView()
{
	// TODO: �ڴ˴���ӹ������
	this->m_pCommandMgr = NULL;
	this->m_pIModelTree = NULL;

	m_bPanning = FALSE;

	aDrawRect_ = new AIS_RubberBand(Quantity_NOC_WHITE, Aspect_TOL_SOLID, 1.0);

	dViewSize_ = 40.0;
	aBackGroundColor_.SetValues(0.1,0.0,0.1,Quantity_TOC_RGB);
}

COcctLabView::~COcctLabView()
{
}

BOOL COcctLabView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸� CREATESTRUCT cs ���޸Ĵ������
	// ��ʽ

	return CView::PreCreateWindow(cs);
}

void COcctLabView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	if(hOccView_.IsNull()){
		COcctLabDoc *pDoc = GetDocument();
		//��ʼ�����������
		pDoc->InitCommandMgr();
		//��ȡ����¼
		m_pCommandMgr = pDoc->GetCommandMgr();
		ASSERT(m_pCommandMgr);
		
		m_aisContext = GetDocument()->GetAISContext();
		
		InitV3dView();
	}

	//�������α�Ҫ�ĳ�ʼ����
	InitInputEdit();

	InitModelTree();

	//��ʼ��Ĭ�϶���ֻ���½��ĵ�����
	if(GetDocument()->IsNewDocument()){
		kcModel *pModel = GetDocument()->GetModel();
		pModel->CreateDefaultLayer();
		pModel->CreateDefaultBasePlane(hOccView_);
	}

	//��ʼ������
	CClientDC dc(this);
	HDC hDC = dc.GetSafeHdc();
	if(hDC){
		glb_InitArialCharText(hDC);
		glb_InitCourierCharText(hDC);
	}

	ShowStatusPrompt("");
}

void COcctLabView::OnDestroy()
{
	CView::OnDestroy();

	if(!hOccView_.IsNull())
		hOccView_->Remove();

	kcModel *pModel = GetDocument()->GetModel();
	ASSERT(pModel);
	pModel->SetModelTreeInterface(NULL);
}

//��ʼ��view
BOOL	COcctLabView::InitV3dView()
{
	kcModel *pModel = GetDocument()->GetModel();

	if(!CreateV3dView())
		return FALSE;
	
	pModel->SetActiveView(hOccView_);//�趨����
	//��������
	m_pCommandMgr->SetV3dView(hOccView_);

	kcSnapMngr *pSnapMgr = GetDocument()->GetSnapMgr();
	ASSERT(pSnapMgr != NULL);
	pSnapMgr->SetView(hOccView_);

	kcDocContext *pDocCtx = GetDocument()->GetDocContext();
	ASSERT(pDocCtx);
	pDocCtx->SetActiveView(this);
	pDocCtx->SetOcctView(hOccView_);

	// �趨������
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	if(pFrame){
		pDocCtx->SetTimerManager(pFrame->GetTimerMgr());
	}

	//������ʾ.
	UpdateTriedronDisplay();
	return TRUE;
}

bool COcctLabView::CreateV3dView()
{
	ASSERT(!m_aisContext.IsNull());
	Handle(V3d_Viewer) aViewer = GetDocument()->GetViewer();
	if(aViewer.IsNull())
		return false;

	hOccView_ = aViewer->CreateView();
	if(hOccView_.IsNull())
		return false;

	// create window
	Handle(Graphic3d_GraphicDriver) theGraphicDriver = ((COcctLabApp *)AfxGetApp())->GetGraphicDriver();
	Handle(WNT_Window) aWNTWindow = new WNT_Window(GetSafeHwnd());
	if(aWNTWindow.IsNull())
		return FALSE;
	hOccView_->SetWindow(aWNTWindow);
	if (!aWNTWindow->IsMapped()) aWNTWindow->Map();

	// set attrib
	hOccView_->SetSize(dViewSize_);
	hOccView_->SetBackgroundColor(aBackGroundColor_);
	hOccView_->SetLightOff();

	return true;
}


// ���±���ɫ
void COcctLabView::UpdateBackGroundColor()
{
	if(hOccView_.IsNull())
		return;

	Quantity_Color col1(1.0,1.0,1.0,Quantity_TOC_RGB);
	Quantity_Color col2(0.0,0.0,0.0,Quantity_TOC_RGB);
	//Quantity_Color col2(1.0,0.5,0.0,Quantity_TOC_RGB);
	//hOccView_->SetBgGradientColors(col1,col2,Aspect_GFM_HOR,Standard_True);
	hOccView_->SetBackgroundColor(col2);
}

// ��ʾ������
void	COcctLabView::UpdateTriedronDisplay()
{
	hOccView_->ZBufferTriedronSetup(Quantity_NOC_RED,Quantity_NOC_GREEN,
		Quantity_NOC_BLUE1,0.8,0.02,20);
	hOccView_->TriedronDisplay(Aspect_TOTP_LEFT_LOWER,Quantity_NOC_WHITE,0.2,V3d_ZBUFFER);
}

//
BOOL	COcctLabView::InitInputEdit()
{
	CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
	ASSERT(pMainFrm);
	CXInputEdit *pInputEdit = pMainFrm->GetInputEdit();
	ASSERT(pInputEdit);
	pInputEdit->Initialize();

	pInputEdit->PromptText("ָ��");

	return TRUE;
}

//
BOOL	COcctLabView::InitModelTree()
{
	CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
	ASSERT(pMainFrm);
	kuiModelTreeInterface *pModelTree = pMainFrm->GetModelTreeInterface();
	kcModel *pModel = GetDocument()->GetModel();
	if(pModel->GetModelTreeInterface() == NULL){
		pModel->SetModelTreeInterface(pModelTree);
	}
	m_pIModelTree = pModelTree;
	//
	kcDocContext *pDocCtx = GetDocument()->GetDocContext();
	ASSERT(pDocCtx);
	pDocCtx->SetModelTreeItf(pModelTree);

	return TRUE;
}

kcBasePlane* COcctLabView::GetCurrentBasePlane() const
{
	kcDocContext *pDocCtx = GetDocument()->GetDocContext();
	ASSERT(pDocCtx);
	return pDocCtx->GetCurrentBasePlane();
}

void COcctLabView::DrawRectangle(const Standard_Integer  MinX    ,
								const Standard_Integer  MinY    ,
								const Standard_Integer  MaxX ,
								const Standard_Integer  MaxY ,
								BOOL  bDraw , 
								const eLineStyle aLineStyle)
{
	static int m_DrawMode = LS_Default;

	if  (!m_Pen && aLineStyle == LS_Solid )
	{
		m_Pen = new CPen(PS_SOLID, 1, RGB(0,0,0)); 
		m_DrawMode = R2_MERGEPENNOT;
	}
	else if (!m_Pen && aLineStyle == LS_Dot )
	{
		m_Pen = new CPen(PS_DOT, 1, RGB(0,0,0));   
		m_DrawMode = R2_XORPEN;
	}
	else if (!m_Pen && aLineStyle == LS_ShortDash)
	{
		m_Pen = new CPen(PS_DASH, 1, RGB(255,0,0));	
		m_DrawMode = R2_XORPEN;
	}
	else if (!m_Pen && aLineStyle == LS_LongDash)
	{
		m_Pen = new CPen(PS_DASH, 1, RGB(0,0,0));	
		m_DrawMode = R2_NOTXORPEN;
	}
	else if (aLineStyle == LS_Default) 
	{ 
		m_Pen = NULL;	
		m_DrawMode = R2_MERGEPENNOT;
	}

	CPen* pOldPen = NULL;
	CClientDC clientDC(this);
	if (m_Pen) 
		pOldPen = clientDC.SelectObject(m_Pen);
	clientDC.SetROP2(m_DrawMode);

	static	int nStoredMinX, nStoredMaxX, nStoredMinY, nStoredMaxY;
	static	BOOL m_bVisible = FALSE;

	if (m_bVisible && !bDraw) // move or up  : erase at the old position 
	{
		clientDC.MoveTo(nStoredMinX,nStoredMinY); 
		clientDC.LineTo(nStoredMinX,nStoredMaxY); 
		clientDC.LineTo(nStoredMaxX,nStoredMaxY); 
		clientDC.LineTo(nStoredMaxX,nStoredMinY); 
		clientDC.LineTo(nStoredMinX,nStoredMinY);
		m_bVisible = FALSE;
	}

	nStoredMinX = min ( MinX, MaxX );
	nStoredMinY = min ( MinY, MaxY );
	nStoredMaxX = max ( MinX, MaxX );
	nStoredMaxY = max ( MinY, MaxY);

	if (bDraw) // move : draw
	{
		clientDC.MoveTo(nStoredMinX,nStoredMinY); 
		clientDC.LineTo(nStoredMinX,nStoredMaxY); 
		clientDC.LineTo(nStoredMaxX,nStoredMaxY); 
		clientDC.LineTo(nStoredMaxX,nStoredMinY); 
		clientDC.LineTo(nStoredMinX,nStoredMinY);
		m_bVisible = TRUE;
	}

	if (m_Pen) clientDC.SelectObject(pOldPen);
}

void COcctLabView::DrawRectangleEx (const Standard_Integer  MinX  ,
		const Standard_Integer  MinY  ,
		const Standard_Integer  MaxX  ,
		const Standard_Integer  MaxY  ,
		BOOL  bDraw  ,
		const eLineStyle aLineStyle)
{
	Handle(AIS_InteractiveContext) aCtx = GetDocument()->GetAISContext();
	if(!bDraw){
		aCtx->Remove(aDrawRect_,false);
		aCtx->CurrentViewer()->RedrawImmediate();
		return;
	}

	CRect rect;
	GetWindowRect(rect);
	//�趨����
	switch(aLineStyle){
	case LS_Solid:
		aDrawRect_->SetLineColor(Quantity_NOC_WHITE);
		aDrawRect_->SetLineType(Aspect_TOL_SOLID);
		break;
	case LS_Dot:
		aDrawRect_->SetLineColor(Quantity_NOC_WHITE);
		aDrawRect_->SetLineType(Aspect_TOL_DOT);
		break;
	case LS_ShortDash:
		aDrawRect_->SetLineColor(Quantity_Color(1.0,0,0,Quantity_TOC_RGB));
		aDrawRect_->SetLineType(Aspect_TOL_DASH);
		break;
	case LS_LongDash:
		aDrawRect_->SetLineColor(Quantity_NOC_WHITE);
		aDrawRect_->SetLineType(Aspect_TOL_DOTDASH);
		break;
	case LS_Default:
	default:
		aDrawRect_->SetLineColor(Quantity_NOC_WHITE);
		aDrawRect_->SetLineType(Aspect_TOL_SOLID);
		break;
	}
	aDrawRect_->SetRectangle(MinX,rect.Height()-MinY,MaxX,rect.Height() - MaxY);
	if(!aCtx->IsDisplayed(aDrawRect_)){
		aCtx->Display(aDrawRect_,false);
	}else{
		aCtx->Redisplay(aDrawRect_,false);
	}
	aCtx->CurrentViewer()->RedrawImmediate();
}

// COcctLabView ����

void COcctLabView::OnDraw(CDC* /*pDC*/)
{
	COcctLabDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if(!hOccView_.IsNull())
	{
		hOccView_->Redraw();
	}
}


// COcctLabView ��ӡ

BOOL COcctLabView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void COcctLabView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��ӡǰ��Ӷ���ĳ�ʼ��
}

void COcctLabView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��ӡ������������
}


// COcctLabView ���

#ifdef _DEBUG
void COcctLabView::AssertValid() const
{
	CView::AssertValid();
}

void COcctLabView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COcctLabDoc* COcctLabView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COcctLabDoc)));
	return (COcctLabDoc*)m_pDocument;
}
#endif //_DEBUG


// COcctLabView ��Ϣ�������

void COcctLabView::OnLButtonDblClk(UINT nFlags, CPoint point)
{


	CView::OnLButtonDblClk(nFlags, point);
}

void COcctLabView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_nStartX = m_nPreX = point.x;
	m_nStartY = m_nPreY = point.y;

	if(m_pCommandMgr && m_pCommandMgr->HasCommandRunning())
	{
		kuiMouseInput mouseInput(nFlags,point.x,point.y,hOccView_);
		m_pCommandMgr->OnLButtonDown(mouseInput);
	}

	CView::OnLButtonDown(nFlags, point);
}

void COcctLabView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(nFlags & MK_CONTROL)
	{//ƽ��
		//���¼��㲶׽�����Ļ����
		UpdateSnapScreenPoint();
	}
	else
	{
		// �������������,���������Ϣ,����Ĭ�ϴ���
		if(m_pCommandMgr && m_pCommandMgr->HasCommandRunning())
		{
			kuiMouseInput mouseInput(nFlags,point.x,point.y,hOccView_);
			m_pCommandMgr->OnLButtonUp(mouseInput);
		}
		else
		{
			if((point.x == this->m_nStartX) && 
				(point.y == this->m_nStartY))//�ͳ�ʼ����бȽϣ�û���ƶ�
			{
				if(nFlags & MK_SHIFT)
				{
					m_aisContext->SelectDetected(AIS_SelectionScheme_XOR);
				}
				else
				{
					m_aisContext->SelectDetected(AIS_SelectionScheme_Replace);
				}
			}
			else
			{//��ѡ
				//����ѡ���
				this->DrawRectangleEx(this->m_nStartX,this->m_nStartY,point.x,point.y,FALSE);

				if(nFlags & MK_SHIFT)
				{
					m_aisContext->SelectRectangle(Graphic3d_Vec2i(m_nStartX,m_nStartY),
						Graphic3d_Vec2i(point.x,point.y),hOccView_,
						AIS_SelectionScheme_XOR);
				}
				else
				{
					m_aisContext->SelectRectangle(Graphic3d_Vec2i(m_nStartX,m_nStartY),
						Graphic3d_Vec2i(point.x,point.y),hOccView_,
						AIS_SelectionScheme_Replace);
				}
			}

			kcModel *pModel = GetDocument()->GetModel();
			kiSelSet *pGlobalSelSet = GetDocument()->GetGlobalSelSet();
			pGlobalSelSet->GetSelected();
			pGlobalSelSet->Hilight(Quantity_NOC_BLUE1);
			//����tree��ʾ
			if(m_pIModelTree){
				m_pIModelTree->UnBoldAllItem(eEntityItem);
				for(pGlobalSelSet->InitSelected();pGlobalSelSet->MoreSelected();
					pGlobalSelSet->NextSelected())
				{
					kiSelEntity se = pGlobalSelSet->CurSelected();
					m_pIModelTree->BoldTreeItem(eEntityItem,se._pEntity->GetName());
				}
			}
			//
			m_aisContext->UpdateCurrentViewer();
		}
	}

	CView::OnLButtonUp(nFlags, point);
}

void COcctLabView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_aisContext.IsNull())
		return;

	//���û�н��㣬���ý���
	CWnd *pActiveWnd = this->GetActiveWindow();
	if(pActiveWnd)
	{
		if((pActiveWnd->GetSafeHwnd() != this->GetSafeHwnd()) &&
			(pActiveWnd->GetSafeHwnd() != NULL))
		{
			if((point.x != this->m_nPreX) ||
				(point.y != this->m_nPreY))//����ƶ�������ԭ���������ý���.
			{
				//DTRACE("\nOnMouseMove SetFocus...");
				SetFocus();
			}
		}
	}

	//�ȴ�����ת
	if(nFlags & MK_MBUTTON)
	{
		this->hOccView_->Rotation(point.x,point.y);
		this->hOccView_->Redraw();

		//��¼��ǰ��
		this->m_nPreX = point.x;
		this->m_nPreY = point.y;
	}
	else if(nFlags & MK_LBUTTON)
	{
		if(nFlags & MK_CONTROL)
		{
			
		}
		else if(nFlags & MK_SHIFT)
		{

		}
		else
		{
			//������,���������
			if(m_pCommandMgr && m_pCommandMgr->HasCommandRunning())
			{
				kuiMouseInput mouseInput(nFlags,point.x,point.y,hOccView_);
				m_pCommandMgr->OnMouseMove(mouseInput);
			}
			else
			{
				//��ѡ����

				//����ѡ���
				this->DrawRectangleEx(this->m_nStartX,this->m_nStartY,point.x,point.y,FALSE);

				if(nFlags & MK_SHIFT)
				{
					m_aisContext->SelectRectangle(Graphic3d_Vec2i(m_nStartX,m_nStartY),
						Graphic3d_Vec2i(point.x,point.y),hOccView_,
						AIS_SelectionScheme_XOR);
				}
				else
				{
					m_aisContext->SelectRectangle(Graphic3d_Vec2i(m_nStartX,m_nStartY),
						Graphic3d_Vec2i(point.x,point.y),hOccView_,
						AIS_SelectionScheme_Replace);
				}

				//��ʾѡ���ѡ
				this->DrawRectangleEx(this->m_nStartX,this->m_nStartY,point.x,point.y,TRUE);
			}
		}
	}
	else if(nFlags & MK_RBUTTON)
	{
		//����
		if(nFlags & MK_CONTROL)
		{
			this->hOccView_->Zoom(m_nPreX,m_nPreY,point.x,point.y);
			//��¼��ǰ��
			this->m_nPreX = point.x;
			this->m_nPreY = point.y;
		}
		else// if(nFlags & MK_SHIFT)//ƽ��
		{
			//����Ĭ������Ҽ���ƽ�ƣ�����̫С�ı䵼�µ��ƶ�
			this->hOccView_->Pan(point.x - this->m_nRPreX,
				this->m_nRPreY - point.y);

			//��¼��ǰ��
			this->m_nRPreX = point.x;
			this->m_nRPreY = point.y;
			m_bPanning = TRUE;
		}
	}
	else
	{
		//������,���������
		if(m_pCommandMgr && m_pCommandMgr->HasCommandRunning())
		{
			kuiMouseInput mouseInput(nFlags,point.x,point.y,hOccView_);
			m_pCommandMgr->OnMouseMove(mouseInput);
		}
		else
		{
			//����ƶ�����Ϣ
			m_aisContext->MoveTo(point.x,point.y,hOccView_,Standard_True);

			//��¼��ǰ��
			this->m_nPreX = point.x;
			this->m_nPreY = point.y;

			kcBasePlane *pWorkPlane = GetCurrentBasePlane();
			if(pWorkPlane){
				kPoint3 pnt;
				pWorkPlane->PixelToWorkPlane(point.x,point.y,pnt);
				CString szMsg;
				szMsg.Format("%.4f,%.4f,%.4f",pnt.x(),pnt.y(),pnt.z());
				ShowStatusPrompt(szMsg);
			}
		}
	}

	CView::OnMouseMove(nFlags, point);
}

void COcctLabView::OnMButtonDown(UINT nFlags, CPoint point)
{
	if(nFlags & MK_SHIFT)
	{

	}
	else
	{
		//hOccView_->SetDegenerateModeOn();
		this->hOccView_->StartRotation(point.x,point.y);
		this->m_nPreX = point.x;
		this->m_nPreY = point.y;
	}

	CView::OnMButtonDown(nFlags, point);
}

void COcctLabView::OnMButtonUp(UINT nFlags, CPoint point)
{
	//���¼��㲶׽�����Ļ����
	UpdateSnapScreenPoint();

	CView::OnMButtonUp(nFlags, point);
}

// Ŀǰ��ʵ��Ϊ�����������뷨�ǣ�
// ����OCCֻ�ܻ�����Ļ���Ľ������ţ���������Ҫ��һ����ȷ�ϣ�����ˣ���ϣ��ģ�;ֲ���ʾʱ
// �����ƶ�����Ļ���ģ�Ȼ���ٽ������š�
//
BOOL COcctLabView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	static CPoint s_prePnt;
	Standard_Real dViewWith,dViewHeight;
	this->hOccView_->Size(dViewWith,dViewHeight);

	double dCoef = 1.0;
	BOOL bZoom = TRUE;
	if(zDelta > 0.0){//�Ŵ�
		if(dViewWith < 0.01 || dViewHeight < 0.01)
			bZoom = FALSE;
		else
			dCoef = 5.0/4.0;
	}else{//��С
		if(dViewWith > 1000000.0 || dViewHeight > 1000000.0)
			bZoom = FALSE;
		else
			dCoef = 4.0/5.0;
	}

	//̫С��̫���ֹ����
	if(bZoom){
#ifdef _R_DEBUG
		//ʹ�������ڵ�ģ��λ���ƶ�����Ļ���Ľ�������
		// λ�øı��ˣ�����������
		if(abs(s_prePnt.x - pt.x) > 4 || abs(s_prePnt.y - pt.y) > 4){
			CRect rc;
			CPoint scp;
			int mx,my,icx,icy;

			scp = pt;
			this->ScreenToClient(&scp);//pt�ǻ�����Ļ������!!!
			GetClientRect(&rc);
			mx = (rc.left + rc.right) / 2;
			my = (rc.top + rc.bottom) / 2;
			TRACE("\n x %d,y %d,mx %d,my %d",scp.x,scp.y,mx,my);
			icx = mx - scp.x;
			icy = scp.y - my;//ע��ƽ������ϵ��ͶӰƽ������ϵ��Y���෴
			TRACE("\n icx %d,icy %d",icx,icy);
			double len = sqrt(icx * icx + icy * icy);
			double mrat = 100 / len;
			if(mrat > 1.0) mrat = 1.0;
			icx = (int)(icx * mrat);
			icy = (int)(icy * mrat);
								
			hOccView_->Pan(icx,icy);
		}
#endif
		this->hOccView_->SetZoom(dCoef,Standard_True);		
		this->hOccView_->Redraw();

		//��ͼ�任����Ҫ����snap��
//		UpdateSnapPoints();
	}

	s_prePnt = pt;

	//���¼��㲶׽�����Ļ����
	UpdateSnapScreenPoint();

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void COcctLabView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// ����ƽ�ƻ�����
	if((nFlags & MK_CONTROL) || (nFlags & MK_SHIFT)){
		m_nPreX = point.x;
		m_nPreY = point.y;
	}

	//Ϊ��ƽ��
	m_nRPreX = point.x;
	m_nRPreY = point.y;
	m_bPanning = FALSE;

	// ��Ӧ��������д���
	if(m_pCommandMgr){
		kuiMouseInput mouseInput(nFlags,point.x,point.y,hOccView_);
		m_pCommandMgr->OnRButtonDown(mouseInput);
	}

	CView::OnRButtonDown(nFlags, point);
}

void COcctLabView::OnRButtonUp(UINT nFlags, CPoint point)
{
	//ƽ��״̬�£����������������
	if(m_pCommandMgr && !m_bPanning){
		kuiMouseInput mouseInput(nFlags,point.x,point.y,hOccView_);
		m_pCommandMgr->OnRButtonUp(mouseInput);
	}

	m_bPanning = FALSE;
	m_nRPreX = m_nRPreY = 0;

	CView::OnRButtonUp(nFlags, point);
}


BOOL COcctLabView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}

void COcctLabView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (!hOccView_.IsNull())
	{
		hOccView_->MustBeResized();

		//���¼��㲶׽�����Ļ����
		UpdateSnapScreenPoint();
	}
}

//
void	COcctLabView::UpdateSnapScreenPoint()
{
	kcSnapMngr *pSnapMgr = GetDocument()->GetSnapMgr();
	if(pSnapMgr)
	{
		pSnapMgr->CalcScreenPoint();
	}
}

void	COcctLabView::ShowStatusPrompt(LPCTSTR lpszPrompt)
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	if(!pFrame){
		COcctLabApp *pApp = (COcctLabApp *)AfxGetApp();
		pFrame = (CMainFrame *)pApp->GetMainWnd();
	}
	if(pFrame){
		pFrame->ShowPrompt(lpszPrompt);
	}
}

//////////////////////////////////////////////////////////////////////////
// ��ͼ���.
//
void  COcctLabView::OnMenuViewFront()
{
	if(!hOccView_.IsNull()){
		hOccView_->SetProj(V3d_Yneg);

		//���¼��㲶׽�����Ļ����
		UpdateSnapScreenPoint();
	}
}

void COcctLabView::OnMenuViewBack()
{
	if(!hOccView_.IsNull()){
		hOccView_->SetProj(V3d_Ypos);

		//���¼��㲶׽�����Ļ����
		UpdateSnapScreenPoint();
	}
}

void COcctLabView::OnMenuViewLeft()
{
	if(!hOccView_.IsNull()){
		hOccView_->SetProj(V3d_Xneg);

		//���¼��㲶׽�����Ļ����
		UpdateSnapScreenPoint();
	}
}

void COcctLabView::OnMenuViewRight()
{
	if(!hOccView_.IsNull()){
		hOccView_->SetProj(V3d_Xpos);

		//���¼��㲶׽�����Ļ����
		UpdateSnapScreenPoint();
	}
}

void COcctLabView::OnMenuViewTop()
{
	if(!hOccView_.IsNull()){
		hOccView_->SetProj(V3d_Zpos);

		//���¼��㲶׽�����Ļ����
		UpdateSnapScreenPoint();
	}
}

void COcctLabView::OnMenuViewBottom()
{
	if(!hOccView_.IsNull()){
		hOccView_->SetProj(V3d_Zneg);

		//���¼��㲶׽�����Ļ����
		UpdateSnapScreenPoint();
	}
}

// Ĭ��XYƽ�棬Ҳ�ǻ�׼��
void COcctLabView::OnMenuViewRestore()
{
	if(!hOccView_.IsNull()){
		kcModel *pModel = GetDocument()->GetModel();
		ASSERT(pModel);
		kcBasePlane *pBasePlane = pModel->GetCurrentBasePlane();
		ASSERT(pBasePlane);
		pBasePlane->SetDefaultView();

		//���¼��㲶׽�����Ļ����
		UpdateSnapScreenPoint();
	}
}

void COcctLabView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//
	if(m_pCommandMgr != NULL){
		kuiKeyInput keyInput(nChar,nRepCnt,nFlags);
		m_pCommandMgr->OnKeyDown(keyInput);
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void COcctLabView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(nChar == VK_ESCAPE)
	{
		if(m_pCommandMgr && m_pCommandMgr->HasCommandRunning())
		{
			m_pCommandMgr->EndCommand(KSTA_CANCEL);
		}
	}
	//
	if(m_pCommandMgr != NULL){
		//ִ��ɾ������
		if(nChar == VK_DELETE && !m_pCommandMgr->HasCommandRunning()){
			m_pCommandMgr->ExecCommand(ID_EDIT_DELETE);
		}

		kuiKeyInput keyInput(nChar,nRepCnt,nFlags);
		m_pCommandMgr->OnKeyUp(keyInput);
	}

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

// �����ǰview�Ĵ�����Ϣ��
void COcctLabView::OnMenuDumpView()
{
	if(hOccView_.IsNull())
		return;

	TCHAR szModulePath[512];
	DWORD dwSize,ix;
	dwSize = ::GetModuleFileName(NULL,szModulePath,512);
	ix = dwSize - 1;
	while(szModulePath[ix] != '\\')
		ix --;
	szModulePath[ix + 1] = '\0';
	CString szOutFile = szModulePath;
	szOutFile = szOutFile + "dumpview.txt";
	//FILE *fout = freopen(szOutFile,"w",stdout);
	//if(fout)
	//{
//		Handle(Visual3d_View) aView = hOccView_->View();
//
//		printf("\n ****************dump view********************\n");
////		aView->Exploration();
//
//		printf("\n ****************dump displayed structures********************\n");
//		//ÿ��stru��dump
//		Graphic3d_MapOfStructure SG;
//		aView->DisplayedStructures(SG);
//
//		Handle(Graphic3d_Structure) G;
//		for(Graphic3d_MapIteratorOfMapOfStructure It(SG); It.More();It.Next())
//		{
//			G = It.Key();
//			printf("\n ****************dump structures********************\n");
////			G->Exploration();
//		}


		//fclose(fout);

		//::ShellExecute(NULL,"open",szOutFile,NULL,NULL,SW_SHOWNORMAL);
	//}
}

void COcctLabView::OnMenuShapeStruct()
{
	if(!m_aisContext.IsNull())
	{
		//m_aisContext->Select();
		m_aisContext->InitSelected();
		if(m_aisContext->MoreSelected())
		{
			Handle(AIS_InteractiveObject) aObj = m_aisContext->SelectedInteractive();
			Handle(AIS_Shape) aShapeObj = Handle(AIS_Shape)::DownCast(aObj);
			TopoDS_Shape aShape = aShapeObj->Shape();
			if(!aShape.IsNull())
			{
				ShowShapeStructDialog(aShape);
			}
		}
	}
}


void COcctLabView::OnMenuFitall()
{
	// TODO: �ڴ���������������
	if(!hOccView_.IsNull()){
		hOccView_->FitAll();
		hOccView_->ZFitAll();
	}
}


void COcctLabView::OnMenuDispSeneitive()
{
	static bool s_bDispSensitive = false;
	if(m_aisContext.IsNull())
		return;

	if(!s_bDispSensitive){
		m_aisContext->DisplayActiveSensitive(hOccView_);
	}else{
		m_aisContext->ClearActiveSensitive(hOccView_);
	}
	s_bDispSensitive = !s_bDispSensitive;
}


void COcctLabView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: �ڴ����ר�ô����/����û���
	TRACE("\n###---------------OnActivateView-------.\n");

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
