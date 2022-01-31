//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_CURVE_BLEND_H_
#define _KCM_CURVE_BLEND_H_

#include "kiCommand.h"

class kiInputEntityTool;

class kcmCurveBlend : public kiCommand
{
public:
	kcmCurveBlend(void);
	virtual ~kcmCurveBlend(void);

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
	BOOL					BuildBlendCurve();

protected:
	kiInputEntityTool		*m_pPickFirstCurveTool;
	kiInputEntityTool		*m_pPickSecondCurveTool;
	kiOptionSet			m_optionSet1,m_optionSet2;
	kiOptionEnum			*m_pContEnum1;
	kiOptionEnum			*m_pContEnum2;
	int					m_nCont1,m_nCont2;
};

#endif