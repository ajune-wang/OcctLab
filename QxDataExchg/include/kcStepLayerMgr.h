#pragma once

#include <string>
#include <vector>
#include <map>
#include <NCollection_IndexedDataMap.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TDocStd_Document.hxx>

class kcStepMbStringMgr;
class kcStepLayer;
typedef std::vector<kcStepLayer *> stdStepLayerVector;
typedef std::map<std::string,kcStepLayer *> stdStepLayerMap;
typedef NCollection_IndexedDataMap<TopoDS_Shape,int,TopTools_ShapeMapHasher> ShapeLayerIdxMap;


//��¼һ��ͼ����Ϣ
class kcStepLayer{
public:
	kcStepLayer();
	kcStepLayer(const std::string &szName,int idx);

	void					AddShape(const TopoDS_Shape &aS);

public:
	std::string			szName_;//ͼ������
	int					nIndex_;//���
	TopTools_ListOfShape	aShapeList_;//ͼ�������shape�б�
};

class kcStepLayerMgr
{
public:
	kcStepLayerMgr(void);
	kcStepLayerMgr(kcStepMbStringMgr *pMbStrMgr);
	~kcStepLayerMgr(void);

	bool					ReadLayers(const Handle(TDocStd_Document) &aDoc);
	void					Clear();

	kcStepLayer*			FindLayer(const TopoDS_Shape &aFace);

protected:
	kcStepLayer*			AddLayer(const std::string &szName,int index);
	//
	void					MapFace(const TopoDS_Shape &aShape,kcStepLayer *pLayer);

protected:
	stdStepLayerVector	aLayerVec_;//ͼ���б�
	stdStepLayerMap		aLayerMap_;//
	ShapeLayerIdxMap		aShapeLayerIdxMap_;

	kcStepMbStringMgr		*pMbStrMgr_;//���ƹ���
};

