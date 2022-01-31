#include "StdAfx.h"
#include "kcEntity.h"
#include "kcModel.h"
#include "AIS_EntityShape.h"
#include "kiSelSet.h"

//һ��ѡ����
kiSelEntity::kiSelEntity()
{
	_pEntity = NULL;
	_bLocalShape = false;
}

kiSelEntity::kiSelEntity(kcEntity *pEntity,const Handle(AIS_InteractiveObject)& hObj)
{
	ASSERT(pEntity);
	ASSERT(!hObj.IsNull());
	_pEntity = pEntity;
	_bLocalShape = false;
	_aAISObj = hObj;
}

kiSelEntity::kiSelEntity(const kiSelEntity& other)
{
	_pEntity = NULL;
	*this = other;
}

//�Ƿ���Ч��ʰȡ��
bool  kiSelEntity::IsValid() const
{
	if(_pEntity == NULL || _aAISObj.IsNull() || _aEntOwner.IsNull())
		return false;
	
	if(_bLocalShape && _aLocShape.IsNull())
		return false;

	return true;
}

void  kiSelEntity::Set(kcEntity *pEntity,const Handle(AIS_InteractiveObject)& hObj)
{
	ASSERT(pEntity);
	ASSERT(!hObj.IsNull());
	_pEntity = pEntity;
	_aAISObj = hObj;
}

void  kiSelEntity::SetEntityOwner(const Handle(SelectMgr_EntityOwner) &aOwner)
{
	ASSERT(!aOwner.IsNull());
	_aEntOwner = aOwner;
}

void  kiSelEntity::SetLocalShape(const TopoDS_Shape &aSLoc)
{
	ASSERT(!aSLoc.IsNull());
	_bLocalShape = true;
	_aLocShape = aSLoc;
}

kiSelEntity&  kiSelEntity::operator=(const kiSelEntity& other)
{
	_pEntity = other.Entity();
	_aAISObj = other.AISObject();
	_aEntOwner = other.EntityOwner();
	_bLocalShape = other.IsLocalShape();
	if(other.IsLocalShape()){
		_aLocShape = other.LocalShape();
	}

	return *this;
}

bool  kiSelEntity::IsLocalShape() const
{
	return _bLocalShape;
}

const TopoDS_Shape  kiSelEntity::EntityShape() const
{
	TopoDS_Shape aS;
	if(_pEntity != NULL){
		aS = _pEntity->GetShape();
	}
	return aS;
}

//ʰȡ�Ķ�����Ȼ�����ֲ�����
const TopoDS_Shape  kiSelEntity::SelectShape() const
{
	TopoDS_Shape aS = EntityShape();
	if(_bLocalShape){
		aS = _aLocShape;
	}
	return aS;
}

bool  kiSelEntity::operator==(const kiSelEntity& other)
{
	if(_pEntity == other.Entity() && 
		_aAISObj == other.AISObject() &&
		(_aEntOwner == other.EntityOwner()) &&
		_bLocalShape == other.IsLocalShape())
	{
		if(_bLocalShape){//�ֲ�����
			if(_aLocShape.IsSame(other.LocalShape()))
				return true;
		}else{
			return true;
		}
	}
	
	return false;
}

//////////////////////////////////////////////////////////////////////////
//
//kiSelSet::kiSelSet(void)
//{
//	m_hAISCtx = glb_GetAISContext();
//	ASSERT(!m_hAISCtx.IsNull());
//	pModel_ = NULL;
//}

kiSelSet::kiSelSet(kcModel *pModel)
{
	ASSERT(pModel);
	pModel_ = pModel;
	m_hAISCtx = pModel->GetAISContext();
}

kiSelSet::~kiSelSet(void)
{
	Clear();
}

// ��Ctx�л�ȡѡ����.������Ҫ������Ȼ�����ѡ��û�о����ʰȡģʽ
//
bool	kiSelSet::GetSelected()
{
	//����ɵġ�
	m_selEntList.clear();
	//
	if(m_hAISCtx.IsNull())
		return false;

	kcEntity *pEntity = NULL;
	Handle(AIS_InteractiveObject) hObj;

	m_hAISCtx->InitSelected();
	while(m_hAISCtx->MoreSelected()){
		hObj = m_hAISCtx->SelectedInteractive();
		//
		if(!hObj.IsNull()){
			pEntity = pModel_->GetEntity(hObj);
			if(pEntity){
				kiSelEntity se(pEntity,hObj);
				se.SetEntityOwner(m_hAISCtx->SelectedOwner());
				m_selEntList.push_back(se);
			}
		}

		m_hAISCtx->NextSelected();
	}

	return true;
}

//���ѡ�񼯺���ʾ
void  kiSelSet::Clear()
{
	if(!m_selEntList.empty()){
		m_selEntList.clear();
	}

	//if(!m_hAISCtx.IsNull()){
	//	m_hAISCtx->ClearSelected(true);
	//}
}

// ���һ������ѡ��
// ���ͨ���������Ȼ����
//
bool  kiSelSet::AddSelected(const Handle(AIS_InteractiveObject)& aisObj,BOOL bUpdateView)
{
	if(m_hAISCtx.IsNull() || aisObj.IsNull())	return false;

	//���û��ѡ�У���ӵ�ѡ����
	if(!m_hAISCtx->IsSelected(aisObj)){
		m_hAISCtx->AddOrRemoveSelected(aisObj,bUpdateView ? true : false);
	}

	//��ӵ��б���
	kcEntity *pEntity = ksGetAisShapeEntity(aisObj);
	if(pEntity){
		kiSelEntityList::iterator it = m_selEntList.begin();
		for(;it != m_selEntList.end();it ++){
			if((*it).Entity() == pEntity){//�Ѿ����
				return true;
			}
		}
		kiSelEntity se(pEntity,aisObj);
		se.SetEntityOwner(aisObj->GlobalSelOwner());

		m_selEntList.push_back(se);
	}
	
	return true;
}

