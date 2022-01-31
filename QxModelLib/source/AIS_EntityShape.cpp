#include "stdafx.h"
#include "kcEntity.h"
#include "AIS_EntityShape.h"

//IMPLEMENT_STANDARD_HANDLE (AIS_EntityShape, AIS_Shape)
IMPLEMENT_STANDARD_RTTIEXT(AIS_EntityShape, AIS_ColoredShape)

AIS_EntityShape::AIS_EntityShape(const TopoDS_Shape& theShape)
: AIS_ColoredShape(theShape),m_pEntity(NULL)
{
}

int	AIS_EntityShape::SetEntity(kcEntity *pEntity)
{
	ASSERT(pEntity);
	m_pEntity = pEntity;
	return 1;
}

kcEntity*	AIS_EntityShape::GetEntity() const
{
	return m_pEntity;
}

//�ӽ��������ȡ������ʵ�����
kcEntity*	ksGetAisShapeEntity(const Handle(AIS_InteractiveObject)& aObj)
{
	kcEntity *pEntity = NULL;
	if(!aObj.IsNull() && IS_AIS_ENTITYSHAPE(aObj)){
		Handle(AIS_EntityShape) entShape = TO_AIS_ENTITYSHAPE(aObj);
		pEntity = entShape->GetEntity();
	}
	return pEntity;
}