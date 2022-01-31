#include "StdAfx.h"
#include "kcEntity.h"
#include "kcEntityFactory.h"
#include "kcModel.h"
#include "kcSnapMngr.h"
#include "kiInputEdit.h"
#include "kiInputTool.h"
#include "kiInputOptionsTool.h"
#include "kvCoordSystem.h"
#include "kcBasePlane.h"
#include "kiSelSet.h"
#include "kcDocContext.h"
#include "QxSysInterface.h"
#include "kiCommandMgr.h"
#include "kiCommand.h"

///////////////////////////////////////////////////////////////////
// ��ʱ��������
class QxCmdTimer : public IQxTimerHandler{
public:
	QxCmdTimer(kiCommand *pCmd);

	virtual int			OnTimer(UINT_PTR nIDEvent,DWORD dwPassTime);

protected:
	kiCommand			*pCommand_;

};

QxCmdTimer::QxCmdTimer(kiCommand *pCmd)
	:pCommand_(pCmd)
{
}

int QxCmdTimer::OnTimer(UINT_PTR nIDEvent,DWORD dwPassTime)
{
	return pCommand_->HandlerTimer(nIDEvent,dwPassTime);
}

///////////////////////////////////////////////////////////////////
//

kiCommand::kiCommand(void)
{
	m_nID = 0;
	pDocContext_ = NULL;
	pModel_ = NULL;
	m_pInputEdit = NULL;
	
	m_strName = "";
	m_strAlias = "";
	m_strDesc = "";

	pCmdMgr_ = NULL;
	m_bRunning = FALSE;
	_bInitialized = FALSE;
	m_pSnapMgr = NULL;
	nRequestDepth_ = 0;

	m_pCurInputTool = NULL;
	pGlobalSelSet_ = NULL;

	pMainButtonOptionSet_ = NULL;
	pMainButtonOptionTool_ = NULL;
}

kiCommand::~kiCommand(void)
{
	ASSERT(!m_bRunning);
	//Destroy();
}

// ִ��һ�γ�ʼ�����ͷŵĵط���
int kiCommand::Initialize(int nCmdID,kcModel *pModel,kiCommandMgr *pCmdMgr)
{
	ASSERT(!m_strName.empty());
	ASSERT(pModel && pCmdMgr);
	if(_bInitialized)
		return 1;

	_bInitialized = TRUE;
	m_nID = nCmdID;
	pModel_ = pModel;
	pCmdMgr_ = pCmdMgr;
	pDocContext_ = pCmdMgr->GetDocContext();//��ȡ�ؼ�����
	ASSERT(pDocContext_);

	//��ȡ
	hAISCtx_ = pModel->GetAISContext();
	ASSERT(!hAISCtx_.IsNull());

	pGlobalSelSet_ = pModel->GetGlobalSelSet();
	ASSERT(pGlobalSelSet_);

	m_pInputEdit = pDocContext_->GetInputEdit();
	ASSERT(m_pInputEdit);

	m_pSnapMgr = pDocContext_->GetSnapManager();
	ASSERT(m_pSnapMgr);

	//���ں��棬��Ҫǰ���һЩ����ָ��
	if(!CreateInputTools())
		return KSTA_ERROR;
	
	return OnInitialize();
}

int kiCommand::Destroy()
{
	// �������빤��.
	DestroyInputTools();

	m_inputToolQueue.clear();
	m_pCurInputTool = NULL;

	m_bRunning = FALSE;
	
	return OnDestroy();
}

int kiCommand::OnInitialize()
{
	return KSTA_SUCCESS;
}

int kiCommand::OnDestroy()
{
	return KSTA_SUCCESS;
}

//�������ȡ��Ӧ��AISContext
Handle(AIS_InteractiveContext)  kiCommand::GetAISContext() const
{
	ASSERT(!hAISCtx_.IsNull());
	return hAISCtx_;
}

kcDocContext*  kiCommand::GetDocContext() const
{
	return pDocContext_;
}

kiInputEdit*  kiCommand::GetInputEdit() const
{
	return m_pInputEdit;
}