bool	kiSelSet::RemoveSelected(const Handle(AIS_InteractiveObject)& aisObj,BOOL bUpdateView)
{
	if(m_hAISCtx.IsNull() || aisObj.IsNull()) return false;

	//�����ѡ���У�ȥѡ��
	if(m_hAISCtx->IsSelected(aisObj)){
		m_hAISCtx->AddOrRemoveSelected(aisObj,bUpdateView ? true : false);
	}

	//��list��ɾ��
	kcEntity *pEntity = ksGetAisShapeEntity(aisObj);
	if(pEntity){
		kiSelEntityList::iterator it = m_selEntList.begin();
		for(;it != m_selEntList.end();it ++){
			if((*it).Entity() == pEntity)//�Ѿ����
				break;
		}
		if(it != m_selEntList.end())
			m_selEntList.erase(it);
	}
	
	return true;
}

// ���ѡ����
// ע�⣺������Ȼ����;ֲ�����Ķ�����owner����
//
bool  kiSelSet::AddSelected(const kiSelEntity& selEnt,BOOL bUpdateView)
{
	if(!selEnt.IsValid()){
		ASSERT(FALSE);
		return false;
	}

	//�ֲ���ȫ�ֶ��󶼻���owner����
	if(!m_hAISCtx->IsSelected(selEnt.EntityOwner())){
		m_hAISCtx->AddOrRemoveSelected(selEnt.EntityOwner(),bUpdateView ? true : false);
	}
	
	kiSelEntityList::iterator it = m_selEntList.begin();
	for(;it != m_selEntList.end();it ++)
	{
		if((*it) == selEnt )//�Ѿ����
			return true;
	}
	m_selEntList.push_back(selEnt);
	
	return true;
}

bool	kiSelSet::AddSelected(kiSelSet& selSet,BOOL bUpdateView)
{
	selSet.InitSelected();
	while(selSet.MoreSelected())
	{
		AddSelected(selSet.CurSelected(),FALSE);
		selSet.NextSelected();
	}

	if(bUpdateView){
		m_hAISCtx->UpdateCurrentViewer();
	}

	return true;
}

//�����Ͳ�������ʾ
bool  kiSelSet::Hilight(BOOL bUpdateView)
{
	if(m_selEntList.empty() || m_hAISCtx.IsNull())
		return false;

	//�������û����ѡ���У���Ҫ��Ӷ���
	kiSelEntityList::iterator it = m_selEntList.begin();
	for(;it != m_selEntList.end();it ++){
		//ȫ�ֻ�ֲ�����Ĵ�������owner
		Handle(SelectMgr_EntityOwner) aOwner = (*it).EntityOwner();
		if(!aOwner.IsNull() && !m_hAISCtx->IsSelected(aOwner)){
			m_hAISCtx->AddOrRemoveSelected(aOwner,false);
		}
	}
	
	//������ʾ
	m_hAISCtx->HilightSelected(bUpdateView ? true : false);

	return true;
}

//�����Ͳ�������ʾ
bool	kiSelSet::Hilight(const Quantity_NameOfColor hicol,BOOL bUpdateView)
{
	if(m_selEntList.empty() || m_hAISCtx.IsNull())
		return false;

	//�������û����ѡ���У���Ҫ��Ӷ���
	kiSelEntityList::iterator it = m_selEntList.begin();
	for(;it != m_selEntList.end();it ++){
		//ȫ�ֻ�ֲ�����Ĵ�������owner
		Handle(SelectMgr_EntityOwner) aOwner = (*it).EntityOwner();
		if(!aOwner.IsNull() && !m_hAISCtx->IsSelected(aOwner)){
			m_hAISCtx->AddOrRemoveSelected(aOwner,false);
		}
	}
	
	//������ʾ
	m_hAISCtx->HilightSelected(bUpdateView ? true : false);

	return true;
}

void  kiSelSet::UnHilight(BOOL bUpdateView)
{
	if(m_selEntList.empty() || m_hAISCtx.IsNull())
		return;

	m_hAISCtx->UnhilightSelected(bUpdateView ? true : false);
}

bool	kiSelSet::IsEmpty() const
{
	return m_selEntList.empty();
}

int		kiSelSet::GetSelCount()
{
	int nCount = 0;
	kiSelEntityList::iterator it = m_selEntList.begin();
	for(;it != m_selEntList.end();it ++)
		nCount ++;
	return nCount;
}

void  kiSelSet::InitSelected()
{
	m_iteList = m_selEntList.begin();
}

bool  kiSelSet::MoreSelected()
{
	return (m_iteList != m_selEntList.end() ? true : false);
}
void  kiSelSet::NextSelected()
{
	m_iteList ++;
}

kiSelEntity  kiSelSet::CurSelected()
{
	return (*m_iteList);
}

kiSelEntity kiSelSet::GetFirstSelected() const
{
	ASSERT(!m_selEntList.empty());
	return m_selEntList.front(); 
}

kiSelEntity kiSelSet::GetSelEntity(int idx) const
{
	kiSelEntity se;
	kiSelEntityList::const_iterator ite = m_selEntList.begin();
	int ix = 0;
	for(;ite != m_selEntList.end();++ite,ix++){
		if(ix == idx){
			se = (*ite);
			break;
		}
	}
	return se;
}

////////////////////////////////////////////////////////
