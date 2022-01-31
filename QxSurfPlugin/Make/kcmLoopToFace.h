//////////////////////////////////////////////////////////////////////////
// �ɱպ�����������������
//
#ifndef _KCM_LOOP_TO_FACE_H_
#define _KCM_LOOP_TO_FACE_H_

#include "kiCommand.h"

class kiInputEntityTool;

class kcmLoopToFace : public kiCommand
{
public:
	kcmLoopToFace(void);
	virtual ~kcmLoopToFace(void);

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
	//
	BOOL						BuildFaceFromLoop();
	BOOL						DoBuildFaceFromLoop();

protected:
	kiInputEntityTool			*m_pInputEntityTool;
};

#endif