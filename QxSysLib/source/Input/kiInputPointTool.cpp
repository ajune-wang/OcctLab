#include "StdAfx.h"
#include "kcBasePlane.h"
#include "kiCommand.h"
#include "kcSnapMngr.h"
#include "QxSurfLib.h"
#include "kcPreviewText.h"
#include "kvCoordSystem.h"
#include "kcDocContext.h"
#include "kiParseString.h"
#include "kiInputPointTool.h"

kiInputPointTool::kiInputPointTool(kiCommand *pCommand,const char *pszPrompt,kiOptionSet *pOptionSet)
:kiInputTool(pCommand,pszPrompt,pOptionSet)
{
	_pPoint = NULL;
	_bSnapPoint = false;
	_bSpacePoint = false;//Ĭ�Ϲ���ƽ���ϵ㡣
	_bRealtimeUpdate = false;
}

kiInputPointTool::kiInputPointTool(kiInputTool *pParent,const char *pszPrompt,kiOptionSet *pOptionSet)
:kiInputTool(pParent,pszPrompt,pOptionSet)
{
	_pPoint = NULL;
	_bSnapPoint = false;
	_bSpacePoint = false;//Ĭ�Ϲ���ƽ���ϵ㡣
	_bRealtimeUpdate = false;
}

kiInputPointTool::~kiInputPointTool(void)
{
}

//��ʼ��
BOOL  kiInputPointTool::Initialize(kPoint3 *pPoint,bool bSpacePoint)
{
	_pPoint = pPoint;
	_bSpacePoint = bSpacePoint;
	return TRUE;
}

// �Ƿ���ֵʵʱ����
void  kiInputPointTool::SetRealtimeUpdate(bool bEnable)
{
	_bRealtimeUpdate = bEnable;
}

int  kiInputPointTool::OnBegin()
{
	ASSERT(_pPoint);
	_bSnapPoint = false;

	return KSTA_CONTINUE;
}

int  kiInputPointTool::OnEnd()
{
	return KSTA_DONE;
}

//�����Ϣ
int  kiInputPointTool::OnLButtonUp(kuiMouseInput& mouseInput)
{
	CalcPoint(mouseInput);

	SetInputState(KINPUT_DONE);
	DoNotify();

	return KINPUT_DONE;
}

int  kiInputPointTool::OnMouseMove(kuiMouseInput& mouseInput)
{
	if(IsInputRunning() && _bRealtimeUpdate){
		//�����
		CalcPoint(mouseInput);

		//ʵʱ���µ�
		if(m_pCommand){
			m_pCommand->OnInputChanged(this);
		}
	}
	return kiInputTool::OnMouseMove(mouseInput);
}

// �Ҽ���Ϊ��ȡ����ȡ����.
//
int  kiInputPointTool::OnRButtonUp(kuiMouseInput& mouseInput)
{
	SetInputState(KINPUT_CANCEL);
	return KSTA_CANCEL;
}

// ��ȡ��ʾ�ַ���
void  kiInputPointTool::DoGetPromptText(std::string& str)
{
	char sbuf[128];
	if(GetCurOptionSet() == NULL)
		sprintf_s(sbuf,127,"%s<%f,%f,%f>",m_szPromptText.c_str(),_pPoint->x(),_pPoint->y(),_pPoint->z());
	else
		sprintf_s(sbuf,127,"%s<%f,%f,%f> ",m_szPromptText.c_str(),_pPoint->x(),_pPoint->y(),_pPoint->z());
	str = sbuf;
}

// ��InputEdit������ɺ󣬵��øú���
//
int  kiInputPointTool::OnEditInputFinished(const char *pInputText)
{
	//���벻��Ϊ��
	if(!pInputText)
		return KSTA_CONTINUE;

	kPoint3 pnt;
	kiParseStringToPoint3 psp(pInputText);
	if(psp.Parse(pnt)){
		*_pPoint = pnt;

		SetInputState(KINPUT_DONE);
		End();

		DoNotify();

		return KSTA_DONE;
	}

	return KSTA_CONTINUE;
}

//���µ��ֵ
void  kiInputPointTool::CalcPoint(kuiMouseInput& mouseInput)
{
	kcBasePlane *pBasePlane = GetCurrentBasePlane();
	ASSERT(pBasePlane);

	_bSnapPoint = false;
	//�Ƿ����Զ������
	if(m_pSnapMgr && m_pSnapMgr->HasSnapPoint()){
		kiSnapPoint sp = m_pSnapMgr->GetSnapPoint();
		_point = sp._point;
		_bSnapPoint = true;
	}else{
		//������ƽ���ϵĵ�
		pBasePlane->PixelToWorkPlane(mouseInput.m_x,mouseInput.m_y,_point);
	}

	if(!_bSpacePoint){
		kvCoordSystem cs;
		pBasePlane->GetCoordSystem(cs);
		//�������ƽ����,ͶӰ��ƽ����
		if(!QxSurfLib::IsPointOnPlane(_point,cs.Org(),cs.Z())){
			kPoint3 pp;
			QxSurfLib::ProjectPointToPlane(_point,cs.Org(),cs.Z(),pp);
			_point = pp;

			if(_bSnapPoint)
				_bSnapPoint = false;
		}
	}
	*_pPoint = _point;
}

