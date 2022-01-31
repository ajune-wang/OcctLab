#include "StdAfx.h"
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include "kcModel.h"
#include "kcBasePlane.h"
#include "kvGrid.h"
#include "kcSnapMngr.h"

/////////////////////////////////////////////////////////////////////////
// �㲶׽�Ļ���˼·��
// a����ǰԤ�������3D����Ϣ�����벶׽������
// b���Զ���׽ʱ����3D��ת��ƽ���ϵ�pixel�㣬�ɴ��жϵ��Ƿ�ѡ��
// ȱ�㣺���ظ��ĵ㣬û����ȡ������ĵ㣬��������Ż�.

static void UxModelToPixel(const Handle(V3d_View) &aView, const kPoint3& wp,int& x,int& y)
{
	aView->Convert(wp.x(),wp.y(),wp.z(),x,y);
}

//////////////////////////////////////////////////////////////////////////
//
kiSnapPointSet::kiSnapPointSet()
{

}

bool	kiSnapPointSet::AddSnapPoint(const kiSnapPoint& sp)
{
	_spArray.push_back(sp);
	return true;
}

void	kiSnapPointSet::Clear()
{
	_spArray.clear();
}

int kiSnapPointSet::GetCount() const
{
	return (int)_spArray.size();
}

const kiSnapPoint&	kiSnapPointSet::GetSnapPoint(int idx) const
{
	return _spArray[idx];
}

bool kiSnapPointSet::CalcScreenPoint(const Handle(V3d_View) &aView)
{
	if(aView.IsNull())
		return false;

	int ix,isize = (int)_spArray.size();
	for(ix = 0;ix < isize;ix ++)
	{
		kiSnapPoint& sp = _spArray.at(ix);
		//ת�뵽��Ļ����
		UxModelToPixel(aView,sp._point,sp._scx,sp._scy);
		//pWorkPlane->ModelToPixel(sp._point,sp._scx,sp._scy);
	}
	return true;
}

void	kiSnapPointSet::DoSnap(int x,int y,int scope,std::vector<kiSnapPoint>& ss)
{
	POINT pnt;
	CRect selrc;
	int ix,isize = (int)_spArray.size();

	pnt.x = x;pnt.y = y;
	for(ix = 0;ix < isize;ix ++)
	{
		kiSnapPoint& sp = _spArray.at(ix);
		selrc.SetRect(sp._scx - scope,sp._scy - scope,sp._scx + scope,sp._scy + scope);
		if(selrc.PtInRect(pnt))
			ss.push_back(sp);
	}
}

//////////////////////////////////////////////////////////////////////////
//
kcSnapMngr::kcSnapMngr(kcModel *pModel)
	:pModel_(pModel)
{
	_nScope = 4;
	_nSnapFlag = KSNAP_ALL;
	//��������
	pModel_->SetSnapManager(this);
}

kcSnapMngr::~kcSnapMngr(void)
{
	Clear();

	//ȡ����Model�Ĺ���
	if(pModel_){
		pModel_->SetSnapManager(NULL);
	}
}

//��ӵ�
bool	kcSnapMngr::AddSnapPoint(int snapType,const kiSnapPoint& sp)
{
	kiSnapPointSet *pSet = GetPointSet(snapType);
	if(!pSet) return false;

	pSet->AddSnapPoint(sp);

	// ��ʱ��Ҫ���¼�����Ļ����.
	if(snapType == KSNAP_TMP_POINT)
	{
		_tmpSet.CalcScreenPoint(aV3dView_);
	}

	return true;
}

kcBasePlane* kcSnapMngr::GetCurrentBasePlane()
{
	if(pModel_){
		return pModel_->GetCurrentBasePlane();
	}
	return NULL;
}

bool kcSnapMngr::AddSnapPointSet(int snapType,const kiSnapPointSet& ss)
{
	kiSnapPointSet *pSet = GetPointSet(snapType);
	if(!pSet) return false;

	int ix,isize = ss.GetCount();
	for(ix = 0;ix < isize;ix ++)
		pSet->AddSnapPoint(ss.GetSnapPoint(ix));

	return true;
}

