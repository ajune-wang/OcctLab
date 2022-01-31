#include "StdAfx.h"
#include "kcEntity.h"
#include "kcModel.h"
#include "QxBRepLib.h"
#include "QxCurveLib.h"
#include "kiInputEntityTool.h"
#include "kiInputValueTool.h"
#include "kiInputPointOnCurve.h"
#include "kcEntityFactory.h"
#include "kcmCurveBlend.h"

kcmCurveBlend::kcmCurveBlend(void)
{
	m_strName = "�������";
	m_strAlias = "BlendCrv";
	m_strDesc = "�������";

	m_pPickFirstCurveTool = NULL;
	m_pPickSecondCurveTool = NULL;
	m_pContEnum1 = NULL;
	m_pContEnum2 = NULL;
	m_nCont1 = 1;
	m_nCont2 = 1;
}

kcmCurveBlend::~kcmCurveBlend(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmCurveBlend::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmCurveBlend::OnInputFinished(kiInputTool *pTool)
{
	if(pTool->IsInputDone())
	{
		if(pTool == m_pPickFirstCurveTool)
		{
			NavToNextTool();
		}
		else if(pTool == m_pPickSecondCurveTool)
		{
			if(BuildBlendCurve())
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
	}
	else
	{
		EndCommand(KSTA_CANCEL);
		return KSTA_CANCEL;
	}
	return KSTA_CONTINUE;
}

int		kcmCurveBlend::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmCurveBlend::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmCurveBlend::CreateInputTools()
{
	m_pContEnum1 = new kiOptionEnum("������",'C',m_nCont1);
	m_pContEnum1->AddEnumItem("G0",'P');
	m_pContEnum1->AddEnumItem("G1",'T');
	m_pContEnum1->AddEnumItem("G2",'C');
	m_optionSet1.AddOption(m_pContEnum1);

	m_pContEnum2 = new kiOptionEnum("������",'C',m_nCont2);
	m_pContEnum2->AddEnumItem("G0",'P');
	m_pContEnum2->AddEnumItem("G1",'T');
	m_pContEnum2->AddEnumItem("G2",'C');
	m_optionSet2.AddOption(m_pContEnum2);

	m_pPickFirstCurveTool = new kiInputEntityTool(this,"ѡ���һ���߶˵㴦",&m_optionSet1);
	m_pPickFirstCurveTool->SetOption(KCT_ENT_EDGE,true);
	m_pPickFirstCurveTool->NeedPickInfo(true);

	m_pPickSecondCurveTool = new kiInputEntityTool(this,"ѡ��ڶ����߶˵㴦",&m_optionSet2);
	m_pPickSecondCurveTool->SetOption(KCT_ENT_EDGE,true);
	m_pPickSecondCurveTool->NeedPickInfo(true);

	return TRUE;
}

BOOL	kcmCurveBlend::DestroyInputTools()
{
	m_optionSet1.Clear();
	m_optionSet2.Clear();

	KC_SAFE_DELETE(m_pPickFirstCurveTool);
	KC_SAFE_DELETE(m_pPickSecondCurveTool);

	return TRUE;
}

BOOL	kcmCurveBlend::InitInputToolQueue()
{
	AddInputTool(m_pPickFirstCurveTool);
	AddInputTool(m_pPickSecondCurveTool);

	return TRUE;
}

//
BOOL	kcmCurveBlend::BuildBlendCurve()
{
	if(m_pPickFirstCurveTool->GetSelCount() != 1 ||
	   m_pPickSecondCurveTool->GetSelCount() != 1)
	   return FALSE;

	double t1,t2;
	TopoDS_Edge aFEdge,aSEdge;
	TopoDS_Shape aS = m_pPickFirstCurveTool->SelectedShape(0);
	aFEdge = TopoDS::Edge(aS);
	aS = m_pPickSecondCurveTool->SelectedShape(0);
	aSEdge = TopoDS::Edge(aS);
	
	QxShapePickInfo info;
	if(!m_pPickFirstCurveTool->GetShapePickInfo(info))
		return FALSE;
	t1 = info._t;

	if(!m_pPickSecondCurveTool->GetShapePickInfo(info))
		return FALSE;
	t2 = info._t;

	//
	TopoDS_Edge aNE;
	kcgBlendCurve blendCrv;
	aNE = blendCrv.Blend(aFEdge,t1,m_nCont1,aSEdge,t2,m_nCont2);
	if(aNE.IsNull())
	{
		PromptMessage("Blend curve failed.");
		return FALSE;
	}

	kcModel *pModel = GetModel();
	kcEntityFactory efac;
	kcEntity *pEnt = efac.Create(aNE);
	if(pEnt)
	{
		pModel->BeginUndo(GetName());
		pModel->AddEntity(pEnt);
		pModel->EndUndo();
	}

	return TRUE;
}
