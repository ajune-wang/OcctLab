//
#ifndef _OX_CMD_PLUGIN_IMPL_H_
#define _OX_CMD_PLUGIN_IMPL_H_

#include "QxLibDef.h"
#include "QxCommandItem.h"

// ������������������Ҫ�ӿں���
#define CMDPLUGIN_EXPORT extern "C" _declspec(dllexport)

class kiCommand;
class QxDynCmdPlugin;
class QxCmdPluginImpl;

// ˵����
//   ÿ�����������һϵ��������ɣ�ÿ�������ڲ���ڲ���һ��Ψһ��ID�����ڲ���ڲ�����Ĺ���
//   Ŀǰ������֧�ֽ����������ӵ��˵��С�Ϊ�˱������������ID��ͻ������������ڲ�Ϊÿ��
//   ���������һ��ID��Χ��ÿ�������һ������ID�����������IDΪ�������ID+����������ID
//   ���������Ա�����������ID��ͻ������Ҳ����ÿ������ڲ��Լ������Լ���ID��
//   

// ���ڲ�������úͲ���
typedef struct stQxCmdPluginCfg{
	int	stsize; //���ṹ�Ĵ�С
	int version; //�汾��
	int ctlflag; //���Ʊ��
}QxCmdPluginCfg;

// ������ʵ�ֵĽӿ��࣬ÿ�������Ҫʵ�ָýӿ���ķ���
// ͬʱ�ṩһЩ��������Щ�����������ڵ�����ʵ�ֶ�Ӧ�Ĺ��ܣ�ͬʱ���ⲿ�����ڲ���ʵ��
// 
class QXLIB_API QxCmdPluginImpl{
public:
	QxCmdPluginImpl();
	virtual ~QxCmdPluginImpl(){}

	// ��ȡ����е��������
	virtual int		GetCommandNum() const = 0;

	// ��ȡ���������,���������Ҫ��֤Ψһ�ԣ����ܺ��Ѿ����ڵĲ����ͻ.
	virtual const char*	GetPluginName() const = 0;

	// �����ʼ�����ͷ�
	virtual bool		Initialize() = 0;
	virtual void		Release() = 0;

	// ���������ڲ���е�ID�������������
	virtual kiCommand*			DoCreateCommand(int nLocalID) = 0;
	// �ͷŴ������������
	virtual bool					DoReleaseCommand(kiCommand *pCommand) = 0;

public:
	// �������ڲ������Ĺ���
	void				SetPlugin(QxDynCmdPlugin *pPlugin);

	// ��������������˵�������
	virtual bool		AddCommandItem(int nLocalID,const char *pszTopMenu,const char *pszSubMenu,
		                                     int nMenuFlag = 0,bool bCached = false);
	virtual bool		AddCommandItem(int nLocalID,const char *pszTopMenu,const char *pszSubMenu,
		                                     const char *pszSubMenu2,int nMenuFlag = 0,bool bCached = false);
	virtual bool		AddCommandItem(int nLocalID,eAppTopMenuID topMenuId,const char *pszSubMenu,
		                                     int nMenuFlag = 0,bool bCached = false);
	virtual bool		AddCommandItem(int nLocalID,eAppTopMenuID topMenuId,const char *pszSubMenu,
		                                     const char *pszSubMenu2,int nMenuFlag = 0,bool bCached = false);

protected:
	bool				DoAddCommandItem(int nLocalID,const char *pszSubMenu,int nMenuFlag = 0,bool bCached = false);
	bool				DoAddCommandItem(int nLocalID,const char *pszSubMenu,
		                            const char *pszSubMenu2,int nMenuFlag = 0,bool bCached = false);

protected:
	QxDynCmdPlugin		*pPlugin_;// �����¼��һ���ڲ�ʵ�ֵĲ���࣬���ڼ��شӸýӿ�����ʵ�ֵĲ����
	eAppTopMenuID			eTopMenuID_; //���ڶ����˵�ID
};

// NOTE:
//   �������Ҫ�ӿں���,ÿ�������ʹ��ͳһ�����ƣ�
//   QxCmdPluginMain��QxCmdPluginFree������ͨ����̬���ػ�ȡ��Щ�ӿں���

// ��Ҫ�ӿں���������������QxCmdPluginImpl����
typedef QxCmdPluginImpl* (*QxCmdPluginMainProc)(QxCmdPluginCfg *pCfg);
// �ͷŴ�����QxCmdPluginImpl����
typedef void (*QxCmdPluginFreeProc)(QxCmdPluginImpl *pImpl);

#endif