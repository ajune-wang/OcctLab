#include "StdAfx.h"
#include "kcEntity.h"
#include "QxAnimationShape.h"
#include "kiInputEntityTool.h"
#include "kcmAnimationDemo.h"

//////////////////////////////////////////////////////////////////////////
//
kcmAnimationDemo::kcmAnimationDemo(void)
{
	m_strName = "AnimationDemo";
	m_strAlias = "AnimDemo";
	m_strDesc = "������ʾ";

	m_pAnimationShape = NULL;
	m_pInputEntityTool = NULL;
	m_nTimer = 0;
	m_dAngle = 0.0;
}

kcmAnimationDemo::~kcmAnimationDemo(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmAnimationDemo::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmAnimationDemo::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntityTool)
	{
		if(m_pInputEntityTool->IsInputDone())
		{
			kiSelSet *pSelSet = m_pInputEntityTool->GetSelSet();
			pSelSet->InitSelected();
			if(pSelSet->MoreSelected())
			{
				kiSelEntity se = pSelSet->CurSelected();
				Handle(AIS_Shape) aiShape = Handle(AIS_Shape)::DownCast(se.AISObject());
				if(!aiShape.IsNull())
				{
					m_pAnimationShape = new QxAnimationShape(
						GetAISContext(),aiShape->Shape());
					m_pAnimationShape->Reset(FALSE);
					m_pAnimationShape->Display(TRUE);

					//������ʱ��
					this->SetTimer(1,500);
				}
				else
				{
					Done();
					return KSTA_DONE;
				}
			}
			else
			{
				Done();

				return KSTA_DONE;
			}
		}
	}

	return KSTA_CONTINUE;
}

int	kcmAnimationDemo::OnExecute()
{
	m_dAngle = 0.0;
	return KSTA_CONTINUE;
}

int		kcmAnimationDemo::OnEnd(int nCode)
{
	return nCode;
}

// ��ʱ��������
int kcmAnimationDemo::OnTimer(int nId,DWORD dwTime)
{
	if(nId == 1){
		m_dAngle += 0.2;
		KTRACE("\n angle is %f.",m_dAngle);
		if(m_dAngle > K_PI * 2.0){
			m_pAnimationShape->Hide(TRUE);
			KillTimer(nId);
			m_nTimer = 0;

			Done();
		}else{
			gp_Trsf trsf;
			trsf.SetRotation(gp_Ax1(),m_dAngle);
			m_pAnimationShape->SetTrsf(trsf,TRUE);
		}
	}

	return 1;
}

// ������Ҫ�����빤��
BOOL	kcmAnimationDemo::CreateInputTools()
{
	m_pInputEntityTool = new kiInputEntityTool(this,"ѡ��һ������:");
	m_pInputEntityTool->SetOption(KCT_ENT_ALL,1);
	m_pInputEntityTool->SetNaturalMode(true);
	
	return TRUE;
}

BOOL	kcmAnimationDemo::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntityTool);
	
	return TRUE;
}

BOOL	kcmAnimationDemo::InitInputToolQueue()
{
	AddInputTool(m_pInputEntityTool);
	return TRUE;
}
