#include "StdAfx.h"
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include "kiInputEntityTool.h"
#include "kiInputVector.h"
#include "kiInputValueTool.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "QxCurveLib.h"
#include "kvCoordSystem.h"
#include "kcEntityFactory.h"
#include "kcmSolidPipeShell.h"

kcmSolidPipeShell::kcmSolidPipeShell(void)
{
	m_strName = "ʵ�����ɹܵ�";
	m_strAlias = "SolidPipeShell";
	m_strDesc = "ʵ�����ɹܵ�";

	m_pInputSpline = NULL;
	m_pInputRadius = NULL;
	m_dRadius = 2.0;

	m_pModelOptItem = NULL;
	m_pTransitionOptItem = NULL;
	m_bBuildSolid = false;
	m_bContact = false;
	m_bCorrect = false;
	m_nModel = 1;
	m_nTranstion = 0;
}

kcmSolidPipeShell::~kcmSolidPipeShell(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmSolidPipeShell::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int		kcmSolidPipeShell::OnInputFinished(kiInputTool *pTool)
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

BOOL	kcmSolidPipeShell::BuildPipe()
{
	kiSelSet *pSelSet = m_pInputSpline->GetSelSet();
	if(pSelSet->GetSelCount() != 1)
		return FALSE;
	pSelSet->InitSelected();
	kiSelEntity se = pSelSet->CurSelected();
	TopoDS_Shape aSpline = se.SelectShape();//�����Ǿֲ�����
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

	TopoDS_Wire aProfile;
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
		aProfile = mw.Wire();
	}catch(Standard_Failure){
		return FALSE;
	}

	//create pipe
	TopoDS_Shape aNS;
	try{
		BRepOffsetAPI_MakePipeShell mps(aWire);
		if(m_nModel == 0)
			mps.SetMode(Standard_True);
		else
			mps.SetMode(Standard_False);
		if(m_nTranstion == 0)
			mps.SetTransitionMode(BRepBuilderAPI_Transformed);
		else if(m_nTranstion == 1)
			mps.SetTransitionMode(BRepBuilderAPI_RightCorner);
		else
			mps.SetTransitionMode(BRepBuilderAPI_RoundCorner);
		//
		mps.Add(aProfile,m_bContact,m_bCorrect);
		if(!mps.IsReady())
			return FALSE;

		mps.Build();
		if(!mps.IsDone())
			return FALSE;

		aNS = mps.Shape();
		if(m_bBuildSolid)
		{
			if(mps.MakeSolid())
				aNS = mps.Shape();
		}
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

int		kcmSolidPipeShell::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmSolidPipeShell::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmSolidPipeShell::CreateInputTools()
{
	//����ѡ��
	m_optionSet.AddBoolOption("ʵ��",'S',m_bBuildSolid);
	m_optionSet.AddBoolOption("�Ӵ�",'T',m_bContact);
	m_optionSet.AddBoolOption("��������",'C',m_bCorrect);

	m_pModelOptItem = new kiOptionEnum("ģʽ",'M',m_nModel);
	m_pModelOptItem->AddEnumItem("��׼���",'F');
	m_pModelOptItem->AddEnumItem("�������",'C');
	//m_pModelOptItem->AddEnumItem("�̶����",'X');
	//m_pModelOptItem->AddEnumItem("�̶�����",'D');
	//m_pModelOptItem->AddEnumItem("��������",'A');
	m_optionSet.AddOption(m_pModelOptItem);

	m_pTransitionOptItem = new kiOptionEnum("ת��ģʽ",'R',m_nTranstion);
	m_pTransitionOptItem->AddEnumItem("Transformed",'T');
	m_pTransitionOptItem->AddEnumItem("RightCorner",'R');
	m_pTransitionOptItem->AddEnumItem("RoundCorner",'N');
	m_optionSet.AddOption(m_pTransitionOptItem);

	m_pInputSpline = new kiInputEntityTool(this,"ѡȡһ���Ǹ���",&m_optionSet);
	m_pInputSpline->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE,1);

	m_pInputRadius = new kiInputDoubleTool(this,"����ܵ��뾶");
	m_pInputRadius->Init(&m_dRadius,0.001,10000);
	//

	return TRUE;
}

BOOL	kcmSolidPipeShell::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputSpline);
	KC_SAFE_DELETE(m_pInputRadius);

	return TRUE;
}

