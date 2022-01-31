//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_TRSF_ROTATE_H_
#define _KCM_TRSF_ROTATE_H_

#include "kiCommand.h"

class kiInputPoint;
class kiInputAngle;
class kiInputEntityTool;
class kcDispFrmShape;
class kcPreviewLine;
class kvCoordSystem;

class kcmTrsfRotate2d : public kiCommand
{
public:
	kcmTrsfRotate2d(void);
	virtual ~kcmTrsfRotate2d(void);

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
	BOOL					DoRotate();

protected:
	int						m_nState;
	kiOptionSet				m_optionSet;
	bool					m_bCopy;
	kiInputEntityTool		*m_pSelEntity;
	kiInputPoint			*m_pInputBasePnt;
	kiInputPoint			*m_pInputRefPnt;
	kiInputAngle			*m_pInputAngle;
	kPoint3					m_basePoint;
	kPoint3					m_refPoint;
	double					m_dAngle;
	kvCoordSystem			*m_pLocalCS;

protected:
	kcDispFrmShape			*m_pFrmShape;
	kcPreviewLine			*m_pPreviewLine;
};

#endif