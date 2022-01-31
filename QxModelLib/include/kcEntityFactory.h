//////////////////////////////////////////////////////////////////////////
//
#ifndef _KC_ENTITY_FACTORY_H_
#define _KC_ENTITY_FACTORY_H_

#include <vector>
#include "QxLibDef.h"

class kcEntity;
class TopoDS_Shape;

class QXLIB_API kcEntityFactory
{
public:
	kcEntityFactory(void);
	~kcEntityFactory(void);

	//�������ͣ�������Ӧ��entity����
	static kcEntity*				CreateByType(int type);

	kcEntity*					Create(const TopoDS_Shape& aShape);
	static kcEntity*				CreateEntity(const TopoDS_Shape& aShape);

	//����һ����һϵ��entity,���shape��compound�����Ƿ�ֽ�Ϊ�Ӷ���
	bool							Create(const TopoDS_Shape& aShape,
									   std::vector<kcEntity *>& aEntList,
									   bool bDeCompound = true);

	//�����ߴ���
	kcEntity*					Create(const Handle(Geom_Curve) &aCurve);
	// �����ơ���ɫ�Ĵ���һ������
	static kcEntity*				Create(const Handle(Geom_Curve) &aCurve,int color,const char *pDesc);

	//�����洴��
	kcEntity*					Create(const Handle(Geom_Surface) &aSurf);
};

#endif