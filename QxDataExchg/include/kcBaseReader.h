//////////////////////////////////////////////////////////////////////////
// ����Ļ���
#ifndef _KC_BASE_READER_H_
#define _KC_BASE_READER_H_

#include <TCollection_AsciiString.hxx>
#include <TopTools_ListOfShape.hxx>
#include "QxLibDef.h"

class kcLayer;
class kcModel;
class Quantity_Color;

class QXLIB_API kcBaseReader
{
public:
	kcBaseReader(kcModel *pModel);
	~kcBaseReader(void);

	//����
	virtual	BOOL			DoImport() = 0;

	const TopTools_ListOfShape& GetShapeList() const { return m_aShapeList; }

protected:
	// ������������model
	virtual BOOL			AddShapeToModel();

	// �����������ӵ�model�С�
	bool					AddEntity(const TopoDS_Shape& aShape,kcLayer *pLayer);
	bool					AddEntity(const TopoDS_Shape& aShape,Quantity_Color &aCol);

protected:
	kcModel					*m_pModel;


	//����Ķ���
	TopTools_ListOfShape	m_aShapeList;
};

//��·���������±���Ϊutf-8��OCCT680��Ҫ����
int   EncodeUtf8FilePath(const char *pFileName,char *pwcFileName,int nSize);

int   EncodeFilePath(const char *pFileName,TCollection_AsciiString& ascStr);

#endif