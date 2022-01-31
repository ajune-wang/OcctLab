#include "StdAfx.h"
#include <vector>
#include <string>
#include <BRepCheck_Face.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GeomLProp_CLProps.hxx>
#include "kcEntity.h"
#include "kcModel.h"
#include "kcDispArrow.h"
#include "kcPreviewObj.h"
#include "kiInputEntityTool.h"
#include "QxCurveLib.h"
#include "QxSurfLib.h"
#include "kcgAnalysis.h"
#include "QxBaseUtils.h"
#include "QxBRepLib.h"
#include "kcStepReader.h"
#include "QxSysLib.h"
#include "kcShapeAttribMgr.h"
#include "kcProgressThread.h"
#include "kcmShapeAnalysis.h"

kcmShapeAnalysisNormal::kcmShapeAnalysisNormal(void)
{
	m_strName = "��������";
	m_strAlias = "AnayNorm";
	m_strDesc = "����������������ķ���";

	m_pInputEntity = NULL;
	m_bTopoNormal = true;	
}

kcmShapeAnalysisNormal::~kcmShapeAnalysisNormal(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmShapeAnalysisNormal::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmShapeAnalysisNormal::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntity){

		CalcNormal();

		RequestEditInput("ѡ��ͬ������ʾ��ʧ����",&m_optionSet);
	}
	return KSTA_CONTINUE;
}

//ѡ�ѡ����
void kcmShapeAnalysisNormal::OnOptionSelected(kiOptionItem *pOptItem)
{
	int nID = pOptItem->GetID();
	switch(nID){
	case 1://quit
		{
			m_pInputEntity->End();
			m_pInputEntity->ClearSel();

			Done();
		}
		break;
	default:
		break;
	}
}

BOOL kcmShapeAnalysisNormal::CalcNormal()
{
	ClearArrowDisp();
	if(m_pInputEntity->GetSelCount() != 1)
		return FALSE;

	kiSelEntity se = m_pInputEntity->GetFirstSelect();
	TopoDS_Shape aS = se.SelectShape();

	TopoDS_Face aFace;
	TopExp_Explorer ex;
	kcDispSolidArrow *pArrow = NULL;
	kPoint3 p,p2;
	kVector3 norm;
	kColor aColor(1.0,0.0,1.0);

	for(ex.Init(aS,TopAbs_FACE);ex.More();ex.Next()){

		aFace = TopoDS::Face(ex.Current());
		if(QxSurfLib::CalcFaceNormal(aFace,m_bTopoNormal,p,norm)){
			norm.normalize();

			pArrow = new kcDispSolidArrow(GetAISContext(),GetDocContext());
			pArrow->SetColor(aColor);
			pArrow->Init(p,norm,20);
			pArrow->Display(TRUE);

			m_arrowList.push_back(pArrow);
		}
	}

	return TRUE;
}

int kcmShapeAnalysisNormal::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmShapeAnalysisNormal::OnEnd(int nCode)
{
	ClearArrowDisp();
	return nCode;
}

void	kcmShapeAnalysisNormal::ClearArrowDisp()
{
	int ix,isize = (int)m_arrowList.size();
	for(ix = 0;ix < isize;ix ++)
		delete m_arrowList[ix];
	m_arrowList.clear();
}

// ������Ҫ�����빤��
BOOL	kcmShapeAnalysisNormal::CreateInputTools()
{
	m_optionSet.AddBoolOption("���˷���",'T',m_bTopoNormal);

	kiOptionButton *pButton = new kiOptionButton("�˳�",'Q',1);
	pButton->SetOptionCallBack(this);
	m_optionSet.AddOption(pButton);

	m_pInputEntity = new kiInputEntityTool(this,"ʰȡFace/Shell/Solid/Compound����",&m_optionSet);
	m_pInputEntity->SetOption(KCT_ENT_FACE | KCT_ENT_SHELL | KCT_ENT_SOLID | KCT_ENT_COMPOUND,false);
	m_pInputEntity->SetNaturalMode(true);
	
	return TRUE;
}

BOOL	kcmShapeAnalysisNormal::DestroyInputTools()
{
	m_optionSet.Clear();
	KC_SAFE_DELETE(m_pInputEntity);

	return TRUE;
}

BOOL	kcmShapeAnalysisNormal::InitInputToolQueue()
{
	AddInputTool(m_pInputEntity);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
kcmShapeAnalysisFaceOrient::kcmShapeAnalysisFaceOrient(void)
{
	m_strName = "���淽��";
	m_strAlias = "FaceOrit";
	m_strDesc = "���漰�价�ķ���";

	m_pInputEntity = NULL;
	m_pInputFace = NULL;

	m_bTopoNormal = false;
	m_bUseWireOrit = true;
	m_bCorrectWireOrder = false;
}

kcmShapeAnalysisFaceOrient::~kcmShapeAnalysisFaceOrient(void)
{

}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmShapeAnalysisFaceOrient::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmShapeAnalysisFaceOrient::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntity)
	{
		if(m_pInputEntity->IsInputDone())
		{
			kiSelEntity se = m_pInputEntity->GetFirstSelect();
			m_pInputFace->SetGlobalAISObject(se.AISObject());
			

			NavToNextTool();
		}
	}
	else if(pTool == m_pInputFace)
	{
		if(m_pInputFace->IsInputDone())
		{
			NavToNextTool();

			ClearArrowDisp();
			ShowFaceWiresAndNorm();
		}
	}
	return KSTA_CONTINUE;
}

