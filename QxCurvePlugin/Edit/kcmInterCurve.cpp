#include "StdAfx.h"
#include <BRepAlgoAPI_Section.hxx>
#include <GeomInt_IntSS.hxx>
#include "kiInputEntityTool.h"
#include "kiInputVector.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "kvCoordSystem.h"
#include "kcEntityFactory.h"
#include "kcmInterCurve.h"

kcmInterCurve::kcmInterCurve(void)
{
	m_strName = "����";
	m_strAlias = "InterCrv";
	m_strDesc = "�����ʵ�彻��";

	m_pInputEntity1 = NULL;
	m_pInputEntity2 = NULL;
}

kcmInterCurve::~kcmInterCurve(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmInterCurve::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmInterCurve::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntity1)
	{
		if(m_pInputEntity1->IsInputDone())
		{
			NavToNextTool();
		}
	}
	else if(pTool == m_pInputEntity2)
	{
		if(m_pInputEntity2->IsInputDone())
		{
			if(DoIntersect())
			{
				m_pInputEntity1->ClearSel();
				m_pInputEntity2->ClearSel();

				Done();
				Redraw();

				return KSTA_DONE;
			}
			else
			{
				m_pInputEntity1->ClearSel();
				m_pInputEntity2->ClearSel();

				EndCommand(KSTA_CANCEL);

				return KSTA_CANCEL;
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

BOOL	kcmInterCurve::DoIntersect()
{
	BRep_Builder aBB;
	kiSelEntity se1 = m_pInputEntity1->GetFirstSelect();
	kiSelEntity se2 = m_pInputEntity2->GetFirstSelect();
	TopoDS_Shape aS1 = se1.SelectShape();
	TopoDS_Shape aS2 = se2.SelectShape();

	if(aS1.IsNull() || aS2.IsNull()){
		AfxMessageBox("has null shape.");
		return FALSE;
	}

	bool bOk = false;
	TopoDS_Shape aNS;
	{
		try{
			BRepAlgoAPI_Section sect(aS1,aS2,Standard_False);
			sect.Approximation(true);
			sect.Build();
			if(sect.IsDone())
			{
				aNS = sect.Shape();
				bOk = true;
			}
		}catch(Standard_Failure){
			TRACE("\n BRepAlgoAPI_Section failed.\n");
		}
		//
		if(!bOk){
			if(aS1.ShapeType() == TopAbs_FACE && aS2.ShapeType() == TopAbs_FACE){
				TopoDS_Face aF1 = TopoDS::Face(aS1);
				TopoDS_Face aF2 = TopoDS::Face(aS2);
				//��ȡ��Ӧ����
				Handle(Geom_Surface) aSurf1 = BRep_Tool::Surface(aF1);
				Handle(Geom_Surface) aSurf2 = BRep_Tool::Surface(aF2);
				if(!aSurf1.IsNull() && !aSurf2.IsNull()){
					GeomInt_IntSS intSS;
					try{
						intSS.Perform(aSurf1,aSurf2,1.e-8);
						if(intSS.IsDone()){
							TopoDS_Compound aComp;
							aBB.MakeCompound(aComp);

							Handle(Geom_Curve) aIntCrv;
							int ix,nbCrv = intSS.NbLines();
							for(ix = 1;ix <= nbCrv;ix ++){
								aIntCrv = intSS.Line(ix);
								//����Edge
								BRepBuilderAPI_MakeEdge ME(aIntCrv);
								if(ME.IsDone()){
									aBB.Add(aComp,ME.Edge());
									bOk = true;
								}
							}
							if(bOk){
								aNS = aComp;
							}
						}
					}catch(Standard_Failure){
					}
				}
			}
		}
	}


	if(aNS.IsNull()){
		AfxMessageBox("��ʧ��.");
		return FALSE;
	}

	std::vector<kcEntity *> aEnt;
	kcEntity *pEnt = NULL;
	kcEntityFactory entfac;
	TopExp_Explorer ex;
	for(ex.Init(aNS,TopAbs_EDGE);ex.More();ex.Next())
	{
		TopoDS_Edge aE = TopoDS::Edge(ex.Current());
		pEnt = entfac.Create(aE);
		if(pEnt)
			aEnt.push_back(pEnt);
	}
	if(aEnt.empty())
		return FALSE;

	kcModel *pModel = GetModel();
	pModel->BeginUndo(GetName());
	{
		int ix,ic = (int)aEnt.size();
		for(ix = 0;ix < ic;ix ++)
			pModel->AddEntity(aEnt[ix]);
	}
	pModel->EndUndo();

	return TRUE;
}

int		kcmInterCurve::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmInterCurve::OnEnd(int nCode)
{
	m_pInputEntity1->ClearSel();
	m_pInputEntity2->ClearSel();

	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmInterCurve::CreateInputTools()
{
	m_pInputEntity1 = new kiInputEntityTool(this,"ѡ���һ������(����/��/ʵ��)");
	m_pInputEntity1->SetOption(KCT_ENT_FACE | KCT_ENT_SHELL | KCT_ENT_SOLID,1);
	m_pInputEntity1->SetNaturalMode(true);

	m_pInputEntity2 = new kiInputEntityTool(this,"ѡ��ڶ�������(����/��/ʵ��)");
	m_pInputEntity2->SetOption(KCT_ENT_FACE | KCT_ENT_SHELL | KCT_ENT_SOLID,1);
	m_pInputEntity2->SetNaturalMode(true);

	return TRUE;
}

BOOL	kcmInterCurve::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntity1);
	KC_SAFE_DELETE(m_pInputEntity2);

	return TRUE;
}

BOOL	kcmInterCurve::InitInputToolQueue()
{
	AddInputTool(m_pInputEntity1);
	AddInputTool(m_pInputEntity2);

	return TRUE;
}