const Handle(V3d_View)  kiCommand::GetCurrV3dView() const
{
	Handle(V3d_View) aView;
	if(pCmdMgr_){
		aView = pCmdMgr_->GetCurrV3dView();
	}
	return aView;
}

CView* kiCommand::GetActiveView() const
{
	if(pDocContext_){
		return pDocContext_->GetActiveView();
	}
	return NULL;
}

// ��ȡ��ǰ��׼��
kcBasePlane* kiCommand::GetCurrentBasePlane() const
{
	if(pModel_){
		return pModel_->GetCurrentBasePlane();
	}
	return NULL;
}

int kiCommand::Execute()
{
	ASSERT(!m_bRunning);//û��������̬

	ASSERT(pModel_);
	if(!pModel_){
		EndCommand(KSTA_ERROR);
		return KSTA_ERROR;
	}

	//����ȫ�ֶ���
	if(pGlobalSelSet_ != NULL){
		pGlobalSelSet_->GetSelected();
	}

	//���㲶׽��
	CalcSnapPoint();

	if(!InitInputToolQueue()){
		EndCommand(KSTA_ERROR);
		return KSTA_ERROR;
	}

	int iret = OnExecute();
	if(KSTA_ERROR == iret){
		EndCommand(KSTA_ERROR);
		return KSTA_ERROR;
	}
	//�Ƿ�ִ�н���
	if(KSTA_DONE == iret){
		Done();
		Redraw();

		return KSTA_DONE;
	}

	// �Զ�ִ�е�һ������.������һ������.
	// ����OnExecute���Ѿ�ִ����һЩ����.
	if(KSTA_ERROR == NavToNextTool()){
		EndCommand(KSTA_ERROR);
		return KSTA_ERROR;
	}

	m_bRunning = TRUE;//��������״̬

	return KSTA_CONTINUE;
}

// ִ�к���
int		kiCommand::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kiCommand::End(int nCode)
{
	//������ǰ���빤��.
	if(m_pCurInputTool)
		m_pCurInputTool->End();

	//���þ���Ľ�������.
	OnEnd(nCode);
		
	//������빤��
	m_inputToolQueue.clear();
	m_pCurInputTool = NULL;

	if(m_pSnapMgr)
		m_pSnapMgr->HideSnapMarker();

	m_bRunning = FALSE;
	//
	Redraw();

	if(m_pInputEdit)
		m_pInputEdit->PromptText("ָ��");

	return nCode;
}

int kiCommand::OnEnd(int nCode)
{
	UNUSED_ALWAYS(nCode);

	m_bRunning = FALSE;

	return KSTA_CONTINUE;
}

// Ĭ��ʵ�֡�
int kiCommand::Apply()
{
	int nErr = KSTA_CONTINUE;
	if(CanFinish())
	{
		nErr = OnApply();
		if(nErr == KSTA_DONE)
		{
			Done();
			return KSTA_DONE;
		}
		else if(nErr == KSTA_ERROR || nErr == KSTA_FAILED)
		{
			EndCommand(nErr);

			return nErr;
		}
	}
	
	return nErr;
}

//
int kiCommand::OnApply()
{
	return KSTA_DONE;
}

// �����ȷ��������
int kiCommand::OnOK()
{
	return KSTA_DONE;
}

// ������ǰ����ִ�С��������ʹ�øú�����
// ����ִ�н�������ֹ��ȡ����Ҫͳһ��cmdMgr��ִ�С�
//
int		kiCommand::EndCommand(int nCode)
{
	ASSERT(pCmdMgr_);
	if(pCmdMgr_){
		pCmdMgr_->EndCommand(nCode);
	}

	return KSTA_DONE;
}

// ����ִ����ɡ����Խ����ˡ�
int		kiCommand::Done()
{
	EndCommand(KSTA_DONE);
	Redraw();

	return KSTA_DONE;
}

// ��ʾ��ʾ��Ϣ��������
void  kiCommand::PromptMessage(const char *lpszMsg)
{
	if(m_pCurInputTool != NULL)
		return;

	if(m_pInputEdit && lpszMsg)
		m_pInputEdit->PromptText(lpszMsg);
}

