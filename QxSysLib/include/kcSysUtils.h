//////////////////////////////////////////////////////////////////////////
//
#ifndef _KC_SYS_UTILS_H_
#define _KC_SYS_UTILS_H_

#include "kBase.h"
#include "kPoint.h"
#include "QxLibDef.h"

class kvCoordSystem;

QXLIB_API bool	kcg_CalcRectCorner(kvCoordSystem& cs,const kPoint3& p1,const kPoint3& p2,double c[4][3]);

// ���㵱ǰ����ƽ���ϵ�box�Ľǵ�ͱߵ���Ϣ��p1��p2�ǹ���ƽ���ϵĵ㡣
QXLIB_API bool    kcg_CalcBoxCorner(kvCoordSystem& cs,const kPoint3& p1,const kPoint3& p2,const kPoint3& p3,
						  double c[8][3],int e[12][2]);

// ����������,������ǰ����ƽ���ϵ�ƽ��.
QXLIB_API TopoDS_Face kcg_BuildPlane(kvCoordSystem& cs,const kPoint3& p1,const kPoint3& p2);

// ʹ������Ի������һ������ֵ
// pszDesc��������Ϣ��dValue�ǿ��Դ����ʼֵ������������ֵ,incStep:��ÿ�ε���ֵ
// 
QXLIB_API bool kcDlgInputDoubleValue(const char *pszDesc,double& dValue,double incStep=0.1);

QXLIB_API bool kcDlgInputIntValue(const char *pszDesc,int& iValue,int incStep=1);

#endif