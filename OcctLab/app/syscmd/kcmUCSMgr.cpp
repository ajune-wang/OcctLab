#include "StdAfx.h"
#include "kcModel.h"
#include "kcBasePlane.h"
#include "kcPreviewObj.h"
#include "kcPreviewText.h"
#include "kiInputPoint.h"
#include "QxBaseUtils.h"
#include "kcgTextLib.h"
#include "kvCoordSystemMgr.h"
#include "kcmUcsMgr.h"

//////////////////////////////////////////////////////////////////////////
// ���㴴������ϵ
#define KC_STA_INPUT_ORG		1
#define KC_STA_INPUT_XPOINT		2
#define KC_STA_INPUT_YPOINT		3

kcmCreateUCSByThreePoint::kcmCreateUCSByThreePoint()
{
	m_strName = "����";
	m_strAlias = "UCS3P";
	m_strDesc = "���㴴������ϵ";

	m_pInputPointOrg = NULL;
	m_pInputPointX = NULL;
	m_pInputPointY = NULL;

	m_pPreviewXLine = NULL;
	m_pPreviewYLine = NULL;
}

kcmCreateUCSByThreePoint::~kcmCreateUCSByThreePoint()
{

}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmCreateUCSByThreePoint::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmCreateUCSByThreePoint::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputPointOrg)
	{
		if(m_pInputPointOrg->IsInputDone())
		{
			m_nState = KC_STA_INPUT_XPOINT;
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputPointX)
	{
		if(m_pInputPointX->IsInputDone())
		{
			m_nState = KC_STA_INPUT_YPOINT;
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputPointY)
	{
		if(m_pInputPointY->IsInputDone())
		{
			if(DoCreateUCS())
			{
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

BOOL	kcmCreateUCSByThreePoint::DoCreateUCS()
{
	//kVector3 X,Y,Z;
	//X = m_pointX - m_pointOrg;
	//Y = m_pointY - m_pointOrg;
	//if(!X.normalize() || !Y.normalize())
	//	return FALSE;

	//Z = X ^ Y;
	//if(!Z.normalize())
	//	return FALSE;

	//Y = Z ^ X;
	//kvCoordSystem cs;
	//cs.Set(m_pointOrg,X,Y,Z);
	//std::string sname = kvCoordSystemMgr::GenCoordSystemName();
	//cs.SetName(sname.c_str());

	//kvCoordSystemMgr& csMgr = m_pModel->GetCoordSystemMgr();
	//kvCoordSystem *pCS = csMgr.AddCoordSystem(cs);
	//if(!pCS)
	//	return FALSE;
	//csMgr.SetCurrentCS(pCS);

	//kvWorkPlane *pWorkPlane = glb_GetCurrentWorkPlane();
	//ASSERT(pWorkPlane);
	//pWorkPlane->SetType(XY_WORKPLANE);
	//SetKVType(KVT_Top);

	return TRUE;
}

int		kcmCreateUCSByThreePoint::OnExecute()
{
	kcDocContext *pDocCtx = GetDocContext();

	m_nState = KC_STA_INPUT_ORG;

	m_pPreviewXLine = new kcPreviewLine(GetAISContext(),pDocCtx);
	m_pPreviewXLine->SetColor(1,1,1);

	m_pPreviewYLine = new kcPreviewLine(GetAISContext(),pDocCtx);
	m_pPreviewYLine->SetColor(1,1,1);

	return KSTA_CONTINUE;
}

int		kcmCreateUCSByThreePoint::OnEnd(int nCode)
{
	KC_SAFE_DELETE(m_pPreviewXLine);
	KC_SAFE_DELETE(m_pPreviewYLine);

	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmCreateUCSByThreePoint::CreateInputTools()
{
	m_pInputPointOrg = new kiInputPoint(this,"������ԭ��");
	m_pInputPointOrg->Init(&m_pointOrg);

	m_pInputPointX = new kiInputPoint(this,"����X���ϵ�");
	m_pInputPointX->Init(&m_pointX);
	m_pInputPointX->SetRealtimeUpdate(true);

	m_pInputPointY = new kiInputPoint(this,"����Y���ϵ�");
	m_pInputPointY->Init(&m_pointY);
	m_pInputPointY->SetRealtimeUpdate(true);

	return TRUE;
}

BOOL	kcmCreateUCSByThreePoint::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputPointOrg);
	KC_SAFE_DELETE(m_pInputPointX);
	KC_SAFE_DELETE(m_pInputPointY);

	return TRUE;
}

BOOL	kcmCreateUCSByThreePoint::InitInputToolQueue()
{
	AddInputTool(m_pInputPointOrg);
	AddInputTool(m_pInputPointX);
	AddInputTool(m_pInputPointY);

	return TRUE;
}

int		kcmCreateUCSByThreePoint::OnMouseMove(kuiMouseInput& mouseInput)
{
	kiCommand::OnMouseMove(mouseInput);

	if(m_nState == KC_STA_INPUT_XPOINT)
	{
		if(m_pPreviewXLine)
		{
			m_pPreviewXLine->Update(KCG_GP_PNT(m_pointOrg),KCG_GP_PNT(m_pointX));
			m_pPreviewXLine->Display(TRUE);
		}
	}
	else if(m_nState == KC_STA_INPUT_YPOINT)
	{
		if(m_pPreviewYLine)
		{
			m_pPreviewYLine->Update(KCG_GP_PNT(m_pointOrg),KCG_GP_PNT(m_pointY));
			m_pPreviewYLine->Display(TRUE);
		}
	}


	return KSTA_CONTINUE;
}

//////////////////////////////////////////////////////////////////////////
// ƽ�Ƶ�ǰ����ϵ
kcmCreateUCSByMove::kcmCreateUCSByMove()
{

}

kcmCreateUCSByMove::~kcmCreateUCSByMove()
{

}