void	kcSnapMngr::Clear()
{
	_pntSet.Clear();
	_termSet.Clear();
	_midSet.Clear();
	_cenSet.Clear();
	_tmpSet.Clear();
	_quaSet.Clear();
}

//�����ʱ��
void	kcSnapMngr::ClearTempPoint()
{
	_tmpSet.Clear();
}

// ����㲶׽��غ���
bool	kcSnapMngr::AddGridSnapPoint(const kiSnapPoint& sp)
{
	_gridSet.AddSnapPoint(sp);
	return true;
}

void	kcSnapMngr::ClearGridSnapPoint()
{
	_gridSet.Clear();
}

// �ϲ�����mgr
bool	kcSnapMngr::Merge(const kcSnapMngr& smgr)
{
	AddSnapPointSet(KSNAP_EXIST_POINT,smgr.ExistPointSet());
	AddSnapPointSet(KSNAP_TERM_POINT,smgr.TermPointSet());
	AddSnapPointSet(KSNAP_MID_POINT,smgr.MidPointSet());
	AddSnapPointSet(KSNAP_CEN_POINT,smgr.CenPointSet());
	AddSnapPointSet(KSNAP_TMP_POINT,smgr.TempPointSet());
	AddSnapPointSet(KSNAP_QUA_POINT,smgr.QuaPointSet());

	return true;
}

void  kcSnapMngr::SetView(const Handle(V3d_View) &aView)
{
	ASSERT(!aView.IsNull());
	aV3dView_ = aView;
}

kiSnapPointSet*  kcSnapMngr::GetPointSet(int snapType) 
{
	kiSnapPointSet *pSet = NULL;
	switch(snapType){
		case KSNAP_EXIST_POINT://���ڵ�
			pSet = &_pntSet;
			break;
		case KSNAP_TERM_POINT: //�˵�
			pSet = &_termSet;
			break;
		case KSNAP_MID_POINT: //�е�
			pSet = &_midSet;
			break;
		case KSNAP_CEN_POINT: //���ĵ㡣
			pSet = &_cenSet;
			break;
		case KSNAP_TMP_POINT: //��ʱ��
			pSet = &_tmpSet;
			break;
		case KSNAP_QUA_POINT:
			pSet = &_quaSet;
			break;
		default:
			break;
	}

	return pSet;
}

void	kcSnapMngr::SetSnapScope(int scope)
{
	_nScope = scope;
}

bool	kcSnapMngr::EnableSnap(int nFlag)
{
	_nSnapFlag |= nFlag;
	return true;
}

bool	kcSnapMngr::DisableSnap(int nFlag)
{
	_nSnapFlag &= ~nFlag;
	return true;
}

bool	kcSnapMngr::EnableAll()
{
	_nSnapFlag = KSNAP_ALL;
	return true;
}

bool	kcSnapMngr::DisableAll()
{
	_nSnapFlag = KSNAP_NIL;
	return true;
}


// ������Ļ����
// ���ݹ���ƽ�棬���㵱ǰ���е����Ļ���ꡣ
//
bool	kcSnapMngr::CalcScreenPoint()
{
	kcBasePlane *pBasePlane = GetCurrentBasePlane();
	if(!pBasePlane)
		return false;

	UxModelToPixel(aV3dView_,_org._point,_org._scx,_org._scy);
	//pWorkPlane->ModelToPixel(_org._point,_org._scx,_org._scy);
	_pntSet.CalcScreenPoint(aV3dView_);
	_termSet.CalcScreenPoint(aV3dView_);
	_midSet.CalcScreenPoint(aV3dView_);
	_cenSet.CalcScreenPoint(aV3dView_);
	_tmpSet.CalcScreenPoint(aV3dView_);
	_quaSet.CalcScreenPoint(aV3dView_);

	if(_nSnapFlag & KSNAP_GRID_POINT)
	{
		kvGrid *pGrid = pBasePlane->GetGrid();
		ASSERT(pGrid);
		pGrid->CalcSnapPoint(*this);
	}

	return true;
}

