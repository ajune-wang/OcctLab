#include "StdAfx.h"
#include "kcEntity.h"
#include "kcModel.h"
#include "kiSelSet.h"
#include "kiInputEntityTool.h"
#include "kcmDelete.h"

kcmDelete::kcmDelete(void)
{
	m_strName = "Delete";
	m_strAlias = "Del";
	m_strDesc = "ɾ������";

	m_pInputEntity = NULL;
}

kcmDelete::~kcmDelete(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmDelete::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	   kcmDelete::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntity)
	{
		if(m_pInputEntity->GetSelCount() > 0)
		{
			kiSelSet *pSelSet = m_pInputEntity->GetSelSet();
			//
			DeleteEntity(pSelSet);
			//
			pSelSet->Clear();

			Done();
			Redraw();

			return KSTA_DONE;
		}
	}
	return KSTA_CONTINUE;
}

int  kcmDelete::OnDestroy()
{
	KC_SAFE_DELETE(m_pInputEntity);
	return KSTA_CONTINUE;
}

// ִ�к���
int  kcmDelete::OnExecute()
{
	kiSelSet *pSelSet = GetGlobalSelSet();
	if(!pSelSet->IsEmpty()){

		pSelSet->UnHilight(FALSE);

		DeleteEntity(pSelSet);
		//
		pSelSet->Clear();

		return KSTA_DONE;
	}
	return KSTA_CONTINUE;
}

// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
BOOL	kcmDelete::CreateInputTools()
{
	m_pInputEntity = new kiInputEntityTool(this,"ѡ��Ҫɾ������:");
	if(!m_pInputEntity)
		return FALSE;

	m_pInputEntity->SetOption(KCT_ENT_ALL);
	m_pInputEntity->SetNaturalMode(true);

	return TRUE;
}

// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
BOOL	kcmDelete::InitInputToolQueue()
{
	if(m_pInputEntity)
		AddInputTool(m_pInputEntity);

	return TRUE;
}

//ɾ��
BOOL	kcmDelete::DeleteEntity(kiSelSet *pSelSet)
{
	kcModel *pModel = GetModel();
	pModel->BeginUndo(GetName());
	pSelSet->InitSelected();
	while(pSelSet->MoreSelected())
	{
		kiSelEntity se = pSelSet->CurSelected();
		if(se._pEntity)
			pModel->DelEntity(se._pEntity,FALSE);

		pSelSet->NextSelected();
	}
	pModel->EndUndo();

	return TRUE;
}