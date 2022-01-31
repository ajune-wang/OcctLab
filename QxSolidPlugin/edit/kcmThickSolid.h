//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_THICK_SOLID_H_
#define _KCM_THICK_SOLID_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputEntityTool;
class kiInputDoubleTool;

//�������
class kcmSheetSolid : public kiCommand
{
public:
	kcmSheetSolid(void);
	virtual ~kcmSheetSolid(void);

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
	BOOL					BuildSheetSolid();

protected:
	kiOptionSet			m_optionSet;
	bool					m_bOutSide;//���⻹������
	kiInputEntityTool		*m_pInputEntity;
	kiInputEntityTool		*m_pInputFace;
	kiInputDoubleTool		*m_pInputThickness;
	double				m_dThick;//ƫ��ֵ
};

//�Ⱥ�����
//�������
class kcmThickSolid : public kiCommand
{
public:
	kcmThickSolid(void);
	virtual ~kcmThickSolid(void);

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
	BOOL					BuildThickSolid();

protected:
	kiOptionSet			m_optionSet;
	kiInputEntityTool		*m_pInputEntity;
	kiInputDoubleTool		*m_pInputThickness;
	bool					m_bRevDir;//�Ƿ���
	double				m_dThick;//ƫ��ֵ
};

#endif