//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLID_SPHERE_H_
#define _KCM_SOLID_SPHERE_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputPoint;
class kiOptionSet;
class kcPreviewCircle;
class kiInputDoubleTool;

class kcmSolidSphere : public kiCommand
{
public:
	kcmSolidSphere(void);
	virtual ~kcmSolidSphere(void);

	// ִ��һ�γ�ʼ�����ͷŵĵط���
	virtual  int			Destroy();

	//ִ�кͽ����ӿ�
	virtual	 int			Execute();
	virtual  int			End(int nCode);
	virtual  BOOL			CanFinish();//�����Ƿ�������

	//���빤�߽���ʱ
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			InitInputToolQueue();
	virtual int				OnMouseMove(kuiMouseInput& mouseInput);
	virtual int				OnLButtonUp(kuiMouseInput& mouseInput);

protected:
	//
	BOOL					MakeSphere();

protected:
	int						m_nState;
	kPoint3					m_dCenter;//Բ��
	double					m_dRadius;//�뾶

protected:
	kiInputPoint			*m_pInputPoint;//�����
	kiInputDoubleTool		*m_pInputRadius;//����뾶
	kcPreviewCircle			*m_pPreviewCircle;
};

#endif