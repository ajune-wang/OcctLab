#include "StdAfx.h"

#include "kcModel.h"
#include "kiCommand.h"
#include "kcSnapMngr.h"
#include "kcBasePlane.h"
#include "kiInputTool.h"

kiInputTool::kiInputTool(kiCommand *pCommand,const char *pPromptText,kiOptionSet *pOptionSet)
:m_pCommand(pCommand)
{
	ASSERT(pCommand && pPromptText);
	m_pParent = NULL;
	m_szPromptText = pPromptText;//��¼��ʾ��Ϣ
	//m_pOptionSet = pOptionSet;
	pInOptionSet_ = pOptionSet;

	if(pCommand != NULL){
		hAISCtx_ = pCommand->GetAISContext();
	}

	DoInitialize();
	Reset();

	// ֻ��û���趨ѡ��ʱ�����趨,�������Ա���OptionSet�ⲿ���趨
	if(pOptionSet && !pOptionSet->HasOptionCallBack()){
		pOptionSet->SetOptionCallBack(this);
	}
}

kiInputTool::kiInputTool(kiInputTool *pParent,const char *pPromptText,kiOptionSet *pOptionSet)
{
	ASSERT(pParent && pPromptText);
	m_pCommand = NULL;
	m_pParent = pParent;
	m_szPromptText = pPromptText;
	//m_pOptionSet = pOptionSet;
	pInOptionSet_ = pOptionSet;

	if(pParent != NULL){
		hAISCtx_ = pParent->GetAISContext();
	}

	DoInitialize();
	Reset();

	// ֻ��û���趨ѡ��ʱ�����趨,�������Ա���OptionSet�ⲿ���趨
	if(pOptionSet && !pOptionSet->HasOptionCallBack()){
		pOptionSet->SetOptionCallBack(this);
	}
}

kiInputTool::~kiInputTool(void)
{
	//if(m_pOptionSet)
	//	m_pOptionSet->SetOptionCallBack(NULL);
	//aToolOptionSet_.Clear();
	//aToolOptionSet_.SetInputTool(NULL);
}

//��Ҫ�ĳ�ʼ��,��Ҫ��ȡ����������ֵ
//
void  kiInputTool::DoInitialize()
{
	m_pInputEdit = GetInputEdit();
	ASSERT(m_pInputEdit);
	m_pSnapMgr = GetSnapManager();
		
	//�趨״̬Ϊ��ʼ״̬
	SetInputState(KINPUT_INITIAL);
}

void	kiInputTool::Reset()
{
	pCurrentOption_ = NULL;	
	pCurOptionSet_ = NULL;

	if(bHasOwnOptions_){
		SplitOptionSet(pInOptionSet_);
	}
	aToolOptionSet_.Clear();
	aToolOptionSet_.SetOptionCallBack(this);

	m_pCurSubTool = NULL;
	bHasOwnOptions_ = false;

	SetInputState(KINPUT_INITIAL);
}

// ���ⲿѡ�����������ѡ�����
//
void  kiInputTool::CombineOptionSet(kiOptionSet *pOptSet)
{
	kiOptionItem *pItem = NULL;
	int ix,count = 0;
	if(pOptSet){
		count = pOptSet->GetCount();
		for(ix = 0;ix < count;ix ++){
			pItem = pOptSet->GetOption(ix);
			if(!aToolOptionSet_.HasOptionItem(pItem)){
				// ��ʱ���ı�֪ͨ����
				//pItem->SetOptionCallBack(this);//�ı�֪ͨ�ù���
				aToolOptionSet_.AddOption(pItem);
			}
		}
	}
}

// ���ⲿѡ��ӹ����ܵ�ѡ���ȥ����
//
void  kiInputTool::SplitOptionSet(kiOptionSet *pOptSet)
{
	kiOptionItem *pItem = NULL;
	int ix,count = 0;
	if(pOptSet){
		count = pOptSet->GetCount();
		for(ix = 0;ix < count;ix ++){
			pItem = pOptSet->GetOption(ix);
			if(!aToolOptionSet_.HasOptionItem(pItem)){ //������
				//pItem->SetOptionCallBack(NULL); //���ı����
				aToolOptionSet_.RemoveOptionItem(pItem);
			}
		}
	}
}

