//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_RULED_SURFACE_H_
#define _KCM_RULED_SURFACE_H_

#include "kiCommand.h"

class kiInputEntityTool;

class kcmRuledSurface : public kiCommand
{
public:
	kcmRuledSurface(void);
	virtual ~kcmRuledSurface(void);

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
	//
	BOOL					BuildRuledSurface();

protected:
	kiInputEntityTool		*m_pInputCurve1;
	kiInputEntityTool		*m_pInputCurve2;
};

#endif