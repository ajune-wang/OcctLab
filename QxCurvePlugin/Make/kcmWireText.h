//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_WIRE_TEXT_H_
#define _KCM_WIRE_TEXT_H_

#include "kiCommand.h"

class kiInputPoint;

class kcmWireText : public kiCommand
{
public:
	kcmWireText(void);
	virtual ~kcmWireText(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnExecute();

	//
protected:
	// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
	virtual	BOOL			CreateInputTools();
	// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
	virtual BOOL			InitInputToolQueue();

protected:
	//
	BOOL					MakeWireText(const char *lpszText,LOGFONT& lf,double dHeight,double dGap,BOOL bWire);

protected:
	kPoint3					m_aPoint;
	kiInputPoint			*m_pInputPoint;

	TopTools_SequenceOfShape m_aSeqOfShape;
};

#endif