BOOL  kcmShapeAnalysisFaceOrient::ShowFaceWiresAndNorm()
{
	TopoDS_Face aFace;
	
	int ix,nbShape = m_pInputFace->GetSelCount();
	for(ix = 0;ix < nbShape;ix ++){
		aFace = TopoDS::Face(m_pInputFace->SelectedShape(ix));

		ShowWiresAndNorm(aFace);
		//BRepCheck_Face chkFace(aFace);
		//chkFace.OrientationOfWires();
		//
	}

	return TRUE;
}

// ��ʾ��ķ�ʧ���ͻ��ķ���
//
BOOL  kcmShapeAnalysisFaceOrient::ShowWiresAndNorm(const TopoDS_Face& aFace)
{
	kcDispSolidArrow *pArrow = NULL;
	kPoint3 p;
	kVector3 norm;

	//��ʾnormal
	if(QxSurfLib::CalcFaceNormal(aFace,m_bTopoNormal,p,norm)){
		norm.normalize();

		pArrow = new kcDispSolidArrow(GetAISContext(),GetDocContext());
		pArrow->Init(p,norm,20);
		pArrow->SetColor(kColor(1.0,0.0,1.0));
		pArrow->Display(TRUE);

		m_arrowList.push_back(pArrow);
	}

	// calc wire
	TopoDS_Iterator ite;
	for(ite.Initialize(aFace,m_bTopoNormal);ite.More();ite.Next())//���������face�ķ���
	{
		TopoDS_Wire aWire = TopoDS::Wire(ite.Value());
		if(!m_bUseWireOrit){
			TopoDS_Shape aS = aWire.Oriented(TopAbs_FORWARD);
			aWire = TopoDS::Wire(aS);
		}

		if(m_bCorrectWireOrder){
			Handle(ShapeExtend_WireData) aWireData = new ShapeExtend_WireData(aWire);
			int ix,nbEdges = aWireData->NbEdges();
			for(ix = 1;ix <= nbEdges;ix ++){
				TopoDS_Edge aE = aWireData->Edge(ix);

				if(QxCurveLib::CalcTangent(aE,aFace,true,p,norm)){
					norm.normalize();

					double len = QxBRepLib::EdgeLength(aE);

					pArrow = new kcDispSolidArrow(GetAISContext(),GetDocContext());
					pArrow->Init(p,norm,len * 0.3);
					pArrow->SetColor(kColor(0.0,0.0,1.0));
					pArrow->Display(TRUE);

					m_arrowList.push_back(pArrow);
				}
			}
		}else{
			TopoDS_Iterator wite;
			for(wite.Initialize(aWire);wite.More();wite.Next()){
				TopoDS_Edge aE = TopoDS::Edge(wite.Value());

				if(QxCurveLib::CalcTangent(aE,aFace,true,p,norm)){
					norm.normalize();

					double len = QxBRepLib::EdgeLength(aE);

					pArrow = new kcDispSolidArrow(GetAISContext(),GetDocContext());
					pArrow->Init(p,norm,len * 0.3);
					pArrow->SetColor(kColor(0.0,0.0,1.0));
					pArrow->Display(TRUE);

					m_arrowList.push_back(pArrow);
				}
			}
		}
	}

	return TRUE;
}

int  kcmShapeAnalysisFaceOrient::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmShapeAnalysisFaceOrient::OnEnd(int nCode)
{
	ClearArrowDisp();
	return nCode;
}

void	kcmShapeAnalysisFaceOrient::ClearArrowDisp()
{
	int ix,isize = (int)m_arrowList.size();
	for(ix = 0;ix < isize;ix ++)
		delete m_arrowList[ix];
	m_arrowList.clear();
}

// ������Ҫ�����빤��
BOOL	kcmShapeAnalysisFaceOrient::CreateInputTools()
{
	m_optionSet.AddBoolOption("���˷���",'T',m_bTopoNormal);
	m_optionSet.AddBoolOption("���ǻ�����",'O',m_bUseWireOrit);
	m_optionSet.AddBoolOption("���ı�����",'C',m_bCorrectWireOrder);

	m_pInputEntity = new kiInputEntityTool(this,"ѡ�����",&m_optionSet);
	m_pInputEntity->SetOption(KCT_ENT_SHELL | KCT_ENT_SOLID,1);

	m_pInputFace = new kiInputEntityTool(this,"ѡ������");
	m_pInputFace->SetOption(KCT_ENT_FACE, 1);
	m_pInputFace->SetNaturalMode(false);
	
	return TRUE;
}

BOOL	kcmShapeAnalysisFaceOrient::DestroyInputTools()
{
	m_optionSet.Clear();
	KC_SAFE_DELETE(m_pInputEntity);
	KC_SAFE_DELETE(m_pInputFace);

	return TRUE;
}

BOOL	kcmShapeAnalysisFaceOrient::InitInputToolQueue()
{
	AddInputTool(m_pInputEntity);
	AddInputTool(m_pInputFace);

	return TRUE;
}

////////////////////////////////////////////////////////////////////
//��ʾ�������ϸ��Ϣ
kiFacePropInfo::kiFacePropInfo()
{
	m_strName = "����ṹ��Ϣ";
	m_strAlias = "FacePropInfo";
	m_strDesc = "������ϸ�ṹ��������Ϣ";

	m_pInputFace = NULL;
}

