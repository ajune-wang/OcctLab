//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLID_PIPE_H_
#define _KCM_SOLID_PIPE_H_

#include "kiCommand.h"

class kiInputEntityTool;
class kiInputDoubleTool;

class kcmSolidPipe : public kiCommand
{
public:
	kcmSolidPipe(void);
	virtual ~kcmSolidPipe(void);

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
	BOOL					BuildPipe();

protected:
	kiInputEntityTool		*m_pInputSpline;
	kiInputDoubleTool		*m_pInputRadius;
	double					m_dRadius;
};

class kcmSolidSweep : public kiCommand
{
public:
	kcmSolidSweep(void);
	virtual ~kcmSolidSweep(void);

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
	BOOL					BuildPipe();

protected:
	kiInputEntityTool		*m_pInputSpline;
	kiInputEntityTool		*m_pInputProfile;
};

#endif