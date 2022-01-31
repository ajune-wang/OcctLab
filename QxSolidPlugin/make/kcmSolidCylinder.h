//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLID_CYLINDER_H_
#define _KCM_SOLID_CYLINDER_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputPoint;
class kiInputLength;
class kcPreviewCircle;
class kcPreviewLine;
class kiInputVector;

class kcmSolidCylinder : public kiCommand
{
public:
	kcmSolidCylinder(void);
	virtual ~kcmSolidCylinder(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

	virtual int				OnMouseMove(kuiMouseInput& mouseInput);
	virtual int				OnLButtonUp(kuiMouseInput& mouseInput);

protected:
	BOOL					DoBuild();

protected:
	int						m_nState;//״̬
	kiOptionSet				m_optionSet;
	kiInputPoint			*m_pInputCenter;
	kPoint3					m_aCenter;
	kiInputLength			*m_pInputRadius;
	double					m_dRadius;
	kiInputLength			*m_pInputHeight;
	double					m_dAngle;
	
	kcPreviewCircle			*m_pPreviewCircle;
	kcPreviewCircle			*m_pPreviewCircle2;
	kcPreviewLine			*m_pPreviewLine;
	kcPreviewLine			*m_pPreviewLine2;

protected:
	kPoint3					m_aXPoint;//X����ʼ���ȷ��
};

#endif