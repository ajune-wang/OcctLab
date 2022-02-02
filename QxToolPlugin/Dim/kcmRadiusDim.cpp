#include "StdAfx.h"
#include <AIS_LengthDimension.hxx>
#include <AIS.hxx>
#include <gce_MakeDir.hxx>
#include "kiInputEntityTool.h"
#include "kiInputValueTool.h"
#include "kiInputLength.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "kvCoordSystem.h"
#include "kcEntityFactory.h"
#include "kcPreviewObj.h"
#include "kcmRadiusDim.h"

#define KC_STA_PICK_FIRST_SHAPE		1
#define KC_STA_PICK_SECOND_SHAPE    2
#define KC_STA_INPUT_OFFSET			3

kcmRadiusDim::kcmRadiusDim(void)
{
	m_strName = "�뾶��ע";
	m_strAlias = "RadDim";
	m_strDesc = "�뾶��ע";

	pPickCircleTool_ = NULL;
	pOptionSet_ = NULL;
	dFlyout_ = 10;
	dArrowLen_ = 2.0;
	dFontHeight_ = 16.0;
}

kcmRadiusDim::~kcmRadiusDim(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmRadiusDim::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmRadiusDim::OnInputFinished(kiInputTool *pTool)
{
	if (pTool->IsInputDone()) {
		if (pTool == pPickCircleTool_) {

			if (BuildDimension()) {
				Done();
				return KSTA_DONE;
			}
			else {
				EndCommand(KSTA_FAILED);
				return KSTA_FAILED;
			}
		}
	}
	else
	{
		EndCommand(KSTA_CANCEL);
		return KSTA_CANCEL;
	}
	return KSTA_CONTINUE;
}

int kcmRadiusDim::OnExecute()
{

	return KSTA_CONTINUE;
}

int kcmRadiusDim::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmRadiusDim::CreateInputTools()
{
	pOptionSet_ = new kiOptionSet(this);

	kiOptionDouble *pOptDbl = new kiOptionDouble("Flyout", 'F', dFlyout_);
	pOptDbl->EnablePopup(true);
	pOptionSet_->AddOption(pOptDbl);

	pOptDbl = new kiOptionDouble("ArrowLen", 'L', dArrowLen_);
	pOptDbl->EnablePopup(true);
	pOptionSet_->AddOption(pOptDbl);

	pOptDbl = new kiOptionDouble("FontHeight", 'H', dFontHeight_);
	pOptDbl->EnablePopup(true);
	pOptionSet_->AddOption(pOptDbl);

	pOptionSet_->AddQuitButtonOption();

	pPickCircleTool_ = new kiInputEntityTool(this, "select an Circle", pOptionSet_);
	pPickCircleTool_->SetOption(KCT_ENT_EDGE, true);
	pPickCircleTool_->SetNaturalMode(false);
	pPickCircleTool_->NeedPickInfo(true);

	return TRUE;
}

BOOL	kcmRadiusDim::DestroyInputTools()
{
	KC_SAFE_DELETE(pPickCircleTool_);

	return TRUE;
}

BOOL	kcmRadiusDim::InitInputToolQueue()
{
	AddInputTool(pPickCircleTool_);

	return TRUE;
}

int	kcmRadiusDim::OnMouseMove(kuiMouseInput& mouseInput)
{
	kiCommand::OnMouseMove(mouseInput);

	//if(m_nState == KC_STA_INPUT_OFFSET)
	//{
	//	double len = m_pInputLength->GetLength();
	//	kPoint3 bp,ep;
	//	
	//	if(m_nType == 0)
	//	{
	//		bp = m_aPnt1 + len * m_aOffDir;
	//		ep = m_aPnt2 + len * m_aOffDir;
	//	}
	//	else if(m_nType == 1)//��ֱ
	//	{
	//		double dist = (m_aPnt2 - m_aPnt1) * m_aOffDir; 
	//		bp = m_aPnt1 + (len + dist / 2.0) * m_aOffDir;
	//		ep = m_aPnt2 + (len - dist / 2.0) * m_aOffDir;
	//	}
	//	else
	//	{
	//		double dist = (m_aPnt2 - m_aPnt1) * m_aOffDir; 
	//		bp = m_aPnt1 + (len + dist / 2.0) * m_aOffDir;
	//		ep = m_aPnt2 + (len - dist / 2.0) * m_aOffDir;
	//	}
	//	
	//	if(m_pPreviewLine)
	//	{
	//		m_pPreviewLine->Update(bp,ep);
	//		m_pPreviewLine->Display(TRUE);
	//	}
	//}

	return KSTA_CONTINUE;
}

//
BOOL	kcmRadiusDim::BuildDimension()
{
	QxShapePickInfo info;
	if (pPickCircleTool_->GetSelCount() != 1 ||
		!pPickCircleTool_->GetShapePickInfo(info))
		return FALSE;

	double dF = 0.0, dL = 0.0;
	TopoDS_Shape aE = pPickCircleTool_->SelectedShape(0);
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(aE), dF, dL);
	GeomAdaptor_Curve aAC;
	aAC.Load(aCurve, dF, dL);
	if (aAC.GetType() != GeomAbs_Circle) {
		return FALSE;
	}
	
	gp_Pnt anchorPnt(info._p.x(), info._p.y(), info._p.z());
	gp_Circ aCirc = aAC.Circle();
	kcRadiusDimEntity *pEnt = new kcRadiusDimEntity;
	pEnt->Initialize(aCirc,anchorPnt);
	pEnt->SetFlyout(dFlyout_);
	pEnt->SetArrowLength(dArrowLen_);
	pEnt->SetFontHeight(dFontHeight_);

	kcModel *pModel = GetModel();
	pModel->BeginUndo(GetName());
	pModel->AddEntity(pEnt);
	pModel->EndUndo();

	Redraw();

	return TRUE;
}
