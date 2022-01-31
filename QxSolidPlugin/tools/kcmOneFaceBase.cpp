#include "stdafx.h"
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <ShapeAnalysis.hxx>
#include <GeomLib.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include "kcEntity.h"
#include "kcEntityFactory.h"
#include "kcModel.h"
#include "kcDispArrow.h"
#include "kiInputEntityTool.h"
#include "kiInputValueTool.h"
#include "QxCurveLib.h"
#include "QxSurfLib.h"
#include "kcSTepExport.h"
#include "kcmOneFaceBase.h"


kcmOneFaceBase::kcmOneFaceBase(void)
{
	m_pSelFaceTool = NULL;
}


kcmOneFaceBase::~kcmOneFaceBase(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmOneFaceBase::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmOneFaceBase::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pSelFaceTool){
		if(m_pSelFaceTool->IsInputDone()){
			int nbSel = m_pSelFaceTool->GetSelCount();
			if(nbSel > 0){
				kiSelEntity se = m_pSelFaceTool->GetFirstSelect();
				TopoDS_Shape aShape = se.SelectShape();
				ASSERT(!aShape.IsNull() && aShape.ShapeType() == TopAbs_FACE);
				m_aFace = TopoDS::Face(aShape);

				OnSelOneFace();
				m_pSelFaceTool->ClearSel();
				Done();

				return KSTA_DONE;
			}
		}
	}
	
	return KSTA_CONTINUE;
}

int		kcmOneFaceBase::OnExecute()
{
	m_aFace = GetFaceFromGlobalSelSet();
	if(!m_aFace.IsNull()){
		OnSelOneFace();
		Done();

		return KSTA_DONE;
	}

	return KSTA_CONTINUE;
}

TopoDS_Face	kcmOneFaceBase::GetFaceFromGlobalSelSet()
{
	TopoDS_Face aFace;
	//�ж��Ƿ�ȫ��ѡ���ж���
	kiSelSet *pSelSet = GetGlobalSelSet();
	if(pSelSet && pSelSet->GetSelCount() > 0){
		pSelSet->InitSelected();
		kiSelEntity se = pSelSet->CurSelected();
		TopoDS_Shape aShape = se._pEntity->GetShape();
		if(aShape.ShapeType() == TopAbs_FACE){
			aFace = TopoDS::Face(aShape);
		}
	}

	return aFace;
}

int		kcmOneFaceBase::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmOneFaceBase::CreateInputTools()
{
	m_pSelFaceTool = new kiInputEntityTool(this,"ʰȡһ��Face:",NULL);
	m_pSelFaceTool->SetEntityTypes(KCT_ENT_FACE);
	m_pSelFaceTool->SetSingleSelect(true);
	m_pSelFaceTool->AddInitSelected(true);
	
	return TRUE;
}

BOOL	kcmOneFaceBase::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pSelFaceTool);

	return TRUE;
}

BOOL	kcmOneFaceBase::InitInputToolQueue()
{
	if(m_pSelFaceTool){
		AddInputTool(m_pSelFaceTool);
	}

	return TRUE;
}

int	kcmOneFaceBase::AddShapeToModel(const TopoDS_Shape& aShape)
{
	kcEntityFactory entfac;
	kcEntity *pEnt = NULL;

	pEnt = entfac.Create(aShape);
	if(pEnt){
		kcModel *pModel = GetModel();
		pModel->BeginUndo(GetName());
		pModel->AddEntity(pEnt);
		pModel->EndUndo();
	}
	return 1;
}

//////////////////////////////////////////////////////
//

kcmOneFaceCopy::kcmOneFaceCopy()
{
	m_strName = "����Face";
	m_strAlias = "FaceCopy";
	m_strDesc = "����һ��Face";
}

kcmOneFaceCopy::~kcmOneFaceCopy()
{
}

