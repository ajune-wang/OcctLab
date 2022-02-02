//
#include "stdafx.h"
#include "kcmDispColor.h"
#include "kcmDispShadingModel.h"
#include "kcmShapeFix.h"
#include "kcmBasePlane.h"
#include "kcmShapeCheck.h"
#include "QxCommandItem.h"
#include "kcmShapeProperty.h"
#include "kcmShapeStatistic.h"
#include "kcmAnimationDemo.h"
#include "kcmLengthDimension.h"
#include "kcmDiameterDim.h"
#include "kcmRadiusDim.h"
#include "kcmAngleDim.h"
#include "QxToolPlugin.h"

//����һ���Լ�����
class QxToolPluginImpl : public QxCmdPluginImpl{
public:
	QxToolPluginImpl();
	~QxToolPluginImpl();

	// ��ȡ����е��������
	virtual int		GetCommandNum() const;
	// ��ȡ���������
	virtual const char*	GetPluginName() const;

	// �����ʼ�����ͷ�
	virtual bool		Initialize();
	virtual void		Release();

	// ���������ڲ���е�ID�������������
	virtual kiCommand*			DoCreateCommand(int nLocalID);
	// �ͷŴ������������
	virtual bool					DoReleaseCommand(kiCommand *pCommand);
};

QxCmdPluginImpl* QxCmdPluginMain(QxCmdPluginCfg *pCfg)
{
	QxToolPluginImpl *pImpl = new QxToolPluginImpl;
	return pImpl;
}

