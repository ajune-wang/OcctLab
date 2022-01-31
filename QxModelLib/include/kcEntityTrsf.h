///////////////////////////////////////////////////////////////////////////
// �����˶�Entity�ı任����
#ifndef _KC_ENTITY_TRSF_H_
#define _KC_ENTITY_TRSF_H_

#include "kPoint.h"
#include "kVector.h"
#include "QxLibDef.h"

class kcEntity;

class QXLIB_API kcEntityTrsf
{
public:
	kcEntityTrsf(void);
	~kcEntityTrsf(void);

	// �ƶ�����,�����µĶ���
	kcEntity*						Move(kcEntity *pEntity,const kVector3& vec);

	// ��ת
	// p:����,dir:��λ������ang����ת�Ƕȣ����ȡ�
	kcEntity*						Rotate(kcEntity *pEntity,const kPoint3& p,const kVector3& dir,double ang);

	// ����
	kcEntity*						Scale(kcEntity *pEntity,const kPoint3& p,double scale);
};

#endif