int	kcmOneFaceCopy::OnSelOneFace()
{
	BRepBuilderAPI_Copy BBC;

	BBC.Perform(m_aFace);//,Standard_True,Standard_True);
	if(BBC.IsDone()){
		TopoDS_Shape aShape = BBC.Shape();
		TopoDS_Face aFace = TopoDS::Face(aShape);
		Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
		double U1,U2,V1,V2;
		ShapeAnalysis::GetFaceUVBounds(aFace,U1,U2,V1,V2);

		gp_Pnt pnt;
		gp_Vec D1U,D1V,norm,vec;
		aSurf->D1((U1+U2)/2,(V1+V2)/2,pnt,D1U,D1V);
		norm = D1U.Crossed(D1V);
		gp_Dir un(norm);
		vec.SetCoord(un.X() * 10,un.Y() * 10,un.Z() * 10);
		//����Face������б�Ҫ�ķ���
		if(aFace.Orientation() == TopAbs_REVERSED)
			vec.Reverse();

		gp_Trsf trsf;
		trsf.SetTranslation(vec);
		BRepBuilderAPI_Transform BBTrsf(trsf);
		BBTrsf.Perform(aShape);
		if(BBTrsf.IsDone()){
			aShape = BBTrsf.Shape();
			AddShapeToModel(aShape);
			//Redraw();
		}	

#ifdef _V_DEBUG
		ShapeAnalysis::OuterWire(m_aFace);
		TopoDS_Iterator ite;
		TopoDS_Wire aTw;
		for(ite.Initialize(m_aFace);ite.More();ite.Next()){
			aTw = TopoDS::Wire(ite.Value());
		}
#endif
		
	}

	return KSTA_DONE;
}

///////////////////////////////////////////////////////
// ��ȡһ��face�Ĳü����Ĳ�������
kcmOneFaceParLoop::kcmOneFaceParLoop()
{
	m_strName = "��Ĳ����ü���";
	m_strAlias = "FaceParLoop";
	m_strDesc = "��ȡFace�Ĳ�����ü���";
}

kcmOneFaceParLoop::~kcmOneFaceParLoop()
{
}

int		kcmOneFaceParLoop::OnSelOneFace()
{
	TopoDS_Wire aWire,aNW;
	TopoDS_Edge aEdge,aNE;
	TopoDS_Compound aComp;
	BRep_Builder BB;
	TopoDS_Iterator wite,eite;
	double dF,dL;
	gp_Ax2 ax2;

	BB.MakeCompound(aComp);

	for(wite.Initialize(m_aFace);wite.More();wite.Next()){
		aWire = TopoDS::Wire(wite.Value());

		BB.MakeWire(aNW);

		for(eite.Initialize(aWire);eite.More();eite.Next()){
			aEdge = TopoDS::Edge(eite.Value());

			Handle(Geom_Curve) aCrv3d;
			Handle(Geom2d_Curve) aCrv2d = BRep_Tool::CurveOnSurface(aEdge,m_aFace,dF,dL);
			if(!aCrv2d.IsNull()){
				aCrv3d = GeomLib::To3d(ax2,aCrv2d);
				if(!aCrv3d.IsNull()){
					BRepBuilderAPI_MakeEdge me(aCrv3d,dF,dL);
					if(me.IsDone()){
						aNE = me.Edge();
						//
						aNE.Orientation(aEdge.Orientation());

						BB.Add(aNW,aNE);
					}
				}
			}
		}

		BB.Add(aComp,aNW);
	}

	AddShapeToModel(aComp);

	return KSTA_DONE;
}

////////////////////////////////////////////////////////////////////////
//
//��һ��������в�ֵ����ȡ��ֵ�����
kcmInterpOneFace::kcmInterpOneFace()
{
	m_strName = "��ֵ��";
	m_strAlias = "InterpOneFace";
	m_strDesc = "������в�����ֵ�����µ���";
}

kcmInterpOneFace::~kcmInterpOneFace()
{
}

