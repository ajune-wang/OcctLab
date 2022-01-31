#include "StdAfx.h"
#include "kiInputPoint.h"
#include "kPoint.h"
#include "kVector.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "kcEntity.h"
#include "kiInputLength.h"
#include "kiInputAngle.h"
#include "kiInputEntityTool.h"
#include "kvCoordSystem.h"
#include "kcmUtils.h"
#include "kcPreviewObj.h"
#include "kcgIntersect.h"
#include "kcPreviewText.h"
#include "kcmPoint.h"

kcmPoint::kcmPoint(void)
{
	m_strName = "Point";
	m_strAlias = "Pnt";
	m_strDesc = "create point";

	m_pInputPoint = NULL;
}

kcmPoint::~kcmPoint(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
//
BOOL	kcmPoint::CanFinish()
{
	return TRUE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmPoint::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputPoint)
	{
		if(m_pInputPoint->IsInputDone())
		{
			kcPointEntity *pPntEntity = new kcPointEntity;
			if(pPntEntity)
			{
				pPntEntity->SetPoint(m_aPoint[0],m_aPoint[1],m_aPoint[2]);

				kcModel *pModel = GetModel();
				pModel->BeginUndo(GetName());
				pModel->AddEntity(pPntEntity);
				pModel->EndUndo();

				Redraw();
			}
		}
		
		NavToInputTool(m_pInputPoint);
	}

	return KSTA_CONTINUE;
}

int	kcmPoint::OnExecute()
{
	return KSTA_CONTINUE;
}

// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
BOOL	kcmPoint::CreateInputTools()
{
	m_pInputPoint = new kiInputPoint(this,"�����:");
	m_pInputPoint->Init(&m_aPoint);

	return TRUE;
}

// ���ٴ��������빤��.ÿ�����������һ��.
BOOL	kcmPoint::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputPoint);

	return TRUE;
}

// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
BOOL	kcmPoint::InitInputToolQueue()
{
	AddInputTool(m_pInputPoint);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ���߽���
kcmCurveInterPoint::kcmCurveInterPoint()
{
	m_strName = "���߽���";
	m_strAlias = "CurIntpt";
	m_strDesc = "�����߽���";

	m_pInputCurve1 = NULL;
	m_pInputCurve2 = NULL;
}

kcmCurveInterPoint::~kcmCurveInterPoint()
{

}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmCurveInterPoint::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmCurveInterPoint::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputCurve1)
	{
		if(m_pInputCurve1->IsInputDone())
		{
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputCurve2)
	{
		if(m_pInputCurve2->IsInputDone())
		{
			if(CalcInterPoint())
			{
				m_pInputCurve1->ClearSel();
				m_pInputCurve2->ClearSel();

				Done();
				Redraw();
				return KSTA_DONE;
			}
			else
			{
				m_pInputCurve1->ClearSel();
				m_pInputCurve2->ClearSel();

				return KSTA_ERROR;
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

BOOL	kcmCurveInterPoint::CalcInterPoint()
{
	kiSelEntity se1,se2;
	kiSelSet *pSelSet = m_pInputCurve1->GetSelSet();
	pSelSet->InitSelected();
	se1 = pSelSet->CurSelected();

	pSelSet = m_pInputCurve2->GetSelSet();
	pSelSet->InitSelected();
	se2 = pSelSet->CurSelected();

	kcgCurveCurveInter ccinter;
	std::vector<kcEntity *> aEnt;
	if(IS_EDGE_ENTITY(se1._pEntity))
	{
		TopoDS_Edge aEdge1 = ((kcEdgeEntity *)se1._pEntity)->GetEdge();
		if(IS_EDGE_ENTITY(se2._pEntity))
		{
			TopoDS_Edge aEdge2 = ((kcEdgeEntity *)se2._pEntity)->GetEdge();
			ccinter.Intersect(aEdge1,aEdge2);
		}
		else
		{
			TopoDS_Wire aWire = ((kcWireEntity *)se2._pEntity)->GetWire();
			ccinter.Intersect(aEdge1,aWire);
		}
	}
	else
	{
		TopoDS_Wire aWire1 = ((kcWireEntity *)se1._pEntity)->GetWire();
		if(IS_EDGE_ENTITY(se2._pEntity))
		{
			TopoDS_Edge aEdge = ((kcEdgeEntity *)se2._pEntity)->GetEdge();
			ccinter.Intersect(aEdge,aWire1);
		}
		else
		{
			TopoDS_Wire aWire2 = ((kcWireEntity *)se2._pEntity)->GetWire();
			ccinter.Intersect(aWire1,aWire2);
		}
	}
	int ix;
	for(ix = 0;ix < ccinter.NbInterPoints();ix ++)
	{
		kcxCCIntpt ipt = ccinter.InterPoint(ix);
		kcPointEntity *pEnt = new kcPointEntity;
		pEnt->SetPoint(ipt._ipt1._p[0],ipt._ipt1._p[1],ipt._ipt1._p[2]);
		aEnt.push_back(pEnt);
	}

	if(!aEnt.empty())
	{
		kcModel *pModel = GetModel();
		pModel->BeginUndo(GetName());
		for(ix = 0;ix < (int)aEnt.size();ix ++)
			pModel->AddEntity(aEnt[ix]);
		pModel->EndUndo();

		return TRUE;
	}

	return FALSE;
}

int		kcmCurveInterPoint::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmCurveInterPoint::OnEnd(int nCode)
{
	m_pInputCurve1->ClearSel();
	m_pInputCurve2->ClearSel();

	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmCurveInterPoint::CreateInputTools()
{
	m_pInputCurve1 = new kiInputEntityTool(this,"ѡ���һ������");
	m_pInputCurve1->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE,1);

	m_pInputCurve2 = new kiInputEntityTool(this,"ѡ���������");
	m_pInputCurve2->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE,1);

	return TRUE;
}

BOOL	kcmCurveInterPoint::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputCurve1);
	KC_SAFE_DELETE(m_pInputCurve2);
	return TRUE;
}

BOOL	kcmCurveInterPoint::InitInputToolQueue()
{
	AddInputTool(m_pInputCurve1);
	AddInputTool(m_pInputCurve2);
	return TRUE;
}
