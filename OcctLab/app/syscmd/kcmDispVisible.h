//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_DISP_VISIBLE_H_
#define _KCM_DISP_VISIBLE_H_

#include "kiCommand.h"

class kiInputEntityTool;

class kcmDispHideSelect : public kiCommand
{
public:
	kcmDispHideSelect(void);
	virtual ~kcmDispHideSelect(void);

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
	BOOL					HideSelSet(kiSelSet *pSelSet);

protected:
	kiInputEntityTool		*m_pInputEntity;
};

class kcmDispShowAll : public kiCommand
{
public:
	kcmDispShowAll();
	virtual ~kcmDispShowAll();

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

protected:
	virtual int				OnExecute();

};

// ����������ʾ
class kcmHideGridDisplay : public kiCommand
{
public:
	kcmHideGridDisplay();
	virtual ~kcmHideGridDisplay();

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

protected:
	virtual int				OnExecute();

};

// ������ʾ
class kcmDispTypeFilter : public kiCommand
{
public:
	kcmDispTypeFilter();
	virtual ~kcmDispTypeFilter();

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

protected:
	virtual int				OnExecute();
};

#endif