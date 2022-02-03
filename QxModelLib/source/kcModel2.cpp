// ģ�͵ı���Ͷ�ȡʵ��
#include "StdAfx.h"
#include <AIS_InteractiveObject.hxx>

#include "kcEntity.h"
#include "kcLayer.h"
#include "kcHandleMgr.h"
#include "QxStgBlock.h"
#include "kuiInterface.h"
#include "QxStgModelDef.h"
#include "kcBasePlane.h"
#include "kcModel.h"

//˵����
//  Ϊ�˷�����������ʵ��ĳЩ��ĺ��ԣ�model�еı��棬ͨ���������·�ʽ��
//  begin block ��¼����Ϣ
//  һϵ�е���block����¼��Ӧ������
//  end block ������¼
//  begin and end block������һ��codeֵ��ͨ����(blocktype,code)ʵ��begin��end�����,���code
//  ��Ҫ�����ͻ���������ǿ�Ƶġ�

// ����Ͷ�ȡ
//����������Ϣ��
// 1��handleMgr��Ϣ 2��ͼ���ͼԪ��Ϣ 3����׼����Ϣ
//
BOOL kcModel::Read(CArchive& ar,UINT dwFileVersion)
{
	ASSERT(ar.IsLoading());
	
	//����ɵ�����
	Clear();

	dwFileVersion_ = dwFileVersion;

	QxStgBlock blk;
	//��ȡ��ʼ��
	if(!blk.Read(ar,KSTG_BLOCK_MODEL)){
		ASSERT(FALSE);
		return FALSE;
	}
	UINT modelTag = 0;
	if(!blk.GetValueByCode(KSTG_CV_CODE_BLOCK_TAG,modelTag) || modelTag != 1){
		ASSERT(FALSE);
		return FALSE;
	}

	if(!ReadHandleMgr(ar) ||
		!ReadLayerData(ar) || 
		!ReadBasePlaneData(ar))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//��ȡ������
	if(!blk.Read(ar,KSTG_BLOCK_MODEL_END)){
		ASSERT(FALSE);
		return FALSE;
	}
	if(!blk.GetValueByCode(KSTG_CV_CODE_BLOCK_TAG,modelTag) || modelTag != 1){
		ASSERT(FALSE);
		return FALSE;
	}
		
	return TRUE;
}

// д���ļ���
BOOL	kcModel::Write(CArchive& ar)
{
	ASSERT(ar.IsStoring());

	UINT modelCode = 1;
	//д��ʼ
	QxStgBlock blk;

	blk.Initialize(KSTG_BLOCK_MODEL);
	blk.AddCodeValue(KSTG_CV_CODE_BLOCK_TAG,modelCode);
	blk.Write(ar);

	//1��HandleMgr��Ϣ
	WriteHandleMgr(ar);

	//2��ͼ����Ϣ
	WriteLayerData(ar);
	
	//3����׼����Ϣ
	WriteBasePlaneData(ar);

	//д�������
	blk.Initialize(KSTG_BLOCK_MODEL_END);
	blk.AddCodeValue(KSTG_CV_CODE_BLOCK_TAG,modelCode);
	blk.Write(ar);
	
	return TRUE;
}

BOOL kcModel::ReadHandleMgr(CArchive& ar)
{
	if(_pHandleMgr){
		if(!_pHandleMgr->Read(ar)){
			ASSERT(FALSE);
			return FALSE;
		}
	}else{
		ASSERT(FALSE);
	}

	return TRUE;
}

BOOL kcModel::WriteHandleMgr(CArchive& ar)
{
	ASSERT(_pHandleMgr);

	if(_pHandleMgr){
		_pHandleMgr->Write(ar);
	}

	return TRUE;
}

