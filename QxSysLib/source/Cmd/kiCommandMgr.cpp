#include "StdAfx.h"
#include "kcModel.h"
#include "kiCommand.h"
#include "kcDocContext.h"
#include "QxPluginMgr.h"
#include "kiCommandMgr.h"

kiCommandMgr::kiCommandMgr(void)
{
	_bInitialized = false;
	_pModel = NULL;
	_pPluginMgr = NULL;
	pGlobalSelSet_ = NULL;

	_pExecCommand = NULL;
}

kiCommandMgr::~kiCommandMgr(void)
{
	Destroy();
}

//��ʼ��
BOOL  kiCommandMgr::Initialize(kcDocContext *pDocCtx,QxPluginMgr *pPluginMgr)
{
	ASSERT(pDocCtx && pPluginMgr);
	if(_bInitialized)
		return TRUE;

	pDocContext_ = pDocCtx;
	_pModel = pDocCtx->GetModel();
	ASSERT(_pModel);
	_pPluginMgr = pPluginMgr;
	pGlobalSelSet_ = _pModel->GetGlobalSelSet();//
		
	_pExecCommand = NULL;
	_bInitialized = true;

	return TRUE;
}

BOOL  kiCommandMgr::Destroy()
{
	if(!_bInitialized)
		return TRUE;

	//����ϵͳ����
	int ix,isize = (int)m_sysCmdArray.size();
	for(ix = 0;ix < isize;ix ++){
		m_sysCmdArray[ix]->Destroy();
		delete m_sysCmdArray[ix];
	}
	m_sysCmdArray.clear();
	_bInitialized = false;

	return TRUE;
}

