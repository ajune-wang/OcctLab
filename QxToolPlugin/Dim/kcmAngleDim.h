//////////////////////////////////////////////////////////////////////////
// ��ע������Ա�ע
#ifndef _KCM_ANGLE_DIMENSION_H_
#define _KCM_ANGLE_DIMENSION_H_

#include "kiCommand.h"

class kiInputEntityTool;
class kiInputLength;
class kcPreviewLine;

class kcmAngleDim : public kiCommand
{
public:
	kcmAngleDim(int subCmd);
	virtual ~kcmAngleDim(void);

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
	bool					GetSelectedLine(bool bFirst);
	//
	bool					BuildDimension();

protected:
	kiInputEntityTool		*pPickLineTool_;
	kiInputEntityTool		*pPickLineTool2_;
	kiOptionSet				*pOptionSet_;
	double					dFlyout_;
	double					dArrowLen_;
	double					dFontHeight_;

protected:
	int						nSubCmd_; //������ 0��2 line 
	int						nState_;
	TopoDS_Edge				aEdge1_, aEdge2_;
};

#endif