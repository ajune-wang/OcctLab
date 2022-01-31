#include "StdAfx.h"
#include <BRepFilletAPI_MakeFillet.hxx>
#include "kiInputEntityTool.h"
#include "kiInputValueTool.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "kvCoordSystem.h"
#include "kcEntityFactory.h"
#include "kcmSolidFillet.h"

kcmSolidFillet::kcmSolidFillet(void)
{
	m_strName = "�Ȱ뾶��Բ��";
	m_strAlias = "FilletSR";
	m_strDesc = "�Ȱ뾶��Բ��";

	_pInputEntity = NULL;
	_pInputEdge = NULL;
	_pInputRadius = NULL;
	_dRadius = 5.0;
}

kcmSolidFillet::~kcmSolidFillet(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmSolidFillet::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmSolidFillet::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == _pInputEntity)
	{
		if(_pInputEntity->IsInputDone())
		{
			kiSelEntity se = _pInputEntity->GetFirstSelect();
			_pInputEdge->SetGlobalAISObject(se.AISObject());

			NavToNextTool();
		}
	}
	else if(pTool == _pInputEdge)
	{
		if(_pInputEdge->IsInputDone())
		{
			NavToNextTool();
		}
	}
	else if(pTool == _pInputRadius)
	{
		if(_pInputRadius->IsInputDone())
		{
			if(DoFillet())
			{
				Done();
				Redraw();

				return KSTA_DONE;
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

BOOL	kcmSolidFillet::DoFillet()
{
	kiSelEntity se = _pInputEntity->GetFirstSelect();
	int ix,icnt = _pInputEdge->GetSelCount();
	TopoDS_Shape aBS = se._pEntity->GetShape();

	TopoDS_Shape aNS;
	try{
		BRepFilletAPI_MakeFillet aFillet(aBS);
		for(ix = 0;ix < icnt;ix ++)
		{
			TopoDS_Shape aSS = _pInputEdge->SelectedShape(ix);
			if(aSS.ShapeType() == TopAbs_EDGE){
				TopoDS_Shape aTS = aSS.Located (TopLoc_Location());
				aFillet.Add(_dRadius,TopoDS::Edge(aTS));
			}else{
				TopExp_Explorer ex;
				for(ex.Init(aSS,TopAbs_EDGE);ex.More();ex.Next())
				{
					TopoDS_Edge aE = TopoDS::Edge(ex.Current());
					aFillet.Add(_dRadius,aE);
				}
			}
		}

		aFillet.Build();
		if(aFillet.IsDone()){
			aNS = aFillet.Shape();
		}
	}catch(Standard_Failure){
		return FALSE;
	}
	if(aNS.IsNull()){
		AfxMessageBox("����ʧ��.");
		return FALSE;
	}

	std::vector<kcEntity *> aEnt;
	kcEntityFactory efac;
	if(!efac.Create(aNS,aEnt,true))
		return FALSE;

	kcModel *pModel = GetModel();
	pModel->BeginUndo(GetName());
	{
		pModel->DelEntity(se._pEntity);
		pModel->AddEntity(aEnt);
	}
	pModel->EndUndo();

	return TRUE;
}

int		kcmSolidFillet::OnExecute()
{

	return KSTA_CONTINUE;
}

int		kcmSolidFillet::OnEnd(int nCode)
{
	_pInputEntity->ClearSel();

	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmSolidFillet::CreateInputTools()
{
	_pInputEntity = new kiInputEntityTool(this,"ѡ��һ��ʵ�����");
	_pInputEntity->SetOption(KCT_ENT_SOLID | KCT_ENT_SHELL,1);
	_pInputEntity->SetNaturalMode(true);

	_pInputEdge = new kiInputEntityTool(this,"ѡ��Ҫ���ǵı߻���");
	_pInputEdge->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE | KCT_ENT_FACE,-1);
	_pInputEdge->SetNaturalMode(false);
	_pInputEdge->SetTypeSwitch(true);

	_pInputRadius = new kiInputDoubleTool(this,"���뵹�ǰ뾶");
	_pInputRadius->Init(&_dRadius,0.001,1000.0);

	return TRUE;
}

BOOL	kcmSolidFillet::DestroyInputTools()
{
	KC_SAFE_DELETE(_pInputEntity);
	KC_SAFE_DELETE(_pInputEdge);
	KC_SAFE_DELETE(_pInputRadius);

	return TRUE;
}

BOOL	kcmSolidFillet::InitInputToolQueue()
{
	AddInputTool(_pInputEntity);
	AddInputTool(_pInputEdge);
	AddInputTool(_pInputRadius);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
kcmSolidFilletAllEdge::kcmSolidFilletAllEdge(void)
{
	m_strName = "ȫ����Բ��";
	m_strAlias = "FilletAE";
	m_strDesc = "ȫ���ߵ�Բ��";

	_pInputEntity = NULL;
	_pInputRadius = NULL;
	_dRadius = 5.0;
}

kcmSolidFilletAllEdge::~kcmSolidFilletAllEdge(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmSolidFilletAllEdge::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmSolidFilletAllEdge::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == _pInputEntity)
	{
		if(_pInputEntity->IsInputDone())
		{
			NavToNextTool();
		}
	}
	else if(pTool == _pInputRadius)
	{
		if(_pInputRadius->IsInputDone())
		{
			if(DoFillet())
			{
				Done();
				Redraw();

				return KSTA_DONE;
			}
			else
			{
				EndCommand(KSTA_CANCEL);
				return KSTA_CANCEL;
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

BOOL	kcmSolidFilletAllEdge::DoFillet()
{
	if(_pInputEntity->GetSelCount() <= 0)
		return FALSE;
	kiSelEntity se = _pInputEntity->GetFirstSelect();
	TopoDS_Shape aBS = se._pEntity->GetShape();

	TopoDS_Shape aNS;
	try{
		BRepFilletAPI_MakeFillet aFillet(aBS);
		TopExp_Explorer ex;
		for(ex.Init(aBS,TopAbs_EDGE);ex.More();ex.Next())
		{
			TopoDS_Edge aE = TopoDS::Edge(ex.Current());
			aFillet.Add(_dRadius,aE);
		}
		aFillet.Build();
		aNS = aFillet.Shape();
	}catch(Standard_Failure){
		return FALSE;
	}
	if(aNS.IsNull())
		return FALSE;

	std::vector<kcEntity *> aEnt;
	kcEntityFactory efac;
	if(!efac.Create(aNS,aEnt,true))
		return FALSE;

	kcModel *pModel = GetModel();
	pModel->BeginUndo(GetName());
	{
		pModel->DelEntity(se._pEntity);
		pModel->AddEntity(aEnt);
	}
	pModel->EndUndo();

	return TRUE;
}

int		kcmSolidFilletAllEdge::OnExecute()
{

	return KSTA_CONTINUE;
}

int		kcmSolidFilletAllEdge::OnEnd(int nCode)
{
	_pInputEntity->ClearSel();

	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmSolidFilletAllEdge::CreateInputTools()
{
	_pInputEntity = new kiInputEntityTool(this,"ѡ��һ��ʵ�����");
	_pInputEntity->SetOption(KCT_ENT_SOLID | KCT_ENT_SHELL,1);

	_pInputRadius = new kiInputDoubleTool(this,"���뵹�ǰ뾶");
	_pInputRadius->Init(&_dRadius,0.001,1000.0);

	return TRUE;
}

BOOL	kcmSolidFilletAllEdge::DestroyInputTools()
{
	KC_SAFE_DELETE(_pInputEntity);
	KC_SAFE_DELETE(_pInputRadius);

	return TRUE;
}

BOOL	kcmSolidFilletAllEdge::InitInputToolQueue()
{
	AddInputTool(_pInputEntity);
	AddInputTool(_pInputRadius);

	return TRUE;
}


