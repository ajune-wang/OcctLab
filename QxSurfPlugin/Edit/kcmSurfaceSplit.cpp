#include "StdAfx.h"
#include <BRepAlgoAPI_Section.hxx>
#include <BRepFeat_SplitShape.hxx>
#include "kiInputEntityTool.h"
#include "kiInputVector.h"
#include "kiInputValueTool.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "QxCurveLib.h"
#include "kvCoordSystem.h"
#include "kcEntityFactory.h"
#include "kcmSurfaceSplit.h"

kcmSurfaceSplit::kcmSurfaceSplit(void)
{
	m_strName = "����ָ�";
	m_strAlias = "SplitSurf";
	m_strDesc = "�ָ�����";

	m_pPickSurfaceTool = NULL;
	m_pPickShapeTool = NULL;
}

kcmSurfaceSplit::~kcmSurfaceSplit(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmSurfaceSplit::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmSurfaceSplit::OnInputFinished(kiInputTool *pTool)
{
	if(pTool->IsInputDone())
	{
		if(pTool == m_pPickSurfaceTool)
		{
			NavToNextTool();
		}
		else if(pTool == m_pPickShapeTool)
		{
			if(DoSplit())
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

int		kcmSurfaceSplit::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmSurfaceSplit::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmSurfaceSplit::CreateInputTools()
{
	m_pPickSurfaceTool = new kiInputEntityTool(this,"ѡ�񱻷ָ�����");
	m_pPickSurfaceTool->SetOption(KCT_ENT_FACE,1);
	m_pPickSurfaceTool->SetNaturalMode(true);

	m_pPickShapeTool = new kiInputEntityTool(this,"ѡ�����ڷָ�Ķ���");
	m_pPickShapeTool->SetOption(KCT_ENT_FACE | KCT_ENT_SHELL | KCT_ENT_SOLID,1);
	m_pPickShapeTool->SetNaturalMode(true);

	return TRUE;
}

BOOL	kcmSurfaceSplit::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pPickSurfaceTool);
	KC_SAFE_DELETE(m_pPickShapeTool);

	return TRUE;
}

BOOL	kcmSurfaceSplit::InitInputToolQueue()
{
	AddInputTool(m_pPickSurfaceTool);
	AddInputTool(m_pPickShapeTool);

	return TRUE;
}

//
BOOL	kcmSurfaceSplit::DoSplit()
{
	if(m_pPickSurfaceTool->GetSelCount() != 1 ||
	   m_pPickShapeTool->GetSelCount() != 1)
	   return FALSE;

	kiSelEntity se = m_pPickSurfaceTool->GetFirstSelect();
	kcEntity *pFaceEnt = se._pEntity;
	TopoDS_Shape aShape = se.SelectShape();//se._pEntity->GetShape();
	
	se = m_pPickShapeTool->GetFirstSelect();
	TopoDS_Shape aShape2 = se.SelectShape() ;//._pEntity->GetShape();

	//��������
	std::vector<kcEntity *> aEnt;
	try{
		BRepAlgoAPI_Section sec(aShape,aShape2,Standard_False);
		sec.ComputePCurveOn1(Standard_True);
		sec.Approximation(Standard_True);
		sec.Build();

		TopoDS_Shape aIntEdges = sec.Shape();
		if(aIntEdges.IsNull())
		{
			AfxMessageBox("û�н���.");
			return FALSE;
		}

		//�ָ�����
		TopoDS_Face aFace = TopoDS::Face(aShape);
		BRepFeat_SplitShape ssplit(aFace);

		TopExp_Explorer ex;
		for(ex.Init(aIntEdges,TopAbs_EDGE);ex.More();ex.Next())
		{
			TopoDS_Edge aE = TopoDS::Edge(ex.Current());
			TopoDS_Shape aF;
			if(sec.HasAncestorFaceOn1(aE,aF))
			{
				TopoDS_Face aAncFace = TopoDS::Face(aF);
				if(aAncFace.IsSame(aFace))
					ssplit.Add(aE,aFace);
			}
		}

		ssplit.Build();

		//�����¶���
		kcEntity *pEnt = NULL;
		kcEntityFactory efac;
		const TopTools_ListOfShape& shapeList = ssplit.Modified(aFace);
		TopTools_ListIteratorOfListOfShape ite(shapeList);
		for(;ite.More();ite.Next())
		{
			TopoDS_Shape aS = ite.Value();
			pEnt = efac.Create(aS);
			if(pEnt)
				aEnt.push_back(pEnt);
		}
	}catch(Standard_Failure){
		return FALSE;
	}

	if(aEnt.empty())
		return FALSE;

	//��ʾ
	kcModel *pModel = GetModel();
	pModel->BeginUndo(GetName());
	{
		//���ؾɵ�
		pModel->DelEntity(pFaceEnt);
		pModel->AddEntity(aEnt);
	}
	pModel->EndUndo();

	return TRUE;
}
