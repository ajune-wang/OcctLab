#include "stdafx.h"
#include <BRepBndLib.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepAdaptor_Curve.hxx>
#include "kcEntity.h"
#include "QxBRepLib.h"
#include "QxBaseUtils.h"
#include "kiInputEntityTool.h"
#include "kcPropertyDlg.h"
#include "kcmShapeProperty.h"

static void uxAddBoundBoxInfo(const TopoDS_Shape &aShape,CTreeCtrl &propTree,HTREEITEM hpItem);

//���Դ�����
class txShapePropHandler : public kcPropHandler{
public:
	txShapePropHandler(const Handle(AIS_InteractiveObject)& aCtxObj,const TopoDS_Shape &aShape);
	~txShapePropHandler();

	//��Ҫ�ӿڣ�����������Ϣ
	virtual int SetProperty(CTreeCtrl &propTree);

protected:
	void			ShowBaseProp(CTreeCtrl &propTree);
	void			ShowAISContextProp(CTreeCtrl &propTree);
	void			ShowToleranceProp(CTreeCtrl &propTree);
	void			ShowTopolProp(CTreeCtrl &propTree);
	void			ShowGeomProp(CTreeCtrl &propTree);

protected:
	Handle(AIS_InteractiveObject) aCtxObject_;
	TopoDS_Shape	aShape_;
};


kcmShapeProperty::kcmShapeProperty(void)
{
	m_strName = "��������";
	m_strAlias = "ShapeProp";
	m_strDesc = "��ʾ���������";

	m_pInputEntityTool = NULL;
}

kcmShapeProperty::~kcmShapeProperty(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL  kcmShapeProperty::CanFinish()//�����Ƿ�������
{
	if(m_pInputEntityTool && m_pInputEntityTool->GetSelCount() == 1)
		return TRUE;

	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int  kcmShapeProperty::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntityTool)
	{
		if(m_pInputEntityTool->IsInputDone())
		{
			DoShowProperty();
			Done();
			return KSTA_DONE;
		}
		else
		{
			EndCommand(KSTA_CANCEL);
			return KSTA_CANCEL;
		}
	}

	return KSTA_CONTINUE;
}

int  kcmShapeProperty::OnExecute()
{
	if(m_pInputEntityTool){
		m_pInputEntityTool->AddInitSelected(true);
		if(m_pInputEntityTool->IsInputDone()){
			DoShowProperty();
			return KSTA_DONE;
		}
	}
	return KSTA_CONTINUE;
}

int  kcmShapeProperty::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL  kcmShapeProperty::CreateInputTools()
{
	m_pInputEntityTool = new kiInputEntityTool(this,"ѡ�����");
	m_pInputEntityTool->SetOption(KCT_ENT_ALL,true);
	m_pInputEntityTool->SetNaturalMode(true);
	

	return TRUE;
}

BOOL  kcmShapeProperty::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntityTool);
	return TRUE;
}

BOOL  kcmShapeProperty::InitInputToolQueue()
{
	if(m_pInputEntityTool){
		AddInputTool(m_pInputEntityTool);
		return TRUE;
	}
	return FALSE;
}

BOOL  kcmShapeProperty::DoShowProperty()
{
	if(m_pInputEntityTool->GetSelCount() == 1){
		kiSelEntity se = m_pInputEntityTool->GetFirstSelect();

		TColStd_ListOfInteger aML;
		TColStd_ListIteratorOfListOfInteger aItML;
		GetAISContext()->ActivatedModes(se.AISObject(),aML);
		aItML.Initialize(aML);
		TRACE("\n select model list: ");
		for(;aItML.More();aItML.Next()){
			TRACE("%d,",aItML.Value());
		}
		TRACE("\n");
		
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			txShapePropHandler propHandler(se.AISObject(),se.Entity()->GetShape());
			kcPropertyDlg dlg(&propHandler);
			dlg.DoModal();
		}
	}

	return TRUE;
}

//////////////////////////////////////////////

txShapePropHandler::txShapePropHandler(const Handle(AIS_InteractiveObject)& aCtxObj,const TopoDS_Shape &aShape)
	:aCtxObject_(aCtxObj),aShape_(aShape)
{
}

txShapePropHandler::~txShapePropHandler()
{
}

