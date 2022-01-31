#include "stdafx.h"
#include "kBase.h"
#include "kPoint.h"
#include "kVector.h"
#include "kiInputDirTool.h"
#include "kiInputValueTool.h"
#include "kiInputPointTool.h"
#include "kiInputEntityTool.h"
#include "kcEntity.h"
#include "kcModel.h"
#include "kcBasePlane.h"
#include "kcmBasePlane.h"


///////////////////////////////////////////////////////
//
kcmOffsetBasePlane::kcmOffsetBasePlane()
{
	m_strName = "OffsetBasePlane";
	m_strAlias = "Obpln";
	m_strDesc = "��׼��ƫ��";

	_pInputDirTool = NULL;
	_pInputOffsetTool = NULL;
	_offDire.set(0,0,1);
	_dOffset = 20;
}

kcmOffsetBasePlane::~kcmOffsetBasePlane()
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL  kcmOffsetBasePlane::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int  kcmOffsetBasePlane::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == _pInputDirTool){
		if(_pInputDirTool->IsInputDone()){

			NavToNextTool();
		}
	}else if(pTool == _pInputOffsetTool){
		if(_pInputOffsetTool->IsInputDone()){

			if(CreateBasePlane()){
				Done();
				Redraw();

				return KSTA_DONE;
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
BOOL  kcmOffsetBasePlane::CreateInputTools()
{
	_pInputDirTool = new kiInputDirTool(this,"ƫ�Ʒ���",NULL);
	_pInputDirTool->Initialize(&_offDire,false,false,true);

	_pInputOffsetTool = new kiInputDoubleTool(this,"ƫ�ƾ���",NULL);
	_pInputOffsetTool->Init(&_dOffset);

	return TRUE;
}

// ���ٴ��������빤��.ÿ�����������һ��.
BOOL  kcmOffsetBasePlane::DestroyInputTools()
{
	KC_SAFE_DELETE(_pInputDirTool);
	KC_SAFE_DELETE(_pInputOffsetTool);

	return TRUE;
}

// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
BOOL  kcmOffsetBasePlane::InitInputToolQueue()
{
	AddInputTool(_pInputDirTool);
	AddInputTool(_pInputOffsetTool);

	return TRUE;
}

BOOL  kcmOffsetBasePlane::CreateBasePlane()
{
	kcModel *pModel = GetModel();
	Handle(V3d_View) hView = GetCurrV3dView();
	kcBasePlane *pCurBPln = GetCurrentBasePlane();
	kvCoordSystem cs;
	kPoint3 org,norg;

	pCurBPln->GetCoordSystem(cs);
	org = cs.Org();
	norg = org + _offDire * _dOffset;
	cs.Set(norg,cs.X(),cs.Y(),cs.Z());

	kcBasePlane *pNewBPln = new kcBasePlane(hView);
	pNewBPln->Create(cs,pModel);
	pModel->AddBasePlane(pNewBPln);

	return TRUE;
}

////////////////////////////////////////////////////////////
//
///�������ɻ�׼��
kcmThreePntBasePlane::kcmThreePntBasePlane()
{
	m_strName = "ThreePntBasePlane";
	m_strAlias = "TPtPln";
	m_strDesc = "�������ɻ�׼��";

	pInputPointTool1_ = NULL;
	pInputPointTool2_ = NULL;
	pInputPointTool3_ = NULL;
}

kcmThreePntBasePlane::~kcmThreePntBasePlane()
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL  kcmThreePntBasePlane::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int  kcmThreePntBasePlane::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == pInputPointTool1_){
		if(pTool->IsInputDone()){
			NavToNextTool();
		}else{
			return KSTA_CONTINUE;
		}
	}else if(pTool == pInputPointTool2_){
		if(pTool->IsInputDone()){
			NavToNextTool();
		}else{
			return KSTA_CONTINUE;
		}
	}else if(pTool == pInputPointTool3_){
		if(pTool->IsInputDone()){
			if(CreateBasePlane()){
				Done();
				return KSTA_DONE;
			}else{
				return KSTA_CONTINUE;
			}
		}else{
			return KSTA_CONTINUE;
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
BOOL  kcmThreePntBasePlane::CreateInputTools()
{
	pInputPointTool1_ = new kiInputPointTool(this,"��һ��:");
	pInputPointTool1_->Initialize(&pnt1_);

	pInputPointTool2_ = new kiInputPointTool(this,"�ڶ���:");
	pInputPointTool2_->Initialize(&pnt2_);

	pInputPointTool3_ = new kiInputPointTool(this,"������:");
	pInputPointTool3_->Initialize(&pnt3_);

	return TRUE;
}
	
// ���ٴ��������빤��.ÿ�����������һ��.
BOOL  kcmThreePntBasePlane::DestroyInputTools()
{
	KC_SAFE_DELETE(pInputPointTool1_);
	KC_SAFE_DELETE(pInputPointTool2_);
	KC_SAFE_DELETE(pInputPointTool3_);
	return TRUE;
}
	
// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
BOOL  kcmThreePntBasePlane::InitInputToolQueue()
{
	AddInputTool(pInputPointTool1_);
	AddInputTool(pInputPointTool2_);
	AddInputTool(pInputPointTool3_);
	return TRUE;
}

BOOL  kcmThreePntBasePlane::CreateBasePlane()
{
	kcModel *pModel = GetModel();
	Handle(V3d_View) hView = GetCurrV3dView();
	kvCoordSystem cs;
	
	if(!cs.Create(pnt1_,pnt2_,pnt3_))
		return FALSE;

	kcBasePlane *pNewBPln = new kcBasePlane(hView);
	pNewBPln->Create(cs,pModel);
	pModel->AddBasePlane(pNewBPln);

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
kcmBasePlaneByFace::kcmBasePlaneByFace()
{
	InitSet("PlaneFaceBasePlane","PFBasePlane","��Face����һ�����ɻ�׼��");

	pFacePickTool_ = NULL;
}

kcmBasePlaneByFace::~kcmBasePlaneByFace()
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL kcmBasePlaneByFace::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int kcmBasePlaneByFace::OnInputFinished(kiInputTool *pTool)
{
	if(pFacePickTool_->IsInputDone()){
		if(CreateBasePlane()){
			Done();
			return KSTA_DONE;
		}else{
			pFacePickTool_->End();

			NavToInputTool(pFacePickTool_);
		}
	}
	return KSTA_CONTINUE;
}

int kcmBasePlaneByFace::OnExecute()
{
	if(pFacePickTool_){
		pFacePickTool_->AddInitSelected(true);
		//
		if(pFacePickTool_->IsInputDone()){
			if(CreateBasePlane()){
				pFacePickTool_->ClearSel();

				Redraw();

				return KSTA_DONE;
			}
		}
	}
	return kiCommand::OnExecute();
}

// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
BOOL kcmBasePlaneByFace::CreateInputTools()
{
	pFacePickTool_ = new kiInputEntityTool(this,"ʰȡһ��Face:");
	pFacePickTool_->SetOption(KCT_ENT_FACE, 1);
	pFacePickTool_->SetNaturalMode(false); //����ѡȡ�ֲ�����

	return TRUE;
}
	
// ���ٴ��������빤��.ÿ�����������һ��.
BOOL kcmBasePlaneByFace::DestroyInputTools()
{
	KC_SAFE_DELETE(pFacePickTool_);

	return TRUE;
}
	
// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
BOOL kcmBasePlaneByFace::InitInputToolQueue()
{
	AddInputTool(pFacePickTool_);

	return TRUE;
}

BOOL kcmBasePlaneByFace::CreateBasePlane()
{
	if(pFacePickTool_->GetSelCount() != 1)
		return FALSE;

	kiSelEntity se = pFacePickTool_->GetFirstSelect();
	TopoDS_Shape aS = se.SelectShape();
	TopoDS_Face aFace = TopoDS::Face(aS);
	//
	const Handle(Geom_Surface) &aSurf = BRep_Tool::Surface(aFace);
	if(aSurf.IsNull()){
		return FALSE;
	}

	Handle(Geom_Surface) aBasSurf = aSurf;
	if(aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))){
		Handle(Geom_RectangularTrimmedSurface) aTrmSurf = Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurf);
		aBasSurf = aTrmSurf->BasisSurface();
	}else if(aSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface))){
		Handle(Geom_OffsetSurface) aOffSurf = Handle(Geom_OffsetSurface)::DownCast(aSurf);
		aBasSurf = aOffSurf->BasisSurface();
	}
	kcModel *pModel = GetModel();
	Handle(V3d_View) hView = GetCurrV3dView();
	kvCoordSystem cs;
	kcBasePlane *pNewBasePlane = NULL;
	
	//
	if(aBasSurf->IsKind(STANDARD_TYPE(Geom_Plane))){//ƽ��
		Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aBasSurf);
		gp_Pln pln = aPlane->Pln();
		gp_Ax3 ax3 = pln.Position();

		if(aFace.Orientation() == TopAbs_FORWARD){
			cs.Set(pln.Position().Ax2());
		}else{
			gp_Dir zD = ax3.Direction();
			zD.Reverse(); //����
			cs.Set(ax3.Location(),ax3.XDirection(),zD);
		}

		pNewBasePlane = new kcBasePlane(hView);
		pNewBasePlane->Create(cs,pModel);
		pModel->AddBasePlane(pNewBasePlane);
	}else{
	}
	
	return TRUE;
}

