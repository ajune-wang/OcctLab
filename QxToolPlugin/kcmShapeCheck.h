//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SHAPE_CHECK_H_
#define _KCM_SHAPE_CHECK_H_

#include "kiCommand.h"

class kiInputEntityTool;

class kcmShapeCheck : public kiCommand
{
public:
	kcmShapeCheck(void);
	virtual ~kcmShapeCheck(void);

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
	virtual BOOL			DoCheck() = 0;

protected:
	kiInputEntityTool		*m_pInputEntityTool;
};

// ���������Ч��
class kcmShapeCheckValid : public kcmShapeCheck{
public:
	kcmShapeCheckValid();
	virtual ~kcmShapeCheckValid();

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();

protected:
	virtual BOOL			DoCheck();
};

// �������������Ч��
class kcmShapeCheckTopoValid : public kcmShapeCheck{
public:
	kcmShapeCheckTopoValid();
	virtual ~kcmShapeCheckTopoValid();

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();

protected:
	virtual BOOL			DoCheck();
};

// ��黷��ǵıպ���
class kcmShapeCheckClose : public kcmShapeCheck{
public:
	kcmShapeCheckClose();
	virtual ~kcmShapeCheckClose();

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();

protected:
	virtual BOOL			DoCheck();
};

// ��黷���Խ�
class kcmShapeCheckSelfIntersect : public kcmShapeCheck{
public:
	kcmShapeCheckSelfIntersect();
	virtual ~kcmShapeCheckSelfIntersect();

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();

protected:
	virtual BOOL			DoCheck();
};

#endif