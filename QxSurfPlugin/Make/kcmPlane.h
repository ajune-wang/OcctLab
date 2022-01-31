//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_PLANE_H_
#define _KCM_PLANE_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputPoint;
class kcPreviewRect;

class kcmPlane : public kiCommand
{
public:
	kcmPlane(void);
	virtual ~kcmPlane(void);

	// ִ��һ�γ�ʼ�����ͷŵĵط���
	virtual  BOOL			CanFinish();//�����Ƿ�������

	//���빤�߽���ʱ
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	//����ĳ�ʼ�����ͷŴ���,ÿ����������������Եĳ�ʼ�������ٲ�����
	virtual int				OnInitialize();
	virtual int				OnDestroy();

	// ִ�к���
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);
	virtual int				OnApply();

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			InitInputToolQueue();
	virtual int				OnMouseMove(kuiMouseInput& mouseInput);
	virtual int				OnLButtonUp(kuiMouseInput& mouseInput);

protected:
	int						m_nState;
	kPoint3					m_aPnt1;
	kPoint3					m_aPnt2;
	kiInputPoint			*m_pInputPoint;
	kiInputPoint			*m_pInputPoint2;
	kcPreviewRect			*m_pPreviewRect;
};

#endif