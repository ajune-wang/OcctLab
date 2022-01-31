#include "StdAfx.h"
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <BOPDS_DS.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include "kcEntity.h"
#include "kcModel.h"
#include "QxBRepLib.h"
#include "kiInputEntityTool.h"
#include "kiInputValueTool.h"
#include "kcEntityFactory.h"
#include "kiInputPicker.h"
#include "kcmCurveTrim.h"

kcmCurveTrim::kcmCurveTrim(void)
{
	m_strName = "���߲ü�";
	m_strAlias = "CrvTrim";
	m_strDesc = "���߲ü�";

	m_pInputCurve = NULL;
	m_pInputCurveToTrim = NULL;

	m_nTrimCount = 0;
	m_bCanTrim = FALSE;
}

kcmCurveTrim::~kcmCurveTrim(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmCurveTrim::CanFinish()//�����Ƿ�������
{
	if(m_nTrimCount > 0)
		return TRUE;
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int		kcmCurveTrim::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputCurve)
	{
		if(m_pInputCurve->IsInputDone())
		{
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputCurveToTrim)
	{
		if(m_pInputCurveToTrim->IsInputDone())
		{
			if(!DoSplit())
			{
				Done();
				return KSTA_DONE;
			}
			else
			{
				m_bCanTrim = TRUE;
				NavToNextTool();
			}
		}
	}
	return KSTA_CONTINUE;
}

int		kcmCurveTrim::OnExecute()
{
	m_listOfObj.Clear();
	m_nTrimCount = 0;
	m_bCanTrim = FALSE;

	return KSTA_CONTINUE;
}

int		kcmCurveTrim::OnEnd(int nCode)
{
	if(nCode == KSTA_CANCEL)//ȡ����Ӧ��������ʾ
	{
		if(m_pInputCurveToTrim->GetSelCount() == 1)
		{
			kiSelEntity se = m_pInputCurveToTrim->GetFirstSelect();
			se._pEntity->Display(true,TRUE);
		}
	}

	//ȥ����ʱ��ʾ����
	Handle(AIS_InteractiveContext) aCtx = GetAISContext();
	AIS_ListIteratorOfListOfInteractive ite(m_listOfObj);
	for(;ite.More();ite.Next())
	{
		Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(ite.Value());
		aCtx->Remove(aObj,Standard_True);
	}

	m_bCanTrim = FALSE;

	return nCode;
}

int		kcmCurveTrim::OnApply()
{
	if(m_pInputCurveToTrim->GetSelCount() != 1)
	{
		EndCommand(KSTA_CANCEL);
		return KSTA_CANCEL;
	}

	kiSelEntity se = m_pInputCurveToTrim->GetFirstSelect();

	kcEntityFactory efac;
	std::vector<kcEntity *> aEnt;
	Handle(AIS_InteractiveContext) aCtx = GetAISContext();
	AIS_ListIteratorOfListOfInteractive ite(m_listOfObj);
	for(;ite.More();ite.Next())
	{
		Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(ite.Value());
		TopoDS_Shape aS = aObj->Shape();
		kcEntity *pEnt = efac.Create(aS);
		if(pEnt)
			aEnt.push_back(pEnt);

		aCtx->Remove(aObj,Standard_True);
	}

	if(m_nTrimCount == 0)
		se._pEntity->Display(true,FALSE);

	kcModel *pModel = GetModel();
	pModel->BeginUndo(GetName());
	{
		if(m_nTrimCount > 0)
			pModel->DelEntity(se._pEntity);//ɾ���ɵ�
		//����µ�
		int ix,isize = (int)aEnt.size();
		for(ix = 0;ix < isize;ix ++)
			pModel->AddEntity(aEnt[ix]);
	}
	pModel->EndUndo();

	Done();
	Redraw();

	return KSTA_DONE;
}

int		kcmCurveTrim::OnLButtonUp(kuiMouseInput& mouseInput)
{
	kiCommand::OnLButtonUp(mouseInput);

	if(m_bCanTrim)
	{
		DoTrim(mouseInput.m_x,mouseInput.m_y);
	}

	return KSTA_CONTINUE;
}

int		kcmCurveTrim::OnMouseMove(kuiMouseInput& mouseInput)
{
	kiCommand::OnMouseMove(mouseInput);

	if(m_bCanTrim)
	{
		Handle(AIS_InteractiveContext) aCtx = GetAISContext();
		Handle(V3d_View) aView = GetCurrV3dView();
		if(!aView.IsNull() && !aCtx.IsNull())
			aCtx->MoveTo(mouseInput.m_x,mouseInput.m_y,aView,Standard_True);
	}

	return KSTA_CONTINUE;
}

// ������Ҫ�����빤��
BOOL	kcmCurveTrim::CreateInputTools()
{
	m_pInputCurve = new kiInputEntityTool(this,"ѡ�����ڲü�������");
	m_pInputCurve->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE,1);

	m_pInputCurveToTrim = new kiInputEntityTool(this,"ѡ��Ҫ�ü�������");
	m_pInputCurveToTrim->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE,1);

	return TRUE;
}

