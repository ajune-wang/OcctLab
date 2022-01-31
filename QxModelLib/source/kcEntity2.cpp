///entity�ı���Ͷ�ȡ����
#include "StdAfx.h"
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include "kcModel.h"
#include "kcLayer.h"
#include "kcEntity.h"
#include "kcEntityFactory.h"
#include "kcHandleMgr.h"
#include "QxBRepLib.h"
#include "QxStgBlock.h"
#include "QxStgModelDef.h"
#include "AIS_EntityShape.h"
#include "kcEntity.h"

///////////////////////////////////////////////////////
//
// ��ȡ,��Ҫ��ȡ��Ϣ��,���Ϳ��ⲿ�Ѿ���ȡ��

// ��ar�ж���һ��kcEntity����
kcEntity* kcEntity::ReadEntity(CArchive &ar)
{
	QxStgBlock tblk;
	int entType = 0;
	
	//��ȡ����
	if(!tblk.Read(ar,KSTG_BLOCK_ENTITY_TYPE) ||
		!tblk.GetValueByCode(KSTG_CV_CODE_TYPE,entType))
	{
		ASSERT(FALSE);
		return NULL;
	}
	
	//��������ȡ����
	kcEntity *pEntity = kcEntityFactory::CreateByType(entType);
	if(pEntity && pEntity->Read(ar)){
		return pEntity;
	}
	KC_SAFE_DELETE(pEntity);

	return NULL;
}

//
BOOL kcEntity::Read(CArchive& ar)
{
	QxStgBlock blk;
	if(!blk.Read(ar,KSTG_BLOCK_ENTITY)){
		ASSERT(FALSE);
		return FALSE;
	}

	//������shape����
	aShape_ = ParseShapeData(blk);
	if(aShape_.IsNull()){
		ASSERT(FALSE);
		return FALSE;
	}

	//��ȡͨ������
	std::string sName;
	bool bVisible = false;
	QxStgBlock *pAttrBlk = blk.GetSubBlock(100); //��ȡ���Կ�
	if(!blk.GetValueByCode(KSTG_CV_CODE_HANDLE,_nHandle) ||
		!blk.GetValueByCode(KSTG_CV_CODE_VISIBLE,bVisible) ||
		!pAttrBlk)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	if(blk.GetValueByCode(KSTG_CV_CODE_NAME,sName)){//����Ϊ��
		//��¼����
		strncpy_s(_szName,KC_ENT_NAME_MAX,sName.c_str(),sName.size());
	}

	//������������
	if(!DoGetOtherCodeValue(blk)){
		return FALSE;
	}

	//�����Կ��ж�ȡ	
	if(!ParseCommonAttrib(*pAttrBlk) || !DoParseAttrib(*pAttrBlk)){
		return FALSE;
	}

	//��������д��Ŀ�
	if(!DoReadOtherData(ar)){
		return FALSE;
	}
	
	SetModified(TRUE);
	SetValid(TRUE);

	return TRUE;
}

// д��
BOOL	kcEntity::Write(CArchive& ar)
{
	//д������block,�������
	QxStgBlock tblk;
	tblk.Initialize(KSTG_BLOCK_ENTITY_TYPE);
	tblk.AddCodeValue(KSTG_CV_CODE_TYPE,_entType);
	tblk.Write(ar);

	//д������,���Ե���д��һ������
	QxStgBlock blk;
	QxStgBlock *pAttrBlk = new QxStgBlock(KSTG_BLOCK_ENTITY_ATTRIB);

	blk.Initialize(KSTG_BLOCK_ENTITY);
	//д��shape������
	AddShapeData(blk,aShape_);
	// ���빲ͬ��ֵ
	//���
	blk.AddCodeValue(KSTG_CV_CODE_HANDLE,_nHandle);
	//����
	if(_szName[0] != '\0'){
		blk.AddCodeValue(KSTG_CV_CODE_NAME,_szName);
	}
	//�Ƿ�ɼ�
	bool bVisible = IsVisible();
	blk.AddCodeValue(KSTG_CV_CODE_VISIBLE,bVisible);
	// ��������������
	DoAddOtherCodeValue(blk);
	
	//����ͨ������
	AddCommonAttrib(*pAttrBlk);
	//��Ӹ��Ե�����
	DoAddAttrib(*pAttrBlk);

	//�������Կ�
	blk.AddCodeValue(100,pAttrBlk,true);

	blk.Write(ar);

	//�����ڸÿ���棬����д��������Ŀ�
	DoWriteOtherData(ar);

	return TRUE;
}

// ��ӹ�������
BOOL	kcEntity::AddCommonAttrib(QxStgBlock& blk)
{
	//��ɫ
	double c[3];
	m_color.get(c);
	blk.AddCodeValue(KSTG_CV_CODE_COLOR,c);
	//

	return TRUE;
}

