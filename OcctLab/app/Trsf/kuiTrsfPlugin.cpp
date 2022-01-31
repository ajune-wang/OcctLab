#include "StdAfx.h"
#include "QxCommandItem.h"
#include "kcmTrsfMove.h"
#include "kcmTrsfRotate.h"
#include "kcmTrsfScale.h"
#include "kuiTrsfPlugin.h"

enum eTrsfPluginCmdType{
	CMD_TRSF_MOVE = 0,
	CMD_TRSF_ROTATE2D,
	CMD_TRSF_SCALE
};

kuiTrsfPlugin::kuiTrsfPlugin(void)
{
	_strName = "�任���";
	_nReserveNum = 50;
}

kuiTrsfPlugin::~kuiTrsfPlugin(void)
{
}

//��ʼ��
BOOL  kuiTrsfPlugin::Initialize()
{
	const char *psztopMenu = "�任(&T)";

	AddCommandItem((int)CMD_TRSF_MOVE,psztopMenu,"ƽ��(&P)");
	AddCommandItem((int)CMD_TRSF_ROTATE2D,psztopMenu,"��ת(&R)");
	AddCommandItem((int)CMD_TRSF_SCALE,psztopMenu,"����(&S)");

	return TRUE;
}

kiCommand*  kuiTrsfPlugin::DoCreateCommand(int nLocalID)
{
	kiCommand *pCommand = NULL;
	switch(nLocalID){
	case CMD_TRSF_MOVE:		pCommand = new kcmTrsfMove;  break;
	case CMD_TRSF_ROTATE2D:	pCommand = new kcmTrsfRotate2d; break;
	case CMD_TRSF_SCALE:		pCommand = new kcmTrsfScale; break;
	}
	return pCommand;
}

bool  kuiTrsfPlugin::DoReleaseCommand(kiCommand *pCommand)
{
	if(pCommand){
		pCommand->Destroy();
		delete pCommand;
		return true;
	}
	return false;
}