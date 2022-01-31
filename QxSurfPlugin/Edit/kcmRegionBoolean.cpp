#include "StdAfx.h"
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepCheck_Wire.hxx>
#include <BRepAlgo_FaceRestrictor.hxx>
#include <TopTools_ListOfShape.hxx>
#include "kiInputEntityTool.h"
#include "kiInputVector.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "kvCoordSystem.h"
#include "kcEntityFactory.h"
#include "kcmRegionBoolean.h"

kcmRegionBoolean::kcmRegionBoolean(void)
{
	m_pInputEntity1 = NULL;
	m_pInputEntity2 = NULL;
}

kcmRegionBoolean::~kcmRegionBoolean(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmRegionBoolean::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmRegionBoolean::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntity1)
	{
		if(m_pInputEntity1->IsInputDone())
		{
			if(CheckInputShape(m_pInputEntity1))
				NavToNextTool();
		}
	}
	else if(pTool == m_pInputEntity2)
	{
		if(m_pInputEntity2->IsInputDone())
		{
			if(!CheckInputShape(m_pInputEntity2))
				return KSTA_CONTINUE;

			if(DoBoolean())
			{
				m_pInputEntity1->ClearSel();
				m_pInputEntity2->ClearSel();

				Done();
				Redraw();

				return KSTA_DONE;
			}
		}
	}
	return KSTA_CONTINUE;
}

// ������Ҫ�����빤��
BOOL	kcmRegionBoolean::CreateInputTools()
{
	m_pInputEntity1 = new kiInputEntityTool(this,"ʰȡ��һ���ջ���ƽ������");
	m_pInputEntity1->SetOption(KCT_ENT_WIRE | KCT_ENT_FACE,1);

	m_pInputEntity2 = new kiInputEntityTool(this,"ʰȡ�ڶ����ջ���ƽ������");
	m_pInputEntity2->SetOption(KCT_ENT_WIRE | KCT_ENT_FACE,1);

	return TRUE;
}

BOOL	kcmRegionBoolean::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntity1);
	KC_SAFE_DELETE(m_pInputEntity2);

	return TRUE;
}

BOOL	kcmRegionBoolean::InitInputToolQueue()
{
	AddInputTool(m_pInputEntity1);
	AddInputTool(m_pInputEntity2);

	return TRUE;
}

