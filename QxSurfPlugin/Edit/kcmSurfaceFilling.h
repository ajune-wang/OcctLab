//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SURFACE_FILLING_H_
#define _KCM_SURFACE_FILLING_H_

#include <vector>
#include <TopTools_MapOfShape.hxx>
#include "kiCommand.h"

class kiInputEntityTool;
class kcPreviewCurve;

class axCrvInfo{
public:
	axCrvInfo(){
		_bBound = true;
		_nCont = 0;
		_pPrevCrv = NULL;
	}

public:
	TopoDS_Edge		_aEdge;
	int				_nCont;
	bool			_bBound;
	kcPreviewCurve *_pPrevCrv;
};

class kcmSurfaceFilling : public kiCommand
{
public:
	kcmSurfaceFilling(void);
	virtual ~kcmSurfaceFilling(void);

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

protected:
	//
	BOOL					DoFilling();

	//
	void					ClearEdgeInfo();

protected:
	kiOptionSet				m_optionSet;
	kiInputEntityTool		*m_pPickEdgeTool;
	int						m_nDegree;
	int						m_nMaxSeg;
	int						m_nbPtsOnCrv;
	int						m_nbIter;
	bool					m_bBound;//�Ƿ�߽�
	int						m_nContType;//����������
	kiOptionEnum			*m_pOptionCont;

protected:
	std::vector<axCrvInfo *>	m_aEdge;
	TopTools_MapOfShape			m_aShapeMap;
};

#endif