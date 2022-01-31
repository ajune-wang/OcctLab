//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_UTILS_H_
#define _KCM_UTILS_H_

#include "kPoint.h"
#include "QxLibDef.h"

class kvCoordSystem;
class kcBasePlane;

// ���ݵ�ǰ����ƽ���Բ�ġ��뾶����Բ����
QXLIB_API Handle(Geom_Circle)  ulb_MakeCircle(const kPoint3& center,double radius,kcBasePlane *pWorkPlane);

QXLIB_API Handle(Geom_Circle)  ulb_MakeCircle(const kvCoordSystem& lcs,double radius);


#endif