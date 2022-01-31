#include "StdAfx.h"
#include <GeomAPI_Interpolate.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include "kcPreviewObj.h"
#include "kcModel.h"
#include "kiInputPoint.h"
#include "kiInputEntityTool.h"
#include "kcEntity.h"
#include "kcEntityFactory.h"
#include "kcmSpline.h"

kcmSpline::kcmSpline(void)
{
	m_strName = "Spline";
	m_strAlias = "bsp";
	m_strDesc = "ͨ�����ֵ��������";

	m_bPeriodic = false;
	m_pInputPoint = NULL;
	m_pPreviewCurve = NULL;
}

kcmSpline::~kcmSpline(void)
{
}

// ִ�к���
int		kcmSpline::OnExecute()
{
	m_aPoint.clear();

	m_pPreviewCurve = new kcPreviewCurve(GetAISContext(),GetDocContext());
	m_pPreviewCurve->SetColor(1.0,0.0,1.0);

	return KSTA_CONTINUE;
}

int		kcmSpline::OnEnd(int nCode)
{
	KC_SAFE_DELETE(m_pPreviewCurve);

	m_aPoint.clear();

	return nCode;
}

int		kcmSpline::OnApply()
{
	if(m_aPoint.size() >= 2)
	{
		int ix,isize = (int)m_aPoint.size();
		Handle(TColgp_HArray1OfPnt) aPoint = new TColgp_HArray1OfPnt(1,isize);
		for(ix = 1;ix <= isize;ix ++)
			aPoint->SetValue(ix,gp_Pnt(m_aPoint[ix-1].x(),m_aPoint[ix-1].y(),m_aPoint[ix-1].z()));

		GeomAPI_Interpolate interp(aPoint,m_bPeriodic ? Standard_True : Standard_False,1e-5);
		interp.Perform();
		if(interp.IsDone())
		{
			Handle(Geom_BSplineCurve) aSpl = interp.Curve();
			if(!aSpl.IsNull())
			{
				//{
				//	gp_Pnt pnt;
				//	aSpl->D0(aSpl->FirstParameter(),pnt);
				//}

				kcEdgeEntity *pCurEnt = new kcEdgeEntity;
				pCurEnt->SetCurve(aSpl);

				kcModel *pModel = GetModel();
				pModel->AddEntity(pCurEnt);

				ClearTempSnapPoint();

				Redraw();

				Done();

				return KSTA_DONE;
			}
		}
	}

	return KSTA_CONTINUE;
}

//�����Ƿ�������
BOOL	kcmSpline::CanFinish()
{
	if(m_aPoint.size() < 2)
		return FALSE;

	return TRUE;
}

//���빤�߽���ʱ
int	kcmSpline::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputPoint)
	{
		kPoint3 kp(m_dPoint[0],m_dPoint[1],m_dPoint[2]);
		m_aPoint.push_back(kp);

		// �����ʼ�������ʱ�㣬�����Զ���׽��
		if(m_aPoint.size() == 1)
			AddTempSnapPoint(m_dPoint[0],m_dPoint[1],m_dPoint[2]);

		int isize = (int)m_aPoint.size();
		if(isize >= 2)
		{
			int ix;
			Handle(TColgp_HArray1OfPnt) aPoint = new TColgp_HArray1OfPnt(1,isize);
			for(ix = 1;ix <= isize;ix ++)
				aPoint->SetValue(ix,gp_Pnt(m_aPoint[ix-1].x(),m_aPoint[ix-1].y(),m_aPoint[ix-1].z()));

			GeomAPI_Interpolate interp(aPoint,m_bPeriodic ? Standard_True : Standard_False,1e-5);
			interp.Perform();
			if(interp.IsDone())
			{
				Handle(Geom_BSplineCurve) aSpl = interp.Curve();
				if(m_pPreviewCurve)
				{
					m_pPreviewCurve->Update(aSpl);
					m_pPreviewCurve->Display(TRUE);

					Redraw();
				}
			}
		}
		NavToInputTool(m_pInputPoint);
	}
	return KSTA_CONTINUE;
}

// ������Ҫ�����빤��
BOOL	kcmSpline::CreateInputTools()
{
	m_optionSet.AddBoolOption("���ڱպ�",'P',m_bPeriodic);

	m_pInputPoint = new kiInputPoint(this,"����һ����",&m_optionSet);
	m_pInputPoint->Init(&m_dPoint);
	m_pInputPoint->SetRealtimeUpdate(true);
	return TRUE;
}

BOOL	kcmSpline::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputPoint);
	return TRUE;
}

BOOL	kcmSpline::InitInputToolQueue()
{
	AddInputTool(m_pInputPoint);
	return TRUE;
}

