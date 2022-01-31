#include "StdAfx.h"
#include <vector>
#include "BRepAlgo_FaceRestrictor.hxx"
#include "kcEntity.h"
#include "kiSelSet.h"
#include "kcModel.h"
#include "QxBRepLib.h"
#include "kiInputEntityTool.h"
#include "kcmLoopToFace.h"

kcmLoopToFace::kcmLoopToFace(void)
{
	m_strName = "Loop To Face";
	m_strAlias = "LTF";
	m_strDesc = "build face from planar loop";

	m_pInputEntityTool = NULL;
}

kcmLoopToFace::~kcmLoopToFace(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmLoopToFace::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int		kcmLoopToFace::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == m_pInputEntityTool){
		if(m_pInputEntityTool->IsInputDone()){
			if(DoBuildFaceFromLoop()){
				m_pInputEntityTool->ClearSel();

				Done();
				Redraw();

				return KSTA_DONE;
			}else{
				m_pInputEntityTool->ClearSel();
				EndCommand(KSTA_ERROR);
				
				return KSTA_ERROR;
			}
		}
	}
	return kiCommand::OnInputFinished(pTool);
}

int		kcmLoopToFace::OnExecute()
{
	return KSTA_CONTINUE;
}

int		kcmLoopToFace::OnEnd(int nCode)
{
	return nCode;
}

// ������Ҫ�����빤��
BOOL	kcmLoopToFace::CreateInputTools()
{
	m_pInputEntityTool = new kiInputEntityTool(this,"ѡ��ƽ���ϱպ�Edge��Wire:");
	m_pInputEntityTool->SetOption(KCT_ENT_EDGE | KCT_ENT_WIRE,false);
	m_pInputEntityTool->SetNaturalMode(true);
	
	return TRUE;
}

BOOL	kcmLoopToFace::DestroyInputTools()
{
	KC_SAFE_DELETE(m_pInputEntityTool);
	return TRUE;
}

BOOL	kcmLoopToFace::InitInputToolQueue()
{
	AddInputTool(m_pInputEntityTool);

	return TRUE;
}

// �ӱպϵĻ�����
BOOL  kcmLoopToFace::DoBuildFaceFromLoop()
{
	TopTools_ListOfShape shapeList;
	kiSelSet *pSelSet = m_pInputEntityTool->GetSelSet();
	if(pSelSet->IsEmpty())
		return FALSE;

	kiSelEntity se;
	pSelSet->InitSelected();
	while(pSelSet->MoreSelected()){
		se = pSelSet->CurSelected();
		TopoDS_Shape aSS = se.SelectShape();
		if(BRep_Tool::IsClosed(aSS)){
			shapeList.Append(aSS);
		}

		pSelSet->NextSelected();
	}
	if(shapeList.IsEmpty())
		return FALSE;
	
	//�����ʼƽ��
	BRep_Builder builder;
	TopoDS_Compound aComp;
	builder.MakeCompound(aComp);
	TopTools_ListIteratorOfListOfShape sit;
	for(sit.Init(shapeList);sit.More();sit.Next()){
		TopoDS_Shape aS = sit.Value();
		builder.Add(aComp,aS);
	}

	try
	{
		BRepBuilderAPI_FindPlane findPlane(aComp);
		if(!findPlane.Found())
			return FALSE;

		BRepBuilderAPI_MakeFace mf(findPlane.Plane(),Precision::Confusion());
		if(!mf.IsDone())
			return FALSE;

		TopoDS_Face aFace = mf.Face();

		BRepAlgo_FaceRestrictor fr;
		fr.Init(aFace,Standard_True,Standard_True);

		for(sit.Init(shapeList);sit.More();sit.Next()){
			TopoDS_Shape aS = sit.Value();
			if(aS.ShapeType() == TopAbs_EDGE){
				TopoDS_Edge aEdge = QxBRepLib::CopyEdge(TopoDS::Edge(aS));
				if(!aEdge.IsNull())
				{
					BRepBuilderAPI_MakeWire mw(aEdge);
					if(mw.IsDone())
					{
						TopoDS_Wire aWire = mw.Wire();
						fr.Add(aWire);
					}
				}
			}else{
				TopoDS_Wire aWire = TopoDS::Wire(aS);
				if(!aWire.IsNull())
					fr.Add(aWire);
			}
		}
		fr.Perform();
		if(fr.IsDone()){
			kcModel *pModel = GetModel();
			pModel->BeginUndo(GetName());
			TopoDS_Face aFace;
			for(;fr.More();fr.Next()){
				aFace = fr.Current();
				if(!aFace.IsNull()){
					kcFaceEntity *pSurfEnt = new kcFaceEntity;
					if(pSurfEnt){
						pSurfEnt->SetFace(aFace);
						pModel->AddEntity(pSurfEnt);
					}
				}
			}
			pModel->EndUndo();

			Redraw();

			return TRUE;
		}
	}
	catch (Standard_Failure){
		
	}
	return TRUE;
}

