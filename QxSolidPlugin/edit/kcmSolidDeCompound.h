//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLID_DECOMPOUND_H_
#define _KCM_SOLID_DECOMPOUND_H_

#include "kiCommand.h"

class kiInputEntityTool;

class kcmSolidDeCompound : public kiCommand
{
public:
	kcmSolidDeCompound(void);
	virtual ~kcmSolidDeCompound(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	BOOL					DeCompound();

protected:
	kiInputEntityTool		*m_pInputEntity;
};

#endif