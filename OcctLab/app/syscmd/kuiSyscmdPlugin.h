//////////////////////////////////////////////////////////////////////////
//
#ifndef _KUI_SYSCMD_PLUGIN_H_
#define _KUI_SYSCMD_PLUGIN_H_

#include "QxCmdPlugin.h"

class kuiSyscmdPlugin : public QxCmdPlugin
{
public:
	kuiSyscmdPlugin(void);
	virtual ~kuiSyscmdPlugin(void);

	//��ʼ�����ͷ�
	virtual BOOL					Initialize();

protected:
	virtual kiCommand*			DoCreateCommand(int nLocalID);
	virtual bool					DoReleaseCommand(kiCommand *pCommand);
};

#endif
