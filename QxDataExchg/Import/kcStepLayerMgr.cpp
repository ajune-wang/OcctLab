#include "stdafx.h"
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include "kcStepMbStringMgr.h"
#include "kcStepLayerMgr.h"

//////////////////////////////////////////////////////////////////
//
kcStepLayer::kcStepLayer()
{
	szName_.clear();
	nIndex_ = -1;
}

kcStepLayer::kcStepLayer(const std::string &szName,int idx)
{
	ASSERT(!szName.empty() && (idx >= 0));
	szName_ = szName;
	nIndex_ = idx;
}

void  kcStepLayer::AddShape(const TopoDS_Shape &aS)
{
	if(!aS.IsNull()){
		aShapeList_.Append(aS);
	}
}

//////////////////////////////////////////////////////////////////
//
kcStepLayerMgr::kcStepLayerMgr(void)
{
	pMbStrMgr_ = NULL;
}

kcStepLayerMgr::kcStepLayerMgr(kcStepMbStringMgr *pMbStrMgr)
{
	pMbStrMgr_ = pMbStrMgr;
}

kcStepLayerMgr::~kcStepLayerMgr(void)
{
	Clear();
}

bool  kcStepLayerMgr::ReadLayers(const Handle(TDocStd_Document) &aDoc)
{
	ASSERT(pMbStrMgr_ != NULL);
	Handle(XCAFDoc_LayerTool) aLyTool = XCAFDoc_DocumentTool::LayerTool(aDoc->Main());
	Handle(XCAFDoc_ShapeTool) aShapeTool = aLyTool->ShapeTool();

	//��ȡͼ���б�
	TDF_LabelSequence aLyLabs;
	aLyTool->GetLayerLabels(aLyLabs);
	if(aLyLabs.IsEmpty())//û��ͼ��
		return true;

	kcStepLayer *pLayer = NULL;
	TCollection_ExtendedString aExtName;
	std::string szName;
	int ix,nbLayers = aLyLabs.Length();
	for(ix = 1;ix <= nbLayers;ix ++){
		TDF_Label aLayerLab = aLyLabs.Value(ix);
		//��ȡͼ������
		if(!aLyTool->GetLayer(aLayerLab,aExtName)){
			continue;
		}
		//
		if(!pMbStrMgr_->DecodeExtString(aExtName,szName)){
			continue;
		}

		//���һ��ͼ�����
		pLayer = AddLayer(szName,ix);

		//��ȡshape�б�
		TDF_LabelSequence aShpLabs;
		aLyTool->GetShapesOfLayer(aLayerLab,aShpLabs);
		
		TopoDS_Shape aShape;
		int kx,nbShpLab = aShpLabs.Length();
		for(kx = 1;kx <= nbShpLab;kx ++){
			TDF_Label aSLab = aShpLabs.Value(kx);
			//��ȡ��Ӧ�Ķ���
			if(aShapeTool->GetShape(aSLab,aShape)){
				pLayer->AddShape(aShape);
				//����shape��face��ͼ��Ķ�Ӧ��ϵ
				MapFace(aShape,pLayer);
			}
		}
	}

	return true;
}

kcStepLayer*  kcStepLayerMgr::AddLayer(const std::string &szName,int index)
{
	kcStepLayer *pLayer = NULL;
	stdStepLayerMap::iterator ite = aLayerMap_.find(szName);
	if(ite != aLayerMap_.end()){
		pLayer = (*ite).second;
	}else{
		pLayer = new kcStepLayer(szName,index);
		aLayerVec_.push_back(pLayer);
	}

	return pLayer;
}

// ��¼ÿ��Face�Ͷ�Ӧ��ͼ�����
void  kcStepLayerMgr::	MapFace(const TopoDS_Shape &aShape,kcStepLayer *pLayer)
{
	int idx = pLayer->nIndex_;
	TopExp_Explorer exp;

	for(exp.Init(aShape,TopAbs_FACE);exp.More();exp.Next()){
		const TopoDS_Shape &aFace = exp.Current();
		//
		if(!aShapeLayerIdxMap_.Contains(aFace)){
			aShapeLayerIdxMap_.Add(aFace,idx);
		}
	}
}

void  kcStepLayerMgr::	Clear()
{
	aShapeLayerIdxMap_.Clear();
	aLayerMap_.clear();

	stdStepLayerVector::size_type ix,isize = aLayerVec_.size();
	for(ix = 0;ix < isize;ix ++){
		delete aLayerVec_[ix];
	}
	aLayerVec_.clear();
}

// ����face�����Ҷ�Ӧ��ͼ����Ϣ
kcStepLayer*  kcStepLayerMgr::FindLayer(const TopoDS_Shape &aFace)
{
	kcStepLayer *pLayer = NULL;
	int nIdx = -1;
	if(aShapeLayerIdxMap_.FindFromKey(aFace,nIdx)){
		stdStepLayerVector::size_type ix,isize = aLayerVec_.size();
		for(ix = 0;ix < isize;ix ++){
			if(aLayerVec_[ix]->nIndex_ == nIdx){
				pLayer = aLayerVec_[ix];
				break;
			}
		}
	}
	return pLayer;
}