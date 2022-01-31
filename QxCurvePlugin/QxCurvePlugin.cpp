//
#include "stdafx.h"
#include "QxCommandItem.h"
#include "kiCommand.h"
#include "QxCommandItem.h"
#include "kcmPoint.h"
#include "kcmLine.h"
#include "kcmCircle.h"
#include "kcmArc.h"
#include "kcmSpline.h"
#include "kcmCurveOffset.h"
#include "kcmRect.h"
#include "kcmCurvePolyline.h"
#include "kcmInterCurve.h"
#include "kcmCurveBreak.h"
#include "kcmCurveTrim.h"
#include "kcmFaceCurve.h"
#include "kcmCurveFillet.h"
#include "kcmNormProjCurve.h"
#include "kcmCurveBlend.h"
#include "kcmSewCurve.h"
#include "kcmWireText.h"
#include "QxCurvePlugin.h"

//����һ���Լ�����
class QxCurvePluginImpl : public QxCmdPluginImpl{
public:
	QxCurvePluginImpl();
	~QxCurvePluginImpl();

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

//

QxCmdPluginImpl* QxCmdPluginMain(QxCmdPluginCfg *pCfg)
{
	QxCurvePluginImpl *pImpl = new QxCurvePluginImpl;
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
QxCurvePluginImpl::QxCurvePluginImpl()
{
	eTopMenuID_ = QX_TOP_MENU_CURVE;
}

QxCurvePluginImpl::~QxCurvePluginImpl()
{
}

// ��ȡ����е��������
int QxCurvePluginImpl::GetCommandNum() const
{
	return 30;
}

// ��ȡ���������
const char* QxCurvePluginImpl::GetPluginName() const
{
	return "Occt�������Ͳ��";
}

enum eCmdPluginCmdType{
	CMD_CRV_POINT = 0,
	CMD_CRV_INTER_POINT,
	CMD_CRV_LINE,
	CMD_CRV_CIRCLE,
	CMD_CRV_ARC,
	CMD_CRV_SPLINE,
	CMD_CRV_UP_SPLINE,
	CMD_CRV_RECT,
	CMD_CRV_POLYLINE,
	CMD_CRV_INTER_CRV,
	CMD_CRV_FACE_BND_EDGE,
	CMD_CRV_FACE_BND_WIRE,
	CMD_CRV_FACE_RAW_WIRE,
	CMD_CRV_FACE_BND_EDGEDIR,
	CMD_CRV_FACE_ISO_CURVE,
	CMD_CRV_NORM_PRJ_CRV,
	CMD_CRV_BREAK_BY_POINT,
	CMD_CRV_BREAK_BY_CURVE,
	CMD_CRV_TRIM,
	CMD_CRV_TRIM_QUICK,
	CMD_CRV_SPLIT_ALL_EDGES,
	CMD_CRV_FILLET,
	CMD_CRV_FILLET_TWO_EDGE,
	CMD_CRV_OFFSET,
	CMD_CRV_BLEND,
	CMD_ID_SEWING_CURVE,
	CMD_CRV_TEXT
};

// �����ʼ�����ͷ�
bool QxCurvePluginImpl::Initialize()
{

	DoAddCommandItem((int)CMD_CRV_POINT,"��(&P)","λ�õ�(&P",KUI_MENU_BEFORE_SEP);
	DoAddCommandItem((int)CMD_CRV_INTER_POINT,"��(&P)","���߽���(&I");
	DoAddCommandItem((int)CMD_CRV_LINE,"ֱ��(&L)");
	DoAddCommandItem((int)CMD_CRV_CIRCLE,"Բ(&C)");
	DoAddCommandItem((int)CMD_CRV_ARC,"Բ��(&A)");
	DoAddCommandItem((int)CMD_CRV_SPLINE,"��������","����(&B)");
	DoAddCommandItem((int)CMD_CRV_UP_SPLINE,"��������","�����ڻ�(&U)");
	DoAddCommandItem((int)CMD_CRV_RECT,"����(&R)");
	DoAddCommandItem((int)CMD_CRV_POLYLINE,"�����(&P)");
	DoAddCommandItem((int)CMD_CRV_INTER_CRV,"����(&I)",KUI_MENU_BEFORE_SEP);
	
	DoAddCommandItem((int)CMD_CRV_FACE_BND_EDGE,"��������","�߽�����(&E)");
	DoAddCommandItem((int)CMD_CRV_FACE_BND_WIRE,"��������","�߽绷(&E)");
	DoAddCommandItem((int)CMD_CRV_FACE_RAW_WIRE,"��������","ԭʼ˳��߽绷(&E)");
	DoAddCommandItem((int)CMD_CRV_FACE_BND_EDGEDIR,"��������","����߽緽��(&D)");
	DoAddCommandItem((int)CMD_CRV_FACE_ISO_CURVE,"��������","��������(&I)");

	DoAddCommandItem((int)CMD_CRV_NORM_PRJ_CRV,"ͶӰ","ͶӰ����(&P)");
	DoAddCommandItem((int)CMD_CRV_BREAK_BY_POINT,"���","ʹ�õ�(&P)",KUI_MENU_BEFORE_SEP);
	DoAddCommandItem((int)CMD_CRV_BREAK_BY_CURVE,"���","ʹ������(&C)");
	DoAddCommandItem((int)CMD_CRV_TRIM,"�ü�","���߲ü�(&C)");
	DoAddCommandItem((int)CMD_CRV_TRIM_QUICK,"�ü�","���ٲü�(&C)");
	DoAddCommandItem((int)CMD_CRV_SPLIT_ALL_EDGES,"�ü�","ȫ���ָ�(&S)");

	DoAddCommandItem((int)CMD_CRV_FILLET,"����","��Բ��(&F)");
	DoAddCommandItem((int)CMD_CRV_FILLET_TWO_EDGE,"����","���ߵ�Բ��(&T)");
	DoAddCommandItem((int)CMD_CRV_OFFSET,"����ƫ��(&O)");
	DoAddCommandItem((int)CMD_CRV_BLEND,"���߻��(&B)");

	DoAddCommandItem((int)CMD_ID_SEWING_CURVE,"�������(&W)");
	
	DoAddCommandItem((int)CMD_CRV_TEXT,"�ı�(&T)",KUI_MENU_BEFORE_SEP);


	return true;
}

void QxCurvePluginImpl::Release()
{
}

// ���������ڲ���е�ID�������������
kiCommand* QxCurvePluginImpl::DoCreateCommand(int nLocalID)
{
	kiCommand *pCommand = NULL;
	switch(nLocalID){
	case CMD_CRV_POINT:		pCommand = new kcmPoint; break;
	case CMD_CRV_INTER_POINT:	pCommand = new kcmCurveInterPoint;	break;
	case CMD_CRV_LINE:		pCommand = new kcmLine;	break;
	case CMD_CRV_CIRCLE:		pCommand = new kcmCircle;	break;
	case CMD_CRV_ARC:			pCommand = new kcmArc; break;
	case CMD_CRV_SPLINE:		pCommand = new kcmSpline; break;
	case CMD_CRV_UP_SPLINE:	pCommand = new kcmSplineUnperiodize; break;
	case CMD_CRV_RECT:		pCommand = new kcmRect; break;
	case CMD_CRV_POLYLINE:		pCommand = new kcmCurvePolyline; break;
	case CMD_CRV_INTER_CRV:	pCommand = new kcmInterCurve; break;
	case CMD_CRV_FACE_BND_EDGE:	pCommand = new kcmFaceBoundEdge; break;
	case CMD_CRV_FACE_BND_WIRE:	pCommand = new kcmFaceBoundWire; break;
	case CMD_CRV_FACE_RAW_WIRE:	pCommand = new kcmFaceRawWire; break;
	case CMD_CRV_FACE_BND_EDGEDIR:	pCommand = new kcmFaceBoundEdgeDire; break;
	case CMD_CRV_FACE_ISO_CURVE:	pCommand = new kcmFaceIsoCurve; break;
	case CMD_CRV_NORM_PRJ_CRV:		pCommand = new kcmNormProjCurve; break;
	case CMD_CRV_BREAK_BY_POINT:	pCommand = new kcmCurveBreakByPoint; break;
	case CMD_CRV_BREAK_BY_CURVE:	pCommand = new kcmCurveBreakByCurve; break;
	case CMD_CRV_TRIM:	pCommand = new kcmCurveTrim; break;
	case CMD_CRV_TRIM_QUICK:	pCommand = new kcmCurveQuickTrim; break;
	case CMD_CRV_SPLIT_ALL_EDGES:	pCommand = new kcmSplitAllEdges; break;
	case CMD_CRV_FILLET:	pCommand = new kcmCurveFillet; break;
	case CMD_CRV_FILLET_TWO_EDGE:	pCommand = new kcmTwoEdgeFillet; break;
	case CMD_CRV_OFFSET:	pCommand = new kcmCurveOffset; break;
	case CMD_CRV_BLEND:	pCommand = new kcmCurveBlend; break;
	case CMD_ID_SEWING_CURVE: pCommand = new kcmSewCurve; break;
	case CMD_CRV_TEXT:	pCommand = new kcmWireText; break;
	default:
		break;
	}
	return pCommand;
}

// �ͷŴ������������
bool QxCurvePluginImpl::DoReleaseCommand(kiCommand *pCommand)
{
	if(pCommand){
		pCommand->Destroy();
		delete pCommand;
		return true;
	}
	return false;
}