// ���в�׽,x,y����Ļ�������㡣
//
void	kcSnapMngr::DoSnap(int x,int y)
{
	std::vector<kiSnapPoint> ss;
	kcBasePlane *pBasePlane = GetCurrentBasePlane();
	ASSERT(pBasePlane);
	if(!pBasePlane) return;

	_bSnaped = false;
	HideSnapMarker();

	POINT pnt;
	CRect orc;
	pnt.x = x;pnt.y = y;
	orc.SetRect(_org._scx - _nScope,_org._scy - _nScope,_org._scx + _nScope,_org._scy + _nScope);
	if(orc.PtInRect(pnt))
		ss.push_back(_org);

	if(_nSnapFlag & KSNAP_EXIST_POINT)
		_pntSet.DoSnap(x,y,_nScope,ss);

	if(_nSnapFlag & KSNAP_TERM_POINT)
		_termSet.DoSnap(x,y,_nScope,ss);

	if(_nSnapFlag & KSNAP_MID_POINT)
		_midSet.DoSnap(x,y,_nScope,ss);

	if(_nSnapFlag & KSNAP_CEN_POINT)
		_cenSet.DoSnap(x,y,_nScope,ss);

	if(_nSnapFlag & KSNAP_TMP_POINT)
		_tmpSet.DoSnap(x,y,_nScope,ss);

	if(_nSnapFlag & KSNAP_QUA_POINT)
		_tmpSet.DoSnap(x,y,_nScope,ss);

	if(ss.empty())//��������׽��
	{
		//û�У����ж��Ƿ�׽������㡣
		if(_nSnapFlag & KSNAP_GRID_POINT)
			_gridSet.DoSnap(x,y,_nScope,ss);
	}

	if(ss.empty())
	{
		_bSnaped = false;
		return;
	}

	//Ѱ�Ҿ�������ġ�
	int minix = -1,mindist = 1000;
	int ix,isize = (int)ss.size();
	for(ix = 0;ix < isize;ix ++)
	{
		if(abs(ss[ix]._scx - x) < mindist)
		{
			mindist = abs(ss[ix]._scx - x);
			minix = ix;
			continue;
		}
		if(abs(ss[ix]._scy - y) < mindist)
		{
			mindist = abs(ss[ix]._scy - y);
			minix = ix;
		}
	}

	_bSnaped = true;
	_curSnapPoint = ss.at(minix);

	ShowSnapMarker();
}

const kiSnapPoint&	kcSnapMngr::GetSnapPoint()
{
	ASSERT(_bSnaped);
	return _curSnapPoint;
}

void	kcSnapMngr::HideSnapMarker()
{
	if(_hSnapPntStru.IsNull())
		return;

	_hSnapPntStru->Erase();
	_hSnapPntStru->Clear();

	if(!aV3dView_.IsNull()){
		aV3dView_->Redraw();
	}
}

void	kcSnapMngr::ShowSnapMarker()
{
	if(_hSnapPntStru.IsNull())
	{
		Handle(AIS_InteractiveContext) AISCtx = pModel_->GetAISContext();
		ASSERT(!AISCtx.IsNull());
		_hSnapPntStru = new Graphic3d_Structure(AISCtx->CurrentViewer()->StructureManager());
		if(_hSnapPntStru.IsNull())
			return;
	}
	_hSnapPntGrp = _hSnapPntStru->NewGroup();//new Graphic3d_Group(_hSnapPntStru);
	if(_hSnapPntStru.IsNull() || _hSnapPntGrp.IsNull())
		return;

	Handle(Graphic3d_AspectMarker3d) mct = new Graphic3d_AspectMarker3d();
	mct->SetColor(Quantity_Color(1.0,1.0,0.0,Quantity_TOC_RGB));
	mct->SetType(Aspect_TOM_O_PLUS);
	mct->SetScale(2.0);
	_hSnapPntGrp->SetGroupPrimitivesAspect(mct);

	Handle(Graphic3d_ArrayOfPoints) aPoints = new Graphic3d_ArrayOfPoints (1);
	aPoints->AddVertex(_curSnapPoint._point.x(),_curSnapPoint._point.y(),_curSnapPoint._point.z());
	_hSnapPntGrp->AddPrimitiveArray(aPoints);

	_hSnapPntStru->Display();

	if(!aV3dView_.IsNull()){
		aV3dView_->Redraw();
	}
}