// ������������
//
int  kiInputTool::Begin()
{
	ASSERT(m_pInputEdit);
	ASSERT(m_pCommand || m_pParent);
	ASSERT(!m_szPromptText.empty());

	Reset();

	//������һЩ����Ĵ����
	// �����ʼ����ˣ����磺��ȫ��ѡ�񼯻�ȡ�˶��󣬿���ֱ�ӷ���
	if(OnBegin() == KSTA_DONE)
		return KSTA_DONE;

	//��ȡ������Ϣ
	std::string strPrompt;
	DoGetPromptText(strPrompt);

	// �������ѡ����뵽����ѡ���
	// ��Ҫ�ж��Ƿ����Լ���ѡ��
	if(!aToolOptionSet_.IsEmpty()){
		bHasOwnOptions_ = true;
		CombineOptionSet(pInOptionSet_);
		pCurOptionSet_ = &aToolOptionSet_;
	}else{
		bHasOwnOptions_ = false;
		//ֱ��ʹ�������ѡ�
		pCurOptionSet_ = pInOptionSet_;
	}
	
	// �������ʾ��ʾ����ʼ���롣
	m_pInputEdit->RequestInput(strPrompt.c_str(),this,pCurOptionSet_);
	
	SetInputState(KINPUT_RUNNING);//ת��������״̬

	return KSTA_CONTINUE;
}

// ������������
// �������н������ڲ����ⲿ���ܻ���øú���
//
int  kiInputTool::End()
{
	//�������ѡ��ӹ���ѡ���з���,ֻ���ڱ�������ѡ��ʱ������Ҫ����
	//
	if(bHasOwnOptions_){
		SplitOptionSet(pInOptionSet_);
	}

	OnEnd();

	if(m_pInputEdit)
		m_pInputEdit->PromptText("ָ��");

	if(m_nInputState == KINPUT_DONE)
		return KSTA_DONE;

	return KSTA_FAILED;
}

Handle(AIS_InteractiveContext)  kiInputTool::GetAISContext() const
{
	ASSERT(!hAISCtx_.IsNull());
	return hAISCtx_;
}

kcDocContext* kiInputTool::GetDocContext() const
{
	if(m_pCommand){
		return m_pCommand->GetDocContext();
	}
	if(m_pParent){
		return m_pParent->GetDocContext();
	}
	return NULL;
}

kiInputEdit* kiInputTool::GetInputEdit() const
{
	if(m_pCommand){
		return m_pCommand->GetInputEdit();
	}

	if(m_pParent){
		return m_pParent->GetInputEdit();//�����ݹ����
	}
	ASSERT(FALSE); //��Ӧ������
	return NULL;
}

const Handle(V3d_View) kiInputTool::GetCurrV3dView() const
{
	Handle(V3d_View) aView;
	if(m_pCommand){
		return m_pCommand->GetCurrV3dView();
	}
	if(m_pParent){
		return m_pParent->GetCurrV3dView();
	}
	return aView;
}

// ��ȡ����ģ��
kcModel* kiInputTool::GetModel() const
{
	if (m_pCommand) {
		return m_pCommand->GetModel();
	}
	if (m_pParent) {
		return m_pParent->GetModel();
	}
	return NULL;
}

// ��ȡ��ǰ��׼��
kcBasePlane* kiInputTool::GetCurrentBasePlane() const
{
	if(m_pCommand){
		return m_pCommand->GetCurrentBasePlane();
	}
	if(m_pParent){
		return m_pParent->GetCurrentBasePlane();
	}
	return NULL;
}

kcSnapMngr* kiInputTool::GetSnapManager() const
{
	if(m_pCommand){
		kcModel *pModel = m_pCommand->GetModel();
		if(pModel){
			return pModel->GetSnapManager();
		}
	}
	if(m_pParent){
		return m_pParent->GetSnapManager();
	}
	return NULL;
}

kiOptionSet* kiInputTool::GetCurOptionSet() const
{
	return pCurOptionSet_;
}

kiOptionSet* kiInputTool::GetInOptionSet() const
{
	return pInOptionSet_;
}

//
int kiInputTool::OnBegin()
{
	return KSTA_CONTINUE;
}

int  kiInputTool::OnEnd()
{
	KTRACE("\nkiInputTool::OnEnd()....\n");
	return KSTA_CONTINUE;
}

// ��tool�Ļص�����,����tool���ý���ʱ,����.
int  kiInputTool::OnSubInputFinished(kiInputTool *pTool)
{
	return KSTA_CONTINUE;
}

int  kiInputTool::GetInputState() const
{
	return m_nInputState;
}

// ��������״̬
void  kiInputTool::SetInputState(int nState)
{
	ASSERT(nState >= KINPUT_INITIAL && nState <= KINPUT_CANCEL);
	m_nInputState = nState;
}

//���㵱ǰ��
void  kiInputTool::CalcInputPoint(kuiMouseInput& mouseInput,double point[3])
{
	//�Ƿ����Զ������
	if(m_pSnapMgr && m_pSnapMgr->HasSnapPoint()){
		kiSnapPoint sp = m_pSnapMgr->GetSnapPoint();
		sp._point.get(point);
	}else{
		if(m_pCommand){
			kcModel *pModel = m_pCommand->GetModel();
			if(pModel){
				kcBasePlane *pBasePlane = GetCurrentBasePlane();
				ASSERT(pBasePlane);
				kPoint3 wp;
				pBasePlane->PixelToWorkPlane(mouseInput.m_x,mouseInput.m_y,wp);
				wp.get(point);
			}
		}
	}
}

