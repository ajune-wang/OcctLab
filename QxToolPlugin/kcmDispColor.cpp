#include "StdAfx.h"
#include "kiInputEntityTool.h"
#include "kiSelSet.h"
#include "kcEntity.h"
#include "kcmDispColor.h"

kcmDispColor::kcmDispColor(void)
{
	m_strName = "Set Color";
	m_strAlias = "SetCol";
	m_strDesc = "���ö�����ɫ";

	m_pInputEntityTool = NULL;
}

kcmDispColor::~kcmDispColor(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmDispColor::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int		kcmDispColor::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntityTool)
	{
		if(m_pInputEntityTool->IsInputDone())
		{
			kiSelSet *pSelSet = m_pInputEntityTool->GetSelSet();
			if(SetColor(pSelSet))
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

int		kcmDispColor::OnExecute()
{
	if(!IsGlobalSelSetEmpty())
	{
		kiSelSet *pSelSet = GetGlobalSelSet();
		if(SetColor(pSelSet))
		{
			ClearGlobalSelSet();

			Done();
			Redraw();

			return KSTA_DONE;
		}
	}
	return KSTA_CONTINUE;
}

int		kcmDispColor::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmDispColor::CreateInputTools()
{
	m_pInputEntityTool = new kiInputEntityTool(this,"ѡ��һ����������:");
	m_pInputEntityTool->SetOption(KCT_ENT_ALL,false);
	m_pInputEntityTool->SetNaturalMode(true);

	return TRUE;
}

BOOL	kcmDispColor::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntityTool);
	return TRUE;
}

BOOL	kcmDispColor::InitInputToolQueue()
{
	AddInputTool(m_pInputEntityTool);
	return TRUE;
}

BOOL	kcmDispColor::SetColor(kiSelSet *pSelSet)
{
	if(pSelSet->GetSelCount() <= 0)
		return FALSE;

	CColorDialog dlg;
	if(dlg.DoModal() == IDOK)
	{
		COLORREF col = dlg.GetColor();
		double r = GetRValue(col) / 256.0,g = GetGValue(col) / 256.0,b = GetBValue(col) / 256.0;
		//������ɫ
		pSelSet->InitSelected();
		while(pSelSet->MoreSelected())
		{
			kiSelEntity se = pSelSet->CurSelected();
			se._pEntity->SetColor(r,g,b,FALSE);

			pSelSet->NextSelected();
		}
	}

	return TRUE;
}