//////////////////////////////////////////////////////////////////////////
//����һ�����ߵ��Զ���׽��
bool	kcSnapMngr::CalcSnapPoint(const Handle(Geom_Curve)& aCurve)
{
	Handle(Geom_Curve) aBasCur;
	if(aCurve.IsNull()) return false;

	//��֤����
	aBasCur = aCurve;
	while(aBasCur->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
	{
		Handle(Geom_OffsetCurve) aOffCur = Handle(Geom_OffsetCurve)::DownCast(aBasCur);
		aBasCur = aOffCur->BasisCurve();
	}
	
	//���޽������,�޷����㲶׽��
	if(aBasCur->IsKind(STANDARD_TYPE(Geom_Line)) ||
	   aBasCur->IsKind(STANDARD_TYPE(Geom_Hyperbola)) ||
	   aBasCur->IsKind(STANDARD_TYPE(Geom_Parabola)))
		return false;

	double t0 = aBasCur->FirstParameter();
	double t1 = aBasCur->LastParameter();

	return CalcSnapPoint(aCurve,t0,t1);
}

bool	kcSnapMngr::CalcSnapPoint(const Handle(Geom_Curve)& aCurve,double t0,double t1)
{
	if(aCurve.IsNull()) return false;

	gp_Pnt pnt,p0,p1,pm;

	//�˵�
	aCurve->D0(t0,pnt);
	AddSnapPoint(KSNAP_TERM_POINT,kiSnapPoint(pnt.X(),pnt.Y(),pnt.Z()));
	aCurve->D0(t1,pnt);
	AddSnapPoint(KSNAP_TERM_POINT,kiSnapPoint(pnt.X(),pnt.Y(),pnt.Z()));
	//�е�
	aCurve->D0((t0 + t1) * 0.5,pnt);
	AddSnapPoint(KSNAP_MID_POINT,kiSnapPoint(pnt.X(),pnt.Y(),pnt.Z()));

	// �����Բ��Բ����
	Handle(Geom_Curve) aBasCur = aCurve;
	if(aCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
	{
		Handle(Geom_OffsetCurve) aOffCur = Handle(Geom_OffsetCurve)::DownCast(aCurve);
		aBasCur = aOffCur->BasisCurve();
	}
	else if(aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
	{
		Handle(Geom_TrimmedCurve) aTrmCur = Handle(Geom_TrimmedCurve)::DownCast(aCurve);
		aBasCur = aTrmCur->BasisCurve();
	}
	if(aBasCur->IsKind(STANDARD_TYPE(Geom_Circle)))
	{
		Handle(Geom_Circle) hCirc = Handle(Geom_Circle)::DownCast(aBasCur);
		gp_Circ gCirc = hCirc->Circ();
		pnt = gCirc.Location();
		AddSnapPoint(KSNAP_CEN_POINT,kiSnapPoint(pnt.X(),pnt.Y(),pnt.Z()));
	}

	//Բ�������ķֵ�
	if(aCurve->IsKind(STANDARD_TYPE(Geom_Circle))){
		double t = t0 + 0.25 * (t1 - t0);
		aCurve->D0(t,pnt);
		AddSnapPoint(KSNAP_QUA_POINT,kiSnapPoint(pnt.X(),pnt.Y(),pnt.Z()));

		t = t0 + 0.75 * (t1 - t0);
		aCurve->D0(t,pnt);
		AddSnapPoint(KSNAP_QUA_POINT,kiSnapPoint(pnt.X(),pnt.Y(),pnt.Z()));
	}

	return true;
}

bool	kcSnapMngr::CalcSnapPoint(const TopoDS_Edge& aEdge)
{
	if(aEdge.IsNull()) return false;
	if (BRep_Tool::Degenerated(aEdge)) return false;
	double t0,t1;
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge,t0,t1);

	return CalcSnapPoint(aCurve,t0,t1);
}

//����һ��������Զ���׽��
bool	kcSnapMngr::CalcSnapPoint(const TopoDS_Face& aFace)
{
	if(aFace.IsNull())
		return false;

	//������еĶ˵�
	TopExp_Explorer ex;
	TopoDS_Vertex aV;
	gp_Pnt pnt;
	ex.Init(aFace,TopAbs_VERTEX);
	for(;ex.More();ex.Next())
	{
		aV = TopoDS::Vertex(ex.Current());
		pnt = BRep_Tool::Pnt(aV);
		AddSnapPoint(KSNAP_TERM_POINT,kiSnapPoint(pnt.X(),pnt.Y(),pnt.Z()));
	}

	//��ӱ߽����ߵ��е�
	TopExp_Explorer ex2;
	TopoDS_Edge aE;
	ex2.Init(aFace,TopAbs_EDGE);
	for(;ex2.More();ex2.Next())
	{
		aE = TopoDS::Edge(ex2.Current());
		CalcSnapPoint(aE,aFace);
	}

	return true;
}

// 
bool	kcSnapMngr::CalcSnapPoint(const TopoDS_Shape& aShape)
{
	if(aShape.IsNull())
		return false;
	TopAbs_ShapeEnum styp = aShape.ShapeType();
	if(styp == TopAbs_VERTEX)
	{
		TopoDS_Vertex aV = TopoDS::Vertex(aShape);
		gp_Pnt pnt = BRep_Tool::Pnt(aV);
		AddSnapPoint(KSNAP_EXIST_POINT,kiSnapPoint(pnt.X(),pnt.Y(),pnt.Z()));
	}
	else if(styp == TopAbs_EDGE)
	{
		CalcSnapPoint(TopoDS::Edge(aShape));
	}
	else if(styp == TopAbs_WIRE)
	{
		TopoDS_Iterator ite(aShape);
		for(;ite.More();ite.Next())
			CalcSnapPoint(TopoDS::Edge(ite.Value()));
	}
	else if(styp == TopAbs_FACE)
	{
		CalcSnapPoint(TopoDS::Face(aShape));
	}
	else if(styp == TopAbs_SHELL || styp == TopAbs_SOLID)
	{
		TopExp_Explorer ex;
		TopoDS_Face aFace;
		ex.Init(aShape,TopAbs_FACE);
		for(;ex.More();ex.Next())
		{
			aFace = TopoDS::Face(ex.Current());
			CalcSnapPoint(aFace);
		}
	}
	else
	{
		TopoDS_Iterator ite(aShape);
		for(;ite.More();ite.Next())
			CalcSnapPoint(ite.Value());//�ݹ����
	}
	return true;	
}

bool	kcSnapMngr::CalcSnapPoint(const TopoDS_Edge& aEdge,const TopoDS_Face& aFace)
{
	//��ȡ�����ϵ�����
	double f,l,m;
	gp_Pnt2d p2d;
	gp_Pnt pnt;
	Handle(Geom2d_Curve) aCur2d = BRep_Tool::CurveOnSurface(aEdge,aFace,f,l);
	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
	if(aCur2d.IsNull() || aSurf.IsNull())
		return false;

	m = (f + l) * 0.5;
	aCur2d->D0(m,p2d);
	aSurf->D0(p2d.X(),p2d.Y(),pnt);
	AddSnapPoint(KSNAP_MID_POINT,kiSnapPoint(pnt.X(),pnt.Y(),pnt.Z()));

	return true;
}
