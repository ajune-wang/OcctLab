/////////////////////////////////////
// ʵ��Ĳü�����
#ifndef _KCM_SOLID_TRIM_H_
#define _KCM_SOLID_TRIM_H_

#include "kiCommand.h"
#include "kiOptionSet.h"
#include "kcLayer.h"

class kiInputEntityTool;
class kcDispSolidArrow;

class kcmSolidTrimByShell : public kiCommand
{
public:
	kcmSolidTrimByShell(void);
	virtual ~kcmSolidTrimByShell(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL				CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int				OnInputFinished(kiInputTool *pTool);
	// ��������ı�ʱ������øûص�����
	virtual  int				OnInputChanged(kiInputTool *pTool);

	virtual void				OnOptionChanged(kiOptionItem *pOptionItem);

protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL				CreateInputTools();
	virtual BOOL				DestroyInputTools();
	virtual BOOL				InitInputToolQueue();

protected:
	void						ShowDirArrow();
	BOOL						DoTrim();

protected:
	kiInputEntityTool			*m_pInputSolidTool;
	kiInputEntityTool			*m_pInputShellTool;

	kiOptionSet				_optionSet;
	bool						_bRevDir;
	bool						_bParallel;//�Ƿ�ʹ�ò���ִ��

	kcDispSolidArrow			*_pDirArrow;
};

//�ָ�ü�
class kcmSolidTrimSplit : public kiCommand
{
public:
	kcmSolidTrimSplit(void);
	virtual ~kcmSolidTrimSplit(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL				CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int				OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL				CreateInputTools();
	virtual BOOL				DestroyInputTools();
	virtual BOOL				InitInputToolQueue();

protected:
	BOOL						DoSplit();
	BOOL						GetInputEntList(kiInputEntityTool *pTool,kcEntityList &entList,
										  TopTools_ListOfShape& aShapeList,TopTools_ListOfShape& aCpyShapeList);

protected:
	kiInputEntityTool			*m_pInputObjTool1;
	kiInputEntityTool			*m_pInputObjTool2;

	kiOptionSet				_optionSet;
	bool						_bParallel;//�Ƿ�ʹ�ò���ִ��
};

#endif