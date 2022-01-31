#include "StdAfx.h"
#include "kiInputEntityTool.h"
#include "kiInputVector.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "kvCoordSystem.h"
#include "kcEntityFactory.h"
#include "QxBRepLib.h"
#include "kcmSolidDeCompound.h"

kcmSolidDeCompound::kcmSolidDeCompound(void)
{
	m_strName = "�ֽ�";
	m_strAlias = "DeCompound";
	m_strDesc = "�ֽ�";

	m_pInputEntity = NULL;
}

kcmSolidDeCompound::~kcmSolidDeCompound(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmSolidDeCompound::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int		kcmSolidDeCompound::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntity)
	{
		if(m_pInputEntity->IsInputDone())
		{

			if(DeCompound())
			{
				Done();
		
				m_pInputEntity->ClearSel();
				Redraw();
				return KSTA_DONE;
			}
			else
			{
				m_pInputEntity->ClearSel();
				return KSTA_ERROR;
			}
		}
		else
		{
			return KSTA_CANCEL;
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

// ������Ҫ�����빤��
BOOL	kcmSolidDeCompound::CreateInputTools()
{
	m_pInputEntity = new kiInputEntityTool(this,"ѡ��һ����϶���");
	m_pInputEntity->SetOption(KCT_ENT_SOLID | KCT_ENT_SHELL | KCT_ENT_COMPOUND,1);
	m_pInputEntity->SetNaturalMode(true);

	return TRUE;
}

BOOL	kcmSolidDeCompound::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntity);

	return TRUE;
}

BOOL	kcmSolidDeCompound::InitInputToolQueue()
{
	AddInputTool(m_pInputEntity);
	return TRUE;
}


BOOL	kcmSolidDeCompound::DeCompound()
{
	kiSelSet *pSelSet = m_pInputEntity->GetSelSet();
	if(pSelSet->GetSelCount() != 1)
		return FALSE;

	pSelSet->InitSelected();
	kiSelEntity se = pSelSet->CurSelected();
	TopoDS_Shape aShape = se.SelectShape();
	if(aShape.IsNull())
		return FALSE;
	
	TopoDS_Iterator dsit;
	kcEntityFactory entfac;
	kcEntity *pEnt = NULL;
	std::vector<kcEntity *> aEnt;

	if(aShape.ShapeType() == TopAbs_COMPOUND){
		dsit.Initialize(aShape);
		for(;dsit.More();dsit.Next())
		{
			TopoDS_Shape aS = dsit.Value();
			//TopoDS_Shape aCS = QxBRepLib::CopyShape(aS);
			//
			pEnt = entfac.Create(aS);
			if(pEnt != NULL){
				aEnt.push_back(pEnt);
			}
		}
	}else if(aShape.ShapeType() == TopAbs_SHELL || aShape.ShapeType() == TopAbs_SOLID){
		TopExp_Explorer aExp;
		for(aExp.Init(aShape,TopAbs_FACE);aExp.More();aExp.Next()){
			const TopoDS_Shape &aF = aExp.Current();
			//
			kcEntity *pEnt = entfac.Create(aF);
			if(pEnt != NULL){
				aEnt.push_back(pEnt);
			}
		}
	}

	if(!aEnt.empty())
	{
		kcModel *pModel = GetModel();
		pModel->BeginUndo(GetName());
		{
			pModel->DelEntity(se._pEntity);
			int ix,ic = (int)aEnt.size();
			for(ix = 0;ix < ic;ix ++)
				pModel->AddEntity(aEnt[ix]);
		}
		pModel->EndUndo();
	}

	return TRUE;
}