#include "stdafx.h"
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include "kcLayer.h"
#include "kcModel.h"
#include "kcEntityFactory.h"
#include "kcgTextLib.h"
//#include "TextEditDialog.h"
#include "kcmDbgTool.h"

kcmInputFaceNoLF::kcmInputFaceNoLF()
{
	m_strName = "InputFaceNoLF";
	m_strAlias = "InFNL";
	m_strDesc = "����face�ı��";
}

kcmInputFaceNoLF::~kcmInputFaceNoLF()
{
}

// ��ʼִ������.
//ִ�кͽ����ӿ�
int  kcmInputFaceNoLF::Execute()
{
	//CFileDialog dlg(TRUE,
	//	NULL,
	//	NULL,
	//	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	//	"FNLF File (*.fnlf)|*.fnlf||", 
	//	NULL );

	//if(dlg.DoModal() == IDOK){
	//	CWaitCursor wc;

	//	CString szFile = dlg.GetPathName();

	//	FILE *fp = NULL;
	//	fopen_s(&fp,szFile,"r");
	//	if(fp == NULL){
	//		AfxMessageBox("Open File Failed.");
	//		return KSTA_FAILED;
	//	}

	//	CTextEditDialog teDlg;
	//	teDlg.m_dTextHeight = 5.0;
	//	teDlg.m_szText = "hh";
	//	if(teDlg.DoModal() != IDOK){
	//		return KSTA_FAILED;
	//	}


	//	TopTools_ListOfShape aShapeList;
	//	char szLine[1024],szStr[64];
	//	int nFx = 0;
	//	double pnt[3],Z[3],X[3];
	//	while(fgets(szLine,1024,fp)){
	//		sscanf_s(szLine,"%d : P={%lf,%lf,%lf},Z={%lf,%lf,%lf},X={%lf,%lf,%lf}",
	//			&nFx,&pnt[0],&pnt[1],&pnt[2],&Z[0],&Z[1],&Z[2],&X[0],&X[1],&X[2]);

	//		gp_Pnt pos(pnt[0],pnt[1],pnt[2]);
	//		gp_Vec zv(Z[0],Z[1],Z[2]),xv(X[0],X[1],X[2]);
	//		zv.Normalize();
	//		pos.Translate(zv.Multiplied(0.1));//��΢�뿪surface����

	//		gp_Ax2 ax2(pos,gp_Dir(zv),gp_Dir(xv));
	//		gp_Ax3 ax3(ax2),dfax3;
	//		gp_Trsf trsf;
	//		trsf.SetTransformation(ax3,dfax3);

	//		//���ɶ���
	//		sprintf_s(szStr,64,"%d",nFx);
	//		TopoDS_Compound aComp = MakeWireText(szStr,teDlg.m_logFont,teDlg.m_dTextHeight,teDlg.m_nCharSpace);
	//		if(!aComp.IsNull()){
	//			//���б任
	//			BRepBuilderAPI_Transform bbTrsf(trsf);
	//			bbTrsf.Perform(aComp);
	//			TopoDS_Shape aS = bbTrsf.Shape();
	//			if(!aS.IsNull()){
	//				aShapeList.Append(aS);
	//			}
	//		}
	//	}

	//	fclose(fp);

	//	//��ӵ�ϵͳ��
	//	//����һ����ͼ�㣬���½�������ӵ���ͼ����
	//	kcModel *pModel = GetModel();
	//	if(pModel != NULL){
	//		kcLayer *pLayer = pModel->CreateNewLayer();
	//		if(!pLayer)
	//			return FALSE;

	//		kcEntityFactory efac;
	//		TopTools_ListIteratorOfListOfShape lls;
	//		for(lls.Initialize(aShapeList);lls.More();lls.Next()){
	//			TopoDS_Shape aS = lls.Value();
	//								
	//			kcEntity *pEntity = efac.Create(aS);
	//			if(pEntity != NULL){
	//				pModel->AddEntity(pEntity,pLayer);
	//			}
	//		}
	//	}
	//}
	return KSTA_DONE;
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL  kcmInputFaceNoLF::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

TopoDS_Compound  kcmInputFaceNoLF::MakeWireText(const char *lpszText,LOGFONT& lf,double dHeight,double dGap)
{
	TopoDS_Compound aComp;
	BRep_Builder BB;
	BOOL bDone = FALSE;
	CView *pView = GetActiveView();
	ASSERT(pView);
	CDC *pDC = pView->GetDC();
	HDC hDC = pDC->GetSafeHdc();

	BB.MakeCompound(aComp);

	TopTools_ListOfShape aList;
	kcFontText fontText(lf);
	if(fontText.GetText(hDC,lpszText,dHeight,dGap,TRUE,aList)){
		TopTools_ListIteratorOfListOfShape lls;
		for(lls.Initialize(aList);lls.More();lls.Next()){
			BB.Add(aComp,lls.Value());
		}
	}

	pView->ReleaseDC(pDC);

	return aComp;
}

////////////////////////////////////////////////////////
//

kcmTestForDebug::kcmTestForDebug()
{
	m_strName = "TestForDebug";
	m_strAlias = "TFDbg";
	m_strDesc = "���Ե�������";
}

kcmTestForDebug::~kcmTestForDebug()
{
}

// ��ʼִ������.
//ִ�кͽ����ӿ�
int  kcmTestForDebug::Execute()
{
	PntInSolidTest();

	return KSTA_DONE;
}

// ��������������,��Ҫ��Apply�������ʹ��.
BOOL  kcmTestForDebug::CanFinish()//�����Ƿ�������
{
	return FALSE;
}

void  kcmTestForDebug::PntInSolidTest()
{
	BRepPrimAPI_MakeBox mBox(50,40,20);
	TopoDS_Shape aSolid = mBox.Solid();

	gp_Pnt aPnt(40,30,15),aPnt2(40,-30,15);
	BRepClass3d_SolidClassifier scls;
	scls.Load(aSolid);
	scls.Perform(aPnt,Precision::Confusion());
	TopAbs_State stat = scls.State();

	scls.Perform(aPnt2,Precision::Confusion());
	stat = scls.State();
	
	int a = 100;
}