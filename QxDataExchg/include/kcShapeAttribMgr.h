////////////////////////////////////////////////
//
#pragma once

#include <NCollection_IndexedDataMap.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopTools_ListOfShape.hxx>
#include "kcShapeAttrib.h"

class STEPControl_Reader;
typedef NCollection_IndexedDataMap<TopoDS_Shape,Handle(kcShapeAttrib),TopTools_ShapeMapHasher> kcShapeAttribIdxMap;

// ����ת�����shape��������Ϣ.
class kcShapeAttribMgr
{
public:
	kcShapeAttribMgr(void);
	~kcShapeAttribMgr(void);

	bool						AddColorAttrib(const TopoDS_Shape &aS,double r,double g,double b);
	bool						AddColorAttrib(const TopoDS_Shape &aS,const Quantity_Color &aCol);

	// ����shape����ɫ����
	bool						FindColorAttrib(const TopoDS_Shape &aS,Quantity_Color &aColor);
	// ����face����ɫ
	bool						FindFaceColor(const TopoDS_Shape &aS,Quantity_Color &aColor);

	kcShapeAttribIdxMap&		ShapeAttribIdxMap() { return aShapeAttribMap_; }
	void						Clear();

	// �Ƿ������ɫ������Ϣ
	bool						HasColorAttribs() const;
	bool						HasFaceColor() const;

	// ��ȡ������ɫ��shape���б�
	bool						GetShapeList(TopTools_ListOfShape &aShapeList);

public:
	//��ȡÿ������ɫ��face����ɫ������Ϣ
	void						ReadFaceColors(STEPControl_Reader &aReader);

	//��ȡÿ������ɫ��shape����ɫ������Ϣ
	void						ReadShapeColors(STEPControl_Reader &aReader);

protected:
	kcShapeAttribIdxMap		aShapeAttribMap_;//shape����������Ϣ
	kcShapeAttribIdxMap		aFaceSpecColorMap_;//��¼���е���ɫ��face����Ϣ�����磺һ��solid��һ����ɫ������һ��face�ǵ�������ɫ
	kcShapeAttribIdxMap		aFaceOfShapeColorMap_;//��solid��shell������ɫ����ʱ�������¼shape��ÿ��face���󣬼�����ɫ��Ϣ
};

