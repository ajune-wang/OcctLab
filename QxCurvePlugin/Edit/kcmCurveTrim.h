//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_CURVE_TRIM_H_
#define _KCM_CURVE_TRIM_H_

#include <AIS_ListOfInteractive.hxx>
#include "kiCommand.h"

class kiInputEntityTool;

// ���߲ü�
class kcmCurveTrim : public kiCommand
{
public:
	kcmCurveTrim(void);
	virtual ~kcmCurveTrim(void);

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

	virtual int				OnLButtonUp(kuiMouseInput& mouseInput);
	virtual int				OnMouseMove(kuiMouseInput& mouseInput);

protected:
	// ���зָ�
	BOOL					DoSplit();
	// �ü�
	BOOL					DoTrim(int x,int y);

protected:
	kiInputEntityTool		*m_pInputCurve;
	kiInputEntityTool		*m_pInputCurveToTrim;

	AIS_ListOfInteractive	m_listOfObj;
	int						m_nTrimCount;
	BOOL					m_bCanTrim;
};

//���ٲü�
//
class kcmCurveQuickTrim : public kiCommand
{
public:
	kcmCurveQuickTrim(void);
	virtual ~kcmCurveQuickTrim(void);

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

	virtual int				OnLButtonUp(kuiMouseInput& mouseInput);
	virtual int				OnMouseMove(kuiMouseInput& mouseInput);

protected:
	// ���зָ�
	BOOL					DoSplit();
	// �ü�
	BOOL					DoTrim(int x,int y);
	// ��ctx�������ʱ��ʾ����
	BOOL					RemoveTmpObjList(AIS_ListOfInteractive& objList);

protected:
	kiInputEntityTool		*m_pInputCurve1;
	kiInputEntityTool		*m_pInputCurve2;

	AIS_ListOfInteractive	m_aObjList1;
	AIS_ListOfInteractive	m_aObjList2;//�ֱ��¼�������ߵ���ʱ��ʾ����
	int						m_nTrimCount1;
	int						m_nTrimCount2;
	BOOL					m_bCanTrim;
};

// ���߲ü�
class kcmSplitAllEdges : public kiCommand
{
public:
	kcmSplitAllEdges(void);
	virtual ~kcmSplitAllEdges(void);

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

	virtual int				OnLButtonUp(kuiMouseInput& mouseInput);
	virtual int				OnMouseMove(kuiMouseInput& mouseInput);

protected:
	// ���зָ�
	BOOL					DoSplit();

protected:
	kiInputEntityTool		*m_pInputEdges;

	AIS_ListOfInteractive	m_listOfObj;
	int					m_nTrimCount;
	BOOL					m_bCanTrim;
};

#endif
