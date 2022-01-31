#include "StdAfx.h"
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include "kcLayer.h"
#include "kcModel.h"
#include "kcEntity.h"
#include "kcEntityFactory.h"
#include "kcgBRepFile.h"
#include "kcBrepReader.h"

kcBRepReader::kcBRepReader(kcModel *pModel)
:kcBaseReader(pModel)
{
}

kcBRepReader::~kcBRepReader(void)
{
}

//����
BOOL	kcBRepReader::DoImport()
{
	if(!m_pModel)
		return FALSE;

	CFileDialog dlg(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"BRep File (*.brep)|*.brep||", 
		NULL );

	if(dlg.DoModal() == IDOK)
	{
		CWaitCursor wc;

		CString szFile = dlg.GetPathName();
		kcgBRepFile brepFile;
		TopoDS_Shape aShape;
		if(!brepFile.Read(aShape,szFile,FALSE))
			return FALSE;

		m_aShapeList.Clear();
		//��ȡ����compound����Ҫ�ڲ��Ķ���
		if(aShape.ShapeType() == TopAbs_COMPOUND ||
			aShape.ShapeType() == TopAbs_COMPSOLID)
		{
			TopoDS_Iterator ite;
			for(ite.Initialize(aShape);ite.More();ite.Next())
			{
				m_aShapeList.Append(ite.Value());
			}
		}
		else
		{
			m_aShapeList.Append(aShape);
		}
		
		if(!m_aShapeList.IsEmpty())
			AddShapeToModel();
	}

	return TRUE;
}