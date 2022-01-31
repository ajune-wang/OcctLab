#include "StdAfx.h"
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include "kcModel.h"
#include "kcLayer.h"
#include "kcHandleMgr.h"
#include "QxBRepLib.h"
#include "kiSnapMngr.h"
#include "QxStgBlock.h"
#include "kcShapeAttribMgr.h"
#include "AIS_EntityShape.h"
#include "kcEntity.h"

#ifndef K_ROUND
#define K_ROUND(v,a,b) \
	if(v < a) v = a; \
	if(v > b) v = b;
#endif

static struct stEntTypePair{
	int		_enttype;
	TopAbs_ShapeEnum  _shapetype;
}st_typePair[8] = {
 	{ KCT_ENT_POINT, TopAbs_VERTEX },
	{ KCT_ENT_EDGE, TopAbs_EDGE },
	{ KCT_ENT_WIRE,  TopAbs_WIRE },
	{ KCT_ENT_FACE,  TopAbs_FACE },
	{ KCT_ENT_SHELL, TopAbs_SHELL },
	{ KCT_ENT_SOLID, TopAbs_SOLID },
	{ KCT_ENT_COMPOUND, TopAbs_COMPOUND },
	{ KCT_ENT_BLOCK, TopAbs_COMPOUND}
};

////////////////////////////////////////////////////////////
//
//��������
bool  ksIsValidEntityType(int entType)
{
	int ix;
	for(ix = 0;ix < 8;ix ++){
		if(st_typePair[ix]._enttype == entType)
			return true;
	}
	return false;
}

// TODO:������Ҫ��block�������⴦��
int  ksEntityType(const TopAbs_ShapeEnum shapeType)
{
	int ix,entType = -1;
	for(ix = 0;ix < 7;ix ++){
		if(st_typePair[ix]._shapetype == shapeType){
			entType = st_typePair[ix]._enttype;
			break;
		}
	}
	return entType;
}

TopAbs_ShapeEnum  ksShapeType(int entType)
{
	TopAbs_ShapeEnum shapeType = TopAbs_SHAPE;
	int ix;
	for(ix = 0;ix < 8;ix ++){
		if(st_typePair[ix]._enttype == entType){
			shapeType = st_typePair[ix]._shapetype;
			break;
		}
	}
	return shapeType;
}

int   ksSeparateEntityTypes(int enttypes,int *pTypeArray)//�������͸���
{
	ASSERT(pTypeArray);
	int ix,cnt = 0;
	for(ix = 0;ix < 8;ix ++){
		if(enttypes & st_typePair[ix]._enttype){
			pTypeArray[cnt] = st_typePair[ix]._enttype;
			cnt ++;
		}
	}
	return cnt;
}

/////////////////////////////////////////////////////////////
//
kcEntity::kcEntity(void)
{
	Reset();
}

kcEntity::kcEntity(kcLayer *pLayer)
{
	Reset();
	SetLayer(pLayer);
}

kcEntity::~kcEntity(void)
{
	Destroy();
}

void kcEntity::Reset()
{
	_entType = 0;
	_sTypeName = "";
	_nHandle = 0;
	_szName[0] = '\0';
	sNameDesc_ = "";
	_pModel = NULL;
	_pLayer = NULL;
	pOwner_ = NULL;
	bValid_ = false;

	bValid_ = FALSE;
	bModified_ = FALSE;
    
	ResetAttribs();  
}

// �趨����ͼ�㣬���ڴ�ʱ����Handle������
// 
BOOL kcEntity::SetLayer(kcLayer *pLayer)
{
	_pLayer = pLayer;
	if(pLayer){
		_pModel = pLayer->GetModel();
		if(_pModel){
			_hAISContext = _pModel->GetAISContext();
			
			//����Handle����������
			//���Ѿ��еĲ������ˣ����磺��ȡ��ent�������ٴμ����ent
			if(!KC_IS_VALID_HANDLE(_nHandle)){
				kcHandleMgr *pHandleMgr = _pModel->GetHandleMgr();
				ASSERT(pHandleMgr);
				_nHandle = pHandleMgr->AllocHandle(GetType());
				ASSERT(KC_IS_VALID_HANDLE(_nHandle));

				//��������
				if(_szName[0] == '\0'){
					if(!sNameDesc_.empty()){
						sprintf_s(_szName,KC_ENT_NAME_MAX,"%s %d",sNameDesc_.c_str(),_nHandle);
					}else{
						sprintf_s(_szName,KC_ENT_NAME_MAX,"%s %d",_sTypeName.c_str(),_nHandle);
					}
				}
			}
		}
	}

	return TRUE;
}