//��Ҫ�ӿڣ�����������Ϣ
int  txShapePropHandler::SetProperty(CTreeCtrl &propTree)
{
	//��ʾ�������
	if(aCtxObject_.IsNull() || aShape_.IsNull())
		return 0;

	ShowBaseProp(propTree);

	//AIS_CTX�������
	ShowAISContextProp(propTree);
	
	//�ݲ���Ϣ
	ShowToleranceProp(propTree);

	//��������
	ShowTopolProp(propTree);

	//��������

	return 1;
}

//�������ԣ����ġ���Χ�е�.
void  txShapePropHandler::ShowBaseProp(CTreeCtrl &propTree)
{
	CString sText;

	HTREEITEM hBasItem = propTree.InsertItem("��������",TVI_ROOT);

	//��Χ�������Ϣ
	uxAddBoundBoxInfo(aShape_,propTree,hBasItem);
	
	TopAbs_ShapeEnum shptype = aShape_.ShapeType();
	if(shptype == TopAbs_EDGE || shptype == TopAbs_WIRE){
		GProp_GProps LProps;
		BRepGProp::LinearProperties(aShape_,LProps);
		double length = LProps.Mass();
		sText.Format("���� : %.4f",length);
		propTree.InsertItem(sText,hBasItem);
	}
	propTree.Expand(hBasItem,TVE_EXPAND);
}

// ��ʾAis_Context�������
void  txShapePropHandler::ShowAISContextProp(CTreeCtrl &propTree)
{
	CString sText;
	
	Handle(AIS_InteractiveContext) aCtx = aCtxObject_->GetContext();
	//HTREEITEM hVisItem = propTree.InsertItem("��ʾ����",TVI_ROOT);
}

//��ʾ�ݲ���Ϣ
void  txShapePropHandler::ShowToleranceProp(CTreeCtrl &propTree)
{
	CString sText;
	HTREEITEM hTolItem = propTree.InsertItem("�ݲ�����",TVI_ROOT);
	double maxTol = 0.0;
	TopAbs_ShapeEnum shptype = aShape_.ShapeType();

	//Vertex 
	maxTol = BRep_Tool::MaxTolerance(aShape_,TopAbs_VERTEX);
	sText.Format("Max Vertex Tolerance : %.10f",maxTol);
	propTree.InsertItem(sText,hTolItem);

	//Edge
	if(shptype <= TopAbs_EDGE){	
		maxTol = BRep_Tool::MaxTolerance(aShape_,TopAbs_EDGE);
		sText.Format("Max Edge   Tolerance : %.10f",maxTol);
		propTree.InsertItem(sText,hTolItem);
	}

	//Face
	if(shptype <= TopAbs_FACE){	
		maxTol = BRep_Tool::MaxTolerance(aShape_,TopAbs_FACE);
		sText.Format("Max Face   Tolerance : %.10f",maxTol);
		propTree.InsertItem(sText,hTolItem);
	}
	propTree.Expand(hTolItem,TVE_EXPAND);
}

void  txShapePropHandler::ShowTopolProp(CTreeCtrl &propTree)
{

}

void  txShapePropHandler::ShowGeomProp(CTreeCtrl &propTree)
{
}

//////////////////////////////////////////////////////////////////
// Edge�������Ե���ʾ

//������
class txEdgePropHandler : public kcPropHandler{
public:
	txEdgePropHandler(const Handle(AIS_InteractiveObject)& aCtxObj,const TopoDS_Edge &aEdge,const TopoDS_Shape &aS);
	~txEdgePropHandler();

	//��Ҫ�ӿڣ�����������Ϣ
	virtual int SetProperty(CTreeCtrl &propTree);

protected:
	void			GetAdjFaces();
	//��������face�ı߽��3D���ߵ�������
	void			CalcMaxDistOf2DAnd3D(const TopoDS_Face &aFace,double &t,double &maxDist);

protected:
	Handle(AIS_InteractiveObject) aCtxObject_;
	TopoDS_Edge	aEdge_;
	TopoDS_Shape	aShape_;

	TopoDS_Face	aAdFace1_,aAdFace2_;//edge���ڵ�����face
	TopTools_SequenceOfShape aAdFaces_;//�ڽӵĶ��face
	int			nbAdFaces_;//�ڽ���ĸ���
};

