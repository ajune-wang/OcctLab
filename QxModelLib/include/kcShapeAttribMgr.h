////////////////////////////////////////////////
//
#pragma once

#include <NCollection_IndexedDataMap.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopTools_ListOfShape.hxx>
#include "kcShapeAttrib.h"
#include "QxLibDef.h"

typedef NCollection_IndexedDataMap<TopoDS_Shape,Handle(Standard_Transient),TopTools_ShapeMapHasher> kcShapeStepObjIdxMap;
typedef NCollection_IndexedDataMap<TopoDS_Shape,Handle(kcShapeAttrib),TopTools_ShapeMapHasher> kcShapeAttribIdxMap;
class STEPControl_Reader;

class QXLIB_API kcShapeAttribMgr
{
public:
	kcShapeAttribMgr(void);
	~kcShapeAttribMgr(void);

	bool						AddColorAttrib(const TopoDS_Shape &aS,double r,double g,double b);
	bool						AddColorAttrib(const TopoDS_Shape &aS,const Quantity_Color &aCol);
	//
	bool						FindColorAttrib(const TopoDS_Shape &aS,Quantity_Color &aColor);

	kcShapeAttribIdxMap&		ShapeAttribIdxMap() { return aShapeAttribMap_; }
	void						Clear();

	bool						GetShapeList(TopTools_ListOfShape &aShapeList);

public:
	//��ȡÿ������ɫ��face����ɫ������Ϣ
	void						ReadFaceColors(STEPControl_Reader &aReader);
	//��ȡÿ������ɫ��shape����ɫ������Ϣ
	void						ReadShapeColors(STEPControl_Reader &aReader);

public:
	bool						Bind(const TopoDS_Shape &aS,const Handle(Standard_Transient) &aEnt);

public:
	bool						WriteToFile(const char *pszFile);

protected:
	void						ResetShapeStats();

protected:
	kcShapeAttribIdxMap		aShapeAttribMap_;
	kcShapeStepObjIdxMap		aShapeStepObjMap_;
	//
	int						aShapeStats_[10];
};

