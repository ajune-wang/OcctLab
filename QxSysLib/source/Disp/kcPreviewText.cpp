#include "StdAfx.h"
#include <Graphic3d_SequenceOfGroup.hxx>
#include <Graphic3d_StructureManager.hxx>
#include "kcDocContext.h"
#include "kcPreviewText.h"

#pragma warning(disable : 4996)

kcPreviewText::kcPreviewText(const Handle(AIS_InteractiveContext)& aCtx,kcDocContext *pDocCtx)
{
	pDocContext_ = pDocCtx;
	m_aStru = NULL;
		
	if(!aCtx.IsNull() && !aCtx->CurrentViewer().IsNull()){
		Handle(Graphic3d_StructureManager) aStruMgr = 
			aCtx->CurrentViewer()->StructureManager();
		if(!aStruMgr.IsNull()){
			m_aStru = new Graphic3d_Structure(aStruMgr);
			//KTRACE("structure id = %d\n",m_aStru->Identification());
		}
	}

	m_dHeight = 16;
}

kcPreviewText::~kcPreviewText(void)
{
	//KTRACE("kcPreviewText destroy.\n");
	Destroy();
}

// �������ԡ��滻�ɵġ�
//
bool	kcPreviewText::SetAspectText(const Handle(Graphic3d_AspectText3d)& aAspText)
{
	ASSERT(!aAspText.IsNull());
	m_aAspText = aAspText;
	return true;
}

// ��ȡ�������ԣ����û�У��Զ�����һ����
//
Handle(Graphic3d_AspectText3d)	kcPreviewText::GetAspectText() 
{
	if(m_aAspText.IsNull())
	{
		m_aAspText = new Graphic3d_AspectText3d;
		if(!m_aAspText.IsNull())
		{
			m_aAspText->SetFont("Arial");
			m_aAspText->SetColor(Quantity_Color(Quantity_NOC_RED));
			m_aAspText->SetStyle(Aspect_TOST_NORMAL);
		}
	}
	return m_aAspText;
}

//������������
void	kcPreviewText::Set(const char *pszFont,double height,int color)
{
	ASSERT(pszFont);
	Handle(Graphic3d_AspectText3d) aspText = GetAspectText();
	if(!aspText.IsNull())
	{
		aspText->SetFont(pszFont);
		aspText->SetColor(Quantity_Color(GetRValue(color)/256.0,GetGValue(color)/256.0,
			GetBValue(color)/256.0,Quantity_TOC_RGB));
	}
	m_dHeight = height;
}

void	kcPreviewText::SetHeight(double height)
{
	m_dHeight = height;
}

void	kcPreviewText::SetColor(double r,double g,double b)
{
	Handle(Graphic3d_AspectText3d) aspText = GetAspectText();
	if(!aspText.IsNull())
	{
		aspText->SetColor(Quantity_Color(r,g,b,Quantity_TOC_RGB));
	}
}

//��ʾ���塣
void	kcPreviewText::Text(const char *pszText,int x,int y,double angle)
{
	Handle(Graphic3d_Group) aGroup;
	int nbGrp = m_aStru->NumberOfGroups();
	if(nbGrp == 0)//new group
	{
		aGroup = m_aStru->NewGroup();//new Graphic3d_Group(m_aStru);
		if(aGroup.IsNull())
			return;
	}
	else
	{
		aGroup = m_aStru->Groups().First();
		//��վɵ�
		aGroup->Clear();
	}
	//set attr
	//��ֹΪ��
	Handle(Graphic3d_AspectText3d) aspText = GetAspectText();
	aspText->SetTextAngle(angle);
	aGroup->SetGroupPrimitivesAspect(aspText);

	//����λ��
	double dx,dy,dz;
	Handle(V3d_View) aView = pDocContext_->GetOcctView();
	if(!aView.IsNull()){
		aView->Convert(x,y,dx,dy,dz);

		//������ʾ����
		aGroup->Text(pszText,Graphic3d_Vertex(dx,dy,dz),m_dHeight);
		//aGroup->AddText()

		// ��ʾһ��
		if(!m_aStru->IsDisplayed())
			m_aStru->Display();

		//������ʾ
		pDocContext_->RefreshCurView();
	}
}

void	kcPreviewText::Clear()
{
	if(!m_aStru.IsNull())
	{
		m_aStru->Clear(Standard_True);

		if(m_aStru->IsDisplayed())
			m_aStru->Erase();	
	}
}

void	kcPreviewText::Destroy()
{
	Clear();

	if(!m_aStru.IsNull())
	{
		//m_aStru->Destroy();

		m_aStru.Nullify();
	}
}