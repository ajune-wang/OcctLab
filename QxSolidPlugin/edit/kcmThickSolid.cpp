#include "StdAfx.h"
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include "kiInputEntityTool.h"
#include "kiInputValueTool.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "kvCoordSystem.h"
#include "kcEntityFactory.h"
#include "QxBRepLib.h"
#include "kcmThickSolid.h"

//////////////////////////////////////////////////////////////////
//
kcmSheetSolid::kcmSheetSolid(void)
{
	m_strName = "���";
	m_strAlias = "SheetSolid";
	m_strDesc = "���";

	m_bOutSide = false;
	m_pInputEntity = NULL;
	m_pInputFace = NULL;
	m_pInputThickness = NULL;
	m_dThick = 2.0;
}

kcmSheetSolid::~kcmSheetSolid(void)
{
}


// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmSheetSolid::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmSheetSolid::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntity)
	{
		if(m_pInputEntity->IsInputDone())
		{
			//kiSelEntity se = m_pInputEntity->GetFirstSelect();
			//m_pInputFace->Set(se.AISObject(),eInputLocalFace);

			NavToNextTool();
		}
	}
	else if(pTool == m_pInputFace)
	{
		if(m_pInputFace->IsInputDone())
		{
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputThickness)
	{
		if(m_pInputThickness->IsInputDone())
		{
			if(BuildSheetSolid())
			{
				Done();
				Redraw();

				return KSTA_DONE;
			}
			else
			{
				EndCommand(KSTA_FAILED);
				return KSTA_FAILED;
			}
		}
	}
	return KSTA_CONTINUE;
}

int		kcmSheetSolid::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmSheetSolid::OnEnd(int nCode)
{
	m_pInputEntity->ClearSel();

	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmSheetSolid::CreateInputTools()
{
	m_optionSet.AddBoolOption("����ƫ��",'O',m_bOutSide);

	m_pInputEntity = new kiInputEntityTool(this,"ѡ��ʵ��",&m_optionSet);
	m_pInputEntity->SetOption(KCT_ENT_SOLID,1);
	m_pInputEntity->SetNaturalMode(true);

	m_pInputFace = new kiInputEntityTool(this,"ѡ��Ҫȥ������");
	m_pInputFace->SetOption(KCT_ENT_FACE,-1);
	m_pInputFace->SetNaturalMode(false);

	m_pInputThickness = new kiInputDoubleTool(this,"������");
	m_pInputThickness->Init(&m_dThick,1e-6,1000);

	return TRUE;
}

BOOL	kcmSheetSolid::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntity);
	KC_SAFE_DELETE(m_pInputFace);
	KC_SAFE_DELETE(m_pInputThickness);

	return TRUE;
}

BOOL	kcmSheetSolid::InitInputToolQueue()
{
	AddInputTool(m_pInputEntity);
	AddInputTool(m_pInputFace);
	AddInputTool(m_pInputThickness);

	return TRUE;
}

