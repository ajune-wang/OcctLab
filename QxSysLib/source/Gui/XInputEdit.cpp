// XInputEdit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "kiInputTool.h"
#include "XInputEdit.h"

////////////////////////////////////////////////////////////
//
XInputRequest::XInputRequest()
{
	_ssPromptText = "";
	_pInputEditCallBack = NULL;
	_pOptionSet = NULL;
}

XInputRequest::XInputRequest(const char *pszPrompt,kiInputEditCallBack *pCallBack,kiOptionSet *pOptSet)
{
	_ssPromptText = pszPrompt;
	_pInputEditCallBack = pCallBack;
	_pOptionSet = pOptSet;
}

XInputRequest::XInputRequest(const XInputRequest& other)
{
	*this = other;
}

XInputRequest& XInputRequest::operator=(const XInputRequest& other)
{
	_ssPromptText = other._ssPromptText;
	_pInputEditCallBack = other._pInputEditCallBack;
	_pOptionSet = other._pOptionSet;

	return *this;
}

////////////////////////////////////////////////////////////
// CXInputEdit

IMPLEMENT_DYNAMIC(CXInputEdit, CWnd)
CXInputEdit::CXInputEdit()
{
	m_szPromptText = "";
	m_szFullPromptText = "";
	m_szInputText = "";
	m_szFullText = "";
	m_promptRect.SetRectEmpty();
	m_bHasFocus = FALSE;

	m_selOptionColor = RGB(0,0,255);
	m_nCaretHeight = 0;
	m_nMargin = -1;

	_pInputEditCallBack = NULL;
	m_pOptionSet = NULL;
}

CXInputEdit::~CXInputEdit()
{
}

// ��Ҫ�ĳ�ʼ��
//
BOOL	CXInputEdit::Initialize()
{
	if(m_nMargin >= 0)//�Ѿ���ʼ�����ˡ�
		return TRUE;

	//�����Ҫ�Ĳ�����Ϣ
	CRect rcClient;
	GetClientRect(&rcClient);
	if(rcClient.Width() <= 0)
		return FALSE;

	TEXTMETRIC tm;
	CClientDC dc(this);
	CFont *pOldFont = dc.SelectObject(&m_textFont);
	dc.GetTextMetrics(&tm);
	dc.SelectObject(pOldFont);

	//�������ֵ����¼�϶�͹��ĸ߶�
	m_nMargin = (rcClient.Height() - tm.tmHeight) / 2;
	m_nCaretHeight = tm.tmHeight + 2;
	
	//��Ҫ�ĳ�ʼ��
	m_pOptionSet = NULL;
	m_promptRect.SetRectEmpty();

	return TRUE;
}

//��ʾһ����ʾ��Ϣ���ȴ��û���������
// ��Ϣ�п��԰������ѡ�һ���ı���Ϊ���������֣�
//  ��ʾ��Ϣ ���ѡ�� : �����ı�
//  ���pOptionSetΪ�գ���û��ѡ���ı�
//
int		CXInputEdit::RequestInput(const char *pPromptText,
					 			  kiInputEditCallBack *pEditCallBack,
								  kiOptionSet *pOptionSet)
{
	ASSERT(pPromptText && pEditCallBack);
	ASSERT(m_nMargin > 0);//�Ѿ���ʼ����

	//����ɵ�
	m_szInputText = _T("");
	m_szFullText = _T("");
	m_chrPosArray.clear();
	m_optPosArray.clear();

	//���¼���
	m_szPromptText = pPromptText;
	_pInputEditCallBack = pEditCallBack;
	m_pOptionSet = pOptionSet;
	if(m_pOptionSet){
		m_pOptionSet->SetInputEdit(this);//��Ҫ�Ĺ���
	}

	CalcPromptTextInfo();
	m_szFullText = m_szFullPromptText;

	m_nCaretIdx = 0;
	Invalidate(FALSE);

	return 1;
}

// ��ʾһ���ı�����Ҫ������
//
void  CXInputEdit::PromptText(const char *lpszText)
{
	ASSERT(lpszText);
	//����ɵ�
	m_szInputText = _T("");
	m_szFullText = _T("");
	m_chrPosArray.clear();
	m_optPosArray.clear();

	m_szPromptText = lpszText;
	_pInputEditCallBack = NULL;
	m_pOptionSet = NULL;

	//���������ı��Ĵ�С����λcaret��
	CalcPromptTextInfo();
	m_szFullText = m_szFullPromptText;

	m_nCaretIdx = 0;

	//�ػ���
	Invalidate(FALSE);
}