BOOL	kcmSolidPipeShell::InitInputToolQueue()
{
	AddInputTool(m_pInputSpline);
	AddInputTool(m_pInputRadius);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
kcmSolidSweepShell::kcmSolidSweepShell(void)
{
	m_strName = "ʵ�岻����ɨ��";
	m_strAlias = "SolidSweepShell";
	m_strDesc = "���߲�����ɨ��";

	m_pInputSpline = NULL;
	m_pInputProfile = NULL;

	m_pModelOptItem = NULL;
	m_pTransitionOptItem = NULL;
	m_bBuildSolid = false;
	m_bContact = false;
	m_bCorrect = false;
	m_nModel = 1;
	m_nTranstion = 0;
}

kcmSolidSweepShell::~kcmSolidSweepShell(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmSolidSweepShell::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int		kcmSolidSweepShell::OnInputFinished(kiInputTool *pTool)
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

BOOL	kcmSolidSweepShell::BuildPipe()
{
	//��ȡ�Ǹ��ߣ�wire
	kiSelSet *pSelSet = m_pInputSpline->GetSelSet();
	if(pSelSet->GetSelCount() != 1)
		return FALSE;
	pSelSet->InitSelected();
	kiSelEntity se = pSelSet->CurSelected();
	TopoDS_Shape aSpline = se._pEntity->GetShape();
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

	// get aprofile,wire or vertex.must be wire
	pSelSet = m_pInputProfile->GetSelSet();
	if(pSelSet->GetSelCount() != 1)
		return FALSE;
	pSelSet->InitSelected();
	se = pSelSet->CurSelected();
	TopoDS_Shape aProfile = se._pEntity->GetShape();
	if(aProfile.IsNull())
		return FALSE;

	TopoDS_Wire aProfileWire;
	if(aProfile.ShapeType() == TopAbs_WIRE)
		aProfileWire = TopoDS::Wire(aProfile);
	else
	{
		TopoDS_Edge aE = TopoDS::Edge(aProfile);
		try{
			BRepBuilderAPI_MakeWire mw(aE);
			if(mw.IsDone())
				aProfileWire = mw.Wire();
		}catch(Standard_Failure){}
	}
	if(aProfileWire.IsNull())
		return FALSE;

	TopoDS_Shape aNS;
	try{
		BRepOffsetAPI_MakePipeShell mps(aWire);
		if(m_nModel == 0)
			mps.SetMode(Standard_True);
		else
			mps.SetMode(Standard_False);
		if(m_nTranstion == 0)
			mps.SetTransitionMode(BRepBuilderAPI_Transformed);
		else if(m_nTranstion == 1)
			mps.SetTransitionMode(BRepBuilderAPI_RightCorner);
		else
			mps.SetTransitionMode(BRepBuilderAPI_RoundCorner);
		//
		mps.Add(aProfileWire,m_bContact,m_bCorrect);
		if(!mps.IsReady())
			return FALSE;

		mps.Build();
		if(!mps.IsDone())
			return FALSE;

		aNS = mps.Shape();
		if(m_bBuildSolid && QxCurveLib::IsWireClosed(aProfileWire))
		{
			if(mps.MakeSolid())
				aNS = mps.Shape();
		}
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

int		kcmSolidSweepShell::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmSolidSweepShell::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmSolidSweepShell::CreateInputTools()
{
	//����ѡ��
	m_optionSet.AddBoolOption("ʵ��",'S',m_bBuildSolid);
	m_optionSet.AddBoolOption("�Ӵ�",'T',m_bContact);
	m_optionSet.AddBoolOption("��������",'C',m_bCorrect);

	m_pModelOptItem = new kiOptionEnum("ģʽ",'M',m_nModel);
	m_pModelOptItem->AddEnumItem("��׼���",'F');
	m_pModelOptItem->AddEnumItem("�������",'C');
	//m_pModelOptItem->AddEnumItem("�̶����",'X');
	//m_pModelOptItem->AddEnumItem("�̶�����",'D');
	//m_pModelOptItem->AddEnumItem("��������",'A');
	m_optionSet.AddOption(m_pModelOptItem);

	m_pTransitionOptItem = new kiOptionEnum("ת��ģʽ",'R',m_nTranstion);
	m_pTransitionOptItem->AddEnumItem("Transformed",'T');
	m_pTransitionOptItem->AddEnumItem("RightCorner",'R');
	m_pTransitionOptItem->AddEnumItem("RoundCorner",'N');
	m_optionSet.AddOption(m_pTransitionOptItem);

	m_pInputSpline = new kiInputEntityTool(this,"ѡȡһ���Ǹ���",&m_optionSet);
	m_pInputSpline->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE,1);

	//
	m_pInputProfile = new kiInputEntityTool(this,"ʰȡҪɨ�ԵĶ���(�߻���)");
	m_pInputProfile->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE ,1);

	return TRUE;
}

BOOL	kcmSolidSweepShell::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputSpline);
	KC_SAFE_DELETE(m_pInputProfile);

	return TRUE;
}

BOOL	kcmSolidSweepShell::InitInputToolQueue()
{
	AddInputTool(m_pInputSpline);
	AddInputTool(m_pInputProfile);

	return TRUE;
}