/////////////////////////////////////////////////////
//
kcmEdgeProperty::kcmEdgeProperty(void)
{
	m_strName = "������";
	m_strAlias = "EdgeProp";
	m_strDesc = "��ʾ�ߵ�����";

	pInputEntityTool_ = NULL;
}

kcmEdgeProperty::~kcmEdgeProperty(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL  kcmEdgeProperty::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int  kcmEdgeProperty::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == pInputEntityTool_){
		if(pInputEntityTool_->IsInputDone()){
			
			DoShowProperty();
			Done();
			return KSTA_DONE;
		}else{
			EndCommand(KSTA_CANCEL);
			return KSTA_CANCEL;
		}
	}

	return KSTA_CONTINUE;
}

// ������Ҫ�����빤��
BOOL  kcmEdgeProperty::CreateInputTools()
{
	pInputEntityTool_ = new kiInputEntityTool(this,"ѡ��һ����");
	pInputEntityTool_->SetOption(KCT_ENT_EDGE,true);
	pInputEntityTool_->SetNaturalMode(false);
	pInputEntityTool_->AddInitSelected(true);//��ʰȡ�����Զ�����

	return TRUE;
}

BOOL  kcmEdgeProperty::DestroyInputTools()
{
	KC_SAFE_DELETE(pInputEntityTool_);

	return TRUE;
}

BOOL  kcmEdgeProperty::InitInputToolQueue()
{
	if(pInputEntityTool_){
		AddInputTool(pInputEntityTool_);
	}

	return TRUE;
}

BOOL  kcmEdgeProperty::DoShowProperty()
{
	TopoDS_Shape aS,aPS;
	kiSelEntity se = pInputEntityTool_->GetFirstSelect();
	{
		aCtxObject_ = se.AISObject();
		if(se.IsLocalShape()){
			aPS = aShape_ = se.Entity()->GetShape();
			aS = se.LocalShape();
		}else{
			aS = se.EntityShape();
		}

		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			txEdgePropHandler propHandler(aCtxObject_,TopoDS::Edge(aS),aPS);
			kcPropertyDlg dlg(&propHandler);
			dlg.DoModal();
		}
	}


	return TRUE;
}

/////////////////////////////////////////////////////
//
txEdgePropHandler::txEdgePropHandler(const Handle(AIS_InteractiveObject)& aCtxObj,const TopoDS_Edge &aEdge,const TopoDS_Shape &aS)
	:aCtxObject_(aCtxObj),aEdge_(aEdge),aShape_(aS)
{
	nbAdFaces_ = 0;
}

txEdgePropHandler::~txEdgePropHandler()
{
}

void  txEdgePropHandler::GetAdjFaces()
{
	if(aShape_.IsNull())
		return;

	TopTools_IndexedDataMapOfShapeListOfShape mapEF;
	TopExp::MapShapesAndAncestors(aShape_,TopAbs_EDGE,TopAbs_FACE,mapEF);
	int idx = mapEF.FindIndex(aEdge_);
	if(idx != 0){
		const TopTools_ListOfShape& aFaces = mapEF(idx);
		nbAdFaces_ = aFaces.Size();

		TopTools_ListIteratorOfListOfShape ite(aFaces);
		if(aFaces.Size() == 2){
			aAdFace1_ = TopoDS::Face(ite.Value());
			ite.Next();
			aAdFace2_ = TopoDS::Face(ite.Value());
		}else if(aFaces.Size() == 1){
			aAdFace1_ = TopoDS::Face(ite.Value());
			aAdFace2_ = aAdFace1_;
		}else{
			aAdFaces_.Clear();
			for(;ite.More();ite.Next()){
				aAdFaces_.Append(ite.Value());
			}
		}
	}
}

