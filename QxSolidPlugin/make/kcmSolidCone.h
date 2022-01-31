//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLID_CONE_H_
#define _KCM_SOLID_CONE_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputPoint;
class kiInputLength;
class kcPreviewCircle;
class kcPreviewLine;
class kiInputVector;


class kcmSolidCone : public kiCommand
{
public:
	kcmSolidCone(void);
	virtual ~kcmSolidCone(void);

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

protected:
	BOOL					DoBuildCone();

protected:
	int						m_nState;//״̬
	kiOptionSet				m_optionSet;
	kiOptionSet				m_optionSet2;
	bool					m_bTCone;//�Ƿ�ƽ��׶��
	kiInputPoint			*m_pInputCenter;
	kPoint3					m_aCenter;
	kiInputLength			*m_pInputRadius;
	kiInputLength			*m_pInputRadius2;
	double					m_dRadius;
	kiInputLength			*m_pInputHeight;
	double					m_dAngle;

	kcPreviewCircle			*m_pPreviewCircle;
	kcPreviewCircle			*m_pPreviewCircle2;
	kcPreviewLine			*m_pPreviewLine;

protected:
	kPoint3					m_aXPoint;//X����ʼ���ȷ��
};

#endif