// ������ǰ�����빤��,��Ӧ����������
//
int		kiCommand::EndCurrentInputTool()
{
	if(m_pCurInputTool)
	{
		m_pCurInputTool->End();
		m_pCurInputTool = NULL;
	}

	return KSTA_SUCCESS;
}

//��InputEdit�����ı��ı�ʱ
int  kiCommand::OnEditInputChanged(const char *pszText)
{
	return 0;
}

//��InputEdit�ı��������ʱ
int  kiCommand::OnEditInputFinished(const char *pszText)
{
	return 0;
}

//��һ��ѡ�ѡ��ʱ�����磺ͨ������ַ������ͨ�����
int  kiCommand::OnEditOptionSelected(int idx,kiOptionItem *pItem)
{
	return 0;
}

// ѡ���ֵ�����˸ı�
void  kiCommand::OnOptionChanged(kiOptionItem *pOptItem)
{
	
}

// ѡ�ѡ����,ͨ����buttonѡ������
// ���ﴦ��ϵͳ��ť�Ĵ���
void  kiCommand::OnOptionSelected(kiOptionItem *pOptItem)
{
	ASSERT(pOptItem);
	int kstat = KSTA_CONTINUE;

	if(pOptItem->GetType() == KOPT_INPUT_BUTTON){
		kiOptionButton *pOptBtn = (kiOptionButton *)pOptItem;
		int nID = pOptBtn->GetID();
		if(nID == QX_SYS_BUTTON_CANCEL){
			//�˳�����
			EndCommand(KSTA_CANCEL);
		}else if(nID == QX_SYS_BUTTON_APPLY){
			kstat = OnApply();
			// ��������
			if(kstat == KSTA_DONE){
				Done();
			}
		}else if(nID == QX_SYS_BUTTON_OK){
			kstat = OnOK();
			// ��������
			if(kstat == KSTA_DONE){
				Done();
			}
		}else{
			//��������
			OnOptionButtonClicked(nID);
		}
	}
}

void kiCommand::OnOptionButtonClicked(int nID)
{
	if(nID >= QX_SYS_BUTTON_ID_BASE){
		ASSERT(FALSE);
	}
}

BOOL kiCommand::CreateInputTools()
{
	return TRUE;
}

BOOL	kiCommand::InitInputToolQueue()
{
	return TRUE;
}

void	kiCommand::ClearInputToolList()
{
	m_inputToolQueue.clear();
}

// ���һ������ť���빤�ߣ���Ϊ�˵�
void kiCommand::AddMainButtonOptionTool(bool withApply)
{
	//���û�У���Ҫ����
	if(!pMainButtonOptionTool_){

		if(!pMainButtonOptionSet_){
			pMainButtonOptionSet_ = new kiOptionSet(this);
			// �����Ҫbutton
			if(withApply){
				pMainButtonOptionSet_->AddSysButtonOption(QX_SYS_BUTTON_APPLY);
			}
			pMainButtonOptionSet_->AddSysButtonOption(QX_SYS_BUTTON_OK);
			pMainButtonOptionSet_->AddSysButtonOption(QX_SYS_BUTTON_CANCEL);
		}
		// ��������
		pMainButtonOptionTool_ = new kiInputOptionsTool(this,"����ѡ��:",pMainButtonOptionSet_);
	}

	// �����ظ����
	TInputToolList::iterator ite = m_inputToolQueue.begin();
	for(;ite != m_inputToolQueue.end(); ++ite){
		if((*ite) == pMainButtonOptionTool_){
			ASSERT(FALSE);
			return;
		}
	}
	// һ��ù������������
	m_inputToolQueue.push_back(pMainButtonOptionTool_);
}

kiOptionSet* kiCommand::GetMainButtonOptionSet() const
{
	return pMainButtonOptionSet_;
}

int kiCommand::AddInputTool(kiInputTool *pTool)
{
	ASSERT(pTool);
	if(!pTool) return KSTA_ERROR;

	m_inputToolQueue.push_back(pTool);

	return KSTA_CONTINUE;
}

