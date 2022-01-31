//////////////////////////////////////////////////////////////////////////
// //ѡ�񼯹���
#ifndef _KI_SELSET_H_
#define _KI_SELSET_H_

#include <vector>
#include <list>
#include <Quantity_NameOfColor.hxx>
#include "QxLibDef.h"

class kcEntity;
class kiSelEntity;
class kcModel;

// ����ֱ��ʹ�ö��󣬲���ָ��
typedef std::list<kiSelEntity>    kiSelEntityList;
typedef std::vector<kiSelEntity>  kiSelEntityArray;

//һ��ѡ���Ŀǰ֧�֣�
// ʰȡһ����Ȼ���˶���
// ʰȡһ���ֲ��������磺ʵ���е�һ����
//
class QXLIB_API kiSelEntity{
public:
	kiSelEntity();
	kiSelEntity(kcEntity *pEntity,const Handle(AIS_InteractiveObject)& hObj);
	kiSelEntity(const kiSelEntity& other);

	bool					IsValid() const;//�Ƿ���Ч��ʰȡ��

	void					Set(kcEntity *pEntity,const Handle(AIS_InteractiveObject)& hObj);
	void					SetEntityOwner(const Handle(SelectMgr_EntityOwner) &aOwner);
	void					SetLocalShape(const TopoDS_Shape &aSLoc);

	kcEntity*				Entity() const { return _pEntity; }
	const Handle(AIS_InteractiveObject)  AISObject() const { return _aAISObj;}
	bool					IsLocalShape() const;
	const Handle(SelectMgr_EntityOwner)  EntityOwner() const { return _aEntOwner;}
	const TopoDS_Shape		LocalShape() const { return _aLocShape; }

	//kcEntity��Ӧ��shape����
	const TopoDS_Shape		EntityShape() const;

	//ʰȡ�Ķ�����Ȼ�����ֲ���������Ǿֲ��������Ǿֲ������Ӧ��shape����
	const TopoDS_Shape		SelectShape() const;

	kiSelEntity&			operator=(const kiSelEntity& other);
	bool					operator==(const kiSelEntity& other);

public:
	// ѡ�ж����Ӧ����Ȼ����������Ǿֲ�����ѡ�еľ�������
	kcEntity						*_pEntity;

protected:
	//���˶����Ӧ��AIS��ʾ����_pEntity,_aAISObjͨ����Ϊ��
	Handle(AIS_InteractiveObject)	_aAISObj;
	//ѡ�ж����owner,�Ծֲ������Ǿֲ������owner����ȫ�ֶ�����ȫ�ֶ����owner��
	Handle(SelectMgr_EntityOwner)	_aEntOwner;
	//�Ƿ�ֲ�����,�����˶����е��Ӷ������磺ʵ���е����
	bool							_bLocalShape;
	TopoDS_Shape					_aLocShape;//ѡ�оֲ������shape���󣬶Ծֲ�����Ϊ�գ�����Ϊ��
};

// һ��ѡ��
// ��¼һϵ��ѡ����󣬲����������ʾ
//
class QXLIB_API kiSelSet
{
public:
	//kiSelSet(void);
	kiSelSet(kcModel *pModel);
	~kiSelSet(void);

	//��ȡѡ����,ͨ���������˶����ѡ��
	bool					GetSelected();
	void					Clear();

	//���ѡ����
	bool					AddSelected(const Handle(AIS_InteractiveObject)& aisObj,BOOL bUpdateView = FALSE);
	bool					RemoveSelected(const Handle(AIS_InteractiveObject)& aisObj,BOOL bUpdateView = FALSE);
	bool					AddSelected(const kiSelEntity& selEnt,BOOL bUpdateView = FALSE);
	bool					AddSelected(kiSelSet& selSet,BOOL bUpdateView = FALSE);

	//�����Ͳ�������ʾ
	bool					Hilight(BOOL bUpdateView = TRUE);//������ʾ��ʹ��Ĭ����ɫ
	bool					Hilight(const Quantity_NameOfColor hicol,BOOL bUpdateView = TRUE);
	void					UnHilight(BOOL bUpdateView = TRUE);

	bool					IsEmpty() const;
	int					GetSelCount();
	void					InitSelected();
	bool					MoreSelected();
	void					NextSelected();
	kiSelEntity			CurSelected();
	//
	kiSelEntity			GetFirstSelected() const;
	kiSelEntity			GetSelEntity(int idx) const;

public:
	Handle(AIS_InteractiveContext)	m_hAISCtx;
	kcModel				*pModel_;//ģ�Ͷ���
	kiSelEntityList		m_selEntList;//ѡ������б�

protected:
	kiSelEntityList::iterator	m_iteList;
};



#endif
