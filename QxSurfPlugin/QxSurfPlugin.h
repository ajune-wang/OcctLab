//2021.12.20
#ifndef _QX_SURF_PLUGIN_H_
#define _QX_SURF_PLUGIN_H_

#include "QxCmdPluginImpl.h"

// 主要接口函数，名称需要是一致的
//
CMDPLUGIN_EXPORT QxCmdPluginImpl* QxCmdPluginMain(QxCmdPluginCfg *pCfg);

//
CMDPLUGIN_EXPORT void QxCmdPluginFree(QxCmdPluginImpl *pImpl);

#endif