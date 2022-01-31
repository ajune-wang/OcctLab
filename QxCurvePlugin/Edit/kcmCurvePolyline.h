//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_CURVE_POLYLINE_H_
#define _KCM_CURVE_POLYLINE_H_

#include <vector>
#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputPoint;
class kcPreviewPolyline;
class kcPreviewLine;

class kcmCurvePolyline : public kiCommand
{
public:
	kcmCurvePolyline(void);
	virtual ~kcmCurvePolyline(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);
	virtual int				OnApply();

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

	virtual int				OnMouseMove(kuiMouseInput& mouseInput);

protected:
	BOOL					MakePolyline();

protected:
	std::vector<kPoint3>	m_pointArray;
	kiOptionSet				m_optionSet;
	kiInputPoint			*m_pInputPoint;
	kcPreviewPolyline		*m_pPreviewPolyline;
	kcPreviewLine			*m_pPreviewLine;
	kPoint3					m_aPoint;
	bool					m_bClosed;
};

#endif
