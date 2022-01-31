#include "StdAfx.h"
#include "QxCommandItem.h"
#include "kcmDispVisible.h"
#include "kcmUCSMgr.h"
#include "kcmShapeAnalysis.h"
#include "kcmDbgTool.h"
#include "kuiSyscmdPlugin.h"

enum eSysCmdPluginCmdType{
	CMD_DISP_HIDESEL = 0,
	CMD_DISP_SHOWALL,
	CMD_DISP_TYPE_FILLTER,
	CMD_DISP_HIDE_GRID,
	CMD_CHECK_SHAPE_VALID,
	CMD_CHECK_TOPOL_VALID,
	CMD_CHECK_SHAPE_CLOSED,
	CMD_ANALYSIS_FACE_NORMAL,
	CMD_ANALYSIS_SHAPE_NORMAL,
	CMD_ANALYSIS_FACE_ORIENT,
	CMD_ANALYSIS_FACE_UV_ORIENT,
	CMD_ANALYSIS_FACE_INFO,
	CMD_ANALYSIS_FACE_WIREORI,
	CMD_ANALYSIS_BAT_STEPFILE,
	CMD_ANALYSIS_NBS_POLES,
	CMD_ANALYSIS_CURVE_CURVATURE,
	CMD_ANALYSIS_MESH_DISPLAY,
	CMD_FIX_SHAPE_FIX,
	CMD_DBGTOOL_INPUT_FNL,
	CMD_TEST_FOR_DEBUG,
};

kuiSyscmdPlugin::kuiSyscmdPlugin(void)
{
	_strName = "ϵͳ������";
	_nReserveNum = 50;
}

kuiSyscmdPlugin::~kuiSyscmdPlugin(void)
{
}

//��ʼ��
BOOL  kuiSyscmdPlugin::Initialize()
{
	const char *pszDispMenu = "��ʾ(&D)";
	const char *pszViewMenu = "��ͼ(&V)";
	const char *pszToolMenu = "����(&T)";
	const char *pszAnalysisMenu = "����(&S)";

	AddCommandItem((int)CMD_DISP_HIDESEL,pszDispMenu,"����ѡ�����(&H)");
	AddCommandItem((int)CMD_DISP_SHOWALL,pszDispMenu,"��ʾ���ض���(&H)");
	AddCommandItem((int)CMD_DISP_TYPE_FILLTER,pszDispMenu,"������ʾ����(&F)");
	AddCommandItem((int)CMD_DISP_HIDE_GRID,pszDispMenu,"����������ʾ");
		

	AddCommandItem((int)CMD_ANALYSIS_FACE_ORIENT,pszToolMenu,"����","���淽��(&O)",KUI_MENU_BEFORE_SEP);
	AddCommandItem((int)CMD_ANALYSIS_FACE_INFO,pszToolMenu,"����","FACE�ṹ����(&F)");
	AddCommandItem((int)CMD_ANALYSIS_FACE_WIREORI,pszToolMenu,"����","FACE������(&F)");
	AddCommandItem((int)CMD_ANALYSIS_BAT_STEPFILE,pszToolMenu,"����","��������STEP�ļ�(&B)");

	//����
	AddCommandItem((int)CMD_ANALYSIS_NBS_POLES,pszAnalysisMenu,"��ʾ���Ƶ�(&P)");
	AddCommandItem((int)CMD_ANALYSIS_CURVE_CURVATURE,pszAnalysisMenu,"��������(&C)");
	AddCommandItem((int)CMD_ANALYSIS_MESH_DISPLAY,pszAnalysisMenu,"����������ʾ(&M)");
	AddCommandItem((int)CMD_ANALYSIS_FACE_UV_ORIENT,pszAnalysisMenu,"����UV����(&O)");
	AddCommandItem((int)CMD_ANALYSIS_FACE_NORMAL,pszAnalysisMenu,"Face����(&N)");
	AddCommandItem((int)CMD_ANALYSIS_SHAPE_NORMAL,pszAnalysisMenu,"Shape����(&N)");

	AddCommandItem((int)CMD_DBGTOOL_INPUT_FNL,pszToolMenu,"��������");
	AddCommandItem((int)CMD_TEST_FOR_DEBUG,pszToolMenu,"��������");

	return TRUE;
}

kiCommand*  kuiSyscmdPlugin::DoCreateCommand(int nLocalID)
{
	kiCommand *pCommand = NULL;
	switch(nLocalID){
	case CMD_DISP_HIDESEL:		pCommand = new kcmDispHideSelect; break;
	case CMD_DISP_SHOWALL:		pCommand = new kcmDispShowAll; break;
	case CMD_DISP_TYPE_FILLTER:	pCommand = new kcmDispTypeFilter; break;
	case CMD_DISP_HIDE_GRID:		pCommand = new kcmHideGridDisplay; break;
	case CMD_ANALYSIS_SHAPE_NORMAL:	pCommand = new kcmShapeAnalysisNormal; break;
	case CMD_ANALYSIS_FACE_NORMAL:	pCommand = new kcmFaceNormalAnalysis; break;
	case CMD_ANALYSIS_FACE_ORIENT:	pCommand = new kcmShapeAnalysisFaceOrient; break;
	case CMD_ANALYSIS_FACE_UV_ORIENT:	pCommand = new kcmFaceOrientAnalysis; break;
	case CMD_ANALYSIS_FACE_INFO:	pCommand = new kiFacePropInfo; break;
	case CMD_ANALYSIS_FACE_WIREORI:	pCommand = new kiFaceWireOri; break;
	case CMD_ANALYSIS_BAT_STEPFILE:	pCommand = new kiBatAnalyze; break;
	case CMD_ANALYSIS_NBS_POLES:	pCommand = new kiDisplayNbsPoles; break;
	case CMD_ANALYSIS_CURVE_CURVATURE:	pCommand = new kcmCurveCurvature; break;
	case CMD_ANALYSIS_MESH_DISPLAY:	pCommand = new kcmMeshDisplay; break;
	case CMD_DBGTOOL_INPUT_FNL:	pCommand = new kcmInputFaceNoLF; break;
	case CMD_TEST_FOR_DEBUG:	pCommand = new kcmTestForDebug; break;
	default:
		break;
	}
	return pCommand;
}

bool  kuiSyscmdPlugin::DoReleaseCommand(kiCommand *pCommand)
{
	if(pCommand){
		pCommand->Destroy();
		delete pCommand;
		return true;
	}
	return false;
}