kiFacePropInfo::~kiFacePropInfo()
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kiFacePropInfo::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kiFacePropInfo::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputFace)
	{
		if(ShowFacePropInfo()){
			Done();
			return KSTA_DONE;
		}else{
			EndCommand(KSTA_CANCEL);
			return KSTA_CANCEL;
		}
	}
	return KSTA_CONTINUE;
}

BOOL	kiFacePropInfo::ShowFacePropInfo()
{
	TopoDS_Face aFace;

	if(m_pInputFace->IsInputDone()){
		int nbShape = m_pInputFace->GetSelCount();
		if(nbShape > 0){
			aFace = TopoDS::Face(m_pInputFace->SelectedShape(0));
		}
	}else{
		//�ж��Ƿ�ȫ��ѡ���ж���
		kiSelSet *pSelSet = GetGlobalSelSet();
		if(pSelSet->GetSelCount() > 0){
			pSelSet->InitSelected();
			kiSelEntity se = pSelSet->CurSelected();
			TopoDS_Shape aShape = se._pEntity->GetShape();
			if(aShape.ShapeType() == TopAbs_FACE){
				aFace = TopoDS::Face(aShape);
			}
		}
	}

	if(aFace.IsNull())
		return FALSE;

#ifdef _V_DEBUG
	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
	if(aSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface))){
		double U1,U2,V1,V2,du,dv;
		aSurf->Bounds(U1,U2,V1,V2);
		TRACE("\n bound U1=%f,U2=%f,V1=%f,V2=%f,",U1,U2,V1,V2);
		Handle(Geom_BSplineSurface) aSurf2 = Handle(Geom_BSplineSurface)::DownCast(aSurf->Copy());
		du = 0.1 * (U2 - U1);
		dv = 0.1 * (V2 - V1);
		aSurf2->Segment(U1 + du,U2 - du,V1 + dv,V2 - dv);
		aSurf2->Bounds(U1,U2,V1,V2);
		TRACE("\n bound U1=%f,U2=%f,V1=%f,V2=%f,",U1,U2,V1,V2);
	}
#endif

	ShowShapeStructDialog(aFace,bCumOri_);
	
	return TRUE;
}

int	kiFacePropInfo::OnExecute()
{
	return KSTA_CONTINUE;
}

int	kiFacePropInfo::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kiFacePropInfo::CreateInputTools()
{
	m_optionSet.AddBoolOption("���Ǹ�����",'C',bCumOri_);

	m_pInputFace = new kiInputEntityTool(this,"ѡ������",&m_optionSet);
	m_pInputFace->SetOption(KCT_ENT_FACE,1);
	m_pInputFace->SetNaturalMode(false);

	return TRUE;
}

BOOL	kiFacePropInfo::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputFace);
	return TRUE;
}

BOOL	kiFacePropInfo::InitInputToolQueue()
{
	AddInputTool(m_pInputFace);
	return TRUE;
}

////////////////////////////////////////////////////////////////////
//��ʾ�������ϸ��Ϣ
kiFaceWireOri::kiFaceWireOri()
{
	m_strName = "����ü�������";
	m_strAlias = "FaceWireOri";
	m_strDesc = "����ü���������Ϣ";

	m_pInputFace = NULL;
	bCumFaceOri_ = false;
	bCumEdgeOri_ = true;
}

kiFaceWireOri::~kiFaceWireOri()
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kiFaceWireOri::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kiFaceWireOri::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputFace)
	{
		NavToNextTool();

		ShowFaceWireOri();
	}
	return KSTA_CONTINUE;
}

BOOL  kiFaceWireOri::ShowFaceWireOri()
{
	TopoDS_Face aFace;

	if(m_pInputFace->IsInputDone()){
		int nbShape = m_pInputFace->GetSelCount();
		if(nbShape > 0){
			aFace = TopoDS::Face(m_pInputFace->SelectedShape(0));
		}
	}else{
		//�ж��Ƿ�ȫ��ѡ���ж���
		kiSelSet *pSelSet = GetGlobalSelSet();
		if(pSelSet->GetSelCount() > 0){
			pSelSet->InitSelected();
			kiSelEntity se = pSelSet->CurSelected();
			TopoDS_Shape aShape = se._pEntity->GetShape();
			if(aShape.ShapeType() == TopAbs_FACE){
				aFace = TopoDS::Face(aShape);
			}
		}
	}

	if(aFace.IsNull())
		return FALSE;

	TopoDS_Face aFF = aFace;
	if(!bCumFaceOri_)
		aFF.Orientation(TopAbs_FORWARD);

	BuildSurfNormArrow(aFace);

	//����Wire
	TopoDS_Iterator wite(aFF);
	for(;wite.More();wite.Next()){
		const TopoDS_Wire& aWire = TopoDS::Wire(wite.Value());
		//����ÿ��Edge
		TopoDS_Iterator eit(aWire);
		for(;eit.More();eit.Next()){
			const TopoDS_Edge& aE = TopoDS::Edge(eit.Value());

			BuildDispArrow(aE,aFF,bCumEdgeOri_);
		}
	}

	return TRUE;
}

// ����face������ķ�ʧ������arrow
void  kiFaceWireOri::BuildSurfNormArrow(const TopoDS_Face& aFace)
{
	double umin,umax,vmin,vmax,u,v;
	BRepTools::UVBounds(aFace,umin,umax,vmin,vmax);
	u = (umin + umax) / 2;
	v = (vmin + vmax) / 2;

	BRepAdaptor_Surface bapSurf(aFace);
	gp_Pnt pnt;
	gp_Vec du1,dv1,norm;

	bapSurf.D1(u,v,pnt,du1,dv1);
	norm = du1.Crossed(dv1);
	norm.Normalize();

	kcDispSolidArrow *pArrow = BuildDispArrow(pnt,norm,20,kColor(0,1.0,1.0));
	if(pArrow){
		pArrow->Display(TRUE);

		sldArrowVec_.push_back(pArrow);
	}
}