// pOwner����ΪNULL
bool kcEntity::SetOwner(kcEntity *pOwner)
{
	pOwner_ = pOwner;
	return true;
}

kcEntity* kcEntity::GetOwner() const
{
	return pOwner_;
}

BOOL kcEntity::Destroy()
{
	DestroyDisplayObj();

	bValid_ = FALSE;
	bModified_ = FALSE;

	return TRUE;
}

bool kcEntity::HasName() const
{
	if(_szName[0] == '\0')
		return false;
	return true;
}

//�趨�������������֣������������
bool kcEntity::SetNameDesc(const char *pszDesc) 
{
	if(pszDesc){
		sNameDesc_ = pszDesc;
		// ����Ҫ��
		if(!sNameDesc_.empty() && sNameDesc_.size() < KC_ENT_NAME_MAX - 8)
			return true;
	}
	sNameDesc_.clear();
	return false;
}

TopoDS_Shape kcEntity::GetShape()
{
	return aShape_;
}

// ����Ĺ������ʾӦ�������ֿ�
// ��ʾ/���أ����ǽ�aisobj����ctx����ʾ������
// ����������ݸı䣬������Ҫ��������ais����.

// ��ʾ����.�������û�ñ仯,��������ʾ
// ��Ҫ����ʾ������ӵ���ʾ��.
// display�������entity�󶨣�undisplay���Ƴ���
//
BOOL  kcEntity::Display(bool bShow,BOOL bUpdateView)
{
	if(!bValid_ || _hAISContext.IsNull() || !_pModel)
		return FALSE;

	// ����ʾ����,Ŀǰ��������ʾ���ݴ�CTx��ȥ��,�Ӷ�����ʾ
	// û�ö���ʾ���ݱ�������ر�Ĵ���.
	if(!bShow){
		if(!_hAISObj.IsNull() && IsDisplayed()){
			_hAISContext->Erase(_hAISObj,bUpdateView ? true : false);
		}
		bValid_ = false;
		return TRUE;
	}

	// �Ƿ����Ϊ��
	BOOL bDone = FALSE;
	if(_hAISObj.IsNull()){
		bDone = BuildDisplayObj();
		// ��һ����ʾ����
		if(bDone){
			_hAISObj->SetEntity(this);//��ʾ�����entity����������
			DoDisplay(bUpdateView);
		}
	}else{
		//���û�иı䣬�����Ѿ���ʾ�����ٴθ��¡�
		BOOL bDisplayed = IsDisplayed();
		if(bDisplayed && !IsModified()){

			return TRUE;
		}

		if(IsModified()){
			bDone = UpdateDisplayObj(bUpdateView);
			if(!bDone)
				return FALSE;
		}

		// ���û����ʾ,����Ҫ��ʾ
		if(!IsDisplayed()){
			bDone = DoDisplay(bUpdateView);
		}else{
			bDone = TRUE;
		}
	}
	//
	bVisible_ = IsDisplayed() ? true : false;
	
	return bDone;
}


// Ϊ�¶���������ʾ����
//
BOOL	kcEntity::BuildDisplayObj()
{
	ASSERT(IsValid());
	ASSERT(_hAISObj.IsNull());

	if(!DoBuildDisplayObj())
		return FALSE;

	// û���޸��ˡ�
	SetModified(FALSE);

	return TRUE;
}

// ������ʾ���󣬵������ı�ʱ�����á�
//
BOOL	kcEntity::UpdateDisplayObj(BOOL bUpdateView)
{
	ASSERT(!_hAISObj.IsNull());

	//�����ʾ,������
	BOOL bDisplayed = IsDisplayed();
	if(bDisplayed)
		Display(FALSE,FALSE);

	//�ͷžɵġ�
	if(!_hAISObj.IsNull()){
		DestroyDisplayObj();
	}

	//�����µ�
	if(!DoBuildDisplayObj())
		return FALSE;

	SetModified(FALSE);

	// ���ԭ����ʾ,��������ʾ
	if(bDisplayed)
		DoDisplay(bUpdateView);

	return TRUE;
}

BOOL  kcEntity::DoBuildDisplayObj()
{
	if(aShape_.IsNull())
		return FALSE;

	_hAISObj = new AIS_EntityShape(aShape_);
	if(!_hAISObj.IsNull()){
		_hAISObj->SetEntity(this);

		return TRUE;
	}

	return FALSE;
}

// �������ʾ����
// �����Ƕ�����AISCTX����ʾ.
//
BOOL  kcEntity::DoDisplay(BOOL bUpdateView)
{
	if(!_hAISObj.IsNull()){
		//��������
		DoSetDisplayAttribs();
		_hAISContext->Display(_hAISObj,bUpdateView ? true : false);
	}

	return TRUE;
}

