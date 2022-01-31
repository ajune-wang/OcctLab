//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_DISP_COLOR_H_
#define _KCM_DISP_COLOR_H_

#include "kiCommand.h"

class kiInputEntityTool;
class kiSelSet;

class kcmDispColor : public kiCommand
{
public:
	kcmDispColor(void);
	virtual ~kcmDispColor(void);

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
	BOOL					SetColor(kiSelSet *pSelSet);

protected:
	kiInputEntityTool		*m_pInputEntityTool;
};

#endif