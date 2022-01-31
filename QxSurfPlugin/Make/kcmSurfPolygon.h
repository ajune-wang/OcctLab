//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SURF_POLYGON_H_
#define _KCM_SURF_POLYGON_H_

#include <vector>
#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputPoint;
class kcPreviewPolyline;
class kcPreviewLine;

class kcmSurfPolygon : public kiCommand
{
public:
	kcmSurfPolygon(void);
	virtual ~kcmSurfPolygon(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);
	virtual int				OnApply();

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

	virtual int				OnMouseMove(kuiMouseInput& mouseInput);

protected:
	BOOL					MakePolyline();

protected:
	std::vector<kPoint3>	m_pointArray;
	kiInputPoint			*m_pInputPoint;
	kcPreviewPolyline		*m_pPreviewPolyline;
	kcPreviewLine			*m_pPreviewLine;
	kcPreviewLine			*m_pPreviewLine2;
	kPoint3					m_aPoint;
};

#endif