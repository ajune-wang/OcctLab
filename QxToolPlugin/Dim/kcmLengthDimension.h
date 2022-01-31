//////////////////////////////////////////////////////////////////////////
// ��ע������Ա�ע
#ifndef _KCM_LENGTH_DIMENSION_H_
#define _KCM_LENGTH_DIMENSION_H_

#include "kiCommand.h"

class kiInputEntityTool;
class kiInputLength;
class kcPreviewLine;

class kcmLengthDimension : public kiCommand
{
public:
	kcmLengthDimension(void);
	virtual ~kcmLengthDimension(void);

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

	virtual int				OnMouseMove(kuiMouseInput& mouseInput);

protected:
	BOOL					CalcPointAndDir();
	//
	BOOL					BuildDimension();

protected:
	kiInputEntityTool		*m_pPickFirstShape;
	kiInputEntityTool		*m_pPickSecondShape;
	kiInputLength			*m_pInputLength;
	kiOptionSet				m_optionSet;
	kiOptionEnum			*m_pTypeOpt;
	int						m_nType;
	double					m_dArrowSize;

protected:
	int						m_nState;
	Handle(Geom_Plane)		m_aPlane;
	kPoint3					m_aBasePoint;
	kVector3				m_aOffDir;
	kPoint3					m_aPnt1,m_aPnt2;
	kcPreviewLine			*m_pPreviewLine;
};

#endif