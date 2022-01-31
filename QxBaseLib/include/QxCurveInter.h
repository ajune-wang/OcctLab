//
#ifndef _QX_CURVE_INTER_H_
#define _QX_CURVE_INTER_H_

#include <vector>
#include <Geom_Curve.hxx>
#include "QxLibDef.h"

class QxCCInterPoint;
class QxCCInterCurve;
typedef std::vector<QxCCInterPoint> KCCInterPointVector;
typedef std::vector<QxCCInterCurve> KCCInterCurveVector;

//˵����
//  �����󽻣���������˲��������е�edge��edge�󽻣�
//  ����Ҫ�Ż����ر��Ƕ����޳�ֱ�ߺ�����������Ŀǰ��û����Ӵ���
//  ���������Ҫ���󽻺���

class QXLIB_API QxParamRange{
public:
	QxParamRange();
	QxParamRange(double t1,double t2);

	void			Set(double t1,double t2);

public:
	double		ts_,te_;//������Χֵ
};

//����
class QXLIB_API QxCCInterPoint{
public:
	QxCCInterPoint();

	void			Set(double t1,double t2);

public:
	double		t1_,t2_;//�������
	double		pnt1_[3],pnt2_[3];//����
};

//����
class QXLIB_API QxCCInterCurve{
public:
	QxCCInterCurve();

public:
	QxParamRange	aRange1_,aRange2_;//������Χ
};

class QXLIB_API QxCurveInter{
public:
	QxCurveInter();
	~QxCurveInter();

	//����������
	bool			Intersect(const Handle(Geom_Curve) &aCurve1,const Handle(Geom_Curve) &aCurve2,double tol);

	//�������߲�������
	bool			Intersect(const Handle(Geom_Curve) &aCurve1,double t11,double t12,
						const Handle(Geom_Curve) &aCurve2,double t21,double t22,double tol);

	void			Clear();

	int			NbPoints() const;
	const QxCCInterPoint& GetInterPoint(int ix) const;

	int			NbCurves() const;
	const QxCCInterCurve& GetInterCurve(int ix) const;

protected:
	KCCInterPointVector	aIntPntVec_;//�����б�
	KCCInterCurveVector	aIntCrvVec_;//�����б�
};


#endif