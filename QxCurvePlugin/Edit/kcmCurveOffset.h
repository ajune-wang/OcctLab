//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_CURVE_OFFSET_H_
#define _KCM_CURVE_OFFSET_H_

#include "kiCommand.h"

class kiInputEntityTool;
class kiInputDoubleTool;

class kcmCurveOffset : public kiCommand
{
public:
	kcmCurveOffset(void);
	~kcmCurveOffset(void);

	// ִ��һ�γ�ʼ�����ͷŵĵط���
	virtual  int			Destroy();

	//ִ�кͽ����ӿ�
	virtual	 int			Execute();
	virtual  int			End(int nCode);
	virtual  int			Apply();//ʵ��Ӧ��
	virtual  BOOL			CanFinish();//�����Ƿ�������

	//���빤�߽���ʱ
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	//
	bool					MakeOffset();

protected:
	kiInputEntityTool		*m_pInputEntityTool;
	kiInputDoubleTool		*m_pInputOffset;
	double					m_dOffset;
};

#endif