//��������face�ı߽��3D���ߵ�������
void  txEdgePropHandler::CalcMaxDistOf2DAnd3D(const TopoDS_Face &aFace,double &par,double &maxDist)
{
	Handle(Geom_Surface) aSurf;
	Handle(Geom_Curve) aCrv3d;
	Handle(Geom2d_Curve) aParCrv;
	double dF,dL,dFp,dLp;

	aSurf = BRep_Tool::Surface(aFace);
	aCrv3d = BRep_Tool::Curve(aEdge_,dF,dL);
	aParCrv = BRep_Tool::CurveOnSurface(aEdge_,aFace,dFp,dLp);
	if(aSurf.IsNull() || aCrv3d.IsNull() || aParCrv.IsNull())
		return;
	
	GeomAPI_ProjectPointOnCurve ppOnCrv;
	int ix,nCnt = 128;
	gp_Pnt2d uv;
	gp_Pnt pnt;
	double t,dstp = (dLp - dFp) / nCnt;
	double ldd = 0.0;

	par = -1.0;
	maxDist = 0.0;
	ppOnCrv.Init(aCrv3d,dF,dL);
	for(ix = 0;ix <= nCnt;ix ++){
		t = dFp + ix * dstp;
		aParCrv->D0(t,uv);
		//
		aSurf->D0(uv.X(),uv.Y(),pnt);
		//
		ppOnCrv.Perform(pnt);
		if(ppOnCrv.NbPoints() > 0){
			ldd = ppOnCrv.LowerDistance();
			if(ldd > maxDist){
				maxDist = ldd;
				par = ppOnCrv.LowerDistanceParameter();
			}
		}
	}
}

static void uxAddFaceSurfType(int idx,const TopoDS_Face &aFace,CTreeCtrl &propTree,HTREEITEM hTopItem)
{
	Handle(Geom_Surface) aSurf;
	TopLoc_Location aLoc;
	CString szText;
		
	aSurf = BRep_Tool::Surface(aFace,aLoc);
	Handle(Standard_Type) aType = aSurf->DynamicType();
	szText.Format("Face %d surface type : %s",idx,aType->Name());
	propTree.InsertItem(szText,hTopItem);
}