BOOL	kcmCurveTrim::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputCurve);
	KC_SAFE_DELETE(m_pInputCurveToTrim);

	return TRUE;
}

BOOL	kcmCurveTrim::InitInputToolQueue()
{
	AddInputTool(m_pInputCurve);
	AddInputTool(m_pInputCurveToTrim);

	return TRUE;
}

// ���зָ�
BOOL	kcmCurveTrim::DoSplit()
{
	if(m_pInputCurve->GetSelCount() <= 0 ||
	   m_pInputCurveToTrim->GetSelCount() <= 0)
	   return FALSE;

	kiSelEntity se1 = m_pInputCurve->GetFirstSelect();
	kiSelEntity se2 = m_pInputCurveToTrim->GetFirstSelect();
	TopoDS_Shape aS1 = se1._pEntity->GetShape();
	TopoDS_Shape aS2 = se2._pEntity->GetShape();

	//ʹ��aS1ȥ�ָ�aS2
	int nC = 0;
	TopTools_SequenceOfShape seqOfShape;
	if(aS2.ShapeType() == TopAbs_EDGE)
	{
		TopoDS_Edge aEdge = TopoDS::Edge(aS2);
		if(aS1.ShapeType() == TopAbs_EDGE)
			nC = kcgEdgeSplit::SplitEdgeByEdge(aEdge,TopoDS::Edge(aS1),seqOfShape);
		else
			nC = kcgEdgeSplit::SplitEdgeByWire(aEdge,TopoDS::Wire(aS1),seqOfShape);
	}
	else
	{
		TopoDS_Wire aWire = TopoDS::Wire(aS2);
		if(aS1.ShapeType() == TopAbs_EDGE)
			nC = kcgWireSplit::SplitWireByEdge(aWire,TopoDS::Edge(aS1),seqOfShape);
		else
			nC = kcgWireSplit::SplitWireByWire(aWire,TopoDS::Wire(aS1),seqOfShape);
	}
	if(nC == 0)//û�������ܲü�
		return TRUE;

	//������Ӧ�Ķ��󣬲���ʾ��
	Handle(AIS_InteractiveContext) aCtx = GetAISContext();
	Handle(AIS_Shape) aObj;
	int ix,nbShape = seqOfShape.Length();
	for(ix = 1;ix <= nbShape;ix ++)
	{
		aObj = new AIS_Shape(seqOfShape.Value(ix));
		if(aObj.IsNull())
			continue;

		aCtx->Display(aObj,Standard_False);
		m_listOfObj.Append(aObj);
	}
	//���ؾɵ�
	se2._pEntity->Display(false,TRUE);

	//�ȴ��ü�
	m_nTrimCount = 0;

	return TRUE;
}

