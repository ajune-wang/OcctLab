//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCG_INTERSECT_H_
#define _KCG_INTERSECT_H_

#include <vector>
#include "QxLibDef.h"
#include "kPoint.h"

class QXLIB_API kcxCurveIntpt{
public:
	kcxCurveIntpt();
	kcxCurveIntpt(const kcxCurveIntpt& other);

	kcxCurveIntpt&				operator=(const kcxCurveIntpt& other);

public:
	int							_widx;//��wire���±�,-1��ʾedge.
	double						_t;//����
	kPoint3						_p;//����
	double						_dist;//���ǵĵľ���
};

class QXLIB_API kcxCCIntpt{
public:
	kcxCCIntpt();
	kcxCCIntpt(const kcxCCIntpt& other);

	kcxCCIntpt&					operator=(const kcxCCIntpt& other);

public:
	kcxCurveIntpt				_ipt1,_ipt2;
};

class QXLIB_API kcxCSIntpt{
public:
	kcxCSIntpt();
	kcxCSIntpt(const kcxCSIntpt& other);

	kcxCSIntpt&					operator=(const kcxCSIntpt& other);

public:
	kcxCurveIntpt				_cvipt;
	double						_u,_v;//���������Ӧ�Ĳ���
	kPoint3						_p;
};

// ���edge��wire�Ľ���
class QXLIB_API kcgPointCurveInter{
public:
	kcgPointCurveInter();

	bool						Intersect(const kPoint3& p,const TopoDS_Edge& aEdge,double tol = 1e-8);
	bool						Intersect(const kPoint3& p,const TopoDS_Wire& aWire,double tol = 1e-8);

	int							NbInterPoints() const;
	kcxCurveIntpt				InterPoint(int ix);

protected:
	void						DoIntersect(const kPoint3& p,const TopoDS_Edge& aEdge,int widx,double tol);

protected:
	std::vector<kcxCurveIntpt>	_aIntpt;//�����б�
};

// ���ߺ����ߵĽ���
class QXLIB_API kcgCurveCurveInter{
public:
	kcgCurveCurveInter();

	//
	bool						Intersect(const TopoDS_Edge& aEdge1,const TopoDS_Edge& aEdge2,double tol = 1e-8);
	bool						Intersect(const TopoDS_Edge& aEdge,const TopoDS_Wire& aWire,double tol = 1e-8);
	bool						Intersect(const TopoDS_Wire& aWire1,const TopoDS_Wire& aWire2,double tol = 1e-8);

	int							NbInterPoints() const;
	kcxCCIntpt					InterPoint(int ix);

protected:
	void						DoIntersect(int widx1,const TopoDS_Edge& aEdge1,int widx2,const TopoDS_Edge& aEdge2,double tol);

protected:
	std::vector<kcxCCIntpt>		_aIntpt;
};

// ���ߺ�����Ľ���
class QXLIB_API kcgCurveSurfaceInter{
public:
	kcgCurveSurfaceInter();

	bool						Intersect(const TopoDS_Edge& aEdge,const TopoDS_Face& aFace,double tol = 1e-8);
	bool						Intersect(const TopoDS_Wire& aWire,const TopoDS_Face& aFace,double tol = 1e-8);

	int							NbInterPoints() const;
	kcxCSIntpt					InterPoint(int ix);

protected:
	void						DoIntersect(int widx,const TopoDS_Edge& aEdge,const TopoDS_Face& aFace,double tol);

protected:
	std::vector<kcxCSIntpt>		_aIntpt;
};


#endif