#include "StdAfx.h"
#include "kiInputEntityTool.h"
#include "kiInputVector.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "kvCoordSystem.h"
#include "kcmPrismSurface.h"

kcmPrismSurface::kcmPrismSurface(void)
{
	m_strName = "Prism Surface";
	m_strAlias = "PrmSurf";
	m_strDesc = "create prism surface";

	m_pInputEntity = NULL;
	m_pInputVec = NULL;
}

kcmPrismSurface::~kcmPrismSurface(void)
{
}

int		kcmPrismSurface::OnExecute()
{
	return KSTA_CONTINUE;
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmPrismSurface::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmPrismSurface::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntity)
	{
		if(pTool->IsInputDone())
		{
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputVec)
	{
		if(pTool->IsInputDone())
		{

			if(MakePrismSurface())
			{
				m_pInputEntity->ClearSel();
				Done();
				Redraw();

				return KSTA_DONE;
			}
		}
	}
	return KSTA_CONTINUE;
}

//
BOOL	kcmPrismSurface::MakePrismSurface()
{
	kiSelSet *pSelSet = m_pInputEntity->GetSelSet();
	if(pSelSet->GetSelCount() != 1)
		return FALSE;

	pSelSet->InitSelected();
	kiSelEntity se = pSelSet->CurSelected();
	if(!IS_EDGE_ENTITY(se._pEntity) && !IS_WIRE_ENTITY(se._pEntity))
		return FALSE;

	kcEntity *pNewEntity = NULL;
	kVector3 vec;
	if(!m_pInputVec->GetVector(vec))
		return FALSE;

	if(IS_EDGE_ENTITY(se._pEntity))
	{
		kcEdgeEntity *pCurEnt = (kcEdgeEntity *)se._pEntity;
		TopoDS_Edge aEdge = pCurEnt->GetEdge();
	
		TopoDS_Face aFace;
		try
		{
			BRepPrimAPI_MakePrism mp(aEdge,gp_Vec(vec[0],vec[1],vec[2]),Standard_True);
			if(mp.IsDone())
			{
				TopoDS_Shape aS = mp.Shape();
				if(!aS.IsNull() && aS.ShapeType() == TopAbs_FACE)
					aFace = TopoDS::Face(aS);
			}
		}
		catch (Standard_Failure){
			return FALSE;
		}

		if(aFace.IsNull())
			return FALSE;

		kcFaceEntity *pSurfEnt = new kcFaceEntity;
		pSurfEnt->SetFace(aFace);
		pNewEntity = pSurfEnt;
	}
	else
	{
		kcWireEntity *pEntity = (kcWireEntity *)se._pEntity; 
		TopoDS_Wire aWire = pEntity->GetWire();
		TopoDS_Shape aShape;
		try{
			BRepPrimAPI_MakePrism mp(aWire,gp_Vec(vec[0],vec[1],vec[2]),Standard_True);
			if(mp.IsDone())
			{
				aShape = mp.Shape();
				if(aShape.IsNull())
					return FALSE;
			}
		}catch(Standard_Failure){
			return FALSE;
		}
		if(aShape.ShapeType() == TopAbs_SHELL)
		{
			TopoDS_Shell aShell = TopoDS::Shell(aShape);
			kcShellEntity *pShellEnt = new kcShellEntity;
			pShellEnt->SetShell(aShell);
			pNewEntity = pShellEnt;
		}
		else if(aShape.ShapeType() == TopAbs_SOLID)
		{
			TopoDS_Solid aSolid = TopoDS::Solid(aShape);
			kcSolidEntity *pSolidEnt = new kcSolidEntity;
			pSolidEnt->SetSolid(aSolid);
			pNewEntity = pSolidEnt;
		}
	}
	if(pNewEntity)
	{
		kcModel *pModel = GetModel();
		pModel->BeginUndo(GetName());
		pModel->AddEntity(pNewEntity);
		pModel->EndUndo();
	}

	return TRUE;
}

// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
BOOL	kcmPrismSurface::CreateInputTools()
{
	m_pInputEntity = new kiInputEntityTool(this,"ѡ��һ������:");
	m_pInputEntity->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE,1);
	m_pInputEntity->SetNaturalMode(true);

	m_pInputVec = new kiInputVector(this,"�������췽��:");
	m_pInputVec->SetType(eInputZVector);

	return TRUE;
}

// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
BOOL	kcmPrismSurface::InitInputToolQueue()
{
	AddInputTool(m_pInputEntity);
	AddInputTool(m_pInputVec);

	return TRUE;
}
