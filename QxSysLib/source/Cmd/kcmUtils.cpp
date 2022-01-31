#include "StdAfx.h"
#include "kcBasePlane.h"
#include "kvCoordSystem.h"
#include "kcmUtils.h"

// ���ݵ�ǰ����ƽ���Բ�ġ��뾶����Բ����
Handle(Geom_Circle)  ulb_MakeCircle(const kPoint3& center,double radius,kcBasePlane *pBasePlane)
{
	Handle(Geom_Circle) aCirc;
	if(radius < 1e-6 || !pBasePlane)
		return aCirc;

	//����Բ
	kvCoordSystem cs;
	pBasePlane->GetCoordSystem(cs);

	gp_Ax2 axis(gp_Pnt(center[0],center[1],center[2]),
		gp_Dir(gp_Vec(cs.Z().x(),cs.Z().y(),cs.Z().z())),
		gp_Dir(gp_Vec(cs.X().x(),cs.X().y(),cs.X().z())));
	aCirc = new Geom_Circle(axis,radius);

	return aCirc;
}

Handle(Geom_Circle)  ulb_MakeCircle(const kvCoordSystem& cs,double radius)
{
	Handle(Geom_Circle) aCirc;
	if(radius < 1e-6)
		return aCirc;

	gp_Ax2 axis(gp_Pnt(cs.Org().x(),cs.Org().y(),cs.Org().z()),
		gp_Dir(gp_Vec(cs.Z().x(),cs.Z().y(),cs.Z().z())),
		gp_Dir(gp_Vec(cs.X().x(),cs.X().y(),cs.X().z())));
	aCirc = new Geom_Circle(axis,radius);

	return aCirc;
}