int		kcmSpline::OnMouseMove(kuiMouseInput& mouseInput)
{
	kiCommand::OnMouseMove(mouseInput);

	int isize = (int)m_aPoint.size();
	if(isize >= 1)
	{
		int ix;
		if(m_aPoint[isize - 1].distance(m_dPoint) < 0.0001)
			return KSTA_CONTINUE;

		Handle(TColgp_HArray1OfPnt) aPoint = new TColgp_HArray1OfPnt(1,isize + 1);
		for(ix = 1;ix <= isize;ix ++)
			aPoint->SetValue(ix,gp_Pnt(m_aPoint[ix-1].x(),m_aPoint[ix-1].y(),m_aPoint[ix-1].z()));
		aPoint->SetValue(isize + 1,gp_Pnt(m_dPoint.x(),m_dPoint.y(),m_dPoint.z()));

		Handle(Geom_BSplineCurve) aSpl;
		try{
			GeomAPI_Interpolate interp(aPoint,m_bPeriodic ? Standard_True : Standard_False,1e-5);
			interp.Perform();
			if(interp.IsDone())
			{
				aSpl = interp.Curve();
			}
		}catch(Standard_Failure){
		}
		
		if(!aSpl.IsNull() && m_pPreviewCurve)
		{
			m_pPreviewCurve->Update(aSpl);
			if(!m_pPreviewCurve->IsDisplayed())
				m_pPreviewCurve->Display(TRUE);

			Redraw();
		}
	}

	return KSTA_CONTINUE;
}

int		kcmSpline::OnLButtonUp(kuiMouseInput& mouseInput)
{
	return kiCommand::OnLButtonUp(mouseInput);
}

//////////////////////////////////////////////////////////////////////////
// ���ߴ��
kcmSplineUnperiodize::kcmSplineUnperiodize()
{
	m_strName = "�������������ڻ�";
	m_strAlias = "SplineUnperiodize";
	m_strDesc = "�������������ڻ�";

	m_pInputCurve = NULL;
}

kcmSplineUnperiodize::~kcmSplineUnperiodize()
{

}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmSplineUnperiodize::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmSplineUnperiodize::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputCurve)
	{
		if(m_pInputCurve->IsInputDone())
		{
			if(DoUnperiodize())
			{
				Done();
				Redraw();

				return KSTA_DONE;
			}
		}
	}

	return KSTA_CONTINUE;
}

int		kcmSplineUnperiodize::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmSplineUnperiodize::OnEnd(int nCode)
{
	m_pInputCurve->ClearSel();
	
	return nCode;
}


// ������Ҫ�����빤��
BOOL	kcmSplineUnperiodize::CreateInputTools()
{
	m_pInputCurve = new kiInputEntityTool(this,"ѡ��������������");
	m_pInputCurve->SetOption(KCT_ENT_EDGE);

	return TRUE;
}

BOOL	kcmSplineUnperiodize::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputCurve);
	return TRUE;
}

BOOL	kcmSplineUnperiodize::InitInputToolQueue()
{
	AddInputTool(m_pInputCurve);
	return TRUE;
}

BOOL   kcmSplineUnperiodize::DoUnperiodize()
{
	if(m_pInputCurve->GetSelCount() != 1)
		return FALSE;

	kiSelEntity se = m_pInputCurve->GetFirstSelect();
	TopoDS_Shape aShape = se._pEntity->GetShape();
	TopoDS_Edge aEdge = TopoDS::Edge(aShape);
	Handle(Geom_Curve) aCurve;
	double dFirst,dLast;
	aCurve = BRep_Tool::Curve(aEdge,dFirst,dLast);
	if(aCurve.IsNull())
		return FALSE;

	if(!aCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
		return FALSE;

	Handle(Geom_BSplineCurve) aBSpl = Handle(Geom_BSplineCurve)::DownCast(aCurve);
	if(!aBSpl->IsPeriodic())
		return TRUE;

	Handle(Geom_BSplineCurve) aNSpl = Handle(Geom_BSplineCurve)::DownCast(aBSpl->Copy());
	aNSpl->SetNotPeriodic();

	aNSpl->Segment(dFirst,dLast);

	//������entity
	kcEntityFactory efac;
	kcEntity *pEntity = NULL;
	BRepBuilderAPI_MakeEdge me(aNSpl);
	if(me.IsDone()){
		aEdge = me.Edge();
		pEntity = efac.Create(aEdge);
		if(pEntity){
			kcModel *pModel = GetModel();
			pModel->BeginUndo(GetName());
			pModel->AddEntity(pEntity);
			pModel->EndUndo();
		}

		return TRUE;
	}

	return FALSE;
}

