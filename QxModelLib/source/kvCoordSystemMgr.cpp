#include "StdAfx.h"
#include "kuiInterface.h"
//#include "ModelTreeCtrl.h"
#include "kvCoordSystemMgr.h"

DWORD kvCoordSystemMgr::m_dwNextNameNo = 1;//0����
//��������
std::string	kvCoordSystemMgr::GenCoordSystemName()
{
	char szbuf[128];
	sprintf_s(szbuf,128,"����ϵ %d",m_dwNextNameNo);
	std::string str = szbuf;
	m_dwNextNameNo ++;

	return str;
}

kvCoordSystemMgr::kvCoordSystemMgr(void)
{
	m_dwCurrCS = 0;
	m_pModelTree = NULL;
}

kvCoordSystemMgr::~kvCoordSystemMgr(void)
{
	Destroy();
}

// ��ʼ��,���Ĭ�ϵ���������ϵ.
//
bool	kvCoordSystemMgr::Initialize()
{
	kvCoordSystem wcs;
	wcs.SetName("��������ϵ");
	AddCoordSystem(wcs);

	m_dwCurrCS = 0;
	if(m_pModelTree){
		m_pModelTree->BoldTreeItem(eCoordSystemItem,"��������ϵ");
	}

	return true;
}

void	kvCoordSystemMgr::Destroy()
{
	Clear();
}

void	kvCoordSystemMgr::SetModelTreeInterface(kuiModelTreeInterface *pModelTree)
{
	m_pModelTree = pModelTree;
	//
	if(pModelTree)
	{
		//������е�����ϵ
		int ix,isize = (int)m_csArray.size();
		for(ix = 0;ix < isize;ix ++)
		{
			kvCoordSystem *pCS = m_csArray[ix];
			//m_pModelTree->AddCoordSystem(pCS->GetName(),pCS);
		}
	}
}

void	kvCoordSystemMgr::Clear()
{
	int ix,isize = (int)m_csArray.size();
	for(ix = 0;ix < isize;ix ++)
		delete m_csArray[ix];
	m_csArray.clear();

	m_dwCurrCS = 0;
}

// ���һ������ϵ
kvCoordSystem*	kvCoordSystemMgr::AddCoordSystem(const kvCoordSystem& cs)
{
	ASSERT(cs.GetName());
	if(!cs.GetName())
		return NULL;

	kvCoordSystem *pCS = new kvCoordSystem;
	if(!pCS)
		return NULL;

	pCS->SetName(cs.GetName());
	pCS->Set(cs.Org(),cs.X(),cs.Y(),cs.Z());
	m_csArray.push_back(pCS);

	if(m_pModelTree)
	{
		//m_pModelTree->AddCoordSystem(cs.GetName(),pCS);
	}

	return pCS;
}

bool	kvCoordSystemMgr::RemoveCoordSystem(const kvCoordSystem *pCS)
{
	DWORD ix = 0,isize = (DWORD)m_csArray.size();
	for(ix = 0;ix < isize;ix ++)
	{
		if(m_csArray[ix] == pCS)
			break;
	}
	if(ix >= isize)
		return false;

	if(m_pModelTree)
	{
		//m_pModelTree->RemoveCoordSystem(pCS->GetName());
	}

	m_csArray.erase(m_csArray.begin() + ix);
	if(m_dwCurrCS == ix)
		m_dwCurrCS = 0;

	return true;
}

kvCoordSystem*	kvCoordSystemMgr::FindCoordSystem(const char *pszName)
{
	TCoordSystemArray::size_type ix,isize = m_csArray.size();
	for(ix = 0;ix < isize;ix ++){
		if(m_csArray[ix]->HasName(pszName))
			return m_csArray[ix];
	}
	return NULL;
}

// ��ȡ��ǰ����ϵ
kvCoordSystem*	kvCoordSystemMgr::GetCurrentCS() const
{
	return m_csArray[m_dwCurrCS];
}

// ���õ�ǰ����ϵ
bool	kvCoordSystemMgr::SetCurrentCS(const kvCoordSystem *pCS)
{
	DWORD ix = 0,isize = (DWORD)m_csArray.size();
	for(ix = 0;ix < isize;ix ++)
	{
		if(m_csArray[ix] == pCS)
			break;
	}
	if(ix >= isize)
		return false;

	m_dwCurrCS = ix;
	if(m_pModelTree){
		m_pModelTree->UnBoldAllItem(eCoordSystemItem);
		m_pModelTree->BoldTreeItem(eCoordSystemItem,pCS->GetName());
	}

	return true;
}

int		kvCoordSystemMgr::GetCount() const
{
	return (int)m_csArray.size();
}

kvCoordSystem*	kvCoordSystemMgr::GetCoordSystem(int idx)
{
	ASSERT(idx >= 0 && idx < (int)m_csArray.size());
	return m_csArray[idx];
}