int  kcmInterpOneFace::OnSelOneFace()
{
	double U1,U2,V1,V2,umin,umax,vmin,vmax;
	int uix,vix,unum = 8,vnum = 8;
	double u,v,ustp,vstp;
	gp_Pnt pnt;

	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(m_aFace);
	if(aSurf.IsNull())
		return KSTA_ERROR;

	aSurf->Bounds(U1,U2,V1,V2);
	if(Precision::IsInfinite(U1) || Precision::IsInfinite(V1)){
		BRepTools::UVBounds(m_aFace,umin,umax,vmin,vmax);
		if(Precision::IsInfinite(U1)){
			U1 = umin;U2 = umax;
		}
		if(Precision::IsInfinite(V1)){
			V1 = vmin;V2 = vmax;
		}
	}
	ustp = (U2 - U1) / unum;
	vstp = (V2 - V1) / vnum;

	TColgp_Array2OfPnt aPntNet(1,unum+1,1,vnum+1);

	for(uix = 0;uix <= unum;uix ++){
		u = U1 + uix * ustp;
		for(vix = 0;vix <= vnum;vix ++){
			v = V1 + vix * vstp;

			aSurf->D0(u,v,pnt);
			aPntNet.SetValue(uix+1,vix+1,pnt);
		}
	}

	//��ֵ��������
	GeomAPI_PointsToBSplineSurface ptos;
	Handle(Geom_BSplineSurface) aBSplSurf;
	try{
		ptos.Interpolate(aPntNet);
		if(ptos.IsDone()){
			aBSplSurf = ptos.Surface();
		}
	}catch(Standard_Failure){
		AfxMessageBox("point to bspline surface failed.catch excption.");
	}

	if(!aBSplSurf.IsNull()){
		//��֤ÿ���㵽����ľ���
		for(uix = 0;uix <= unum;uix ++){
			for(vix = 0;vix <= vnum;vix ++){
				pnt = aPntNet.Value(uix+1,vix+1);

				GeomAPI_ProjectPointOnSurf ppons(pnt,aBSplSurf);
				if(ppons.IsDone()){
					double lowdist = ppons.LowerDistance();
					if(lowdist > 0.00001){
						TRACE("\n %d %d low dist is %f.",uix,vix,lowdist);
					}
				}
			}
		}

		BRepBuilderAPI_MakeFace mf(aBSplSurf,0.00001);
		if(mf.IsDone()){
			TopoDS_Face aFace = mf.Face();
			AddShapeToModel(aFace);
		}
	}
		
	return KSTA_DONE;
}

/////////////////////////////////////////////////////////////////////////
//
//����һ��ѡ�е���
kcmExpOneFaceToStep::kcmExpOneFaceToStep()
{
	m_strName = "����face��step";
	m_strAlias = "ExpOneFaceToStep";
	m_strDesc = "����һ��ѡ�е�face��step�ļ�";
}

kcmExpOneFaceToStep::~kcmExpOneFaceToStep()
{
}

int  kcmExpOneFaceToStep::OnSelOneFace()
{
	kcSTEPExport exp(GetModel());
	//
	exp.AddShape(m_aFace);
	exp.DoExport();

	return KSTA_DONE;
}



/////////////////////////////////////////////////////////////////////////
//
//�������������һЩ����
kcmPeriodicBSpSurfTry::kcmPeriodicBSpSurfTry(int subcmd)
	:subCmd_(subcmd)
{
	if(subcmd == 1){
		m_strName = "ת����������";
		m_strAlias = "PBspSurfToNP";
		m_strDesc = "����������ת������������";
	}else if(subcmd == 2){
		m_strName = "�ı�U�����";
		m_strAlias = "PBspSurfChgUOrg";
		m_strDesc = "����������ı�U�������ʼ��";
	}else if(subcmd == 3){
		m_strName = "�ı�V�����";
		m_strAlias = "PBspSurfChgVOrg";
		m_strDesc = "����������ı�V�������ʼ��";
	}
}