////////////////////////////////////////////////////////////////
//InputEdit����Ļص�����
int  kiInputTool::OnEditInputChanged(const char *pszText)
{
	return 0;
}

// ��edit�����ı���ɺ󣬵��øú���,�������������ı���Ϣ
//
int  kiInputTool::OnEditInputFinished(const char *pInputText)
{
	std::string szPrompt;
	int nSelfOptLimit = 0,idxOpt;

	//�������ֹ�������ѡ����ⲿѡ��
	if(bHasOwnOptions_){
		nSelfOptLimit = pCurOptionSet_->GetCount();
		if(pInOptionSet_)
			nSelfOptLimit -= pInOptionSet_->GetCount();
	}

	if(pCurrentOption_){//��ѡ�������
		//��������ɹ�
		if(pCurrentOption_->ParseInputText(pInputText)){
			idxOpt = pCurOptionSet_->IndexOfOption(pCurrentOption_);
			if(pCurrentOption_->GetType() == KOPT_INPUT_BUTTON){
				if(idxOpt < nSelfOptLimit)
					OnToolOptionClicked(pCurrentOption_);
				else
					OnOptionSelected(pCurrentOption_);
			}else{
				if(idxOpt < nSelfOptLimit)
					OnToolOptionChanged(pCurrentOption_);
				else
					OnOptionChanged(pCurrentOption_);
			}
			//���»�ȡ��ʾ��Ϣ����������ʾ:
			DoGetPromptText(szPrompt);
			pCurrentOption_ = NULL;//�ÿ�
			m_pInputEdit->RequestInput(szPrompt.c_str(),this,pCurOptionSet_); //m_pOptionSet);
		}else{
			//Ҫ����������
		}
	}else{
		DoTextInputFinished(pInputText);
	}

	return 1;
}
	
// ��edit��ѡ����ѡ��������������key�������øú���,�������ѡ�е�option�±�
//
int  kiInputTool::OnEditOptionSelected(int idx,kiOptionItem *pOptionItem)
{
	ASSERT(pCurOptionSet_);
	std::string szPrompt;
	int nSelfOptLimit = 0,idxOpt;

	//�������ֹ�������ѡ����ⲿѡ��
	if(bHasOwnOptions_){
		nSelfOptLimit = pCurOptionSet_->GetCount();
		if(pInOptionSet_)
			nSelfOptLimit -= pInOptionSet_->GetCount();
	}

	if(pCurrentOption_){//��ѡ�����ѡ�ѡ��,Ŀǰֻ��enum֧����ѡ��
		if(pCurrentOption_->GetType() == KOPT_INPUT_ENUM){
			pCurrentOption_->SetCurrentOption(idx);
			idxOpt = pCurOptionSet_->IndexOfOption(pCurrentOption_);
			if(idxOpt < nSelfOptLimit) //���߱���ѡ�ѡ����
				OnToolOptionChanged(pCurrentOption_);
			else
				OnOptionChanged(pCurrentOption_);

			//���»�ȡ��ʾ��Ϣ����������ʾ:
			//DoGetPromptText(szPrompt);
			//pCurrentOption_ = NULL;//�ÿ�
			//m_pInputEdit->RequestInput(szPrompt.c_str(),this,pCurOptionSet_); //m_pOptionSet);
		}
	}else{//��ǰ��ѡ�ѡ��
		pCurrentOption_ = pCurOptionSet_->GetOption(idx);
		ASSERT(pCurrentOption_);
		//boolѡ����磺����(&Y)�������������Ϊ:����(&N).����������
		if(pCurrentOption_->GetType() == KOPT_INPUT_BOOL){
			((kiOptionBool *)pCurrentOption_)->NotValue();
			m_pInputEdit->Update();//������ʾ
			if(idx < nSelfOptLimit)
				OnToolOptionChanged(pCurrentOption_);
			else
				OnOptionChanged(pCurrentOption_);
		}else if(pCurrentOption_->GetType() == KOPT_INPUT_BUTTON){
			if(idx < nSelfOptLimit)
				OnToolOptionClicked(pCurrentOption_);
			else
				OnOptionChanged(pCurrentOption_);
		}else{//��������
			//�б�ѡ�Ӧ�������б�
			kiOptionSet *pSubSet = pCurrentOption_->GetSubOptionSet();
			pCurrentOption_->GetPromptText(szPrompt);
			//������ѡ��������ʾ
			m_pInputEdit->RequestInput(szPrompt.c_str(),this,pSubSet);
		}
	}

	return 1;
}

