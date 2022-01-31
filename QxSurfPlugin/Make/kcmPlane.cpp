#include "StdAfx.h"
#include "kiInputPoint.h"
#include "kcModel.h"
#include "kcBasePlane.h"
#include "kcPreviewObj.h"
#include "kcSysUtils.h"
#include "kcEntity.h"
#include "kcmPlane.h"

#define STA_PLANE_INPUT_PNT1		1
#define STA_PLANE_INPUT_PNT2		2

kcmPlane::kcmPlane(void)
{
	m_strName = "Plane";
	m_strAlias = "Pln";
	m_strDesc = "�������ƽ��";

	m_pInputPoint = NULL;
	m_pInputPoint2 = NULL;
	m_pPreviewRect = NULL;
}

kcmPlane::~kcmPlane(void)
{
}

// ִ��һ�γ�ʼ�����ͷŵĵط���
BOOL	kcmPlane::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

//���빤�߽���ʱ
int	kcmPlane::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputPoint)
	{
		m_nState = STA_PLANE_INPUT_PNT2;
		NavToNextTool();
	}
	else if(pTool == m_pInputPoint2)
	{
		kvCoordSystem cs;
		kcBasePlane *pWorkPlane = GetCurrentBasePlane();
		ASSERT(pWorkPlane);
		pWorkPlane->GetCoordSystem(cs);
		TopoDS_Face aFace = kcg_BuildPlane(cs,kPoint3(m_aPnt1),kPoint3(m_aPnt2));
		if(!aFace.IsNull())
		{
			kcFaceEntity *pEntity = new kcFaceEntity;
			if(pEntity)
			{
				pEntity->SetFace(aFace);
				
				kcModel *pModel = GetModel();
				pModel->AddEntity(pEntity);

				Done();
				Redraw();

				return KSTA_DONE;
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

//����ĳ�ʼ�����ͷŴ���,ÿ����������������Եĳ�ʼ�������ٲ�����
int		kcmPlane::OnInitialize()
{
	return KSTA_CONTINUE;
}

int		kcmPlane::OnDestroy()
{
	KC_SAFE_DELETE(m_pPreviewRect);
	return KSTA_CONTINUE;
}

// ִ�к���
int		kcmPlane::OnExecute()
{
	ASSERT(m_pPreviewRect == NULL);
	m_nState = STA_PLANE_INPUT_PNT1;

	return KSTA_CONTINUE;
}

int		kcmPlane::OnEnd(int nCode)
{
	KC_SAFE_DELETE(m_pPreviewRect);

	return KSTA_CONTINUE;
}

int		kcmPlane::OnApply()
{
	return KSTA_CONTINUE;
}

// ������Ҫ�����빤��
BOOL	kcmPlane::CreateInputTools()
{
	m_pInputPoint = new kiInputPoint(this,"�����һ����:");
	m_pInputPoint2 = new kiInputPoint(this,"����ڶ�����:");
	m_pInputPoint->Init(&m_aPnt1);
	m_pInputPoint2->Init(&m_aPnt2);
	m_pInputPoint2->SetRealtimeUpdate(true);

	return TRUE;
}

BOOL	kcmPlane::InitInputToolQueue()
{
	AddInputTool(m_pInputPoint);
	AddInputTool(m_pInputPoint2);

	return TRUE;
}

int		kcmPlane::OnMouseMove(kuiMouseInput& mouseInput)
{
	if(m_nState == STA_PLANE_INPUT_PNT2)
	{
		kcBasePlane *pWorkPlane = GetCurrentBasePlane();
		ASSERT(pWorkPlane);

		if(m_pPreviewRect == NULL)
		{
			m_pPreviewRect = new kcPreviewRect(GetAISContext(),GetDocContext());
			m_pPreviewRect->SetColor(0.0,1.0,1.0);
			m_pPreviewRect->Init(gp_Pnt(m_aPnt1[0],m_aPnt1[1],m_aPnt1[2]),pWorkPlane);
		}
		if(m_pPreviewRect)
		{
			m_pPreviewRect->Update(gp_Pnt(m_aPnt2[0],m_aPnt2[1],m_aPnt2[2]));
			m_pPreviewRect->Display(TRUE);

			Redraw();
		}
	}
	return kiCommand::OnMouseMove(mouseInput);
}

int		kcmPlane::OnLButtonUp(kuiMouseInput& mouseInput)
{
	return kiCommand::OnLButtonUp(mouseInput);
}