// ������ʾ��������
void	kcEntity::DoSetDisplayAttribs()
{
	_hAISContext->SetColor(_hAISObj,Quantity_Color(m_color[0],m_color[1],m_color[2],Quantity_TOC_RGB),Standard_False);

	if(fabs(m_dLineWidth - 1.0) > KDBL_MIN)
		_hAISContext->SetWidth(_hAISObj,m_dLineWidth,Standard_False);
	//����?
	if(m_dTransparency < 1.0)//��͸��ֵ
		_hAISContext->SetTransparency(_hAISObj,m_dTransparency,Standard_False);
	//��ʾģʽ
	if(!_hAISObj.IsNull() && _hAISObj->AcceptDisplayMode(m_nDisplayModel)){
		_hAISContext->SetDisplayMode(_hAISObj,m_nDisplayModel,Standard_False);
	}

	DoSetFaceDisplayAttribs();
}

// ����Shell��Solid��Compound�ȶ����е����������ɫ
// ͨ������step�������ģ��򵥶��趨�ġ�
//
void  kcEntity::SetFaceColorAttribs()
{
	ASSERT(_pModel);
	if(aShape_.IsNull()){
		return;
	}

	Quantity_Color aCol;
	TopExp_Explorer exp;
	for(exp.Init(aShape_,TopAbs_FACE);exp.More();exp.Next()){
		const TopoDS_Shape &aS = exp.Current();
		// Ϊ������face�趨��ɫ
		if(_pModel->FindShapeAttrib(aS,aCol)){
			_hAISObj->SetCustomColor(aS,aCol);
		}
	}
}

// �����Ƿ�����ʾ
//
BOOL  kcEntity::IsDisplayed() const
{
	if(_hAISObj.IsNull()) return FALSE;
	return _hAISContext->IsDisplayed(_hAISObj);
}

// ������ʾ����.
//
BOOL	kcEntity::DestroyDisplayObj()
{
	if(!_hAISObj.IsNull()){
		// ������.
		if(_hAISContext->IsDisplayed(_hAISObj))
			_hAISContext->Erase(_hAISObj,Standard_False);

		//�ͷ���ʾ����
		_hAISContext->Remove(_hAISObj,Standard_False);

		KTRACE("\n ais obj refcnt = %d.",_hAISObj->GetRefCount());
		_hAISObj.Nullify();
	}

	return TRUE;
}

BOOL	kcEntity::HasDispalyObj() const
{
	if(_hAISObj.IsNull())
		return FALSE;

	return TRUE;
}

//
void	kcEntity::SetValid(BOOL bValid)
{
	bValid_ = bValid;
}

void	kcEntity::SetModified(BOOL bModified)
{
	bModified_ = bModified;
}

// ���������úͲ�ѯ���Ժ���
//
bool  kcEntity::SetVisible(bool bVisible,BOOL bUpdate)
{
	BOOL bDisplayed = IsDisplayed();
	if((bDisplayed && bVisible) || (!bDisplayed && !bVisible))
		return true;
	
	if(bVisible){
		Display(true,bUpdate);
	}else{
		Display(false,bUpdate);
	}
	bVisible_ = IsDisplayed() ? true : false;
	
	return true;
}

bool  kcEntity::IsVisible()
{
	//����ʵ��״̬����
	return (IsDisplayed() ? true : false);
}

// ���ö������ɫ
void	kcEntity::SetColor(double r,double g,double b,BOOL bUpdate)
{
	K_ROUND(r,0.0,1.0);
	K_ROUND(g,0.0,1.0);
	K_ROUND(b,0.0,1.0);
	m_color.set(r,g,b);
	
	if(!_hAISContext.IsNull() && !_hAISObj.IsNull() && IsDisplayed()){
		Quantity_Color col(r,g,b,Quantity_TOC_RGB);
		_hAISContext->SetColor(_hAISObj,col,bUpdate ? true : false);
	}
}

void kcEntity::SetColor(int icolor,BOOL bUpdate)
{
	double r = GetRValue(icolor) / 255.0;
	double g = GetGValue(icolor) / 255.0;
	double b = GetBValue(icolor) / 255.0;
	SetColor(r,g,b,bUpdate);
}

void kcEntity::GetColor(double& r,double& g,double& b)
{
	r = m_color[0];g = m_color[1];b = m_color[2];
}

Quantity_Color kcEntity::GetColor() const
{
	Quantity_Color aCol(m_color[0],m_color[1],m_color[2],Quantity_TOC_RGB);
	return aCol;
}