//
BOOL	kcEntity::ParseCommonAttrib(QxStgBlock& blk)
{
	double col[3];

	if(!blk.GetValueByCode(KSTG_CV_CODE_COLOR,col)){
		ASSERT(FALSE);
	}

	//��ɫ
	m_color.set(col);

	return TRUE;
}

// ��TopoDS_Shape��������ݣ�ת����byte���飬��ӵ�blk��
//
BOOL  kcEntity::AddShapeData(QxStgBlock& blk,const TopoDS_Shape& aShape)
{
	if(!aShape.IsNull()){
		kstgShapeData sd;
		BYTE *pBuf = NULL;
		int nSize = 0;

		if(sd.Build(aShape,pBuf,nSize)){
			blk.AddCodeValue(KSTG_CV_CODE_SHAPE,pBuf,nSize,false);
			return TRUE;
		}
	}
	
	return FALSE;
}

// ��blk�н�����TopoDS_Shape��������ݣ����������ɶ���
//
TopoDS_Shape	kcEntity::ParseShapeData(QxStgBlock& blk)
{
	TopoDS_Shape aShape;
	QxStgCodeValue *pCV = NULL;
	BYTE *pData = NULL;
	int nSize = 0;

	pCV = blk.GetCodeValueByCode(KSTG_CV_CODE_SHAPE);
	if(pCV && pCV->GetValue(pData,nSize)){
		kstgShapeData sd;
		aShape = sd.Parse(pData,nSize);	
	}else{
		ASSERT(FALSE);
	}

	return aShape;
}

///////////////////////////////////////////////////////
//

BOOL	kcPointEntity::DoAddAttrib(QxStgBlock& blk)
{
	return TRUE;
}

BOOL	kcPointEntity::DoParseAttrib(QxStgBlock& blk)
{
	return TRUE;
}

///////////////////////////////////////////////////////
//
BOOL	kcEdgeEntity::DoAddAttrib(QxStgBlock& blk)
{
	return TRUE;
}

BOOL	kcEdgeEntity::DoParseAttrib(QxStgBlock& blk)
{
	return TRUE;
}

///////////////////////////////////////////////////////
//
BOOL	kcWireEntity::DoAddAttrib(QxStgBlock& blk)
{
	return TRUE;
}

BOOL	kcWireEntity::DoParseAttrib(QxStgBlock& blk)
{
	return TRUE;
}

///////////////////////////////////////////////////////
//
BOOL	kcFaceEntity::DoAddAttrib(QxStgBlock& blk)
{
	return TRUE;
}

BOOL	kcFaceEntity::DoParseAttrib(QxStgBlock& blk)
{
	return TRUE;
}


///////////////////////////////////////////////////////
//
BOOL	kcShellEntity::DoAddAttrib(QxStgBlock& blk)
{
	return TRUE;
}

BOOL	kcShellEntity::DoParseAttrib(QxStgBlock& blk)
{
	return TRUE;
}

///////////////////////////////////////////////////////
//
BOOL	kcSolidEntity::DoAddAttrib(QxStgBlock& blk)
{
	return TRUE;
}

BOOL	kcSolidEntity::DoParseAttrib(QxStgBlock& blk)
{
	return TRUE;
}

///////////////////////////////////////////////////////
//
BOOL	kcCompoundEntity::DoAddAttrib(QxStgBlock& blk)
{
	return TRUE;
}

BOOL	kcCompoundEntity::DoParseAttrib(QxStgBlock& blk)
{
	return TRUE;
}

///////////////////////////////////////////////////////
//
bool  kcBlockEntity::DoAddOtherCodeValue(QxStgBlock& blk)
{
	//��¼�Ӷ���ĸ���
	int num = (int)aEntList_.size();
	blk.AddCodeValue(20,num);

	return true;
}

bool kcBlockEntity::DoGetOtherCodeValue(QxStgBlock& blk)
{
	if(!blk.GetValueByCode(20,nReadEntNum_)){
		nReadEntNum_ = 0;
	}
	return true;
}

bool kcBlockEntity::DoWriteOtherData(CArchive& ar)
{
	//����д��ÿ����Entity
	kcEntityList::iterator ite = aEntList_.begin();
	for(;ite != aEntList_.end();++ite){
		(*ite)->Write(ar);
	}
	return true;
}

bool kcBlockEntity::DoReadOtherData(CArchive& ar)
{
	//���ζ�ȡÿ����Entity
	int ix = 0;
	kcEntity *pEntity = NULL;
	for(ix = 0;ix < nReadEntNum_;ix ++){
		pEntity = kcEntity::ReadEntity(ar);
		if(!pEntity){
			return false;
		}
		//
		AddEntity(pEntity);
	}

	return true;
}