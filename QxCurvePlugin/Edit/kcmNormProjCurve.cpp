#include "StdAfx.h"
#include <BRepOffsetAPI_NormalProjection.hxx>
#include "kcEntity.h"
#include "kcModel.h"
#include "QxBRepLib.h"
#include "QxCurveLib.h"
#include "kiInputEntityTool.h"
#include "kiInputValueTool.h"
#include "kiInputPointOnCurve.h"
#include "kcEntityFactory.h"
#include "kcmNormProjCurve.h"

kcmNormProjCurve::kcmNormProjCurve(void)
{
	m_strName = "ͶӰ����";
	m_strAlias = "PrjCrv";
	m_strDesc = "����ͶӰ������";

	m_pPickCurveTool = NULL;
	m_pPickFaceTool = NULL;
}

kcmNormProjCurve::~kcmNormProjCurve(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmNormProjCurve::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmNormProjCurve::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pPickCurveTool)
	{
		if(pTool->IsInputDone())
		{
			NavToNextTool();
		}
	}
	else if(pTool == m_pPickFaceTool)
	{
		if(pTool->IsInputDone())
		{
			m_pPickFaceTool->End();
			//
			if(DoNormProj())
			{
				Done();
				return KSTA_DONE;
			}
			else
			{
				EndCommand(KSTA_FAILED);
				return KSTA_FAILED;
			}
		}
		else
		{
			EndCommand(KSTA_CANCEL);
			return KSTA_CANCEL;
		}
	}
	return KSTA_CONTINUE;
}

int		kcmNormProjCurve::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmNormProjCurve::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmNormProjCurve::CreateInputTools()
{
	m_pPickCurveTool = new kiInputEntityTool(this,"ʰȡҪͶӰ������");
	m_pPickCurveTool->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE);
	m_pPickCurveTool->SetNaturalMode(true);

	m_pPickFaceTool = new kiInputEntityTool(this,"ʰȡͶӰ������");
	m_pPickFaceTool->SetOption(KCT_ENT_FACE,true);
	m_pPickFaceTool->SetNaturalMode(false); //����ѡ��ֲ�����

	return TRUE;
}

BOOL	kcmNormProjCurve::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pPickCurveTool);
	KC_SAFE_DELETE(m_pPickFaceTool);

	return TRUE;
}

BOOL	kcmNormProjCurve::InitInputToolQueue()
{
	AddInputTool(m_pPickCurveTool);
	AddInputTool(m_pPickFaceTool);

	return TRUE;
}

//
BOOL	kcmNormProjCurve::DoNormProj()
{
	if(m_pPickCurveTool->GetSelCount() <= 0)
		return FALSE;
	int nbShape = m_pPickFaceTool->GetSelCount();
	if(nbShape != 1)
		return FALSE;
	TopoDS_Shape aShape = m_pPickFaceTool->SelectedShape(0);

	TopoDS_Shape aNS;
	try{
		BRepOffsetAPI_NormalProjection np(aShape);
		kiSelSet *pSelSet = m_pPickCurveTool->GetSelSet();
		pSelSet->InitSelected();
		while(pSelSet->MoreSelected())
		{
			kiSelEntity se = pSelSet->CurSelected();
			TopoDS_Shape aS = se._pEntity->GetShape();
			np.Add(aS);
			pSelSet->NextSelected();
		}
		np.SetLimit(Standard_True);
		np.Compute3d(Standard_True);
		np.Build();
		if(np.IsDone())
			aNS = np.Shape();
	}catch(Standard_Failure){}

	if(aNS.IsNull())
		return FALSE;

	kcEntityFactory entfac;
	kcEntity *pEnt = entfac.Create(aNS);
	if(pEnt)
	{
		kcModel *pModel = GetModel();
		pModel->BeginUndo(GetName());
		pModel->AddEntity(pEnt);
		pModel->EndUndo();

		return TRUE;
	}

	return FALSE;
}

