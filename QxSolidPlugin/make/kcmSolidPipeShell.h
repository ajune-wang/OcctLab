//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLID_PIPE_SHELL_H_
#define _KCM_SOLID_PIPE_SHELL_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputEntityTool;
class kiInputDoubleTool;

class kcmSolidPipeShell : public kiCommand
{
public:
	kcmSolidPipeShell(void);
	virtual ~kcmSolidPipeShell(void);

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
	kiOptionSet				m_optionSet;
	kiInputEntityTool		*m_pInputSpline;
	kiInputDoubleTool		*m_pInputRadius;
	double					m_dRadius;

	kiOptionEnum			*m_pModelOptItem;
	kiOptionEnum			*m_pTransitionOptItem;
	bool					m_bBuildSolid;//�Ƿ񴴽�ʵ��
	bool					m_bContact;
	bool					m_bCorrect;
	int						m_nModel;
	int						m_nTranstion;
};

class kcmSolidSweepShell : public kiCommand
{
public:
	kcmSolidSweepShell(void);
	virtual ~kcmSolidSweepShell(void);

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
	kiOptionSet				m_optionSet;
	kiInputEntityTool		*m_pInputSpline;
	kiInputEntityTool		*m_pInputProfile;

	kiOptionEnum			*m_pModelOptItem;
	kiOptionEnum			*m_pTransitionOptItem;
	bool					m_bBuildSolid;//�Ƿ񴴽�ʵ��
	bool					m_bContact;
	bool					m_bCorrect;
	int						m_nModel;
	int						m_nTranstion;
};


#endif