// �ü�
BOOL	kcmCurveTrim::DoTrim(int x,int y)
{
	double t;
	kPoint3 pp;
	Handle(AIS_InteractiveContext) aCtx = GetAISContext();
	AIS_ListIteratorOfListOfInteractive ite(m_listOfObj);
	for(;ite.More();ite.Next())
	{
		Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(ite.Value());
		TopoDS_Shape aS = aObj->Shape();
		kiCurvePicker cpick(GetDocContext());
		if(aS.ShapeType() == TopAbs_EDGE)
			cpick.Init(TopoDS::Edge(aS));
		else
			cpick.Init(TopoDS::Wire(aS));

		//�������
		if(cpick.Pick(x,y,5,t,pp))
		{
			m_listOfObj.Remove(ite);//���
			aCtx->Remove(aObj,Standard_True);
			m_nTrimCount ++;
			break;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// ���ٲü�
kcmCurveQuickTrim::kcmCurveQuickTrim(void)
{
	m_strName = "���ٲü�";
	m_strAlias = "CrvQuickTrim";
	m_strDesc = "�����߿��ٲü�";

	m_pInputCurve1 = NULL;
	m_pInputCurve2 = NULL;

	m_nTrimCount1 = 0;
	m_nTrimCount2 = 0;
	m_bCanTrim = FALSE;
}

kcmCurveQuickTrim::~kcmCurveQuickTrim(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmCurveQuickTrim::CanFinish()//�����Ƿ�������
{
	if(m_nTrimCount1 > 0 || m_nTrimCount2 > 0)
		return TRUE;
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int		kcmCurveQuickTrim::OnInputFinished(kiInputTool *pTool)
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
			if(!DoSplit())
			{
				Done();
				return KSTA_DONE;
			}
			else
			{
				m_bCanTrim = TRUE;
				NavToNextTool();
			}
		}
	}
	return KSTA_CONTINUE;
}

int		kcmCurveQuickTrim::OnExecute()
{
	m_aObjList1.Clear();
	m_aObjList2.Clear();
	m_nTrimCount1 = 0;
	m_nTrimCount2 = 0;
	m_bCanTrim = FALSE;

	return KSTA_CONTINUE;
}

BOOL	kcmCurveQuickTrim::RemoveTmpObjList(AIS_ListOfInteractive& objList)
{
	Handle(AIS_InteractiveContext) aCtx = GetAISContext();
	AIS_ListIteratorOfListOfInteractive ite(objList);
	for(;ite.More();ite.Next())
	{
		Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(ite.Value());
		aCtx->Remove(aObj,Standard_True);
	}
	return TRUE;
}

int		kcmCurveQuickTrim::OnEnd(int nCode)
{
	if(nCode == KSTA_CANCEL)//ȡ����Ӧ��������ʾ
	{
		if(m_pInputCurve1->GetSelCount() == 1)
		{
			kiSelEntity se = m_pInputCurve1->GetFirstSelect();
			se._pEntity->Display(true,TRUE);
		}
		if(m_pInputCurve2->GetSelCount() == 1)
		{
			kiSelEntity se = m_pInputCurve2->GetFirstSelect();
			se._pEntity->Display(true,TRUE);
		}
	}

	//ȥ����ʱ��ʾ����
	RemoveTmpObjList(m_aObjList1);
	RemoveTmpObjList(m_aObjList2);

	m_bCanTrim = FALSE;

	return nCode;
}

int		kcmCurveQuickTrim::OnApply()
{
	if(m_pInputCurve1->GetSelCount() != 1 ||
	   m_pInputCurve2->GetSelCount() != 1)
	{
		EndCommand(KSTA_CANCEL);
		return KSTA_CANCEL;
	}

	kiSelEntity se1 = m_pInputCurve1->GetFirstSelect();
	kiSelEntity se2 = m_pInputCurve2->GetFirstSelect();

	kcEntityFactory efac;
	std::vector<kcEntity *> aEnt;
	Handle(AIS_InteractiveContext) aCtx = GetAISContext();
	if(m_nTrimCount1 == 0)
		se1._pEntity->Display(true,FALSE);//������ʾ
	else
	{
		AIS_ListIteratorOfListOfInteractive ite(m_aObjList1);
		for(;ite.More();ite.Next())
		{
			Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(ite.Value());
			TopoDS_Shape aS = aObj->Shape();
			kcEntity *pEnt = efac.Create(aS);
			if(pEnt)
				aEnt.push_back(pEnt);

			aCtx->Remove(aObj,Standard_True);
		}
	}
	
	if(m_nTrimCount2 == 0)
		se2._pEntity->Display(true,FALSE);
	else
	{
		AIS_ListIteratorOfListOfInteractive ite(m_aObjList2);
		for(;ite.More();ite.Next())
		{
			Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(ite.Value());
			TopoDS_Shape aS = aObj->Shape();
			kcEntity *pEnt = efac.Create(aS);
			if(pEnt)
				aEnt.push_back(pEnt);

			aCtx->Remove(aObj,Standard_True);
		}
	}

	kcModel *pModel = GetModel();
	pModel->BeginUndo(GetName());
	{
		//ֻ�вü��˵Ĳ�ɾ��
		if(m_nTrimCount1 > 0)
			pModel->DelEntity(se1._pEntity);//ɾ���ɵ�
		if(m_nTrimCount2 > 0)
			pModel->DelEntity(se2._pEntity);
		//����µ�
		int ix,isize = (int)aEnt.size();
		for(ix = 0;ix < isize;ix ++)
			pModel->AddEntity(aEnt[ix]);
	}
	pModel->EndUndo();

	Done();
	Redraw();

	return KSTA_DONE;
}

int		kcmCurveQuickTrim::OnLButtonUp(kuiMouseInput& mouseInput)
{
	kiCommand::OnLButtonUp(mouseInput);

	if(m_bCanTrim)
	{
		DoTrim(mouseInput.m_x,mouseInput.m_y);
	}

	return KSTA_CONTINUE;
}

int		kcmCurveQuickTrim::OnMouseMove(kuiMouseInput& mouseInput)
{
	kiCommand::OnMouseMove(mouseInput);

	if(m_bCanTrim)
	{
		Handle(AIS_InteractiveContext) aCtx = GetAISContext();
		Handle(V3d_View) aView = GetCurrV3dView();
		if(!aCtx.IsNull() && !aView.IsNull())
			aCtx->MoveTo(mouseInput.m_x,mouseInput.m_y,aView,Standard_True);
	}

	return KSTA_CONTINUE;
}

// ������Ҫ�����빤��
BOOL	kcmCurveQuickTrim::CreateInputTools()
{
	m_pInputCurve1 = new kiInputEntityTool(this,"ѡ���һ������");
	m_pInputCurve1->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE,1);

	m_pInputCurve2 = new kiInputEntityTool(this,"ѡ��ڶ�������");
	m_pInputCurve2->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE,1);

	return TRUE;
}

