//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_PRISM_SURFACE_H_
#define _KCM_PRISM_SURFACE_H_

#include "kiCommand.h"

class kiInputEntityTool;
class kiInputVector;

class kcmPrismSurface : public kiCommand
{
public:
	kcmPrismSurface(void);
	virtual ~kcmPrismSurface(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnExecute();

	//
protected:
	// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
	virtual	BOOL			CreateInputTools();
	// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
	virtual BOOL			InitInputToolQueue();

protected:
	//
	BOOL					MakePrismSurface();

protected:
	kiInputEntityTool		*m_pInputEntity;
	kiInputVector			*m_pInputVec;
};

#endif