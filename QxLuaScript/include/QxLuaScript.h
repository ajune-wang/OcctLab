//
#ifndef _QX_LUA_SCRIPT_H_
#define _QX_LUA_SCRIPT_H_

#include "QxLibDef.h"

class kcDocContext;

//��ʼ��
QXLIB_API bool LuaScriptInit(kcDocContext *pDocCtx);

// ��ʾ�ű��Ի���
QXLIB_API bool ShowLuaScriptDialog(bool bShow);

kcDocContext* GetDocContext();

#endif