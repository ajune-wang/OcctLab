//////////////////////////////////////////////////////////////////////////
// ����һ��ͼ�������ͼԪ
#ifndef _KC_LAYER_H_
#define _KC_LAYER_H_

#include <list>
#include <string>

#include "kcEntity.h"
#include "QxLibDef.h"

class CArchive;
class kiSnapMngr;
class kcModel;
class kcEntity;

#define  KCT_ENT_LAYER			0x10000  //�����Entity�ĳ�ͻ

class QXLIB_API kcLayer
{
public:
	kcLayer();
	kcLayer(const char *pszName);
	~kcLayer(void);

	int						Initialize(kcModel *pModel);
	void					Clear();
	void					Destroy();

	kcModel*				GetModel() const { return pModel_; }

	//��� ɾ��ͼԪ,���ڿ��ܷ���undo�У���Ϣ�������ƻ���
	BOOL					AddEntity(kcEntity *pEntity);
	kcEntity*				DelEntity(kcEntity *pEntity);
	BOOL					FindEntity(kcEntity *pEntity);
	kcEntity*				FindEntity(const char *pszName);//�������ƻ�ȡ

	void					GetAllEntity(kcEntityList& entList);
	void					GetAllEntity(int entType,kcEntityList& entList);

	//��ʾ���ж���
	BOOL					ShowAll(BOOL bUpdateView = FALSE);
	// ��ʾ�ض����͵Ķ���
	BOOL					FilterDisplay(int nTypeFlag,BOOL bUpdateView = FALSE);

	int						GetID() const { return nId_; }
	//��ȡͼ������
	const std::string&		GetLayerName() const { return sName_; }
	bool					HasName(const char *pszName);

public:
	void					SetColor(double r,double g,double b,BOOL bUpdateView = TRUE);
	void					SetLineStyle(eLineStyle eStyle,BOOL bUpdateView = TRUE);
	void					SetLineWidth(double width,BOOL bUpdateView = TRUE);

public:
	//�����Զ���׽��
	BOOL					CalcSnapPoint(kiSnapMngr *pSnapMgr);

public:
	// ��ȡ
	BOOL						Read(CArchive& ar,kcModel *pModel,UINT dwFileVer);
	// д��
	BOOL						Write(CArchive& ar);

protected:
	void						GenHandleAndName();
	// ��ȡentity
	BOOL						ReadEntity(CArchive& ar);

	//
	void						UpdateCurrentViewer();

protected:
	kcModel					*pModel_;
	int						nId_;//���,Ψһ��Id
	std::string				sName_;
	kcEntityList			aEntList_;

	friend class kcModel;
};

#endif
