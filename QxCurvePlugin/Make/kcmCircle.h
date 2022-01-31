//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_CIRCLE_H_
#define _KCM_CIRCLE_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputPoint;
class kiInputLength;
class kcPreviewCircle;
class kcPreviewLine;
class kiOptionSet;
class kcPreviewText;

class kcmCircle : public kiCommand
{
public:
	kcmCircle(void);
	virtual ~kcmCircle(void);

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
	virtual int				OnLButtonUp(kuiMouseInput& mouseInput);

protected:
	//
	bool					MakeCircleEntity();

protected:
	int						m_nState;//״̬
	kiInputPoint			*m_pInputCenter;
	kiInputLength			*m_pInputRadius;
	kPoint3					m_aCenter;
	double					m_aCircPnt[3];
	double					m_dRadius;
	bool					m_bPreviewInit;//Ԥ�������Ƿ��ʼ��.
	kcPreviewCircle			*m_pPreviewCircle;
};

#endif