bool kcEntity::SetLineStyle(eLineStyle linStyle,BOOL bUpdate)
{
	if(m_eLineStyle == linStyle)
		return true;

	m_eLineStyle = linStyle;

	if(!_hAISContext.IsNull() && !_hAISObj.IsNull() && IsDisplayed()){
		Handle(Prs3d_Drawer) aDrawer = _hAISObj->Attributes();
		Aspect_TypeOfLine aType = (Aspect_TypeOfLine)m_eLineStyle;
		aDrawer->WireAspect()->SetTypeOfLine(aType);
	}
	
	//
	return true;
}

bool	kcEntity::SetLineWidth(double width,BOOL bUpdate)
{
	if(fabs(m_dLineWidth - width) < KDBL_MIN)
		return true;

	m_dLineWidth = width;
	if(!_hAISContext.IsNull() && !_hAISObj.IsNull() && IsDisplayed()){
		_hAISContext->SetWidth(_hAISObj,width,bUpdate ? true : false);
	}

	return true;
}

bool	kcEntity::SetTransparency(double transp,BOOL bUpdate)
{
	K_ROUND(transp,0.0,1.0);
	if(fabs(transp - m_dTransparency) < KDBL_MIN)
		return true;

	m_dTransparency = transp;
	if(!_hAISContext.IsNull() && !_hAISObj.IsNull() && IsDisplayed()){
		_hAISContext->SetTransparency(_hAISObj,transp,bUpdate ? true : false);
	}

	return true;
}

bool	kcEntity::SetDisplayModel(int model,BOOL bUpdate)
{
	if(m_nDisplayModel == model)
		return true;

	m_nDisplayModel = model;
	if(!_hAISContext.IsNull() && !_hAISObj.IsNull() && IsDisplayed()){
		_hAISContext->SetDisplayMode(_hAISObj,model,bUpdate ? true : false);
	}

	return true;
}

void	kcEntity::ResetAttribs()
{
	//Ĭ����ʾ��ɫ
	Quantity_Color col(Quantity_NOC_ORANGE2);
	m_color.set(col.Red(),col.Green(),col.Blue());

	m_eLineStyle = eTOL_SOLID;
	m_dLineWidth = 1.0;
	m_dTransparency = 1.0;//Ĭ��Ϊ1.0,��ʾû��͸������
	m_nDisplayModel = 0;
}

//������������
bool	kcEntity::CloneCommonAttribs(kcEntity *pEntity)
{
	ASSERT(pEntity);
	if(!pEntity) return false;

	//��������
	ResetAttribs();
	//
	pEntity->GetColor(m_color[0],m_color[1],m_color[2]);
	m_eLineStyle = pEntity->GetLineStyle();
	m_dLineWidth = pEntity->GetLineWidth();
	m_dTransparency = pEntity->GetTransparency();
	m_nDisplayModel = pEntity->GetDisplayModel();

	return true;
}


//////////////////////////////////////////////////////////////////////////
//
kcPointEntity::kcPointEntity()
{
	_entType = KCT_ENT_POINT;
	_sTypeName = "Point";
	
	_dPoint[0] = _dPoint[1] = _dPoint[2] = 0.0;
}

kcPointEntity::kcPointEntity(double point[3])
{
	_entType = KCT_ENT_POINT;
	_sTypeName = "Point";
	
	SetPoint(point);
}

kcPointEntity::~kcPointEntity()
{

}

BOOL	kcPointEntity::SetPoint(double point[3])
{
	_dPoint[0] = point[0];
	_dPoint[1] = point[1];
	_dPoint[2] = point[2];

	BuildShape();

	SetValid(TRUE);
	SetModified(TRUE);

	return TRUE;
}

BOOL	kcPointEntity::SetPoint(double x,double y,double z)
{
	_dPoint[0] = x;
	_dPoint[1] = y;
	_dPoint[2] = z;

	BuildShape();

	SetValid(TRUE);
	SetModified(TRUE);

	return TRUE;
}

BOOL  kcPointEntity::BuildShape()
{
	try{
		BRepBuilderAPI_MakeVertex mv(gp_Pnt(_dPoint[0],_dPoint[1],_dPoint[2]));
		if(mv.IsDone()){
			aShape_ = mv.Vertex();
		}else{
			aShape_.Nullify();
		}
	}catch(Standard_Failure){
		aShape_.Nullify();
	}

	return (aShape_.IsNull() ? FALSE : TRUE);
}

BOOL  kcPointEntity::GetPoint(double point[3])
{
	if(!IsValid()) return FALSE;

	point[0] = _dPoint[0];
	point[1] = _dPoint[1];
	point[2] = _dPoint[2];

	return TRUE;
}

