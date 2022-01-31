//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_ANIMATION_DEMO_H_
#define _KCM_ANIMATION_DEMO_H_

#include "kiCommand.h"

class kiInputEntityTool;
class QxAnimationShape;

class kcmAnimationDemo : public kiCommand
{
public:
	kcmAnimationDemo(void);
	virtual ~kcmAnimationDemo(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);

	// ��ʱ��������
	virtual int				OnTimer(int nId,DWORD dwTime);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	QxAnimationShape		*m_pAnimationShape;
	kiInputEntityTool		*m_pInputEntityTool;
	UINT					m_nTimer;
	double					m_dAngle;
};

#endif