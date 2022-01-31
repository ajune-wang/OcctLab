//////////////////////////////////////////////////////////////////////////
//
#ifndef _KI_UNDOMGR_H_
#define _KI_UNDOMGR_H_

#include <list>
#include <vector>
#include <string>
#include "QxLibDef.h"

class kcEntity;
class kcModel;
class kiCommand;

#define KCT_UNDO_NEWDEL			1
#define KCT_UNDO_MODIFY			2

class kiUndoItem;
	class kiNewDelUndoItem;
	class kiModifyUndoItem;

typedef std::list<kiUndoItem *>  kiUndoItemStack;

//��¼һ��undo��redo��
//
class kiUndoItem{
public:
	kiUndoItem(const char *lpszName,kcModel *pModel) {
		_name = lpszName;
		_pModel = pModel;
	}

	virtual int					GetType() const = 0;

	virtual bool				Undo() = 0;
	virtual bool				Redo() = 0;

	const char*					GetName() const { return _name.c_str(); }

protected:
	std::string					_name;//undo����
	kcModel						*_pModel;
};

class QXLIB_API kiNewDelUndoItem : public kiUndoItem{
public:
	kiNewDelUndoItem(const char *lpszName,kcModel *pModel);
	~kiNewDelUndoItem();

	virtual int					GetType() const { return KCT_UNDO_NEWDEL; }

	bool						AddNewEntity(kcEntity *pEntity);
	bool						AddDelEntity(kcEntity *pEntity);

	virtual bool				Undo();
	virtual bool				Redo();

protected:
	std::vector<kcEntity *>		_aNewEntity;//�´����Ķ���
	std::vector<kcEntity *>		_aDelEntity;//ɾ���Ķ���
};

class QXLIB_API kiModifyUndoItem : public kiUndoItem{
public:
	kiModifyUndoItem(const char *lpszName,kcModel *pModel,kiCommand *pCommand);
	~kiModifyUndoItem();

	virtual int					GetType() const { return KCT_UNDO_MODIFY; }

	virtual bool				Undo();
	virtual bool				Redo();

protected:
	kiCommand					*_pCommand;
	std::vector<kcEntity *>		_aEntity;//���޸ĵĶ���
};

class QXLIB_API kiUndoMgr
{
public:
	kiUndoMgr(kcModel *pModel);
	~kiUndoMgr(void);

	bool						Initialize();
	bool						UnInitialize();

	// ѹ��һ��undo�
	bool						Push(kiUndoItem *pItem);

	//
	bool						Undo();
	bool						Redo();

	bool						CanUndo() const;
	bool						CanRedo() const;

	void						Clear();

protected:
	//
	bool						PushRedo(kiUndoItem *pItem);
	//�������
	void						ClearAllItems();

protected:
	kcModel						*m_pModel;//����ģ�͡�
    kiUndoItemStack				m_undoStack;//ջ
	kiUndoItemStack				m_redoStack;//ջ
	int							m_nUndoLimit;//undo������
	int							m_nRedoLimit;//redo������
};

#endif