BOOL  kcModel::ReadLayerData(CArchive& ar)
{
	//ͼ����Ϣ
	QxStgBlock blk;

	if(!blk.Read(ar,KSTG_BLOCK_LAYERS)){
		TRACE("\n ReadLayerData False.");
		return FALSE;
	}

	int nCurLayerId = 0,nbLayer = 0;
	if(!blk.GetValueByCode(1,nbLayer) ||
		!blk.GetValueByCode(2, nCurLayerId))//��ȡ��ǰͼ���handle
	{
		ASSERT(FALSE);
		return FALSE;
	}
	
	//���ζ�ȡÿ��ͼ��
	kcLayer *pLayer = NULL;
	int idx;

	for(idx = 0;idx < nbLayer;idx ++){
		pLayer = new kcLayer;
		if(!pLayer->Read(ar,this,dwFileVersion_)){
			Clear();
			//
			if(_pModelTreeInterface)
				_pModelTreeInterface->RemoveLayer(pLayer->GetLayerName().c_str());
			return FALSE;
		}
		_layerList.push_back(pLayer);

		if(pLayer->GetID() == nCurLayerId){
			SetCurrLayer(pLayer);
		}
	}

	//��ȡ��β��
	if(!blk.Read(ar,KSTG_BLOCK_LAYERS_END)){
		return FALSE;
	}

	return TRUE;
}

// LAYERS LAYER1 LAYER2... LAYERS_END
//
BOOL  kcModel::WriteLayerData(CArchive& ar)
{
	QxStgBlock blk;

	blk.Initialize(KSTG_BLOCK_LAYERS);
	blk.AddCodeValue(KSTG_CV_CODE_BLOCK_TAG,1);//tag = 1

	// ͼ�����
	int nbLayer = (int)_layerList.size();
	blk.AddCodeValue(1,nbLayer);
	//��ǰͼ��
	int nId = _pCurLayer->GetID();
	blk.AddCodeValue(2,nId);
	
	blk.Write(ar);

	//����д��ÿ��ͼ��
	kcLayer *pLayer = NULL;
	kcLayerList::iterator it = _layerList.begin();
	for(;it != _layerList.end();it ++){
		(*it)->Write(ar);
	}

	//д�������
	blk.Initialize(KSTG_BLOCK_LAYERS_END);
	blk.AddCodeValue(KSTG_CV_CODE_BLOCK_TAG,1);//tag = 1
	blk.Write(ar);

	return TRUE;
}

BOOL  kcModel::ReadBasePlaneData(CArchive& ar)
{
	QxStgBlock blk;

	if(!blk.Read(ar)){
		TRACE("\n ReadBasePlaneData failed.");
		return FALSE;
	}

	int ix,nbCnt = 0,nCurHandle = 0;
	if(blk.GetType() != KSTG_BLOCK_BASEPLANE || 
		!blk.GetValueByCode(1,nCurHandle) ||
		!blk.GetValueByCode(6,nbCnt))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//���ζ�ȡÿ����׼����Ϣ
	Handle(V3d_View) aView = GetActiveView();//;glb_GetCurrV3dView();
	kcBasePlane *pBasePlane = NULL;
	for(ix = 0;ix < nbCnt;ix ++){
		pBasePlane = new kcBasePlane(aView);
		if(!pBasePlane->Read(ar,this)){
			Clear();

			return FALSE;
		}
		AddBasePlane(pBasePlane);

		if(pBasePlane->GetHandle() == nCurHandle){
			SetCurrentBasePlane(pBasePlane);
		}
	}

	return TRUE;
}

BOOL  kcModel::WriteBasePlaneData(CArchive& ar)
{
	QxStgBlock basePlnBlk;
	int nbBasePlane = (int)_basePlaneList.size();

	basePlnBlk.Initialize(KSTG_BLOCK_BASEPLANE);
	int nHandle = _pCurBasePlane->GetHandle();
	basePlnBlk.AddCodeValue(1,nHandle);
	basePlnBlk.AddCodeValue(6,nbBasePlane);

	basePlnBlk.Write(ar);

	//һ��д��ÿ����׼��
	kcBasePlaneList::iterator bpit = _basePlaneList.begin();
	for(;bpit != _basePlaneList.end();bpit ++){
		(*bpit)->Write(ar);
	}

	return TRUE;
}
