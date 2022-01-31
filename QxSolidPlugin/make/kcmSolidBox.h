//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLIDBOX_H_
#define _KCM_SOLIDBOX_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputPoint;
class kiInputDoubleTool;
class kiOptionSet;
class kcPreviewRect;
class kcPreviewBox;

class kcmSolidBox : public kiCommand
{
public:
	kcmSolidBox(void);
	virtual ~kcmSolidBox(void);

	// ִ��һ�γ�ʼ�����ͷŵĵط���
	virtual  BOOL			CanFinish();//�����Ƿ�������

	//���빤�߽���ʱ
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	//����ĳ�ʼ�����ͷŴ���,ÿ����������������Եĳ�ʼ�������ٲ�����
	virtual int				OnInitialize();
	virtual int				OnDestroy();

	// ִ�к���
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);
	virtual int				OnApply();

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			InitInputToolQueue();
	virtual int				OnMouseMove(kuiMouseInput& mouseInput);
	virtual int				OnLButtonUp(kuiMouseInput& mouseInput);

protected:
	//
	bool					BuildBox();

protected:
	int						m_nState;
	
protected:
	kPoint3					m_aPnt1;
	kPoint3					m_aPnt2;
	double					m_dHeight;
	kiInputPoint			*m_pInputPoint1;
	kiInputPoint			*m_pInputPoint2;
	kiInputDoubleTool		*m_pInputHeight;

protected:
	kcPreviewRect			*m_pPreviewRect;
	kcPreviewBox			*m_pPreviewBox;
};

#endif