//�����Զ���׽����Ϣ
BOOL	kcPointEntity::CalcSnapPoint(kiSnapMngr& snapMgr)
{
	if(!IsValid()) return FALSE;
	kiSnapPoint sp(_dPoint[0],_dPoint[1],_dPoint[2]);
	snapMgr.AddSnapPoint(KSNAP_EXIST_POINT,sp);
	return TRUE;
}

//��������
kcEntity*	kcPointEntity::Clone()
{
	kcPointEntity *pEntity = new kcPointEntity;
	if(!pEntity)
		return NULL;

	pEntity->CloneCommonAttribs(this);
	pEntity->SetPoint(_dPoint);

	return pEntity;
}

////////////////////////////////////////////////////////
//

kcEdgeEntity::kcEdgeEntity(void)
{
	_entType = KCT_ENT_EDGE;
	_sTypeName = "Curve";
}

kcEdgeEntity::~kcEdgeEntity(void)
{
}

//�Ƿ�պ�
bool	kcEdgeEntity::IsClosed()
{
	ASSERT(IsValid());
	if(!IsValid()) return false;
	//
	if(aShape_.Closed())
		return true;
	return false;
}

Handle(Geom_Curve)	kcEdgeEntity::GetCurve()
{
	Handle(Geom_Curve) aCurve;
	if(aShape_.IsNull())
		return aCurve;

	double t0,t1;
	aCurve = BRep_Tool::Curve(TopoDS::Edge(aShape_),t0,t1);
	return aCurve;
}

Handle(Geom_Curve)	kcEdgeEntity::GetCurve(double& t0,double& t1)
{
	Handle(Geom_Curve) aCurve;
	if(aShape_.IsNull())
		return aCurve;

	aCurve = BRep_Tool::Curve(TopoDS::Edge(aShape_),t0,t1);
	return aCurve;
}

BOOL	kcEdgeEntity::SetCurve(const Handle(Geom_Curve)& aCurve)
{
	if(aCurve.IsNull())
		return FALSE;

	aShape_.Nullify();
	try{
		TopoDS_Edge aEdge;
		BRepBuilderAPI_MakeEdge mkEdge(aCurve);
		if(mkEdge.IsDone()){
			aEdge = mkEdge.Edge();
			if(!aEdge.IsNull()){
				aShape_ = aEdge;
			}
		}
	}catch(Standard_Failure){
		return FALSE;
	}

	if(!aShape_.IsNull()){
		SetModified(TRUE);
		SetValid(TRUE);
	}

	return TRUE;
}

BOOL	kcEdgeEntity::SetEdge(const TopoDS_Edge& aEdge)
{
	if(aEdge.IsNull())
		return FALSE;

	aShape_ = aEdge;

	SetModified(TRUE);
	SetValid(TRUE);

	return TRUE;
}

const TopoDS_Edge  kcEdgeEntity::GetEdge() const
{
	TopoDS_Edge aEdge;
	if(!aShape_.IsNull()){
		aEdge = TopoDS::Edge(aShape_);
	}
	return aEdge;
}

//��������
kcEntity*  kcEdgeEntity::Clone()
{
	kcEdgeEntity *pEntity = new kcEdgeEntity;
	if(!pEntity)
		return NULL;

	pEntity->CloneCommonAttribs(this);
	//
	if(!aShape_.IsNull()){
		TopoDS_Edge aEdge = QxBRepLib::CopyEdge(TopoDS::Edge(aShape_));
		if(aEdge.IsNull()){
			delete pEntity;
			return NULL;
		}
		pEntity->SetEdge(aEdge);
	}
	
	return pEntity;
}

//�����Զ���׽����Ϣ
BOOL	kcEdgeEntity::CalcSnapPoint(kiSnapMngr& snapMgr)
{
	if(aShape_.IsNull())
		return FALSE;

	return snapMgr.CalcSnapPoint(TopoDS::Edge(aShape_));
}

//////////////////////////////////////////////////////////////////////////
//
// һ�������
kcWireEntity::kcWireEntity()
{
	_entType = KCT_ENT_WIRE;
	_sTypeName = "Wire";
}

kcWireEntity::~kcWireEntity()
{

}

//�Ƿ�պ�
bool	kcWireEntity::IsClosed()
{
	ASSERT(IsValid());
	if(!IsValid()) return false;

	if(BRep_Tool::IsClosed(aShape_))
		return true;
	return false;
}

