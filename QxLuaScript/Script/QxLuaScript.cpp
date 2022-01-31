//
#include "stdafx.h"
#include "sciModelLib.h"
#include "kcDocContext.h"
#include "sciTopolLib.h"
#include "QxLuaScript.h"

//��¼�ⲿ�������Ҫ����
static kcDocContext *s_pDocContext = NULL;

bool LuaScriptInit(kcDocContext *pDocCtx)
{
	s_pDocContext = pDocCtx;

	InitLuaExtModuleLib();
	InitLuaExtTopolLib();

	return true;
}

kcDocContext* GetDocContext()
{
	return s_pDocContext;
}