#include "stdafx.h"
#include <map>
#include "kiOptionSet.h"
#include "kcEntity.h"
#include "kcModel.h"
#include "QxSurfLib.h"
#include "kcgAnalysis.h"
#include "kcmShapeStatistic.h"

kcmShapeStatistic::kcmShapeStatistic()
{
	m_strName = "Shape Statistic";
	m_strAlias = "ShapeStat";
	m_strDesc = "����ͳ��";

	_pOptionSet = NULL;
}

kcmShapeStatistic::~kcmShapeStatistic()
{
}

int  kcmShapeStatistic::OnExecute()
{
	RequestEditInput("choise a action",_pOptionSet);
	return KSTA_CONTINUE;
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL  kcmShapeStatistic::CanFinish()//�����Ƿ�������
{
	return TRUE;
}

// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//
int  kcmShapeStatistic::OnInputFinished(kiInputTool *pTool)
{
	return kiCommand::OnInputFinished(pTool);
}

	//ѡ�ѡ����
void  kcmShapeStatistic::OnOptionSelected(kiOptionItem *pOptItem)
{
	if(pOptItem->GetID() == 1){
		//DoCountSurfInfo();
		DoCollectSurfTypeInfo();
	}
	if(pOptItem->GetID() == 2){
		//AfxMessageBox("curve info");
		DoCollectCurveTypeInfo();
	}
	if(pOptItem->GetID() == 3){
		DoGetOrientStat();
	}
}

void  kcmShapeStatistic::DoCollectSurfTypeInfo()
{
	kcgAnalysis cgAnalysis;
	kcEntityList entList;
	kcEntityList::iterator ite;
	kcEntity *pEntity = NULL;
	kcModel *pModel = GetModel();
	
	CWaitCursor wc;

	pModel->GetAllEntity(entList);
	for(ite = entList.begin();ite != entList.end();ite ++){
		pEntity = *ite;
		TopoDS_Shape aShape = pEntity->GetShape();
		if(!aShape.IsNull()){
			cgAnalysis.CollectSurfTypeInfo(aShape);
		}
	}
	
	const char *pszFile = "d:\\DbgDump\\surftypeinfo.txt";
	if(cgAnalysis.WriteToFile(pszFile)){
		//��
		::ShellExecute(NULL,"Open",pszFile,NULL,NULL,SW_SHOWNORMAL);
	}
}

void  kcmShapeStatistic::DoCollectCurveTypeInfo()
{
	kcgAnalysis cgAnalysis;
	kcEntityList entList;
	kcEntityList::iterator ite;
	kcEntity *pEntity = NULL;
	kcModel *pModel = GetModel();
	
	CWaitCursor wc;

	pModel->GetAllEntity(entList);
	for(ite = entList.begin();ite != entList.end();ite ++){
		pEntity = *ite;
		TopoDS_Shape aShape = pEntity->GetShape();
		if(!aShape.IsNull()){
			cgAnalysis.CollectCurveTypeInfo(aShape);
		}
	}
	
	const char *pszFile = "d:\\DbgDump\\crvtypeinfo.txt";
	if(cgAnalysis.WriteToFile(pszFile)){
		//��
		::ShellExecute(NULL,"Open",pszFile,NULL,NULL,SW_SHOWNORMAL);
	}
}

void  kcmShapeStatistic::DoCountSurfInfo()
{
	std::map<int,int> surfCntMap;
	std::map<int,int>::iterator mit;
	kcEntityList entList;
	kcEntityList::iterator ite;
	kcEntity *pEntity = NULL;
	kcModel *pModel = GetModel();

	pModel->GetAllEntity(entList);
	for(ite = entList.begin();ite != entList.end();ite ++){
		pEntity = *ite;
		TopoDS_Shape aShape = pEntity->GetShape();
		if(!aShape.IsNull()){
			if(aShape.ShapeType() > TopAbs_FACE)
				continue;

			TopExp_Explorer topExp;
			for(topExp.Init(aShape,TopAbs_FACE);topExp.More();topExp.Next()){
				TopoDS_Face aFace = TopoDS::Face(topExp.Current());
				TopLoc_Location L;
				Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace,L);
				if(!aSurf.IsNull()){
					int idx = kcgSurfNameAndIndex::GeomSurfIndex(aSurf);
					if(idx >= 0){
						mit = surfCntMap.find(idx);
						if(mit == surfCntMap.end()){
							surfCntMap.insert(std::map<int,int>::value_type(idx,1));
						}else{
							(*mit).second ++;
						}
					}
				}
			}
		}
	}

	if(!surfCntMap.empty()){
		CString szMsg,ss;
		for(mit = surfCntMap.begin();mit != surfCntMap.end();mit ++){
			int idx,cnt;
			idx = (*mit).first;
			cnt = (*mit).second;
			ss.Format("%s count is %d.",kcgSurfNameAndIndex::GeomSurfName(idx),cnt);
			szMsg += ss + "\r\n";
		}

		AfxMessageBox(szMsg);
	}
}