// ע���ȡ������
//
BOOL	kiCommandMgr::RegisterCommand(kiCommand *pCommand)
{
	if(!pCommand)
		return FALSE;

	//�Ƿ��Ѿ�����,ID ���� �������붼��Ψһ��.
	if(FindRegisterCommand(pCommand->GetID()) != NULL ||
	   FindRegisterCommand(pCommand->GetName()) != NULL ||
	   FindRegisterCommandByAlias(pCommand->GetAlias()) != NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_cmdIDMap.insert(kiCommandIDMap::value_type(pCommand->GetID(),pCommand));
	m_cmdNameMap.insert(kiCommandNameMap::value_type(pCommand->GetName(),pCommand));
	m_cmdAliasMap.insert(kiCommandNameMap::value_type(pCommand->GetAlias(),pCommand));

	m_cmdArray.push_back(pCommand);

	return TRUE;
}

BOOL	kiCommandMgr::RegisterSysCommand(kiCommand *pCommand)
{
	if(!RegisterCommand(pCommand))
		return FALSE;

	//��¼����,�Ա�����
	m_sysCmdArray.push_back(pCommand);

	return TRUE;
}

BOOL	kiCommandMgr::UnRegisterCommand(kiCommand *pCommand)
{
	if(!pCommand || FindRegisterCommand(pCommand->GetID()) != pCommand)
		return FALSE;

	kiCommandIDMap::iterator iit = m_cmdIDMap.find(pCommand->GetID());
	if(iit != m_cmdIDMap.end())
		m_cmdIDMap.erase(iit);

	kiCommandNameMap::iterator nit;
	nit = m_cmdNameMap.find(pCommand->GetName());
	if(nit != m_cmdNameMap.end())
		m_cmdNameMap.erase(nit);

	nit = m_cmdAliasMap.find(pCommand->GetAlias());
	if(nit != m_cmdAliasMap.end())
		m_cmdAliasMap.erase(nit);

	kiCommandArray::iterator ait = m_cmdArray.begin();
	for(;ait != m_cmdArray.end();ait ++)
	{
		if((*ait)->GetID() == pCommand->GetID())
			break;
	}
	if(ait != m_cmdArray.end())
		m_cmdArray.erase(ait);

	return TRUE;
}

//
kiCommand*	kiCommandMgr::FindRegisterCommand(UINT nID)
{
	kiCommand *pCommand = NULL;
	if(m_cmdIDMap.empty())
		return NULL;

	kiCommandIDMap::iterator it = m_cmdIDMap.find(nID);
	if(it != m_cmdIDMap.end())
		pCommand = (*it).second;

	return pCommand;
}

kiCommand*	kiCommandMgr::FindRegisterCommand(const std::string& strName)
{
	kiCommand *pCommand = NULL;
	if(m_cmdNameMap.empty())
		return NULL;

	kiCommandNameMap::iterator it = m_cmdNameMap.find(strName);
	if(it != m_cmdNameMap.end())
		pCommand = (*it).second;

	return pCommand;
}

kiCommand*	kiCommandMgr::FindRegisterCommandByAlias(const std::string& strAlias)
{
	kiCommand *pCommand = NULL;
	if(m_cmdAliasMap.empty())
		return NULL;

	kiCommandNameMap::iterator it = m_cmdAliasMap.find(strAlias);
	if(it != m_cmdAliasMap.end())
		pCommand = (*it).second;

	return pCommand;
}

//////////////////////////////////////////////////////////////////////////
//
//ִ������
int	kiCommandMgr::ExecCommand(UINT nID)
{
	if(HasCommandRunning())
	{//�����ɵġ�
		EndCommand(KSTA_CANCEL);
	}

	kiCommand *pCommand = FindRegisterCommand(nID);
	//����ע����󣬴Ӳ���л�ȡ
	if(!pCommand){
		pCommand = _pPluginMgr->GetCommand(nID);
		if(!pCommand){
			ASSERT(FALSE);
			return KSTA_ERROR;
		}
	}

	//��Ҫ�ĳ�ʼ��
	pCommand->Initialize(nID,_pModel,this);
	
	//�������Ҫ��¼����������Execute��Ҫ��������ʱ�����Ҳ���
	//��ǰִ�е�����޷���������
	_pExecCommand = pCommand;
	int iret = pCommand->Execute();//û������,�򵥵�ִ��
	if(iret == KSTA_DONE || iret == KSTA_ERROR)//ִ����ɻ�ʧ�ܣ���ֱ�ӽ����ˡ�
		return iret;
		
	return KSTA_CONTINUE;
}

int  kiCommandMgr::EndCommand(int nCode)
{
	int iret = nCode;
	if(_pExecCommand){
		iret = _pExecCommand->End(nCode);
		//!!!
		// ����Էǻ���������������ͷŶ�����˴�ʱ��������������
		// ���øú������ͷź󣬺����ײ���ָ�����⡣
		// �������������´�ִ�и�����ʱ�����ͷž����������������
		_pExecCommand = NULL;
	}

	return iret;
}

BOOL	kiCommandMgr::HasCommandRunning()
{
	if(_pExecCommand != NULL && _pExecCommand->IsRunning())
		return TRUE;
	return FALSE;
}

kcDocContext*	 kiCommandMgr::GetDocContext() const
{
	return pDocContext_;
}

kiSelSet* kiCommandMgr::GetGlobalSelSet() const
{
	return pGlobalSelSet_;
}

kiInputEdit* kiCommandMgr::GetInputEdit() const
{
	if(pDocContext_){
		return pDocContext_->GetInputEdit();
	}
	return NULL;
}

//���ù�����v3d_View
void  kiCommandMgr::SetV3dView(const Handle(V3d_View) &aView)
{
	aV3dView_ = aView;
}

const Handle(V3d_View)  kiCommandMgr::GetCurrV3dView() const
{
	if(pDocContext_){
		return pDocContext_->GetOcctView();
	}
	return aV3dView_;
}

//////////////////////////////////////////////////////////////////////////
//
//�����Ϣ
void  kiCommandMgr::OnLButtonDown(kuiMouseInput& mouseInput)
{
	if(_pExecCommand){
		_pExecCommand->OnLButtonDown(mouseInput);
	}
}

int  kiCommandMgr::OnLButtonUp(kuiMouseInput& mouseInput)
{
	int iret = KSTA_CONTINUE;
	if(_pExecCommand){
		iret = _pExecCommand->OnLButtonUp(mouseInput);
	}

	return iret;
}

int		kiCommandMgr::OnLButtonDblClk(kuiMouseInput& mouseInput)
{
	if(_pExecCommand){
		_pExecCommand->OnLButtonDblClk(mouseInput);
	}

	return KSTA_CONTINUE;
}

int		kiCommandMgr::OnMouseMove(kuiMouseInput& mouseInput)
{
	if(_pExecCommand){
		_pExecCommand->OnMouseMove(mouseInput);
	}

	return KSTA_CONTINUE;
}

void	kiCommandMgr::OnRButtonDown(kuiMouseInput& mouseInput)
{
	if(_pExecCommand){
		_pExecCommand->OnRButtonDown(mouseInput);
	}
}

// ��������ķ���ֵ����Ӱ�쵽����ִ�е�����
int  kiCommandMgr::OnRButtonUp(kuiMouseInput& mouseInput)
{
	int nErr = KSTA_CONTINUE;
	if(_pExecCommand){
		nErr = _pExecCommand->OnRButtonUp(mouseInput);
	}

	return nErr;
}

//////////////////////////////////////////////////////////////////////////
//������Ϣ
int  kiCommandMgr::OnKeyDown(kuiKeyInput& keyInput)
{
	int nResult = KSTA_CONTINUE;
	if(_pExecCommand){
		nResult = _pExecCommand->OnKeyDown(keyInput);
	}
	return nResult;
}

int  kiCommandMgr::OnKeyUp(kuiKeyInput& keyInput)
{
	int nResult = KSTA_CONTINUE;
	if(_pExecCommand){
		nResult = _pExecCommand->OnKeyUp(keyInput);
	}
	return nResult;
}

int  kiCommandMgr::OnChar(kuiKeyInput& keyInput)
{
	int nResult = KSTA_CONTINUE;
	if(_pExecCommand){
		nResult = _pExecCommand->OnChar(keyInput);
	}
	return nResult;
}

// ��commandbar��ȡ������.
int  kiCommandMgr::ReceiveInputString(const char* pszString)
{
	return KSTA_CONTINUE;
}