BOOL	kcWireEntity::SetWire(const TopoDS_Wire& aWire)
{
	ASSERT(!aWire.IsNull());
	if(aWire.IsNull())
		return FALSE;

	aShape_ = aWire;

	SetModified(TRUE);
	SetValid(TRUE);

	return TRUE;
}

const TopoDS_Wire  kcWireEntity::GetWire() const
{
	TopoDS_Wire aWire;
	if(!aShape_.IsNull()){
		aWire = TopoDS::Wire(aShape_);
	}
	return aWire;
}

//�����Զ���׽����Ϣ
BOOL  kcWireEntity::CalcSnapPoint(kiSnapMngr& snapMgr)
{
	if(aShape_.IsNull())
		return FALSE;

	TopExp_Explorer exp;
	for (exp.Init(aShape_,TopAbs_EDGE); exp.More(); exp.Next()){
		const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
		snapMgr.CalcSnapPoint(E);
	}

	return TRUE;
}


//��������
kcEntity*	kcWireEntity::Clone()
{
	kcWireEntity *pEntity = new kcWireEntity;
	if(!pEntity)
		return NULL;

	pEntity->CloneCommonAttribs(this);
	//
	if(!aShape_.IsNull()){
		TopoDS_Wire aWire = QxBRepLib::CopyWire(TopoDS::Wire(aShape_));
		if(aWire.IsNull()){
			delete pEntity;
			return NULL;
		}
		pEntity->SetWire(aWire);
	}
	
	return pEntity;
}

//////////////////////////////////////////////////////////////////////////
// ����
kcFaceEntity::kcFaceEntity()
{
	_entType = KCT_ENT_FACE;
	_sTypeName = "Surface";
	
	m_nDisplayModel = 1;
}

kcFaceEntity::~kcFaceEntity()
{

}

BOOL	kcFaceEntity::SetSurface(const Handle(Geom_Surface)& aSurf)
{
	ASSERT(!aSurf.IsNull());
	if(aSurf.IsNull())
		return FALSE;

	try{
		BRepBuilderAPI_MakeFace mf(aSurf,Precision::Confusion());
		if(!mf.IsDone())
			return FALSE;

		aShape_ = mf.Face();
	}catch(Standard_Failure){
		return FALSE;
	}

	if(!aShape_.IsNull()){
		SetValid(TRUE);
		SetModified(TRUE);
	}

	return TRUE;
}

BOOL	kcFaceEntity::SetFace(const TopoDS_Face& aFace)
{
	if(aFace.IsNull())
		return FALSE;

	aShape_ = aFace;

	SetValid(TRUE);
	SetModified(TRUE);

	return TRUE;
}

const TopoDS_Face  kcFaceEntity::GetFace() const
{
	TopoDS_Face aFace;
	if(!aShape_.IsNull()){
		aFace = TopoDS::Face(aShape_);
	}
	return aFace;
}

const Handle(Geom_Surface)  kcFaceEntity::GetSurface()
{
	Handle(Geom_Surface) aSurf;
	if(!aShape_.IsNull()){
		TopLoc_Location L;
		aSurf = BRep_Tool::Surface(TopoDS::Face(aShape_),L);
	}
	return aSurf;
}

//�����Զ���׽����Ϣ
BOOL	kcFaceEntity::CalcSnapPoint(kiSnapMngr& snapMgr)
{
	if(aShape_.IsNull())
		return FALSE;

	snapMgr.CalcSnapPoint(TopoDS::Face(aShape_));

	return TRUE;
}


//��������
kcEntity*	kcFaceEntity::Clone()
{
	if(aShape_.IsNull())
		return NULL;

	kcFaceEntity *pEntity = new kcFaceEntity;
	if(!pEntity)
		return NULL;

	pEntity->CloneCommonAttribs(this);
	//
	if(!aShape_.IsNull()){
		TopoDS_Face aFace = QxBRepLib::CopyFace(TopoDS::Face(aShape_));
		if(aFace.IsNull()){
			delete pEntity;
			return NULL;
		}
		pEntity->SetFace(aFace);
	}

	return pEntity;
}

//////////////////////////////////////////////////////////////////////////
// shell
kcShellEntity::kcShellEntity()
{
	_entType = KCT_ENT_SHELL;
	_sTypeName = "Shell";
	
	m_nDisplayModel = 1;
}

kcShellEntity::~kcShellEntity()
{

}

BOOL  kcShellEntity::SetShell(const TopoDS_Shell& aShell)
{
	if(aShell.IsNull())
		return FALSE;

	aShape_ = aShell;

	SetValid(TRUE);
	SetModified(TRUE);

	return TRUE;
}

