//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_INTER_CURVE_H_
#define _KCM_INTER_CURVE_H_

#include "kiCommand.h"

class kiInputEntityTool;

class kcmInterCurve : public kiCommand
{
public:
	kcmInterCurve(void);
	virtual ~kcmInterCurve(void);

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

protected:
	BOOL					DoIntersect();

protected:
	kiInputEntityTool		*m_pInputEntity1;
	kiInputEntityTool		*m_pInputEntity2;
};

#endif