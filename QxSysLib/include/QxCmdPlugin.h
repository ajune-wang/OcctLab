//////////////////////////////////////////////////////////////////////////
// ��������ʵ����.
#ifndef _QX_CMD_PLUGIN_H_
#define _QX_CMD_PLUGIN_H_

#include <string>
#include <vector>
#include <map>
#include "QxLibDef.h"
#include "QxCmdPluginImpl.h"

class QxCommandItem;
typedef std::vector<QxCommandItem *>		kuiCmdItemArray;
typedef std::map<int,QxCommandItem *>		kuiCmdItemMap;

// ����࣬����һ�������ڲ��Ĳ�������ⲿ�Ķ�̬�ɼ��ز��
// �ڲ�����࣬��QxCmdPlugin����������ʵ�ִ��麯����
// �ⲿ��̬���ز��������QxDynCmdPlugin��Ͳ���е�OxCmdPluginImpl��������ʵ�֡�
//
// ÿ�������һ��Ψһ�Ĳ�����ƣ������໥������
//

class QXLIB_API QxCmdPlugin
{
public:
	QxCmdPlugin(void);
	virtual ~QxCmdPlugin(void);

	//��ʼ�����ͷ�
	virtual BOOL					Initialize() = 0;
	virtual void					Release();

	//���ݻ�׼ID����ÿ�������Ӧ������ID
	virtual int					GenCommandID(int nBaseCmdID);
		
	virtual int					GetReserveNum() const;//����Ԥ��������ĸ���
	virtual int					GetItemCount() const;
	virtual	QxCommandItem*		GetCmdItem(int ix);

	const std::string&			GetName() const;

public:
	//������������id��ȡ����
	kiCommand*					GetCommand(int nCmdID);
	//�ͷŸ��������ǻ�������ᱻfree��
	bool							ReleaseCommand(kiCommand *pCommand);

protected:
	virtual kiCommand*			DoCreateCommand(int nLocalID) = 0;
	virtual bool					DoReleaseCommand(kiCommand *pCommand) = 0;

public:
	//
	virtual BOOL					AddCommandItem(QxCommandItem *pItem);
	virtual BOOL					AddCommandItem(int nLocalID,const char *pszTopMenu,const char *pszSubMenu,
		                                     int nMenuFlag = 0,bool bCached = false);
	virtual BOOL					AddCommandItem(int nLocalID,const char *pszTopMenu,const char *pszSubMenu,
		                                     const char *pszSubMenu2,int nMenuFlag = 0,bool bCached = false);
	virtual BOOL					AddCommandItem(int nLocalID,eAppTopMenuID topMenuId,const char *pszSubMenu,
		                                     int nMenuFlag = 0,bool bCached = false);
	virtual BOOL					AddCommandItem(int nLocalID,eAppTopMenuID topMenuId,const char *pszSubMenu,
		                                     const char *pszSubMenu2,int nMenuFlag = 0,bool bCached = false);
	QxCommandItem*				FindCommandItem(int nLocalID);

protected:
	bool							DoCheckLocalID(int nLocalID);

protected:
	std::string					_strName;
	kuiCmdItemArray				_cmdItemArray;//�������б�
	kuiCmdItemMap					_cmdItemMap;//�������
	int							_nBaseCmdID;//��׼����ID����ʼ������ID
	int							_nReserveNum;//Ԥ���ĸ���
};

//
class QXLIB_API QxDynCmdPlugin : public QxCmdPlugin{
public:
	QxDynCmdPlugin(QxCmdPluginImpl *pImpl);
	~QxDynCmdPlugin();

	virtual BOOL					Initialize();
	virtual void					Release();

	virtual int					GetReserveNum() const;//����Ԥ��������ĸ���

protected:
	virtual kiCommand*			DoCreateCommand(int nLocalID);
	virtual bool					DoReleaseCommand(kiCommand *pCommand);

protected:
	QxCmdPluginImpl				*pPluginImpl_;//�����ʵ����
};

#endif