kcmPeriodicBSpSurfTry::~kcmPeriodicBSpSurfTry()
{
}

int  kcmPeriodicBSpSurfTry::OnSelOneFace()
{
	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(m_aFace);
	if(aSurf.IsNull())
		return KSTA_ERROR;

	if(!aSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface))){
		AfxMessageBox("not bspline surface!");
		return KSTA_DONE;
	}

	Handle(Geom_BSplineSurface) aBspSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);
	if(!aBspSurf){
		return KSTA_ERROR;
	}

	if(!aBspSurf->IsUPeriodic() && !aBspSurf->IsVPeriodic()){
		AfxMessageBox("not periodic surface.");
		return KSTA_DONE;
	}

	switch(subCmd_){
	case 1:
		ToNoPeriodicSurf(aBspSurf);
		break;
	case 2:
		ChangeUVOrigin(aBspSurf,1);
		break;
	case 3:
		ChangeUVOrigin(aBspSurf,2);
		break;
	default:
		break;
	}

	return KSTA_DONE;
}

//������������תΪ��������������
//
void  kcmPeriodicBSpSurfTry::ToNoPeriodicSurf(const Handle(Geom_BSplineSurface) &aBspSurf)
{
	Standard_Boolean bUPeriodic = aBspSurf->IsUPeriodic();
	Standard_Boolean bVPeriodic = aBspSurf->IsVPeriodic();
	Handle(Geom_BSplineSurface) aCpSurf = Handle(Geom_BSplineSurface)::DownCast(aBspSurf->Copy());

	if(bUPeriodic){
		aCpSurf->SetUNotPeriodic();
	}
	if(bVPeriodic){
		aCpSurf->SetVNotPeriodic();
	}

	double NU1,NU2,NV1,NV2;
	aCpSurf->Bounds(NU1,NU2,NV1,NV2);

	aCpSurf->Segment(NU1,NU2,NV1,NV2);

	//�����µ�Face
	BRepBuilderAPI_MakeFace mf(aCpSurf,0.00001);
	if(mf.IsDone()){
		TopoDS_Face aFace = mf.Face();
		AddShapeToModel(aFace);
	}
}

/// �ı��������������ԭ�㣬����������Χ�п�������
void  kcmPeriodicBSpSurfTry::ChangeUVOrigin(const Handle(Geom_BSplineSurface) &aBspSurf,int uvflag)
{
	if((uvflag == 1 && !aBspSurf->IsUPeriodic()) ||
		(uvflag == 2 && !aBspSurf->IsVPeriodic()))
		return;

	double U1,U2,V1,V2;
	double umin,umax,vmin,vmax;
	int lx,rx;
	double patol = Precision::PConfusion();

	aBspSurf->Bounds(U1,U2,V1,V2);
	//����������Χ��
	BRepTools::UVBounds(m_aFace,umin,umax,vmin,vmax);

	Handle(Geom_BSplineSurface) aCpSurf;
	if(uvflag == 1){
		if(!OLB_IN_RANGE(umin,U1,U2,patol) || 
			!OLB_IN_RANGE(umax,U1,U2,patol))
		{
			aCpSurf = Handle(Geom_BSplineSurface)::DownCast(aBspSurf->Copy());
			aCpSurf->LocateU(umin,patol,lx,rx);
			aCpSurf->SetUOrigin(lx);
		}
	}else{
		if(!OLB_IN_RANGE(vmin,V1,V2,patol) || 
			!OLB_IN_RANGE(vmax,V1,V2,patol))
		{
			aCpSurf = Handle(Geom_BSplineSurface)::DownCast(aBspSurf->Copy());
			aCpSurf->LocateV(vmin,patol,lx,rx);
			aCpSurf->SetVOrigin(lx);
		}
	}

	if(!aCpSurf.IsNull()){
		//�����µ�Face
		BRepBuilderAPI_MakeFace mf(aCpSurf,0.00001);
		if(mf.IsDone()){
			TopoDS_Face aFace = mf.Face();
			AddShapeToModel(aFace);
		}
	}
}