//��Ҫ�ӿڣ�����������Ϣ
int  txEdgePropHandler::SetProperty(CTreeCtrl &propTree)
{
	CString sText;
	if(aCtxObject_.IsNull() || aEdge_.IsNull())
		return 0;

	GetAdjFaces();

	HTREEITEM hBasItem = propTree.InsertItem("��������",TVI_ROOT);

	sText.Format("���� : %s",kxCurveType(aEdge_));
	propTree.InsertItem(sText,hBasItem);
	//����
	double crvlen = QxBRepLib::EdgeLengthEx(aEdge_);
	sText.Format("���� : %.4f",crvlen);
	propTree.InsertItem(sText,hBasItem);

	//�׵�,�����Ϸ���
	TopoDS_Vertex V1 = TopExp::FirstVertex(aEdge_,Standard_True);
	gp_Pnt bp = BRep_Tool::Pnt(V1);

	sText.Format("��� : %.4f,%.4f,%.4f",bp.X(),bp.Y(),bp.Z());
	propTree.InsertItem(sText,hBasItem);
	
	//ĩ��
	TopoDS_Vertex V2 = TopExp::LastVertex(aEdge_,Standard_True);
	gp_Pnt ep = BRep_Tool::Pnt(V2);
	sText.Format("ĩ�� : %.4f,%.4f,%.4f",ep.X(),ep.Y(),ep.Z());
	propTree.InsertItem(sText,hBasItem);
	
	double tol = BRep_Tool::Tolerance(V1);
	sText.Format("����ݲ� : %.8f",tol);
	propTree.InsertItem(sText,hBasItem);

	tol = BRep_Tool::Tolerance(V2);
	sText.Format("����ݲ� : %.8f",tol);
	propTree.InsertItem(sText,hBasItem);
		
	//�ݲ�
	tol = BRep_Tool::Tolerance(aEdge_);
	sText.Format("�ݲ� : %.8f",tol);
	propTree.InsertItem(sText,hBasItem);

	//��Χ����Ϣ
	uxAddBoundBoxInfo(aEdge_,propTree,hBasItem);

	propTree.Expand(hBasItem,TVE_EXPAND);

	HTREEITEM hAdvItem = propTree.InsertItem("�߼�����",TVI_ROOT);

	//������
	if(!aAdFace1_.IsNull()){
		if(BRep_Tool::HasContinuity(aEdge_,aAdFace1_,aAdFace2_)){
			GeomAbs_Shape cont = BRep_Tool::Continuity(aEdge_,aAdFace1_,aAdFace2_);
			sText.Format("Continuity : %s",kxContinuity(cont));
			propTree.InsertItem(sText,hAdvItem);
		}
	}

	Standard_Boolean bv = BRep_Tool::SameParameter(aEdge_);
	if(bv){
		propTree.InsertItem("SameParameter : true",hAdvItem);
	}else{
		propTree.InsertItem("SameParameter : false",hAdvItem);
	}

	bv = BRep_Tool::SameRange(aEdge_);
	if(bv){
		propTree.InsertItem("SameRange : true",hAdvItem);
	}else{
		propTree.InsertItem("SameRange : false",hAdvItem);
	}

	double dF,dL;

	BRep_Tool::Range(aEdge_,dF,dL);
	sText.Format("3d range : %f,%f",dF,dL);
	propTree.InsertItem(sText,hAdvItem);

	if(!aAdFace1_.IsNull()){
		BRep_Tool::Range(aEdge_,aAdFace1_,dF,dL);
		sText.Format("2d face1 range : %f,%f",dF,dL);
		propTree.InsertItem(sText,hAdvItem);
	}
	if(!aAdFace2_.IsNull() && !aAdFace1_.IsSame(aAdFace2_)){
		BRep_Tool::Range(aEdge_,aAdFace2_,dF,dL);
		sText.Format("2d face2 range : %f,%f",dF,dL);
		propTree.InsertItem(sText,hAdvItem);
	}

	//����������������
	if(!aShape_.IsNull()){
		//�ڽ������
		sText.Format("adjust faces num : %d",nbAdFaces_);
		propTree.InsertItem(sText,hAdvItem);

		if(!aAdFace1_.IsNull()){
			uxAddFaceSurfType(1,aAdFace1_,propTree,hAdvItem);
			uxAddFaceSurfType(2,aAdFace2_,propTree,hAdvItem);

			//����߽��3D���ߵ�������
			if(aAdFace1_ != aAdFace2_){
				double t,maxDist = 0.0;
				CalcMaxDistOf2DAnd3D(aAdFace1_,t,maxDist);
				sText.Format("Face 1 bnd and 3D curve max dist : %f,at %f",maxDist,t);
				propTree.InsertItem(sText,hAdvItem);

				CalcMaxDistOf2DAnd3D(aAdFace2_,t,maxDist);
				sText.Format("Face 2 bnd and 3D curve max dist : %f,at %f",maxDist,t);
				propTree.InsertItem(sText,hAdvItem);
			}
		}
	}

	propTree.Expand(hAdvItem,TVE_EXPAND);

	return 1;
}


//////////////////////////////////////////////////////////////////
//
//������
class txFacePropHandler : public kcPropHandler{
public:
	txFacePropHandler(const Handle(AIS_InteractiveObject)& aCtxObj,const TopoDS_Face &aFace);
	~txFacePropHandler();

	//��Ҫ�ӿڣ�����������Ϣ
	virtual int SetProperty(CTreeCtrl &propTree);

protected:
	Handle(AIS_InteractiveObject) aCtxObject_;
	TopoDS_Face	aFace_;
};

/////////////////////////////////////////////////////
//
kcmFaceProperty::kcmFaceProperty(void)
{
	m_strName = "������";
	m_strAlias = "FaceProp";
	m_strDesc = "��ʾ�������";

	pInputEntityTool_ = NULL;
//	pInputFaceTool_ = NULL;

	bLocalFace_ = false;
}

kcmFaceProperty::~kcmFaceProperty(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL  kcmFaceProperty::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int  kcmFaceProperty::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == pInputEntityTool_){
		if(pInputEntityTool_->IsInputDone()){
			DoShowProperty();
			Done();

			return KSTA_DONE;
		}else{
			EndCommand(KSTA_CANCEL);
			return KSTA_CANCEL;
		}
	}

	return KSTA_CONTINUE;
}

int  kcmFaceProperty::OnExecute()
{
	bLocalFace_ = false;

	return KSTA_CONTINUE;
}

