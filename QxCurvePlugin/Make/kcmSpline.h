///////////////////////////////////////////////////////////////////////////
// 
#ifndef _KCM_SPLINE_H_
#define _KCM_SPLINE_H_

#include <vector>
#include "kPoint.h"

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputEntityTool;
class kiInputPoint;
class kiOptionSet;
class kcPreviewCurve;

class kcmSpline : public kiCommand
{
public:
	kcmSpline(void);
	virtual ~kcmSpline(void);

	virtual  BOOL				CanFinish();//�����Ƿ�������

	//���빤�߽���ʱ
	virtual  int				OnInputFinished(kiInputTool *pTool);

protected:
	// ִ�к���
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);
	virtual int				OnApply();

protected:
	// ������Ҫ�����빤��
	virtual	BOOL				CreateInputTools();
	virtual BOOL				DestroyInputTools();
	virtual BOOL				InitInputToolQueue();
	virtual int				OnMouseMove(kuiMouseInput& mouseInput);
	virtual int				OnLButtonUp(kuiMouseInput& mouseInput);

protected:
	kiOptionSet				m_optionSet;
	bool						m_bPeriodic;//�Ƿ����ڱպ�
	kiInputPoint				*m_pInputPoint;
	std::vector<kPoint3>		m_aPoint;//�������
	kcPreviewCurve			*m_pPreviewCurve;
	kPoint3					m_dPoint;
};

class kcmSplineUnperiodize : public kiCommand{
public:
	kcmSplineUnperiodize();
	virtual ~kcmSplineUnperiodize();

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
	BOOL					DoUnperiodize();

protected:
	kiInputEntityTool		*m_pInputCurve;
};

#endif