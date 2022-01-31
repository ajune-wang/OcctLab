//////////////////////////////////////////////////////////////////////////
// Ԥ��������������ʱ���ơ����ᱣ�棬����ʰȡ����Ҫ���ڽ�������ʱ����Ļ��ơ�
// 
#ifndef _KC_PREVIEW_OBJ_H_
#define _KC_PREVIEW_OBJ_H_

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <vector>
#include "kPoint.h"

#include "QxLibDef.h"

class kvCoordSystem;
class kcBasePlane;
class kcDocContext;

class QXLIB_API kcPreviewObj
{
public:
	kcPreviewObj(const Handle(AIS_InteractiveContext)& aCtx,kcDocContext *pDocCtx);
	virtual ~kcPreviewObj(void);

	//��ʾ������
	virtual BOOL						Display(BOOL bShow);
	virtual BOOL						IsDisplayed() const;

	//���
	virtual void						Clear();
	virtual void						Destroy();

public:
	//������ʾ��ɫ
	virtual void						SetColor(double r,double g,double b);

protected:
	// ���ݲ���������ʾ����,����Ҫʹ�ö���Groupʱ���������ظú���
	virtual bool						DoBuildDisplay();
	// ����Ĵ�����ʾ����,������һ��Group��
	virtual bool						BuildGroupDisplay(const Handle(Graphic3d_Group)& hGroup) = 0;

	// ������ʾ����,�����ݷ����ı�ʱ�����Ե��øú������ؽ���ʾ����
	virtual void						UpdateDisplay();
	
protected:
	Handle(AIS_InteractiveContext)		aAISCtx_;
	kcDocContext						*pDocContext_;//
	
	Handle(Prs3d_Presentation)			_hStru;
	double							_dColR,_dColG,_dColB;//��ɫ
	bool								_bBuilded;//�Ƿ񹹽�����ʾ����
};

// ��ʱ�����ʾ
class QXLIB_API kcPreviewPoint : public kcPreviewObj{
public:
	kcPreviewPoint(const Handle_AIS_InteractiveContext& aCtx,kcDocContext *pDocCtx);
	virtual ~kcPreviewPoint();

	//����
	void								Update(gp_Pnt& pnt);

	/* enum Aspect_TypeOfMarker {
	Aspect_TOM_POINT,
	Aspect_TOM_PLUS,
	Aspect_TOM_STAR,
	Aspect_TOM_O,
	Aspect_TOM_X,
	Aspect_TOM_O_POINT,
	Aspect_TOM_O_PLUS,
	Aspect_TOM_O_STAR,
	Aspect_TOM_O_X,
	Aspect_TOM_BALL,
	Aspect_TOM_RING1,
	Aspect_TOM_RING2,
	Aspect_TOM_RING3,
	Aspect_TOM_USERDEFINED
	};*/
	void								SetType(int type);	
	void								SetScale(double scale);

	Handle(Graphic3d_AspectMarker3d)&	GetMasker3d() { return m_aMaskerAttr;}

protected:
	// ����Ĵ�����ʾ����
	virtual bool						BuildGroupDisplay(const Handle(Graphic3d_Group)& hGroup);

protected:
	gp_Pnt							m_pnt;
	int								m_eType;
	double							m_dScale;

	Handle(Graphic3d_AspectMarker3d)	m_aMaskerAttr;
};

//////////////////////////////////////////////////////////////////////////
// ���࣬ʵ��һЩ������ػ�������
class QXLIB_API kcPreviewCurveObj : public kcPreviewObj{
public:
	kcPreviewCurveObj(const Handle_AIS_InteractiveContext& aCtx,kcDocContext *pDocCtx);
	virtual ~kcPreviewCurveObj();

	/*
	enum Aspect_TypeOfLine {
	Aspect_TOL_SOLID,
	Aspect_TOL_DASH,
	Aspect_TOL_DOT,
	Aspect_TOL_DOTDASH,
	Aspect_TOL_USERDEFINED
	};
	*/
	void								SetType(int type);
	void								SetWidth(double width);
	bool								HasInited() const {  return _bInited; }

	Handle(Graphic3d_AspectLine3d)&		GetLine3d() { return _aLineAttr;}

	//���
	virtual void						Clear();
	virtual void						Destroy();

protected:
	virtual  void						SetPrimAspect();
	void								SetInited(bool bInited) { _bInited = bInited;}

protected:
	int								_eType;//����
	double							_dWidth;//�߿�

	Handle(Graphic3d_AspectLine3d)		_aLineAttr;

private:
	bool								_bInited;//�Ƿ��ʼ��
};

class QXLIB_API kcPreviewLine : public kcPreviewCurveObj{
public:
	kcPreviewLine(const Handle_AIS_InteractiveContext& aCtx,kcDocContext *pDocCtx);
	virtual ~kcPreviewLine();
	
	//����ֱ�ߵ�����
	void								Update(gp_Pnt& pnt1,gp_Pnt& pnt2);
	void								Update(const kPoint3& p1,const kPoint3& p2);

protected:
	// ����Ĵ�����ʾ����
	virtual bool						BuildGroupDisplay(const Handle(Graphic3d_Group)& hGroup);

protected:
	gp_Pnt							m_pnt1,m_pnt2;
};

typedef std::vector<gp_Pnt> stdPntVector;

//һϵ���߶�
class QXLIB_API kcPreviewSegments : public kcPreviewCurveObj{
public:
	kcPreviewSegments(const Handle_AIS_InteractiveContext& aCtx,kcDocContext *pDocCtx);
	~kcPreviewSegments();

	void								AddSegment(const gp_Pnt &p1,const gp_Pnt &p2);
	virtual void						Clear();

protected:
	// ����Ĵ�����ʾ����
	virtual bool						BuildGroupDisplay(const Handle(Graphic3d_Group)& hGroup);

protected:
	stdPntVector						aPntVec_;//��¼һϵ�е�
};

// ������
class QXLIB_API kcPreviewTriangle : public kcPreviewCurveObj{
public:
	kcPreviewTriangle(const Handle_AIS_InteractiveContext& aCtx,kcDocContext *pDocCtx);
	virtual ~kcPreviewTriangle();

	//�趨������
	void								Set(const gp_Pnt& p1,const gp_Pnt& p2,const gp_Pnt& p3);

protected:
	// ����Ĵ�����ʾ����
	virtual bool						BuildGroupDisplay(const Handle(Graphic3d_Group)& hGroup);

protected:
	gp_Pnt							m_pnt1,m_pnt2,m_pnt3;

};

// ����
class QXLIB_API kcPreviewRect : public kcPreviewCurveObj{
public:
	kcPreviewRect(const Handle_AIS_InteractiveContext& aCtx,kcDocContext *pDocCtx);
	virtual ~kcPreviewRect();

	//
	void								Init(gp_Pnt& pnt1,kcBasePlane *pWP);
	void								Update(gp_Pnt& pnt2);

protected:
	// ����Ĵ�����ʾ����
	virtual bool						BuildGroupDisplay(const Handle(Graphic3d_Group)& hGroup);

protected:
	kcBasePlane						*m_pWorkPlane;//����ƽ��
	gp_Pnt							m_pnt1;
	gp_Pnt							m_aPnt[4];//�ĸ�����

};

// box���Ԥ��
class QXLIB_API kcPreviewBox : public kcPreviewCurveObj{
public:
	kcPreviewBox(const Handle_AIS_InteractiveContext& aCtx,kcDocContext *pDocCtx);
	virtual ~kcPreviewBox();

	//
	bool								Init(gp_Pnt& pnt1,gp_Pnt& pnt2,kcBasePlane *pWP);
	void								Update(gp_Pnt& pnt3);

protected:
	// ����Ĵ�����ʾ����
	virtual bool						BuildGroupDisplay(const Handle(Graphic3d_Group)& hGroup);

protected:
	kcBasePlane						*m_pWorkPlane;//����ƽ��
	gp_Pnt							m_pnt1,m_pnt2;
	double							m_aPnt[8][3];//��
	int								m_aEdge[12][2];//��
};

// Բ����ʱ��ʾ
class QXLIB_API kcPreviewCircle : public kcPreviewCurveObj{
public:
	kcPreviewCircle(const Handle_AIS_InteractiveContext& aCtx,kcDocContext *pDocCtx);
	virtual ~kcPreviewCircle();

	//��ʼ��
	BOOL								Init(const Handle(Geom_Circle)& hCircle);
	void								Update(double radius);

	//���
	virtual void						Clear();
	virtual void						Destroy();

protected:
	// ����Ĵ�����ʾ����
	virtual bool						BuildGroupDisplay(const Handle(Graphic3d_Group)& hGroup);

protected:
	Handle(Geom_Circle)				m_hCircle;
};

