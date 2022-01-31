//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_TRSF_MOVE_H_
#define _KCM_TRSF_MOVE_H_

#include "kVector.h"
#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputDirTool;
class kiInputDoubleTool;
class kiInputVector;
class kiInputEntityTool;
class kcDispFrmShape;

class kcmTrsfMove : public kiCommand
{
public:
	kcmTrsfMove(void);
	virtual ~kcmTrsfMove(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);

	virtual int				OnMouseMove(kuiMouseInput& mouseInput);

	//
protected:
	// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
	virtual	BOOL			CreateInputTools();
	// ���ٴ��������빤��.ÿ�����������һ��.
	virtual BOOL			DestroyInputTools();
	// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
	virtual BOOL			InitInputToolQueue();

protected:
	//
	BOOL					DoMove();

protected:
	int						m_nState;
	kVector3				m_aDir;
	double					m_dDist;
	kiOptionSet				m_optionSet;
	bool					m_bCopy;
	kiInputEntityTool		*m_pSelEntity;
	kiInputDirTool			*m_pInputDir;
	kiInputDoubleTool		*m_pInputDistTool;
	//kcDispFrmShape			*m_pFrmShape;
};

#endif