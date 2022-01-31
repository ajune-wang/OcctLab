//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLID_PRISM_H_
#define _KCM_SOLID_PRISM_H_

#include "kiCommand.h"

class kiInputEntityTool;
class kiInputLength;
class kiInputDoubleTool;
class kiOptionSet;

class kcmSolidPrism : public kiCommand
{
public:
	kcmSolidPrism(void);
	virtual ~kcmSolidPrism(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);


protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);

public://IOptionCallBack
	//ѡ��ı���
	virtual void				OnOptionChanged(kiOptionItem *pOptItem);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	//
	BOOL					MakePrismShape();

protected:
	kiOptionSet			*pFaceOptionSet_;
	kiInputEntityTool		*pFacePickTool_;
	kiOptionSet			*pMainOptionSet_;
	kiInputLength			*pInputLenTool_;

	double				dExtrudeLen_;
	int					nDirFlag_; 
};

#endif