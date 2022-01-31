//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_REVOL_SURFACE_H_
#define _KCM_REVOL_SURFACE_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputEntityTool;
class kiInputVector;

class kcmRevolSurface : public kiCommand
{
public:
	kcmRevolSurface(void);
	virtual ~kcmRevolSurface(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

	//
protected:
	// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
	virtual BOOL			InitInputToolQueue();

protected:
	//
	BOOL					MakeRevolSurface();

protected:
	int						m_nState;
	kiInputEntityTool		*m_pInputEntity;
	kiInputVector			*m_pInputVec;

protected:
	kiOptionSet				m_optionSet;
	double					m_dAngle;//�Ƕ�
};

#endif