void	kiInputTool::DoGetPromptText(std::string& str)
{
	str = m_szPromptText;
}

// ���������ı�
int		kiInputTool::DoTextInputFinished(const char *pInputText)
{
	return 1;
}

/////////////////////////////////////////////////
// �����Ӧ������

void	kiInputTool::OnLButtonDown(kuiMouseInput& mouseInput) 
{
	if(m_pCurSubTool)
		m_pCurSubTool->OnLButtonDown(mouseInput);
}

int		kiInputTool::OnLButtonUp(kuiMouseInput& mouseInput) 
{  
	if(m_pCurSubTool)
		return m_pCurSubTool->OnLButtonUp(mouseInput);
	return KSTA_CONTINUE; 
}

int		kiInputTool::OnLButtonDblClk(kuiMouseInput& mouseInput) 
{  
	if(m_pCurSubTool)
		return m_pCurSubTool->OnLButtonDblClk(mouseInput);
	return KSTA_CONTINUE; 
}

int		kiInputTool::OnMouseMove(kuiMouseInput& mouseInput) 
{  
	if(m_pCurSubTool)
		m_pCurSubTool->OnMouseMove(mouseInput);
	return KSTA_CONTINUE; 
}

void	kiInputTool::OnRButtonDown(kuiMouseInput& mouseInput) 
{
	if(m_pCurSubTool)
		m_pCurSubTool->OnRButtonDown(mouseInput);
}

// �Ҽ�Ĭ�ϱ�ʾȡ�����롣
// ���������Ҫ���⴦�������ظú�����
//
int		kiInputTool::OnRButtonUp(kuiMouseInput& mouseInput) 
{ 
	if(m_pCurSubTool)
		return m_pCurSubTool->OnRButtonUp(mouseInput);
	return KSTA_CANCEL;//Ĭ�ϱ�ʾȡ������ 
}

//������Ϣ
int	kiInputTool::OnKeyDown(kuiKeyInput& keyInput) 
{
	if(m_pCurSubTool)
		return m_pCurSubTool->OnKeyDown(keyInput);
	return KSTA_CONTINUE; 
}

int	kiInputTool::OnKeyUp(kuiKeyInput& keyInput) 
{
	if(m_pCurSubTool)
		return m_pCurSubTool->OnKeyUp(keyInput);
	return KSTA_CONTINUE; 
}

int	kiInputTool::OnChar(kuiKeyInput& keyInput) 
{
	if(m_pCurSubTool)
		return m_pCurSubTool->OnChar(keyInput);
	return KSTA_CONTINUE; 
}

// �������,���ö�Ӧ�Ļص�����
// ���ݶ�Ӧ�Ĵ����������ض�Ӧ��״̬��
//
int	kiInputTool::DoNotify()
{
	int rc = KSTA_DONE;

	if(IsInputDone()){
		if(m_pCommand){
			rc = m_pCommand->OnInputFinished(this);
		}
		if(m_pParent){
			rc = m_pParent->OnSubInputFinished(this);
		}
	}else{//Ӧ��Ҳ���ö�Ӧ�Ļص���������֪ͨ������ɣ��������һЩ��������
		if(m_pCommand){
			rc = m_pCommand->OnInputFinished(this);
		}
	}
	
	return rc;
}

int	kiInputTool::DoNotifyOptionChanged(kiOptionItem *pOptionItem)
{
	if(m_pCommand)
		m_pCommand->OnOptionChanged(this,pOptionItem);

	if(m_pParent)
		m_pParent->OnOptionChanged(pOptionItem);

	return KSTA_CONTINUE;
}

int	kiInputTool::DoNotifyOptionClicked(kiOptionItem *pOptionItem)
{
	if(m_pCommand)
		m_pCommand->OnOptionClicked(this,pOptionItem);

	if(m_pParent)
		m_pParent->OnOptionSelected(pOptionItem);

	return KSTA_CONTINUE;
}

void kiInputTool::OnToolOptionChanged(kiOptionItem *pOptionItem)
{
}

void kiInputTool::OnToolOptionClicked(kiOptionItem *pOptionItem)
{
}

// ��ѡ��ı��ĵ��ú���
void  kiInputTool::OnOptionChanged(kiOptionItem *pOptionItem)
{
	if(m_pCommand){
		m_pCommand->OnOptionChanged(pOptionItem);
	}
	if(m_pParent){
		m_pParent->OnOptionChanged(pOptionItem);
	}
}

void  kiInputTool::OnOptionSelected(kiOptionItem *pOptionItem)
{
	if(m_pCommand){
		m_pCommand->OnOptionSelected(pOptionItem);
	}

	if(m_pParent){
		m_pParent->OnOptionSelected(pOptionItem);
	}
}