void  kiFaceWireOri::BuildDispArrow(const TopoDS_Edge& aE,const TopoDS_Face& aF,bool bCumEdgeOri)
{
	double dF,dL,dm;
	gp_Pnt pnt;
	gp_Vec Tg;

	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aE,dF,dL);
	if(aCurve.IsNull())
		return;

	dm = (dF + dL) / 2;
	aCurve->D1(dm,pnt,Tg);
	double len = QxCurveLib::CurveLength(aCurve,dF,dL);
	double arlen = len * 0.4;
	if(arlen > 40) arlen = 40;
	
	if(bCumEdgeOri && aE.Orientation() == TopAbs_REVERSED){
		Tg.Reverse();
	}

	kcDispSolidArrow *pArrow = BuildDispArrow(pnt,Tg,arlen,kColor(1.0,0.0,1.0));
	if(pArrow){
		pArrow->Display(TRUE);

		sldArrowVec_.push_back(pArrow);
	}
}

void  kiFaceWireOri::ClearArrowDisp()
{
	std::vector<kcDispSolidArrow *>::size_type ix,isize = sldArrowVec_.size();
	for(ix = 0;ix < isize;ix ++){
		delete sldArrowVec_[ix];
	}
	sldArrowVec_.clear();
}

kcDispSolidArrow*  kiFaceWireOri::BuildDispArrow(const gp_Pnt& pnt,const gp_Vec& dire,double arrlen,const kColor color)
{
	kcDispSolidArrow *pArrow = NULL;
	kPoint3 p;
	kVector3 n;

	p.set(pnt.X(),pnt.Y(),pnt.Z());
	n.set(dire.X(),dire.Y(),dire.Z());
	n.normalize();

	pArrow = new kcDispSolidArrow(GetAISContext(),GetDocContext());
	pArrow->Init(p,n,arrlen);
	pArrow->SetColor(color);
	
	return pArrow;
}

int  kiFaceWireOri::OnExecute()
{
	return KSTA_CONTINUE;
}

int	kiFaceWireOri::OnEnd(int nCode)
{
	ClearArrowDisp();

	return nCode;
}

// ������Ҫ�����빤��
BOOL	kiFaceWireOri::CreateInputTools()
{
	m_optionSet.AddBoolOption("����Face����",'F',bCumFaceOri_);
	m_optionSet.AddBoolOption("����Edge����",'E',bCumEdgeOri_);

	m_pInputFace = new kiInputEntityTool(this,"ѡ������",&m_optionSet);
	m_pInputFace->SetOption(KCT_ENT_FACE, 1);
	m_pInputFace->SetNaturalMode(false);

	return TRUE;
}

BOOL	kiFaceWireOri::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputFace);
	return TRUE;
}

BOOL	kiFaceWireOri::InitInputToolQueue()
{
	AddInputTool(m_pInputFace);
	return TRUE;
}

//////////////////////////////////////////////////////////////////
//
class kiBatAnalyzeThread : public kcProgressThread{
public:
	kiBatAnalyzeThread();

	bool					Init(const std::vector<std::string> &sPathArr);

public:
	virtual void			Run();

protected:
	void					AnalyzeStepFile(const std::string& szFile);

protected:
	std::vector<std::string>	aPathArray_;
};

kiBatAnalyzeThread::kiBatAnalyzeThread()
{
}

bool  kiBatAnalyzeThread::Init(const std::vector<std::string> &sPathArr)
{
	if(sPathArr.empty())
		return false;

	aPathArray_.reserve(sPathArr.size());
	std::vector<std::string>::size_type ix,isize = sPathArr.size();
	for(ix = 0;ix < isize;ix ++){
		aPathArray_.push_back(sPathArr[ix]);
	}
	return true;
}

void  kiBatAnalyzeThread::Run()
{
	std::string szStepFile;
	std::vector<std::string>::size_type ix,isize = aPathArray_.size();
	for(ix = 0;ix < isize;ix ++){
		if(bQuit_){
			bRunning_ = false;
			break;
		}

		szStepFile = aPathArray_[ix];
		//
		SetProgText(szStepFile);
		//
		AnalyzeStepFile(szStepFile);
	}
}

void  kiBatAnalyzeThread::AnalyzeStepFile(const std::string& szFile)
{
	kcSTEPReader aReader(NULL);
	if(!aReader.ReadStepFile(szFile.c_str())){
		return;
	}

	const TopTools_ListOfShape &aShapeList = aReader.GetShapeList();
	if(aShapeList.IsEmpty())
		return;

	SetProgText("Read File Done.Begin collect surface type info.");

	//
	std::string szAnaFile = szFile + "-anay.txt";
	std::string szSAFile = szFile + "-sacol.txt";
	kcgAnalysis cgAnalysis;

	TopTools_ListIteratorOfListOfShape lls;
	for(lls.Initialize(aShapeList);lls.More();lls.Next()){
		TopoDS_Shape aS = lls.Value();
		//
		cgAnalysis.CollectCurveTypeInfo(aS);
		cgAnalysis.CollectSurfTypeInfo(aS);
	}

	cgAnalysis.WriteToFile(szAnaFile.c_str());

	//kcShapeAttribMgr *pSAMgr = aReader.ShapeAttribMgr();
	//if(pSAMgr != NULL){
	//	pSAMgr->WriteToFile(szSAFile.c_str());
	//}
}