// Բ������ʱ��ʾ
class QXLIB_API kcPreviewArc : public kcPreviewCurveObj{
public:
	kcPreviewArc(const Handle_AIS_InteractiveContext& aCtx,kcDocContext *pDocCtx);
	virtual ~kcPreviewArc();

	//��ʼ��
	BOOL								Init(const Handle(Geom_Circle)& aCircle);
	void								Update(double ang1,double ang2);

	//���
	virtual void						Clear();
	virtual void						Destroy();

protected:
	// ����Ĵ�����ʾ����
	virtual bool						BuildGroupDisplay(const Handle(Graphic3d_Group)& hGroup);

protected:
	Handle(Geom_Circle)				m_aCircle;
	Handle(Geom_TrimmedCurve)			m_aArc;
};

// ���ߵ���ʱ��ʾ
class QXLIB_API kcPreviewCurve : public kcPreviewCurveObj{
public:
	kcPreviewCurve(const Handle_AIS_InteractiveContext& aCtx,kcDocContext *pDocCtx);
	virtual ~kcPreviewCurve();

	void								Update(const Handle(Geom_Curve)& aCurve);
	void								Update(const TopoDS_Edge& aEdge);

	//���
	virtual void						Clear();
	virtual void						Destroy();

protected:
	// ����Ĵ�����ʾ����
	virtual bool						BuildGroupDisplay(const Handle(Graphic3d_Group)& hGroup);

protected:
	Handle(Geom_Curve)				m_aCurve;

};

// �����ߵ���ʱ��ʾ
class QXLIB_API kcPreviewPolyline : public kcPreviewCurveObj{
public:
	kcPreviewPolyline(const Handle_AIS_InteractiveContext& aCtx,kcDocContext *pDocCtx);
	virtual ~kcPreviewPolyline();

	void								AddPoint(const kPoint3& p);

	//���
	virtual void						Clear();
	virtual void						Destroy();

protected:
	// ����Ĵ�����ʾ����
	virtual bool						BuildGroupDisplay(const Handle(Graphic3d_Group)& hGroup);

protected:
	std::vector<kPoint3>				m_pointArray;

};

//��ʾ���Ƶ�����
class QXLIB_API kcPreviewNurbsPoles : public kcPreviewObj{
public:
	kcPreviewNurbsPoles(const Handle_AIS_InteractiveContext& aCtx,kcDocContext *pDocCtx);
	~kcPreviewNurbsPoles();

	bool					Initialize(const Handle(Geom_Curve) &aSplCrv);
	bool					Initialize(const Handle(Geom_Surface) &aSplSurf);

	void					SetLineColor(double r,double g,double b);

	//
	void					ShowUIsoLine(bool bShow);//�Ƿ���ʾU�򣨵�V����������
	void					ShowVIsoLine(bool bShow);//�Ƿ���ʾV�򣨵�U����������

protected:
	// ���ݲ���������ʾ����,����Ҫʹ�ö���Groupʱ���������ظú���
	virtual bool						DoBuildDisplay();
	// ����Ĵ�����ʾ����,������һ��Group��
	virtual bool						BuildGroupDisplay(const Handle(Graphic3d_Group)& hGroup);

protected:
	Handle(Geom_BSplineCurve)		aSplCurve_;//ҪԤ������������
	Handle(Geom_BSplineSurface)	aSplSurf_;//ҪԤ������������

	double				dLineColorR_,dLineColorG_,dLineColorB_;
	bool					bShowUIsoLine_,bShowVIsoLine_;
};

// ��ʾ������
class QXLIB_API kcPreviewMesh : public kcPreviewObj{
public:
	kcPreviewMesh(const Handle_AIS_InteractiveContext& aCtx,kcDocContext *pDocCtx);
	~kcPreviewMesh();

	bool					Initialize(const TopoDS_Face &aFace);
	bool					Initialize(const TopoDS_Shape &aS);

protected:
	// ���ݲ���������ʾ����,����Ҫʹ�ö���Groupʱ���������ظú���
	virtual bool			DoBuildDisplay();

	// ����Ĵ�����ʾ����
	virtual bool			BuildGroupDisplay(const Handle(Graphic3d_Group)& hGroup);

protected:
	void					DoBuildFaceDisplay(const TopoDS_Face &aF);

protected:
	TopoDS_Face			aFace_;
	TopoDS_Shape			aShape_;

};

#endif