const TopoDS_Shell  kcShellEntity::GetShell() const
{
	TopoDS_Shell aShell;
	if(!aShape_.IsNull()){
		aShell = TopoDS::Shell(aShape_);
	}
	return aShell;
}

//�����Զ���׽����Ϣ
BOOL  kcShellEntity::CalcSnapPoint(kiSnapMngr& snapMgr)
{
	if(!aShape_.IsNull())
		snapMgr.CalcSnapPoint(aShape_);
	return TRUE;
}

//��������
kcEntity*  kcShellEntity::Clone()
{
	if(aShape_.IsNull())
		return FALSE;

	kcShellEntity *pEntity = new kcShellEntity;
	if(!pEntity)
		return NULL;

	pEntity->CloneCommonAttribs(this);
	//
	if(!aShape_.IsNull()){
		TopoDS_Shape aS = QxBRepLib::CopyShape(aShape_);
		if(aS.IsNull() || aS.ShapeType() != TopAbs_SHELL){
			delete pEntity;
			return NULL;
		}
		pEntity->SetShell(TopoDS::Shell(aS));
	}

	return pEntity;
}

void  kcShellEntity::DoSetFaceDisplayAttribs()
{
	SetFaceColorAttribs();
}

//////////////////////////////////////////////////////////////////////////
//��ʵ��
kcSolidEntity::kcSolidEntity()
{
	_entType = KCT_ENT_SOLID;
	_sTypeName = "Solid";
	
	m_nDisplayModel = 1;
}

kcSolidEntity::~kcSolidEntity()
{

}

BOOL	kcSolidEntity::SetSolid(const TopoDS_Solid& aSolid)
{
	if(aSolid.IsNull())
		return FALSE;

	aShape_ = aSolid;

	SetValid(TRUE);
	SetModified(TRUE);

	return TRUE;
}

const TopoDS_Solid  kcSolidEntity::GetSolid() const
{
	TopoDS_Solid aSolid;
	if(!aShape_.IsNull()){
		aSolid = TopoDS::Solid(aShape_);
	}
	return aSolid;
}

//�����Զ���׽����Ϣ
BOOL  kcSolidEntity::CalcSnapPoint(kiSnapMngr& snapMgr)
{
	if(!aShape_.IsNull())
		snapMgr.CalcSnapPoint(aShape_);
	return TRUE;
}

//��������
kcEntity*  kcSolidEntity::Clone()
{
	if(aShape_.IsNull())
		return NULL;

	kcSolidEntity *pEntity = new kcSolidEntity;
	if(!pEntity)
		return NULL;

	pEntity->CloneCommonAttribs(this);
	//
	if(!aShape_.IsNull()){
		TopoDS_Shape aS = QxBRepLib::CopyShape(aShape_);
		if(aS.IsNull() || aS.ShapeType() != TopAbs_SOLID){
			delete pEntity;
			return NULL;
		}
		pEntity->SetSolid(TopoDS::Solid(aS));
	}

	return pEntity;
}

void  kcSolidEntity::DoSetFaceDisplayAttribs()
{
	SetFaceColorAttribs();
}

//////////////////////////////////////////////////////////////////////////
//��ϣ�����������ϡ�
kcCompoundEntity::kcCompoundEntity()
{
	_entType = KCT_ENT_COMPOUND;
	_sTypeName = "Compound";

	m_nDisplayModel = 1;
}

kcCompoundEntity::~kcCompoundEntity()
{

}

BOOL	kcCompoundEntity::SetCompound(const TopoDS_Compound& aComp)
{
	if(aComp.IsNull())
		return FALSE;

	aShape_ = aComp;
	
	SetValid(TRUE);
	SetModified(TRUE);

	return TRUE;
}

const TopoDS_Compound  kcCompoundEntity::GetCompound() const
{
	TopoDS_Compound aComp;
	if(!aShape_.IsNull()){
		aComp = TopoDS::Compound(aShape_);
	}
	return aComp;
}

//�����Զ���׽����Ϣ
BOOL	kcCompoundEntity::CalcSnapPoint(kiSnapMngr& snapMgr)
{
	if(!aShape_.IsNull())
		snapMgr.CalcSnapPoint(aShape_);
	return TRUE;
}


//��������
kcEntity*  kcCompoundEntity::Clone()
{
	if(aShape_.IsNull())
		return NULL;

	kcCompoundEntity *pEntity = new kcCompoundEntity;
	if(!pEntity)
		return NULL;

	pEntity->CloneCommonAttribs(this);
	//
	if(!aShape_.IsNull()){
		TopoDS_Shape aS = QxBRepLib::CopyShape(aShape_);
		if(aS.IsNull() || aS.ShapeType() != TopAbs_COMPOUND){
			delete pEntity;
			return NULL;
		}
		pEntity->SetCompound(TopoDS::Compound(aS));
	}

	return pEntity;
}


