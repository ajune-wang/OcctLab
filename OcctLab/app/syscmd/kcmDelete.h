//////////////////////////////////////////////////////////////////////////
// ɾ����������
//
#ifndef _KCM_DELETE_H_
#define _KCM_DELETE_H_

#include "kiCommand.h"

class kiSelSet;
class kiInputEntityTool;

class kcmDelete : public kiCommand
{
public:
	kcmDelete(void);
	virtual ~kcmDelete(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnDestroy();
	// ִ�к���
	virtual int				OnExecute();

	// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
	virtual	BOOL			CreateInputTools();
	// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
	virtual BOOL			InitInputToolQueue();

protected:
	//ɾ��
	BOOL					DeleteEntity(kiSelSet *pSelSet);

protected:
	kiInputEntityTool		*m_pInputEntity;
};

#endif