// ��������,��Ҫ�������ֵ���ʾ��ѡ���ֵ
//
void  CXInputEdit::Update()
{
	if(!_pInputEditCallBack){
		ASSERT(FALSE);
		return;
	}

	//������������ı���Ϣ
	m_szFullPromptText = "";
	
	//���¼���
	CalcPromptTextInfo();
	//���¹��������ı���
	m_szFullText = m_szFullPromptText + m_szInputText;

	Invalidate(FALSE);
}

//��ȡ������ı�
std::string  CXInputEdit::GetInputText() const
{
	std::string szText = "";
	if(!m_szInputText.IsEmpty()){
		int ix,len = m_szInputText.GetLength();
		for(ix = 0;ix < len;ix ++){
			szText += m_szInputText.GetAt(ix);
		}
	}
	return szText;
}

// ������ʾ�ı���ѡ�������ʾ�ı�����Ϣ����Ҫ�ǣ�
//  ��Edit����ռ������Χ
//  ѡ���λ����Ϣ��
//
BOOL  CXInputEdit::CalcPromptTextInfo()
{
	ASSERT(m_nMargin > 0);
	ASSERT(!m_szPromptText.IsEmpty());
	//����ɵ�
	m_optPosArray.clear();
	m_promptRect.SetRectEmpty();

	CSize sz;
	CClientDC dc(this);
	CFont *pOldFont = dc.SelectObject(&m_textFont);

	if(!m_pOptionSet){//û��ѡ��
		m_szFullPromptText = m_szPromptText + " : ";//���Ϸָ��
	}else{
		std::string ssOptText;
		int ix,nbOpt = m_pOptionSet->GetCount();

		m_szFullPromptText = m_szPromptText;
		m_szFullPromptText += " ( ";//��ʼ�ַ�

		for(ix = 0;ix < nbOpt;ix ++){
			kiOptionItem  *pItem = m_pOptionSet->GetOption(ix);
			XOptionPos optPos;

			sz = dc.GetTextExtent(m_szFullPromptText);
			//��¼option�ķ�Χ
			optPos._rect.left = sz.cx + m_nMargin;
			optPos._rect.top = m_nMargin;
			optPos._rect.bottom = m_nMargin + sz.cy;

			//�����ѡ���ı����ٴμ��㷶Χ
			pItem->GetPromptText(ssOptText);
			m_szFullPromptText += ssOptText.c_str();
			sz = dc.GetTextExtent(m_szFullPromptText);
			optPos._rect.right = m_nMargin + sz.cx;

			if(ix < nbOpt - 1)
				m_szFullPromptText += "  ";

			m_optPosArray.push_back(optPos);
		}
		m_szFullPromptText += ") : ";
	}

	//��¼����ѡ�����ʾ�ı���������Χ
	sz = dc.GetTextExtent(m_szFullPromptText);
	m_promptRect.left = m_nMargin;
	m_promptRect.top = m_nMargin;
	m_promptRect.right = m_nMargin + sz.cx;
	m_promptRect.bottom = m_nMargin + sz.cy;
	
	dc.SelectObject(pOldFont);

	return TRUE;
}

//��һ����������ѹ��ջ�У��Ա����ָ�������
//
int  CXInputEdit::PushRequest()
{
	if(!m_szPromptText.IsEmpty()){
		XInputRequest inputReq(m_szPromptText,_pInputEditCallBack,m_pOptionSet);
		m_inputReqStack.push(inputReq);

		return 1;
	}

	return 0;
}

// ��ջ�лָ�һ��ǰ�����������
//
int  CXInputEdit::PopRequest()
{
	if(!m_inputReqStack.empty()){
		XInputRequest inputReq = m_inputReqStack.top();
		//��������
		this->RequestInput(inputReq._ssPromptText.c_str(),inputReq._pInputEditCallBack,inputReq._pOptionSet);
		
		m_inputReqStack.pop();

		return 1;
	}

	return 0;
}

///////////////////////////////////////////////////////////
//

BEGIN_MESSAGE_MAP(CXInputEdit, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONUP()
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CXInputEdit ��Ϣ�������
int  CXInputEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// �����ض�������
	CFont font;
	LOGFONT lf;
	memset(&lf,0,sizeof(LOGFONT));
	lf.lfHeight = 12;
	strcpy_s(lf.lfFaceName,32,"����");
	m_textFont.CreateFontIndirect(&lf);

	return 0;
}

void CXInputEdit::OnDestroy()
{
	CWnd::OnDestroy();

	if(m_textFont.GetSafeHandle())
		m_textFont.DeleteObject();
}

