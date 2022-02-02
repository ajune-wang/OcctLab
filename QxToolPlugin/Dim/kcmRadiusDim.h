//////////////////////////////////////////////////////////////////////////
// ��ע������Ա�ע
#ifndef _KCM_RADIUS_DIMENSION_H_
#define _KCM_RADIUS_DIMENSION_H_

#include "kiCommand.h"

class kiInputEntityTool;
class kiInputLength;
class kcPreviewLine;

class kcmRadiusDim : public kiCommand
{
public:
	kcmRadiusDim(void);
	virtual ~kcmRadiusDim(void);

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
	//
	BOOL					BuildDimension();

protected:
	kiInputEntityTool		*pPickCircleTool_;
	kiOptionSet				*pOptionSet_;
	double					dFlyout_;
	double					dArrowLen_;
	double					dFontHeight_;
};

#endif