/////////////////////////////////////////////////////////////////////////
//
kcmExtractNFace::kcmExtractNFace()
{
	m_strName = "��ǰ��n����";
	m_strAlias = "ExtractNFace";
	m_strDesc = "�Ӷ�������ǰ��n����";

	_pInputEntityTool = NULL;
	_pInputNumTool = NULL;
	_nFaceNo = 1;
}

kcmExtractNFace::~kcmExtractNFace()
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL  kcmExtractNFace::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int  kcmExtractNFace::	OnInputFinished(kiInputTool *pTool)
{
	if(pTool == _pInputEntityTool){
		if(_pInputEntityTool->IsInputDone()){
			NavToNextTool();
		}
	}else if(pTool == _pInputNumTool){
		if(_pInputNumTool->IsInputDone()){

			if(DoExtractFace()){

				Done();
				return KSTA_DONE;
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

BOOL  kcmExtractNFace::DoExtractFace()
{
	int ix = 1;
	kiSelEntity se = _pInputEntityTool->GetFirstSelect();
	TopoDS_Shape aShape = se._pEntity->GetShape();
	TopExp_Explorer exp;
	TopoDS_Face aFace;
	for(exp.Init(aShape,TopAbs_FACE);exp.More();exp.Next()){
		if(ix == _nFaceNo){
			aFace = TopoDS::Face(exp.Current());
			break;
		}
		ix ++;
	}

	if(aFace.IsNull()){
		AfxMessageBox("Extract face failed.");
		return FALSE;
	}

	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
	double U1,U2,V1,V2;
	ShapeAnalysis::GetFaceUVBounds(aFace,U1,U2,V1,V2);

	gp_Pnt pnt;
	gp_Vec D1U,D1V,norm,vec;
	aSurf->D1((U1+U2)/2,(V1+V2)/2,pnt,D1U,D1V);
	norm = D1U.Crossed(D1V);
	gp_Dir un(norm);
	vec.SetCoord(un.X() * 10,un.Y() * 10,un.Z() * 10);
	//����Face������б�Ҫ�ķ���
	if(aFace.Orientation() == TopAbs_REVERSED)
		vec.Reverse();

	gp_Trsf trsf;
	trsf.SetTranslation(vec);
	BRepBuilderAPI_Transform BBTrsf(trsf);
	BBTrsf.Perform(aFace);
	if(!BBTrsf.IsDone())
		return FALSE;

	aShape = BBTrsf.Shape();
	kcEntityFactory efac;
	kcEntity *pNewEnt = efac.Create(aShape);
	if(pNewEnt){
		kcModel *pModel = GetModel();
		pModel->BeginUndo(GetName());
		pModel->AddEntity(pNewEnt);
		pModel->EndUndo();

		return TRUE;
	}
	
	return FALSE;
}

// ������Ҫ�����빤��
BOOL  kcmExtractNFace::CreateInputTools()
{
	_pInputEntityTool = new kiInputEntityTool(this,"ѡ��һ������");
	_pInputEntityTool->SetOption(KCT_ENT_SOLID | KCT_ENT_SHELL | KCT_ENT_COMPOUND,true);

	_pInputNumTool = new kiInputIntTool(this,"��ǰ�ڼ�����(��1��ʼ)");
	_pInputNumTool->Init(&_nFaceNo);

	return TRUE;
}

BOOL  kcmExtractNFace::DestroyInputTools()
{
	KC_SAFE_DELETE(_pInputEntityTool);
	KC_SAFE_DELETE(_pInputNumTool);

	return TRUE;
}

BOOL  kcmExtractNFace::InitInputToolQueue()
{
	AddInputTool(_pInputEntityTool);
	AddInputTool(_pInputNumTool);

	return TRUE;
}