#include "StdAfx.h"
#include <ShapeFix_Shape.hxx>
#include "kcEntity.h"
#include "kiInputEntityTool.h"
#include "kcModel.h"
#include "kcEntityFactory.h"
#include "kcmShapeFix.h"

kcmShapeFixBase::kcmShapeFixBase(void)
{
	m_pInputEntityTool = NULL;
	m_nEntType = 0;
}

kcmShapeFixBase::~kcmShapeFixBase(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmShapeFixBase::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmShapeFixBase::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntityTool)
	{
		if(DoFix()){
			Done();
			return KSTA_DONE;
		}else{
			EndCommand(KSTA_CANCEL);
			return KSTA_CANCEL;
		}
	}
	return KSTA_CONTINUE;
}

int		kcmShapeFixBase::OnExecute()
{
	if(!IsGlobalSelSetEmpty()){
		if(DoFix())
		{
			ClearGlobalSelSet();

			Done();
			Redraw();

			return KSTA_DONE;
		}
	}
	return KSTA_CONTINUE;
}

int		kcmShapeFixBase::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmShapeFixBase::CreateInputTools()
{
	m_pInputEntityTool = new kiInputEntityTool(this,"ѡ��Ҫ�����Ķ���");
	m_pInputEntityTool->SetOption(m_nEntType,1);
	m_pInputEntityTool->SetNaturalMode(true);

	return TRUE;
}

BOOL	kcmShapeFixBase::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntityTool);
	return TRUE;
}
BOOL	kcmShapeFixBase::InitInputToolQueue()
{
	AddInputTool(m_pInputEntityTool);
	return TRUE;
}

BOOL	kcmShapeFixBase::DoFix()
{
	TopoDS_Shape aShape;
	kiSelEntity se;

	if(m_pInputEntityTool->IsInputDone()){
		if(m_pInputEntityTool->GetSelCount() != 1)
			return FALSE;
		se = m_pInputEntityTool->GetFirstSelect();
		aShape = se.EntityShape();
	}else{
		//�ж��Ƿ�ȫ��ѡ���ж���
		kiSelSet *pSelSet = GetGlobalSelSet();
		if(pSelSet->GetSelCount() > 0){
			pSelSet->InitSelected();
			se = pSelSet->CurSelected();
			aShape = se.EntityShape();
		}
	}
	if(aShape.IsNull())
		return FALSE;

	TopoDS_Shape aNS = DoFixShape(aShape);
	if(aNS.IsNull())
		return FALSE;

	kcEntityFactory entfac;
	kcEntity *pEnt = entfac.Create(aNS);
	if(pEnt)
	{
		kcModel *pModel = GetModel();
		pModel->BeginUndo(GetName());
		pModel->DelEntity(se.Entity());
		pModel->AddEntity(pEnt);
		pModel->EndUndo();

		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//

kcmShapeFix::kcmShapeFix(void)
{
	m_strName = "��������";
	m_strAlias = "FixShape";
	m_strDesc = "������������";

	m_nEntType = KCT_ENT_ALL;
}

kcmShapeFix::~kcmShapeFix(void)
{
}

TopoDS_Shape	kcmShapeFix::DoFixShape(const TopoDS_Shape& aS)
{
	TopoDS_Shape aNS;
	if(aS.IsNull()) return aNS;

	try{
		ShapeFix_Shape fs(aS);
		fs.Perform();
		aNS = fs.Shape();
	}catch(Standard_Failure){}

	return aNS;
}
