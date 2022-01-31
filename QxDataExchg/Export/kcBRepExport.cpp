#include "stdafx.h"
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include "kcLayer.h"
#include "kcModel.h"
#include "kcEntity.h"
#include "kcEntityFactory.h"
#include "kcgBRepFile.h"
#include "kcBRepExport.h"

kcBRepExport::kcBRepExport(kcModel *pModel)
	:kcBaseExport(pModel)
{
	ASSERT(pModel);
}


kcBRepExport::~kcBRepExport(void)
{
}

void	kcBRepExport::DoExport()
{
	//�������ӵĵ���shape���Ͳ���������Ĵ������ڵ���
	//��ӵĶ���
	if(!_bHasAddedShape){
		if(_bExportSelShape){
			GetAllSelectedShape();
		}else{
			GetAllShapeFromModel();
		}
	}
	if(_aShapeList.IsEmpty()){
		AfxMessageBox("û�ж�����Ե���.");
		return;
	}

	TopoDS_Shape aShape;
	if(_aShapeList.Extent() == 1){
		aShape = _aShapeList.First();
	}else{
		TopoDS_Compound aComp;
		BRep_Builder BB;

		BB.MakeCompound(aComp);
		TopTools_ListIteratorOfListOfShape lls;
		for(lls.Initialize(_aShapeList);lls.More();lls.Next())
		{
			TopoDS_Shape aS = lls.Value();
			BB.Add(aComp,aS);
		}
		aShape = aComp;
	}

	CFileDialog dlg(FALSE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"BRep File (*.brep)|*.brep||", 
		NULL );

	if(dlg.DoModal() == IDOK){
		CWaitCursor wc;

		CString szFile = dlg.GetPathName();
		kcgBRepFile brepFile;
		if(!brepFile.Write(aShape,szFile,FALSE)){
			AfxMessageBox("export to BREP failed.");
		}		
	}
}