// ͳ��Face��wire��������
void  kcmShapeStatistic::DoGetOrientStat()
{
	int nbRevFace = 0;
	int nbRevWire1 = 0,nbRevWire2 = 0;
	kcEntityList entList;
	kcEntityList::iterator ite;
	kcEntity *pEntity = NULL;
	kcModel *pModel = GetModel();

	pModel->GetAllEntity(entList);
	for(ite = entList.begin();ite != entList.end();ite ++){
		pEntity = *ite;
		TopoDS_Shape aShape = pEntity->GetShape();
		if(!aShape.IsNull()){
			if(aShape.ShapeType() > TopAbs_FACE)
				continue;

			TopExp_Explorer exp;
			exp.Init(aShape,TopAbs_FACE);
			for(;exp.More();exp.Next()){
				const TopoDS_Face &aFace = TopoDS::Face(exp.Current());

				bool bFaceRev = false;
				//face�������
				if(aFace.Orientation() == TopAbs_REVERSED){
					bFaceRev = true;
					nbRevFace ++;
				}

				TopoDS_Iterator ite(aFace,false);
				for(;ite.More();ite.Next()){
					const TopoDS_Wire &aW = TopoDS::Wire(ite.Value());

					if(bFaceRev && aW.Orientation() == TopAbs_REVERSED){
						nbRevWire1 ++;
					}
					if(!bFaceRev && aW.Orientation() == TopAbs_REVERSED){
						nbRevWire2 ++;
					}
				}
			}
		}
	}

	CString szMsg,ss;
	szMsg.Format("Reversed Face : %d",nbRevFace);
	ss.Format("\nReversed Face,Reversed Wire to Surf : %d",nbRevWire1); 
	szMsg += ss;
	ss.Format("\nForward Face,Reversed Wire to Surf : %d",nbRevWire2);
	szMsg += ss;

	AfxMessageBox(szMsg);
}

// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
BOOL  kcmShapeStatistic::CreateInputTools()
{
	_pOptionSet = new kiOptionSet;
	kiOptionButton *pButton = new kiOptionButton("surface stat",'S',1);
	pButton->SetOptionCallBack(this);
	_pOptionSet->AddOption(pButton);

	pButton = new kiOptionButton("curve stat",'C',2);
	pButton->SetOptionCallBack(this);
	_pOptionSet->AddOption(pButton);

	pButton = new kiOptionButton("orient stat",'O',3);
	pButton->SetOptionCallBack(this);
	_pOptionSet->AddOption(pButton);

	return TRUE;
}

// ���ٴ��������빤��.ÿ�����������һ��.
BOOL  kcmShapeStatistic::DestroyInputTools()
{
	KC_SAFE_DELETE(_pOptionSet);
	return TRUE;
}

// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
BOOL  kcmShapeStatistic::InitInputToolQueue()
{
	return TRUE;
}