BOOL	kcmRegionBoolean::CheckInputShape(kiInputEntityTool *pTool)
{
	if(pTool->GetSelCount() != 1)
		return FALSE;
	kiSelEntity se = pTool->GetFirstSelect();
	TopoDS_Shape aShape = se._pEntity->GetShape();

	if(aShape.ShapeType() == TopAbs_WIRE)
	{
		//�Ƿ�պ�
		BRepCheck_Wire chkWire(TopoDS::Wire(aShape));
		if(BRepCheck_NoError != chkWire.Closed())
			return FALSE;

		//�Ƿ���ƽ����
		BRepBuilderAPI_FindPlane fp(aShape);
		if(!fp.Found())
			return FALSE;
	}
	else
	{
		TopoDS_Face aFace = TopoDS::Face(aShape);
		Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
		if(aSurf.IsNull())
			return FALSE;

		if(aSurf->IsKind(STANDARD_TYPE(Geom_Plane)))
			return TRUE;

		if(aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
		{
			Handle(Geom_RectangularTrimmedSurface) aTrmSurf =
				Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurf);
			Handle(Geom_Surface) aBasSurf = aTrmSurf->BasisSurface();
			if(!aBasSurf.IsNull() &&
				aBasSurf->IsKind(STANDARD_TYPE(Geom_Plane)))
				return TRUE;
		}
		if(aSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
		{
			Handle(Geom_OffsetSurface) aOffSurf = 
				Handle(Geom_OffsetSurface)::DownCast(aSurf);
			Handle(Geom_Surface) aBasSurf = aOffSurf->BasisSurface();
			if(!aBasSurf.IsNull() &&
				aBasSurf->IsKind(STANDARD_TYPE(Geom_Plane)))
				return TRUE;
		}

		return FALSE;
	}

	return TRUE;
}

BOOL	kcmRegionBoolean::DoBoolean()
{
	kcEntity *pEntity1 = NULL,*pEntity2 = NULL;
	TopoDS_Shape aShape1,aShape2;
	TopTools_ListOfShape aLS;
	if(!GetShapeAndEntity(m_pInputEntity1,pEntity1,aShape1) ||
		!GetShapeAndEntity(m_pInputEntity2,pEntity2,aShape2))
		return FALSE;

	TopoDS_Shape aS;
	aS = DoBoolean(aShape1,aShape2);
	if(aS.IsNull())
		return FALSE;

	kcEntityFactory entfac;
	kcEntity *pEnt = NULL;
	std::vector<kcEntity *> aEnt;
	if(aS.ShapeType() == TopAbs_SHELL)
	{
		pEnt = entfac.Create(aS);
		if(pEnt)
			aEnt.push_back(pEnt);
	}
	if(aS.ShapeType() == TopAbs_SOLID)
	{
		pEnt = entfac.Create(aS);
		if(pEnt)
			aEnt.push_back(pEnt);
	}
	else if(aS.ShapeType() == TopAbs_COMPOUND)
	{
		TopoDS_Compound aComp = TopoDS::Compound(aS);
		TopoDS_Iterator ite(aComp);
		for(;ite.More();ite.Next())
		{
			pEnt = entfac.Create(ite.Value());
			if(pEnt)
				aEnt.push_back(pEnt);
		}
	}

	kcModel *pModel = GetModel();
	if(!aEnt.empty())
	{
		pModel->BeginUndo(GetName());
		pModel->DelEntity(pEntity1);
		pModel->DelEntity(pEntity2);
		int ix,ic = (int)aEnt.size();
		for(ix = 0;ix < ic;ix ++)
			pModel->AddEntity(aEnt[ix]);
		pModel->EndUndo();
	}

	return TRUE;
}

BOOL	kcmRegionBoolean::GetSolidEntity(kiInputEntityTool *pTool,kcSolidEntity *&pEntity)
{
	if(!pTool->IsInputDone())
		return FALSE;

	kiSelSet *pSelSet = pTool->GetSelSet();
	if(pSelSet->GetSelCount() != 1)
		return FALSE;

	pSelSet->InitSelected();
	kiSelEntity se = pSelSet->CurSelected();
	if(!IS_SOLID_ENTITY(se._pEntity))
		return FALSE;

	pEntity = (kcSolidEntity *)se._pEntity;
	return TRUE;
}

BOOL	kcmRegionBoolean::GetShapeAndEntity(kiInputEntityTool *pTool,kcEntity *&pEntity,TopoDS_Shape& aShape)
{
	if(!pTool->IsInputDone())
		return FALSE;

	kiSelEntity se = pTool->GetFirstSelect();
	TopoDS_Shape aS = se._pEntity->GetShape();
	TopoDS_Face aFace;
	pEntity = se._pEntity;

	if(aS.ShapeType() == TopAbs_WIRE)
	{
		TopoDS_Wire aWire = TopoDS::Wire(aS);
		try{
			BRepBuilderAPI_FindPlane fp(aS);
			if(!fp.Found())
				return FALSE;

			BRepBuilderAPI_MakeFace mf(fp.Plane(),Precision::Confusion());
			if(!mf.IsDone())
				return FALSE;
			
			aFace = mf.Face();

			BRepAlgo_FaceRestrictor fr;
			fr.Init(aFace,Standard_True,Standard_True);
			fr.Add(aWire);
			fr.Perform();
			if(fr.IsDone())
				aFace = fr.Current();

		}catch(Standard_Failure){
			return FALSE;
		}
	}
	else if(aS.ShapeType() == TopAbs_FACE)
	{
		aFace = TopoDS::Face(aS);
	}
	else
		return FALSE;

	if(!aFace.IsNull())
	{
		BRep_Builder BB;
		TopoDS_Shell aShell;
		BB.MakeShell(aShell);
		BB.Add(aShell,aFace);

		aShape = aShell;

		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// ��
kcmRegionBoolFuse::kcmRegionBoolFuse()
{
	m_strName = "���򲢼�";
	m_strAlias = "RegionFuse";
	m_strDesc = "���򲢼�";
}

TopoDS_Shape	kcmRegionBoolFuse::DoBoolean(const TopoDS_Shape& aS1,const TopoDS_Shape& aS2)
{
	TopoDS_Shape aS;
	try{
		BRepAlgoAPI_Fuse fuse(aS1,aS2);
		if(!fuse.IsDone())
			return aS;
		aS = fuse.Shape();
	}catch(Standard_Failure){
	}

	return aS;
}


//////////////////////////////////////////////////////////////////////////
// ��
kcmRegionBoolCommon::kcmRegionBoolCommon()
{
	m_strName = "���򽻼�";
	m_strAlias = "RegionComm";
	m_strDesc = "���򽻼�";
}

TopoDS_Shape	kcmRegionBoolCommon::DoBoolean(const TopoDS_Shape& aS1,const TopoDS_Shape& aS2)
{
	TopoDS_Shape aS;
	try{
		BRepAlgoAPI_Common comm(aS1,aS2);
		if(!comm.IsDone())
			return aS;
		aS = comm.Shape();
	}catch(Standard_Failure){
	}

	return aS;
}

//////////////////////////////////////////////////////////////////////////
// ��
kcmRegionBoolCut::kcmRegionBoolCut()
{
	m_strName = "����";
	m_strAlias = "RegionCut";
	m_strDesc = "����";
}

TopoDS_Shape	kcmRegionBoolCut::DoBoolean(const TopoDS_Shape& aS1,const TopoDS_Shape& aS2)
{
	TopoDS_Shape aS;
	try{
		BRepAlgoAPI_Cut fcut(aS1,aS2);
		if(!fcut.IsDone())
			return aS;
		aS = fcut.Shape();
	}catch(Standard_Failure){
	}

	return aS;
}