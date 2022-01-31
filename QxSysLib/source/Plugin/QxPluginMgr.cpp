#include "StdAfx.h"
#include "QxCommandItem.h"
#include "kiCommand.h"
#include "QxCmdPlugin.h"
#include "QxPluginMgr.h"

QxPluginMgr::QxPluginMgr(void)
{
	_uiCurrentID = KI_CMD_ID_BEGIN;
}

QxPluginMgr::~QxPluginMgr(void)
{
	Destroy();
}

int  QxPluginMgr::Initialize()
{
	return 1;
}

void  QxPluginMgr::Destroy()
{
	//ɾ����̬�����
	QxCmdPlugin *pPlugin = NULL;
	size_t ix,isize = aPluginArray_.size();
	for(ix = 0;ix < isize;ix ++){
		pPlugin = aPluginArray_.at(ix);
		pPlugin->Release();
		delete pPlugin;
	}
	aPluginArray_.clear();

	//����ͷŶ�̬�⣬��Ϊǰ��Ҫ�õ�
	QxDynPluginData *pData = NULL;
	isize = aDynPluginArray_.size();
	for(ix = 0;ix < isize;ix ++){
		pData = aDynPluginArray_[ix];
		pData->FreePluginDll();
		delete pData;
	}
	aDynPluginArray_.clear();
}

//����һ�����,������Ҫ��������Ļ�����Ϣ
//
BOOL  QxPluginMgr::LoadPlugin(QxCmdPlugin *pPlugin)
{
	if(!pPlugin)
		return FALSE;

	//�������Ӧ�����ظ�
	if(FindPlugin(pPlugin->GetName()) != NULL){
		ASSERT(FALSE);
		return FALSE;
	}

	if(!pPlugin->Initialize())
		return FALSE;

	//������������Ӧ��ID
	pPlugin->GenCommandID(_uiCurrentID);
	_uiCurrentID += pPlugin->GetReserveNum();//��һ������ֵ

	//�����е�item����¼��һ�������У�����˵�����
	int ix,iCount = pPlugin->GetItemCount();
	QxCommandItem *pCmdItem = NULL;
	for(ix = 0;ix < iCount;ix ++){
		pCmdItem = pPlugin->GetCmdItem(ix);
		_cmdItemArray.push_back(pCmdItem);
	}

	//�����¼��������
	aPluginArray_.push_back(pPlugin);

	return TRUE;
}

// ж��һ�����,�ݲ�ʵ��
//
BOOL	QxPluginMgr::UnloadPlugin(QxCmdPlugin *pPlugin)
{
	return TRUE;
}

QxCmdPlugin*	QxPluginMgr::FindPlugin(const std::string& strName)
{
	int ix,isize = (int)aPluginArray_.size();
	for(ix = 0;ix < isize;ix ++){
		const std::string& strTempName = aPluginArray_[ix]->GetName();
		ASSERT(!strTempName.empty());
		if(strTempName == strName)
			return aPluginArray_[ix];
	}
	return NULL;
}

//��������id��ȡ����
kiCommand*  QxPluginMgr::GetCommand(int nCmdID)
{
	kiCommand *pCommand = NULL;
	kuiPluginArray::size_type ix,isize = aPluginArray_.size();

	for(ix = 0;ix < isize;ix ++){
		pCommand = aPluginArray_[ix]->GetCommand(nCmdID);
		if(pCommand)
			return pCommand;
	}

	return NULL;
}

// �ͷŸ��������ǻ�������ᱻfree��
//
void  QxPluginMgr::ReleaseCommand(kiCommand *pCommand)
{
	kuiPluginArray::size_type ix,isize = aPluginArray_.size();

	for(ix = 0;ix < isize;ix ++){
		if(aPluginArray_[ix]->ReleaseCommand(pCommand))
			break;
	}
}

int  QxPluginMgr::GetItemCount() const
{
	return (int)_cmdItemArray.size();
}

QxCommandItem*  QxPluginMgr::GetCmdItem(int ix)
{
	if(ix < 0 || ix >= (int)_cmdItemArray.size())
		return NULL;
	return _cmdItemArray[ix];
}

//////////////////////////////////////////////////////////////////////////
//
QxDynPluginData::QxDynPluginData(HINSTANCE hDll)
	:pDllModule_(hDll)
{
	pMainProc_ = NULL;
	pFreeProc_ = NULL;
	pImpl_ = NULL;
}

QxDynPluginData::~QxDynPluginData()
{
	
}

void QxDynPluginData::FreePluginDll()
{
	if(pDllModule_){
		::FreeLibrary(pDllModule_);
		pDllModule_ = NULL;
	}
}

//ͨ����̬�⣬����һ�����
BOOL QxPluginMgr::LoadDynPlugin(const char *pszDllPath)
{
	if(!pszDllPath)
		return FALSE;

	HINSTANCE hDll = ::LoadLibrary(pszDllPath);
	if(!hDll || hDll == INVALID_HANDLE_VALUE)
		return FALSE;

	QxCmdPluginMainProc mainProc = (QxCmdPluginMainProc)::GetProcAddress(hDll,"QxCmdPluginMain");
	QxCmdPluginFreeProc freeProc = (QxCmdPluginFreeProc)::GetProcAddress(hDll,"QxCmdPluginFree");
	if(!mainProc || !freeProc){
		::FreeLibrary(hDll);
		return FALSE;
	}

	// ��ȡ���ʵ�֣�����ʼ��
	QxCmdPluginCfg plgCfg;
	QxCmdPluginImpl *pPlgImpl = mainProc(&plgCfg);
	if(!pPlgImpl){
		::FreeLibrary(hDll);
		return FALSE;
	}

	QxDynPluginData *pData = new QxDynPluginData(hDll);
	pData->pMainProc_ = mainProc;
	pData->pFreeProc_ = freeProc;
	pData->pImpl_ = pPlgImpl;

	aDynPluginArray_.push_back(pData);

	//������Ӧ��plugin
	QxDynCmdPlugin *pPlugin = new QxDynCmdPlugin(pPlgImpl);
	
	return LoadPlugin(pPlugin);
}