//////////////////////////////////////////////////////////////////
//

kiDisplayNbsPoles::kiDisplayNbsPoles()
{
	m_strName = "��ʾ���Ƶ�";
	m_strAlias = "DispPoles";
	m_strDesc = "��ʾ�������߻�����Ŀ��Ƶ�";

	pPickEntTool_ = NULL;
	pPreviewPoles_ = NULL;
}

kiDisplayNbsPoles::~kiDisplayNbsPoles()
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL  kiDisplayNbsPoles::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int  kiDisplayNbsPoles::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == pPickEntTool_){
		if(pTool->IsInputDone()){
			if(DoShowPoles()){
				NavToNextTool();
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

bool  kiDisplayNbsPoles::DoShowPoles()
{
	int nbS = pPickEntTool_->GetSelCount();
	if(nbS > 0){
		kiSelEntity se = pPickEntTool_->GetFirstSelect();
		TopoDS_Shape aS = se.SelectShape();
		if(!aS.IsNull()){
			if(aS.ShapeType() == TopAbs_EDGE){
				double dF,dL;
				Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(aS),dF,dL);
				if(!aCurve.IsNull()){
					if(pPreviewPoles_->Initialize(aCurve)){
						pPreviewPoles_->Display(TRUE);
						PromptMessage("�Ҽ���������");

						return true;
					}
				}
			}else if(aS.ShapeType() == TopAbs_FACE){
				Handle(Geom_Surface) aSurf = BRep_Tool::Surface(TopoDS::Face(aS));
				if(!aSurf.IsNull()){
					if(pPreviewPoles_->Initialize(aSurf)){
						pPreviewPoles_->Display(TRUE);
						PromptMessage("�Ҽ���������");

						return true;
					}
				}
			}
		}
	}
	return false;
}

int  kiDisplayNbsPoles::OnExecute()
{
	pPreviewPoles_ = new kcPreviewNurbsPoles(GetAISContext(),GetDocContext());
	pPreviewPoles_->SetColor(1.0,1.0,0.0);
	pPreviewPoles_->SetLineColor(0.0,1.0,1.0);
	pPreviewPoles_->ShowUIsoLine(true);
	pPreviewPoles_->ShowVIsoLine(true);
	
	//ʰȡѡ�񼯶���
	if(pPickEntTool_ != NULL){
		pPickEntTool_->AddInitSelected(true);
	}

	return kiCommand::OnExecute();
}

int  kiDisplayNbsPoles::OnEnd(int nCode)
{
	KC_SAFE_DELETE(pPreviewPoles_);

	return kiCommand::OnEnd(nCode);
}

// ������Ҫ�����빤��
BOOL  kiDisplayNbsPoles::CreateInputTools()
{
	pPickEntTool_ = new kiInputEntityTool(this,"ѡ���������߻�����",NULL);
	pPickEntTool_->SetOption(KCT_ENT_EDGE | KCT_ENT_FACE,1);
	pPickEntTool_->SetNaturalMode(true);
	
	return TRUE;
}

BOOL  kiDisplayNbsPoles::DestroyInputTools()
{
	KC_SAFE_DELETE(pPickEntTool_);
	return TRUE;
}

BOOL  kiDisplayNbsPoles::InitInputToolQueue()
{
	AddInputTool(pPickEntTool_);
	// ���ϵͳ�˵�
	AddMainButtonOptionTool();

	return TRUE;
}


//////////////////////////////////////////////////////////////////
//
kiBatAnalyze::kiBatAnalyze()
{
	m_strName = "��������";
	m_strAlias = "BatAnalyze";
	m_strDesc = "��������step����ģ��";

	strPath_ = "";
}

kiBatAnalyze::~kiBatAnalyze()
{

}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL  kiBatAnalyze::CanFinish()//�����Ƿ�������
{
	return (FALSE);
}

int  kiBatAnalyze::OnExecute()
{
	CString szFolder;
	CWnd *pMainWnd = AfxGetMainWnd();
	if(!kuiBrowseForFolder(pMainWnd,"ѡ��һ��Ŀ¼",szFolder)){
		return KSTA_FAILED;
	}

	if(!GetStepFileList(szFolder)){
		return KSTA_FAILED;
	}

	kiBatAnalyzeThread aThread;
	aThread.Init(stepFileList_);
	aThread.Start();

	return KSTA_DONE;
}

int  kiBatAnalyze::OnEnd(int nCode)
{

	return KSTA_DONE;
}


bool  kiBatAnalyze::GetStepFileList(const CString &szFolder)
{
	CFileFind aFinder;
	CString szPath,szToFind;

	szToFind = szFolder + "\\*.*";

	BOOL bFound = aFinder.FindFile(szToFind);
	while(bFound){
		bFound = aFinder.FindNextFile();
		if(!bFound)
			break;

		if(aFinder.IsDirectory() || aFinder.IsDots()){
			continue;
		}
		//
		CString szPath = aFinder.GetFilePath();
		int ips = szPath.ReverseFind('.');
		if(ips > 0){
			CString szExt = szPath.Right(szPath.GetLength() - ips - 1);
			szExt.MakeLower();
			if(szExt == "stp" || szExt == "step"){
				TRACE("\n file : %s",szPath);
				stepFileList_.push_back(szPath.GetBuffer());
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////
//
kcmCurveCurvature::kcmCurveCurvature()
{
	m_strName = "��������ͼ";
	m_strAlias = "CurveCurvature";
	m_strDesc = "��ʾ���ߵ�����";

	pPickEntTool_ = NULL;
	pPreviewSegs_ = NULL;

	nbSamp_ = 200;
	dScale_ = 1.0;
}

kcmCurveCurvature::~kcmCurveCurvature()
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL  kcmCurveCurvature::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int  kcmCurveCurvature::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == pPickEntTool_){
		if(pTool->IsInputDone()){
			if(DoShowCurvature()){
				//NavToNextTool();
				RequestEditInput("�����������˳�",&aOptionSet_);
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

void kcmCurveCurvature::OnOptionChanged(kiOptionItem *pOptItem)
{
	if(pOptItem && pOptItem->GetType() == KOPT_INPUT_DOUBLE){
		TRACE("\n scale value changed. %f ",dScale_);
		if(DoBuildDispData()){
			pPreviewSegs_->Display(TRUE);
		}
	}
}

int kcmCurveCurvature::OnExecute()
{
	pPreviewSegs_ = new kcPreviewSegments(GetAISContext(),GetDocContext());
	pPreviewSegs_->SetColor(1.0,0.0,1.0);

	return kiCommand::OnExecute();
}

int  kcmCurveCurvature::OnEnd(int nCode)
{
	KC_SAFE_DELETE(pPreviewSegs_);
	aShapeAry_.clear();

	return kiCommand::OnEnd(nCode);
}

// ������Ҫ�����빤��
BOOL  kcmCurveCurvature::CreateInputTools()
{
	aOptionSet_.SetOptionCallBack(this);

	kiOptionInt *pIntItem = new kiOptionInt("��������",'N',nbSamp_);
	pIntItem->EnablePopup(true);
	aOptionSet_.AddOption(pIntItem);

	kiOptionDouble *pDblItem = new kiOptionDouble("���ű���",'S',dScale_);
	pDblItem->EnablePopup(true);
	aOptionSet_.AddOption(pDblItem);

	aOptionSet_.AddQuitButtonOption();

	pPickEntTool_ = new kiInputEntityTool(this,"ѡ��һ�����������",&aOptionSet_);
	pPickEntTool_->SetOption(KCT_ENT_EDGE,-1); //��ѡ����
	pPickEntTool_->SetNaturalMode(true);

	return TRUE;
}

BOOL  kcmCurveCurvature::DestroyInputTools()
{
	KC_SAFE_DELETE(pPickEntTool_);
	return TRUE;
}

BOOL  kcmCurveCurvature::InitInputToolQueue()
{
	AddInputTool(pPickEntTool_);
	return TRUE;
}

bool kcmCurveCurvature::DoShowCurvature()
{
	aShapeAry_.clear();
	int ix,nbS = pPickEntTool_->GetSelCount();
	if(nbS > 0){
		TopoDS_Shape aS;
		for(ix = 0;ix < nbS;ix ++){
			aS = pPickEntTool_->SelectedShape(ix);
			if(!aS.IsNull() && aS.ShapeType() == TopAbs_EDGE){
				aShapeAry_.push_back(aS);
			}
		}
		
		if(DoBuildDispData()){
			pPreviewSegs_->Display(TRUE);

			return true;
		}
	}
	
	return false;
}

bool kcmCurveCurvature::DoBuildDispData()
{
	if(aShapeAry_.empty()) return false;

	double dF,dL;
	TopoDS_Shape aS;
	std::vector<TopoDS_Shape>::size_type ix,isize = aShapeAry_.size();

	pPreviewSegs_->Clear();
	for(ix = 0;ix < isize;ix ++){
		//
		aS = aShapeAry_.at(ix);
		// ���ߵ����ʰ뾶�������û����ʾ

		Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(aS),dF,dL);
		if(!aCurve.IsNull() && !QxCurveLib::IsLine(aCurve)){
			double t,tstp = (dL - dF)/(nbSamp_ - 1);
			int ix;
			gp_Pnt aPnt,aCenPnt;
			GeomLProp_CLProps aProp(aCurve,3,gp::Resolution());

			for(ix = 0;ix < nbSamp_;ix ++){
				t = dF + ix * tstp;
				//
				aCurve->D0(t,aPnt);

				aProp.SetParameter(t);
				aProp.CentreOfCurvature(aCenPnt);

				gp_Vec aV(aPnt,aCenPnt);
				aV.Scale(dScale_);
				//
				pPreviewSegs_->AddSegment(aPnt,gp_Pnt(aPnt.XYZ()+aV.XYZ()));
			}
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////

//�ʷ�������ʾ
kcmMeshDisplay::kcmMeshDisplay()
{
	InitSet("�ʷ�������ʾ","meshDisp","��ʾģ�͵��ʷ�����");

	pPickEntTool_ = NULL;
	pPreivewMesh_ = NULL;
}

kcmMeshDisplay::~kcmMeshDisplay()
{
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int kcmMeshDisplay::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == pPickEntTool_){
		if(pPickEntTool_->IsInputDone()){
			if(DoDispMesh()){	

				return KSTA_CONTINUE;
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

bool kcmMeshDisplay::DoDispMesh()
{
	int nbS = pPickEntTool_->GetSelCount();
	if(nbS > 0){
		kiSelEntity se = pPickEntTool_->GetFirstSelect();
		TopoDS_Shape aS = se.SelectShape();
		if(!aS.IsNull()){
			if(pPreivewMesh_->Initialize(aS)){
				pPreivewMesh_->Display(TRUE);
				return true;
			}
		}
	}
	return false;
}

int kcmMeshDisplay::OnExecute()
{
	pPreivewMesh_ = new kcPreviewMesh(GetAISContext(),GetDocContext());

	return kiCommand::OnExecute();
}

int kcmMeshDisplay::OnEnd(int nCode)
{
	KC_SAFE_DELETE(pPreivewMesh_);

	return kiCommand::OnEnd(nCode);
}

// ������Ҫ�����빤��
BOOL kcmMeshDisplay::CreateInputTools()
{
	pPickEntTool_ = new kiInputEntityTool(this,"ʰȡ����",NULL);
	pPickEntTool_->SetOption(KCT_ENT_FACE | KCT_ENT_SHELL | KCT_ENT_SOLID,true);
	pPickEntTool_->SetNaturalMode(true);

	return TRUE;
}

BOOL kcmMeshDisplay::DestroyInputTools()
{
	KC_SAFE_DELETE(pPickEntTool_);

	return TRUE;
}

BOOL kcmMeshDisplay::InitInputToolQueue()
{
	AddInputTool(pPickEntTool_);

	return TRUE;
}

///////////////////////////////////////////////////////////
//
kcmFaceOrientAnalysis::kcmFaceOrientAnalysis()
{
	InitSet("FaceUVOrient","FUVO","����uv����");

	pPickFaceTool_ = NULL;

	pUIsoPreviewCurve_ = NULL;
	pVIsoPreviewCurve_ = NULL;
	pUIsoCrvArrow_ = NULL;
	pVIsoCrvArrow_ = NULL;
}

kcmFaceOrientAnalysis::~kcmFaceOrientAnalysis()
{
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int kcmFaceOrientAnalysis::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == pPickFaceTool_){

		ShowUVOrient();

		RequestEditInput("���ѡȡ��ͬFace(U���ɫ��ͷ):",&aOptionSet_);
	}
	return kiCommand::OnInputFinished(pTool);
}

// ��������ı�ʱ������øûص�����
int kcmFaceOrientAnalysis::OnInputChanged(kiInputTool *pTool)
{
	return KSTA_CONTINUE;
}

//ѡ�ѡ����
void kcmFaceOrientAnalysis::OnOptionSelected(kiOptionItem *pOptItem)
{
	int nID = pOptItem->GetID();
	switch(nID){
	case 1://quit
		{
			pPickFaceTool_->End();
			pPickFaceTool_->ClearSel();

			Done();
		}
		break;
	case 2:
		{
			pPickFaceTool_->ClearSel();
			//
			NavToInputTool(pPickFaceTool_);
		}
		break;
	default:
		break;
	}
}

int kcmFaceOrientAnalysis::OnExecute()
{
	kcDocContext *pDocCtx = GetDocContext();

	pUIsoPreviewCurve_ = new kcPreviewCurve(GetAISContext(),pDocCtx);
	pVIsoPreviewCurve_ = new kcPreviewCurve(GetAISContext(),pDocCtx);
	pUIsoCrvArrow_ = new kcDispSolidArrow(GetAISContext(),pDocCtx);
	pVIsoCrvArrow_ = new kcDispSolidArrow(GetAISContext(),pDocCtx);

	pUIsoPreviewCurve_->SetColor(0.0,1,0.0);
	pUIsoPreviewCurve_->SetWidth(3.0);
	pUIsoCrvArrow_->SetColor(kColor(1.0,0.0,0.0));

	pVIsoPreviewCurve_->SetColor(1.0,0.0,0);
	pVIsoPreviewCurve_->SetWidth(3.0);
	pVIsoCrvArrow_->SetColor(kColor(0.0,0.0,1.0));

	return kiCommand::OnExecute();
}

int kcmFaceOrientAnalysis::OnEnd(int nCode)
{
	KC_SAFE_DELETE(pUIsoPreviewCurve_);
	KC_SAFE_DELETE(pVIsoPreviewCurve_);
	KC_SAFE_DELETE(pUIsoCrvArrow_);
	KC_SAFE_DELETE(pVIsoCrvArrow_);

	return kiCommand::OnEnd(nCode);
}

BOOL kcmFaceOrientAnalysis::CreateInputTools()
{
	//���ѡ��
	kiOptionButton *pButton = new kiOptionButton("�˳�",'Q',1);
	pButton->SetOptionCallBack(this);
	aOptionSet_.AddOption(pButton);
	
	pPickFaceTool_ = new kiInputEntityTool(this,"ʰȡFace",&aOptionSet_);
	pPickFaceTool_->SetOption(KCT_ENT_FACE,true);
	pPickFaceTool_->SetNaturalMode(false);

	return TRUE;
}

BOOL kcmFaceOrientAnalysis::DestroyInputTools()
{
	KC_SAFE_DELETE(pPickFaceTool_);
	return TRUE;
}

BOOL kcmFaceOrientAnalysis::InitInputToolQueue()
{
	AddInputTool(pPickFaceTool_);
	return TRUE;
}

void  kcmFaceOrientAnalysis::ShowUVOrient()
{
	int nbSel = pPickFaceTool_->GetSelCount();
	if(nbSel == 1){
		TopoDS_Shape aS = pPickFaceTool_->GetFirstSelect().SelectShape();
		//
		if(!aS.IsNull() && aS.ShapeType() == TopAbs_FACE){
			double UMin,UMax,VMin,VMax;
			gp_Pnt aP;
			gp_Vec aV;
			TopoDS_Face aF = TopoDS::Face(aS);
			BRepTools::UVBounds(aF,UMin,UMax,VMin,VMax);
			//���ɶ�Ӧ������
			Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aF);
			if(!aSurf.IsNull()){
				//U��
				Handle(Geom_Curve) aUIsoCrv = aSurf->VIso(VMin);
				if(!aUIsoCrv.IsNull()){
					Handle(Geom_Curve) aUTrmIsoCrv = new Geom_TrimmedCurve(aUIsoCrv,UMin,UMax);

					pUIsoPreviewCurve_->Update(aUTrmIsoCrv);
					pUIsoPreviewCurve_->Display(TRUE);

					aUIsoCrv->D1((UMin + UMax)/2,aP,aV);
					double clen = QxCurveLib::CurveLength(aUIsoCrv,UMin,UMax);
					aV.Normalize();

					kPoint3 O(aP.X(),aP.Y(),aP.Z());
					kVector3 dir(aV.X(),aV.Y(),aV.Z());
					pUIsoCrvArrow_->Init(O,dir,clen * 0.3);
					pUIsoCrvArrow_->UpdateDisplay();
					pUIsoCrvArrow_->Display(TRUE);
				}
				//V��
				Handle(Geom_Curve) aVIsoCrv = aSurf->UIso(UMin);
				if(!aVIsoCrv.IsNull()){
					Handle(Geom_Curve) aVTrmIsoCrv = new Geom_TrimmedCurve(aVIsoCrv,VMin,VMax);
					//
					pVIsoPreviewCurve_->Update(aVTrmIsoCrv);
					pVIsoPreviewCurve_->Display(TRUE);

					aVIsoCrv->D1((VMin + VMax)/2,aP,aV);
					double clen = QxCurveLib::CurveLength(aVIsoCrv,VMin,VMax);
					aV.Normalize();

					kPoint3 O(aP.X(),aP.Y(),aP.Z());
					kVector3 dir(aV.X(),aV.Y(),aV.Z());
					pVIsoCrvArrow_->Init(O,dir,clen * 0.3);
					pVIsoCrvArrow_->UpdateDisplay();
					pVIsoCrvArrow_->Display(TRUE);
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
kcmFaceNormalAnalysis::kcmFaceNormalAnalysis(void)
{
	m_strName = "���淨��";
	m_strAlias = "FaceNorm";
	m_strDesc = "����ѡ������ķ���";

	m_pInputEntity = NULL;
	m_bTopoNormal = true;

	pNormalArrow_ = NULL;
}

kcmFaceNormalAnalysis::~kcmFaceNormalAnalysis(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmFaceNormalAnalysis::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmFaceNormalAnalysis::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntity){

		DisplayNormal();

		RequestEditInput("ѡ��ͬFace��ʾ��ʧ����",&m_optionSet);
	}
	return KSTA_CONTINUE;
}

//ѡ�ѡ����
void kcmFaceNormalAnalysis::OnOptionSelected(kiOptionItem *pOptItem)
{
	int nID = pOptItem->GetID();
	switch(nID){
	case 1://quit
		{
			m_pInputEntity->End();
			m_pInputEntity->ClearSel();

			Done();
		}
		break;
	default:
		break;
	}
}

BOOL kcmFaceNormalAnalysis::DisplayNormal()
{
	if(m_pInputEntity->GetSelCount() != 1)
		return FALSE;

	kiSelEntity se = m_pInputEntity->GetFirstSelect();
	TopoDS_Shape aS = se.SelectShape();
	TopoDS_Face aFace = TopoDS::Face(aS);
	kPoint3 p,p2;
	kVector3 norm;

	if(QxSurfLib::CalcFaceNormal(aFace,m_bTopoNormal,p,norm)){
		
		norm.normalize();
		//p2 = p + norm * 10;

		pNormalArrow_->Init(p,norm,20);
		//pNormalArrow_->UpdateDisplay();
		pNormalArrow_->Display(TRUE);
	}

	return TRUE;
}

int kcmFaceNormalAnalysis::OnExecute()
{
	kColor aColor(1.0,0.0,1.0);
	pNormalArrow_ = new kcDispSolidArrow(GetAISContext(),GetDocContext());
	pNormalArrow_->SetColor(aColor);

	return KSTA_CONTINUE;
}

int		kcmFaceNormalAnalysis::OnEnd(int nCode)
{
	KC_SAFE_DELETE(pNormalArrow_);
	
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmFaceNormalAnalysis::CreateInputTools()
{
	m_optionSet.AddBoolOption("���˷���",'T',m_bTopoNormal);

	kiOptionButton *pButton = new kiOptionButton("�˳�",'Q',1);
	pButton->SetOptionCallBack(this);
	m_optionSet.AddOption(pButton);

	m_pInputEntity = new kiInputEntityTool(this,"ѡ�����",&m_optionSet);
	m_pInputEntity->SetOption(KCT_ENT_FACE,true);
	m_pInputEntity->SetNaturalMode(false);
	
	return TRUE;
}

BOOL	kcmFaceNormalAnalysis::DestroyInputTools()
{
	m_optionSet.Clear();
	KC_SAFE_DELETE(m_pInputEntity);

	return TRUE;
}

BOOL	kcmFaceNormalAnalysis::InitInputToolQueue()
{
	AddInputTool(m_pInputEntity);

	return TRUE;
}
