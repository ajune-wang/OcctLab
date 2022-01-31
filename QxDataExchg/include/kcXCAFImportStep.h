//////////////////////////////////////////////////////////////////
// ����XCAF��ʽ������STEP�ļ�

#pragma once

#include <TDocStd_Document.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include "kcStepMbStringMgr.h"
#include "kcShapeAttribMgr.h"
#include "kcStepLayerMgr.h"

class TCollection_AsciiString;

#define XSTEPIMP_UNKNOWN			0
#define XSTEPIMP_SUCCESS			1	// ����ɹ�
#define XSTEPIMP_PART_SUCCESS		2   // ���벿�ֳɹ�
#define XSTEPIMP_FAILED			-1	// ����ʧ��
#define XSTEPIMP_NOTHING			-2	// û�е����κζ���
#define XSTEPIMP_READ_FAILED		-3	// ��ȡSTEP�ļ�ʧ��
#define XSTEPIMP_TRANSFER_FAILED	-4	// ��ȡSTEP�ļ�ʧ��
#define XSTEPIMP_NO_SOLID			-5  // �ļ���û��ʵ�����
#define XSTEPIMP_CANCEL			-6  // ������̱���ֹ��

//��¼һ����������


class kcXCAFImportStep
{
public:
	kcXCAFImportStep(void);
	~kcXCAFImportStep(void);

	void							EnableConvAsm(bool bEnable);

	// ��ȡSTEP�ļ�
	virtual int					ReadStep(const char *pszStepFile);

	// ����ת��
	virtual int					DoConvert();

protected:
	// �򵥵�ת����ʽ��ֻ����shape
	virtual int					DoConvertShape();

	// ���ӵ�ת����ʽ����������װ����ṹ
	virtual int					DoConvertAsm();
	//
	virtual int					ConvertAssembly(const TDF_Label &aLabel);
	virtual int					ConvertPart(const TDF_Label &aLabel);

	//��Ҫ����ʵ�ֵ���
	virtual int					DoConvOccShape(const TopoDS_Shape &aShape) = 0;

protected:
	int							EncodeFilePath(const char *pFileName,TCollection_AsciiString& ascStr);

protected:
	int							bConvAsm_;//�Ƿ�ת��װ����ṹ

protected:
	Handle(TDocStd_Document)		aDoc_;//�ĵ�����
	Handle(XCAFDoc_ShapeTool)		aShapeTool_;
	Handle(XCAFDoc_ColorTool)		aColorTool_;
	Handle(XCAFDoc_LayerTool)		aLayerTool_;

protected:
	kcStepMbStringMgr				aStepMbStrMgr_;//��¼���ֽ��ַ��������н���
	kcShapeAttribMgr				aShapeAttrMgr_;//��¼��ɫ��Ϣ
	kcStepLayerMgr				aStepLayerMgr_;//��¼ͼ����Ϣ

protected:
	int							nResult_;//
};

