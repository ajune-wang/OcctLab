#include "StdAfx.h"
#include "kiSnapMngr.h"
#include "kcEntity.h"
#include "kcEntityFactory.h"
#include "kcModel.h"
#include "kcHandleMgr.h"
#include "QxStgBlock.h"
#include "QxStgModelDef.h"
#include "kuiInterface.h"
#include "kcLayer.h"

kcLayer::kcLayer()
{
	pModel_ = NULL;
	sName_ = "";
	nId_ = 0;//��Ч���
}

kcLayer::kcLayer(const char *pszName)
{
	pModel_ = NULL;
	sName_ = pszName;
	nId_ = 0;
}

kcLayer::~kcLayer(void)
{
	Destroy();
}

int kcLayer::Initialize(kcModel *pModel)
{
	ASSERT(pModel);
	pModel_ = pModel;

	GenHandleAndName();

	return 1;
}

void kcLayer::GenHandleAndName()
{
	ASSERT(pModel_);
	//��ȡ���
	kcHandleMgr *pHandleMgr = pModel_->GetHandleMgr();
	ASSERT(pHandleMgr);
	nId_ = pHandleMgr->AllocHandle(KCT_ENT_LAYER);
	ASSERT(KC_IS_VALID_HANDLE(nId_));

	if(sName_.empty()){
		char cbuf[64];
		int len = sprintf_s(cbuf,64,"Layer %d",nId_);
		cbuf[len] = '\0';
		sName_ = cbuf;
	}
}

void kcLayer::Clear()
{
	kcEntityList::iterator it;
	for(it = aEntList_.begin();it != aEntList_.end();it ++){
		(*it)->Destroy();
		delete *it;
	}
	aEntList_.clear();
}

void kcLayer::Destroy()
{
	// �ͷž��
	kcHandleMgr *pHandleMgr = pModel_->GetHandleMgr();
	ASSERT(pHandleMgr);
	pHandleMgr->FreeHandle(KCT_ENT_LAYER,nId_);
	nId_ = 0;

	Clear();
}

// ���ͼԪ
// �������´�����ͼԪ��������undoʱ������ӵ�ͼԪ,Ҳ������
//
BOOL kcLayer::AddEntity(kcEntity *pEntity)
{
	ASSERT(pEntity);
	ASSERT(!FindEntity(pEntity));//���費�ܴ���
	if(!pEntity)
		return FALSE;

	//����ͼ��
	kcLayer *pLayer = pEntity->GetLayer();
	if(pLayer == NULL)
		pEntity->SetLayer(this);
	else
	{//Ӧ����undo��ӵ�
		ASSERT(pLayer == this);
	}
	aEntList_.push_back(pEntity);

	//��ӵ�modeltree��
	kuiModelTreeInterface *pModelTreeIF = pModel_->GetModelTreeInterface();
	if(pModelTreeIF){
		pModelTreeIF->AddEntity(sName_.c_str(),pEntity->GetName(),pEntity);
	}

	return TRUE;
}

// ɾ��ͼԪ
// ͨ����������undo���У���ˣ����������ƻ����ڲ����ݡ�
//
kcEntity* kcLayer::DelEntity(kcEntity *pEntity)
{
	BOOL bFound = FALSE;
	kcEntityList::iterator it;
	for(it = aEntList_.begin();it != aEntList_.end();it ++){
		if(*it == pEntity){
			aEntList_.erase(it);
			pEntity->SetLayer(NULL);
			//��modeltree��ɾ��
			kuiModelTreeInterface *pModelTreeIF = pModel_->GetModelTreeInterface();
			if(pModelTreeIF){
				pModelTreeIF->RemoveEntity(sName_.c_str(),pEntity->GetName());
			}

			bFound = TRUE;
			break;
		}
	}
	
	return bFound ? pEntity : NULL;
}

BOOL kcLayer::FindEntity(kcEntity *pEntity)
{
	BOOL bFound = FALSE;
	kcEntityList::iterator it;
	for(it = aEntList_.begin();it != aEntList_.end();it ++){
		if(*it == pEntity){
			bFound = TRUE;
			break;
		}
	}
	return bFound;
}

kcEntity*  kcLayer::FindEntity(const char *pszName)//�������ƻ�ȡ
{
	std::string sName(pszName),snam;
	if(!sName.empty()){
		kcEntityList::iterator ite;
		for(ite = aEntList_.begin();ite != aEntList_.end();++ite){
			snam = (*ite)->GetName();
			if(snam == sName){
				return (*ite);
			}
		}
	}
	return NULL;
}

void kcLayer::GetAllEntity(kcEntityList& entList)
{
	kcEntityList::iterator it;
	for(it = aEntList_.begin();it != aEntList_.end();it ++){
		entList.push_back(*it);
	}
}

void kcLayer::GetAllEntity(int entType,kcEntityList& entList)
{
	kcEntityList::iterator it;
	for(it = aEntList_.begin();it != aEntList_.end();it ++){
		if((*it)->GetType() == entType)
			entList.push_back(*it);
	}
}

BOOL kcLayer::ShowAll(BOOL bUpdateView)
{
	kuiModelTreeInterface *pModelTreeIF = pModel_->GetModelTreeInterface();
	kcEntityList::iterator it;

	for(it = aEntList_.begin();it != aEntList_.end();it ++){
		(*it)->Display(true,bUpdateView);
		//
		if(pModelTreeIF){
			pModelTreeIF->ShowEntity((*it)->GetName(),TRUE);
		}
	}
	return TRUE;
}

