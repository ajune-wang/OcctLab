//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLID_REVOL_H_
#define _KCM_SOLID_REVOL_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputEntityTool;
class kiInputVector;

class kcmSolidRevol : public kiCommand
{
public:
	kcmSolidRevol(void);
	virtual ~kcmSolidRevol(void);

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
	BOOL					MakeRevolShape();

protected:
	int						m_nState;
	kiInputEntityTool		*m_pInputEntity;
	kiInputVector			*m_pInputVec;

protected:
	kiOptionSet				m_optionSet;
	double					m_dAngle;//�Ƕ�
};

#endif