// ����Edit�����򣬸�����ʾ��ǰѡ��
//
void CXInputEdit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect cliRect;
	GetClientRect(&cliRect);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	ASSERT(memDC.GetSafeHdc());

	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dc,cliRect.Width(),cliRect.Height());
	ASSERT(memBmp.GetSafeHandle());

	CBitmap *pOldBmp = memDC.SelectObject(&memBmp);
	CFont *pOldFont = memDC.SelectObject(&m_textFont);

	//���
	memDC.FillSolidRect(&cliRect,RGB(255,255,255));

	//�����ı�,�����������ı�����������ʾ��Ϣ��ѡ�
	if(!m_szFullText.IsEmpty())
		memDC.TextOut(m_nMargin,m_nMargin,m_szFullText);

	//����ѡ�е�ѡ���������ƻ��������»���һ�Ρ�
	if(!m_optPosArray.empty()){
		CString szName;
		std::string str;
		int ix,isize = (int)m_optPosArray.size();
		COLORREF textcol = memDC.GetTextColor();

		for(ix = 0;ix < isize;ix ++){
			XOptionPos& optPos = m_optPosArray.at(ix);

			//������ʾ��ǰѡ��
			if(optPos._bHilight){
				kiOptionItem *pItem = m_pOptionSet->GetOption(ix);

				pItem->GetPromptText(str);
				szName = str.c_str();

				memDC.SetTextColor(m_selOptionColor);
				memDC.TextOut(optPos._rect.left,optPos._rect.top,szName);
				//����һ���ߡ�
				CPen pen(PS_SOLID,1,m_selOptionColor);
				CPen *pOldPen = memDC.SelectObject(&pen);

				memDC.MoveTo(optPos._rect.left,optPos._rect.bottom);
				memDC.LineTo(optPos._rect.right,optPos._rect.bottom);
				memDC.SelectObject(pOldPen);

				break;
			}	
		}
		memDC.SetTextColor(textcol);
	}

	//���Ʊ߿�
	CBrush bsh(RGB(0,64,128));//GetSysColor(COLOR_3DSHADOW));
	memDC.FrameRect(&cliRect,&bsh);

	dc.BitBlt(0,0,cliRect.Width(),cliRect.Height(),&memDC,0,0,SRCCOPY);

	memDC.SelectObject(pOldFont);
	memDC.SelectObject(pOldBmp);
	memBmp.DeleteObject();
	memDC.DeleteDC();
}

// ͨ��������ȱ任����Ҫ�ر���
//
void CXInputEdit::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);


}

void CXInputEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_DELETE){
		DeleteChar(FALSE);
	}else if(nChar == VK_BACK){
		DeleteChar(TRUE);
	}else if(nChar == VK_SPACE || nChar == VK_RETURN){
		//���ûص�����,�������ʵ��ļ����.
		// �������Ϊ��.
		//���ûص�����
		if(_pInputEditCallBack){
			//�ж��ǲ���ѡ���ַ�
			if(m_pOptionSet && m_szInputText.GetLength() == 1){
				int iSel = m_pOptionSet->FindOption(m_szInputText.GetAt(0));
				if(iSel >= 0){
					kiOptionItem *pOptionItem = m_pOptionSet->GetOption(iSel);
					pOptionItem->OnEditOptionSelected(iSel,pOptionItem);
					//_pInputEditCallBack->OnEditOptionSelected(iSel,pOptionItem);
				}else{
					_pInputEditCallBack->OnEditInputFinished(m_szInputText);
				}
			}else{
				_pInputEditCallBack->OnEditInputFinished(m_szInputText);
			}
		}
	}else if(nChar == VK_LEFT){
		//���ƹ��
		if(m_nCaretIdx > 0){
			m_nCaretIdx --;
			UpdateCaret();
		}
	}else if(nChar == VK_RIGHT){
		//���ƹ��
		if(m_nCaretIdx < (int)m_chrPosArray.size()){
			m_nCaretIdx ++;
			UpdateCaret();
		}
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

// ɾ����ǰ���ǰ���һ���ַ���
//
int  CXInputEdit::DeleteChar(BOOL bBack)
{
	if(m_szInputText.IsEmpty())
		return 0;

	int nsize = (int)m_chrPosArray.size();
	int nLen = m_szInputText.GetLength();
	ASSERT(nsize == nLen);

	//û��ʲô��ɾ��
	if(bBack && m_nCaretIdx == 0 ||
	   !bBack && m_nCaretIdx == nsize)
	   return 0;

	if(!bBack && m_nCaretIdx == nsize - 1 ||
	   bBack && m_nCaretIdx == nsize)
	{//ɾ�����һ��
		m_szInputText.Delete(nLen - 1,1);
		nLen = m_szFullText.GetLength();
		m_szFullText.Delete(nLen - 1);

		m_chrPosArray.pop_back();
		if(bBack)
			m_nCaretIdx --;
	}else{
		//ɾ���м�ġ�
		CClientDC dc(this);
		CSize sz,sz2;
		CFont *pOldFont = dc.SelectObject(&m_textFont);
		int nIdx = 0,ndf = 0,dx = 0;
		int ix,nL,nR,xShift = 0;
		CString szL,szR,szLL;

		//���������ַ���
		if(bBack){
			nIdx = m_nCaretIdx - 1;//��ɾ���ַ��±�
		}else{
			nIdx = m_nCaretIdx;
		}
		nL = nIdx + 1;//��֤����ɾ������ַ��������һ��
		nR = nsize - nL;
		szL = m_szInputText.Left(nL);
		szR = m_szInputText.Right(nR);
		
		//
		sz = dc.GetTextExtent(szL);
		if(nIdx > 0){
			szLL = szL.Left(nIdx);
			sz2 = dc.GetTextExtent(szLL);
			xShift = sz.cx - sz2.cx;// > 0
		}else{//��ɾ����ʼһ���ַ�
			szLL = "";
			xShift = sz.cx;
		}
		//���������Ӧ�
		for(ix = nIdx + 1;ix < nsize;ix ++){
			m_chrPosArray[ix]._nStart -= xShift;
			m_chrPosArray[ix]._nEnd -= xShift;
		}
		//ɾ��
		m_chrPosArray.erase(m_chrPosArray.begin() + nIdx);
		
		//�ع��ַ���
		m_szInputText = szLL + szR;
		m_szFullText = m_szFullPromptText + m_szInputText;
				
		if(bBack)
			m_nCaretIdx --;

		dc.SelectObject(pOldFont);
	}

	UpdateCaret();
	Invalidate(FALSE);

	return 0;
}

void CXInputEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CXInputEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	TRACE("\n onchar %d.",nChar);
	char c = (char)nChar;
	if((c >= '0' && c <= '9') || 
		(c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z'))
	{
		AcceptChar(c);
	}
	else if(c == '.' || c == ',' || c == '+' || c == '-')
	{
		AcceptChar(c);
	}

	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

// ����һ���ַ�,�����ǲ��룬Ҳ�������ں���׷�ӡ�
//
int  CXInputEdit::AcceptChar(char c)
{
	CClientDC dc(this);
	CSize sz;
	int ix,nsize = (int)m_chrPosArray.size();
	CFont *pOldFont = dc.SelectObject(&m_textFont);

	if(m_szInputText.IsEmpty()){
		ASSERT(m_nCaretIdx == 0);
	}

	ASSERT(m_szInputText.GetLength() == nsize);//�����������ʼ����ͬ
	if(m_nCaretIdx == nsize){//ĩβ׷���ַ�
	
		m_szInputText += (char)c;
		m_szFullText += (char)c;
		//
		sz = dc.GetTextExtent(m_szInputText);
		//
		XCharPos cpos;
		if(nsize == 0){//��һ������
			cpos._nStart = m_promptRect.right;
		}else{
			cpos._nStart = m_chrPosArray[nsize - 1]._nEnd;
		}
		cpos._nEnd = m_promptRect.right + sz.cx;
		m_chrPosArray.push_back(cpos);

		//���¹��λ��
		m_nCaretIdx ++;
		//
	}else{//�����ַ�

		CString szL,szR;
		int nl = m_nCaretIdx;
		int nr = nsize - nl; 
		int xShift = 0;//ƫ�Ƶľ���

		//��ȡ������ҵ��ַ�����
		if(nl > 0){
			szL = m_szInputText.Left(nl);
			szR = m_szInputText.Right(nr);
		}else{
			szL.Empty();
			szR = m_szInputText;
		}
		//
		szL += c;
		sz = dc.GetTextExtent(szL);

		XCharPos cpos;
		cpos._nStart = m_chrPosArray[nl]._nStart;//ԭ���ַ�����ʼλ��
		cpos._nEnd = m_promptRect.right + sz.cx;//
		xShift = cpos._nEnd - cpos._nStart;
		//���������
		for(ix = m_nCaretIdx;ix < nsize;ix ++){
			m_chrPosArray[ix]._nStart += xShift;
			m_chrPosArray[ix]._nEnd += xShift;
		}
		//�����µ�
		m_chrPosArray.insert(m_chrPosArray.begin() + m_nCaretIdx,cpos);

		//�ع��ı�
		m_szInputText = szL + szR;
		m_szFullText = m_szFullPromptText + m_szInputText;
		
		// �����¡�
		m_nCaretIdx ++;
	}

	dc.SelectObject(pOldFont);

	UpdateCaret();

	Invalidate(FALSE);
	return 0;
}

void  CXInputEdit::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	CreateSolidCaret(1,m_nCaretHeight);
	UpdateCaret();

	m_bHasFocus = TRUE;
}

void CXInputEdit::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	HideCaret();
	DestroyCaret();

	m_bHasFocus = FALSE;
}

void CXInputEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CXInputEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(!m_bHasFocus){
		CRect rc;
		GetClientRect(&rc);
		if(rc.PtInRect(point)){
			CalcCaretPos(point);
			SetFocus();
		}
	}else{
		//�������λ�á�
		CalcCaretPos(point);
		UpdateCaret();
	}

	//�����Ƿ�����ѡ����.
	int isel = HitOptionTest(point);
	if(isel >= 0){
		//���ûص�����,�������Item�Ļص�����
		if(_pInputEditCallBack){
			kiOptionItem *pItem = m_pOptionSet->GetOption(isel);
			pItem->OnEditOptionSelected(isel,pItem);
			//_pInputEditCallBack->OnEditOptionSelected(isel,pItem);//Ӧ����Ϊclicked
		}
	}

	CWnd::OnLButtonUp(nFlags, point);
}