void  kcCompoundEntity::DoSetFaceDisplayAttribs()
{
	SetFaceColorAttribs();
}

///////////////////////////////////////////////////////////////////////////////
//
kcBlockEntity::kcBlockEntity()
{
	_entType = KCT_ENT_BLOCK;
	_sTypeName = "Block";

	nReadEntNum_ = 0;
}

kcBlockEntity::~kcBlockEntity()
{
}

TopoDS_Shape kcBlockEntity::GetShape()
{
	return aComp_;
}

// ���һ�����󣬶��󽫼�¼�ڸö����ڲ�
bool kcBlockEntity::AddEntity(kcEntity *pEntity)
{
	if(!pEntity || aEntMap_.find(pEntity) != aEntMap_.end()){
		return false;
	}

	TopoDS_Shape aS = pEntity->GetShape();
	if(aS.IsNull()){
		ASSERT(FALSE);
		return false;
	}

	BRep_Builder aBB;
	if(aComp_.IsNull()){
		aBB.MakeCompound(aComp_);
		aShape_ = aComp_;
	}
	aBB.Add(aComp_,aS);

	pEntity->SetOwner(this); //��¼��������
	//��¼����
	aEntList_.push_back(pEntity);
	aEntMap_.insert(kcEntityMap::value_type(pEntity,0));
	SetModified(TRUE); //�����˸ı�
	SetValid(TRUE);

	return true;
}

const kcEntityList& kcBlockEntity::GetEntityList() const
{
	return aEntList_;
}

BOOL  kcBlockEntity::DoBuildDisplayObj()
{
	if(aComp_.IsNull())
		return FALSE;

	_hAISObj = new AIS_EntityShape(aComp_);
	if(!_hAISObj.IsNull()){
		_hAISObj->SetEntity(this);
		//��Ҫ������ɫ��������Ϣ
		//��Ҫ���ǵ��ݹ�Ĵ���
		kcEntityList::iterator ite = aEntList_.begin();
		for(;ite != aEntList_.end();++ite){
			DoSetEntityAttribs(*ite);
		}

		return TRUE;
	}

	return FALSE;
}

void kcBlockEntity::DoSetDisplayAttribs()
{
	// ���ﲻ�����κ����飬��Ҫʹ�ø����Ӷ��������
}

// �����Ҫ����Ƕ�׵����
void kcBlockEntity::DoSetEntityAttribs(kcEntity *pEnt)
{
	if(IS_BLOCK_ENTITY(pEnt)){
		kcBlockEntity *pBlkEnt = (kcBlockEntity *)pEnt;
		const kcEntityList &aEntList = pBlkEnt->GetEntityList();
		kcEntityList::const_iterator ite = aEntList.begin();
		for(;ite != aEntList.end();++ite){
			DoSetEntityAttribs(*ite);
		}
	}else{
		TopoDS_Shape aS = pEnt->GetShape();
		// ��ɫ
		Quantity_Color aColor = pEnt->GetColor();
		_hAISObj->SetCustomColor(aS,aColor);
		// TODO:��������
	}
}

//�����Զ���׽����Ϣ
BOOL kcBlockEntity::CalcSnapPoint(kiSnapMngr& snapMgr)
{
	return TRUE;
}

//��������
kcEntity* kcBlockEntity::Clone()
{
	ASSERT(FALSE);
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//

kcDimEntity::kcDimEntity()
{
	}

kcDimEntity::~kcDimEntity()
{
}

void kcDimEntity::SetFlyout(double dValue)
{
	dFlyout_ = dValue;
	SetModified(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
//

kcLengthDimEntity::kcLengthDimEntity()
{
	_entType = KCT_ENT_DIM_LENGTH;
	_sTypeName = "LengthDim";
}

kcLengthDimEntity::~kcLengthDimEntity()
{
}

///////////////////////////////////////////////////////////////////////////////
//

kcDiameterDimEntity::kcDiameterDimEntity()
{
	_entType = KCT_ENT_DIM_DIAMETER;
	_sTypeName = "DiameterDim";
}

kcDiameterDimEntity::~kcDiameterDimEntity()
{
}

///////////////////////////////////////////////////////////////////////////////
//

kcRadiusDimEntity::kcRadiusDimEntity()
{
	_entType = KCT_ENT_DIM_RADIUS;
	_sTypeName = "RadiusDim";
}

kcRadiusDimEntity::~kcRadiusDimEntity()
{
}

