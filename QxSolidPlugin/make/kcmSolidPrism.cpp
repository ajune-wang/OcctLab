#include "StdAfx.h"
#include "kiInputEntityTool.h"
#include "kiInputVector.h"
#include "kiInputValueTool.h"
#include "kiInputLength.h"
#include "kcEntity.h"
#include "kcBasePlane.h"
#include "kcModel.h"
#include "kiOptionSet.h"
#include "kvCoordSystem.h"
#include "kcmSolidPrism.h"

kcmSolidPrism::kcmSolidPrism(void)
{
	m_strName = "Solid Prism";
	m_strAlias = "SolidPrism";
	m_strDesc = "prism face to solid";

	pFaceOptionSet_ = NULL;
	pFacePickTool_ = NULL;
	pMainOptionSet_ = NULL;
	pInputLenTool_ = NULL;

	dExtrudeLen_ = 10.0;
	nDirFlag_ = 0;
}

kcmSolidPrism::~kcmSolidPrism(void)
{
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL	kcmSolidPrism::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int	kcmSolidPrism::OnInputFinished(kiInputTool *pTool)
{
	if(pTool == pFacePickTool_)
	{
		if(pTool->IsInputDone())
		{
			QxShapePickInfo pickInfo;
			pFacePickTool_->GetShapePickInfo(pickInfo);
			kcBasePlane *pBPln = GetCurrentBasePlane();
			pInputLenTool_->Set(pickInfo._p,pBPln->GetZDirection(),true);

			NavToNextTool();
		}
	}
	else if(pTool == pInputLenTool_)
	{
		if(pTool->IsInputDone())
		{
			if(MakePrismShape())
			{
				pFacePickTool_->ClearSel();
				Done();
				Redraw();

				return KSTA_DONE;
			}
		}
	}
	return KSTA_CONTINUE;
}


int	kcmSolidPrism::OnExecute()
{
	return KSTA_CONTINUE;
}

int kcmSolidPrism::OnEnd(int nCode)
{
	return nCode;
}

void kcmSolidPrism::OnOptionChanged(kiOptionItem *pOptItem)
{
	TRACE("\n XXXX----, nDirFlag is %d", nDirFlag_);
	if(!pOptItem) return;

	if(pOptItem->GetType() == KOPT_INPUT_ENUM){
		kiOptionEnum *pOptEnum = (kiOptionEnum *)pOptItem;
		int preVal = pOptEnum->GetPrevEnumValue();
		if(nDirFlag_ == 1){ //������
			pInputLenTool_->ReverseBaseDir();
		}else if(preVal == 1){//��Ҫ�ָ�����
			pInputLenTool_->ReverseBaseDir();
		}else{
		}
	}
}

static TopoDS_Face uxMoveFace(const TopoDS_Face &aF,const kVector3& vdir)
{
	TopoDS_Shape aNS;
	gp_Trsf trsf;
	trsf.SetTranslation(gp_Vec(vdir.x(),vdir.y(),vdir.z()));
	try
	{
		BRepBuilderAPI_Transform trans(aF,trsf,Standard_True);
		if(trans.IsDone())
			aNS = trans.Shape();
	}catch (Standard_Failure){
	}
	return TopoDS::Face(aNS);
}

//
BOOL	kcmSolidPrism::MakePrismShape()
{
	kiSelSet *pSelSet = pFacePickTool_->GetSelSet();
	if(pSelSet->GetSelCount() != 1)
		return FALSE;

	pSelSet->InitSelected();
	kiSelEntity se = pSelSet->CurSelected();
	if(!IS_FACE_ENTITY(se._pEntity))
		return FALSE;

	TopoDS_Shape aS = se.SelectShape();
	TopoDS_Face aFace = TopoDS::Face(aS);

	kcBasePlane *pBPln = GetCurrentBasePlane();
	kVector3 vdir = pBPln->GetZDirection();
	dExtrudeLen_ = pInputLenTool_->GetLength(); //�����Ǹ�ֵ
	//
	if(nDirFlag_ == 0){// ͬ��
		vdir *= dExtrudeLen_; 
	}else if(nDirFlag_ == 1){//����
		vdir *= -dExtrudeLen_;
	}else{
		// �����ƶ�face
		kVector3 mvdir = vdir * dExtrudeLen_;
		mvdir.reverse();
		aFace = uxMoveFace(aFace, mvdir);

		vdir *= 2 * dExtrudeLen_;
	}

	kcEntity *pNewEntity = NULL;
	TopoDS_Shape aShape;
	try{
		BRepPrimAPI_MakePrism mp(aFace,gp_Vec(vdir.x(),vdir.y(),vdir.z()),Standard_True);
		if(mp.IsDone()){
			aShape = mp.Shape();
			if(aShape.IsNull())
				return FALSE;
		}
	}catch (Standard_Failure){
		return FALSE;
	}

	if(aShape.ShapeType() == TopAbs_SOLID){
		TopoDS_Solid aSolid = TopoDS::Solid(aShape);
		kcSolidEntity *pSolidEnt = new kcSolidEntity;
		pSolidEnt->SetSolid(aSolid);
		pNewEntity = pSolidEnt;
	}

	if(pNewEntity)
	{
		kcModel *pModel = GetModel();
		pModel->BeginUndo(GetName());
		pModel->AddEntity(pNewEntity);
		pModel->EndUndo();
	}

	return TRUE;
}

// ������Ҫ�����빤��
BOOL kcmSolidPrism::CreateInputTools()
{
	pFaceOptionSet_ = new kiOptionSet(this);
	pFaceOptionSet_->AddSysButtonOption(QX_SYS_BUTTON_CANCEL);

	pFacePickTool_ = new kiInputEntityTool(this,"ѡ��һ��Face:", pFaceOptionSet_);
	pFacePickTool_->SetOption(KCT_ENT_FACE,1);
	pFacePickTool_->SetNaturalMode(false); //����ѡ������ģ�͵���
	pFacePickTool_->NeedPickInfo(true);

	pMainOptionSet_ = new kiOptionSet(this);
	kiOptionEnum *pDirEnum = new kiOptionEnum("���췽��",'D',nDirFlag_);
	pDirEnum->AddEnumItem("����",'F');
	pDirEnum->AddEnumItem("����",'R');
	pDirEnum->AddEnumItem("˫��",'B');
	pMainOptionSet_->AddOption(pDirEnum);
	pMainOptionSet_->AddSysButtonOption(QX_SYS_BUTTON_CANCEL);

	pInputLenTool_ = new kiInputLength(this,"�����������:", pMainOptionSet_);
	
	return TRUE;
}

BOOL kcmSolidPrism::DestroyInputTools()
{
	KC_SAFE_DELETE(pFacePickTool_);
	KC_SAFE_DELETE(pInputLenTool_);

	return TRUE;
}

BOOL kcmSolidPrism::InitInputToolQueue()
{
	AddInputTool(pFacePickTool_);
	AddInputTool(pInputLenTool_);

	return TRUE;
}
