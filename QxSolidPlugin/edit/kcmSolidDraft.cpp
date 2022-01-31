#include "StdAfx.h"
#include <BRepOffsetAPI_MakeDraft.hxx>
#include "kiInputEntityTool.h"
#include "kiInputVector.h"
#include "kiInputValueTool.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "QxBRepLib.h"
#include "QxCurveLib.h"
#include "kvCoordSystem.h"
#include "kcEntityFactory.h"
#include "kcmSolidDraft.h"

kcmSolidDraft::kcmSolidDraft(void)
{
	m_strName = "��ģ����";
	m_strAlias = "SolidDraft";
	m_strDesc = "ʵ���ģ����";

	m_pPickEntityTool = NULL;
	m_pInputVectorTool = NULL;
	m_pInputDoubleTool = NULL;
	m_pPickStopEntityTool = NULL;
	m_dAngle = K_PI / 6;

	m_pDraftMode = NULL;
	m_nDraftMode = 0;
	m_pTransitionMode = NULL;
	m_nTransitionMode = 0;
	m_bInternal = false;

	m_bReady = false;
}

kcmSolidDraft::~kcmSolidDraft(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmSolidDraft::CanFinish()//�����Ƿ�������
{
	if(m_bReady)
		return TRUE;
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int		kcmSolidDraft::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pPickEntityTool)
	{
		if(m_pPickEntityTool->IsInputDone())
		{
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputVectorTool)
	{
		if(m_pInputVectorTool->IsInputDone())
		{
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputDoubleTool)
	{
		if(pTool->IsInputDone())
		{
			if(m_nDraftMode == 0)
			{
				m_bReady = true;
				PromptMessage("����Ҽ�Ӧ�û�ȡ��:");
			}
			else
			{
				if(m_nDraftMode == 1)
					m_pPickStopEntityTool->SetOption(KCT_ENT_FACE,1);
				else
					m_pPickStopEntityTool->SetOption(KCT_ENT_SHELL | KCT_ENT_SOLID,1);

				AddInputTool(m_pPickStopEntityTool);
				NavToNextTool();
			}
		}
	}
	else if(pTool == m_pPickStopEntityTool)
	{
		if(pTool->IsInputDone())
		{
			m_bReady = true;
		}
	}

	return KSTA_CONTINUE;
}

int		kcmSolidDraft::OnExecute()
{
	m_bReady = false;

	return KSTA_CONTINUE;
}

int		kcmSolidDraft::OnEnd(int nCode)
{
	return nCode;
}

int		kcmSolidDraft::OnApply()
{
	int ista = KSTA_CONTINUE;
	if(DoDraft())
	{
		Done();
		ista = KSTA_DONE;
	}
	else
	{
		EndCommand(KSTA_FAILED);
		ista = KSTA_FAILED;
	}

	return ista;
}

BOOL	kcmSolidDraft::DoDraft()
{
	if(m_pPickEntityTool->GetSelCount() != 1)
		return FALSE;
	
	kVector3 vdir;
	if(!m_pInputVectorTool->GetVector(vdir) || vdir.length() < 0.001)
		return FALSE;

	kiSelEntity se = m_pPickEntityTool->GetFirstSelect();
	TopoDS_Shape aShape = se._pEntity->GetShape();
	
	TopoDS_Shape aNS;
	try{
		BRepOffsetAPI_MakeDraft mdraft(aShape,gp_Dir(vdir[0],vdir[1],vdir[2]),m_dAngle);
		if(m_nTransitionMode == 0)
			mdraft.SetOptions(BRepBuilderAPI_RightCorner);
		else
			mdraft.SetOptions(BRepBuilderAPI_RoundCorner);
		if(m_bInternal)
			mdraft.SetDraft(Standard_True);
		else
			mdraft.SetDraft(Standard_False);

		if(m_nDraftMode == 0)
		{
			mdraft.Perform(vdir.length());
		}
		if(mdraft.IsDone())
			aNS = mdraft.Shape();
	}catch(Standard_Failure){}
	
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

// ������Ҫ�����빤��
BOOL	kcmSolidDraft::CreateInputTools()
{
	//����ѡ��
	m_pDraftMode = new kiOptionEnum("���췽ʽ",'M',m_nDraftMode);
	m_pDraftMode->AddEnumItem("��������",'L');
	m_pDraftMode->AddEnumItem("���쵽��",'F');
	m_pDraftMode->AddEnumItem("���쵽��",'S');

	m_pTransitionMode = new kiOptionEnum("ת��ģʽ",'T',m_nTransitionMode);
	m_pTransitionMode->AddEnumItem("RightCorner",'R');
	m_pTransitionMode->AddEnumItem("RoundCorner",'O');

	m_optionSet.AddBoolOption("����",'I',m_bInternal);

	m_optionSet.AddOption(m_pDraftMode);
	m_optionSet.AddOption(m_pTransitionMode);

	m_pPickEntityTool = new kiInputEntityTool(this,"ѡ���ߡ�����",&m_optionSet);
	m_pPickEntityTool->SetOption(KCT_ENT_WIRE | KCT_ENT_FACE | KCT_ENT_SHELL,1);

	m_pInputVectorTool = new kiInputVector(this,"�������췽��",&m_optionSet);
	m_pInputVectorTool->SetType(eInputZVector);

	m_pInputDoubleTool = new kiInputDoubleTool(this,"����Ƕ�");
	m_pInputDoubleTool->Init(&m_dAngle);

	m_pPickStopEntityTool = new kiInputEntityTool(this,"ʰȡ��ֹ����");

	return TRUE;
}

BOOL	kcmSolidDraft::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pPickEntityTool);
	KC_SAFE_DELETE(m_pInputVectorTool);
	KC_SAFE_DELETE(m_pInputDoubleTool);
	KC_SAFE_DELETE(m_pPickStopEntityTool);

	return TRUE;
}

BOOL	kcmSolidDraft::InitInputToolQueue()
{
	AddInputTool(m_pPickEntityTool);
	AddInputTool(m_pInputVectorTool);
	AddInputTool(m_pInputDoubleTool);
	

	return TRUE;
}


