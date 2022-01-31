//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCG_CURVLIB_H_
#define _KCG_CURVLIB_H_


#include <ShapeExtend_WireData.hxx>
#include "QxLibDef.h"
#include "kPoint.h"
#include "kVector.h"

class TopoDS_Wire;

class QXLIB_API QxCurveLib
{
public:
	static Handle(Geom_Curve)		MakeSegment(const gp_Pnt &aP1,const gp_Pnt &aP2);
	static Handle(Geom_Curve)		MakeSegment(double p1[3],double p2[3]);

	static double				CurveLength(const Handle(Geom_Curve)& aCrv,double t0,double t1);

	//��������ֱ�߼��������뼰���Ӧ�㡣
	static bool					CalcLineLineExtrema(const kPoint3& org1,const kVector3& dir1,
											const kPoint3& org2,const kVector3& dir2,
											kPoint3& rp1,kPoint3& rp2,double& dist);

	// �㵽ֱ�ߵ�ͶӰ��tΪ���ز�����
	static bool					ProjectPointToLine(const kPoint3& org,const kVector3& dir,const kPoint3& point,
											kPoint3& prjpoint,double& t);
	// �㵽ֱ�ߵ�ͶӰ��tΪ���ز�����
	static bool					ProjectPointToLine(const kPoint3& org,const kVector3& dir,const kPoint3& point,
											kPoint3& prjpoint);

	// Z must be unit vector
	static Handle(Geom_Circle)	MakeCircle(const kPoint3& aCenter,const kPoint3& aXPoint,const kVector3& Z,double radius);

	//
	static bool					CalcTangent(const TopoDS_Edge& aE,const TopoDS_Face& aFace,double t,bool bTopo,kPoint3& p,kVector3& tang);
	static bool					CalcTangent(const TopoDS_Edge& aE,const TopoDS_Face& aFace,bool bTopo,kPoint3& p,kVector3& tang);

	// ��face�ı߽�edge�����µ�edge
	static TopoDS_Edge			BuildBoundEdge(const TopoDS_Edge& aEdge,const TopoDS_Face& aFace);

	static TopoDS_Wire			BuildBoundWire(const TopoDS_Wire& aWire,const TopoDS_Face& aFace);

	static BOOL					IsWireClosed(const TopoDS_Wire& aWire);
	static BOOL					IsEdgeClosed(const TopoDS_Edge& aEdge);

	// �Ƿ���ֱ��
	static bool					IsLine(const Handle(Geom_Curve) &aCurve);

	static Handle(Geom_Curve)	GetCurveCopy(const TopoDS_Edge& aEdge);

	//����edge�ķ�����
	static TopoDS_Edge			BuildEdgeDirFlag(const TopoDS_Edge& aEdge,const TopoDS_Face& aFace,double radius = -1.0);
};

//2D���ߵĸ�����
class QXLIB_API kcgCurve2DLib{
public:
	//
	static Handle(Geom2d_Curve)	MakeSegLine(const gp_Pnt2d &p1,const gp_Pnt2d &p2);
};

// ������������һ��wire
class QXLIB_API kcgSewCurve{
public:
	kcgSewCurve();
	~kcgSewCurve();

	bool						Init();

	// ���������ߣ���Ӱ��ԭ�����ߡ�
	bool						AddCurve(const Handle(Geom_Curve)& aCurve);
	// ����ӿ�������Ӱ��ԭ���Ĳ��֡�
	bool						AddEdge(const TopoDS_Edge& aEdge);
	bool						AddWire(const TopoDS_Wire& aWire);

	bool						Sew(bool bClosed);

	void						Clear();

	TopoDS_Wire					Wire() const;//���ؽ��

protected:
	bool						m_bDone;
	TopoDS_Wire					m_aWire;//���
	Handle(ShapeExtend_WireData)	m_wireData;
};

// �����������
class QXLIB_API kcgBlendCurve{
public:
	kcgBlendCurve();
	~kcgBlendCurve();

	// ��������edge���������t����������Լ����
	// �������ɵ�edge��
	TopoDS_Edge					Blend(const TopoDS_Edge& aFEdge,double t1,int nCout1,const TopoDS_Edge& aLEdge,double t2,int nCont2);
};



#endif