// ������Ҫ�����빤��
BOOL  kcmFaceProperty::CreateInputTools()
{
	pInputEntityTool_ = new kiInputEntityTool(this,"ѡ�������ʵ��");
	pInputEntityTool_->SetOption(KCT_ENT_FACE,true);
	pInputEntityTool_->SetNaturalMode(false);
	pInputEntityTool_->AddInitSelected(true);//��ʰȡ�����Զ�����

	//pInputFaceTool_ = new kiInputLocalShape(this,"ʰȡ��");
	//pInputFaceTool_->SetSingleSelect();//��ѡ

	return TRUE;
}

BOOL  kcmFaceProperty::DestroyInputTools()
{
	KC_SAFE_DELETE(pInputEntityTool_);
//	KC_SAFE_DELETE(pInputFaceTool_);

	return TRUE;
}

BOOL  kcmFaceProperty::InitInputToolQueue()
{
	if(pInputEntityTool_){
		AddInputTool(pInputEntityTool_);
	}
	//if(pInputFaceTool_){
	//	AddInputTool(pInputFaceTool_);
	//}
	return TRUE;
}

BOOL  kcmFaceProperty::DoShowProperty()
{
	if(pInputEntityTool_->GetSelCount() > 0){
		kiSelEntity se = pInputEntityTool_->GetFirstSelect();
		TopoDS_Shape aF = se.SelectShape();

		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			txFacePropHandler propHandler(se.AISObject(),TopoDS::Face(aF));
			kcPropertyDlg dlg(&propHandler);
			dlg.DoModal();
		}

		pInputEntityTool_->ClearSel();
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////
//
txFacePropHandler::txFacePropHandler(const Handle(AIS_InteractiveObject)& aCtxObj,const TopoDS_Face &aFace)
	:aCtxObject_(aCtxObj),aFace_(aFace)
{
}

txFacePropHandler::~txFacePropHandler()
{
}

//��Ҫ�ӿڣ�����������Ϣ
int  txFacePropHandler::SetProperty(CTreeCtrl &propTree)
{
	CString sText;
	if(aCtxObject_.IsNull() || aFace_.IsNull())
		return 0;

	HTREEITEM hBasItem = propTree.InsertItem("��������",TVI_ROOT);

	const char *sType = kxSurfaceType(aFace_);
	if(sType){
		sText.Format("���� : %s",sType);
		propTree.InsertItem(sText,hBasItem);
	}

	TopLoc_Location L;
	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace_,L);
	if(!aSurf.IsNull()){
		double U1,U2,V1,V2;
		aSurf->Bounds(U1,U2,V1,V2);

		sText.Format("U Range : %f - %f",U1,U2);
		propTree.InsertItem(sText,hBasItem);
		sText.Format("V Range : %f - %f",V1,V2);
		propTree.InsertItem(sText,hBasItem);
	}

	//��Χ����Ϣ
	uxAddBoundBoxInfo(aFace_,propTree,hBasItem);



	//Vertex tol
	double maxTol = BRep_Tool::MaxTolerance(aFace_,TopAbs_VERTEX);
	sText.Format("Max Vertex Tolerance : %.10f",maxTol);
	propTree.InsertItem(sText,hBasItem);

	//Edge tol
	maxTol = BRep_Tool::MaxTolerance(aFace_,TopAbs_EDGE);
	sText.Format("Max Edge   Tolerance : %.10f",maxTol);
	propTree.InsertItem(sText,hBasItem);

	propTree.Expand(hBasItem,TVE_EXPAND);

	return 1;
}


//////////////////////////////////////////////////////////////////
// ��������

//��Ӱ�Χ����Ϣ
static void uxAddBoundBoxInfo(const TopoDS_Shape &aShape,CTreeCtrl &propTree,HTREEITEM hpItem)
{
	CString sText;
	Bnd_Box aBox;
	double x1,x2,y1,y2,z1,z2;

	BRepBndLib::Add(aShape,aBox);
	aBox.Get(x1,y1,z1,x2,y2,z2);
	//��Χ������
	sText.Format("��Χ������ : %.4f,%.4f,%.4f",(x1+x2)/2,(y1+y2)/2,(z1+z2)/2);
	propTree.InsertItem(sText,hpItem);

	//��Χ�д�С
	sText.Format("��Χ�д�С : %.4f,%.4f,%.4f",x2-x1,y2-y1,z2-z1);
	propTree.InsertItem(sText,hpItem);
}