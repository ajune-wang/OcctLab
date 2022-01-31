//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_RECT_H_
#define _KCM_RECT_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputPoint;
class kcPreviewRect;
class kiOptionSet;
class kcPreviewText;

class kcmRect : public kiCommand
{
public:
	kcmRect(void);
	virtual ~kcmRect(void);

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
	//�������߶���
	BOOL					MakeRectEntity();

protected:
	kiOptionSet				m_optionSet;

protected:
	kiInputPoint			*_pInputPoint1;
	kiInputPoint			*_pInputPoint2;
	kiInputPoint			*_pInputPoint3;
	kPoint3					_aPoint1,
							_aPoint2,
							_aPoint3;

protected:
	int						m_nState;//
	kcPreviewRect			*_pPreviewRect;
	kcPreviewText			*_pPreviewText;
	kcPreviewText			*_pPreviewText2;
};

#endif