#include "StdAfx.h"
#include <AIS_TextLabel.hxx>
#include <AIS_RubberBand.hxx>
#include "kcModel.h"
#include "kcBasePlane.h"
#include "kcEntity.h"
#include "kcPreviewObj.h"
#include "kcPreviewText.h"
#include "kiInputPoint.h"
#include "kcgTextLib.h"
#include "TextEditDialog.h"
#include "kcmWireText.h"

kcmWireText::kcmWireText(void)
{
	m_strName = "Wire Text";
	m_strAlias = "WText";
	m_strDesc = "Wire Text";

	m_pInputPoint = NULL;
}

kcmWireText::~kcmWireText(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmWireText::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmWireText::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputPoint)
	{
		TopoDS_Shape aS;
		int ix,nLen = m_aSeqOfShape.Length();
		for(ix = 1;ix <= nLen;ix ++)
		{
			aS = m_aSeqOfShape.Value(ix);
			TopoDS_Compound aC = TopoDS::Compound(aS);

			kcCompoundEntity *pComEnt = new kcCompoundEntity;
			pComEnt->SetCompound(aC);

			kcModel *pModel = GetModel();
			pModel->AddEntity(pComEnt);
		}
		
		Done();
		Redraw();
		return KSTA_DONE;

	}
	return kiCommand::OnInputFinished(pTool);
}

int	kcmWireText::OnExecute()
{
	//���
	Handle(AIS_InteractiveContext) aCtx = GetAISContext();
	
	Handle(AIS_RubberBand) aBand = new AIS_RubberBand;
	aBand->SetColor(Quantity_Color(1.0,0.0,1.0,Quantity_TOC_RGB));
	aBand->SetRectangle(10,10,200,100);
	aBand->SetFilling(Quantity_Color(1.0,1.0,0.0,Quantity_TOC_RGB),0.0);
	aCtx->Display(aBand,true);
	aBand = new AIS_RubberBand;
	aBand->SetColor(Quantity_Color(1.0,0.0,1.0,Quantity_TOC_RGB));
	aBand->SetRectangle(20,20,90,80);
	aBand->SetLineWidth(2.0);
	aBand->SetFilling(Quantity_Color(0.0,1.0,0.0,Quantity_TOC_RGB),0.0);
	aCtx->Display(aBand,true);

	Handle(AIS_TextLabel) aTextLabel = new AIS_TextLabel;
	aTextLabel->SetTransformPersistence (new Graphic3d_TransformPers (Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER));
	aTextLabel->SetColor(Quantity_Color(1.0,0.0,0.0,Quantity_TOC_RGB));
	aTextLabel->SetPosition(gp_Pnt(30,30,10));
	aTextLabel->SetText("Ok,Cancel");
	aTextLabel->SetTransparency(0.1);
	aTextLabel->SetZLayer(Graphic3d_ZLayerId_TopOSD);
	aCtx->Display(aTextLabel,true);

	CTextEditDialog dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(MakeWireText(dlg.m_szText,dlg.m_logFont,dlg.m_dTextHeight,dlg.m_nCharSpace,!dlg.m_bMakeFace))
		{
			AddInputTool(m_pInputPoint);

			return KSTA_CONTINUE;
		}
	}

	return KSTA_CANCEL;
}

//
BOOL	kcmWireText::MakeWireText(const char *lpszText,LOGFONT& lf,double dHeight,double dGap,BOOL bWire)
{
	BOOL bDone = FALSE;
	CView *pView = (CView *)GetActiveView();
	ASSERT(pView);
	CDC *pDC = pView->GetDC();
	HDC hDC = pDC->GetSafeHdc();

	m_aSeqOfShape.Clear();

	TopTools_ListOfShape aList;
	kcFontText fontText(lf);
	if(fontText.GetText(hDC,lpszText,dHeight,dGap,bWire,aList))
	{
		TopTools_ListIteratorOfListOfShape lls;
		for(lls.Initialize(aList);lls.More();lls.Next())
			m_aSeqOfShape.Append(lls.Value());

		bDone = TRUE;
	}

	pView->ReleaseDC(pDC);

	return bDone;
}

// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
BOOL	kcmWireText::CreateInputTools()
{
	m_pInputPoint = new kiInputPoint(this,"����Ŀ���:");
	m_pInputPoint->Init(&m_aPoint);

	return TRUE;
}

// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
BOOL	kcmWireText::InitInputToolQueue()
{
	return TRUE;
}

