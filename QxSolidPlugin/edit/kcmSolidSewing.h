//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLID_SEWING_H_
#define _KCM_SOLID_SEWING_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputEntityTool;

class kcmSolidSewing : public kiCommand
{
public:
	kcmSolidSewing(void);
	virtual ~kcmSolidSewing(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL				CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int				OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL				CreateInputTools();
	virtual BOOL				DestroyInputTools();
	virtual BOOL				InitInputToolQueue();

protected:
	BOOL						DoSewing();

protected:
	kiInputEntityTool			*m_pInputEntity;
	kiOptionSet				m_optionSet;
	double					m_dSewingTol;//��Ͼ���
};

#endif