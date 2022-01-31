//////////////////////////////////////////////////////////////////////////
// ��ȡ�����ϵ����ߣ��������߽����ߡ��������ߵȵȡ�
#ifndef _KCM_FACE_CURVE_H_
#define _KCM_FACE_CURVE_H_

#include <string>
#include <vector>
#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputEntityTool;
class kcEntity;
class kcPreviewObj;
class kcPreviewCurve;

//����ı߽�
class kiFaceBoundCurve : public kiCommand{
public:
	kiFaceBoundCurve();
	virtual ~kiFaceBoundCurve();

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
	//
	virtual BOOL			DoBuildBoundCurve() = 0;

protected:
	kiInputEntityTool		*m_pPickEntityTool;
	kiInputEntityTool		*m_pPickLocalShapeTool;

protected:
	std::string				m_strPrompt;
	int						m_nSelModel;
};

//�߽�����
class kcmFaceBoundEdge : public kiFaceBoundCurve{
public:
	kcmFaceBoundEdge();

protected:
	//
	virtual BOOL			DoBuildBoundCurve();
};

//�߽����߷���
class kcmFaceBoundEdgeDire : public kiFaceBoundCurve{
public:
	kcmFaceBoundEdgeDire();
	virtual ~kcmFaceBoundEdgeDire();

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnEnd(int nCode);
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();

protected:
	//
	virtual BOOL			DoBuildBoundCurve();
	bool					DoShowWireDire();
	bool					DoGenEdgeDireDisp(const TopoDS_Edge& aEdge,const TopoDS_Face& aFace);

	void					ClearTempDispObj();

protected:
	bool					m_bTopolDire;//�Ƿ����˲���ķ���
	kiOptionSet				m_optionSet;

	std::vector<kcPreviewObj *>	m_prevObjArray;
};

class kcmFaceBoundWire : public kiFaceBoundCurve{
public:
	kcmFaceBoundWire();

protected:
	//
	virtual BOOL			DoBuildBoundCurve();

	virtual BOOL			DoBuildEntity(const TopoDS_Wire& aWire,const TopoDS_Face& aFace,
										  std::vector<kcEntity *>& aEnt);
};

// ��ȡFace��wire��ԭʼ˳������
class kcmFaceRawWire : public kcmFaceBoundWire{
public:
	kcmFaceRawWire();

protected:
	//
	virtual BOOL			DoBuildEntity(const TopoDS_Wire& aWire,const TopoDS_Face& aFace,
										  std::vector<kcEntity *>& aEnt);
};

//��������
class kcmFaceIsoCurve : public kiCommand{
public:
	kcmFaceIsoCurve();

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

public:
	virtual int			OnLButtonUp(kuiMouseInput& mouseInput);
	virtual int			OnMouseMove(kuiMouseInput& mouseInput);

protected:
	virtual int			OnExecute();
	virtual int			OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:

	bool					DoGetSelFace();
	bool					DoBuildIsoCurve();
	Handle(Geom_Curve)	DoGetIsoCurve(double uv);

protected:
	kiInputEntityTool		*pPickEntityTool_;
	kiOptionSet			aOptionSet_;
	kcPreviewCurve		*pPrvwCurve_;

	TopoDS_Face			aSelFace_;
	int					nIsoDir_; //0 U�� 1: V��
	double				dParam_;//����ֵ
};

#endif