BOOL	kcmCurveQuickTrim::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputCurve1);
	KC_SAFE_DELETE(m_pInputCurve2);

	return TRUE;
}

BOOL	kcmCurveQuickTrim::InitInputToolQueue()
{
	AddInputTool(m_pInputCurve1);
	AddInputTool(m_pInputCurve2);

	return TRUE;
}

// ���зָ�
BOOL	kcmCurveQuickTrim::DoSplit()
{
	if(m_pInputCurve1->GetSelCount() <= 0 ||
	   m_pInputCurve2->GetSelCount() <= 0)
		return FALSE;

	kiSelEntity se1 = m_pInputCurve1->GetFirstSelect();
	kiSelEntity se2 = m_pInputCurve2->GetFirstSelect();
	TopoDS_Shape aS1 = se1._pEntity->GetShape();
	TopoDS_Shape aS2 = se2._pEntity->GetShape();

	int nC1 = 0,nC2 = 0;
	TopTools_SequenceOfShape seqOfShape1,seqOfShape2;
	//ʹ��aS1ȥ�ָ�aS2
	if(aS2.ShapeType() == TopAbs_EDGE)
	{
		TopoDS_Edge aEdge = TopoDS::Edge(aS2);
		if(aS1.ShapeType() == TopAbs_EDGE)
			nC1 = kcgEdgeSplit::SplitEdgeByEdge(aEdge,TopoDS::Edge(aS1),seqOfShape2);
		else
			nC1 = kcgEdgeSplit::SplitEdgeByWire(aEdge,TopoDS::Wire(aS1),seqOfShape2);
	}
	else
	{
		TopoDS_Wire aWire = TopoDS::Wire(aS2);
		if(aS1.ShapeType() == TopAbs_EDGE)
			nC1 = kcgWireSplit::SplitWireByEdge(aWire,TopoDS::Edge(aS1),seqOfShape2);
		else
			nC1 = kcgWireSplit::SplitWireByWire(aWire,TopoDS::Wire(aS1),seqOfShape2);
	}
	//ʹ��aS2ȥ�ָ�aS1
	if(aS1.ShapeType() == TopAbs_EDGE)
	{
		TopoDS_Edge aEdge = TopoDS::Edge(aS1);
		if(aS2.ShapeType() == TopAbs_EDGE)
			nC2 = kcgEdgeSplit::SplitEdgeByEdge(aEdge,TopoDS::Edge(aS2),seqOfShape1);
		else
			nC2 = kcgEdgeSplit::SplitEdgeByWire(aEdge,TopoDS::Wire(aS2),seqOfShape1);
	}
	else
	{
		TopoDS_Wire aWire = TopoDS::Wire(aS1);
		if(aS2.ShapeType() == TopAbs_EDGE)
			nC2 = kcgWireSplit::SplitWireByEdge(aWire,TopoDS::Edge(aS2),seqOfShape1);
		else
			nC2 = kcgWireSplit::SplitWireByWire(aWire,TopoDS::Wire(aS2),seqOfShape1);
	}
	if(nC1 == 0 && nC2 == 0)
		return FALSE;

	//������Ӧ�Ķ��󣬲���ʾ��
	Handle(AIS_InteractiveContext) aCtx = GetAISContext();
	Handle(AIS_Shape) aObj;
	int ix,nbShape = seqOfShape1.Length();
	for(ix = 1;ix <= nbShape;ix ++)
	{
		aObj = new AIS_Shape(seqOfShape1.Value(ix));
		if(aObj.IsNull())
			continue;

		aCtx->Display(aObj,Standard_False);
		m_aObjList1.Append(aObj);
	}
	//
	nbShape = seqOfShape2.Length();
	for(ix = 1;ix <= nbShape;ix ++)
	{
		aObj = new AIS_Shape(seqOfShape2.Value(ix));
		if(aObj.IsNull())
			continue;

		aCtx->Display(aObj,Standard_False);
		m_aObjList2.Append(aObj);
	}
	//���ؾɵ�
	se1._pEntity->Display(false,TRUE);
	se2._pEntity->Display(false,TRUE);

	//�ȴ��ü�
	m_nTrimCount1 = 0;
	m_nTrimCount2 = 0;

	return TRUE;
}

