//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLID_OFFSET_H_
#define _KCM_SOLID_OFFSET_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputEntityTool;
class kiInputDoubleTool;

class kcmSolidOffset : public kiCommand
{
public:
	kcmSolidOffset(void);
	virtual ~kcmSolidOffset(void);

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
	BOOL					BuildOffsetShape();

protected:
	kiOptionSet				m_optionSet;
	bool						m_bOutSide;//���⻹������
	kiInputEntityTool			*m_pInputEntity;
	kiInputDoubleTool			*m_pInputOffset;
	double					m_dOffset;//ƫ��ֵ
};

#endif