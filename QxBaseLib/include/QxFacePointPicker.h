///
#ifndef _KC_FACE_POINT_PICKER_H_
#define _KC_FACE_POINT_PICKER_H_

#include "QxLibDef.h"
#include <V3d_View.hxx>

//����ƽ�����꣬��ȡ����һ��
class QXLIB_API QxFacePointPicker{
public:
	QxFacePointPicker(const TopoDS_Face &aFace,const Handle(V3d_View) &aView);

	//����ƽ�棬���������ϵ�,�ɹ���������Ӧ��ֵ��ʧ�ܣ�����false
	bool					PickPoint(int scrX,int scrY,double pnt[3],double uv[2]);

protected:
	TopoDS_Face			aFace_;
	Handle(V3d_View)		aView_;
	Handle(Geom_Surface)	aSurf_;
};

#endif