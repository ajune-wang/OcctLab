//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SHAPE_FIX_H_
#define _KCM_SHAPE_FIX_H_

#include "kiCommand.h"

class kiInputEntityTool;

class kcmShapeFixBase : public kiCommand
{
public:
	kcmShapeFixBase(void);
	virtual ~kcmShapeFixBase(void);

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

protected:
	virtual BOOL			DoFix();
	virtual TopoDS_Shape	DoFixShape(const TopoDS_Shape& aS) = 0;

protected:
	kiInputEntityTool		*m_pInputEntityTool;

protected:
	int						m_nEntType;
};

class kcmShapeFix : public kcmShapeFixBase
{
public:
	kcmShapeFix(void);
	virtual ~kcmShapeFix(void);

protected:
	virtual TopoDS_Shape	DoFixShape(const TopoDS_Shape& aS);

};

#endif