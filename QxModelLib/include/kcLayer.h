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
	void						Clear();
	void						Destroy();

	kcModel*					GetModel() const { return _pModel; }

	//��� ɾ��ͼԪ,���ڿ��ܷ���undo�У���Ϣ�������ƻ���
	BOOL						AddEntity(kcEntity *pEntity);
	kcEntity*				DelEntity(kcEntity *pEntity);
	BOOL						FindEntity(kcEntity *pEntity);
	kcEntity*				FindEntity(const char *pszName);//�������ƻ�ȡ

	void						GetAllEntity(kcEntityList& entList);
	void						GetAllEntity(int entType,kcEntityList& entList);

	//��ʾ���ж���
	BOOL						ShowAll(BOOL bUpdateView = FALSE);
	// ��ʾ�ض����͵Ķ���
	BOOL						FilterDisplay(int nTypeFlag,BOOL bUpdateView = FALSE);

	int						GetHandle() const { return _nHandle; }
	//��ȡͼ������
	const std::string&		GetLayerName() const { return _strName; }
	bool						HasName(const char *pszName);

public:
	void						SetColor(double r,double g,double b,BOOL bUpdateView = TRUE);
	void						SetLineStyle(eLineStyle eStyle,BOOL bUpdateView = TRUE);
	void						SetLineWidth(double width,BOOL bUpdateView = TRUE);

public:
	//�����Զ���׽��
	BOOL						CalcSnapPoint(kiSnapMngr *pSnapMgr);

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
	kcModel					*_pModel;
	int						_nHandle;//���
	std::string				_strName;
	kcEntityList				_entityList;

	friend class kcModel;
};

#endif
