//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_POINT_H_
#define _KCM_POINT_H_

#include "kiCommand.h"

class kiInputPoint;
class kiInputEntityTool;

class kcmPoint : public kiCommand
{
public:
	kcmPoint(void);
	virtual ~kcmPoint(void);

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
	// ���ٴ��������빤��.ÿ�����������һ��.
	virtual BOOL			DestroyInputTools();
	// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
	virtual BOOL			InitInputToolQueue();

protected:
	kPoint3					m_aPoint;
	kiInputPoint			*m_pInputPoint;
};

//////////////////////////////////////////////////////////////////////////
// ���߽���
class kcmCurveInterPoint : public kiCommand
{
public:
	kcmCurveInterPoint();
	virtual ~kcmCurveInterPoint();

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
	BOOL					CalcInterPoint();

protected:
	kiInputEntityTool		*m_pInputCurve1;
	kiInputEntityTool		*m_pInputCurve2;
};

#endif