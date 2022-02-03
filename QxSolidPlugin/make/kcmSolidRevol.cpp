#include "StdAfx.h"
#include "kiInputEntityTool.h"
#include "kiInputVector.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "kvCoordSystem.h"
#include "kcmSolidRevol.h"

#define KC_STA_INPUT_ENTITY		1
#define KC_STA_INPUT_VEC		2

kcmSolidRevol::kcmSolidRevol(void)
{
	m_strName = "Revol Solid";
	m_strAlias = "RevolSolid";
	m_strDesc = "Revol face to solid";

	m_nState = KC_STA_INPUT_ENTITY;
	m_pInputEntity = NULL;
	m_pInputVec = NULL;
	m_dAngle = 180.0;
}

kcmSolidRevol::~kcmSolidRevol(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmSolidRevol::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmSolidRevol::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntity){
		if(m_pInputEntity->IsInputDone()){
			m_nState = KC_STA_INPUT_VEC;

			NavToNextTool();
		}
	}else if(pTool == m_pInputVec){
		if(m_pInputVec->IsInputDone()){
			//������ת��
			if(MakeRevolShape()){
				m_pInputEntity->ClearSel();

				Done();
				Redraw();

				return KSTA_DONE;
			}
		}
	}
	return KSTA_CONTINUE;
}


int	kcmSolidRevol::OnExecute()
{
	return KSTA_CONTINUE;
}

int	kcmSolidRevol::OnEnd(int nCode)
{
	return nCode;
}

//
BOOL	kcmSolidRevol::MakeRevolShape()
{
	kiSelSet *pSelSet = m_pInputEntity->GetSelSet();
	if(pSelSet->GetSelCount() != 1)
		return FALSE;

	kiSelEntity se = pSelSet->GetFirstSelected();
	TopoDS_Shape aS = se.SelectShape();//֧���ڲ����ѡȡ
	if (aS.IsNull() || aS.ShapeType() != TopAbs_FACE)
		return FALSE;
	TopoDS_Face aFace = TopoDS::Face(se.SelectShape());

	kPoint3 org; 
	kVector3 vec;
	m_pInputVec->GetPoint(org);
	m_pInputVec->GetVector(vec);
	vec.normalize();

	gp_Ax1 ax1(gp_Pnt(org[0],org[1],org[2]),gp_Dir(vec[0],vec[1],vec[2]));
	double rad = m_dAngle * K_PI / 180.0;

	kcEntity *pNewEntity = NULL;
	TopoDS_Shape aShape;
	try{
		BRepPrimAPI_MakeRevol mr(aFace,ax1,rad,Standard_True);
		if(mr.IsDone()){
			aShape = mr.Shape();
			if(aShape.IsNull())
				return FALSE;
		}
	}catch(Standard_Failure){
		return FALSE;
	}

	if(aShape.ShapeType() == TopAbs_SOLID){
		TopoDS_Solid aSolid = TopoDS::Solid(aShape);
		kcSolidEntity *pSolidEnt = new kcSolidEntity;
		pSolidEnt->SetSolid(aSolid);
		pNewEntity = pSolidEnt;
	}

	if(pNewEntity){
		kcModel *pModel = GetModel();
		pModel->BeginUndo(GetName());
		pModel->AddEntity(pNewEntity);
		pModel->EndUndo();

		Redraw();
	}

	return TRUE;
}

// ������Ҫ�����빤��
BOOL	kcmSolidRevol::CreateInputTools()
{
	kiOptionItem *pItem = new kiOptionDouble("��ת�Ƕ�",'A',m_dAngle);
	m_optionSet.AddOption(pItem);

	m_optionSet.AddQuitButtonOption();

	m_pInputEntity = new kiInputEntityTool(this,"select a FACE",&m_optionSet);
	m_pInputEntity->SetOption(KCT_ENT_FACE,1);
	m_pInputEntity->SetNaturalMode(false);

	m_pInputVec = new kiInputVector(this,"ѡ����ת��");
	m_pInputVec->SetType(eInputWorkPlaneVector);

	return TRUE;
}

BOOL	kcmSolidRevol::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntity);
	KC_SAFE_DELETE(m_pInputVec);

	return TRUE;
}

BOOL	kcmSolidRevol::InitInputToolQueue()
{
	AddInputTool(m_pInputEntity);
	AddInputTool(m_pInputVec);

	return TRUE;
}