// ִ�е���һ�����빤��
//
int		kiCommand::NavToNextTool()
{
	KTRACE("\n kiCommand::NavToNextTool().");
	//�����ɵģ���Ϊ�ɵ�������������
	if(m_pCurInputTool)
	{
		m_pCurInputTool->End();
		m_pCurInputTool = NULL;
	}

	if(m_inputToolQueue.empty())
		return KSTA_CONTINUE;

	int iret = KSTA_CONTINUE;

	m_pCurInputTool = m_inputToolQueue.front();
	m_inputToolQueue.pop_front();
	if(m_pCurInputTool)
	{
		iret = m_pCurInputTool->Begin();
		if(iret == KSTA_ERROR)
		{
			//
			m_pCurInputTool = NULL;
			return KSTA_ERROR;
		}
	}

	return KSTA_CONTINUE;
}

int		kiCommand::NavToInputTool(kiInputTool *pTool)
{
	if(!pTool)
		return KSTA_ERROR;

	//�����ɵ�
	if(m_pCurInputTool)
		m_pCurInputTool->End();

	m_pCurInputTool = pTool;
	int iret = m_pCurInputTool->Begin();
	if(iret == KSTA_ERROR)
	{
		//
		m_pCurInputTool = NULL;
		return KSTA_ERROR;
	}

	return KSTA_CONTINUE;
}

BOOL	kiCommand::DestroyInputTools()
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//�����Ϣ
void	kiCommand::OnLButtonDown(kuiMouseInput& mouseInput)
{
	if(m_pCurInputTool)
		m_pCurInputTool->OnLButtonDown(mouseInput);
}

int		kiCommand::OnLButtonUp(kuiMouseInput& mouseInput)
{
	int iret = KSTA_CONTINUE;
	if(m_pCurInputTool)
		m_pCurInputTool->OnLButtonUp(mouseInput);

	return KSTA_CONTINUE;
}

int		kiCommand::OnLButtonDblClk(kuiMouseInput& mouseInput)
{
	if(m_pCurInputTool)
		m_pCurInputTool->OnLButtonDblClk(mouseInput);

	return KSTA_CONTINUE;
}

int		kiCommand::OnMouseMove(kuiMouseInput& mouseInput)
{
	//
	if(m_pSnapMgr)
	{
		m_pSnapMgr->DoSnap(mouseInput.m_x,mouseInput.m_y);
	}
	if(m_pCurInputTool)
	{
		m_pCurInputTool->OnMouseMove(mouseInput);
	}
	return KSTA_CONTINUE;
}

void	kiCommand::OnRButtonDown(kuiMouseInput& mouseInput)
{
	if(m_pCurInputTool)
	{
		m_pCurInputTool->OnRButtonDown(mouseInput);
	}
}

// Ӧ�ã���mgr����顣
// ȡ����,���ⲿ��CmdMgr������.
//
int		kiCommand::OnRButtonUp(kuiMouseInput& mouseInput)
{
	int nErr = KSTA_CONTINUE;
	//����й��߽������룬�ɹ��߽���ִ��
	if(m_pCurInputTool)
	{
		nErr = m_pCurInputTool->OnRButtonUp(mouseInput);
		if(nErr == KSTA_CANCEL)
		{
			if(!CanFinish())//������ɣ�����Ϊ��ȡ������
			{
				EndCommand(KSTA_CANCEL);
				return KSTA_CANCEL;
			}
			else//ȡ����ǰ���룬���������������룬Ҳ�����ǿ���ִ�������ˣ����磺�����㹹�������ȡ�
			{
				nErr = Apply();
				return nErr;
			}
		}else if (nErr == KSTA_DONE) {
			//Ӧ��
			if (CanFinish()) {
				//
				nErr = Apply();
				return nErr;
			}
		}
	}
	else
	{
		//������ܹ���ɣ���Ϊȡ������ִ�С�
		if(!CanFinish())
		{
			EndCommand(KSTA_CANCEL);
			return KSTA_CANCEL;
		}
		else
		{
			//Ӧ��
			nErr = Apply();
			return nErr;
		}
	}

	return KSTA_CONTINUE;
}

