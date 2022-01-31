//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SURFACE_PIPE_H_
#define _KCM_SURFACE_PIPE_H_

#include "kiCommand.h"

class kiInputEntityTool;
class kiInputDoubleTool;

class kcmSurfacePipe : public kiCommand
{
public:
	kcmSurfacePipe(void);
	virtual ~kcmSurfacePipe(void);

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
	BOOL					BuildPipe();

protected:
	kiInputEntityTool		*m_pPickPathTool;
	kiInputDoubleTool		*m_pInputRadiusTool;
	double					m_dRadius;
};

// ����ɨ��
class kcmSurfaceSweep1 : public kiCommand
{
public:
	kcmSurfaceSweep1(void);
	virtual ~kcmSurfaceSweep1(void);

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
	BOOL					BuildSweep();

protected:
	kiOptionSet				m_optionSet;
	kiInputEntityTool		*m_pPickPathTool;
	kiInputEntityTool		*m_pPickSectTool;
	int						m_nTrihedronType;
	kiOptionEnum			*m_pTrihedronOpt;
};

// �������ɨ��
class kcmSurfaceSweepN : public kiCommand
{
public:
	kcmSurfaceSweepN(void);
	virtual ~kcmSurfaceSweepN(void);

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
	BOOL					BuildSweep();

protected:
	kiInputEntityTool		*m_pPickPathTool;
	kiInputEntityTool		*m_pPickSectTool;
};

#endif