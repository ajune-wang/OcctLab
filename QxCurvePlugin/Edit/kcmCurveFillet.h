//////////////////////////////////////////////////////////////////////////
// ���ߵ���
#ifndef _KCM_CURVE_FILLET_H_
#define _KCM_CURVE_FILLET_H_

#include <vector>
#include <TopTools_MapOfShape.hxx>
#include "kiCommand.h"

class kiOptionSet;
class kiInputEntityTool;
class kiInputDoubleTool;

class axVertexData{
public:
	axVertexData(){
		_dRadius = 0.0;
	}

public:
	TopoDS_Vertex			_aVertex;
	double					_dRadius;
};

class kcmCurveFillet : public kiCommand
{
public:
	kcmCurveFillet(void);
	virtual ~kcmCurveFillet(void);

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
	void					ClearVertexData();
	//
	BOOL					DoFillet();

protected:
	kiInputEntityTool		*m_pPickEntityTool;
	kiInputEntityTool		*m_pPickLocalShapeTool;
	kiInputDoubleTool		*m_pInputRadiusTool;
	double					m_dRadius;

protected:
	std::vector<axVertexData *>	m_aVertexData;
	TopTools_MapOfShape			m_aVertexMap;
};

class kcmTwoEdgeFillet : public kiCommand
{
public:
	kcmTwoEdgeFillet(void);
	virtual ~kcmTwoEdgeFillet(void);

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
	void					ClearVertexData();
	//
	BOOL					DoFillet();

protected:
	kiOptionSet			*pOptionSet_;
	kiInputEntityTool		*pPickEdgeTool_;
	double				dRadius_;
	bool					bToWire_;//����Ƿ��黷
	bool					bCopy_; //�Ƿ񿽱�
};


#endif