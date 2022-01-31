//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_LOFT_SURFACE_H_
#define _KCM_LOFT_SURFACE_H_

#include "kiCommand.h"

class kiInputEntityTool;

class kcmLoftSurface : public kiCommand
{
public:
	kcmLoftSurface(void);
	virtual ~kcmLoftSurface(void);

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
	BOOL					DoLoft();

protected:
	kiInputEntityTool		*m_pInputStart;
	kiInputEntityTool		*m_pInputThrough;
	kiInputEntityTool		*m_pInputEnd;
};

#endif