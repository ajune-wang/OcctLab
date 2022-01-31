//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_ARC_H_
#define _KCM_ARC_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputPoint;
class kcPreviewCircle;
class kcPreviewLine;
class kiOptionSet;
class kcPreviewText;
class kcPreviewArc;
class kiInputAngle;
class kiInputLength;
class kvCoordSystem;

class kcmArc : public kiCommand
{
public:
	kcmArc(void);
	virtual ~kcmArc(void);

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
	BOOL					BuildArcEntity();

protected:
	int						m_nState;//״̬
	kiInputPoint			*m_pInputCenter;
	kiInputLength			*m_pInputRadius;
	kiInputPoint			*m_pInputStartPnt;
	kiInputAngle			*m_pInputAngle;
	kPoint3					m_aCenter;
	kPoint3					m_aCircPnt;
	kPoint3					m_aStartPnt,m_aEndPnt;
	double					m_dRadius;
	double					m_dAngle; //�Ƕ�ֵ,�ǻ���ֵ
	kvCoordSystem			*m_pLocalCS;
	bool					m_bPreviewInit;//Ԥ�������Ƿ��ʼ��.
	kcPreviewLine			*m_pPreviewLine;
	kcPreviewCircle			*m_pPreviewCircle;
	kcPreviewText			*m_pPreviewText;
	kcPreviewArc			*m_pPreviewArc;
	gp_Ax2					m_arcAxis;
};

#endif