// �ü�
BOOL	kcmCurveQuickTrim::DoTrim(int x,int y)
{
	double t;
	kPoint3 pp;
	Handle(AIS_InteractiveContext) aCtx = GetAISContext();
	AIS_ListIteratorOfListOfInteractive ite(m_aObjList1);
	for(;ite.More();ite.Next())
	{
		Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(ite.Value());
		TopoDS_Shape aS = aObj->Shape();
		kiCurvePicker cpick(GetDocContext());
		if(aS.ShapeType() == TopAbs_EDGE)
			cpick.Init(TopoDS::Edge(aS));
		else
			cpick.Init(TopoDS::Wire(aS));

		//�������
		if(cpick.Pick(x,y,5,t,pp))
		{
			m_aObjList1.Remove(ite);//���
			aCtx->Remove(aObj,Standard_True);
			m_nTrimCount1 ++;
			return TRUE;
		}
	}
	//
	ite.Initialize(m_aObjList2);
	for(;ite.More();ite.Next())
	{
		Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(ite.Value());
		TopoDS_Shape aS = aObj->Shape();
		kiCurvePicker cpick(GetDocContext());
		if(aS.ShapeType() == TopAbs_EDGE)
			cpick.Init(TopoDS::Edge(aS));
		else
			cpick.Init(TopoDS::Wire(aS));

		//�������
		if(cpick.Pick(x,y,5,t,pp))
		{
			m_aObjList2.Remove(ite);//���
			aCtx->Remove(aObj,Standard_True);
			m_nTrimCount2 ++;
			return TRUE;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
kcmSplitAllEdges::kcmSplitAllEdges(void)
{
	m_strName = "���ߴ��";
	m_strAlias = "SplitAllEdges";
	m_strDesc = "���߲ü�";

	m_pInputEdges = NULL;
	
	m_nTrimCount = 0;
	m_bCanTrim = FALSE;
}

kcmSplitAllEdges::~kcmSplitAllEdges(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL kcmSplitAllEdges::CanFinish()//�����Ƿ�������
{
	return TRUE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int kcmSplitAllEdges::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEdges)
	{
		if(m_pInputEdges->IsInputDone())
		{
			if(DoSplit())
			{
				Done();
				return KSTA_DONE;
			}
		}
	}
	return KSTA_CONTINUE;
}

int kcmSplitAllEdges::OnExecute()
{
	m_listOfObj.Clear();
	m_nTrimCount = 0;
	m_bCanTrim = FALSE;

	return KSTA_CONTINUE;
}

int kcmSplitAllEdges::OnEnd(int nCode)
{
	if(nCode == KSTA_CANCEL)//ȡ����Ӧ��������ʾ
	{
		//if(m_pInputCurveToTrim->GetSelCount() == 1)
		//{
		//	kiSelEntity se = m_pInputCurveToTrim->GetFirstSelect();
		//	se._pEntity->Display(TRUE);
		//}
	}

	//ȥ����ʱ��ʾ����
	Handle(AIS_InteractiveContext) aCtx = GetAISContext();
	AIS_ListIteratorOfListOfInteractive ite(m_listOfObj);
	for(;ite.More();ite.Next())
	{
		Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(ite.Value());
		aCtx->Remove(aObj,Standard_True);
	}

	m_bCanTrim = FALSE;

	return nCode;
}

int kcmSplitAllEdges::OnApply()
{
	return KSTA_DONE;
}

int kcmSplitAllEdges::OnLButtonUp(kuiMouseInput& mouseInput)
{
	kiCommand::OnLButtonUp(mouseInput);

	return KSTA_CONTINUE;
}

int kcmSplitAllEdges::OnMouseMove(kuiMouseInput& mouseInput)
{
	kiCommand::OnMouseMove(mouseInput);

	return KSTA_CONTINUE;
}

// ������Ҫ�����빤��
BOOL kcmSplitAllEdges::CreateInputTools()
{
	m_pInputEdges = new kiInputEntityTool(this,"ѡ��Ҫ�໥��ϵ�edge");
	m_pInputEdges->SetOption(KCT_ENT_EDGE,false);
	m_pInputEdges->SetNaturalMode(true);
	
	return TRUE;
}

BOOL kcmSplitAllEdges::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEdges);


	return TRUE;
}

BOOL kcmSplitAllEdges::InitInputToolQueue()
{
	AddInputTool(m_pInputEdges);

	return TRUE;
}

// ���зָ�
BOOL kcmSplitAllEdges::DoSplit()
{
	if(m_pInputEdges->GetSelCount() < 2){
		return FALSE;
	}
	
	kiSelEntityArray selEntAry;
	TopTools_ListOfShape aEdgeList;
	TopoDS_Shape aE;
	kiSelSet *pSelSet = m_pInputEdges->GetSelSet();
	for(pSelSet->InitSelected();pSelSet->MoreSelected();pSelSet->NextSelected()){
		kiSelEntity se = pSelSet->CurSelected();
		aE = se.EntityShape();
		if(!aE.IsNull()){
			aEdgeList.Append(aE);
			selEntAry.push_back(se);
		}
	}
	if(aEdgeList.Size() < 2){
		return FALSE;
	}

	BOPAlgo_PaveFiller aPF;
	aPF.SetArguments(aEdgeList);
	aPF.SetFuzzyValue(1.0e-5);
	
	aPF.Perform();
	if(aPF.HasErrors()){
		AfxMessageBox("BOPAlgo_PaveFiller failed.");
		return TRUE;
	}

	//����MakeBlocks�����ƴ���
	BOPDS_PDS pDS = aPF.PDS();
	if(pDS->NbSourceShapes() == pDS->NbShapes()){//û�������µĶ���
		return TRUE;
	}

	kiSelEntityArray aRmEntAry;
	TopTools_ListOfShape aNewSList;
	TopTools_ListIteratorOfListOfShape aIteEdge;
	kiSelEntityArray::size_type isx = 0;

	for(aIteEdge.Initialize(aEdgeList);aIteEdge.More();aIteEdge.Next(),isx++){
		aE = aIteEdge.Value();
		int idx = pDS->Index(aE);
		//
		if(!pDS->HasPaveBlocks(idx))
			continue;

		const BOPDS_ListOfPaveBlock& aPBList = pDS->PaveBlocks(idx);
		if(aPBList.Size() <= 1)
			continue;

		//�������ˣ���¼�µ�
		aRmEntAry.push_back(selEntAry[isx]);
		BOPDS_ListIteratorOfListOfPaveBlock aItePB;
		for(aItePB.Initialize(aPBList);aItePB.More();aItePB.Next()){
			const Handle(BOPDS_PaveBlock) &aPB = aItePB.Value();
			int ex = aPB->Edge();
			if(ex >= 0 && pDS->IsNewShape(ex)){
				aNewSList.Append(pDS->Shape(ex));
			}
		}
	}

	kcEntityFactory entFac;
	kcEntity *pEnt;
	kcModel *pModel = GetModel();

	pModel->BeginUndo(GetName());
	//ɾ���ɵ�
	for(isx = 0;isx < aRmEntAry.size();isx ++){
		pModel->DelEntity(aRmEntAry[isx].Entity());
	}
	//����µ�
	for(aIteEdge.Initialize(aNewSList);aIteEdge.More();aIteEdge.Next()){
		pEnt = entFac.Create(aIteEdge.Value());
		if(pEnt){
			pModel->AddEntity(pEnt);
		}
	}

	pModel->EndUndo();

	return TRUE;
}

