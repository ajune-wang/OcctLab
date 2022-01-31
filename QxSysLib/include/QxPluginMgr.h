//////////////////////////////////////////////////////////////////////////
// ���������

#ifndef _QX_PLUGIN_MGR_H_
#define _QX_PLUGIN_MGR_H_

#include <string>
#include <vector>
#include "QxLibDef.h"
#include "QxCmdPlugin.h"

//�˵����ӦcmdID����ʼ��Χ.
#define KI_CMD_ID_BEGIN		33000
#define KI_CMD_ID_END			39000 //6000������,�㹻��.

class kiCommand;
class QxCommandItem;
class QxCmdPlugin;
typedef std::vector<QxCmdPlugin *>		kuiPluginArray;
typedef std::vector<QxCommandItem *>	kuiCmdItemArray;

//��¼һ����̬�������Ϣ
class QxDynPluginData{
public:
	QxDynPluginData(HINSTANCE hDll);
	~QxDynPluginData();

	void					FreePluginDll();

public:
	HINSTANCE 			pDllModule_;//��̬����
	QxCmdPluginMainProc	pMainProc_;
	QxCmdPluginFreeProc	pFreeProc_;
	QxCmdPluginImpl		*pImpl_;//����ʵ����
};

class QXLIB_API QxPluginMgr
{
public:
	QxPluginMgr(void);
	~QxPluginMgr(void);

	int						Initialize();
	void						Destroy();

	//����һ�����
	BOOL						LoadPlugin(QxCmdPlugin *pPlugin);
	//ж��һ�����
	BOOL						UnloadPlugin(QxCmdPlugin *pPlugin);

	QxCmdPlugin*				FindPlugin(const std::string& strName);

	int						GetItemCount() const;
	QxCommandItem*			GetCmdItem(int ix);

public:
	//������������id��ȡ����
	kiCommand*				GetCommand(int nCmdID);
	//�ͷŸ��������ǻ�������ᱻfree��
	//!!��ʱû����
	void						ReleaseCommand(kiCommand *pCommand);

public:
	//ͨ����̬�⣬����һ�����
	BOOL						LoadDynPlugin(const char *pszDllPath);

protected:
	std::vector<QxDynPluginData*>	aDynPluginArray_;

protected:
	kuiPluginArray			aPluginArray_;//����б�
	kuiCmdItemArray			_cmdItemArray;//�����������б�.������¼ָ��.ָ���ͷ��ɲ������.
	UINT						_uiCurrentID;//��ǰ���õ�ID.
};

#endif