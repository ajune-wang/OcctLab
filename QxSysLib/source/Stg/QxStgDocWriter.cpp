#include "StdAfx.h"
#include "kcModel.h"
#include "QxStgBlock.h"
#include "kcDocContext.h"
#include "QxStgModelDef.h"
#include "QxStgDocDef.h"
#include "QxStgDocWriter.h"

QxStgDocWriter::QxStgDocWriter(kcDocContext *pDocCtx)
{
	ASSERT(pDocCtx);
	pDocContext_ = pDocCtx;
}

QxStgDocWriter::~QxStgDocWriter(void)
{
}

//д������
bool QxStgDocWriter::Save(CArchive &ar)
{
	ASSERT(ar.IsStoring());
	kcModel *pModel = pDocContext_->GetModel();
	if(!pModel)
		return false;

	//д���ļ�ͷ
	WriteHeaderBlock(ar);

	//д��ȫ������
	WriteGlobalBlock(ar);

	//д��ģ������
	WriteModel(ar,pModel);

	return true;
}

//д���ļ�
BOOL QxStgDocWriter::WriteModel(CArchive& ar,kcModel *pModel)
{
	ASSERT(ar.IsStoring());
	ASSERT(pModel);
	
	//д��ģ���ļ�
	if(!pModel->Write(ar))
		return FALSE;

	return TRUE;
}

//����ͷ��,Ŀǰ��ͷ�ܼ�
BOOL QxStgDocWriter::WriteHeaderBlock(CArchive& ar)
{
	//�������
	QxStgBlock blk;
	//
	blk.Initialize(KSTG_BLOCK_DOC_HEADER);
	blk.AddCodeValue(0,(UINT)KSTG_DOC_FILE_MAGIC);//magic
	blk.AddCodeValue(1,(UINT)KSTG_DOC_FILE_VERSION);//version
	//
	blk.AddCodeValue(2,"opencasecade org");//desc

	blk.Write(ar);
    
	return TRUE;
}

//����ȫ�ֿ�
BOOL QxStgDocWriter::WriteGlobalBlock(CArchive& ar)
{
	QxStgBlock blk;
	//
	blk.Initialize(KSTG_BLOCK_DOC_GLOBAL);

	//����ɫ
	double bkCol[3];
	bkCol[0] = 0.2;
	bkCol[1] = 0.3;
	bkCol[2] = 0.4;
	blk.AddCodeValue(0,bkCol);
	//
	blk.Write(ar);

	return TRUE;
}