// ���ȫ��ѡ�񼯡�
bool	kiCommand::ClearGlobalSelSet()
{
	ASSERT(pGlobalSelSet_);
	if(!pGlobalSelSet_) return false;

	pGlobalSelSet_->UnHilight();
	pGlobalSelSet_->Clear();

	return true;
}

bool kiCommand::IsGlobalSelSetEmpty()
{
	if(pGlobalSelSet_ != NULL){
		return pGlobalSelSet_->IsEmpty();
	}
	return true;
}

kiSelSet* kiCommand::GetGlobalSelSet() const
{
	return pGlobalSelSet_;
}

// ���»���
void kiCommand::Redraw()
{
	//Handle(V3d_View) aView = pModel_->GetActiveView();
	//if(!aView.IsNull()){
	//	aView->Redraw();
	//}
	//������ڴ��ڵ���Ϣ
	if(pDocContext_){
		pDocContext_->RedrawCurView();
		//pDocContext_->RefreshCurView();
	}
}

// ���㲶׽��
BOOL	kiCommand::CalcSnapPoint()
{
	ASSERT(pModel_);
	ASSERT(m_pSnapMgr);
	m_pSnapMgr->Clear();

	pModel_->CalcSnapPoint(m_pSnapMgr);
	//
	m_pSnapMgr->CalcScreenPoint();

	return TRUE;
}

// �����ʱ��׽��
bool	kiCommand::AddTempSnapPoint(double x,double y,double z)
{
	if(!m_pSnapMgr)
		return false;

	kiSnapPoint sp(x,y,z);
	return m_pSnapMgr->AddSnapPoint(KSNAP_TMP_POINT,sp);
}

void	kiCommand::ClearTempSnapPoint()
{
	if(m_pSnapMgr)
		m_pSnapMgr->ClearTempPoint();
}

//the aux function used inside the command
kVector3 kiCommand::GetWorkPlaneZDir()
{
	kcBasePlane *pBasePlane = GetCurrentBasePlane();
	ASSERT(pBasePlane);
	kvCoordSystem cs;
	pBasePlane->GetCoordSystem(cs);
	return cs.Z();
}

void  kiCommand::GetWorkPlaneCoordSystem(kvCoordSystem& cs)
{
	kcBasePlane *pBasePlane = GetCurrentBasePlane();
	ASSERT(pBasePlane);
	if(pBasePlane){
		pBasePlane->GetCoordSystem(cs);
	}
}

//����һ��InputEdit����
int  kiCommand::RequestEditInput(const char *pszPromptText,kiOptionSet *pOptionSet)
{
	ASSERT(m_pInputEdit);
	ASSERT(pszPromptText);

	m_pInputEdit->RequestInput(pszPromptText,this,pOptionSet);

	return KSTA_CONTINUE;
}

// ����ǰ��������ѹջ
int  kiCommand::PushCurrentRequest()
{
	ASSERT(m_pInputEdit);
	if(m_pInputEdit->PushRequest()){
		nRequestDepth_ ++;
		return 1;
	}
	
	return 0;
}

int  kiCommand::PopRequest()
{
	ASSERT(m_pInputEdit);
	if(nRequestDepth_ <= 0)
		return 0;

	m_pInputEdit->PopRequest();
	nRequestDepth_ --;

	return 1;
}

//
void  kiCommand::InitSet(const char *pszName,const char *pszAlias,const char *pszDesc)
{
	ASSERT(pszName && pszAlias && pszDesc);
	m_strName = pszName;
	m_strAlias = pszAlias;
	m_strDesc = pszDesc;
}

void kiCommand::UAddToModel(const std::vector<kcEntity*> &aEntVec)
{
	if(pModel_ != NULL && !aEntVec.empty()){
		pModel_->BeginUndo(GetName());
		//
		pModel_->AddEntity(aEntVec,TRUE);
		pModel_->EndUndo();
	}
}

