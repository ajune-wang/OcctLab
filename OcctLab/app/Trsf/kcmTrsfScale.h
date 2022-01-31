//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_TRSF_SCALE_H_
#define _KCM_TRSF_SCALE_H_

#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputPoint;
class kiInputEntityTool;
class kcDispFrmShape;
class kcPreviewLine;

class kcmTrsfScale : public kiCommand
{
public:
	kcmTrsfScale(void);
	virtual ~kcmTrsfScale(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

	// ��ѡ��ı��ĵ��ú���
	virtual void			OnOptionChanged(kiInputTool *pTool,kiOptionItem *pOptionItem); 

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
	BOOL					DoScale();

protected:
	int						m_nState;
	kiOptionSet				m_optionSet;
	kiOptionSet				m_optionSet2;
	bool					m_bCopy;
	kiInputEntityTool		*m_pSelEntity;
	kiInputPoint			*m_pInputBasePnt;
	kiInputPoint			*m_pInputRefPnt;
	kiInputPoint			*m_pInputScalePnt;
	kPoint3					m_basePoint;
	kPoint3					m_refPoint;
	kPoint3					m_aScalePoint;
	double					m_dScale;

protected:
	bool					m_bInputScale;

protected:
	kcDispFrmShape			*m_pFrmShape;
	kcPreviewLine			*m_pPreviewLine;
	kcPreviewLine			*m_pPreviewLine2;
};

#endif