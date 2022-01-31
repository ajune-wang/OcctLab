// OcctTryDoc.h :  COcctLabDoc ��Ľӿ�
//
#pragma once

#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>

class kcModel;
class kiSnapMngr;
class kcSnapMngr;
class kiUndoMgr;
class kcContext;
class kiCommandMgr;
class kiSelSet;
class kcDocContext;

class COcctLabDoc : public CDocument
{
protected: // �������л�����
	COcctLabDoc();
	DECLARE_DYNCREATE(COcctLabDoc)

// ����
public:
	Handle(AIS_InteractiveContext)& GetAISContext(){ return m_AISContext; };
	Handle(V3d_Viewer)		GetViewer()  { return m_Viewer; };
	
	kcDocContext*				GetDocContext() const { return pDocContext_; }

	kcModel*					GetModel() { return m_pModel; }
	kiCommandMgr*				GetCommandMgr() const { return m_pCommandMgr; }
	kcSnapMngr*				GetSnapMgr() { return m_pSnapMgr; }
	// ��ȡȫ��ѡ�񼯡�
	kiSelSet*				GetGlobalSelSet() const; 
	BOOL						IsNewDocument() const { return m_bNewDocument; }

	// ��ʼ�����������,��Ϊʹ����InputEdit����Ҫ��View��ʼ��ʱ����
	BOOL						InitCommandMgr();

// ����
public:

// ��д
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~COcctLabDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//������Ҫ�ĳ�Ա.��Doc�������������
	Handle(V3d_Viewer)		m_Viewer;  //�������е�view.
	Handle(AIS_InteractiveContext) m_AISContext;//������.
	// �ĵ��������.
	kcModel					*m_pModel;//ģ�Ͷ���ÿ��Doc�ĵ�һ��ģ�Ͷ���
	kiCommandMgr				*m_pCommandMgr;//���������,ÿ��Docһ������
	kcSnapMngr				*m_pSnapMgr;

	//��¼������Doc��Ҫ������ݵĶ���
	kcDocContext				*pDocContext_;

	BOOL						m_bNewDocument;//�Ƿ��½��ĵ�
	BOOL						m_bSerializeOK;//�Ƿ񱣴桢��ȡ�ɹ�

	BOOL						OcctInit();
	
// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

	//ע��ϵͳ����
	BOOL						RegisterSysCommand();
	void						OnMenuItemDispatch(UINT nID);

public:
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI *pCmdUI);
	afx_msg void OnMenuImportIges();
	afx_msg void OnMenuImportDxf();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	virtual void OnCloseDocument();
	virtual void SetTitle(LPCTSTR lpszTitle);
	afx_msg void OnImportBrepFile();
	afx_msg void OnImportStepFile();
	afx_msg void OnMenuExportStep();
	afx_msg void OnMenuExportBrep();
	afx_msg void OnMenuExportSelstep();
	afx_msg void OnMenuExportSelbrep();
	afx_msg void OnMenuExportIges();
	afx_msg void OnExportSelIges();
};


