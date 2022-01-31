#include "StdAfx.h"
#include <BRepOffsetAPI_MakePipe.hxx>
#include "kiInputEntityTool.h"
#include "kiInputVector.h"
#include "kiInputValueTool.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "kvCoordSystem.h"
#include "kcEntityFactory.h"
#include "kcmSolidPipe.h"

kcmSolidPipe::kcmSolidPipe(void)
{
	m_strName = "ʵ��ܵ�";
	m_strAlias = "SolidPipe";
	m_strDesc = "ʵ��ܵ�";

	m_pInputSpline = NULL;
	m_pInputRadius = NULL;
	m_dRadius = 2.0;
}

kcmSolidPipe::~kcmSolidPipe(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmSolidPipe::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int		kcmSolidPipe::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputSpline)
	{
		if(m_pInputSpline->IsInputDone())
		{
			//Ӧ�����spline��������
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputRadius)
	{
		if(m_pInputRadius->IsInputDone())
		{
			if(BuildPipe())
			{
				Done();
				m_pInputSpline->ClearSel();
				
				Redraw();

				return KSTA_DONE;
			}
			else
			{
				m_pInputSpline->ClearSel();
				EndCommand(KSTA_FAILED);
				return KSTA_FAILED;
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

BOOL	kcmSolidPipe::BuildPipe()
{
	kiSelSet *pSelSet = m_pInputSpline->GetSelSet();
	if(pSelSet->GetSelCount() != 1)
		return FALSE;
	pSelSet->InitSelected();
	kiSelEntity se = pSelSet->CurSelected();
	TopoDS_Shape aSpline = se.SelectShape();//����ȫ�֣����ֲܾ�
	if(aSpline.IsNull())
		return FALSE;

	TopoDS_Wire aWire;
	if(aSpline.ShapeType() == TopAbs_WIRE)
		aWire = TopoDS::Wire(aSpline);
	else
	{
		TopoDS_Edge aEdge = TopoDS::Edge(aSpline);
		try
		{
			BRepBuilderAPI_MakeWire mw(aEdge);
			if(mw.IsDone())
				aWire = mw.Wire();
		}
		catch (Standard_Failure)
		{

		}
	}
	if(aWire.IsNull())
		return FALSE;

	// ����Բƽ�棬ʹ��spline���������Ϣ��
	TopoDS_Edge aSE;
	if(aSpline.ShapeType() == TopAbs_WIRE)
	{
		TopExp_Explorer ex;
		ex.Init(aSpline,TopAbs_EDGE);
		if(!ex.More())
			return FALSE;
		aSE = TopoDS::Edge(ex.Current());
	}
	else
	{
		aSE = TopoDS::Edge(aSpline);
	}

	TopoDS_Face aFace;
	try{
		double df,dl;
		Handle(Geom_Curve) aCrv = BRep_Tool::Curve(aSE,df,dl);
		if(aCrv.IsNull())
			return FALSE;
		//������ʧ
		gp_Pnt pnt;
		gp_Vec dir;
		aCrv->D1(df,pnt,dir);

		gp_Ax2 ax2(pnt,gp_Dir(dir));

		//����circle
		Handle(Geom_Circle) aCirc = new Geom_Circle(ax2,m_dRadius);
		if(aCirc.IsNull())
			return FALSE;

		BRepBuilderAPI_MakeEdge me(aCirc);
		if(!me.IsDone())
			return FALSE;

		BRepBuilderAPI_MakeWire mw(me.Edge());
		if(!mw.IsDone())
			return FALSE;
		//
		BRepBuilderAPI_MakeFace mf(mw.Wire());
		if(!mf.IsDone())
			return FALSE;

		aFace = mf.Face();

	}catch(Standard_Failure){
		return FALSE;
	}

	//create pipe
	TopoDS_Shape aNS;
	try{
		BRepOffsetAPI_MakePipe mpipe(aWire,aFace);
		if(mpipe.IsDone())
			aNS = mpipe.Shape();
	}catch(Standard_Failure){
	}
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

int		kcmSolidPipe::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmSolidPipe::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmSolidPipe::CreateInputTools()
{
	m_pInputSpline = new kiInputEntityTool(this,"ѡȡһ���Ǹ���");
	m_pInputSpline->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE,1);

	m_pInputRadius = new kiInputDoubleTool(this,"����ܵ��뾶");
	m_pInputRadius->Init(&m_dRadius,0.001,10000);
	//

	return TRUE;
}

BOOL	kcmSolidPipe::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputSpline);
	KC_SAFE_DELETE(m_pInputRadius);

	return TRUE;
}

BOOL	kcmSolidPipe::InitInputToolQueue()
{
	AddInputTool(m_pInputSpline);
	AddInputTool(m_pInputRadius);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
kcmSolidSweep::kcmSolidSweep(void)
{
	m_strName = "ʵ��ɨ��";
	m_strAlias = "SolidSweep";
	m_strDesc = "ʵ�嵥��ɨ��";

	m_pInputSpline = NULL;
	m_pInputProfile = NULL;
}

kcmSolidSweep::~kcmSolidSweep(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmSolidSweep::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int		kcmSolidSweep::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputSpline)
	{
		if(m_pInputSpline->IsInputDone())
		{
			//Ӧ�����spline��������
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputProfile)
	{
		if(m_pInputProfile->IsInputDone())
		{
			if(BuildPipe())
			{
				Done();
				m_pInputSpline->ClearSel();
				m_pInputProfile->ClearSel();
				Redraw();

				return KSTA_DONE;
			}
			else
			{
				m_pInputSpline->ClearSel();
				m_pInputProfile->ClearSel();

				return KSTA_ERROR;
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

BOOL	kcmSolidSweep::BuildPipe()
{
	kiSelSet *pSelSet = m_pInputSpline->GetSelSet();
	if(pSelSet->GetSelCount() != 1)
		return FALSE;
	pSelSet->InitSelected();
	kiSelEntity se = pSelSet->CurSelected();
	TopoDS_Shape aSpline = se.SelectShape();//�����Ǿֲ�����
	if(aSpline.IsNull())
		return FALSE;

	pSelSet = m_pInputProfile->GetSelSet();
	if(pSelSet->GetSelCount() != 1)
		return FALSE;
	pSelSet->InitSelected();
	se = pSelSet->CurSelected();
	TopoDS_Shape aProfile = se.SelectShape();//�����Ǿֲ�����
	if(aProfile.IsNull())
		return FALSE;

	TopoDS_Wire aWire;
	if(aSpline.ShapeType() == TopAbs_WIRE)
		aWire = TopoDS::Wire(aSpline);
	else
	{
		TopoDS_Edge aEdge = TopoDS::Edge(aSpline);
		try
		{
			BRepBuilderAPI_MakeWire mw(aEdge);
			if(mw.IsDone())
				aWire = mw.Wire();
		}
		catch (Standard_Failure)
		{

		}
	}
	if(aWire.IsNull())
		return FALSE;

	TopoDS_Shape aNS;
	try{
		BRepOffsetAPI_MakePipe mpipe(aWire,aProfile);
		if(mpipe.IsDone())
			aNS = mpipe.Shape();
	}catch(Standard_Failure){

	}
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

int		kcmSolidSweep::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmSolidSweep::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmSolidSweep::CreateInputTools()
{
	m_pInputSpline = new kiInputEntityTool(this,"ѡȡһ���Ǹ���");
	m_pInputSpline->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE,1);

	//
	m_pInputProfile = new kiInputEntityTool(this,"ʰȡҪɨ�ԵĶ���(�߻���)");
	m_pInputProfile->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE | KCT_ENT_FACE | KCT_ENT_SHELL,1);

	return TRUE;
}

BOOL	kcmSolidSweep::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputSpline);
	KC_SAFE_DELETE(m_pInputProfile);

	return TRUE;
}

BOOL	kcmSolidSweep::InitInputToolQueue()
{
	AddInputTool(m_pInputSpline);
	AddInputTool(m_pInputProfile);

	return TRUE;
}