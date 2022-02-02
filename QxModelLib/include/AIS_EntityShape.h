///��¼entity�����Ӧ��ais��shape����
#ifndef _AIS_ENTITYSHAPE_H_
#define _AIS_ENTITYSHAPE_H_

#include <AIS_ColoredShape.hxx>
#include "QxLibDef.h"

class kcEntity;
class TopoDS_Shape;

#define IS_AIS_ENTITYSHAPE(aisObj) \
	(aObj->IsKind(STANDARD_TYPE(AIS_EntityShape)))

#define TO_AIS_ENTITYSHAPE(aisObj) \
	Handle(AIS_EntityShape)::DownCast(aisObj)

class AIS_EntityShape : public AIS_ColoredShape
{
public:
	AIS_EntityShape(const TopoDS_Shape& theShape);

	int				SetEntity(kcEntity *pEntity);
	kcEntity*		GetEntity() const;

protected:
	kcEntity			*m_pEntity;//��ais���������Ķ���

public:
	DEFINE_STANDARD_RTTIEXT(AIS_EntityShape,AIS_ColoredShape)
};

//������������ȡEntityShape��entity
//QXLIB_API kcEntity*	ksGetAisShapeEntity(const Handle(AIS_InteractiveObject)& aObj);

DEFINE_STANDARD_HANDLE(AIS_EntityShape,AIS_ColoredShape)


#endif