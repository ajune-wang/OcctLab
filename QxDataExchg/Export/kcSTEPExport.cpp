#include "stdafx.h"
#include <string>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_Writer.hxx>
#include <XSControl_WorkSession.hxx>
#include <Transfer_TransientProcess.hxx>
#include "kcLayer.h"
#include "kcModel.h"
#include "kcEntity.h"
#include "kcEntityFactory.h"
#include "kcBaseReader.h"
#include "kcSTEPExport.h"


kcSTEPExport::kcSTEPExport(kcModel *pModel)
	:kcBaseExport(pModel)
{
}


kcSTEPExport::~kcSTEPExport(void)
{
}

void	kcSTEPExport::DoExport()
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
	
	CFileDialog dlg(FALSE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"STEP Files (*.step , *.stp)|*.step; *.stp||", 
		NULL );

	if(dlg.DoModal() == IDOK){
		CString szFile = dlg.GetPathName();

		CWaitCursor wc;
		char utf8Char[1024];
		
		EncodeUtf8FilePath(szFile,utf8Char,1024);

		IFSelect_ReturnStatus status = WriteSTEP(utf8Char);
		if(status != IFSelect_RetDone){
			AfxMessageBox("����ʧ��!");
		}
	}
}

IFSelect_ReturnStatus	kcSTEPExport::WriteSTEP(const Standard_CString& aFileName)
{
	STEPControl_Writer aWriter;
	STEPControl_StepModelType aExpType;

	IFSelect_ReturnStatus status;
	TopTools_ListIteratorOfListOfShape lls;
	for(lls.Initialize(_aShapeList);lls.More();lls.Next())
	{
		TopoDS_Shape aS = lls.Value();
		//��ȡ�����ʽ
		aExpType = GetExportType(aS);
		
		//ת��
		status =  aWriter.Transfer(aS, aExpType);
		if ( status != IFSelect_RetDone ) return status;
	}
	status = aWriter.Write(aFileName);

	return status;
}

static STEPControl_StepModelType axGetCompoundExpType(const TopoDS_Compound& aComp)
{
	STEPControl_StepModelType expType = STEPControl_ManifoldSolidBrep,tType;
	TopoDS_Iterator ite;

	for(ite.Initialize(aComp);ite.More();ite.Next()){
		TopoDS_Shape aS = ite.Value();
		if(aS.ShapeType() == TopAbs_VERTEX || aS.ShapeType() == TopAbs_EDGE || 
			aS.ShapeType() == TopAbs_WIRE)
		{
			expType = STEPControl_GeometricCurveSet;//�������
			break;
		}
		if(aS.ShapeType() == TopAbs_SHELL || aS.ShapeType() == TopAbs_FACE){
			expType = STEPControl_ShellBasedSurfaceModel;//�������
		}
		if(aS.ShapeType() == TopAbs_COMPOUND){
			tType = axGetCompoundExpType(TopoDS::Compound(aS));
			if(tType != STEPControl_ManifoldSolidBrep){
				expType = tType;
				if(expType == STEPControl_GeometricCurveSet)
					break;
			}
		}
	}

	return expType;
}

STEPControl_StepModelType kcSTEPExport::GetExportType(const TopoDS_Shape& aShape)
{
	STEPControl_StepModelType expType = STEPControl_ManifoldSolidBrep;
	TopAbs_ShapeEnum shpType = aShape.ShapeType();

	if(shpType == TopAbs_VERTEX || shpType == TopAbs_EDGE || shpType == TopAbs_WIRE)
		expType = STEPControl_GeometricCurveSet;
	else if(shpType == TopAbs_FACE || shpType == TopAbs_SHELL)
		expType = STEPControl_ShellBasedSurfaceModel;
	else if(shpType == TopAbs_COMPOUND){
		expType = axGetCompoundExpType(TopoDS::Compound(aShape));
	}else{
		expType = STEPControl_ManifoldSolidBrep;
	}

	return expType;
}