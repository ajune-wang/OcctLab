//////////////////////////////////////////////////////////////////////////
// ���������,�����������,����ַ���.

#ifndef _KI_COMMAND_MGR_H_
#define _KI_COMMAND_MGR_H_

#include <map>
#include <vector>
#include "QxLibDef.h"


class kuiMouseInput;
class kuiKeyInput;
class kiCommand;
typedef std::vector<kiCommand *>	kiCommandArray;
typedef std::map<UINT,kiCommand *>  kiCommandIDMap;
typedef std::map<std::string,kiCommand *>	kiCommandNameMap;

class kcModel;
class QxPluginMgr;
class kiSelSet;
class kiInputEdit;
class kcDocContext;

class QXLIB_API kiCommandMgr
{
public:
	kiCommandMgr(void);
	~kiCommandMgr(void);

	//��ʼ��,ʹ��ģ�Ͷ���Ͳ������������
	BOOL						Initialize(kcDocContext *pDocCtx,QxPluginMgr *pPluginMgr);
	BOOL						Destroy();

	//ע���ȡ������
	// ע��һ���������,ͨ���ǲ���е�����,���������ɲ������.
	// ÿ������Ҫ��ID name alias��Ҫ��Ψһ��.
	BOOL						RegisterCommand(kiCommand *pCommand);
	// ע��һ��ϵͳ����,��ϵͳ������,�и�mgr��������,����unreg.
	BOOL						RegisterSysCommand(kiCommand *pCommand);
	// ע��һ���������.
	BOOL						UnRegisterCommand(kiCommand *pCommand);
	kiCommand*				FindRegisterCommand(UINT nID);
	kiCommand*				FindRegisterCommand(const std::string& strName);
	kiCommand*				FindRegisterCommandByAlias(const std::string& strAlias);

protected:
	bool						_bInitialized;//�Ƿ��ʼ����
	kcDocContext				*pDocContext_; //Doc����Ҫ���ݼ�¼�ڸö���
	kcModel					*_pModel;//������ģ��
	QxPluginMgr				*_pPluginMgr;//�������
	Handle(V3d_View)			aV3dView_;//������V3d_View������
	kiSelSet					*pGlobalSelSet_;//�����������Ӧȫ��ѡ��
	
	kiCommand				*_pExecCommand;//��ǰִ������
	
	kiCommandArray			m_cmdArray;//��¼�����������.�±�+KI_CMD_ID_BEGIN��Ϊ�˵����Ӧ��ID.
	kiCommandIDMap			m_cmdIDMap;//ID��command�Ķ�Ӧ.
	kiCommandNameMap			m_cmdNameMap;//���ƺ������Ӧ
	kiCommandNameMap			m_cmdAliasMap;//�����������Ӧ
	kiCommandArray			m_sysCmdArray;//ϵͳ����,��Щ������Ҫ�ڲ��ͷ�.

public:
	//ִ������
	int						ExecCommand(UINT nID);
	int						EndCommand(int nCode);

	BOOL						HasCommandRunning();

public:
	kcDocContext*				GetDocContext() const;
	// ��ȡȫ��ѡ�񼯶���ָ��
	kiSelSet*				GetGlobalSelSet() const;

	//
	kiInputEdit*				GetInputEdit() const;

	//���ù�����v3d_View
	void						SetV3dView(const Handle(V3d_View) &aView);
	const Handle(V3d_View)	GetCurrV3dView() const;
	
public:
	//�����Ϣ
	void						OnLButtonDown(kuiMouseInput& mouseInput);
	int						OnLButtonUp(kuiMouseInput& mouseInput);
	int						OnLButtonDblClk(kuiMouseInput& mouseInput);
	int						OnMouseMove(kuiMouseInput& mouseInput);
	void						OnRButtonDown(kuiMouseInput& mouseInput);
	int						OnRButtonUp(kuiMouseInput& mouseInput);

public:
	//������Ϣ
	int						OnKeyDown(kuiKeyInput& keyInput);
	int						OnKeyUp(kuiKeyInput& keyInput);
	int						OnChar(kuiKeyInput& keyInput);

public:
	// ��commandbar��ȡ������.
	int						ReceiveInputString(const char* pszString);
};

#endif