// ��ʾ�ض����͵Ķ���
BOOL kcLayer::FilterDisplay(int nTypeFlag,BOOL bUpdateView)
{
	kcEntityList::iterator it;
	kcEntity *pEnt = NULL;
	for(it = aEntList_.begin();it != aEntList_.end();it ++){
		pEnt = (*it);
		int typ = pEnt->GetType();
		//����ȡ������
		typ = (typ >> 8) << 8;
		if(typ & nTypeFlag)
			(*it)->Display(true,bUpdateView);
		else
			(*it)->Display(false,bUpdateView);
	}

	return TRUE;
}

bool kcLayer::HasName(const char *pszName)
{
	std::string ssName = pszName;
	if(ssName != sName_)
		return false;
	return true;
}

void kcLayer::SetColor(double r,double g,double b,BOOL bUpdateView)
{
	kcEntityList::iterator it;
	kcEntity *pEnt = NULL;
	for(it = aEntList_.begin();it != aEntList_.end();it ++){
		pEnt = (*it);
		pEnt->SetColor(r,g,b,FALSE);
	}
	//ͳһ����
	if(bUpdateView){
		UpdateCurrentViewer();
	}
}

void kcLayer::SetLineStyle(eLineStyle eStyle,BOOL bUpdateView)
{
	kcEntityList::iterator it;
	kcEntity *pEnt = NULL;
	for(it = aEntList_.begin();it != aEntList_.end();it ++){
		pEnt = (*it);
		pEnt->SetLineStyle(eStyle,FALSE);
	}
	//ͳһ����
	if(bUpdateView){
		UpdateCurrentViewer();
	}
}

void kcLayer::SetLineWidth(double width,BOOL bUpdateView)
{
	kcEntityList::iterator it;
	kcEntity *pEnt = NULL;
	for(it = aEntList_.begin();it != aEntList_.end();it ++){
		pEnt = (*it);
		pEnt->SetLineWidth(width,FALSE);
	}
	//ͳһ����
	if(bUpdateView){
		UpdateCurrentViewer();
	}
}


//�����Զ���׽��
BOOL kcLayer::CalcSnapPoint(kiSnapMngr *pSnapMgr)
{
	if(aEntList_.empty())
		return TRUE;

	kcEntityList::iterator it;
	for(it = aEntList_.begin();it != aEntList_.end();it ++){
		(*it)->CalcSnapPoint(*pSnapMgr);
	}
	return TRUE;
}

// ��ȡ
BOOL kcLayer::Read(CArchive& ar,kcModel *pModel,UINT dwFileVer)
{
	ASSERT(ar.IsLoading());
	ASSERT(pModel);
	dwFileVer;
	
	pModel_ = pModel;

	QxStgBlock blk;
	if(!blk.Read(ar,KSTG_BLOCK_LAYER)){
		ASSERT(FALSE);
		return FALSE;
	}

	int nbChild = 0;
	if(!blk.GetValueByCode(KSTG_CV_CODE_HANDLE,nId_) ||
		!blk.GetValueByCode(KSTG_CV_CODE_NAME,sName_) ||
		!blk.GetValueByCode(7,nbChild))
	{
		ASSERT(FALSE);
	}
	//����Ѿ�ʹ����
	kcHandleMgr *pHandleMgr = pModel_->GetHandleMgr();
	pHandleMgr->MaskHandle(KCT_ENT_LAYER,nId_);

	//��ӵ�modeltree
	kuiModelTreeInterface *pModelTreeIF = pModel_->GetModelTreeInterface();
	if(pModelTreeIF){
		pModelTreeIF->AddLayer(sName_.c_str(),this);
	}

	int idx;
	for(idx = 0;idx < nbChild;idx ++){
		if(!ReadEntity(ar)){
			Clear();
			return FALSE;
		}
	}

	//����β��
	if(!blk.Read(ar,KSTG_BLOCK_LAYER_END)){
		return FALSE;
	}
	
	return TRUE;
}

// ��ȡentity
BOOL kcLayer::ReadEntity(CArchive& ar)
{
	kcEntity *pEntity = kcEntity::ReadEntity(ar);
	if(!pEntity)
		return FALSE;

	kcHandleMgr *pHandleMgr = pModel_->GetHandleMgr();
	pHandleMgr->MaskHandle(pEntity->GetType(),pEntity->GetID());

	AddEntity(pEntity);

	//��ʾ
	pEntity->Display(true,FALSE);

	return TRUE;
}

// д��
BOOL kcLayer::Write(CArchive& ar)
{
	ASSERT(ar.IsStoring());

	QxStgBlock blk;

	//д��layer����
	blk.Initialize(KSTG_BLOCK_LAYER);
	blk.AddCodeValue(KSTG_CV_CODE_BLOCK_TAG,nId_);//tag == handle

	blk.AddCodeValue(KSTG_CV_CODE_NAME,sName_.c_str());//ͼ������
	blk.AddCodeValue(KSTG_CV_CODE_HANDLE,nId_);//ͼ����
	int nbEnt = (int)aEntList_.size();
	blk.AddCodeValue(7,nbEnt);

	blk.Write(ar);

	kcEntityList::iterator it;
	for(it = aEntList_.begin();it != aEntList_.end();it ++){
		(*it)->Write(ar);
	}

	//д���β��
	blk.Initialize(KSTG_BLOCK_LAYER_END);
	blk.AddCodeValue(KSTG_CV_CODE_BLOCK_TAG,nId_);//tag == handle
	blk.Write(ar);

	return TRUE;
}

void  kcLayer::UpdateCurrentViewer()
{
	if(pModel_ != NULL){
		Handle(AIS_InteractiveContext) aCtx = pModel_->GetAISContext();
		if(!aCtx.IsNull()){
			aCtx->UpdateCurrentViewer();
		}
	}
}