BOOL	kcmSheetSolid::BuildSheetSolid()
{
	if(m_pInputEntity->GetSelCount() <= 0 ||
		m_pInputFace->GetSelCount() <= 0)
		return FALSE;

	kiSelEntity se = m_pInputEntity->GetFirstSelect();
	TopoDS_Shape aShape = se._pEntity->GetShape();
	kcEntity *pSelEnt = se._pEntity;
	double dThick = m_dThick;
	if(!m_bOutSide)
		dThick = -dThick;

	TopoDS_Shape aFace;
	TopTools_ListOfShape aFaceList;
	kiSelSet *pSelSet = m_pInputFace->GetSelSet();
	for(pSelSet->InitSelected();pSelSet->MoreSelected();pSelSet->NextSelected()){
		se = pSelSet->CurSelected();
		aFace = se.SelectShape();
		if(!aFace.IsNull()){
			aFaceList.Append(aFace);
		}
	}
	

	TopoDS_Shape aNS;
	try{
		BRepOffsetAPI_MakeThickSolid  aMakeThickSolid;
		aMakeThickSolid.MakeThickSolidByJoin(aShape,aFaceList,dThick,0.0001);
		//msolid.Build();
		if(aMakeThickSolid.IsDone())
			aNS = aMakeThickSolid.Shape();
		if(aNS.IsNull())
			return FALSE;
	}catch(Standard_Failure){
		return FALSE;
	}

	kcEntityFactory efac;
	kcEntity *pEntity = efac.Create(aNS);
	if(pEntity)
	{
		kcModel *pModel = GetModel();
		pModel->BeginUndo(GetName());
		pModel->AddEntity(pEntity);
		pModel->DelEntity(pSelEnt);
		pModel->EndUndo();

		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////
//
kcmThickSolid::kcmThickSolid(void)
{
	m_strName = "�Ⱥ�";
	m_strAlias = "ThickSolid";
	m_strDesc = "�Ⱥ�����ʵ��";

	m_pInputEntity = NULL;
	m_pInputThickness = NULL;
	m_bRevDir = false;
	m_dThick = 2.0;
}

kcmThickSolid::~kcmThickSolid(void)
{
}


// ��������������,��Ҫ��Apply�������ʹ��.
BOOL  kcmThickSolid::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int  kcmThickSolid::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntity){
		if(m_pInputEntity->IsInputDone()){
			kiSelEntity se = m_pInputEntity->GetFirstSelect();
			NavToNextTool();
		}
	}else if(pTool == m_pInputThickness){
		if(m_pInputThickness->IsInputDone()){
			if(BuildThickSolid()){
				Done();
				Redraw();

				return KSTA_DONE;
			}else{
				EndCommand(KSTA_FAILED);
				return KSTA_FAILED;
			}
		}
	}
	return KSTA_CONTINUE;
}

int  kcmThickSolid::OnExecute()
{
	return KSTA_CONTINUE;
}

int  kcmThickSolid::OnEnd(int nCode)
{
	m_pInputEntity->ClearSel();

	return nCode;
}

// ������Ҫ�����빤��
BOOL  kcmThickSolid::CreateInputTools()
{
	m_optionSet.AddBoolOption("����",'R',m_bRevDir);

	m_pInputEntity = new kiInputEntityTool(this,"ѡ����/��",&m_optionSet);
	m_pInputEntity->SetOption(KCT_ENT_FACE | KCT_ENT_SHELL,1);

	m_pInputThickness = new kiInputDoubleTool(this,"������");
	m_pInputThickness->Init(&m_dThick,1e-6,1000);

	return TRUE;
}

BOOL	kcmThickSolid::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntity);
	KC_SAFE_DELETE(m_pInputThickness);

	return TRUE;
}

BOOL	kcmThickSolid::InitInputToolQueue()
{
	AddInputTool(m_pInputEntity);
	AddInputTool(m_pInputThickness);

	return TRUE;
}

BOOL	kcmThickSolid::BuildThickSolid()
{
	if(m_pInputEntity->GetSelCount() <= 0)
		return FALSE;

	kiSelEntity se = m_pInputEntity->GetFirstSelect();
	TopoDS_Shape aShape = se._pEntity->GetShape();
	kcEntity *pSelEnt = se._pEntity;
	double dThick = m_dThick;
	if(m_bRevDir)
		dThick = -dThick;

	TopoDS_Shape aNS;
	try{
		BRepOffsetAPI_MakeThickSolid  aMakeThickSolid;
		aMakeThickSolid.MakeThickSolidBySimple(aShape,dThick);

		if(aMakeThickSolid.IsDone())
			aNS = aMakeThickSolid.Shape();
		if(aNS.IsNull())
			return FALSE;
	}catch(Standard_Failure){
		return FALSE;
	}

	kcEntityFactory efac;
	kcEntity *pEntity = efac.Create(aNS);
	if(pEntity)
	{
		kcModel *pModel = GetModel();
		pModel->BeginUndo(GetName());
		pModel->AddEntity(pEntity);
		pModel->DelEntity(pSelEnt);
		pModel->EndUndo();

		return TRUE;
	}

	return FALSE;
}