//
BOOL	kcmLoopToFace::BuildFaceFromLoop()
{
	TopTools_ListOfShape shapeList;
	kiSelSet *pSelSet = m_pInputEntityTool->GetSelSet();
	if(pSelSet->IsEmpty())
		return FALSE;

	kiSelEntity se;
	pSelSet->InitSelected();
	while(pSelSet->MoreSelected())
	{
		se = pSelSet->CurSelected();
		pSelSet->NextSelected();

		TopoDS_Shape aSS = se.SelectShape();
		if(BRep_Tool::IsClosed(aSS)){
			shapeList.Append(aSS);
		}
		//if(se._aShape.ShapeType() == TopAbs_EDGE){
		//	
		//	//if(se._pEntity->IsClosed())
		//	//	entList.push_back(se);
		//}
		//else if(IS_WIRE_ENTITY(se._pEntity))
		//{
		//	if(se._pEntity->IsClosed())
		//		entList.push_back(se);
		//}
	}
	if(shapeList.IsEmpty())
		return FALSE;
	//if(entList.empty())
	//	return FALSE;

	//�����ʼƽ��
	BRep_Builder builder;
	TopoDS_Compound aComp;
	builder.MakeCompound(aComp);
	TopTools_ListIteratorOfListOfShape sit;
	for(sit.Init(shapeList);sit.More();sit.Next()){
		TopoDS_Shape aS = sit.Value();
		builder.Add(aComp,aS);
	}
	//kiSelEntityList::iterator it = entList.begin();
	//for(;it != entList.end();it ++)
	//{
	//	kcEntity *pEnt = (*it)._pEntity;
	//	if(IS_EDGE_ENTITY(pEnt))
	//	{
	//		kcEdgeEntity *pCurEnt = (kcEdgeEntity *)pEnt;
	//		builder.Add(aComp,pCurEnt->GetEdge());
	//	}
	//	else
	//	{
	//		kcWireEntity *pWireEnt = (kcWireEntity *)pEnt;
	//		builder.Add(aComp,pWireEnt->GetWire());
	//	}
	//}

	try
	{
		BRepBuilderAPI_FindPlane findPlane(aComp);
		if(!findPlane.Found())
			return FALSE;

		BRepBuilderAPI_MakeFace mf(findPlane.Plane(),Precision::Confusion());
		if(!mf.IsDone())
			return FALSE;

		TopoDS_Face aFace = mf.Face();

		BRepAlgo_FaceRestrictor fr;
		fr.Init(aFace,Standard_True,Standard_True);

		for(sit.Init(shapeList);sit.More();sit.Next()){
			TopoDS_Shape aS = sit.Value();
			if(aS.ShapeType() == TopAbs_EDGE){
				TopoDS_Edge aEdge = QxBRepLib::CopyEdge(TopoDS::Edge(aS));
				if(!aEdge.IsNull())
				{
					BRepBuilderAPI_MakeWire mw(aEdge);
					if(mw.IsDone())
					{
						TopoDS_Wire aWire = mw.Wire();
						fr.Add(aWire);
					}
				}
			}else{
				TopoDS_Wire aWire = QxBRepLib::CopyWire(TopoDS::Wire(aS));
				if(!aWire.IsNull())
					fr.Add(aWire);
			}
		}
		//it = entList.begin();
		//for(;it != entList.end();it ++)
		//{
		//	kcEntity *pEnt = (*it)._pEntity;
		//	if(IS_EDGE_ENTITY(pEnt))
		//	{
		//		kcEdgeEntity *pCurEnt = (kcEdgeEntity *)pEnt;
		//		TopoDS_Edge aEdge = QxBRepLib::CopyEdge(pCurEnt->GetEdge());
		//		if(!aEdge.IsNull())
		//		{
		//			BRepBuilderAPI_MakeWire mw(aEdge);
		//			if(mw.IsDone())
		//			{
		//				TopoDS_Wire aWire = mw.Wire();
		//				fr.Add(aWire);
		//			}
		//		}
		//	}
		//	else
		//	{
		//		kcWireEntity *pWireEnt = (kcWireEntity *)pEnt;
		//		TopoDS_Wire aWire = QxBRepLib::CopyWire(pWireEnt->GetWire());
		//		if(!aWire.IsNull())
		//			fr.Add(aWire);
		//	}
		//}
		fr.Perform();
		if(fr.IsDone())
		{
			kcModel *pModel = GetModel();
			pModel->BeginUndo(GetName());
			TopoDS_Face aFace;
			for(;fr.More();fr.Next())
			{
				aFace = fr.Current();
				if(!aFace.IsNull())
				{
					kcFaceEntity *pSurfEnt = new kcFaceEntity;
					if(pSurfEnt)
					{
						pSurfEnt->SetFace(aFace);
						pModel->AddEntity(pSurfEnt);
					}
				}
			}
			pModel->EndUndo();

			return TRUE;
		}
	}
	catch (Standard_Failure){
		
	}

	return FALSE;
}