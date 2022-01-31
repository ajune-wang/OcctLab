#include "StdAfx.h"
#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include "kiInputEntityTool.h"
#include "kiInputValueTool.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "kvCoordSystem.h"
#include "kcEntityFactory.h"
#include "QxBRepLib.h"
#include "kcmSolidOffset.h"

kcmSolidOffset::kcmSolidOffset(void)
{
	m_strName = "ʵ��ƫ��";
	m_strAlias = "SolidOffset";
	m_strDesc = "�����ʵ��ƫ��";

	m_bOutSide = true;
	m_pInputEntity = NULL;
	m_pInputOffset = NULL;
	m_dOffset = 5.0;
}

kcmSolidOffset::~kcmSolidOffset(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmSolidOffset::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmSolidOffset::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntity)
	{
		if(m_pInputEntity->IsInputDone())
		{
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputOffset)
	{
		if(m_pInputOffset->IsInputDone())
		{
			if(BuildOffsetShape())
			{
				m_pInputEntity->ClearSel();
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

int		kcmSolidOffset::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmSolidOffset::OnEnd(int nCode)
{
	//m_pInputEntity->ClearSel();
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmSolidOffset::CreateInputTools()
{
	m_optionSet.AddBoolOption("����ƫ��",'D',m_bOutSide);

	m_pInputEntity = new kiInputEntityTool(this,"ѡ����/��/ʵ��",&m_optionSet);
	m_pInputEntity->SetOption(KCT_ENT_FACE | KCT_ENT_SHELL | KCT_ENT_SOLID,1);

	m_pInputOffset = new kiInputDoubleTool(this,"����ƫ�ƾ���");
	m_pInputOffset->Init(&m_dOffset,0,10000);

	return TRUE;
}

BOOL	kcmSolidOffset::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntity);
	KC_SAFE_DELETE(m_pInputOffset);

	return TRUE;
}

BOOL	kcmSolidOffset::InitInputToolQueue()
{
	AddInputTool(m_pInputEntity);
	AddInputTool(m_pInputOffset);

	return TRUE;
}


BOOL	kcmSolidOffset::BuildOffsetShape()
{
	if(m_pInputEntity->GetSelCount() <= 0)
		return FALSE;
	kiSelEntity se = m_pInputEntity->GetFirstSelect();
	TopoDS_Shape aShape = se._pEntity->GetShape();
	double doff = m_dOffset;
	if(!m_bOutSide)
		doff = -doff;

	TopoDS_Shape aNS;
	try{
		BRepOffsetAPI_MakeOffsetShape mos;//(aShape,doff,0.001);
		//mos.Build();
		mos.PerformByJoin(aShape,doff,0.001,BRepOffset_Skin,true,false,GeomAbs_Intersection);
		if(mos.IsDone())
			aNS = mos.Shape();
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
		pModel->EndUndo();

		return TRUE;
	}


	return FALSE;
}

