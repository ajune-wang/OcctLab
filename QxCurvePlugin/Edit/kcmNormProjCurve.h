//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_NORM_PROJ_CURVE_H_
#define _KCM_NORM_PROJ_CURVE_H_

#include "kiCommand.h"

class kiInputEntityTool;

class kcmNormProjCurve : public kiCommand
{
public:
	kcmNormProjCurve(void);
	virtual ~kcmNormProjCurve(void);

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
	BOOL					DoNormProj();

protected:
	kiInputEntityTool		*m_pPickCurveTool;
	kiInputEntityTool		*m_pPickFaceTool;

};

#endif