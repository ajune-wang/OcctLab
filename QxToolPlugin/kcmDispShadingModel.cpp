#include "StdAfx.h"
#include "kiInputEntityTool.h"
#include "kiSelSet.h"
#include "kcEntity.h"
#include "kcmdispshadingmodel.h"

kcmDispShadingModel::kcmDispShadingModel(void)
{
	m_nModel = -1;
}

kcmDispShadingModel::~kcmDispShadingModel(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmDispShadingModel::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmDispShadingModel::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntityTool)
	{
		if(m_pInputEntityTool->IsInputDone())
		{
			kiSelSet *pSelSet = m_pInputEntityTool->GetSelSet();
			if(SetModel(pSelSet))
			{
				m_pInputEntityTool->ClearSel();

				Done();
				Redraw();

				return KSTA_DONE;
			}
		}
		else
		{
			return KSTA_CANCEL;
		}
	}
	return KSTA_CONTINUE;
}


int		kcmDispShadingModel::OnExecute()
{
	if(!IsGlobalSelSetEmpty())
	{
		kiSelSet *pSelSet = GetGlobalSelSet();
		if(SetModel(pSelSet))
		{
			ClearGlobalSelSet();

			Done();
			Redraw();

			return KSTA_DONE;
		}
	}
	return KSTA_CONTINUE;
}

int		kcmDispShadingModel::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmDispShadingModel::CreateInputTools()
{
	m_pInputEntityTool = new kiInputEntityTool(this,"ѡ��һ����������:");
	m_pInputEntityTool->SetOption(KCT_ENT_ALL);

	return TRUE;
}

BOOL	kcmDispShadingModel::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntityTool);
	return TRUE;
}

BOOL	kcmDispShadingModel::InitInputToolQueue()
{
	AddInputTool(m_pInputEntityTool);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// �����ʾ
kcmDispWireFrame::kcmDispWireFrame()
{
	m_strName = "WireFrame";
	m_strAlias = "WF disp";
	m_strDesc = "�����ʾ";

	m_nModel = 0;
}

kcmDispWireFrame::~kcmDispWireFrame()
{

}

BOOL	kcmDispWireFrame::SetModel(kiSelSet *pSelSet)
{
	pSelSet->InitSelected();
	while(pSelSet->MoreSelected())
	{
		kiSelEntity se = pSelSet->CurSelected();
		se._pEntity->SetDisplayModel(m_nModel,FALSE);

		pSelSet->NextSelected();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// ��Ⱦ��ʾ
kcmDispShading::kcmDispShading()
{
	m_strName = "Shading Display";
	m_strAlias = "SD disp";
	m_strDesc = "��Ⱦ��ʾ";

	m_nModel = 1;
}

kcmDispShading::~kcmDispShading()
{

}

BOOL	kcmDispShading::SetModel(kiSelSet *pSelSet)
{
	pSelSet->InitSelected();
	while(pSelSet->MoreSelected())
	{
		kiSelEntity se = pSelSet->CurSelected();
		se._pEntity->SetDisplayModel(m_nModel,FALSE);

		pSelSet->NextSelected();
	}

	return TRUE;
}
