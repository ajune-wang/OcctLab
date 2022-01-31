//2021.12.20
#include "stdafx.h"
#include "QxCommandItem.h"
#include "kiCommand.h"
#include "kcmPlane.h"
#include "kcmRevolSurface.h"
#include "kcmPrismSurface.h"
#include "kcmLoopToFace.h"
#include "kcmLoftSurface.h"
#include "kcmSurfPolygon.h"
#include "kcmRegionBoolean.h"
#include "kcmRuledSurface.h"
#include "kcmSurfaceFilling.h"
#include "kcmSurfacePipe.h"
#include "kcmSurfaceSplit.h"
#include "kcmSurfaceTool.h"
#include "QxSurfPlugin.h"

//����һ���Լ�����
class QxSurfPluginImpl : public QxCmdPluginImpl{
public:
	QxSurfPluginImpl();
	~QxSurfPluginImpl();

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
	QxSurfPluginImpl *pImpl = new QxSurfPluginImpl;
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
QxSurfPluginImpl::QxSurfPluginImpl()
{
	eTopMenuID_ = QX_TOP_MENU_SURFACE;
}

QxSurfPluginImpl::~QxSurfPluginImpl()
{
}

// ��ȡ����е��������
int QxSurfPluginImpl::GetCommandNum() const
{
	return 30;
}

// ��ȡ���������
const char* QxSurfPluginImpl::GetPluginName() const
{
	return "Occt�������Ͳ��";
}

enum eCmdPluginCmdType{
	CMD_SURF_PLANE = 0,
	CMD_SURF_PLN_POLYGON,
	CMD_SURF_PLN_LOOP_TO_FACE,
	CMD_SURF_REV_SURF,
	CMD_SURF_PRISM,
	CMD_SURF_LOFT,
	CMD_SURF_RULED,
	CMD_SURF_PIPE,
	CMD_SURF_SWEEP1,
	CMD_SURF_SWEEPN,
	CMD_SURF_SPLIT,
	CMD_SURF_FILLING,
	CMD_SURF_PRIMARY,
	CMD_REGION_BOOL_FUSE,
	CMD_REGION_BOOL_COMMON,
	CMD_REGION_BOOL_CUT,
	CMD_HELICOID_ARCHIMEDES,
	CMD_HELICOID_SCREWINVSURF,
	CMD_HELICOID_ARCHIMEDES_ISOCRV,
};

// �����ʼ�����ͷ�
bool QxSurfPluginImpl::Initialize()
{
	//const char *pszTopMenu = "����(&S)";

	DoAddCommandItem((int)CMD_SURF_PLANE,"ƽ��(&P)","����ƽ��(&T)");
	DoAddCommandItem((int)CMD_SURF_PLN_POLYGON,"ƽ��(&P)","�����(&P)");
	DoAddCommandItem((int)CMD_SURF_PLN_LOOP_TO_FACE,"ƽ��(&P)","�պϻ�����ƽ��(&P)");
	DoAddCommandItem((int)CMD_SURF_REV_SURF,"��ת��(&R)");
	DoAddCommandItem((int)CMD_SURF_PRISM,"������(&R)");

	DoAddCommandItem((int)CMD_SURF_LOFT,"����(&R)");
	DoAddCommandItem((int)CMD_SURF_RULED,"ֱ����(&L)");
	DoAddCommandItem((int)CMD_SURF_PIPE,"�ܵ���(&P)");
	DoAddCommandItem((int)CMD_SURF_SWEEP1,"ɨ����","����(&S)");
	DoAddCommandItem((int)CMD_SURF_SWEEPN,"ɨ����","�����(&M)");
	DoAddCommandItem((int)CMD_SURF_SPLIT,"�ָ�����(&S)");

	DoAddCommandItem((int)CMD_SURF_FILLING,"����(&F)");

	DoAddCommandItem((int)CMD_SURF_PRIMARY,"ԭʼ��(&P)",KUI_MENU_BEFORE_SEP);

	DoAddCommandItem((int)CMD_REGION_BOOL_FUSE,"����","���򲢼�(&U)",KUI_MENU_BEFORE_SEP);
	DoAddCommandItem((int)CMD_REGION_BOOL_COMMON,"����","���򽻼�(&C)");
	DoAddCommandItem((int)CMD_REGION_BOOL_CUT,"����","����(&D)");
		
	return true;
}

void QxSurfPluginImpl::Release()
{
}

// ���������ڲ���е�ID�������������
kiCommand* QxSurfPluginImpl::DoCreateCommand(int nLocalID)
{
	kiCommand *pCommand = NULL;
	switch(nLocalID){
	case CMD_SURF_PLANE:			pCommand = new kcmPlane; break;
	case CMD_SURF_PLN_POLYGON:		pCommand = new kcmSurfPolygon; break;
	case CMD_SURF_PLN_LOOP_TO_FACE:		pCommand = new kcmLoopToFace; break;
	case CMD_SURF_REV_SURF:		pCommand = new kcmRevolSurface; break;
	case CMD_SURF_PRISM:			pCommand = new kcmPrismSurface; break;
	case CMD_SURF_LOFT:			pCommand = new kcmLoftSurface; break;
	case CMD_SURF_RULED:			pCommand = new kcmRuledSurface; break;
	case CMD_SURF_PIPE:			pCommand = new kcmSurfacePipe; break;
	case CMD_SURF_SWEEP1:			pCommand = new kcmSurfaceSweep1; break;
	case CMD_SURF_SWEEPN:			pCommand = new kcmSurfaceSweepN; break;
	case CMD_SURF_SPLIT:			pCommand = new kcmSurfaceSplit; break;
	case CMD_SURF_FILLING:			pCommand = new kcmSurfaceFilling; break;
	case CMD_SURF_PRIMARY:			pCommand = new kcmPrimarySurface; break;
	case CMD_REGION_BOOL_FUSE:		pCommand = new kcmRegionBoolFuse; break;
	case CMD_REGION_BOOL_COMMON:	pCommand = new kcmRegionBoolCommon; break;
	case CMD_REGION_BOOL_CUT:		pCommand = new kcmRegionBoolCut; break;
	default:
		break;
	}
	return pCommand;
}

// �ͷŴ������������
bool QxSurfPluginImpl::DoReleaseCommand(kiCommand *pCommand)
{
	if(pCommand){
		pCommand->Destroy();
		delete pCommand;
		return true;
	}
	return false;
}