//////////////////////////////////////////////////////////////////////////
// ������߹��ܡ�
#ifndef _KCM_SEWCURVE_H_
#define _KCM_SEWCURVE_H_

#include "kiCommand.h"

class kiInputEntityTool;

class kcmSewCurve : public kiCommand
{
public:
	kcmSewCurve(void);
	virtual ~kcmSewCurve(void);

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

protected:
	bool					SewCurve();
	bool					DoSewingCurve();

protected:
	kiOptionSet				m_optionSet;
	bool					m_bCopy;//�Ƿ񿽱�����
	kiInputEntityTool		*m_pInputEntity;
};

#endif