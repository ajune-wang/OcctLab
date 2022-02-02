#include "StdAfx.h"
#include <AIS_LengthDimension.hxx>
#include <AIS_DiameterDimension.hxx>
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
#include "kcmDiameterDim.h"

#define KC_STA_PICK_FIRST_SHAPE		1
#define KC_STA_PICK_SECOND_SHAPE    2
#define KC_STA_INPUT_OFFSET			3

kcmDiameterDim::kcmDiameterDim(void)
{
	m_strName = "ֱ����ע";
	m_strAlias = "DiamDim";
	m_strDesc = "ֱ����ע";

	pPickCircleTool_ = NULL;
	pOptionSet_ = NULL;
	dFlyout_ = 10;
	dArrowLen_ = 2.0;
	dFontHeight_ = 16.0;
}

kcmDiameterDim::~kcmDiameterDim(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmDiameterDim::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmDiameterDim::OnInputFinished(kiInputTool *pTool)
{
	if(pTool->IsInputDone()){
		if(pTool == pPickCircleTool_){
			
			if(BuildDimension()){
				Done();
				return KSTA_DONE;
			}else{
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

int		kcmDiameterDim::OnExecute()
{

	return KSTA_CONTINUE;
}

int		kcmDiameterDim::OnEnd(int nCode)
{

	return nCode;
}

// ������Ҫ�����빤��
BOOL kcmDiameterDim::CreateInputTools()
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
	
	return TRUE;
}

BOOL kcmDiameterDim::DestroyInputTools()
{
	KC_SAFE_DELETE(pPickCircleTool_);
	KC_SAFE_DELETE(pOptionSet_);
	return TRUE;
}

BOOL	kcmDiameterDim::InitInputToolQueue()
{
	AddInputTool(pPickCircleTool_);

	return TRUE;
}

int kcmDiameterDim::OnMouseMove(kuiMouseInput& mouseInput)
{
	kiCommand::OnMouseMove(mouseInput);



	return KSTA_CONTINUE;
}

//
BOOL kcmDiameterDim::BuildDimension()
{
	if (pPickCircleTool_->GetSelCount() != 1)
		return FALSE;
	
	double dF = 0.0, dL = 0.0;
	TopoDS_Shape aE = pPickCircleTool_->SelectedShape(0);
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(aE), dF, dL);
	GeomAdaptor_Curve aAC;
	aAC.Load(aCurve, dF, dL);
	if (aAC.GetType() != GeomAbs_Circle) {
		return FALSE;
	}

	gp_Circ aCirc = aAC.Circle();
	kcDiameterDimEntity *pEnt = new kcDiameterDimEntity;
	pEnt->Initialize(aCirc);
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
