#include "StdAfx.h"
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_Wire.hxx>
#include <BRepCheck_Shell.hxx>
#include "kcEntity.h"
#include "kiInputEntityTool.h"
#include "kcmShapeCheck.h"

static const char *pszBChkStatus[] = {
	"BRepCheck_NoError",
    "BRepCheck_InvalidPointOnCurve",
    "BRepCheck_InvalidPointOnCurveOnSurface",
    "BRepCheck_InvalidPointOnSurface",
    "BRepCheck_No3DCurve",
    "BRepCheck_Multiple3DCurve",
    "BRepCheck_Invalid3DCurve",
    "BRepCheck_NoCurveOnSurface",
    "BRepCheck_InvalidCurveOnSurface",
    "BRepCheck_InvalidCurveOnClosedSurface",
    "BRepCheck_InvalidSameRangeFlag",
    "BRepCheck_InvalidSameParameterFlag",
    "BRepCheck_InvalidDegeneratedFlag",
    "BRepCheck_FreeEdge",
    "BRepCheck_InvalidMultiConnexity",
    "BRepCheck_InvalidRange",
    "BRepCheck_EmptyWire",
    "BRepCheck_RedundantEdge",
    "BRepCheck_SelfIntersectingWire",
    "BRepCheck_NoSurface",
    "BRepCheck_InvalidWire",
    "BRepCheck_RedundantWire",
    "BRepCheck_IntersectingWires",
    "BRepCheck_InvalidImbricationOfWires",
    "BRepCheck_EmptyShell",
    "BRepCheck_RedundantFace",
    "BRepCheck_UnorientableShape",
    "BRepCheck_NotClosed",
    "BRepCheck_NotConnected",
    "BRepCheck_SubshapeNotInShape",
    "BRepCheck_BadOrientation",
    "BRepCheck_BadOrientationOfSubshape",
    "BRepCheck_InvalidPolygonOnTriangulation",
    "BRepCheck_InvalidToleranceValue",
    "BRepCheck_CheckFail",
	""
};

kcmShapeCheck::kcmShapeCheck(void)
{
	m_pInputEntityTool = NULL;
}

kcmShapeCheck::~kcmShapeCheck(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmShapeCheck::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmShapeCheck::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntityTool)
	{
		if(m_pInputEntityTool->IsInputDone())
		{
			DoCheck();
			Done();
			return KSTA_DONE;
		}
		else
		{
			EndCommand(KSTA_CANCEL);
			return KSTA_CANCEL;
		}
	}
	return KSTA_CONTINUE;
}

int		kcmShapeCheck::OnExecute()
{
	if(m_pInputEntityTool){
		m_pInputEntityTool->AddInitSelected(true);
		//
		if(m_pInputEntityTool->IsInputDone()){
			DoCheck();
			Done();
			return KSTA_DONE;
		}
	}
	
	return KSTA_CONTINUE;
}

int		kcmShapeCheck::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmShapeCheck::CreateInputTools()
{
	ASSERT(FALSE);
	return FALSE;
}

