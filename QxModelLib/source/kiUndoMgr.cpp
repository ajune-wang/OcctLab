#include "StdAfx.h"
#include "kcEntity.h"
#include "kcModel.h"
#include "kcLayer.h"
#include "kiUndoMgr.h"

//////////////////////////////////////////////////////////////////////////
//
kiNewDelUndoItem::kiNewDelUndoItem(const char *lpszName,kcModel *pModel)
:kiUndoItem(lpszName,pModel)
{

}

kiNewDelUndoItem::~kiNewDelUndoItem()
{
	_aNewEntity.clear();
	_aDelEntity.clear();
}

bool	kiNewDelUndoItem::Undo()
{
	if(_pModel == NULL) return false;
	// ɾ���´����Ķ���
	int ix,isize = (int)_aNewEntity.size();
	for(ix = 0;ix < isize;ix ++)
		_pModel->DelEntity(_aNewEntity[ix]);

	// ���ɾ���Ķ���
	kcLayer *pLayer = NULL;
	isize = (int)_aDelEntity.size();
	for(ix = 0;ix < isize;ix ++)
	{
		pLayer = _aDelEntity[ix]->GetLayer();
		ASSERT(pLayer);
		_pModel->AddEntity(_aDelEntity[ix],pLayer);
	}
		
	return true;
}

bool	kiNewDelUndoItem::Redo()
{
	if(_pModel == NULL) return false;
	// ɾ����ɾ���Ķ���
	int ix,isize = (int)_aDelEntity.size();
	for(ix = 0;ix < isize;ix ++)
		_pModel->DelEntity(_aDelEntity[ix]);

	// ����´����Ķ���
	kcLayer *pLayer = NULL;
	isize = (int)_aNewEntity.size();
	for(ix = 0;ix < isize;ix ++)
	{
		pLayer = _aNewEntity[ix]->GetLayer();
		ASSERT(pLayer);
		_pModel->AddEntity(_aNewEntity[ix],pLayer);
	}

	return true;
}

bool	kiNewDelUndoItem::AddNewEntity(kcEntity *pEntity)
{
	int ix,isize = (int)_aNewEntity.size();
	for(ix = 0;ix < isize;ix ++)
	{
		if(_aNewEntity[ix] == pEntity)
			return false;
	}
	_aNewEntity.push_back(pEntity);
	return true;
}

bool	kiNewDelUndoItem::AddDelEntity(kcEntity *pEntity)
{
	int ix,isize = (int)_aDelEntity.size();
	for(ix = 0;ix < isize;ix ++)
	{
		if(_aDelEntity[ix] == pEntity)
			return false;
	}
	_aDelEntity.push_back(pEntity);
	return true;
}

//////////////////////////////////////////////////////////////////////////
//
kiModifyUndoItem::kiModifyUndoItem(const char *lpszName,kcModel *pModel,kiCommand *pCommand)
:kiUndoItem(lpszName,pModel)
{
}

kiModifyUndoItem::~kiModifyUndoItem()
{

}

bool	kiModifyUndoItem::Undo()
{
	return true;
}

bool	kiModifyUndoItem::Redo()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
//
kiUndoMgr::kiUndoMgr(kcModel *pModel)
:m_pModel(pModel)
{
	ASSERT(m_pModel);

	m_nUndoLimit = 500;
	m_nRedoLimit = 500;
}

kiUndoMgr::~kiUndoMgr(void)
{
	UnInitialize();
}

bool	kiUndoMgr::Initialize()
{
	return true;
}

bool	kiUndoMgr::UnInitialize()
{
	ClearAllItems();

	return true;
}

// ѹ��һ��undo�������ˣ���ȥ����ɵģ�����µġ�
//
bool	kiUndoMgr::Push(kiUndoItem *pItem)
{
	ASSERT(pItem);
	if(!pItem) return false;

	int isize = (int)m_undoStack.size();
	if(isize >= m_nUndoLimit)//����Ƿ���
	{
		kiUndoItem *pOldItem = m_undoStack.back();
		m_undoStack.pop_back();
		delete pOldItem;
	}
	//ѹ�뵽��ʼ
	m_undoStack.push_front(pItem);

	return true;
}

bool	kiUndoMgr::PushRedo(kiUndoItem *pItem)
{
	ASSERT(pItem);
	int isize = (int)m_redoStack.size();
	if(isize >= m_nRedoLimit)
	{
		kiUndoItem *pOldItem = m_redoStack.back();
		m_redoStack.pop_back();
		delete pOldItem;
	}
	m_redoStack.push_front(pItem);
	return true;
}

// undoһ����
//
bool	kiUndoMgr::Undo()
{
	if(m_undoStack.empty())
		return true;

	kiUndoItem *pItem = m_undoStack.front();
	m_undoStack.pop_front();
	//ִ��undo
	if(pItem->Undo())
	{
		//��Ϊredo
		PushRedo(pItem);
	}
	else
	{
		//ִ��ʧ�ܣ���ɾ��
		delete pItem;
		return false;
	}
	
	return true;
}

// redoһ��
//
bool	kiUndoMgr::Redo()
{
	if(m_redoStack.empty())
		return true;

	kiUndoItem *pItem = m_redoStack.front();
	m_redoStack.pop_front();
	//
	if(pItem->Redo())
	{
		Push(pItem);
	}
	else
	{
		delete pItem;
		return false;
	}

	return true;
}

bool	kiUndoMgr::CanUndo() const
{
	return (m_undoStack.empty() ? false : true);
}

bool	kiUndoMgr::CanRedo() const
{
	return (m_redoStack.empty() ? false : true);
}

//�������
void	kiUndoMgr::ClearAllItems()
{
	kiUndoItemStack::iterator ite;
	if(!m_undoStack.empty())
	{
		for(ite = m_undoStack.begin();ite != m_undoStack.end();ite ++)
			delete *ite;
		m_undoStack.clear();
	}
	
	if(!m_redoStack.empty())
	{
		for(ite = m_redoStack.begin();ite != m_redoStack.end();ite ++)
			delete *ite;
		m_redoStack.clear();
	}
}

void	kiUndoMgr::Clear()
{
	ClearAllItems();
}