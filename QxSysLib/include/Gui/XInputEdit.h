#pragma once

#include <vector>
#include <stack>
#include "kiOptionSet.h"
#include "kiInputEdit.h"
#include "QxLibDef.h"

//��¼һ������������Ϣ
class QXLIB_API XInputRequest{
public:
	XInputRequest();
	XInputRequest(const char *pszPrompt,kiInputEditCallBack *pCallBack,kiOptionSet *pOptSet);
	XInputRequest(const XInputRequest& other);

	XInputRequest&			operator=(const XInputRequest& other);

public:
	std::string				_ssPromptText;
	kiInputEditCallBack		*_pInputEditCallBack;
	kiOptionSet				*_pOptionSet;
};

typedef std::stack<XInputRequest>	stdInputRequestStack;

//��¼һ��option��λ����Ϣ
class XOptionPos{
public:
	XOptionPos(){
		_rect.left = _rect.top = _rect.right = _rect.bottom = 0;
		_bHilight = false;
	}

public:
	RECT				_rect;//option��edit��ռ�ݵ�����Χ
	bool				_bHilight;//�Ƿ������ʾ������ѡ�С������
};

// ��¼һ��char��λ����Ϣ��Ϊ����caret���㡣
class XCharPos{
public:
	XCharPos(){
		_nStart = _nEnd = 0;
	}

public:
	int				_nStart,_nEnd;//��Edit�ڵ���ʼ����ֹλ�ã���λ�ǣ�����
};

// CXInputEdit

class QXLIB_API CXInputEdit : public CWnd,public kiInputEdit
{
	DECLARE_DYNAMIC(CXInputEdit)

public:
	CXInputEdit();
	virtual ~CXInputEdit();

	// ��Ҫ�ĳ�ʼ��
	BOOL						Initialize();

	//��Ҫ�ӿڣ�Ϊ���빤����ʾһ����ʾ��Ϣ�����Դ�ѡ���б�
	//InputToolָ�벻��ΪNULL.
	virtual int				RequestInput(const char *pPromptText,
										 kiInputEditCallBack *pEditCallBack,
										 kiOptionSet *pOptionSet = NULL);
	//��ʾһ����ʾ��Ϣ.
	virtual void				PromptText(const char *lpszText);

	//��������
	virtual void				Update();

	//��ȡ�����ı�
	virtual std::string		GetInputText() const;		

	//���ڱ��浱ǰ��������ͻָ���������
	virtual int				PushRequest();
	virtual int				PopRequest();

protected:
	// ������ʾ�ı�����Ϣ
	BOOL						CalcPromptTextInfo();
	//
	void						CalcCaretPos(CPoint point);

	void						UpdateCaret();

	// ����һ���ַ�
	int						AcceptChar(char c);
	int						DeleteChar(BOOL bBack);

protected:
	//��ʾ�ı���Ϣ
	CString					m_szPromptText;//������ʾ�ı�
	CString					m_szFullPromptText;//������ѡ����Ϣ����������ʾ�ı������ɸ��ġ�
	CRect					m_promptRect;//��ʾ�ı��ķ�Χ
	
	//�����ı���Ϣ
	CString					m_szInputText;//�û����������ı�
	std::vector<XCharPos>		m_chrPosArray;//��¼���������ֵ�ÿ���ַ���λ����Ϣ
	int						m_nCaretIdx;//Caret��Ӧ��chrPosArray���±ꡣ0-size,size��ʾ���

	CString					m_szFullText;//����������ı�������: prompt : input

protected:
	std::vector<XOptionPos>	m_optPosArray;//��¼option��λ����Ϣ

	int						HitOptionTest(CPoint point);

protected:
	CFont					m_textFont; //�ı�����
	COLORREF					m_selOptionColor;//ѡ�е�option����ɫ��
	int						m_nCaretHeight;//���ĸ߶�
	int						m_nMargin;//��϶,������ߺ��ϱ�.
	BOOL						m_bHasFocus;//�Ƿ��н���

protected:
	stdInputRequestStack		m_inputReqStack;//��������ջ
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


