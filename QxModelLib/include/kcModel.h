//////////////////////////////////////////////////////////////////////////
// �������е�ͼԪ��Ϣ.
#ifndef _KC_MODEL_H_
#define _KC_MODEL_H_

#include <vector>
#include <list>
#include <map>
#include <string>

#include <V3d_View.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>

#include "QxLibDef.h"

class CArchive;
class kiSnapMngr;
class kiUndoMgr;
class kcEntity;
class kcLayer;
class kiNewDelUndoItem;
class kiSelSet;
class kcHandleMgr;
class kuiModelTreeInterface;
class kcBasePlane;
class kcShapeAttribMgr;
class kcSnapMngr;

typedef std::list<kcEntity *>  kcEntityList;
typedef std::vector<kcEntity *> kcEntityArray;
typedef std::list<kcLayer *>	kcLayerList;
typedef std::list<kcBasePlane *>	kcBasePlaneList;

// ģ�Ͷ��󣬼�¼��ģ����ص�������Ϣ
//
class QXLIB_API kcModel
{
public:
	//Model���Ƕ�һ��AIS_Ctx�ķ�װ����AIS_Ctx����һһ��Ӧ�Ĺ�ϵ
	kcModel(const Handle(AIS_InteractiveContext)& aCtx);
	~kcModel(void);

	//��ʼ��
	BOOL					Initialize();
	//���ģ��
	void					Clear();
	//�ͷ�����
	void					Destroy();

	BOOL					CreateDefaultLayer();
	BOOL					CreateDefaultBasePlane(const Handle(V3d_View)& aView);

public:
	void					SetModelTreeInterface(kuiModelTreeInterface *pModelTreeInterface);
	kuiModelTreeInterface*	GetModelTreeInterface() const {  return _pModelTreeInterface; }

	//BOOL					SetAISContext(const Handle(AIS_InteractiveContext)& aCtx);
	const Handle(AIS_InteractiveContext)& GetAISContext() { return _hAISContext; }

	BOOL					SetActiveView(const Handle(V3d_View)& aView);
	const Handle(V3d_View)& GetActiveView() const { return aActiveView_; }

	kcHandleMgr*			GetHandleMgr() const { return _pHandleMgr; }
	kcShapeAttribMgr*		GetShapeAttribMgr() const { return _pShapeAttribMgr; }

	kiSelSet*			GetGlobalSelSet() const;

	void					SetSnapManager(kcSnapMngr *pMngr);
	kcSnapMngr*			GetSnapManager() const;

public://ͼ�����
	//������ɾ��ͼ��
	kcLayer*				CreateNewLayer();
	kcLayer*				CreateLayer(const char* szLayerName);
	BOOL					DeleteLayer(kcLayer *pLayer);
	kcLayer*				DeleteLayer(const char* szLayerName);
	kcLayer*				FindLayer(const char* szLayerName);
	BOOL					SetCurrLayer(const char* szLayerName);
	BOOL					SetCurrLayer(kcLayer *pLayer);
	kcLayer*				GetCurrLayer() const;

	const kcLayerList&	GetLayerList() const;
	int					GetLayerCount() const;

public:
	//��Ӻ�ɾ��ͼԪ
	BOOL					AddEntity(kcEntity *pEntity,BOOL bUpdateView = FALSE);
	BOOL					AddEntity(const std::vector<kcEntity *>& aEnt,BOOL bUpdateView = FALSE);
	BOOL					AddEntity(kcEntity *pEntity,kcLayer *pLayer,BOOL bUpdateView = FALSE);
	BOOL					DelEntity(kcEntity *pEntity,BOOL bUpdateView = FALSE);
	kcEntity*			DelEntity(Handle(AIS_InteractiveObject)& aObj,BOOL bUpdateView = FALSE);
	kcEntity*			GetEntity(Handle(AIS_InteractiveObject)& aObj);

	void					GetAllEntity(kcEntityList& entList);
	void					GetAllEntity(int entType,kcEntityList& entList);

public://��׼�����
	BOOL					AddBasePlane(kcBasePlane *pBasePlane);
	BOOL					SetCurrentBasePlane(kcBasePlane *pBasePlane);
	kcBasePlane*			GetCurrentBasePlane() const;
	// ��ʾ���Ի�׼���Grid
	void					ShowAllBasePlaneGrid(bool bShow);
	
public:
	//��ʾ���ж���
	BOOL					ShowAll(BOOL bUpdateView = FALSE);
	// ��ʾ�ض����͵Ķ���
	BOOL					FilterDisplay(int nTypeFlag,BOOL bUpdateView = FALSE);

public:
	//shape���Թ���
	BOOL					AddShapeAttrib(const TopoDS_Shape &aS,const Quantity_Color &aCol);
	//������ɫ����
	BOOL					FindShapeAttrib(const TopoDS_Shape &aS,Quantity_Color &aCol);
	//
	void					ClearAllShapeAttrib();

public:
	//��undo��ؽӿڡ���ʼ�ͽ���undo�������������ɶԵ��á���������������
	//AddEntity��DelEntity���Զ������ɵ�Undo��档
	//
	BOOL					BeginUndo(const char *pszUndoName);
	BOOL					EndUndo();

public:
	kiUndoMgr*			GetUndoMgr();

	//�������в�׽��
	BOOL					CalcSnapPoint(kiSnapMngr *pSnapMgr);

public:
	// ����Ͷ�ȡ,dwFileVersion:�Ƕ�ȡ�ļ��İ汾
	BOOL					Read(CArchive& ar,UINT dwFileVersion);
	BOOL					Write(CArchive& ar);

protected:
	BOOL					ReadHandleMgr(CArchive& ar);
	BOOL					WriteHandleMgr(CArchive& ar);
	BOOL					ReadLayerData(CArchive& ar);
	BOOL					WriteLayerData(CArchive& ar);
	BOOL					ReadBasePlaneData(CArchive& ar);
	BOOL					WriteBasePlaneData(CArchive& ar);
	
protected://ͼ�㱣�������
	kcHandleMgr					*_pHandleMgr;//handle����
	kcLayerList					_layerList;//�������е�ͼ����Ϣ
	kcLayer						*_pCurLayer;//��ǰͼ��

	kcShapeAttribMgr				*_pShapeAttribMgr;//�������Թ���

	kcBasePlaneList				_basePlaneList;//��׼���б�
	kcBasePlane					*_pCurBasePlane;//��ǰ��õĻ�׼��

protected://Undo����
	kiUndoMgr					*_pUndoMgr;//undo������
	kiNewDelUndoItem				*_pCurrUndoItem;//��ǰ��UndoItem��

protected:
	//ȫ��ѡ�񼯹������,ѡ���Ƕ�AIS_CTX����ķ�װ��Ŀǰ���ֺ����һһ��Ӧ��ϵ
	kiSelSet						*pGlobalSelSet_;

protected:
	// �����¼Doc��Ӧ��Context����
	Handle(AIS_InteractiveContext)	_hAISContext;
	kuiModelTreeInterface			*_pModelTreeInterface;
	Handle(V3d_View)				aActiveView_;//��ǰ���view
	kcSnapMngr					*pSnapMngr_;// �����¼�����ⲿ��������ָ��

protected:
	UINT							dwFileVersion_;//�����ļ��İ汾�ţ�����ģ�ʹ��ļ��ж�ȡʹ��
};

inline kiUndoMgr*	kcModel::GetUndoMgr()
{
	ASSERT(_pUndoMgr);
	return _pUndoMgr;
}

#endif