//
void QxCmdPluginFree(QxCmdPluginImpl *pImpl)
{
	if(pImpl){
		delete pImpl;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//
QxToolPluginImpl::QxToolPluginImpl()
{
}

QxToolPluginImpl::~QxToolPluginImpl()
{
}

// ��ȡ����е��������
int QxToolPluginImpl::GetCommandNum() const
{
	return 30;
}

// ��ȡ���������
const char* QxToolPluginImpl::GetPluginName() const
{
	return "Occt���߲��";
}

enum eCmdPluginCmdType{
	CMD_DISP_COLOR = 0,
	CMD_DISP_WIREFRM,
	CMD_DISP_SHADING,
	CMD_BPLN_OFFSET,
	CMD_BPLN_THREEPNT,
	CMD_BPLN_FACE,
	CMD_ID_CHECK_SHAPE_VALID,
	CMD_ID_CHECK_TOPOL_VALID,
	CMD_ID_CHECK_SHAPE_CLOSED,
	CMD_ID_SHAPE_FIX,
	CMD_STAT_SHAPE,
	CMD_SHAPE_PROPERTY,
	CMD_EDGE_PROPERTY,
	CMD_FACE_PROPERTY,
	CMD_ANIMA_DEMO,
	CMD_LENGTH_DIM,
	CMD_DIAMETER_DIM,
	CMD_RADIUS_DIM,
	CMD_ANGGLE_DIM_TWO_LINE,
};

// �����ʼ�����ͷ�
bool QxToolPluginImpl::Initialize()
{
	const char *pszDispMenu = "��ʾ(&D)";
	const char *pszToolMenu = "����(&T)";
	const char *pszViewMenu = "��ͼ(&V)";
	
	AddCommandItem((int)CMD_DISP_COLOR,pszDispMenu,"��ɫ(&C)",KUI_MENU_BEFORE_SEP);
	AddCommandItem((int)CMD_DISP_WIREFRM,pszDispMenu,"�����ʾ(&W)");
	AddCommandItem((int)CMD_DISP_SHADING,pszDispMenu,"��Ⱦ��ʾ(&S)");

	//��׼��
	AddCommandItem((int)CMD_BPLN_FACE,pszViewMenu,"��׼��","����Face(&F)");
	AddCommandItem((int)CMD_BPLN_OFFSET,pszViewMenu,"��׼��","ƫ��(&O)",KUI_MENU_BEFORE_SEP);
	AddCommandItem((int)CMD_BPLN_THREEPNT,pszViewMenu,"��׼��","����(&T)");
	
	////���
	AddCommandItem((int)CMD_ID_CHECK_SHAPE_VALID,pszToolMenu,"���","������Ч��(&V)",KUI_MENU_BEFORE_SEP);
	AddCommandItem((int)CMD_ID_CHECK_TOPOL_VALID,pszToolMenu,"���","������Ч��(&T)");
	AddCommandItem((int)CMD_ID_CHECK_SHAPE_CLOSED,pszToolMenu,"���","�պ���(&C)");

	AddCommandItem((int)CMD_ID_SHAPE_FIX,pszToolMenu,"����","��������(&O)");

	AddCommandItem((int)CMD_STAT_SHAPE,pszToolMenu,"ͳ��(&C)");
	AddCommandItem((int)CMD_SHAPE_PROPERTY,pszToolMenu,"��������(&P)",KUI_MENU_BEFORE_SEP);
	AddCommandItem((int)CMD_EDGE_PROPERTY,pszToolMenu,"������(&E)");
	AddCommandItem((int)CMD_FACE_PROPERTY,pszToolMenu,"������(&F)");

	AddCommandItem((int)CMD_ANIMA_DEMO,pszToolMenu,"������ʾ(&T)",KUI_MENU_BEFORE_SEP);

	AddCommandItem((int)CMD_LENGTH_DIM,pszToolMenu,"��ע","���Ա�ע(&L)");
	AddCommandItem((int)CMD_DIAMETER_DIM,pszToolMenu, "��ע", "ֱ����ע(&D)");
	AddCommandItem((int)CMD_RADIUS_DIM, pszToolMenu, "��ע", "�뾶��ע(&R)");
	AddCommandItem((int)CMD_ANGGLE_DIM_TWO_LINE, pszToolMenu, "��ע", "���߽Ƕȱ�ע(&R)");

	return true;
}

void QxToolPluginImpl::Release()
{
}

// ���������ڲ���е�ID�������������
kiCommand* QxToolPluginImpl::DoCreateCommand(int nLocalID)
{
	kiCommand *pCommand = NULL;
	switch(nLocalID){
	case CMD_DISP_COLOR:		pCommand = new kcmDispColor; break;
	case CMD_DISP_WIREFRM:		pCommand = new kcmDispWireFrame; break;
	case CMD_DISP_SHADING:		pCommand = new kcmDispShading; break;
	case CMD_ID_CHECK_SHAPE_VALID:	pCommand = new kcmShapeCheckValid; break;
	case CMD_ID_CHECK_TOPOL_VALID:	pCommand = new kcmShapeCheckTopoValid; break;
	case CMD_ID_CHECK_SHAPE_CLOSED:	pCommand = new kcmShapeCheckClose; break;
	case CMD_BPLN_OFFSET:		pCommand = new kcmOffsetBasePlane; break;
	case CMD_BPLN_THREEPNT:	pCommand = new kcmThreePntBasePlane; break;
	case CMD_BPLN_FACE:	pCommand = new kcmBasePlaneByFace; break;
	case CMD_ID_SHAPE_FIX:	pCommand = new kcmShapeFix; break;
	case CMD_STAT_SHAPE:		pCommand = new kcmShapeStatistic; break;
	case CMD_SHAPE_PROPERTY:	pCommand = new kcmShapeProperty; break;
	case CMD_EDGE_PROPERTY:	pCommand = new kcmEdgeProperty; break;
	case CMD_FACE_PROPERTY:	pCommand = new kcmFaceProperty; break;
	case CMD_ANIMA_DEMO:	pCommand = new kcmAnimationDemo; break;
	case CMD_LENGTH_DIM:	pCommand = new kcmLengthDimension(0); break;
	case CMD_DIAMETER_DIM:	pCommand = new kcmDiameterDim; break;
	case CMD_RADIUS_DIM:	pCommand = new kcmRadiusDim; break;
	case CMD_ANGGLE_DIM_TWO_LINE:	pCommand = new kcmAngleDim(0); break;
	default:
		break;
	}
	return pCommand;
}

// �ͷŴ������������
bool QxToolPluginImpl::DoReleaseCommand(kiCommand *pCommand)
{
	if(pCommand){
		pCommand->Destroy();
		delete pCommand;
		return true;
	}
	return false;
}