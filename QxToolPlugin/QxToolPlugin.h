//
#ifndef _QX_TOOL_PLUGIN_H_
#define _QX_TOOL_PLUGIN_H_

#include "QxCmdPluginImpl.h"

// ��Ҫ�ӿں�����������Ҫ��һ�µ�
//
CMDPLUGIN_EXPORT QxCmdPluginImpl* QxCmdPluginMain(QxCmdPluginCfg *pCfg);

//
CMDPLUGIN_EXPORT void QxCmdPluginFree(QxCmdPluginImpl *pImpl);


#endif