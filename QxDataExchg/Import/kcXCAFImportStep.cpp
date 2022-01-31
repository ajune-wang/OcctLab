#include "stdafx.h"
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPCAFControl_Reader.hxx>
//#include <XSControl_WorkSession.hxx>
//#include <TDocStd_Document.hxx>
#include <XCAFApp_Application.hxx>
//#include <Transfer_TransientProcess.hxx>
//#include <StepData_StepModel.hxx>
//#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
//#include <XCAFDoc_ColorTool.hxx>
//#include <XCAFDoc_LayerTool.hxx>
//#include <TDF_Tool.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_LabelSequence.hxx>
//#include <TDF_AttributeIterator.hxx>
//#include <TDataStd_Name.hxx>
//#include <NCollection_UtfIterator.hxx>
//#include <STEPConstruct_Styles.hxx>
//#include <StepVisual_StyledItem.hxx>
//#include <StepRepr_Representation.hxx>
//#include <StepShape_AdvancedFace.hxx>
//#include <XSControl_TransferReader.hxx>
//#include <Transfer_TransientProcess.hxx>
//#include <TransferBRep.hxx>
//#include <TopTools_MapOfShape.hxx>
//#include <StepVisual_Colour.hxx>

#include "kcXCAFImportStep.h"


kcXCAFImportStep::kcXCAFImportStep(void)
{
	bConvAsm_ = false;
	nResult_ = XSTEPIMP_UNKNOWN;
}


kcXCAFImportStep::~kcXCAFImportStep(void)
{
}

void  kcXCAFImportStep::EnableConvAsm(bool bEnable)
{
	bConvAsm_ = bEnable;
}

int  kcXCAFImportStep::ReadStep(const char *pszStepFile)
{
	ASSERT(pszStepFile != NULL);
	
	//��ȡ���ֽ��ַ���
	aStepMbStrMgr_.Clear();
	aStepMbStrMgr_.Read(pszStepFile);

	//��ȡ�ļ�����
	STEPCAFControl_Reader aReader;
	try{
		//���ļ�·��תΪunicode��ʹ��utf8����
		TCollection_AsciiString ascPath;
		if(!EncodeFilePath(pszStepFile,ascPath)){
			return XSTEPIMP_FAILED;
		}

		IFSelect_ReturnStatus status = aReader.ReadFile(ascPath.ToCString());
		if (status != IFSelect_RetDone){
			TRACE("\n ##-- STEPCAFControl_Reader read file failed.error is %d.....",(int)status);
			return XSTEPIMP_READ_FAILED;
		}
	}catch(Standard_Failure){
		TRACE("\n ##-- STEPCAFControl_Reader read file has exception.....");
		return XSTEPIMP_READ_FAILED;
	}

	//������ת��document��
	Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
	TCollection_ExtendedString docnam("MDTV-XCAF");
	app->NewDocument(docnam,aDoc_);

	try{
		if(!aReader.Transfer(aDoc_)){
			TRACE("\n ##-- STEPCAFControl_Reader Transfer failed.....");
			return XSTEPIMP_TRANSFER_FAILED;
		}
	}catch(Standard_Failure){
		TRACE("\n ##-- STEPCAFControl_Reader Transfer has exception.....");
		return XSTEPIMP_TRANSFER_FAILED;
	}

	//��ȡ��ɫ��Ϣ
	aShapeAttrMgr_.Clear();
	aShapeAttrMgr_.ReadShapeColors(aReader.ChangeReader());

	//��ȡͼ����Ϣ
	aStepLayerMgr_.Clear();
	aStepLayerMgr_.ReadLayers(aDoc_);

	nResult_ = XSTEPIMP_SUCCESS;

	return XSTEPIMP_SUCCESS;
}

int  kcXCAFImportStep::EncodeFilePath(const char *pFileName,TCollection_AsciiString& ascStr)
{
	wchar_t wcFileName[1024];
	int wLen = 0;

	wLen = MultiByteToWideChar(CP_ACP,0,pFileName,-1,NULL,0);
	if(wLen > 0 && wLen < 1024){
		MultiByteToWideChar(CP_ACP,0,pFileName,-1,wcFileName,wLen + 1);//תΪunicode
		TCollection_AsciiString tmpStr((const Standard_WideChar*)wcFileName);
		ascStr.Copy(tmpStr);

		return 1;
	}

	return 0;
}

// ����ת��
int  kcXCAFImportStep::DoConvert()
{
	if(nResult_ != XSTEPIMP_SUCCESS){
		return nResult_;
	}

	int rc = XSTEPIMP_UNKNOWN;
	if(bConvAsm_){
		rc = DoConvertAsm();
	}else{
		rc = DoConvertShape();
	}

	return rc;
}

// �򵥵�ת����ʽ��ֻ����shape
int  kcXCAFImportStep::DoConvertShape()
{

	return XSTEPIMP_SUCCESS;
}

// ���ӵ�ת����ʽ����������װ����ṹ
int  kcXCAFImportStep::DoConvertAsm()
{
	aShapeTool_ = XCAFDoc_DocumentTool::ShapeTool(aDoc_->Main());
	aColorTool_ = XCAFDoc_DocumentTool::ColorTool(aDoc_->Main());
	aLayerTool_ = XCAFDoc_DocumentTool::LayerTool(aDoc_->Main());

	TDF_LabelSequence aFreeLabels;
	aShapeTool_->GetFreeShapes(aFreeLabels);
	if(aFreeLabels.IsEmpty()){
		return XSTEPIMP_NOTHING;
	}

	int ix,nbLabs = aFreeLabels.Length();
	int rc = XSTEPIMP_SUCCESS;

	for(ix = 1;ix <= nbLabs;ix ++){
		TDF_Label aLabel = aFreeLabels.Value(ix);

		//
		if(XCAFDoc_ShapeTool::IsAssembly(aLabel)){
			rc = ConvertAssembly(aLabel);
		}else{
			rc = ConvertPart(aLabel);
		}
	}

	aShapeTool_.Nullify();
	aColorTool_.Nullify();
	aLayerTool_.Nullify();

	return XSTEPIMP_SUCCESS;
}

// ��װ�������������ת��
int  kcXCAFImportStep::ConvertAssembly(const TDF_Label &aLabel)
{
	int rc = XSTEPIMP_SUCCESS;
	TDF_ChildIterator ite(aLabel);
	for(;ite.More();ite.Next()){
		TDF_Label aSL = ite.Value();

		//�ж��Ƿ���װ����
		bool bSubAsm = false;
		if(XCAFDoc_ShapeTool::IsComponent(aSL)){
			TDF_Label aRefLab;
			if(aShapeTool_->GetReferredShape(aSL,aRefLab) && 
				XCAFDoc_ShapeTool::IsAssembly(aRefLab))
			{
				bSubAsm = true;
				rc = ConvertAssembly(aRefLab);
			}
		}
		//����װ���壬ֱ��ת��
		if(!bSubAsm){
			rc = ConvertPart(aSL);
		}
	}
	
	return XSTEPIMP_SUCCESS;
}

// ������������ת��
int  kcXCAFImportStep::ConvertPart(const TDF_Label &aLabel)
{
	TopoDS_Shape aShape;
	if(!aShapeTool_->GetShape(aLabel,aShape)){
		TRACE("\n #-- Get Shape of Label failed.");
		return XSTEPIMP_FAILED;
	}
	
	return DoConvOccShape(aShape);
}
