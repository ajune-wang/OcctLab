// OcctTryView.h : COcctLabView ��Ľӿ�
//


#pragma once

#include <V3d_View.hxx>
#include <AIS_RubberBand.hxx>

class kcBasePlane;
class kvGrid;
class kiCommandMgr;
class COcctLabDoc;
class kuiModelTreeInterface;

class COcctLabView : public CView
{
protected: // �������л�����
	COcctLabView();
	DECLARE_DYNCREATE(COcctLabView)

// ����
public:
	COcctLabDoc*		GetDocument() const;

	Handle(V3d_View)	GetV3dView() const { return hOccView_; }

// ����
public:
	// ���±���ɫ
	void				UpdateBackGroundColor();

	// ��ʾ������
	void				UpdateTriedronDisplay();

protected:
	//
	void				UpdateSnapScreenPoint();
	void				ShowStatusPrompt(LPCTSTR lpszPrompt);

// ��д
	public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~COcctLabView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	Handle(V3d_View)				hOccView_; //
	Handle(AIS_InteractiveContext)	m_aisContext;//������¼����
	kiCommandMgr					*m_pCommandMgr;//������¼ָ�롣
	kuiModelTreeInterface			*m_pIModelTree;//
	int							m_nPreX,m_nPreY;//ǰһ�����λ�á�
	int							m_nStartX,m_nStartY;//��ʼ��X��Y��
	int							m_nRPreX,m_nRPreY;//ǰһ���Ҽ�����λ��
	BOOL							m_bPanning;//�Ƿ���ƽ��
	Handle(AIS_RubberBand)		aDrawRect_;//���Ƶ�ѡ�����

	double						dViewSize_; //v3d_view��size
	Quantity_Color				aBackGroundColor_;

	//��ʼ��view
	BOOL							InitV3dView();
	bool							CreateV3dView();
	//
	BOOL							InitInputEdit();
	//
	BOOL							InitModelTree();

	kcBasePlane*					GetCurrentBasePlane() const;

protected:
	enum eLineStyle { LS_Solid, LS_Dot, LS_ShortDash, LS_LongDash, LS_Default };
	CPen*  m_Pen;

	virtual void			DrawRectangle (const Standard_Integer  MinX  ,
		const Standard_Integer  MinY  ,
		const Standard_Integer  MaxX  ,
		const Standard_Integer  MaxY  ,
		BOOL  bDraw  ,
		const eLineStyle aLineStyle = LS_Default);

	virtual void			DrawRectangleEx (const Standard_Integer  MinX  ,
		const Standard_Integer  MinY  ,
		const Standard_Integer  MaxX  ,
		const Standard_Integer  MaxY  ,
		BOOL  bDraw  ,
		const eLineStyle aLineStyle = LS_Default);


// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMenuViewFront();
	afx_msg void OnMenuViewBack();
	afx_msg void OnMenuViewLeft();
	afx_msg void OnMenuViewRight();
	afx_msg void OnMenuViewTop();
	afx_msg void OnMenuViewBottom();
	afx_msg void OnMenuViewRestore();

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	afx_msg void OnMenuDumpView();
	afx_msg void OnMenuShapeStruct();
	afx_msg void OnMenuFitall();
	afx_msg void OnMenuDispSeneitive();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
};

#ifndef _DEBUG  // OcctLabView.cpp �ĵ��԰汾
inline COcctLabDoc* COcctLabView::GetDocument() const
   { return reinterpret_cast<COcctLabDoc*>(m_pDocument); }
#endif

