#include "StdAfx.h"
#include "kcModel.h"
#include "kiInputPoint.h"
#include "kiInputVector.h"
#include "kiInputEntityTool.h"
#include "kiInputAngle.h"
#include "kcEntity.h"
#include "kiSelSet.h"
#include "kvCoordSystem.h"
#include "kcBasePlane.h"
#include "kcEntityTrsf.h"
#include "kcDispObj.h"
#include "QxBaseUtils.h"
#include "kcPreviewObj.h"
#include "kcmTrsfRotate.h"

#define KC_STA_PICK_ENTITY		1
#define KC_STA_INPUT_BASEPNT	2
#define KC_STA_INPUT_REFPNT		3
#define KC_STA_INPUT_ANGLE		4

kcmTrsfRotate2d::kcmTrsfRotate2d(void)
{
	m_strName = "2d��ת";
	m_strAlias = "Rot2d";
	m_strDesc = "2d��ת";

	m_pSelEntity = NULL;
	m_pInputBasePnt = NULL;
	m_pInputRefPnt = NULL;
	m_pInputAngle = NULL;
	m_bCopy = false;
	m_dAngle = K_PI / 2;

	m_pFrmShape = NULL;
	m_pPreviewLine = NULL;

	m_pLocalCS = new kvCoordSystem;

}
kcmTrsfRotate2d::~kcmTrsfRotate2d(void)
{

}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmTrsfRotate2d::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmTrsfRotate2d::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pSelEntity)
	{
		if(m_pSelEntity->IsInputDone())
		{
			kiSelSet *pSelSet = m_pSelEntity->GetSelSet();
			kiSelEntity selEnt;
			pSelSet->InitSelected();
			while(pSelSet->MoreSelected())
			{
				selEnt = pSelSet->CurSelected();
				TopoDS_Shape aShape = selEnt._pEntity->GetShape();
				if(!aShape.IsNull())
				{
					m_pFrmShape->AddShape(aShape);
				}

				pSelSet->NextSelected();
			}

			m_nState = KC_STA_INPUT_BASEPNT;
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputBasePnt)
	{
		if(m_pInputBasePnt->IsInputDone())
		{
			m_nState = KC_STA_INPUT_REFPNT;
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputRefPnt)
	{
		if(m_pInputRefPnt->IsInputDone())
		{
			kcBasePlane *pWorkPlane = GetCurrentBasePlane();
			pWorkPlane->GetCoordSystem(*m_pLocalCS);
			kVector3 xv = m_refPoint - m_basePoint,zv = m_pLocalCS->Z();
			xv.normalize();
			m_pLocalCS->Set(m_basePoint,xv,zv);
			m_pInputAngle->Init(m_pLocalCS,&m_dAngle,true);

			m_nState = KC_STA_INPUT_ANGLE;
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputAngle)
	{
		if(m_pInputAngle->IsInputDone())
		{
			DoRotate();

			Done();
			Redraw();

			return KSTA_DONE;
		}
	}
	return KSTA_CONTINUE;
}

BOOL	kcmTrsfRotate2d::DoRotate()
{
	if(fabs(m_dAngle) < KDBL_MIN)
		return FALSE;

	kVector3 zdir = GetWorkPlaneZDir();
	kcEntityTrsf entrsf;
	std::vector<kcEntity *> aEnt,aNewEnt;
	kcEntity *pEntity = NULL;
	kiSelSet *pSelSet = m_pSelEntity->GetSelSet();
	kiSelEntity selEnt;
	pSelSet->InitSelected();
	while(pSelSet->MoreSelected())
	{
		selEnt = pSelSet->CurSelected();
		//
		if(!m_bCopy)
			aEnt.push_back(selEnt._pEntity);

		//
		pEntity = entrsf.Rotate(selEnt._pEntity,m_basePoint,zdir,m_dAngle);
		if(pEntity)
			aNewEnt.push_back(pEntity);

		pSelSet->NextSelected();
	}

	if(aNewEnt.empty())
		return FALSE;

	kcModel *pModel = GetModel();
	pModel->BeginUndo(GetName());
	{
		int ix,isize;
		if(!m_bCopy)
		{
			isize = (int)aEnt.size();
			for(ix = 0;ix < isize;ix ++)
				pModel->DelEntity(aEnt[ix]);
		}
		pModel->AddEntity(aNewEnt);
	}
	pModel->EndUndo();

	return TRUE;
}

int		kcmTrsfRotate2d::OnExecute()
{
	m_nState = KC_STA_PICK_ENTITY;

	m_pFrmShape = new kcDispFrmShape(GetModel()->GetAISContext());

	m_pPreviewLine = new kcPreviewLine(GetModel()->GetAISContext(),GetDocContext());
	m_pPreviewLine->SetColor(1,1,1);

	return KSTA_CONTINUE;
}

int		kcmTrsfRotate2d::OnEnd(int nCode)
{
	KC_SAFE_DELETE(m_pFrmShape);
	KC_SAFE_DELETE(m_pPreviewLine);

	return nCode;
}

int		kcmTrsfRotate2d::OnMouseMove(kuiMouseInput& mouseInput)
{
	kiCommand::OnMouseMove(mouseInput);

	if(m_nState == KC_STA_INPUT_REFPNT)
	{
		if(m_pPreviewLine)
		{
			m_pPreviewLine->Update(KCG_GP_PNT(m_basePoint),KCG_GP_PNT(m_refPoint));
			m_pPreviewLine->Display(TRUE);
		}
	}
	else if(m_nState == KC_STA_INPUT_ANGLE)
	{
		if(m_pFrmShape)
		{
			kVector3 zdir = GetWorkPlaneZDir();
			m_pFrmShape->Rotate(m_basePoint,zdir,m_dAngle);
			m_pFrmShape->Show();
		}
	}

	return KSTA_CONTINUE;
}

// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
BOOL	kcmTrsfRotate2d::CreateInputTools()
{
	m_optionSet.AddBoolOption("��������",'C',m_bCopy);
	m_pSelEntity = new kiInputEntityTool(this,"ʰȡ����",&m_optionSet);
	m_pSelEntity->SetOption(KCT_ENT_ALL);

	m_pInputBasePnt = new kiInputPoint(this,"ѡ�����");
	m_pInputBasePnt->Init(&m_basePoint,false);

	m_pInputRefPnt = new kiInputPoint(this,"ѡ���һ�ο���");
	m_pInputRefPnt->Init(&m_refPoint,false);
	m_pInputRefPnt->SetRealtimeUpdate(true);

	m_pInputAngle = new kiInputAngle(this,"����Ƕ�");

	return TRUE;
}

// ���ٴ��������빤��.ÿ�����������һ��.
BOOL	kcmTrsfRotate2d::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pSelEntity);
	KC_SAFE_DELETE(m_pInputBasePnt);
	KC_SAFE_DELETE(m_pInputRefPnt);
	KC_SAFE_DELETE(m_pInputAngle);

	return TRUE;
}

// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
BOOL	kcmTrsfRotate2d::InitInputToolQueue()
{
	AddInputTool(m_pSelEntity);
	AddInputTool(m_pInputBasePnt);
	AddInputTool(m_pInputRefPnt);
	AddInputTool(m_pInputAngle);

	return TRUE;
}
