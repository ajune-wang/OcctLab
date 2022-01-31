/// ��������
#ifndef _KCM_SHAPE_PROPERTY_H_
#define _KCM_SHAPE_PROPERTY_H_

#include "kiCommand.h"

class kiInputEntityTool;

//һ��shape���������
//
class kcmShapeProperty : public kiCommand
{
public:
	kcmShapeProperty(void);
	virtual ~kcmShapeProperty(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int			OnExecute();
	virtual int			OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	virtual BOOL			DoShowProperty();

protected:
	kiInputEntityTool		*m_pInputEntityTool;
};

//һ���ߵ�����
class kcmEdgeProperty : public kiCommand
{
public:
	kcmEdgeProperty(void);
	virtual ~kcmEdgeProperty(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	virtual BOOL			DoShowProperty();

protected:
	kiInputEntityTool		*pInputEntityTool_;

	Handle(AIS_InteractiveObject)  aCtxObject_;
	TopoDS_Shape			aShape_;
};

//һ���������
class kcmFaceProperty : public kiCommand
{
public:
	kcmFaceProperty(void);
	virtual ~kcmFaceProperty(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int			OnExecute();

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	virtual BOOL			DoShowProperty();

protected:
	kiInputEntityTool		*pInputEntityTool_;

	Handle(AIS_InteractiveObject)  aCtxObject_;
	TopoDS_Face			aFace_;//
	bool					bLocalFace_;//�Ƿ�ֲ���
};

#endif