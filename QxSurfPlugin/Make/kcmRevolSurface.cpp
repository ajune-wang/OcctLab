#include "StdAfx.h"
#include "kiInputEntityTool.h"
#include "kiInputVector.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "kvCoordSystem.h"
#include "kcmRevolSurface.h"

#define KC_STA_INPUT_ENTITY		1
#define KC_STA_INPUT_VEC		2

kcmRevolSurface::kcmRevolSurface(void)
{
	m_strName = "Revolve Surface";
	m_strAlias = "RevolSurf";
	m_strDesc = "��ת��";

	m_nState = KC_STA_INPUT_ENTITY;
	m_pInputEntity = NULL;
	m_pInputVec = NULL;
	m_dAngle = 180.0;
}

kcmRevolSurface::~kcmRevolSurface(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmRevolSurface::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmRevolSurface::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntity)
	{
		if(m_pInputEntity->IsInputDone())
		{
			m_nState = KC_STA_INPUT_VEC;

			NavToNextTool();
		}
	}
	else if(pTool == m_pInputVec)
	{
		if(m_pInputVec->IsInputDone())
		{
			//������ת��
			if(MakeRevolSurface())
			{
				m_pInputEntity->ClearSel();

				Done();
				Redraw();

				return KSTA_DONE;
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

//
BOOL	kcmRevolSurface::MakeRevolSurface()
{
	kiSelSet *pSelSet = m_pInputEntity->GetSelSet();
	if(pSelSet->GetSelCount() != 1)
		return FALSE;

	pSelSet->InitSelected();
	kiSelEntity se = pSelSet->CurSelected();
	if(!IS_EDGE_ENTITY(se._pEntity) && !IS_WIRE_ENTITY(se._pEntity))
		return FALSE;

	//kvCoordSystem cs;
	//kvWorkPlane *pWorkPlane = glb_GetCurrentWorkPlane();
	//pWorkPlane->CurrentWorkPlaneCS(cs);
	kPoint3 org; 
	kVector3 vec;
	m_pInputVec->GetPoint(org);
	m_pInputVec->GetVector(vec);
	vec.normalize();
	//kVector3 z = cs.Z();
	//kVector3 x = z ^ vec;
	//x.normalize();

	gp_Ax1 ax1(gp_Pnt(org[0],org[1],org[2]),gp_Dir(vec[0],vec[1],vec[2]));
	double rad = m_dAngle * K_PI / 180.0;
	
	kcEntity *pNewEntity = NULL;
	if(IS_EDGE_ENTITY(se._pEntity))
	{
		kcEdgeEntity *pCurEnt = (kcEdgeEntity *)se._pEntity;
		TopoDS_Edge aEdge = pCurEnt->GetEdge();

		TopoDS_Face aFace;
		try
		{
			BRepPrimAPI_MakeRevol mr(aEdge,ax1,rad,Standard_True);
			if(mr.IsDone())
			{
				TopoDS_Shape aS = mr.Shape();
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
		kcWireEntity *pWireEnt = (kcWireEntity *)se._pEntity;
		TopoDS_Wire aWire = pWireEnt->GetWire();

		TopoDS_Shape aShape;
		try{
			BRepPrimAPI_MakeRevol mr(aWire,ax1,rad,Standard_True);
			if(mr.IsDone())
			{
				aShape = mr.Shape();
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
BOOL	kcmRevolSurface::CreateInputTools()
{
	kiOptionItem *pItem = new kiOptionDouble("��ת�Ƕ�",'A',m_dAngle);
	pItem->EnablePopup(true);
	m_optionSet.AddOption(pItem);

	m_pInputEntity = new kiInputEntityTool(this,"ѡ��һ������");
	m_pInputEntity->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE,1);

	m_pInputVec = new kiInputVector(this,"ѡ����ת��",&m_optionSet);
	m_pInputVec->SetType(eInputWorkPlaneVector);

	return TRUE;
}

BOOL	kcmRevolSurface::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntity);
	KC_SAFE_DELETE(m_pInputVec);

	return TRUE;
}

// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
BOOL	kcmRevolSurface::InitInputToolQueue()
{
	AddInputTool(m_pInputEntity);
	AddInputTool(m_pInputVec);

	return TRUE;
}
