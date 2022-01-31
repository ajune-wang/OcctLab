//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_UCSMGR_H_
#define _KCM_UCSMGR_H_

#include "kiCommand.h"

class kiInputPoint;
class kcPreviewLine;

// ���㴴������ϵ
class kcmCreateUCSByThreePoint : public kiCommand{
public:
	kcmCreateUCSByThreePoint();
	virtual ~kcmCreateUCSByThreePoint();

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
	BOOL					DoCreateUCS();

protected:
	int						m_nState;
	kiInputPoint			*m_pInputPointOrg;
	kiInputPoint			*m_pInputPointX;
	kiInputPoint			*m_pInputPointY;
	kPoint3					m_pointOrg;
	kPoint3					m_pointX;
	kPoint3					m_pointY;

	kcPreviewLine			*m_pPreviewXLine;
	kcPreviewLine			*m_pPreviewYLine;
};

// ƽ�Ƶ�ǰ����ϵ
class kcmCreateUCSByMove : public kiCommand{
public:
	kcmCreateUCSByMove();
	virtual	~kcmCreateUCSByMove();
};

#endif