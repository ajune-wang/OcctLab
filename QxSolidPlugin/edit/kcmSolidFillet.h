//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLID_FILLET_H_
#define _KCM_SOLID_FILLET_H_

#include "kiCommand.h"

class kiInputEntityTool;
class kiInputDoubleTool;

class kcmSolidFillet : public kiCommand
{
public:
	kcmSolidFillet(void);
	virtual ~kcmSolidFillet(void);

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
	BOOL					DoFillet();

protected:
	kiInputEntityTool			*_pInputEntity;
	kiInputEntityTool			*_pInputEdge;
	kiInputDoubleTool			*_pInputRadius;
	double						_dRadius;
};

// ȫ����Բ��
class kcmSolidFilletAllEdge : public kiCommand{
public:
	kcmSolidFilletAllEdge();
	virtual ~kcmSolidFilletAllEdge();

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
	BOOL					DoFillet();

protected:
	kiInputEntityTool			*_pInputEntity;
	kiInputDoubleTool			*_pInputRadius;
	double						_dRadius;
};

#endif