BOOL	kcmShapeCheck::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntityTool);
	return TRUE;
}
BOOL	kcmShapeCheck::InitInputToolQueue()
{
	AddInputTool(m_pInputEntityTool);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
// ���������Ч��
kcmShapeCheckValid::kcmShapeCheckValid()
{
	m_strName = "��Ч��";
	m_strAlias = "ChkValid";
	m_strDesc = "��������Ч��";
}

kcmShapeCheckValid::~kcmShapeCheckValid()
{

}

// ������Ҫ�����빤��
BOOL	kcmShapeCheckValid::CreateInputTools()
{
	m_pInputEntityTool = new kiInputEntityTool(this,"ѡ�����");
	m_pInputEntityTool->SetOption(KCT_ENT_ALL,true);
	m_pInputEntityTool->SetNaturalMode(true);
	
	return TRUE;
}

BOOL	kcmShapeCheckValid::DoCheck()
{
	if(m_pInputEntityTool->GetSelCount() <= 0)
		return FALSE;
	kiSelEntity se = m_pInputEntityTool->GetFirstSelect();
	TopoDS_Shape aShape = se.EntityShape();

	try
	{
		BRepCheck_Analyzer analyzer(aShape);
		if(analyzer.IsValid())
			AfxMessageBox("Valid Shape");
		else{
			CString szErr = "Invalid Shape :",ss;
			Handle(BRepCheck_Result) bcRes = analyzer.Result(aShape);
			const BRepCheck_ListOfStatus& staList = bcRes->Status();
			BRepCheck_ListIteratorOfListOfStatus iteStatus;
			for(iteStatus.Initialize(staList);iteStatus.More();iteStatus.Next()){
				BRepCheck_Status sta = iteStatus.Value();
				szErr += pszBChkStatus[(int)sta];
				szErr += ", ";
			}

			AfxMessageBox(szErr);
		}
	}catch (Standard_Failure){
		
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
// �������������Ч��
kcmShapeCheckTopoValid::kcmShapeCheckTopoValid()
{
	m_strName = "������Ч��";
	m_strAlias = "ChkTopoValid";
	m_strDesc = "������������Ч��";
}

kcmShapeCheckTopoValid::~kcmShapeCheckTopoValid()
{

}

// ������Ҫ�����빤��
BOOL	kcmShapeCheckTopoValid::CreateInputTools()
{
	m_pInputEntityTool = new kiInputEntityTool(this,"ѡ�����");
	m_pInputEntityTool->SetOption(KCT_ENT_ALL,true);
	m_pInputEntityTool->SetNaturalMode(true);

	return TRUE;
}

BOOL	kcmShapeCheckTopoValid::DoCheck()
{
	if(m_pInputEntityTool->GetSelCount() <= 0)
		return FALSE;
	kiSelEntity se = m_pInputEntityTool->GetFirstSelect();
	TopoDS_Shape aShape = se.EntityShape();

	try
	{
		BRepCheck_Analyzer analyzer(aShape,Standard_False);
		if(analyzer.IsValid())
			AfxMessageBox("Valid Shape");
		else
			AfxMessageBox("Invalid Shape");
	}catch (Standard_Failure){

	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// ��黷��ǵıպ���
kcmShapeCheckClose::kcmShapeCheckClose()
{
	m_strName = "�պ���";
	m_strAlias = "ChkClose";
	m_strDesc = "��黷��ǵıպ���";
}

kcmShapeCheckClose::~kcmShapeCheckClose()
{

}

// ������Ҫ�����빤��
BOOL	kcmShapeCheckClose::CreateInputTools()
{
	m_pInputEntityTool = new kiInputEntityTool(this,"ѡ�����");
	m_pInputEntityTool->SetOption(KCT_ENT_WIRE | KCT_ENT_SHELL,true);
	m_pInputEntityTool->SetNaturalMode(true);

	return TRUE;
}

BOOL	kcmShapeCheckClose::DoCheck()
{
	if(m_pInputEntityTool->GetSelCount() <= 0)
		return FALSE;
	kiSelEntity se = m_pInputEntityTool->GetFirstSelect();
	TopoDS_Shape aShape = se.EntityShape();

	try
	{
		BOOL bClosed = FALSE;
		if(aShape.ShapeType() == TopAbs_WIRE)
		{
			BRepCheck_Wire chkWire(TopoDS::Wire(aShape));
			if(chkWire.Closed() == BRepCheck_NoError)
				bClosed = TRUE;
		}
		else
		{
			BRepCheck_Shell chkShell(TopoDS::Shell(aShape));
			if(chkShell.Closed() == BRepCheck_NoError)
				bClosed = TRUE;
		}
		if(bClosed)
			AfxMessageBox("Closed Shape");
		else
			AfxMessageBox("Open Shape");
	}catch (Standard_Failure){

	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// ��黷���Խ�
kcmShapeCheckSelfIntersect::kcmShapeCheckSelfIntersect()
{
	m_strName = "�����Խ�";
	m_strAlias = "ChkSelfInter";
	m_strDesc = "��黷���Խ�";
}

kcmShapeCheckSelfIntersect::~kcmShapeCheckSelfIntersect()
{

}

// ������Ҫ�����빤��
BOOL	kcmShapeCheckSelfIntersect::CreateInputTools()
{
	m_pInputEntityTool = new kiInputEntityTool(this,"ѡ�����");
	m_pInputEntityTool->SetOption(KCT_ENT_WIRE,1);
	m_pInputEntityTool->SetNaturalMode(true);

	return TRUE;
}

BOOL	kcmShapeCheckSelfIntersect::DoCheck()
{
	if(m_pInputEntityTool->GetSelCount() <= 0)
		return FALSE;
	kiSelEntity se = m_pInputEntityTool->GetFirstSelect();
	TopoDS_Shape aShape = se.EntityShape();

	try
	{

	}catch (Standard_Failure){

	}

	return TRUE;
}