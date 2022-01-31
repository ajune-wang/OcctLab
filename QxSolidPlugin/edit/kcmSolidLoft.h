//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLID_LOFT_H_
#define _KCM_SOLID_LOFT_H_

#include "kiCommand.h"

class kiInputEntityTool;

class kcmSolidLoft : public kiCommand
{
public:
	kcmSolidLoft(void);
	virtual ~kcmSolidLoft(void);

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
	kiOptionSet				m_optionSet;
	kiInputEntityTool		*m_pInputStart;
	kiInputEntityTool		*m_pInputThrough;
	kiInputEntityTool		*m_pInputEnd;
	bool					m_bBuildSolid;
};

#endif