void kiCommand::UAddToModel(kcEntity *pEntity,BOOL bUpdateView)
{
	if(pEntity && pEntity->IsValid() && pModel_){
		pModel_->BeginUndo(GetName());
		//
		pModel_->AddEntity(pEntity,bUpdateView);
		pModel_->EndUndo();
	}
}

void kiCommand::UAddToModel(const TopoDS_Shape &aS,BOOL bUpdateView)
{
	if(!aS.IsNull()){
		kcEntity *pEntity = kcEntityFactory::CreateEntity(aS);
		if(pEntity && pEntity->IsValid()){
			UAddToModel(pEntity,bUpdateView);
		}
	}
}

///////////////////////
//������Ϣ

int  kiCommand::OnKeyDown(kuiKeyInput& keyInput)
{
	keyInput;
	return KSTA_CONTINUE;
}

int  kiCommand::OnKeyUp(kuiKeyInput& keyInput)
{
	keyInput;
	return KSTA_CONTINUE;
}

int  kiCommand::OnChar(kuiKeyInput& keyInput)
{
	keyInput;
	return KSTA_CONTINUE;
}

/////////////////////////////////////////////////////////////
// ��ʱ������

bool kiCommand::SetTimer(int nId,UINT uElapse)
{
	if(HasTimer(nId)){
		ASSERT(FALSE);
		return false;
	}

	IQxTimerManager *pTimerMgr = pDocContext_->GetTimerManager();
	QxCmdTimer *pHandler = new QxCmdTimer(this);
	UINT_PTR nIDEvent = pTimerMgr->SetTimer(pHandler,uElapse);
	if(nIDEvent == 0){ //ʧ��
		ASSERT(FALSE);
		return false;
	}
	
	//��¼����
	aTimerIdMap_.insert(stdTimerIDMap::value_type(nId,nIDEvent));

	return true;
}

bool kiCommand::KillTimer(int nId)
{
	UINT_PTR nIDEvent = GetTimerEventID(nId);
	if(nIDEvent == 0)
		return false;

	IQxTimerManager *pTimerMgr = pDocContext_->GetTimerManager();
	pTimerMgr->KillTimer(nIDEvent);

	return true;
}

bool kiCommand::PauseTimer(int nId)
{
	UINT_PTR nIDEvent = GetTimerEventID(nId);
	if(nIDEvent > 0){
		IQxTimerManager *pTimerMgr = pDocContext_->GetTimerManager();
		return pTimerMgr->PauseTimer(nIDEvent);
	}
	return false;
}

bool	 kiCommand::ResumeTimer(int nId)
{
	UINT_PTR nIDEvent = GetTimerEventID(nId);
	if(nIDEvent > 0){
		IQxTimerManager *pTimerMgr = pDocContext_->GetTimerManager();
		return pTimerMgr->ResumeTimer(nIDEvent);
	}
	return false;
}

bool kiCommand::HasTimer(int nId)
{
	stdTimerIDMap::iterator ite = aTimerIdMap_.find(nId);
	return (ite != aTimerIdMap_.end());
}

int kiCommand::OnTimer(int nId,DWORD dwPassTime)
{
	nId;dwPassTime;
	return 0;
}

// �����nIDEventתΪ�����ڲ���idֵ��
//
int kiCommand::HandlerTimer(UINT_PTR nIDEvent,DWORD dwPassTime)
{
	int rc = 0;
	stdTimerIDMap::iterator ite = aTimerIdMap_.begin();
	for(;ite != aTimerIdMap_.end();++ite){
		if((*ite).second == nIDEvent){
			rc = OnTimer((*ite).first,dwPassTime);
			break;
		}
	}
	return rc;
}

// ���������ڵ�id��ȡTimer��nIDֵ��
UINT_PTR kiCommand::GetTimerEventID(int nID)
{
	stdTimerIDMap::iterator ite = aTimerIdMap_.find(nID);
	if(ite != aTimerIdMap_.end()){
		return (*ite).second;
	}
	return 0;
}