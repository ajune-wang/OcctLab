#include "StdAfx.h"
#include "kiInputEntityTool.h"
#include "kuiInterface.h"
#include "kiSelSet.h"
#include "kcEntity.h"
#include "kcModel.h"
#include "kvGrid.h"
#include "kcBasePlane.h"
#include "FilterDispDialog.h"
#include "kcmDispVisible.h"

kcmDispHideSelect::kcmDispHideSelect(void)
{
	m_strName = "����ѡ�����";
	m_strAlias = "HideSel";
	m_strDesc = "����ѡ�����";

	m_pInputEntity = NULL;
}

kcmDispHideSelect::~kcmDispHideSelect(void)
{

}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmDispHideSelect::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmDispHideSelect::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntity)
	{
		if(m_pInputEntity->IsInputDone())
		{
			kiSelSet *pSelSet = m_pInputEntity->GetSelSet();
			if(HideSelSet(pSelSet))
			{
				Done();
				Redraw();

				return KSTA_DONE;
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

int		kcmDispHideSelect::OnExecute()
{
	if(!IsGlobalSelSetEmpty())
	{
		kiSelSet *pSelSet = GetGlobalSelSet();
		if(HideSelSet(pSelSet))
		{
			ClearGlobalSelSet();

			Done();
			Redraw();

			return KSTA_DONE;
		}
	}
	return KSTA_CONTINUE;
}

BOOL	kcmDispHideSelect::HideSelSet(kiSelSet *pSelSet)
{
	kcModel *pModel = GetModel();
	kuiModelTreeInterface *pIModelTree = pModel->GetModelTreeInterface();
	pSelSet->InitSelected();
	while(pSelSet->MoreSelected())
	{
		kiSelEntity se = pSelSet->CurSelected();
		se._pEntity->Display(false,FALSE);
		//
		if(pIModelTree){
			pIModelTree->ShowEntity(se._pEntity->GetName(),FALSE);
		}

		pSelSet->NextSelected();
	}

	return TRUE;
}

int		kcmDispHideSelect::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmDispHideSelect::CreateInputTools()
{
	m_pInputEntity = new kiInputEntityTool(this,"ѡ��Ҫ���ض���");
	m_pInputEntity->SetOption(KCT_ENT_ALL);

	return TRUE;
}

BOOL	kcmDispHideSelect::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntity);
	return TRUE;
}

BOOL	kcmDispHideSelect::InitInputToolQueue()
{
	AddInputTool(m_pInputEntity);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
kcmDispShowAll::kcmDispShowAll()
{
	m_strName = "��ʾ���ض���";
	m_strAlias = "ShowAll";
	m_strDesc = "��ʾ�������ض���";
}

kcmDispShowAll::~kcmDispShowAll()
{

}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmDispShowAll::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

int	kcmDispShowAll::OnExecute()
{
	GetModel()->ShowAll(FALSE);
	Redraw();
	Done();

	return KSTA_DONE;
}

//////////////////////////////////////////////////////////////////////////
// ������ʾ
kcmDispTypeFilter::kcmDispTypeFilter()
{
	m_strName = "���͹�����ʾ";
	m_strAlias = "TypeFilterDisplay";
	m_strDesc = "������ʾ";
}

kcmDispTypeFilter::~kcmDispTypeFilter()
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmDispTypeFilter::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

int		kcmDispTypeFilter::OnExecute()
{
	CFilterDispDialog dlg;
	if(IDOK == dlg.DoModal())
	{
		int nFlag = 0;
		if(dlg.m_bDispPoint)	nFlag |= KCT_ENT_POINT;
		if(dlg.m_bDispCurve)	nFlag |= KCT_ENT_EDGE;
		if(dlg.m_bDispWire)		nFlag |= KCT_ENT_WIRE;
		if(dlg.m_bDispSurface)	nFlag |= KCT_ENT_FACE;
		if(dlg.m_bDispShell)	nFlag |= KCT_ENT_SHELL;
		if(dlg.m_bDispSolid)	nFlag |= KCT_ENT_SOLID;
		if(dlg.m_bDispCompound)	nFlag |= KCT_ENT_COMPOUND;

		GetModel()->FilterDisplay(nFlag,FALSE);
		Redraw();
	}

	Done();
	return KSTA_DONE;
}

///////////////////////////////////////////////////////////////////////////////
//
kcmHideGridDisplay::kcmHideGridDisplay()
{
	InitSet("����������ʾ","HideGrid","���ص�ǰ����ƽ���������ʾ");
}

kcmHideGridDisplay::~kcmHideGridDisplay()
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL kcmHideGridDisplay::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

int kcmHideGridDisplay::OnExecute()
{
	kcModel *pModel = GetModel();
	if(pModel != NULL){
		kcBasePlane *pBasePlane = GetCurrentBasePlane();
		if(pBasePlane != NULL){
			kvGrid *pGrid = pBasePlane->GetGrid();
			if(pGrid != NULL && pGrid->IsVisible()){
				pGrid->SetVisible(false);
				//
				Redraw();
			}
		}else{
			ASSERT(FALSE);
		}
	}
	return KSTA_DONE;
}


