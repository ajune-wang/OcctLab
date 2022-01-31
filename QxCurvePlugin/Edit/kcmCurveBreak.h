//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_CURVE_BREAK_H_
#define _KCM_CURVE_BREAK_H_

#include "kiCommand.h"

class kiInputEntityTool;
class kiInputPointOnCurve;

//////////////////////////////////////////////////////////////////////////
// ����
class kcmCurveBreakByPoint : public kiCommand
{
public:
	kcmCurveBreakByPoint(void);
	virtual ~kcmCurveBreakByPoint(void);

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
	BOOL					DoBreak();

protected:
	kiInputEntityTool		*m_pInputCurve;
	kiInputPointOnCurve		*m_pPickPointOnCurve;
};

//////////////////////////////////////////////////////////////////////////
// ���ߴ��
class kcmCurveBreakByCurve : public kiCommand{
public:
	kcmCurveBreakByCurve();
	virtual ~kcmCurveBreakByCurve();

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
	BOOL					DoBreak();

protected:
	kiInputEntityTool		*m_pInputCurve;
	kiInputEntityTool		*m_pInputCurve2;
};

#endif