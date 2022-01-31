// MainFrm.h : CMainFrame ��Ľӿ�
//
#ifndef _MAIN_FRAME_H_
#define _MAIN_FRAME_H_

#include "autoHideBar.h"
#include "kuiCmdBar.h"
#include "kuiMainBar.h"

class CXInputEdit;
class kuiModelTreeInterface;
class QxCommandItem;
class QxMainWndTimerMgr;

class CMainFrame : public CFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:

// ����
public:
	int						ShowInputString(LPCTSTR lpszInputStr);
	int						ShowPrompt(LPCTSTR lpszPrompt);

	CXInputEdit*				GetInputEdit();
	kuiModelTreeInterface		*GetModelTreeInterface();
	QxMainWndTimerMgr*		GetTimerMgr() const;
	
// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CStatusBar		m_wndStatusBar;
	CToolBar		m_wndToolBar;
	kuiCmdBar		m_wndCmdBar;
	kuiMainBar		m_wndMainBar;

	CImageList		m_wndBarImgList;

	QxMainWndTimerMgr	*m_pTimerMgr;

protected:
	// ���ݲ�������˵�
	BOOL			CreateMainMenu();

	CMenu*			FindSubMenu(CMenu *pMenu,const char* pszName);
	int				FindMenuItem(CMenu *pMenu,const char* pszName);
	const char*		DoGetTopMenuText(QxCommandItem *pItem);

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	DECLARE_PINDOCK()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
public:
	afx_msg void OnMenuLuaScript();
	afx_msg void OnComTestAdd();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

void ComTest_Add();

#endif
