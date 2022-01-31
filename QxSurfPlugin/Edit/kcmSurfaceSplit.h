//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SURFACE_SPLIT_H_
#define _KCM_SURFACE_SPLIT_H_

#include "kiCommand.h"

class kiInputEntityTool;

class kcmSurfaceSplit : public kiCommand
{
public:
	kcmSurfaceSplit(void);
	virtual ~kcmSurfaceSplit(void);

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
	BOOL					DoSplit();

protected:
	kiInputEntityTool		*m_pPickSurfaceTool;
	kiInputEntityTool		*m_pPickShapeTool;
};

#endif