// ���ݵ�����Ӧ��caret��λ��.
//
void		CXInputEdit::CalcCaretPos(CPoint point)
{
	if(point.x < m_nMargin || m_szFullText.IsEmpty()){//��ʼλ��
		m_nCaretIdx = 0;
		return;
	}

	if(point.x < m_promptRect.right || m_chrPosArray.empty()){
		m_nCaretIdx = 0;
		return;
	}

	//�������н��в���
	int ix,isize = (int)m_chrPosArray.size();
	if(point.x >= m_chrPosArray[isize - 1]._nEnd)
		m_nCaretIdx = isize;//���
	else if(point.x <= m_chrPosArray[0]._nStart)
		m_nCaretIdx = 0;
	else{
		int nm = 0;
		for(ix = 0;ix < isize;ix ++){
			XCharPos& cpos = m_chrPosArray.at(ix);
			nm = (cpos._nStart + cpos._nEnd) / 2;
			if(point.x >= cpos._nStart && point.x < nm){
				m_nCaretIdx = ix;
				break;
			}else if(point.x >= nm && point.x < cpos._nEnd){
				m_nCaretIdx = ix + 1;
				break;
			}
		}
	}
}

void	CXInputEdit::UpdateCaret()
{
	POINT pos;

	pos.y = m_nMargin - 1;
	if(m_szFullText.IsEmpty())
		pos.x = m_nMargin;
	else if(m_chrPosArray.empty()){
		ASSERT(m_szInputText.IsEmpty());
		pos.x = m_promptRect.right;
	}else{
		int nSize = (int)m_chrPosArray.size();
		ASSERT(m_nCaretIdx >= 0 && m_nCaretIdx <= nSize);
		if(m_nCaretIdx == nSize)
			pos.x = m_chrPosArray[nSize - 1]._nEnd;
		else
			pos.x = m_chrPosArray[m_nCaretIdx]._nStart;
	}

	if(m_bHasFocus)
		HideCaret();
	SetCaretPos(pos);
	ShowCaret();
}

//�趨��ǣ�����onchar�������еļ�ֵ
//
UINT CXInputEdit::OnGetDlgCode()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	UINT nFlag = CWnd::OnGetDlgCode();
	nFlag |= DLGC_WANTALLKEYS;
	return nFlag;
}

void CXInputEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if((nFlags & MK_LBUTTON) || (nFlags & MK_RBUTTON)){
		//do nothing
	}else{
		HitOptionTest(point);
		Invalidate(FALSE);
	}

	CWnd::OnMouseMove(nFlags, point);
}

// �Ƿ��ӹ�������ѡ��
//
int	CXInputEdit::HitOptionTest(CPoint point)
{
	int ix,isel = -1,isize = (int)m_optPosArray.size();

	for(ix = 0;ix < isize;ix ++){
		XOptionPos& optPos = m_optPosArray.at(ix);
		CRect rect(optPos._rect);
		if(rect.PtInRect(point)){
			isel = ix;
			optPos._bHilight = true;
		}else{
			optPos._bHilight = false;
		}
	}

	return isel;
}