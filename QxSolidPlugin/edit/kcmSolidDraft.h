//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLID_DRAFT_H_
#define _KCM_SOLID_DRAFT_H_

#include "kiCommand.h"

class kiInputEntityTool;
class kiInputVector;
class kiInputDoubleTool;

class kcmSolidDraft : public kiCommand
{
public:
	kcmSolidDraft(void);
	virtual ~kcmSolidDraft(void);

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

protected:
	//
	BOOL					DoDraft();

protected:
	kiOptionSet				m_optionSet;
	kiInputEntityTool		*m_pPickEntityTool;
	kiInputVector			*m_pInputVectorTool;
	kiInputDoubleTool		*m_pInputDoubleTool;
	kiInputEntityTool		*m_pPickStopEntityTool;
	double					m_dAngle;

	kiOptionEnum			*m_pDraftMode;
	int						m_nDraftMode;
	kiOptionEnum			*m_pTransitionMode;
	int						m_nTransitionMode;
	bool					m_bInternal;

protected:
	bool					m_bReady;
};

#endif