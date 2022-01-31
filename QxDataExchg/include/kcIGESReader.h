//////////////////////////////////////////////////////////////////////////
// ����IGES
#ifndef _KC_IGES_READER_H_
#define _KC_IGES_READER_H_

#include "QxLibDef.h"

class kcModel;

class QXLIB_API kcIGESReader
{
public:
	kcIGESReader(kcModel *pModel);
	~kcIGESReader(void);

	//����IGES�ļ�
	bool					DoImport();

protected:
	// �����������ӵ�model�С�
	bool					AddEntity(const TopoDS_Shape& aShape);
	// ���һ������
	bool					AddOneShape(const TopoDS_Shape& aShape);
	// ��Ӿ���Ķ���
	bool					AddVertex(const TopoDS_Shape& aShape);
	bool					AddEdge(const TopoDS_Shape& aShape);
	bool					AddWire(const TopoDS_Shape& aShape);
	bool					AddFace(const TopoDS_Shape& aShape);
	bool					AddShell(const TopoDS_Shape& aShape);
	bool					AddSolid(const TopoDS_Shape& aShape);
	bool					AddCompound(const TopoDS_Shape& aShape);

protected:
	kcModel					*m_pModel;
};

#endif